#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "GeometricTransformer.h"

using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
	Mat inputImage = imread(argv[3],CV_LOAD_IMAGE_COLOR);

	// Kiểm tra xem có mở được ảnh hay không
	if (inputImage.data == NULL) {
		cout << "Khong the mo duoc anh" << endl;
		return 0;
	}
	Mat outputImage;

	PixelInterpolate *interpolator;

	// Kiểm tra pp gán giá trị màu
	if(strcmp(argv[2],"--mn") == 0)
		interpolator = new NearestNeighborInterpolate;
	else if(strcmp(argv[2],"--bl") == 0)
		interpolator = new BilinearInterpolate;
	else {
		cout << strcmp(argv[2],"--bl")  << endl;
		cout << "Interpolation error " << endl;
		return 0;
	}

	GeometricTransformer gt;
	if (strcmp(argv[1],"--zoom") == 0) {
		// Lấy 2 đối số
		gt.Scale(inputImage, outputImage, atof(argv[4]), atof(argv[5]), interpolator);
	}
	else if (strcmp(argv[1],"--rotate") == 0) {
		gt.RotateKeepImage(inputImage, outputImage, atof(argv[4]), interpolator);
	}
	else if (strcmp(argv[1],"--rotateN") == 0) {
		gt.RotateUnkeepImage(inputImage, outputImage, atof(argv[4]), interpolator);
	} 
	else{
		cout << "Command error" << endl;
		return 0;
	}

	// Xóa biến interpolator
	delete interpolator;

	imwrite("output.jpg", outputImage);
	namedWindow("Input Image");
	namedWindow("Output Image");

	imshow("Input Image", inputImage);
	imshow("Output Image", outputImage);
	waitKey(0);

	return 0;
}