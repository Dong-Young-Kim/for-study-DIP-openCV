#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

double gaussian(float x, double sigma) {
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}

double gaussian2D(float c, float r, double sigma) {
	return exp(-(pow(c, 2) + pow(r, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}

float distance(int x, int y, int i, int j) {
	return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

void myKernelConv(const Mat& src_img, Mat& dst_img, const Mat& kn) {

	dst_img = Mat::zeros(src_img.size(), CV_8UC1);

	int wd = src_img.cols; int hg = src_img.rows;
	int kwd = kn.cols; int khg = kn.rows;
	int rad_w = kwd / 2; int rad_h = khg / 2;

	float* kn_data = (float*)kn.data;
	uchar* src_data = (uchar*)src_img.data;
	uchar* dst_data = (uchar*)dst_img.data;

	float wei, tmp, sum;

	for (int c = rad_w + 1; c < wd - rad_w; c++) {
		for (int r = rad_h + 1; r < hg - rad_h; r++) {
			tmp = 0.f;
			sum = 0.f;
			for (int kc = -rad_w; kc <= rad_w; kc++) {
				for (int kr = -rad_h; kr <= rad_h; kr++) {
					wei = (float)kn_data[(kr + rad_h) * kwd + (kc + rad_w)];
					tmp += wei * (float)src_data[(r + kr) * wd + (c + kc)];
					sum += wei;
				}
			}
			if (sum != 0.f) tmp = abs(tmp) / sum;
			else tmp = abs(tmp);
			if (tmp > 255.f) tmp = 255.f;
			dst_data[r * wd + c] = (uchar)tmp;
		}
	}
}

void myGaussian(const Mat& src_img, Mat& dst_img, Size size, double sigma) {
	//커널생성
	Mat kn = Mat::zeros(size, CV_32FC1);
	float* kn_data = (float*)kn.data;
	for (int c = 0; c < kn.cols; c++) {
		for (int r = 0; r < kn.rows; r++) {
			kn_data[r * kn.cols + c] = (float)gaussian2D((float)(c - kn.cols / 2),
				(float)(r - kn.rows / 2), sigma);
		}
	}

	myKernelConv(src_img, dst_img, kn);
}

void myMedian(const Mat& src_img, Mat& dst_img, Size kn_size) {
	dst_img = Mat::zeros(src_img.size(), CV_8UC1);

	int wd = src_img.cols; int hg = src_img.rows;
	int kwd = kn_size.width; int khg = kn_size.height;
	int rad_w = kwd / 2; int rad_h = khg / 2;

	uchar* src_data = (uchar*)src_img.data;
	uchar* dst_data = (uchar*)dst_img.data;

	uchar* table = new uchar[kwd * khg]();

	for (int c = rad_w + 1; c < wd - rad_w; c++) {
		for (int r = rad_h + 1; r < hg - rad_h; r++) {
			int it = 0;
			for (int kc = -rad_w; kc <= rad_w; kc++) {
				for (int kr = -rad_h; kr <= rad_h; kr++) {
					table[it] = src_data[(r + kr) * wd + (c + kc)];
					it++;
				}
			}
			sort(table, table + it); //sorting
			dst_data[r * wd + c] = table[kwd * khg / 2];
		}
	}
	delete[] table;
}

void bilateral(const Mat& src_img, Mat& dst_img,
	int c, int r, int diameter, double sig_r, double sig_s){
	int radius = diameter / 2;
	double gr, gs, wei;
	double tmp = 0.;
	double sum = 0.;

	for (int kc = -radius; kc <= radius; kc++) {
		for (int kr = -radius; kr <= radius; kr++) {
			gr = gaussian((float)src_img.at<uchar>(c + kc, r + kr)
				- (float)src_img.at<uchar>(c, r), sig_r);
			//range calc
				gs = gaussian(distance(c, r, c + kc, r + kr), sig_s);
			//spatial calc
				wei = gr * gs;
				tmp += src_img.at<uchar>(c + kc, r + kr) * wei;
				sum += wei;			
		}
	}
	dst_img.at <double>(c, r) = tmp / sum; // 정규화
}

void myBilateral(const Mat& src_img, Mat& dst_img, int diameter, double sig_r, double sig_s) {

	dst_img = Mat::zeros(src_img.size(), CV_8UC1);

	Mat guide_img = Mat::zeros(src_img.size(), CV_64F);
	int wh = src_img.cols; int hg = src_img.rows;
	int radius = diameter / 2;

	//픽셀 인덱싱(가장자리 제외)
	for (int c = radius + 1; c < hg - radius; c++) {
		for (int r = radius + 1; r < wh - radius; r++) {
			bilateral(src_img, guide_img, c, r, diameter, sig_r, sig_s);
		}
	}
	guide_img.convertTo(dst_img, CV_8UC1);

}
void doBilateralEx() {
	cout << "---doBilateralEx()---" << endl;
	Mat src_img = imread("rock.png", 0);
	Mat dst_img;
	if (!src_img.data)printf("no image data\n");

	myBilateral(src_img, dst_img, 5, 25.0, 50.0);

	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doBilateralEx()", result_img);
	waitKey(0);
}

void hw1() {
	Mat src_img = imread("lab5/salt_pepper2.png", 0);
	Mat med_img;

	myMedian(src_img, med_img, Size(5, 5));

	Mat res_img;
	hconcat(src_img, med_img, res_img);
	imshow("12204284 median image", res_img);

	waitKey(0);
	destroyAllWindows;
}

void hw2() {
	Mat src_img = imread("lab5/edge_test.jpg", 0);

	clock_t start, end;

	Mat edge_img;
	for (int high = 50; high <= 350; high += 100)
		for (int low = 20; low <= 220; low += 100) {
			start = clock();
			Canny(src_img, edge_img, low, high); //openCV의 Canny 함수 사용
			end = clock();
			cout << "실행시간 (thres_high : " << high << ", thres_low : " << low << ") -> " << end - start << "ms" << endl;
			imshow("img_edge", edge_img);
			waitKey(0);
		}
	destroyAllWindows;
}

void hw3() {
	Mat src_img = imread("lab5/rock.png", 0);

	Mat gau_img;

	double sigma_r[] = { 10, 25, 9999 };
	double sigma_s[] = { 6, 18, 50 };

	for (int s = 0; s < 3; s++) 
		for (int r = 0; r < 3; r++){
			myBilateral(src_img, gau_img, 15, sigma_r[r], sigma_s[s]);
			imshow("img_bil", gau_img);
			waitKey(0);
		}
	destroyAllWindows;
}

int main() {

	hw3();

	return 0;
}