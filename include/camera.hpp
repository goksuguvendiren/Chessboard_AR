//
//  camera.hpp
//  ass2_xcode
//
//  Created by Göksu Güvendiren on 4.02.2019.
//  Copyright © 2019 Göksu Güvendiren. All rights reserved.
//

#pragma once
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <calibrator.hpp>

namespace cc
{
    class camera
    {
        double fov_horizontal;
        double fov_vertical;
        double focal_length;
        double aspect_ratio;

        cv::Mat intrinsic;

        cv::Point2d principal_point;
//        std::tuple<double, double, double, double> distortion_coeffs;
        cv::Mat distortion_coeffs;

        cv::VideoCapture cam;
        bool should_close = false;

    public:
        camera(const cv::Mat& intrinsic, const cv::Mat& distortion_coeffs, const std::vector<cv::Mat>& rotation, const std::vector<cv::Mat>& translation,
               const cv::Size& size);
        camera(const cc::calibrator& calibrator);
        camera(const std::string& settings);

        void Serialize(const nlohmann::json& camera_params);
        nlohmann::json Deserialize() const;

        void SaveSettings(const std::string &path, const std::string &filename) const;

        bool Open() const { return cam.isOpened(); }
        bool Close() const { return should_close; }
        bool Render(const cv::Mat& frame) const
        {
            cv::imshow("chessboard", frame);
            return cv::waitKey(30) != 27;
        }

        cv::Mat GetFrame()
        {
            cv::Mat frame;
            cam >> frame;

            cv::waitKey(1);
            return frame;
        }

        std::pair<std::vector<cv::Mat>, std::vector<cv::Mat>> GetTVMatrices(const std::vector<cv::Point2f>& points, const cv::Size& board_size);
    };
}
