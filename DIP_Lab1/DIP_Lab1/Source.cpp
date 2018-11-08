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
	Mat inputImage = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat outputImage;

	Converter convert;
	ColorTransformer transform;
	
	if (strcmp(argv[1], "--rgb2gray") == 0) {

	}
	else if (strcmp(argv[1], "--gray2rgb") == 0) {

	}
	else if (strcmp(argv[1], "--rgb2hsv") == 0) {

	}
	else if (strcmp(argv[1], "--hsv2rgb") == 0) {

	}
	else if (strcmp(argv[1], "--bright") == 0) {

	}
	else if (strcmp(argv[1], "--contrast") == 0) {

	}
	else if (strcmp(argv[1], "--hist") == 0) {

	}
	else if (strcmp(argv[1], "--equalhist") == 0) {
		transform.HistogramEqualization(inputImage, outputImage);
	}
	else if (strcmp(argv[1], "--drawhist") == 0) {

	}

	
	imwrite("output.jpg", outputImage);
	namedWindow("Input Image");
	namedWindow("Output Image");

	imshow("Input Image", inputImage);
	imshow("Output Image", outputImage);
	waitKey(0);
	return 0;
}