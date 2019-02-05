#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <calibrator.hpp>
#include <utils.hpp>
#include <camera.hpp>

int main(int argc, const char** argv)
{
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
        frame = camera.GetFrame();
        float time;
        auto th = cc::utils::DrawChessboardPoints(frame, board_size);

        frame = th.first;
        time = th.second;

        std::cout << 1000 / time << " fps " << '\n';

//        frame = camera.GetFrame();
//        auto beg = std::chrono::system_clock::now();
//
//        auto points = cc::utils::FindChessboardPoints(frame, board_size);
//        if (points)
//        {
//            frame = cc::utils::DrawChessboardPoints(frame, board_size, *points);
////            auto vecs = cc::camera::GetTVMatrices(points, board_size);
//        }
//        auto end = std::chrono::system_clock::now();
//        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count();
//
//        time += 0.05;
//        std::cout << 1000 / time << " fps " << '\n';
    }

	return 0;
}
