#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "GeometricTransformer.h"
#define Pi 3.14

using namespace std;
using namespace cv;
//int main(int argc, char *argv[])
//{
//	Mat inputImage = imread(argv[3], CV_LOAD_IMAGE_COLOR);
//	Mat outputImage;
//
//	/*PixelInterpolate *interpolator;
//	if(strcmp(argv[2],"--nn") == 0)
//		interpolator = new NearestNeighborInterpolate;
//	else if(strcmp(argv[2],"--bl") == 0)
//		interpolator = new BilinearInterpolate;
//	else {
//		cout << strcmp(argv[2],"--bl")  << endl;
//		cout << "Interpolation error " << endl;
//		return 0;
//	}
//
//	GeometricTransformer gt;
//	if (strcmp(argv[1],"--zoom") == 0) {
//		gt.Scale(inputImage, outputImage, atof(argv[4]), atof(argv[4]), interpolator);
//	}
//	else if (strcmp(argv[1],"--rotate") == 0) {
//
//	}
//	else if (strcmp(argv[1],"--rotateN") == 0) {
//
//	} 
//	else{
//		cout << "Command error" << endl;
//		return 0;
//	}*/
//
//	GeometricTransformer gt;
//	PixelInterpolate *interpolator;
//	interpolator = new NearestNeighborInterpolate;
//	gt.Scale(inputImage, outputImage, 2, 2, interpolator);
//
//	imwrite("output.jpg", outputImage);
//	namedWindow("Input Image");
//	namedWindow("Output Image");
//
//	imshow("Input Image", inputImage);
//	imshow("Output Image", outputImage);
//	waitKey(0);
//
//	return 0;
//}

void main() {
	Mat inputImage = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat outputImage(inputImage.rows, inputImage.cols, CV_8UC3);
	GeometricTransformer gt;
	
	PixelInterpolate *pi = new NearestNeighborInterpolate();



	gt.RotateUnkeepImage(inputImage, outputImage,0, pi);
	delete pi;
	
	imshow("input", inputImage);
	imshow("output", outputImage);
	waitKey(0);
}