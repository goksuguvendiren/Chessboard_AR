#include <iostream>
#include <boost/filesystem.hpp>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <calibrator.hpp>
#include <utils.hpp>
#include <camera.hpp>

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

int main(int argc, const char** argv)
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

    cv::Mat frame = camera.GetFrame();

    while(camera.Render(frame))
    {
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

        std::cout << 1000 / time << " fps " << '\n';
    }

	return 0;
}
//
//if (argc == 2)
//{
//// calibrate the camera with the saved images
//setting_path = std::string(argv[1]);
////        images_path  = std::string(argv[1]);
//
//captured_frames = cc::utils::GetImagesFromFolder(images_path);
//
//}
//else if (argc == 2)
//{
//setting_path = std::string(argv[2]);
//
//std::string path = argv[1];
//captured_frames = cc::utils::GetImagesFromCamera("/Users/goksu/Documents/UCSB/2018-2019/Winter/CS291A-MR-AR/ass2/dataset/generated/");
//}
//else
//{
////        cc::utils::PrintHelp();
//return 0;
//}