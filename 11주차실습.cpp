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

void cvRotation() {
	Mat src = imread("lab9/Lenna.png", 1);
	Mat dst, matrix;

	Point center = Point(src.cols / 2, src.rows / 2);
	matrix = getRotationMatrix2D(center, 45.0, 1.0);
	warpAffine(src, dst, matrix, src.size());

	imshow("nonrot", src);
	imshow("rot", dst);
	waitKey(0);
	destroyAllWindows();
}

void cvPerspective() {
	Mat src = imread("lab9/Lenna.png", 1);
	//resize(src, src, Size(512, 512), 0, 0, INTER_CUBIC);
	Mat dst, matrix;

	Point2f src_p[4];
	src_p[0] = Point2f(0, 0);
	src_p[1] = Point2f(src.cols-1, 0);
	src_p[2] = Point2f(0, src.rows - 1);
	src_p[3] = Point2f(src.cols - 1, src.rows - 1);

	Point2f dst_p[4];
	dst_p[0] = Point2f(src.cols * 0, src.rows * 0.33);
	dst_p[1] = Point2f(src.cols * 0.85, src.rows * 0.25);
	dst_p[2] = Point2f(src.cols * 0.15, src.rows * 0.7);
	dst_p[3] = Point2f(src.cols * 0.85, src.rows * 0.7);

	matrix = getPerspectiveTransform(src_p, dst_p);
	warpPerspective(src, dst, matrix, src.size());
	imshow("nonper", src);
	imshow("per", dst);
	waitKey();
}

int main() {
	cvPerspective();
}