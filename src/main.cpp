#include <iostream>
#include <boost/filesystem.hpp>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <calibrator.hpp>
#include <utils.hpp>
#include <camera.hpp>
#include <vertex_shader.hpp>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <app.hpp>
#include <model.hpp>
#include <transform.hpp>
#include <chrono>

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
        captured_frames =
                cc::utils::GetImagesFromFolder("/home/goksu/calib");
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

    std::cout << error << '\n';

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
    auto& window = app.CreateWindow(960, 540, "CC");
    app.SetCurrentContext(window);
    app.Initialize();

    grt::point_light light({100.0f, 100.0f, 100.0f});
    grt::model teapot("../models/teapot/teapot.obj");

    grt::transform teapot_transform;
    grt::transform quad_transform;
    grt::transform light_transform;

    light_transform.Translate(glm::vec3{5.0, 5.0, -2.0});
    quad_transform.Scale({100, 100, 10});
    quad_transform.Translate({0, 0, 10});

    teapot.SetColor({160/255.f, 82/255.f, 45/255.f});

    app.AddHandler(GLFW_KEY_ESCAPE, [&]
    {
        app.SetShouldClose(true);
    });

    grt::vertex_shader   vertex_shader(read_file("../shaders/basic_vert.glsl"));
    grt::fragment_shader texture_frag(read_file("../shaders/texture_frag.glsl"));
    grt::fragment_shader teapot_frag(read_file("../shaders/basic_frag.glsl"));

    auto texture_program = initializeShaders(vertex_shader, texture_frag);
    auto shader_program = initializeShaders(vertex_shader, teapot_frag);
    texture_program.SetVariable("tex", 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const auto proj = glm::perspective<float>(glm::radians(camera.Fov()), 960/540.f, 1, 100);
    auto view = glm::lookAt<float>(glm::vec3(0,0, -10), glm::vec3(0,0,0), glm::vec3(0, 1, 0));

    bool done = false;
    cv::Mat t;
    cv::Mat r;
    auto object_points = cc::utils::GetObjectPoints(board_size);

    glm::mat4 view_proj = proj * view;
    bool have = false;
    glm::vec3 camera_pos;

    while(!glfwWindowShouldClose(window.Get()))
    {
        auto beg = std::chrono::system_clock::now();
        frame = camera.GetFrame();

        cv::Mat equalized;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
        cv::equalizeHist(gray, equalized);

        auto points = cc::utils::FindChessboardPoints(equalized, board_size);


        if (points)
        {
            if (!cv::solvePnP(object_points, *points, camera.Intrinsic(), camera.DistortCoeffs(), r, t))
            {
                continue;
            }
            std::cout << r << "\n" << t << "\n\n";

            cv::Mat rot;
            cv::Rodrigues(r, rot);
            cv::Mat viewMatrix(4, 4, CV_64F);

            for(int row=0; row<3; ++row)
            {
                for(int col=0; col<3; ++col)
                {
                    viewMatrix.at<double>(row, col) = rot.at<double>(row, col);
                }
                viewMatrix.at<double>(row, 3) = t.at<double>(row, 0);
            }
            viewMatrix.at<double>(3, 3) = 1.0f;

            cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_64F);
            cvToGl.at<double>(0, 0) = 1.0f;
            cvToGl.at<double>(1, 1) = -1.0f;
            cvToGl.at<double>(2, 2) = -1.0f;
            cvToGl.at<double>(3, 3) = 1.0f;
            viewMatrix = cvToGl * viewMatrix;

            cv::Mat glViewMatrix = cv::Mat::zeros(4, 4, CV_64F);
            cv::transpose(viewMatrix, glViewMatrix);

            glm::mat4 m = cc::utils::mat2mat4(glViewMatrix);
            camera_pos = m * glm::vec4(0, 0, 0, 1);

            auto world_points = cc::utils::GetObjectPoints(board_size);

            std::vector<cv::Point2f> projected_points;
            cv::projectPoints(world_points, r, t, camera.Intrinsic(), camera.DistortCoeffs(), projected_points);
            frame = cc::utils::DrawChessboardPoints(frame, board_size, projected_points);
            view_proj = proj * m;
            have = true;
        }

        do_render:

        glViewport(0, 0, 960, 540);

        cv::flip(frame, frame, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

        glfwMakeContextCurrent(window.Get());
        glfwPollEvents();

        window.CallCallbacks();

        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

//        texture_program.SetVariable("view_proj", view_proj);
//        texture_program.SetVariable("tex", 0);
//        texture_program.SetVariable("transform", quad_transform.Data());
//        renderQuad();

        shader_program.SetVariable("view_proj", view_proj);
        shader_program.SetVariable("transform", teapot_transform.Data());
        shader_program.SetVariable("obj_color", teapot.GetColor());
        shader_program.SetVariable("camera_position", camera_pos);
        shader_program.SetVariable("light_intensity", light.GetIntensity());
        shader_program.SetVariable("light_position", light_transform.GetPosition());

        teapot.Draw();

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count();

        app.Render();
        std::cout << 1000 / time << " fps " << '\n';
    }

	return 0;
}