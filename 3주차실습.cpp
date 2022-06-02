#include <iostream>
#include "opencv2/core/core.hpp"			// Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp"		// GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp"		// ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;


void SpreadSalts(Mat& img, int num) {
	for (int n = 0; n < num; n++) {
		int x = rand() % img.cols;
		int y = rand() % img.rows;

		if (img.channels() == 1) img.at<uchar>(y, x) = 255;
		else {
			img.at<Vec3b>(y, x)[0] = 255;
			img.at<Vec3b>(y, x)[1] = 255;
			img.at<Vec3b>(y, x)[2] = 255;
		}
	}
}

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


void ex4() {
	Mat imgA = imread("img3.jpg", 1);
	Mat imgB = imread("img4.jpg", 1);
	resize(imgB, imgB, Size(imgA.cols, imgA.rows));
	Mat dst1, dst2, dst3, dst4;
	add(imgA, imgB, dst1);
	// dst1 = imgA + imgB;�� ������
	add(imgA, Scalar(100, 100, 100), dst2);
	// dst2 = imgA + Scalar(100, 100, 100);�� ������
	subtract(imgA, imgB, dst3);
	subtract(imgA, Scalar(100, 100, 100), dst4);
	imshow("Test2", dst2);
	Mat img1(520, 480, CV_8UC3, Scalar(100, 100, 100));
	imshow("Tests", img1);
	waitKey(0);
	destroyAllWindows();
}

void ex5() {
	Mat image = imread("landing.jpg", 1);
	Mat black(image.size(), CV_8UC3, Scalar(0)); // ������ ũ���� ���� ���� ����
	Mat mask(image.size(), CV_8U, Scalar(0)); // ������ ũ���� ����ũ ����
	circle(mask, Point(500, 250), 200, Scalar(255), -1, -1);
	// ����ũ�� �� ���� Mat dst1, dst2;
	Mat dst1, dst2;
	bitwise_and(image, image, dst1, mask);
	// ����ũ �������� image�� image�� AND����
	bitwise_or(black, image, dst2, mask);
	// ����ũ �������� ���� ����� image�� OR����
	imshow("Test2", dst1);
	imshow("Test1", dst2);
	waitKey(0);
	destroyAllWindows();
}


void ex6() {
	Mat color_img = imread("img3.jpg", 1);

	Mat gray_img;
	cvtColor(color_img, gray_img, CV_BGR2GRAY);

	Mat binary_img1, binary_img2;
	threshold(gray_img, binary_img1, 127, 255, THRESH_BINARY);
	threshold(gray_img, binary_img2, 0, 255, THRESH_OTSU);

	imshow("Test1", binary_img1);
	imshow("Test2", color_img);
	waitKey(0);
	destroyAllWindows();
}


int main() {
	Mat src_img = imread("landing.jpg", 0);		// �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����



	src_img = GetHistogram(src_img);

	imshow("Test window2", src_img);				// �̹��� ���
	waitKey(0);									// Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window2");				// �̹��� ���â ����
	//imwrite("langding_gray.png", src_img);		// �̹��� ����


	

	return 0;
}


