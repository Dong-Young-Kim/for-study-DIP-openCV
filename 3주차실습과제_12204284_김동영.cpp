#include <iostream>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;




Mat GetHistogram(Mat src) {
	Mat histogram;
	const int* channel_numbers = { 0 };
	float channel_range[] = { 0.0, 255.0 };
	const float* channel_ranges = channel_range;
	int number_bins = 255;

	calcHist(&src, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

	int hist_w = 512;
	int hist_h = 400;
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


void SpreadRGB(Mat& img, int num_R, int num_G, int num_B) {
	//빨간 점 무작위 생성
	for (int n = 0; n < num_R; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1)  //이미지가 단일 채녈인 경우
			img.at<uchar>(y, x) = 255;
		else {
			img.at<Vec3b>(y, x)[0] = 0;
			img.at<Vec3b>(y, x)[1] = 0;
			img.at<Vec3b>(y, x)[2] = 255;
		}
	}

	//초록 점 무작위 생성
	for (int n = 0; n < num_G; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1)  //이미지가 단일 채녈인 경우
			img.at<uchar>(y, x) = 255;
		else {
			img.at<Vec3b>(y, x)[0] = 0;
			img.at<Vec3b>(y, x)[1] = 255;
			img.at<Vec3b>(y, x)[2] = 0;
		}
	}

	//파랑 점 생성
	for (int n = 0; n < num_B; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1)  //이미지가 단일 채녈인 경우
			img.at<uchar>(y, x) = 255;
		else {
			img.at<Vec3b>(y, x)[0] = 255;
			img.at<Vec3b>(y, x)[1] = 0;
			img.at<Vec3b>(y, x)[2] = 0;
		}
	}
}


void countRGB(const Mat& img, int& count_R, int& count_G, int& count_B) {
	count_R = 0; count_G = 0; count_B = 0;
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (img.at<Vec3b>(y, x)[0] == 0
				&& img.at<Vec3b>(y, x)[1] == 0
				&& img.at<Vec3b>(y, x)[2] == 255)
				count_R++;
			else if (img.at<Vec3b>(y, x)[0] == 0
				&& img.at<Vec3b>(y, x)[1] == 255
				&& img.at<Vec3b>(y, x)[2] == 0)
				count_G++;
			else if (img.at<Vec3b>(y, x)[0] == 255
				&& img.at<Vec3b>(y, x)[1] == 0
				&& img.at<Vec3b>(y, x)[2] == 0)
				count_B++;
		}
	}
}

void hw1(){
	Mat src_img = imread("image/img1.jpg", 1);

	SpreadRGB(src_img, 180, 150, 100);

	int count_R, count_G, count_B;
	countRGB(src_img, count_R, count_G, count_B);
	cout << "R: " << count_R << ", G: " << count_G << ", B: " << count_B << endl;

	imshow("Test window", src_img);
	waitKey(0);
	destroyWindow("Test window");
}

void hw2() {
	Mat img_1 = imread("image/img2.jpg", 0);
	Mat img_2 = imread("image/img2.jpg", 0);

	for (int y = 0; y < img_1.rows; y++) {
		int addValue = 255 - (255 * y / img_1.rows);
		for (int x = 0; x < img_1.cols; x++) {
			img_1.at<uchar>(y, x) = std::max(img_2.at<uchar>(y, x) - addValue, 0);
		}
	}

	for (int y = 0; y < img_2.rows; y++) {
		int addValue = 255 * y / img_2.rows;
		for (int x = 0; x < img_2.cols; x++) {
			img_2.at<uchar>(y, x) = std::max(img_2.at<uchar>(y, x) - addValue, 0);
		}
	}

	imshow("위로 갈수록 점점 어두움", img_1);
	imshow("아래로 갈수록 점점 어두움", img_2);
	imshow("위로 갈수록 점점 어두움_히스토그램", GetHistogram(img_1));
	imshow("아래로 갈수록 점점 어두움_히스토그램", GetHistogram(img_2));
	imshow("원본 히스토그램", GetHistogram(imread("image/img2.jpg", 0)));

	waitKey(0);
	destroyAllWindows();
}


void hw3() {
	Mat imgA = imread("image/img3.jpg", 1);
	Mat imgB = imread("image/img4.jpg", 1);
	Mat logo = imread("image/img5.jpg", 1); // 로고 영상 읽기
	Mat logo_gray = imread("image/img5.jpg", 0); // 마스크로 사용위해 흑백영상으로 읽기
	resize(imgB, imgB, Size(imgA.cols, imgA.rows)); //이미지 사이즈 변환
	Mat dst;

	subtract(imgA, imgB, dst); //비네팅 완료된 영상

	Mat ROIdst(dst, Rect(dst.cols/2 - logo.cols/2, dst.rows/2 - logo.rows/2 + 60, logo.cols, logo.rows));
	Mat mask(180 - logo_gray);
	logo.copyTo(ROIdst, mask);
	
	imshow("Tests_12204284", dst);

	waitKey(0);
	destroyAllWindows();

}


int main() {

	hw3();

	return 0;
}

//Mat src_img = imread("image/landing.jpg", 0);		// 이미지 읽기
//// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
//// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
//// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음



//src_img = GetHistogram(src_img);

//imshow("Test window2", src_img);				// 이미지 출력
//waitKey(0);									// 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
//destroyWindow("Test window2");				// 이미지 출력창 종료
////imwrite("image/langding_gray.png", src_img);		// 이미지 쓰기


