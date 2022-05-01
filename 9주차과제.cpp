#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;




void grabCut() {
	Mat img = imread("lab7/fruit.jpg");

	resize(img, img, Size(256, 256), 0, 0, CV_INTER_AREA);
	imshow("src", img);
	if (img.empty()) exit(-1);


	Rect rect = Rect(Point(0, 0), Point(200, 200));

	Mat result, bg_model, fg_model;

	grabCut(img, result,
		rect,
		bg_model, fg_model,
		5,
		GC_INIT_WITH_RECT);

	compare(result, GC_PR_FGD, result, CMP_EQ);

	Mat mask(img.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	img.copyTo(mask, result);

	imshow("mask", result);
	imshow("dst", mask);
	waitKey();
	destroyAllWindows();
}

int main() {
	grabCut();
	return 0;
}