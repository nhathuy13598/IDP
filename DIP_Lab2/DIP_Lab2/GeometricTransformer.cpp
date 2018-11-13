#include "GeometricTransformer.h"
int flag = 1;
float EuclidDistance(float x1, float y1, float x2, float y2) {
	return sqrt(pow((x1 - x2), 2.0) + pow((y1 - y2), 2.0));
}

void BilinearInterpolate::Interpolate(
	float tx, float ty,
	uchar* pSrc, int srcWidthStep, int nChannels,
	uchar* pDstRow) {
	int nb[4][2] = { {floor(tx), floor(ty)}, // tọa độ của 4 điểm lân cận gần nhất của điểm (tx, ty)
					 {ceil(tx), floor(ty)},
					 {ceil(tx), ceil(ty)},
					 {floor(tx), ceil(ty)} };
	int colorVal[4][3]; // giá trị r, g, b của 4 điểm lân cận
	uchar* pPoint;
	for (int i = 0; i < 4; i++) {
		pPoint = pSrc;
		pPoint += nb[i][1] * srcWidthStep + nb[i][0] * nChannels;
		colorVal[i][2] = pPoint[2];//r
		colorVal[i][1] = pPoint[1];//g
		colorVal[i][0] = pPoint[0];//b

	}
	// distance
	float a = float(tx) - nb[0][0], b = float(ty) - nb[0][1];
	// r, g, b interpolation
	float rVal = b * a*colorVal[0][2] + b * (1 - a)*colorVal[1][2] + (1 - b)*(1 - a)*colorVal[2][2] + (1 - b)*a*colorVal[3][2];
	float gVal = b * a*colorVal[0][1] + b * (1 - a)*colorVal[1][1] + (1 - b)*(1 - a)*colorVal[2][1] + (1 - b)*a*colorVal[3][1];
	float bVal = b * a*colorVal[0][0] + b * (1 - a)*colorVal[1][0] + (1 - b)*(1 - a)*colorVal[2][0] + (1 - b)*a*colorVal[3][0];
	pDstRow[2] = floor(rVal);
	pDstRow[1] = floor(gVal);
	pDstRow[0] = floor(bVal);
}

void NearestNeighborInterpolate::Interpolate(
	float tx, float ty,
	uchar* pSrc, int srcWidthStep, int nChannels,
	uchar* pDstRow) {

	int nb[4][2] = { {floor(tx), floor(ty)}, // tọa độ của 4 điểm lân cận gần nhất của điểm (tx, ty)
					 {ceil(tx), floor(ty)},
					 {ceil(tx), ceil(ty)},
					 {floor(tx), ceil(ty)} };
	float minDist = 999999999;
	int chosenIndex = 0;
	for (int i = 0; i < 4; i++) { // tìm điểm có khoảng cách ngắn nhất so với (tx, ty)
		if (nb[i][0] >= 0 && nb[i][1] >= 0) {
			float dist = EuclidDistance(tx, ty, nb[i][0], nb[i][1]);
			if (dist < minDist) {
				minDist = dist;
				chosenIndex = i;
			}
		}
	}
	uchar* pPoint = pSrc;
	if (minDist != 999999999) {
		pPoint += nb[chosenIndex][1] * srcWidthStep + nb[chosenIndex][0] * nChannels;
		pDstRow[2] = pPoint[2];
		pDstRow[1] = pPoint[1];
		pDstRow[0] = pPoint[0];
	}
	else {
		pDstRow[2] = 0;
		pDstRow[1] = 0;
		pDstRow[0] = 0;
	}
}

void AffineTransform::Translate(float dx, float dy) {
	float data[9] = { 1,0,dx,0,1,dy,0,0,1 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
}
void AffineTransform::Rotate(float angle) {
	float data[9] = { cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0.0, 0.0, 0.0, 1.0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
}
void AffineTransform::Scale(float sx, float sy) {
	float data[9] = { sx, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, 1.0};
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
}
void AffineTransform::TransformPoint(float &x, float &y) {
	
	float v, w;
	v = x * _matrixTransform.at<float>(0, 0) + y * _matrixTransform.at<float>(0,1) + _matrixTransform.at<float>(0,2);
	w = x * _matrixTransform.at<float>(1,0) + y * _matrixTransform.at<float>(1, 1) + _matrixTransform.at<float>(1, 2);
	x = v;
	y = w;
}

int GeometricTransformer::Transform(
	const Mat &beforeImage,
	Mat &afterImage,
	AffineTransform* transformer,
	PixelInterpolate* interpolator) {

	uchar* pData1 = (uchar*)beforeImage.data;
	uchar* pData2 = (uchar*)afterImage.data;
	int height = afterImage.rows, width = afterImage.cols;
	int widthStep1 = beforeImage.step[0];
	int widthStep2 = afterImage.step[0];
	int nChannels = afterImage.channels();

	for (int y2 = 0; y2 < height; y2++, pData2 += widthStep2) {
		uchar* pRow2 = pData2;
		uchar* pPoint;
		for (int x2 = 0; x2 < width; x2++, pRow2 += nChannels) {
			float x1 = x2, y1 = y2;
			
			
			transformer->TransformPoint(x1,y1); 
			pPoint = pData1;
			
			if (int(x1) == x1 && int(y1) == y1) {//backward mapping cho diem trung voi diem trong input
				
				pPoint += int(y1) * widthStep1 + int(x1) * nChannels;
				pRow2[0] = pPoint[0];
				pRow2[1] = pPoint[1];
				pRow2[2] = pPoint[2];
			}
			else {
				interpolator->Interpolate(x1, y1, pPoint, widthStep1, nChannels, pRow2); 
			}
		}
	}
	return 1;
}

int GeometricTransformer::RotateKeepImage(
	const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator) {

	// Tạo khung hình mới
	int diagonal = (int)sqrt(srcImage.cols*srcImage.cols + srcImage.rows*srcImage.rows);
	int newWidth = diagonal;
	int newHeight = diagonal;


	dstImage = Mat(newHeight, newWidth, CV_8UC3, Scalar(0, 0, 0));
	AffineTransform* transformer = new AffineTransform;
	transformer->Rotate(angle);

	int offsetX = (newWidth - srcImage.cols) / 2;
	int offsetY = (newHeight - srcImage.rows) / 2;

	dstImage = Mat(newWidth, newHeight, srcImage.type());

	Mat frame = Mat(newWidth, newHeight, srcImage.type());

	// Hiệu chỉnh hình ảnh về giữa
	srcImage.copyTo(frame.colRange(offsetX, offsetX + srcImage.cols).rowRange(offsetY, offsetY + srcImage.rows));


	uchar* pData1 = (uchar*)frame.data;
	uchar* pData2 = (uchar*)dstImage.data;
	int nchannels = srcImage.channels();
	int widthStep1 = frame.step[0];
	int widthStep2 = dstImage.step[0];

	// Lấy dòng và cột ảnh dstImage
	int rows = dstImage.rows;
	int cols = dstImage.cols;

	// Lấy nửa dòng và cột ảnh dstImage
	int rowHalf = rows / 2;
	int colHalf = cols / 2;
	for (int y2 = -rowHalf; y2 < rowHalf; y2++, pData2 += widthStep2)
	{
		uchar* pRow2 = pData2;
		uchar* pPoint;
		float x1, y1;
		for (int x2 = -colHalf; x2 < colHalf; x2++, pRow2 += nchannels)
		{
			x1 = x2;
			y1 = y2;
			transformer->TransformPoint(x1, y1);
			x1 += colHalf;
			y1 += rowHalf;
			if (x1 < 0 || x1 > cols || y1 < 0 || y1 > rows)
				continue;
			pPoint = pData1;
			if (int(x1) == x1 && int(y1) == y1)
			{
				pPoint += int(y1) *widthStep1 + int(x1) *nchannels;
				pRow2[0] = pPoint[0];
				pRow2[1] = pPoint[1];
				pRow2[2] = pPoint[2];
			}
			else
				interpolator->Interpolate(x1, y1, pPoint, widthStep1, nchannels, pRow2);
		}
	}	
	delete transformer;
	return 1;
}
int GeometricTransformer::RotateUnkeepImage(
	const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator) {

	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0, 0, 0));
	AffineTransform* transformer = new AffineTransform;
	transformer->Rotate(angle);

	Mat frame = srcImage;

	uchar* pData1 = (uchar*)frame.data;
	uchar* pData2 = (uchar*)dstImage.data;
	int nchannels = srcImage.channels();
	int widthStep1 = frame.step[0];
	int widthStep2 = dstImage.step[0];

	// Lấy dòng và cột ảnh dstImage
	int rows = dstImage.rows;
	int cols = dstImage.cols;

	// Lấy nửa dòng và cột ảnh dstImage
	int rowHalf = rows / 2;
	int colHalf = cols / 2;

	for (int y2 = -rowHalf; y2 < rowHalf; y2++, pData2 += widthStep2)
	{
		uchar* pRow2 = pData2;
		uchar* pPoint;
		float x1, y1;
		for (int x2 = -colHalf; x2 < colHalf; x2++, pRow2 += nchannels)
		{
			x1 = x2;
			y1 = y2;
			transformer->TransformPoint(x1, y1);
			x1 += colHalf;
			y1 += rowHalf;
			if (x1 < 0 || x1 > cols || y1 < 0 || y1 > rows)
				continue;
			pPoint = pData1;
			if (int(x1) == x1 && int(y1) == y1)
			{
				pPoint += int(y1) *widthStep1 + int(x1) *nchannels;
				pRow2[0] = pPoint[0];
				pRow2[1] = pPoint[1];
				pRow2[2] = pPoint[2];
			}
			else
				interpolator->Interpolate(x1, y1, pPoint, widthStep1, nchannels, pRow2);
		}
	}

	delete transformer;
	return 1;
}
int GeometricTransformer::Scale(
	const Mat &srcImage,
	Mat &dstImage,
	float sx, float sy,
	PixelInterpolate* interpolator) {


	dstImage = Mat(srcImage.rows*sy, srcImage.cols*sx, CV_8UC3, Scalar(0, 0, 0));
	AffineTransform *transformer;
	transformer = new AffineTransform;
	transformer->Scale(1 / sx, 1 / sy);
	Transform(srcImage, dstImage, transformer, interpolator);
	delete transformer;
	return 1;
}

PixelInterpolate::PixelInterpolate() {

}
PixelInterpolate::~PixelInterpolate() {

}
BilinearInterpolate::BilinearInterpolate() {

}
BilinearInterpolate::~BilinearInterpolate() {

}
NearestNeighborInterpolate::NearestNeighborInterpolate() {

}
NearestNeighborInterpolate::~NearestNeighborInterpolate() {

}
AffineTransform::AffineTransform() {
	float data[9] = { 0,0,0,0,0,0,0,0,0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
}
AffineTransform::~AffineTransform() {

}
GeometricTransformer::GeometricTransformer() {

}
GeometricTransformer::~GeometricTransformer() {

}
