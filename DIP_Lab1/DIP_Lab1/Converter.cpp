#include "Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

void Converter::RGB2HSI(float r, float g, float b, float &h, float &s, float &i) {
	// chuẩn hóa các giá trị r, g, b
	r = r / 255.0;
	g = g / 255.0;
	b = b / 255.0;
	float minVal = min(min(r, g), b);
	float rg = r - g, rb = r - b, gb = g - b;
	// tìm giá trị cho intensity
	i = (r + g + b) / 3.0;
	// tìm giá trị cho saturation
	s = 1 - ((3.0 * minVal) / (r + g + b));
	// tìm giá trị cho hue
	float degree = (acos(0.5*(rg + rb) / sqrt(rg*rg + rb * gb))) * 180.0 / PI; // Tính góc lệch theo đơn vị độ
	h = degree;
	if (b > g)
		h = 360 - degree;
	if (r == g && g == b)
		h = 0;
	h = h / 360;// chuẩn hóa thông số hue
}

void Converter::HSI2RGB(float h, float s, float i, float &r, float &g, float &b) {
	float tmp = h;
	h *= 360.0; // đưa h về trong khoảng [0, 360]
	// h lúc này là góc lệch của trục hue đang xét so với trục hue đỏ (theo đơn vị độ)
	if (h >= 0 && h < 120) { // trường hợp 1) 0 <= h < 120
		b = i * (1 - s);
		// turn degree to radian
		h = (h * PI) / 180.0;
		r = i * (1 + ((s*cos(h)) / cos((PI / 3) - h)));
		g = 3 * i - (r + b);
	}
	else if (h >= 120 && h < 240) { // trường hợp 2) 120 <= h <240
		h -= 120;
		r = i * (1 - s);
		h = (h*PI) / 180.0;
		g = i * (1 + ((s*cos(h)) / cos(PI / 3 - h)));
		b = 3 * i - (r + g);
	}
	else { // trường hợp 3) h >= 240
		h -= 240;
		g = i * (1 - s);
		h = (h*PI) / 180.0;
		b = i * (1 + ((s*cos(h)) / cos(PI / 3 - h)));
		r = 3 * i - (g + b);
	}
	r *= 255.0;
	g *= 255.0;
	b *= 255.0;
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	return 1;
}

int Converter::Convert(IplImage* sourceImage, IplImage* destinationImage, int type) {
	return 1;
}

Converter::Converter() {

}

Converter::~Converter() {

}
