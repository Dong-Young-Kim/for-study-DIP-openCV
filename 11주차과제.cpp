#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"

using namespace cv;
using namespace std;

Mat cvRotation(Mat &src) {	
	Mat dst, matrix;

	Point center = Point(src.cols / 2, src.rows / 2);
	matrix = getRotationMatrix2D(center, 45.0, 1.0);
	warpAffine(src, dst, matrix, src.size());

	return dst;
}

Mat myRotationMatrix(double theta, Mat& src) {
	
	Mat matrix1 = (Mat_<double>(3, 3) <<
		1,0,src.cols/2,
		0, 1, src.rows / 2,
		0,0,1);
	Mat matrix2 = (Mat_<double>(3, 3) <<
		 cos(theta * CV_PI / 180), sin(theta * CV_PI / 180), 0,
		-sin(theta * CV_PI / 180), cos(theta * CV_PI / 180), 0,
		 0,						   0,						 1);
	Mat matrix3 = (Mat_<double>(3, 3) <<
		1, 0, -src.cols / 2,
		0, 1, -src.rows / 2,
		0, 0, 1);
	return matrix1 * matrix2 * matrix3;
}

Mat myRotation(Mat& src) {
	Mat dst, matrix;
	double theta = 45;
	matrix = myRotationMatrix(theta, src);
	warpPerspective(src, dst, matrix, src.size());

	return dst;
}

Mat cvPerspective(Mat& src) {
	Mat dst, matrix;

	Point2f src_p[4];
	src_p[0] = Point2f(126, 134);
	src_p[1] = Point2f(374, 179);
	src_p[2] = Point2f(73, 351);
	src_p[3] = Point2f(425, 320);

	Point2f dst_p[4];
	dst_p[0] = Point2f(20, 110);
	dst_p[1] = Point2f(480, 110);
	dst_p[2] = Point2f(20, 390);
	dst_p[3] = Point2f(480, 390);

	matrix = getPerspectiveTransform(src_p, dst_p);

	warpPerspective(src, dst, matrix, src.size());
	
	return dst;


}

Mat cvHarrisCorner(Mat& img) {
	resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Mat harr;
	cornerHarris(gray, harr, 4, 5, 0.02, 4);
	normalize(harr, harr, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

	Mat harr_abs;
	convertScaleAbs(harr, harr_abs);

	int thresh = 125;
	Mat result = img.clone();
	int i = 0;
	for (int y = 0; y < harr.rows; y++) {
		for (int x = 0; x < harr.cols; x++) {
			if ((int)harr.at<float>(y, x) > thresh) {
				circle(result, Point(x, y), 7, Scalar(255, 0, 255), 0, 4, 0);
				i++;
			}
		}
	}
	cout << i << endl;
	return result;
}

void hw1() {
	Mat src = imread("lab9/Lenna.png", 1);

	Mat cv_dst = cvRotation(src);
	Mat my_dst = myRotation(src);

	imshow("nonrot", src);
	imshow("cv_rot", cv_dst);
	imshow("my_rot", my_dst);
	//waitKey(0);
	//destroyAllWindows();
}

void hw2() {
	Mat src = imread("lab9/card_per.png", 1);

	Mat harr_dst = cvHarrisCorner(src);

	Mat dst = cvPerspective(src);

	imshow("nonper", src);
	imshow("per", harr_dst);
	waitKey();
	destroyAllWindows();
}

int main() {
	hw1();
	hw2();
}