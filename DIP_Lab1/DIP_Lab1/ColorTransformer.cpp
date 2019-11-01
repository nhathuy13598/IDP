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
	if (!sourceImage.data)
		return -1;

	int cols = sourceImage.cols;
	int rows = sourceImage.rows;

	destinationImage = Mat::zeros(sourceImage.size(), sourceImage.type());

	if (!destinationImage.data)
		return -1;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			destinationImage.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[0] + b);
			destinationImage.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[1] + b);
			destinationImage.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[2] + b);
		}
	}
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	if (!sourceImage.data)
		return -1;

	int cols = sourceImage.cols;
	int rows = sourceImage.rows;

	destinationImage = Mat::zeros(sourceImage.size(), sourceImage.type());

	if (!destinationImage.data)
		return -1;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			destinationImage.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[0] * c);
			destinationImage.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[1] * c);
			destinationImage.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(sourceImage.at<Vec3b>(i, j)[2] * c);
		}
	}
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
	if (!sourceImage.data)
		return 0;

	int cols = sourceImage.cols;
	int rows = sourceImage.rows;

	if (sourceImage.channels() == 1)
	{
		histogram = Mat(Size(1, 256), CV_32FC1);

		for (int i = 0; i < 256; i++)
			histogram.at<float>(i) = 0;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				histogram.at<float>(sourceImage.at<uchar>(i, j)) += 1;
	}
	else if (sourceImage.channels() == 3)
	{
		Mat* channels = new Mat[3];
		split(sourceImage, channels);

		histogram = Mat(Size(1, 256), CV_32FC3);
		int cols = sourceImage.cols;
		int rows = sourceImage.rows;

		for (int i = 0; i < 256; i++)
		{
			histogram.at<Vec3f>(i)[0] = 0;
			histogram.at<Vec3f>(i)[1] = 0;
			histogram.at<Vec3f>(i)[2] = 0;
		}

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
			{
				histogram.at<Vec3f>(channels[0].at<uchar>(i, j))[0] += 1;
				histogram.at<Vec3f>(channels[1].at<uchar>(i, j))[1] += 1;
				histogram.at<Vec3f>(channels[2].at<uchar>(i, j))[2] += 1;
			}
	}

	return 1;
}

int ColorTransformer::DrawHistogram(const Mat& sourceImage, Mat& histImage) {
	if (!sourceImage.data)
		return 0;

	Mat histogram;

	int result = CalcHistogram(sourceImage, histogram);

	if (result == 0)
		return 0;

	//establish the histograms
	int histogram_width = 512;
	int histogram_height = 400;
	int histogram_size = 256;

	//establish each bin's width
	int bin_width = cvRound((double)histogram_width / histogram_size);

	if (sourceImage.channels() == 1)
	{
		histImage = Mat(histogram_height, histogram_width, CV_8UC1, Scalar(0, 0, 0));

		Normalize(histogram, histogram, 0, histogram.rows);

		for (int i = 1; i < histogram_size; i++)
		{
			line(histImage,
				Point(bin_width*(i - 1), histogram_height - cvRound(histogram.at<float>(i - 1))),
				Point(bin_width*(i), histogram_height - cvRound(histogram.at<float>(i))),
				Scalar(255, 255, 255), 2, 8, 0);
		}
	}
	else if (sourceImage.channels() == 3)
	{
		//Create an image to display, with black screen
		histImage = Mat(histogram_height, histogram_width, CV_8UC3, Scalar(0, 0, 0));

		//Normalize the result to [0, histImage.rows]
		Normalize(histogram, histogram, 0, histImage.rows);

		//draw each channel
		for (int i = 1; i < histogram_size; i++)
		{
			line(histImage,
				Point(bin_width*(i - 1), histogram_height - cvRound(histogram.at<Vec3f>(i - 1)[0])),
				Point(bin_width*(i), histogram_height - cvRound(histogram.at<Vec3f>(i)[0])),
				Scalar(255, 0, 0), 2, 8, 0);
			line(histImage,
				Point(bin_width*(i - 1), histogram_height - cvRound(histogram.at<Vec3f>(i - 1)[1])),
				Point(bin_width*(i), histogram_height - cvRound(histogram.at<Vec3f>(i)[1])),
				Scalar(0, 255, 0), 2, 8, 0);
			line(histImage,
				Point(bin_width*(i - 1), histogram_height - cvRound(histogram.at<Vec3f>(i - 1)[2])),
				Point(bin_width*(i), histogram_height - cvRound(histogram.at<Vec3f>(i)[2])),
				Scalar(0, 0, 255), 2, 8, 0);
		}
	}
	return 1;
}
void ColorTransformer::Normalize(const Mat& source_histogram, Mat& destination_histogram, double alpha, double beta)
{
	if (source_histogram.channels() == 1)
	{
		int min = source_histogram.at<float>(0);
		int max = source_histogram.at<float>(0);

		for (int i = 1; i < 256; i++)
		{
			if (max < source_histogram.at<float>(i))
				max = source_histogram.at<float>(i);
			if (min > source_histogram.at<float>(i))
				min = source_histogram.at<float>(i);
		}

		int delta = max - min;

		for (int i = 0; i < 256; i++)
			destination_histogram.at<float>(i) = float(source_histogram.at<float>(i) - min) / delta * (beta - alpha);
	}
	else if (source_histogram.channels() == 3)
	{
		for (int p = 0; p < 3; p++)
		{
			int min = source_histogram.at<Vec3f>(0)[p];
			int max = source_histogram.at<Vec3f>(0)[p];

			for (int i = 1; i < 256; i++)
			{
				if (max < source_histogram.at<Vec3f>(i)[p])
					max = source_histogram.at<Vec3f>(i)[p];
				if (min > source_histogram.at<Vec3f>(i)[p])
					min = source_histogram.at<Vec3f>(i)[p];
			}

			int delta = max - min;

			for (int i = 0; i < 256; i++)
				destination_histogram.at<Vec3f>(i)[p] = float(source_histogram.at<Vec3f>(i)[p] - min) / delta * (beta - alpha);
		}
	}

}
ColorTransformer::ColorTransformer() {

}
ColorTransformer::~ColorTransformer() {

}
