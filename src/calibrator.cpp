//
//  calibrator.cpp
//  ass2_xcode
//
//  Created by Göksu Güvendiren on 2.02.2019.
//  Copyright © 2019 Göksu Güvendiren. All rights reserved.
//

#include "calibrator.hpp"

#include <cmath>
#include <opencv2/opencv.hpp>

#include "nlohmann/json.hpp"

cc::calibrator::calibrator(const std::vector<cv::Mat>& images, const cv::Size& board_size) : image_count(images.size()), point_count(board_size.width * board_size.height)
{
    for (auto& image : images)
    {
        std::vector<cv::Point2f> cs;
        std::vector<cv::Point3f> coords;
        std::tie(cs, coords) = GetPoints(image, board_size);

        if (cs.size() == 0) continue;

        corners.push_back(std::move(cs));
        coordinates.push_back(std::move(coords));
    }

    cv::calibrateCamera(coordinates, corners, images[0].size(), intrinsic, distortion_coeffs, rotation_vectors, translation_vectors);

    std::cout << intrinsic << '\n';
    size = images[0].size();
}

std::pair<std::vector<cv::Point2f>, std::vector<cv::Point3f>> cc::calibrator::GetPoints(const cv::Mat& image, const cv::Size& board_size)
{
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point3f> coordinates;

//    cv::imshow("input image", image);
//    cv::waitKey(0);

    auto patternFound = cv::findChessboardCorners(image, board_size, corners);
    if (!patternFound)
    {
        std::cerr << "could not find the points\n";
        return {{}, {}};
    }

    for (int i = 0; i < board_size.height; ++i)
    {
        for (int j = 0; j < board_size.width; ++j)
        {
            coordinates.push_back(cv::Point3f(j, i, 0.f));
        }
    }

    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
    cv::cornerSubPix(gray_image, corners, {11, 11}, {-1, -1}, cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 50, 0.0001));
    cv::drawChessboardCorners(image, board_size, corners, patternFound);

//    cv::Mat image = cv::imread(p + "");
//    cv::imshow("chessboard corners", image);
//    cv::waitKey(0);

    return {corners, coordinates};
}

cv::Mat cc::calibrator::Calibrate(const cv::Mat& image)
{
    cv::Mat undistorted_image;
    cv::undistort(image, undistorted_image, intrinsic, distortion_coeffs);

    return undistorted_image;
}

float cc::calibrator::CalculateError()
{
    float total_error = 0.f;
    for (int i = 0; i < image_count; ++i)
    {
        std::vector<cv::Point2f> projected_points;
        cv::projectPoints(coordinates[i], rotation_vectors[i], translation_vectors[i], intrinsic, distortion_coeffs, projected_points);

        for (int j = 0; j < point_count; ++j)
        {
            float diff = 0.f;
            auto difference = corners[i][j] - projected_points[j];
            diff += std::pow(difference.x, 2);
            diff += std::pow(difference.y, 2);

//            std::cout << diff << '\n';

            total_error += diff;
        }
    }

    auto pixel_diff = total_error / float(image_count * point_count);
    auto avg_pix_diff = std::sqrt(pixel_diff);
    std::cout << "Average pixel diff is : " << avg_pix_diff << '\n';

    return total_error;
}