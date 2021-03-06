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
	resize(img, img, Size(512, 512), 0, 0, INTER_CUBIC);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint> keypoints;
	detector->detect(gray, keypoints);

	cout << "normal SIFT kp num = " << keypoints.size() << endl;

	Mat result;
	drawKeypoints(img, keypoints, result);
	imshow("sift result", result);
	//waitKey(0);
	//cvDestroyAllWindows();
}

void cvBlobDetection() {
	Mat img = imread("lab8/coin.png", IMREAD_COLOR);
	//Mat img = imread("lab8/circle.jpg", IMREAD_COLOR);

	SimpleBlobDetector::Params params;
	params.minThreshold = 5;
	params.maxThreshold = 500;
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

void warpPers() {
	Mat src = imread("lab8/church.jpg", 1);
	resize(src, src, Size(512, 512), 0, 0, INTER_CUBIC);
	Mat dst;

	Point2f src_p[4], dst_p[4];

	src_p[0] = Point2f(0, 0);
	src_p[1] = Point2f(512, 0);
	src_p[2] = Point2f(0, 512);
	src_p[3] = Point2f(512, 512);

	dst_p[0] = Point2f(0, 0);
	dst_p[1] = Point2f(512, 0);
	dst_p[2] = Point2f(0, 512);
	dst_p[3] = Point2f(412, 412);

	Mat pers_mat = getPerspectiveTransform(src_p, dst_p);
	warpPerspective(src, dst, pers_mat, Size(512, 512));
	imshow("result", dst);
	waitKey();
}

void hw3() {
	Mat src = imread("lab8/church.jpg", 1);
	resize(src, src, Size(512, 512), 0, 0, INTER_CUBIC);

	Mat brig_dst;
	add(src, Scalar(100), brig_dst);

	Mat dst;

	Point2f src_p[4], dst_p[4];

	src_p[0] = Point2f(0, 0);
	src_p[1] = Point2f(512, 0);
	src_p[2] = Point2f(0, 512);
	src_p[3] = Point2f(512, 512);

	dst_p[0] = Point2f(70, 70);
	dst_p[1] = Point2f(512, 0);
	dst_p[2] = Point2f(0, 512);
	dst_p[3] = Point2f(480, 480);

	Mat pers_mat = getPerspectiveTransform(src_p, dst_p);
	warpPerspective(src, brig_dst, pers_mat, Size(512, 512));


	Mat gray;
	cvtColor(dst, gray, CV_BGR2GRAY);

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint> keypoints;
	detector->detect(gray, keypoints);

	cout << "changed SIFT kp num = " << keypoints.size() << endl;

	Mat result;
	drawKeypoints(dst, keypoints, result);
	imshow("hw3 result", result);
	waitKey();
}

int main() {
	//cvFeaturesSIFT();
	//hw3();
}