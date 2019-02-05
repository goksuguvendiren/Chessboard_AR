//
//  camera.cpp
//  ass2_xcode
//
//  Created by Göksu Güvendiren on 4.02.2019.
//  Copyright © 2019 Göksu Güvendiren. All rights reserved.
//

#include "camera.hpp"
#include <opencv2/opencv.hpp>
#include <camera.hpp>
#include <calibrator.hpp>


cc::camera::camera(const cv::Mat& intr, const cv::Mat& distortion_coefficients, const std::vector<cv::Mat>& rotation, const std::vector<cv::Mat>& translation,
                   const cv::Size& size) : cam(0)
{
    intr.copyTo(intrinsic);
    distortion_coefficients.copyTo(distortion_coeffs);

    cv::calibrationMatrixValues(intrinsic, size, 0, 0, fov_horizontal, fov_vertical, focal_length, principal_point, aspect_ratio);

//    std::get<0>(distortion_coeffs) = distortion_coefficients.at<double>(0);
//    std::get<1>(distortion_coeffs) = distortion_coefficients.at<double>(1);
//    std::get<2>(distortion_coeffs) = distortion_coefficients.at<double>(2);
//    std::get<3>(distortion_coeffs) = distortion_coefficients.at<double>(3);
}

cc::camera::camera(const cc::calibrator &calibrator) : camera(calibrator.Intrinsic(), calibrator.DistortionCoeffs(), calibrator.RotationVectors(), calibrator.TranslationVectors(), calibrator.Size())
{}

void cc::camera::Serialize(const nlohmann::json& camera_params)
{

}

nlohmann::json cc::camera::Deserialize() const
{
    nlohmann::json camera_params;

    camera_params["horizontal_fov"] = fov_horizontal;
    camera_params["vertical_fov"] = fov_vertical;
    camera_params["focal_length"] = focal_length;
    camera_params["principal_point"]["x"] = principal_point.x;
    camera_params["principal_point"]["y"] = principal_point.y;
    camera_params["distortion_parameters"]["k_1"] = distortion_coeffs.at<double>(0);//std::get<0>(distortion_coeffs);
    camera_params["distortion_parameters"]["k_2"] = distortion_coeffs.at<double>(1);//std::get<1>(distortion_coeffs);
    camera_params["distortion_parameters"]["p_1"] = distortion_coeffs.at<double>(2);//std::get<2>(distortion_coeffs);
    camera_params["distortion_parameters"]["p_2"] = distortion_coeffs.at<double>(3);//std::get<3>(distortion_coeffs);

    std::vector<std::vector<double>> mat;
    for (int i = 0; i < 3; i++)
    {
        std::vector<double> tmp;
        for (int j = 0; j < 3; j++)
        {
            tmp.push_back(intrinsic.at<double>(i, j));
        }
        mat.push_back(tmp);
    }

    for (auto elem : mat)
    {
        for (auto e : elem) std::cout << e << ", ";
        std::cout << '\n';
    }

    std::cout << intrinsic << '\n';

    camera_params["camera_matrix"] = mat;

    return camera_params;
}

void cc::camera::SaveSettings(const std::string &path, const std::string &filename) const
{
    auto p = path;
    if (path.back() != '/') p = path + "/";
    std::ofstream file(p + filename);

    nlohmann::json camera_params = Deserialize();

//    std::cout << camera_params.dump(4) << '\n';

    file << camera_params.dump(4) << '\n';
}

std::pair<std::vector<cv::Mat>, std::vector<cv::Mat>> cc::camera::GetTVMatrices(const std::vector<cv::Point2f>& points, const cv::Size& board_size)
{
    std::vector<cv::Mat> rvec;
    std::vector<cv::Mat> tvec;

    std::vector<cv::Point3f> object_points;
    for (int i = 0; i < board_size.height; ++i)
    {
        for (int j = 0; j < board_size.width; ++j)
        {
            object_points.emplace_back(cv::Point3f(j, i, 0.f));
        }
    }

    if (cv::solvePnPRansac(object_points, points, intrinsic, distortion_coeffs, rvec, tvec))
    {
        return {rvec, tvec};
    }
    else
    {
        return {};
    }
}
