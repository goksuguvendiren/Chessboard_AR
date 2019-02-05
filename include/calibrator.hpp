//
//  calibrator.hpp
//  ass2
//
//  Created by Göksu Güvendiren on 2.02.2019.
//  Copyright © 2019 Göksu Güvendiren. All rights reserved.
//

#pragma once

#include <vector>
#include <experimental/optional>
#include <opencv2/opencv.hpp>

namespace cc
{
	class calibrator
	{
		unsigned int image_count;
		unsigned int point_count;
		std::vector<std::vector<cv::Point2f>> corners;
		std::vector<std::vector<cv::Point3f>> coordinates;

		cv::Mat intrinsic = cv::Mat(3, 3, CV_64FC1);
		cv::Mat distortion_coeffs;

		std::vector<cv::Mat> rotation_vectors;
		std::vector<cv::Mat> translation_vectors;

		cv::Size size;

		std::pair<std::vector<cv::Point2f>, std::vector<cv::Point3f>> GetPoints(const cv::Mat& image, const cv::Size& board_size);

	public:
		calibrator(const std::vector<cv::Mat>& images, const cv::Size& board_size);

		cv::Mat Calibrate(const cv::Mat& image);
		float CalculateError();

//		void SaveCameraSettings(const std::string& path, const std::string& filename);

		cv::Mat Intrinsic() const { return intrinsic; }
		cv::Mat DistortionCoeffs() const { return distortion_coeffs; }

		std::vector<cv::Mat> RotationVectors() const { return rotation_vectors; }
		std::vector<cv::Mat> TranslationVectors() const { return translation_vectors; }

		cv::Size Size() const { return size; }


	};
}
