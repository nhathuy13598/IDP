#include "Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage) {
	return 1;
}

int Converter::RGB2HSV(const Mat & sourceImage, Mat & destinationImage)
{
	if (sourceImage.data == NULL) {
		return 1;
	}
	int row = sourceImage.rows;
	int col = sourceImage.cols;
	int channel = sourceImage.channels();

	/*if (row != destinationImage.rows || col != destinationImage.cols || channel != destinationImage.channels()) {
		return 1;
	}*/

	int widthStep = sourceImage.step[0];


	uchar* pData = sourceImage.data;
	uchar* pDataDes = destinationImage.data;

	for (int i = 0; i < row; i++, pData += widthStep, pDataDes += widthStep) {
		uchar* pRow = pData;
		uchar* pRowDes = pDataDes;
		for (int j = 0; j < col; j++, pRow += channel, pRowDes += channel) {
			uchar blue = pRow[0];
			uchar green = pRow[1];
			uchar red = pRow[2];

			float fBlue = float(blue) / 255;
			float fGreen = float(green) / 255;
			float fRed = float(red) / 255;

			float max, min, H, S, V;
			max = min = H = S = V = 0;
			if (blue == red && red == green) {
				max = min = fBlue;
			}
			else {
				/* blue>red */
				if (blue > red) {


					/* red>green */
					if (red > green) {
						/* blue>red>green */
						max = fBlue;
						min = fGreen;
					}


					/* green>red */
					else {
						if (blue > green) {
							/* blue>green>red */
							max = fBlue;
							min = fRed;
						}

						/* green>blue */
						else {
							/* green>blue>red */
							max = fGreen;
							min = fRed;
						}
					}
				}



				/* blue<red */
				else {
					/* blue>green */
					if (blue > green) {
						/* red>blue>green */
						max = fRed;
						min = fGreen;
					}
					/* green>blue */
					else {
						/* green>red */
						if (green > red) {
							/* green>red>blue */
							max = fGreen;
							min = fBlue;
						}
						/* green<red */
						else {
							/* red>green>blue */
							max = fRed;
							min = fBlue;
						}
					}
				}
			}

			V = max;
			if (max == 0) {
				S = 0;
			}
			else {
				S = (max - min) / max;
			}

			if (max == min) {
				H = 0;
			}
			else {
				if (max == fRed) {
					H = 60 * (fGreen - fBlue) / (max - min);
				}
				else if (max == fGreen) {
					H = 60 * (2 + (fBlue - fRed) / (max - min));
				}
				else if (max == fBlue) {
					H = 60 * (4 + (fRed - fGreen) / (max - min));
				}
			}
			if (H < 0) {
				H += 360;
			}

			uchar iV = uchar(V * 255);
			uchar iS = uchar(S * 255);
			uchar iH = uchar(H / 2);

			pRowDes[0] = iH;
			pRowDes[1] = iS;
			pRowDes[2] = iV;
		}
	}
	return 0;
}

int Converter::HSV2RGB(const Mat & sourceImage, Mat & destinationImage)
{
	if (sourceImage.data == NULL) {
		return 1;
	}
	int row = sourceImage.rows;
	int col = sourceImage.cols;
	int channel = sourceImage.channels();

	if ((row != destinationImage.rows) || (col != destinationImage.cols) || (channel != destinationImage.channels())) {
		return 1;
	}

	int widthStep = sourceImage.step[0];


	uchar* pData = sourceImage.data;
	uchar* pDataDes = destinationImage.data;

	for (int i = 0; i < row; i++, pData += widthStep, pDataDes += widthStep) {
		uchar* pRow = pData;
		uchar* pRowDes = pDataDes;
		for (int j = 0; j < col; j++, pRow += channel, pRowDes += channel) {
			float fH = float(pRow[0] * 2);
			float fS = float(pRow[1]) / 255;
			float fV = float(pRow[2]) / 255;



			float fC = fS * fV;
			float fHComma = fH / 60;
			float fX = fC * (1 - abs((int(fHComma) % 2 - 1)));

			float fR1, fG1, fB1;
			fR1 = fG1 = fB1 = 0;
			if (fHComma >= 0 && fHComma <= 1) {
				fR1 = fC;
				fG1 = fX;
				fB1 = 0;
			}
			else if (fHComma <= 2) {
				fR1 = fX;
				fG1 = fC;
				fB1 = 0;
			}
			else if (fHComma <= 3) {
				fR1 = 0;
				fG1 = fC;
				fB1 = fX;
			}
			else if (fHComma <= 4) {
				fR1 = 0;
				fG1 = fX;
				fB1 = fC;
			}
			else if (fHComma <= 5) {
				fR1 = fX;
				fG1 = 0;
				fB1 = fC;
			}
			else if (fHComma <= 6) {
				fR1 = fC;
				fG1 = 0;
				fB1 = fX;
			}

			float m = fV - fC;
			float fR, fG, fB;
			fR = fR1 + m;
			fG = fG1 + m;
			fB = fB1 + m;
			pRowDes[0] = uchar(fB * 255);
			pRowDes[1] = uchar(fG * 255);
			pRowDes[2] = uchar(fR * 255);
		}
	}
	return 0;
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
	switch (type) {
	case 0:
		return RGB2GrayScale(sourceImage, destinationImage);
		break;
	case 1:
		return GrayScale2RGB(sourceImage, destinationImage);
		break;
	case 2:
		return RGB2HSV(sourceImage, destinationImage);
		break;
	case 3:
		return HSV2RGB(sourceImage, destinationImage);
		break;
	}
	return 1;
}

int Converter::Convert(IplImage* sourceImage, IplImage* destinationImage, int type) {
	return 1;
}

Converter::Converter() {

}

Converter::~Converter() {

}
