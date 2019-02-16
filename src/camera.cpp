//
//  camera.cpp
//  ass2_xcode
//
//  Created by Göksu Güvendiren on 4.02.2019.
//  Copyright © 2019 Göksu Güvendiren. All rights reserved.
//

#include "camera.hpp"
#include <utils.hpp>

#include <opencv2/opencv.hpp>
#include <camera.hpp>
#include <calibrator.hpp>


cc::camera::camera(const cv::Mat& intr, const cv::Mat& distortion_coefficients, const std::vector<cv::Mat>& rotation, const std::vector<cv::Mat>& translation,
                   const cv::Size& size) : cam(0)
{
    intr.copyTo(intrinsic);
    distortion_coefficients.copyTo(distortion_coeffs);

    cv::calibrationMatrixValues(intrinsic, size, 0, 0, fov_horizontal, fov_vertical, focal_length, principal_point, aspect_ratio);
}

cc::camera::camera(const cc::calibrator &calibrator) : camera(calibrator.Intrinsic(), calibrator.DistortionCoeffs(), calibrator.RotationVectors(),
        calibrator.TranslationVectors(), calibrator.Size())
{}

cc::camera::camera(const std::string &settings)
{
    std::ifstream param_file(settings);
    nlohmann::json camera_params;
    param_file >> camera_params;

    Serialize(camera_params);
}

void cc::camera::Serialize(const nlohmann::json& camera_params)
{
    fov_horizontal = camera_params["horizontal_fov"];
    fov_vertical   = camera_params["vertical_fov"];
    focal_length = camera_params["focal_length"];
    principal_point.x = camera_params["principal_point"]["x"];
    principal_point.y = camera_params["principal_point"]["y"];

    distortion_coeffs = cv::Mat::zeros(1, 4, CV_64F);

    distortion_coeffs.at<double>(0) = camera_params["distortion_parameters"]["k_1"];
    distortion_coeffs.at<double>(1) = camera_params["distortion_parameters"]["k_2"];
    distortion_coeffs.at<double>(2) = camera_params["distortion_parameters"]["p_1"];
    distortion_coeffs.at<double>(3) = camera_params["distortion_parameters"]["p_2"];

    std::vector<std::vector<double>> mat = camera_params["camera_matrix"];
    intrinsic = cv::Mat(3, 3, CV_64FC1);
    for (int i = 0; i < mat.size(); ++i)
    {
        for (int j = 0; j < mat[0].size(); ++j)
        {
            intrinsic.at<double>(i, j) = mat[i][j];
            std::cout << mat[i][j] << ", ";
        }
        std::cout << '\n';
    }
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
    file << camera_params.dump(4) << '\n';
}

std::pair<cv::Mat, cv::Mat> cc::camera::GetTRMatrices(const std::vector<cv::Point2f>& points, const cv::Size& board_size)
{
    cv::Mat rvec;
    cv::Mat tvec;

    auto object_points = cc::utils::GetObjectPoints(board_size);

    if (cv::solvePnPRansac(object_points, points, intrinsic, distortion_coeffs, rvec, tvec))
    {
        return {tvec, rvec};
    }
    else
    {
        return {};
    }
}