#include "ColorTransformer.h"
#include "Converter.h"

struct HSI { // Các thành phần của hệ màu HSI
	float h; // hue
	float s; // saturation
	float in; // intensity
	HSI() {
		h = 0.0;
		s = 0.0;
		in = 0.0;
	}
};

int ColorTransformer::ChangeBrightness(const Mat& sourceImage, Mat& destinationImage, uchar b) {
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	Converter colorConvert;
	destinationImage = Mat(sourceImage.size(), CV_8UC3, Scalar(0, 0, 0));
	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	int widthStep = sourceImage.step[0];
	uchar* pData1 = (uchar*)sourceImage.data;
	uchar* pData2 = (uchar*)destinationImage.data;

	vector<HSI> hsiElement(width*height); // lưu giữ các giá trị của h, s, i tại mỗi pixel

	// tìm các giá trị của h, s, i tại mỗi pixel trong sourceImage
	for (int y = 0; y < height; y++, pData1 += widthStep) {
		uchar* pRow1 = pData1;
		for (int x = 0; x < width; x++, pRow1 += nChannels) {
			int index = y * width + x; // tính index tương ứng để truy cập các phần tử của hsiElement 
			colorConvert.RGB2HSI(pRow1[2], pRow1[1], pRow1[0], hsiElement[index].h, hsiElement[index].s, hsiElement[index].in);
		}
	}
	pData1 = (uchar*)sourceImage.data;
	// histogram equalization cho kênh intensity
	int H[256] = { 0 };
	for (int i = 0; i < width*height; i++) {
		H[int(floor((hsiElement[i].in) * 255.0))]++;
	}
	int T[256];
	T[0] = H[0];
	for (int i = 1; i < 256; i++)
		T[i] = T[i - 1] + H[i];
	for (int i = 0; i < 256; i++)
		T[i] = floor(((float)255.0 / (width * height))*T[i]);
	for (int i = 0; i < width*height; i++) {
		hsiElement[i].in = T[int(floor((hsiElement[i].in) * 255))] / (255.0);
	}
	// chuyển HSI trở lại mô hình RGB
	for (int y = 0; y < height; y++, pData2 += widthStep) {
		uchar* pRow2 = pData2;
		for (int x = 0; x < width; x++, pRow2 += nChannels) {
			float r, g, b;
			HSI tmp = hsiElement[y*width + x];
			colorConvert.HSI2RGB(tmp.h, tmp.s, tmp.in, r, g, b);
			pRow2[0] = floor(b);
			pRow2[1] = floor(g);
			pRow2[2] = floor(r);
		}
	}

	return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histogram) {
	return 1;
}

int ColorTransformer::DrawHistogram(const Mat& sourceImage, Mat& histImage) {
	return 1;
}
ColorTransformer::ColorTransformer() {

}
ColorTransformer::~ColorTransformer() {

}
