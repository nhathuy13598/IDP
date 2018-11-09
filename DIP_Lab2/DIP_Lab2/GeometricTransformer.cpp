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
	float data[9] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, dx, dy, 1.0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
}
void AffineTransform::Rotate(float angle) {
	float data[9] = { cos(angle), sin(angle), 0.0, -1*sin(angle), cos(angle), 0.0, 0.0, 0.0, 1.0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i*3+j];
		}
	}
	cout << _matrixTransform << endl;
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
	Mat matrixInverse = _matrixTransform.inv();// ma trận nghịch đảo của matrixInverse
	float v, w;
	v = x * matrixInverse.at<float>(0, 0) + y * matrixInverse.at<float>(1, 0) + matrixInverse.at<float>(2, 0);
	w = x * matrixInverse.at<float>(0, 1) + y * matrixInverse.at<float>(1, 1) + matrixInverse.at<float>(2, 1);
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
			transformer->TransformPoint(x1, y1);
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
	return 1;
}
int GeometricTransformer::RotateUnkeepImage(
	const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator) {
	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC3);
	uchar* pSrc = srcImage.data;
	uchar* pDst = dstImage.data;
	int rows = srcImage.rows;
	int cols = srcImage.cols;
	int channels = srcImage.channels();
	int widthStep = srcImage.step[0];
	// Tạo ma trận Transform
	AffineTransform *at = new AffineTransform();
	at->Rotate(angle);
	uchar* pRowDst = pDst;
	for (int i = 0; i < rows; i++ , pRowDst += widthStep) {		
		for (int j = 0; j < cols; j++ , pDst += channels) {
			float iTemp = i;
			float jTemp = j;
			// Tìm điểm ảnh trên ảnh gốc dựa vào ảnh kết quả * ma trận nghịch đảo Transform
			at->TransformPoint(iTemp,jTemp);

			// Nội suy giá trị màu
			interpolator->Interpolate(iTemp, jTemp, pSrc, widthStep, channels, pDst);
		}
	}
	delete at;
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
	transformer->Scale(sx, sy);
	Transform(srcImage, dstImage, transformer, interpolator);
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
