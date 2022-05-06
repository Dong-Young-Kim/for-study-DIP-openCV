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

void cvHarrisCorner() {
	Mat img = imread("lab8/ship.png");
	if (img.empty()) {
		cout << "empty image!\n";
		exit(-1);
	}
	resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Mat harr;
	cornerHarris(gray, harr, 2, 3, 0.05, BORDER_DEFAULT);
	normalize(harr, harr, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

	Mat harr_abs;
	convertScaleAbs(harr, harr_abs);

	int thresh = 125;
	Mat result = img.clone();
	for (int y = 0; y < harr.rows; y++) {
		for (int x = 0; x < harr.cols; x++) {
			if ((int)harr.at<float>(y, x) > thresh)
				circle(result, Point(x, y), 7, Scalar(255, 0, 255), 0, 4, 0);
		}
	}
	
	imshow("source im", img);
	imshow("Harris im", harr_abs);
	imshow("Target im", result);
	waitKey(0);
	cvDestroyAllWindows();

}

void cvFeaturesSIFT() {
	Mat img = imread("lab8/church.jpg", 1);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint> keypoints;
	detector->detect(gray, keypoints);

	Mat result;
	drawKeypoints(img, keypoints, result);
	imshow("sift result", result);
	waitKey(0);
	cvDestroyAllWindows();
}

void cvBlobDetection() {
	Mat img = imread("lab8/coin.png", IMREAD_COLOR);
	//Mat img = imread("lab8/circle.jpg", IMREAD_COLOR);

	SimpleBlobDetector::Params params;
	params.minThreshold = 5;
	params.maxThreshold = 900;
	params.filterByArea = true;
	params.minArea = 120;
	params.maxArea = 9000;
	params.filterByCircularity = false;
	params.minCircularity = 0.895;
	params.filterByConvexity = true;
	params.minConvexity = 0.9;
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> ketpoints;
	detector->detect(img, ketpoints);

	cout << ketpoints.size() << endl;

	Mat result;
	drawKeypoints(img, ketpoints, result,
		Scalar(255, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	imshow("keypoints", result);
	waitKey(0);
	cvDestroyAllWindows();
}

int main() {
	cvHarrisCorner();
}