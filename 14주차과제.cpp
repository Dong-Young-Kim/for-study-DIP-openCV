#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;


Mat GetHist(Mat src) {
	Mat histogram;
	const int* channel_numbers = { 0 };
	float channel_range[] = { 0.0, 255.0 };
	const float* channel_ranges = channel_range;
	int number_bins = 255;

	cvtColor(src, src, CV_RGB2GRAY); //grey scale로 변환
	calcHist(&src, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

	int hist_w = 400;
	int hist_h = 250;
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


//void readImagesAndTimes(vector<Mat>& images, vector<float>& times) {
//	int numImages = 3;
//	static const float timesArray[] = { 0.01f, 0.04f, 0.32f};
//	times.assign(timesArray, timesArray + numImages);
//	static const char* filenames[] = { "lab12/img_60.jpg","lab12/img_0.200.jpg", "lab12/img_800.jpg" };
//	for (int i = 0; i < numImages; i++) {
//		Mat im = imread(filenames[i]);
//		images.push_back(im);
//	}
//}

void readImagesAndTimes(vector<Mat>& images, vector<float>& times) {
	int numImages = 4;
	static const float timesArray[] = { 0.01f, 0.06f, 0.12f, 1.00f };
	times.assign(timesArray, timesArray + numImages);
	static const char* filenames[] = { "lab12/img_001.jpg","lab12/img_006.jpg", "lab12/img_012.jpg", "lab12/img_100.jpg" };
	for (int i = 0; i < numImages; i++) {
		Mat im = imread(filenames[i]);
		images.push_back(im);
	}
}


void HDRIMG() {
	cout << "Reading images and exposure times ... ";
	vector<Mat> images;
	vector<float> times;

	readImagesAndTimes(images, times);
	cout << "finished" << endl;

	cout << "aligning images ... ";
	Ptr<AlignMTB> alignMTB = createAlignMTB();
	alignMTB->process(images, images);
	cout << "finished" << endl;


	cout << "calculating camera response function ... ";
	Mat responseDebevec;
	Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();
	calibrateDebevec->process(images, responseDebevec, times);
	cout << "finished" << endl;
	cout << "-----------CRF-------------" << endl;
	cout << responseDebevec << endl;


	cout << "merging images into one HDR image ... " << endl;
	Mat hdrDebevec;
	Ptr<MergeDebevec> mergeDebevec = createMergeDebevec();
	mergeDebevec->process(images, hdrDebevec, times, responseDebevec);
	//imwrite("lab12/hdrDebevec.hdr", hdrDebevec);
	//imshow("lab12/hdrDebevec.hdr", hdrDebevec);


	cout << "tonemapping using Drago's method ...  " << endl;
	Mat IdrDrago;
	Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.3f, 1.25f, 0.85f);
	tonemapDrago->process(hdrDebevec, IdrDrago);
	IdrDrago = 3 * IdrDrago;
	imwrite("lab12/Idr-Drago3.jpg", IdrDrago * 255);
	imshow("histDrago2", GetHist(IdrDrago * 255));


	cout << "tonemapping using Reinhard's method ...  " << endl;
	Mat IdrReinhard;
	Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(2.0f, 0, 0, 0);
	tonemapReinhard->process(hdrDebevec, IdrReinhard);
	imwrite("lab12/Idr-Reinhard3.jpg", IdrReinhard * 255);
	imshow("histReinhard2", GetHist(IdrReinhard * 255));

	cout << "tonemapping using Mantiuk's method ...  " << endl;
	Mat IdrMantiuk;
	Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(1.5f, 0.8f, 1.5f);
	tonemapMantiuk->process(hdrDebevec, IdrMantiuk);
	IdrMantiuk = 3 * IdrMantiuk;
	imwrite("lab12/Idr-Mantiuk3.jpg", IdrMantiuk * 255);
	imshow("histMantiuk", GetHist(IdrMantiuk * 255));

	//waitKey();


}

int main() {
	imshow("hist001", GetHist(imread("lab12/img_001.jpg", IMREAD_COLOR)));
	imshow("hist006", GetHist(imread("lab12/img_006.jpg", IMREAD_COLOR)));
	imshow("hist012", GetHist(imread("lab12/img_012.jpg", IMREAD_COLOR)));
	imshow("hist100", GetHist(imread("lab12/img_100.jpg", IMREAD_COLOR)));

	HDRIMG();

	waitKey();
}