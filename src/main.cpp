#include <iostream>
#include <boost/filesystem.hpp>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <calibrator.hpp>
#include <utils.hpp>
#include <camera.hpp>
#include <vertex_shader.hpp>

#include <app.hpp>

cc::camera CheckNecessities()
{
    auto current_path = boost::filesystem::current_path().parent_path();
    auto camera_path  = current_path / "camera" / "camera_params.json";

    std::cout << camera_path.string() << '\n';
    if (boost::filesystem::exists(camera_path))
    {
        // the camera is already calibrated, use it.
        cc::camera camera(camera_path.string());
        return camera;
    }
    else
    {
        // calibrate camera from scratch.

        std::vector<cv::Mat> captured_frames;

        if (boost::filesystem::exists(current_path / "dataset" / "generated"))
        {
            // chessboard images are already taken, no need to retake.
            std::string path = (current_path / "dataset" / "generated").string();
            captured_frames = cc::utils::GetImagesFromFolder(path);
        }
        else
        {
            captured_frames = cc::utils::GetImagesFromCamera("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/dataset/generated/");
        }

        cv::Size board_size = {5, 7};
        cc::calibrator calibrator(captured_frames, board_size);

        auto undistorted_image = calibrator.Calibrate(captured_frames[0]);
        auto error = calibrator.CalculateError();

        cc::camera camera(calibrator);
        camera.SaveSettings("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/camera/", "camera_params.json");
        return camera;
    }
}

static std::string read_file(const std::string& filename)
{
    std::ifstream file;
    file.open(filename);

    if(!file.good())
    {
        std::cout << "File no good!" << '\n';
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();

    std::string result = stream.str();

    return result;
}

grt::program initializeShaders(const grt::vertex_shader& vertex_shader, const grt::fragment_shader& fragment_shader)
{
    auto program = grt::program();
    program.Attach(vertex_shader);
    program.Attach(fragment_shader);
    program.Link();

    return program;
}

cc::camera InitializeCamera(int argc, const char** argv)
{
//    auto camera = CheckNecessities();
//    if(!camera.Open())
//    {
//        std::cout << "Camera is not open.\n";
//        return 0;
//    }
//
    std::vector<cv::Mat> captured_frames;

    if (argc == 1)
    {
        captured_frames = cc::utils::GetImagesFromCamera("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/dataset/generated/");
    }
    else
    {
        std::string path = argv[1];
        captured_frames = cc::utils::GetImagesFromFolder(path);
    }

    cv::Size board_size = {5, 7};
    cc::calibrator calibrator(captured_frames, board_size);

    auto undistorted_image = calibrator.Calibrate(captured_frames[0]);
    auto error = calibrator.CalculateError();

    cc::camera camera(calibrator);
    camera.SaveSettings("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/camera/", "cameraparams.json");
    assert(camera.Open());

    return camera;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main(int argc, const char** argv)
{
    cv::Size board_size = {5, 7};
    auto camera = InitializeCamera(argc, argv);
    assert(camera.Open());
    cv::Mat frame = camera.GetFrame();
    std::cout << frame.cols << ", " << frame.rows << '\n';

    grt::app app(0, 0);
    auto& window = app.CreateWindow(1280, 720, "CC");
    app.SetCurrentContext(window);
    app.Initialize();

    app.AddHandler(GLFW_KEY_ESCAPE, [&]
    {
        app.SetShouldClose(true);
    });

    grt::vertex_shader   vertex_shader(read_file("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/shaders/basic_vert.glsl"));
    grt::fragment_shader fragment_shader(read_file("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/shaders/basic_frag.glsl"));

    auto shader_program = initializeShaders(vertex_shader, fragment_shader);
    shader_program.SetVariable("tex", 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    while(!glfwWindowShouldClose(window.Get()))
    {
        glViewport(0, 0, 1280, 720);

        glUseProgram(shader_program.GetProgram());
        frame = camera.GetFrame();
        cv::flip(frame, frame, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        renderQuad();

        auto beg = std::chrono::system_clock::now();
        frame = camera.GetFrame();

        auto points = cc::utils::FindChessboardPoints(frame, board_size);
        if (points)
        {
            frame = cc::utils::DrawChessboardPoints(frame, board_size, *points);
//            auto vecs = cc::camera::GetTVMatrices(points, board_size);
        }
        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count();

        app.Render();
//        std::cout << 1000 / time << " fps " << '\n';
    }

	return 0;
}