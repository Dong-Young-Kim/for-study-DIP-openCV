#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp"		// GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp"		// ���� �̹��� ó�� �Լ��� �����ϴ� ���
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/stitching.hpp" //��Ƽó �̿� ���ؼ� ����

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

Mat makePanorama(Mat& img_l, Mat& img_r, int thresh_dist, int min_matches) {

	Mat img_gray_l, img_gray_r;
	cvtColor(img_l, img_gray_l, CV_BGR2GRAY);
	cvtColor(img_r, img_gray_r, CV_BGR2GRAY);

	Ptr<SurfFeatureDetector> Detector = SURF::create(300);
	vector<KeyPoint> kpts_obj, kpts_scene;
	Detector->detect(img_gray_l, kpts_obj);
	Detector->detect(img_gray_r, kpts_scene);

	Mat img_kpts_l, img_kpts_r;
	drawKeypoints(img_gray_l, kpts_obj, img_kpts_l, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img_gray_r, kpts_scene, img_kpts_r, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("img_kpts_l", img_kpts_l);
	imshow("img_kpts_r", img_kpts_r);


	Ptr<SurfDescriptorExtractor> Extractor = SURF::create(100, 4, 3, false, true);

	Mat img_des_obj, img_des_scene;
	Extractor->compute(img_gray_l, kpts_obj, img_des_obj);
	Extractor->compute(img_gray_r, kpts_scene, img_des_scene);

	BFMatcher matcher(NORM_L2);
	vector<DMatch> matches;
	matcher.match(img_des_obj, img_des_scene, matches);

	Mat img_matches;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("img_matches", img_matches);

	double dist_min = matches[0].distance;
	double dist_max = matches[0].distance;
	double dist;
	for (int i = 0; i < img_des_obj.rows; i++) {
		dist = matches[i].distance;
		if (dist < dist_min)dist_min = dist;
		if (dist > dist_min)dist_max = dist;
	}
	printf("max_dist : %f \n", dist_max);
	printf("min_dist : %f \n", dist_min);

	vector<DMatch> matches_good;
	do {
		vector<DMatch> good_matches2;
		for (int i = 0; i < img_des_obj.rows; i++) {
			if (matches[i].distance < thresh_dist * dist_min)
				good_matches2.push_back(matches[i]);
		}
		matches_good = good_matches2;
		thresh_dist -= 1;
	} while (thresh_dist != 2 && matches_good.size() > min_matches);

	Mat img_matches_good;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("img_matches_good", img_matches_good);




	vector<Point2f>obj, scene;
	for (int i = 0; i < matches_good.size(); i++) {
		obj.push_back(kpts_obj[matches_good[i].queryIdx].pt);
		scene.push_back(kpts_scene[matches_good[i].trainIdx].pt);
	}

	Mat mat_homo = findHomography(scene, obj, RANSAC);


	Mat img_result;
	warpPerspective(img_r, img_result, mat_homo,
		Size(img_l.cols * 2, img_l.rows * 1.2), INTER_CUBIC);

	Mat img_pano = img_result.clone();

	Mat roi(img_pano, Rect(0, 0, img_l.cols, img_l.rows));
	img_l.copyTo(roi);

	int cut_x = 0, cut_y = 0;
	for (int y = 0; y < img_pano.rows; y++) {
		for (int x = 0; x < img_pano.cols; x++) {
			if (img_pano.at<Vec3b>(y, x)[0] == 0 &&
				img_pano.at<Vec3b>(y, x)[1] == 0 &&
				img_pano.at<Vec3b>(y, x)[2] == 0) {
				continue;
			}
			if (cut_x < x) cut_x = x;
			if (cut_y < y) cut_y = y;
		}
	}
	Mat img_pano_cut = img_pano(Range(0, cut_y), Range(0, cut_x));
	//imshow("img_pano_cut", img_pano_cut);
	return img_pano_cut;
}

Mat findBook(Mat& backGnd, Mat& obj_book, int thresh_dist, int min_matches) {

	Mat backGnd_gray, obj_gray;
	cvtColor(backGnd, backGnd_gray, CV_BGR2GRAY);
	cvtColor(obj_book, obj_gray, CV_BGR2GRAY);


	//===================================================================
	//sift�� �̿��� Ư¡�� ����
	Ptr<SiftFeatureDetector> Detector = SiftFeatureDetector::create();
		
	vector<KeyPoint> kpts_backGnd, kpts_obj;
	Detector->detect(backGnd_gray, kpts_backGnd);
	Detector->detect(obj_gray, kpts_obj);

	Mat img_kpts_backGnd, img_kpts_book;
	drawKeypoints(backGnd_gray, kpts_backGnd, img_kpts_backGnd, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(obj_gray, kpts_obj, img_kpts_book, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("img_kpts_l", img_kpts_backGnd);
	imshow("img_kpts_r", img_kpts_book);


	Ptr<SiftDescriptorExtractor> Extractor = SiftDescriptorExtractor::create(100, 4, 3, false, true);

	Mat img_des_obj, img_des_scene;
	Extractor->compute(backGnd_gray, kpts_backGnd, img_des_obj);
	Extractor->compute(obj_gray, kpts_obj, img_des_scene);

	BFMatcher matcher(NORM_L2);
	vector<DMatch> matches;
	matcher.match(img_des_obj, img_des_scene, matches);

	Mat img_matches;
	drawMatches(backGnd_gray, kpts_backGnd, obj_gray, kpts_obj,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("img_matches", img_matches);



	double dist_min = matches[0].distance;
	double dist_max = matches[0].distance;
	double dist;
	for (int i = 0; i < img_des_obj.rows; i++) {
		dist = matches[i].distance;
		if (dist < dist_min)dist_min = dist;
		if (dist > dist_min)dist_max = dist;
	}
	printf("max_dist : %f \n", dist_max);
	printf("min_dist : %f \n", dist_min);

	vector<DMatch> matches_good;
	do {
		vector<DMatch> good_matches2;
		for (int i = 0; i < img_des_obj.rows; i++) {
			if (matches[i].distance < thresh_dist * dist_min)
				good_matches2.push_back(matches[i]);
		}
		matches_good = good_matches2;
		thresh_dist -= 1;
	} while (thresh_dist != 2 && matches_good.size() > min_matches);

	Mat img_matches_good;
	drawMatches(backGnd_gray, kpts_backGnd, obj_gray, kpts_obj,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("img_matches_good", img_matches_good);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




	vector<Point2f>obj, scene;
	for (int i = 0; i < matches_good.size(); i++) {
		obj_book.push_back(kpts_backGnd[matches_good[i].queryIdx].pt);
		scene.push_back(kpts_obj[matches_good[i].trainIdx].pt);
	}

	Mat mat_homo = findHomography(scene, obj_book, RANSAC);










	Mat img_result;
	warpPerspective(obj_book, img_result, mat_homo,
		Size(backGnd.cols * 2, backGnd.rows * 1.2), INTER_CUBIC);

	Mat img_pano = img_result.clone();

	Mat roi(img_pano, Rect(0, 0, backGnd.cols, backGnd.rows));
	backGnd.copyTo(roi);

	int cut_x = 0, cut_y = 0;
	for (int y = 0; y < img_pano.rows; y++) {
		for (int x = 0; x < img_pano.cols; x++) {
			if (img_pano.at<Vec3b>(y, x)[0] == 0 &&
				img_pano.at<Vec3b>(y, x)[1] == 0 &&
				img_pano.at<Vec3b>(y, x)[2] == 0) {
				continue;
			}
			if (cut_x < x) cut_x = x;
			if (cut_y < y) cut_y = y;
		}
	}
	Mat img_pano_cut = img_pano(Range(0, cut_y), Range(0, cut_x));
	//imshow("img_pano_cut", img_pano_cut);
	return img_pano_cut;
}

void hw2() {
	Mat matImg1 = imread("lab10/Scene.jpg", IMREAD_COLOR);
	Mat matImg2 = imread("lab10/Book1.jpg", IMREAD_COLOR);

	Mat res = makePanorama(matImg1, matImg2, 3, 60);
	imshow("panorama_res", res);
	waitKey();

}

void ex_panorama() {
	Mat matImg1 = imread("lab10/myCenter.jpg", IMREAD_COLOR);
	Mat matImg2 = imread("lab10/myLeft.jpg", IMREAD_COLOR);
	Mat matImg3 = imread("lab10/myRight.jpg", IMREAD_COLOR);

	int addBright = 9;
	add(matImg1, Scalar(addBright, addBright, addBright), matImg1);

	Mat res;
	flip(matImg1, matImg1, 1);
	flip(matImg2, matImg2, 1);

	res = makePanorama(matImg1, matImg2, 3, 60);

	flip(res, res, 1);
	res = makePanorama(res, matImg3, 3, 60);

	imshow("panorama_res", res);
	waitKey();

}

void ex_panorama_simple() {
	Mat img;
	vector<Mat>imgs;
	img = imread("lab10/myLeft.jpg", IMREAD_COLOR);
	imgs.push_back(img);
	img = imread("lab10/myCenter.jpg", IMREAD_COLOR);
	imgs.push_back(img);
	img = imread("lab10/myRight.jpg", IMREAD_COLOR);
	imgs.push_back(img);

	Mat res;
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);
	Stitcher::Status status = stitcher->stitch(imgs, res);
	if (status != Stitcher::OK) {
		cout << "Can't stitch images, error code = " << int(status) << endl;
		exit(-1);
	}
	imshow("res", res);
	waitKey();
}

int main() {
	ex_panorama_simple();
}
