// Goksu Guvendiren on 2.2.2019

#pragma once
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <experimental/optional>
#include <window.hpp>
#include <program.hpp>

namespace cc
{
	namespace utils
	{
        inline std::vector<cv::Mat> GetImagesFromFolder(const std::string& p)
        {
            namespace fs = boost::filesystem;
            fs::path path(p);
            if (!fs::is_directory(path))
            {
                std::cerr << "A directory should be given\n";
                return {};
            }
            
            std::vector<std::string> image_names;
            for (auto& image_path : fs::directory_iterator(p))
            {
//                std::cout << image_path.path().string() << '\n';
                image_names.push_back(image_path.path().string());
            }
            
            std::vector<cv::Mat> images;
            for (auto& name : image_names)
            {
                cv::Mat image = cv::imread(name);
                images.emplace_back(std::move(image));
            }
            
            return images;
        }

        inline std::vector<cv::Mat> GetImagesFromCamera(const std::string &folder_path)
        {
            cv::VideoCapture video(0);
            if (!video.isOpened())
            {
                std::cerr << "Could not open the camera.\n";
                return {};
            }

            std::vector<cv::Mat> captured_frames;

            cv::Mat frame;
            int key = 0;
            int i = 0;
            while(key != 27)
            {
                video >> frame;

                if (key == 99) // pressed c
                {
                    std::cout << "Captured " << std::to_string(i++) << "th frame\n";
                    captured_frames.push_back(frame);
                    cv::imwrite(folder_path + std::to_string(i) + "_th.jpg", frame);
                }

                cv::imshow("frames", frame);
                key = cv::waitKey(30);
            }

            return captured_frames;
        }

        inline cv::Mat DrawChessboardPoints(const cv::Mat& frame, const cv::Size& size, const std::vector<cv::Point2f>& corners)
        {
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 50, 0.1));
            drawChessboardCorners(frame, size, cv::Mat(corners), true);

            return frame;
        }

        inline std::experimental::optional<std::vector<cv::Point2f>> FindChessboardPoints(const cv::Mat& frame, const cv::Size& size)
        {
            std::vector<cv::Point2f> corners;
            if (cv::findChessboardCorners(frame, size, corners, cv::CALIB_CB_FAST_CHECK))
            {
                return corners;
            }

            return std::experimental::nullopt;
        }

//        inline std::string read_file(const std::string& filename)
//        {
//            std::ifstream file;
//            file.open(filename);
//
//            std::stringstream stream;
//            stream << file.rdbuf();
//
//            std::string result = stream.str();
//
//            return result;
//        }

        class key_handlers
        {
            std::map<int, std::function<void()>> functions;

        public:

            void Add(int key, std::function<void()> function)
            {
                auto r = functions.emplace(key, std::move(function));
            }

            void operator()(const grt::window& w) const
            {
                for (auto& function : functions)
                {
                    if (glfwGetKey(w.Get(), function.first) == GLFW_PRESS)
                    {
                        function.second();
                    }
                }
            }
        };
    };
}
