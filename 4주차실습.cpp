#include <iostream>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;



Mat myCopy(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;


	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = srcData[y * width + x];
		}
	}

	return dstImg;
}

int myKernerConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
	int sum = 0;
	int sumKernel = 0;

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
				sum += arr[(y + j) * width + (x + i)] * kernel[i + 3][j + 3];
				sumKernel += kernel[i + 3][j + 3];
			}
		}
	}

	if (sumKernel != 0) { return sum / sumKernel; }
	else return sum;
}


Mat myGaussianFilter(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;
	int kernel[3][3] = { 1, 2, 1,
						 2, 4, 2,
						 1, 2, 1 };

	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = myKernerConv3x3(srcData, kernel, x, y, width, height);
		}
	}

	return dstImg;
}

Mat mySobelFilter(Mat srcImg) {
	int kernelX[3][3] = { -1, 0, 1,
					      -2, 0, 2,
					      -1, 0, 1 };
	int kernelY[3][3] = { -1, -2, -1,
						   0,  0,  0,
						   1,  2,  1};

	int width = srcImg.cols;
	int height = srcImg.rows;
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernerConv3x3(srcData, kernelX, x, y, width, height)) + abs(myKernerConv3x3(srcData, kernelX, x, y, width, height)) / 2);
		}
	}

	return dstImg;
}

Mat mySampling(Mat srcImg) {
	int width = (srcImg.cols / 2);
	int height = (srcImg.rows / 2);
	Mat dstImg(height, width, CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
		}
	}

	return dstImg;
}

vector<Mat> myGaussianPyramid(Mat srcImg) {
	vector<Mat> Vec;
	Vec.push_back(srcImg);
	for (int i = 0; i < 4; i++) {
		srcImg = mySampling(srcImg);
		srcImg = myGaussianFilter(srcImg);
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

			srcImg = mySampling(srcImg);
			srcImg = myGaussianFilter(srcImg);

			Mat lowImg = srcImg;
			resize(lowImg, lowImg, highImg.size());
			Vec.push_back(highImg - lowImg + 128);
		}
		else Vec.push_back(srcImg);
	}
	return Vec;
}

//vector<Mat> myRestoreLaplacianPyramid(vector<Mat> VecLab) {
//	vector<Mat> Vec;
//	Vec.push_back(srcImg);
//	for (int i = 0; i < 4; i++) {
//		if (i != 3) {
//			Mat highImg = srcImg;
//
//			srcImg = mySampling(srcImg);
//			srcImg = myGaussianFilter(srcImg);
//
//			Mat lowImg = srcImg;
//			resize(lowImg, lowImg, highImg.size());
//			Vec.push_back(highImg - lowImg + 128);
//		}
//		else Vec.push_back(srcImg);
//	}
//	return Vec;
//}

void ex5() {
	Mat src_img = imread("image/gear.jpg", 0);
	vector<Mat> VecLap = myLaplacianPyramid(src_img);
	reverse(VecLap.begin(), VecLap.end());
	Mat dst_img;
	for (int i = 0; i < VecLap.size(); i++){
		if (i == 0) dst_img = VecLap[i];
		else {
			resize(dst_img, dst_img, VecLap[i].size());
			dst_img = dst_img + VecLap[i] - 128;
		}
		imshow("ex5", dst_img);
		waitKey(0);
		destroyWindow("ex5");
	}

}



int main() {
	Mat imgA = imread("image/gear.jpg", 1);

	Mat imgB = myGaussianFilter(imgA);

	imshow("Tests_12204284", imgA);
	waitKey(0);
	destroyWindow("Tests_12204284");
	imshow("Tests_12204284", imgB);

	waitKey(0);
	destroyAllWindows();

	//ex5();

}
