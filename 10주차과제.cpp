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

Mat cvHarrisCorner(Mat &img) {
	resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Mat harr;
	cornerHarris(gray, harr, 4, 3, 0.03, 4);
	normalize(harr, harr, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

	Mat harr_abs;
	convertScaleAbs(harr, harr_abs);

	int thresh = 205;
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

Mat cvFeaturesSIFT(Mat& img) {
	//Mat img = imread("lab8/church.jpg", 1);
	resize(img, img, Size(512, 512), 0, 0, INTER_CUBIC);

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint> keypoints;
	detector->detect(gray, keypoints);

	cout << keypoints.size() << endl;

	Mat result;
	drawKeypoints(img, keypoints, result);
	return result;
	//imshow("sift result", result);
	//waitKey(0);
	//cvDestroyAllWindows();
}

Mat warpPers(Mat& src) {
	resize(src, src, Size(512, 512), 0, 0, INTER_CUBIC);
	Mat dst;

	Point2f src_p[4], dst_p[4];

	src_p[0] = Point2f(0, 0);
	src_p[1] = Point2f(512, 0);
	src_p[2] = Point2f(0, 512);
	src_p[3] = Point2f(512, 512);

	dst_p[0] = Point2f(50, 50);
	dst_p[1] = Point2f(450, 50);
	dst_p[2] = Point2f(0, 512);
	dst_p[3] = Point2f(512, 512);

	Mat pers_mat = getPerspectiveTransform(src_p, dst_p);
	warpPerspective(src, dst, pers_mat, Size(512, 512));
	return dst;
}

Mat cvBlobDetection(Mat& img) {
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


	return result;
}

void hw1() {
	Mat img = imread("lab8/coin.png", IMREAD_COLOR);
	Mat rst = cvBlobDetection(img);
	imshow("keypoints", rst);
	waitKey(0);
	cvDestroyAllWindows();

}

void hw2() {
	Mat tri_img = imread("lab8/triangle.jpg");
	Mat squ_img = imread("lab8/square.jpg");
	Mat pen_img = imread("lab8/pentagon.jpg");
	Mat hex_img = imread("lab8/hexagon.jpg");

	cout << "triangle corner = ";
	Mat tri_dst = cvHarrisCorner(tri_img);
	cout << "square corner = ";
	Mat squ_dst = cvHarrisCorner(squ_img);
	cout << "pentagon corner = ";
	Mat pen_dst = cvHarrisCorner(pen_img);
	cout << "hexagon corner = ";
	Mat hex_dst = cvHarrisCorner(hex_img);

	imshow("tri_dst", tri_dst);
	imshow("squ_dst", squ_dst);
	imshow("pen_dst", pen_dst);
	imshow("hex_dst", hex_dst);
	waitKey(0);
	cvDestroyAllWindows();
}

void hw3() {
	Mat src = imread("lab8/church.jpg", 1);
	resize(src, src, Size(512, 512), 0, 0, INTER_CUBIC);

	cout << "SIFT original img = ";
	Mat original_SIFT_result = cvFeaturesSIFT(src);

	Mat brig_dst;
	add(src, Scalar(100,100,100), brig_dst);

	Mat dst = warpPers(brig_dst); //투시변환

	cout << "SIFT trans img = ";
	Mat trans_SIFT_result = cvFeaturesSIFT(dst);

	imshow("hw3 original_SIFT_result", original_SIFT_result);
	imshow("hw3 trans_SIFT_result", trans_SIFT_result);
	waitKey();
}



int main(){
	hw2();
}