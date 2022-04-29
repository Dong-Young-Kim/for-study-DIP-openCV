#include <iostream>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

Mat padding(Mat img) {
	int dftRows = getOptimalDFTSize(img.rows);
	int dftCols = getOptimalDFTSize(img.cols);

	Mat padded;
	copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));

	return padded;
}

Mat doDft(Mat src_img) {
	Mat float_img;
	src_img.convertTo(float_img, CV_32F);

	Mat complex_img;
	dft(float_img, complex_img, DFT_COMPLEX_OUTPUT);

	return complex_img;
}

Mat getMagnitude(Mat complexImg) {
	Mat planes[2];
	split(complexImg, planes);

	Mat magImg;
	magnitude(planes[0], planes[1], magImg);
	magImg += Scalar::all(1);
	log(magImg, magImg); //값이 많아 로그를 취함

	return magImg;
}

Mat myNormalize(Mat src) {
	Mat dst;
	src.copyTo(dst);
	normalize(dst, dst, 0, 255, NORM_MINMAX);
	dst.convertTo(dst, CV_8UC1);

	return dst;
}

Mat getPhase(Mat complexImg) {
	Mat planes[2];
	split(complexImg, planes);

	Mat phaImg;
	phase(planes[0], planes[1], phaImg);

	return phaImg;
}

Mat centralize(Mat complex) {
	Mat planes[2];
	split(complex, planes);
	int cx = planes[0].cols / 2;
	int cy = planes[1].rows / 2;

	Mat q0Re(planes[0], Rect(0, 0, cx, cy));
	Mat q1Re(planes[0], Rect(cx, 0, cx, cy));
	Mat q2Re(planes[0], Rect(0, cy, cx, cy));
	Mat q3Re(planes[0], Rect(cx, cy, cx, cy));

	Mat tmp;
	q0Re.copyTo(tmp);
	q3Re.copyTo(q0Re);
	tmp.copyTo(q3Re);
	q1Re.copyTo(tmp);
	q2Re.copyTo(q1Re);
	tmp.copyTo(q2Re);

	Mat q0Im(planes[1], Rect(0, 0, cx, cy));
	Mat q1Im(planes[1], Rect(cx, 0, cx, cy));
	Mat q2Im(planes[1], Rect(0, cy, cx, cy));
	Mat q3Im(planes[1], Rect(cx, cy, cx, cy));

	q0Im.copyTo(tmp);
	q3Im.copyTo(q0Im);
	tmp.copyTo(q3Im);
	q1Im.copyTo(tmp);
	q2Im.copyTo(q1Im);
	tmp.copyTo(q2Im);

	Mat centerComplex;
	merge(planes, 2, centerComplex);
	return centerComplex;
}

Mat setComplex(Mat mag_img, Mat pha_img) {
	exp(mag_img, mag_img);
	mag_img -= Scalar::all(1);

	Mat planes[2];
	polarToCart(mag_img, pha_img, planes[0], planes[1]);

	Mat complex_img;
	merge(planes, 2, complex_img);

	return complex_img;
}

Mat doIdft(Mat complex_img) {
	Mat idftcvt;
	idft(complex_img, idftcvt);

	Mat planes[2];
	split(idftcvt, planes);

	Mat dst_img;
	magnitude(planes[0], planes[1], dst_img);
	normalize(dst_img, dst_img, 255, 0, NORM_MINMAX);
	dst_img.convertTo(dst_img, CV_8UC1);

	return dst_img;
}

Mat doLPF(Mat src_img) {
	Mat pad_img = padding(src_img);
	Mat complex_img = doDft(pad_img);
	Mat center_complex_img = centralize(complex_img);
	Mat mag_img = getMagnitude(center_complex_img);
	Mat pha_img = getPhase(center_complex_img);

	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(mag_img, &minVal, &maxVal, &minLoc, &maxLoc);
	normalize(mag_img, mag_img, 0, 1, NORM_MINMAX);

	Mat mask_img = Mat::zeros(mag_img.size(), CV_32F);
	circle(mask_img, Point(mask_img.cols / 2, mask_img.rows / 2), 20, Scalar::all(1), -1, -1, 0);

	Mat mag_img2;
	multiply(mag_img, mask_img, mag_img2);

	normalize(mag_img2, mag_img2, (float)minVal, (float)maxVal, NORM_MINMAX);
	Mat complex_img2 = setComplex(mag_img2, pha_img);
	Mat dst_img = doIdft(complex_img2);

	return myNormalize(dst_img);
}

Mat doHPF(Mat src_img) {


	Mat pad_img = padding(src_img);
	Mat complex_img = doDft(pad_img);
	Mat center_complex_img = centralize(complex_img);
	Mat mag_img = getMagnitude(center_complex_img);
	Mat pha_img = getPhase(center_complex_img);

	double minVal, maxVal;
	Point minLoc, maxLoc;

	minMaxLoc(mag_img, &minVal, &maxVal, &minLoc, &maxLoc);
	normalize(mag_img, mag_img, 0, 1, NORM_MINMAX);

	Mat mask_img = Mat::ones(mag_img.size(), CV_32F);
	circle(mask_img, Point(mask_img.cols / 2, mask_img.rows / 2), 50, Scalar::all(0), -1, -1, 0);

	Mat mag_img2;
	multiply(mag_img, mask_img, mag_img2);



	normalize(mag_img2, mag_img2, (float)minVal, (float)maxVal, NORM_MINMAX);
	Mat complex_img2 = setComplex(mag_img2, pha_img);
	Mat dst_img = doIdft(complex_img2);

	return myNormalize(dst_img);
}

Mat doBPF(Mat src_img) {
	Mat pad_img = padding(src_img);
	Mat complex_img = doDft(pad_img);
	Mat center_complex_img = centralize(complex_img);
	Mat mag_img = getMagnitude(center_complex_img);
	Mat pha_img = getPhase(center_complex_img);

	double minVal, maxVal;
	Point minLoc, maxLoc;

	minMaxLoc(mag_img, &minVal, &maxVal, &minLoc, &maxLoc);
	normalize(mag_img, mag_img, 0, 1, NORM_MINMAX);

	Mat mask_img = Mat::zeros(mag_img.size(), CV_32F);
	circle(mask_img, Point(mask_img.cols / 2, mask_img.rows / 2), 50, Scalar::all(1), -1, -1, 0);
	circle(mask_img, Point(mask_img.cols / 2, mask_img.rows / 2), 3, Scalar::all(0), -1, -1, 0);

	Mat mag_img2;
	multiply(mag_img, mask_img, mag_img2);

	normalize(mag_img2, mag_img2, (float)minVal, (float)maxVal, NORM_MINMAX);
	Mat complex_img2 = setComplex(mag_img2, pha_img);
	Mat dst_img = doIdft(complex_img2);

	return myNormalize(dst_img);
}

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

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernerConv3x3(srcData, kernelX, x, y, width, height)) + abs(myKernerConv3x3(srcData, kernelY, x, y, width, height)) / 2);
		}
	}

	return dstImg;
}

int main() {
	/*
	{//HW1 
		Mat src_img = imread("lab4/img1.jpg", 0);
		Mat dst_img = doBPF(src_img);

		imshow("Tests_12204284", dst_img);

		waitKey(0);
		destroyAllWindows();
	}
	//*/

	//*
	{//HW2
		Mat src_img = imread("lab4/img2.jpg", 0);
		Mat sobel_img = imread("lab4/img4.jpg", 0); //sobel filter 영상 input
		sobel_img.convertTo(sobel_img, CV_32F);
		sobel_img = myNormalize(sobel_img);


		//src_img에 sobel filter 적용
		Mat spa_sobel_img = mySobelFilter(src_img);
		imshow("Tests_12204284", spa_sobel_img);
		waitKey(0);


		//sobel filter를 적용한 이미지를 fourier transform
		Mat pad_img = padding(src_img);
		Mat complex_img = doDft(pad_img);
		Mat center_complex_img = centralize(complex_img);
		Mat mag_img = getMagnitude(center_complex_img);
		mag_img = myNormalize(mag_img);

		//곱셈 이용한 sobel filter
		resize(sobel_img, sobel_img, Size(mag_img.cols, mag_img.rows)); //이미지 사이즈 변환
		Mat dst_img;
		multiply(mag_img, sobel_img, dst_img, 1, -1);

		dst_img = myNormalize(dst_img);
		imshow("Tests_12204284", dst_img);
		waitKey(0);
		imshow("Tests_12204284", mag_img);
		waitKey(0);
		imshow("Tests_12204284", sobel_img);
		waitKey(0);
		destroyAllWindows();
	}
	//*/

	/*
	{//HW3
		Mat src_img = imread("lab4/img3.jpg", 0);

		Mat pad_img = padding(src_img);
		Mat complex_img = doDft(pad_img);
		Mat center_complex_img = centralize(complex_img);
		Mat mag_img = getMagnitude(center_complex_img);
		Mat pha_img = getPhase(center_complex_img);

		double minVal, maxVal;
		Point minLoc, maxLoc;

		minMaxLoc(mag_img, &minVal, &maxVal, &minLoc, &maxLoc);
		normalize(mag_img, mag_img, 0, 1, NORM_MINMAX);

		Mat mask_img = Mat::ones(mag_img.size(), CV_32F);
		rectangle(mask_img, Rect(Point(250, 330), Point(400, 420)), Scalar(0, 0, 255), -1);
		rectangle(mask_img, Rect(Point(600, 330), Point(750, 420)), Scalar(0, 0, 255), -1);
		rectangle(mask_img, Rect(Point(480, 330), Point(520, 370)), Scalar(0, 0, 255), -1);
		rectangle(mask_img, Rect(Point(480, 380), Point(520, 420)), Scalar(0, 0, 255), -1);

		Mat mag_img2;
		multiply(mag_img, mask_img, mag_img2);

		normalize(mag_img2, mag_img2, (float)minVal, (float)maxVal, NORM_MINMAX);
		Mat complex_img2 = setComplex(mag_img2, pha_img);
		Mat dst_img = doIdft(complex_img2);

		Mat res_img = myNormalize(dst_img);

		imshow("Tests_12204284", res_img);
		waitKey(0);
		destroyAllWindows();
	}
	//*/
}