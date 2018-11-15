#pragma once
#include <iostream>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Converter.h"
#include "ColorTransformer.h"
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
	Mat inputImage = imread(argv[2], CV_LOAD_IMAGE_COLOR);
	Mat outputImage;

	Converter convert;
	ColorTransformer transform;
	
	if (strcmp(argv[1], "--rgb2gray") == 0) {
		convert.Convert(inputImage, outputImage, 0);
	}
	else if (strcmp(argv[1], "--gray2rgb") == 0) {
		convert.Convert(inputImage, outputImage, 1);
	}
	else if (strcmp(argv[1], "--rgb2hsv") == 0) {
		outputImage= Mat(inputImage.rows, inputImage.cols, CV_8UC3);
		convert.Convert(inputImage, outputImage, 2);
	}
	else if (strcmp(argv[1], "--hsv2rgb") == 0) {
		outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
		convert.Convert(inputImage, outputImage, 3);
	}
	else if (strcmp(argv[1], "--bright") == 0) {
		transform.ChangeBrightness(inputImage, outputImage, atoi(argv[3]));
	}
	else if (strcmp(argv[1], "--contrast") == 0) {
		transform.ChangeContrast(inputImage, outputImage, atoi(argv[3]));
	}
	else if (strcmp(argv[1], "--hist") == 0) {
		outputImage = Mat::zeros(inputImage.size(), inputImage.type());
		transform.CalcHistogram(inputImage, outputImage);
	}
	else if (strcmp(argv[1], "--equalhist") == 0) {
		transform.HistogramEqualization(inputImage, outputImage);
	}
	else if (strcmp(argv[1], "--drawhist") == 0) {
		transform.DrawHistogram(inputImage, outputImage);
	}

	
	imwrite("output.jpg", outputImage);

	imshow("Input Image", inputImage);
	imshow("Output Image", outputImage);
	waitKey(0);
	return 0;
}