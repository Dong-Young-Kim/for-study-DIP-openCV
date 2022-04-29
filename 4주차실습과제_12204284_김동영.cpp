#include <iostream>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

int myKernerConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
	int sum = 0;
	int sumKernel = 0;

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
				sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
				sumKernel += kernel[i + 1][j + 1];
			}
		}
	}

	if (sumKernel != 0) { return sum / sumKernel; }
	else return sum;
}

int myKernerConv9x9(uchar* arr, int kernel[][9], int x, int y, int width, int height) {
	int sum = 0;
	int sumKernel = 0;

	for (int j = -4; j <= 4; j++) { //반영할 커널 사이즈 9x9에 맟추어 적용 -4 ~ 4
		for (int i = -4; i <= 4; i++) {
			if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
				sum += arr[(y + j) * width + (x + i)] * kernel[i + 4][j + 4];//커널 사이즈 맟추어 +4
				sumKernel += kernel[i + 4][j + 4];
			}
		}
	}

	if (sumKernel != 0) { return sum / sumKernel; }
	else return sum;
}

Mat my9x9GaussianFilter(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;
	int kernel[9][9];
	double sigma = 1.5;
	for (int x = -4; x <= 4; ++x)
		for (int y = -4; y <= 4; ++y) {
			kernel[x + 4][y + 4] = 1000000 * exp(-0.5 * (pow(x, 2.0) + pow(y, 2.0)) / sigma / sigma)
				/ (2 * 3.141592 * sigma * sigma);
		}

	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = myKernerConv9x9(srcData, kernel, x, y, width, height);
		}
	}

	return dstImg;
}

Mat GetHistogram(Mat src) {
	Mat histogram;
	const int* channel_numbers = { 0 };
	float channel_range[] = { 0.0, 255.0 };
	const float* channel_ranges = channel_range;
	int number_bins = 255;

	calcHist(&src, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / number_bins);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
	normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	for (int i = 1; i < number_bins; i++) {
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(histogram.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
	}

	return histImage;
}

Mat SpreadSaltsAndPepper(Mat img, int num) {
	for (int n = 0; n < num; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1) img.at<uchar>(y, x) = 255;
		else {
			img.at<Vec3b>(y, x)[0] = 255;
			img.at<Vec3b>(y, x)[1] = 255;
			img.at<Vec3b>(y, x)[2] = 255;
		}
	}
	for (int n = 0; n < num; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1) img.at<uchar>(y, x) = 0;
		else {
			img.at<Vec3b>(y, x)[0] = 0;
			img.at<Vec3b>(y, x)[1] = 0;
			img.at<Vec3b>(y, x)[2] = 0;
		}
	}
	return img;

}

Mat mySobelFilter(Mat srcImg) {
	int kernelX[3][3] = { -1, 0, 1,
						  -2, 0, 2,
						  -1, 0, 1 };
	int kernelY[3][3] = { -1, -2, -1,
						   0,  0,  0,
						   1,  2,  1 };
	int kernel45[3][3] = { 0, 1, 2,
						  -1, 0, 1,
						  -2,-1, 0 };
	int kernel135[3][3] = { -2, -1,0,
							-1, 0, 1,
							 0, 1, 2 };
	int width = srcImg.cols;
	int height = srcImg.rows;
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	//for (int y = 0; y < height; y++) {
	//	for (int x = 0; x < width; x++) {
	//		dstData[y * width + x] = (abs(myKernerConv3x3(srcData, kernelX, x, y, width, height)) + abs(myKernerConv3x3(srcData, kernelY, x, y, width, height)) / 2);
	//	}
	//}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernerConv3x3(srcData, kernel135, x, y, width, height)));
		}
	}

	return dstImg;
}

int myColorKernerConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
	int sum = 0;
	int sumKernel = 0;

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			if ((y + j) >= 0 && (y + j) < 3 * height && (x + i) >= 0 && (x + i) < 3 * width) {
				sum += arr[(y + 3 * j) * width + (x + 3 * i)] * kernel[i + 1][j + 1];
				sumKernel += kernel[i + 1][j + 1];
			}
		}
	}

	if (sumKernel != 0) { return sum / sumKernel; }
	else return sum;
}

Mat myColorGaussianFilter(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;
	int kernel[3][3] = { 1, 2, 1,
						 2, 4, 2,
						 1, 2, 1 };

	Mat dstImg(srcImg.size(), CV_8UC3);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < 3 * height; y += 3) {
		for (int x = 0; x < 3 * width; x += 3) {
			dstData[y * width + x] = myColorKernerConv3x3(srcData, kernel, x, y, width, height);
			dstData[y * width + x + 1] = myColorKernerConv3x3(srcData, kernel, x + 1, y, width, height);
			dstData[y * width + x + 2] = myColorKernerConv3x3(srcData, kernel, x + 2, y, width, height);
		}
	}

	return dstImg;
}

Mat myColorSampling(Mat srcImg) {
	int width = (srcImg.cols / 2);
	int height = (srcImg.rows / 2);
	Mat dstImg(height, width, CV_8UC3);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < 3 * height; y += 3) {
		for (int x = 0; x < 3 * width; x += 3) {
			dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
			dstData[y * width + x + 1] = srcData[(y * 2) * (width * 2) + (x * 2) + 1];
			dstData[y * width + x + 2] = srcData[(y * 2) * (width * 2) + (x * 2) + 2];
		}
	}
	return dstImg;
}

vector<Mat> myGaussianPyramid(Mat srcImg) {
	vector<Mat> Vec;
	Vec.push_back(srcImg);
	for (int i = 0; i < 4; i++) {
		srcImg = myColorSampling(srcImg);
		srcImg = myColorGaussianFilter(srcImg);
		Vec.push_back(srcImg);
	}
	return Vec;
}

vector<Mat> myLaplacianPyramid(Mat srcImg) {
	vector<Mat> Vec;
	Vec.push_back(srcImg);
	for (int i = 0; i < 4; i++) {
		if (i != 3) {
			Mat highImg = srcImg;

			srcImg = myColorSampling(srcImg);
			srcImg = myColorGaussianFilter(srcImg);

			Mat lowImg = srcImg;
			resize(lowImg, lowImg, highImg.size());
			Vec.push_back(highImg - lowImg + 128);
		}
		else Vec.push_back(srcImg);
	}
	return Vec;
}

void restoreColorLaplacian(Mat srcImg) {
	vector<Mat> VecLap = myLaplacianPyramid(srcImg);
	reverse(VecLap.begin(), VecLap.end());
	Mat dst_img;
	for (int i = 0; i < VecLap.size(); i++) {
		if (i == 0) dst_img = VecLap[i];
		else {
			resize(dst_img, dst_img, VecLap[i].size());
			dst_img = dst_img + VecLap[i] - 128;
		}
		imshow("restore Color Laplacian", dst_img);
		waitKey(0);
		destroyWindow("restore Color Laplacian");
	}

}

int main() {
	Mat imgA = imread("image/gear.jpg", 0);
	Mat img_col = imread("image/gear.jpg", 1);

	Mat imgB = my9x9GaussianFilter(imgA);
	imshow("9x9_Gaussian_Filter_12204284", imgB);
	//Mat imgA1 = GetHistogram(imgA);
	//imshow("original_img_histogram_12204284", imgA1);
	//Mat imgB1 = GetHistogram(imgB);
	//imshow("Gaussian_img_histogram_12204284", imgB1);

	//Mat imgB = SpreadSaltsAndPepper(imgA, 3000);
	//imshow("SpreadSalts_And_Pepper_12204284", imgB);
	//Mat imgC = my9x9GaussianFilter(imgB);
	//imshow("9x9_Gaussian_Filter_after_salt_pepper_12204284", imgC);

	//Mat imgB = mySobelFilter(imgA);
	//imshow("135deg_sobel_Filter_12204284", imgB);

	//vector<Mat> vecMat = myGaussianPyramid(img_col);
	//imshow("Color Gaussian pyramid 1", vecMat[1]);
	//imshow("Color Gaussian pyramid 2", vecMat[2]);
	//imshow("Color Gaussian pyramid 3", vecMat[3]);
	//imshow("Color Gaussian pyramid 4", vecMat[4]);

	//vector<Mat> vecMat = myLaplacianPyramid(img_col);
	//imshow("Color Laplacian pyramid 1", vecMat[1]);
	//imshow("Color Laplacian pyramid 2", vecMat[2]);
	//imshow("Color Laplacian pyramid 3", vecMat[3]);
	//imshow("Color Laplacian pyramid 4", vecMat[4]);

	//restoreColorLaplacian(img_col);

	waitKey(0);
	destroyAllWindows();

}