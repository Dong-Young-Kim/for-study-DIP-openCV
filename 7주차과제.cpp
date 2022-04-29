#include <iostream>
#include <ctime>
#include <algorithm>
#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

Mat GetHSV(Mat src_img) {
	double b, g, r, h, s, v;
	Mat dst_img(src_img.size(), src_img.type());
	for (int row = 0; row < src_img.rows; row++) {
		for (int col = 0; col < src_img.cols; col++) {
			b = (double)src_img.at<Vec3b>(row, col)[0];
			g = (double)src_img.at<Vec3b>(row, col)[1];
			r = (double)src_img.at<Vec3b>(row, col)[2];
			
			double max_val = max({ b, g, r });
			double min_val = min({ b, g, r });
			v = max_val;

			if (v == 0) {
				h = 0;
				s = 0;
			}
			else {
				s = (max_val - min_val) / max_val * 255;
				if (max_val == r) h = 60 * (0 + (g - b) / (max_val - min_val));
				else if (max_val == g) h = 60 * (2 + (b - r) / (max_val - min_val));
				else h = 60 * (0 + (r - g) / (max_val - min_val));
				if (h < 0) h += 360;
				h = h * 255 / 360;
			}

			dst_img.at<Vec3b>(row, col)[0] = (uchar)h;
			dst_img.at<Vec3b>(row, col)[1] = (uchar)s;
			dst_img.at<Vec3b>(row, col)[2] = (uchar)v;
		}
	}
	return dst_img;
}

Mat InRange(Mat src_img) {
	double h, s, v, m;
	Mat dst_img(src_img.size(), src_img.type());

	//색 판별
	h = (double)src_img.at<Vec3b>(src_img.rows / 2, src_img.cols / 2)[0];
	if (h > 320 || h < 35) { cout << "red" << endl; m = 0; }
	else if (h > 36 && h < 60) { cout << "yellow" << endl; m = 60;	}
	else if (h > 61 && h < 160) { cout << "green" << endl; m = 120;	}
	else if (h > 161 && h < 260) { cout << "blue" << endl; m = 240;	}
	else { cout << "purple" << endl; m = 300;	}

	cout << "h = " << h << endl;


	for (int row = 0; row < src_img.rows; row++) {
		for (int col = 0; col < src_img.cols; col++) {
			h = (double)src_img.at<Vec3b>(row, col)[0];
			s = (double)src_img.at<Vec3b>(row, col)[1];
			v = (double)src_img.at<Vec3b>(row, col)[2];

			//h기반 masking
			if (abs(h - m) < 60 || abs(h - m) > 300) {
				h = 255; s = 255; v = 255;
			}
			else {
				h = 0; s = 0; v = 0;
			}

			dst_img.at<Vec3b>(row, col)[0] = (uchar)h;
			dst_img.at<Vec3b>(row, col)[1] = (uchar)s;
			dst_img.at<Vec3b>(row, col)[2] = (uchar)v;
		}
	}
	return dst_img;
}

Mat applyFinalClusterTolmage(Mat src_img, int n_cluster, vector<vector<Point>>& ptInClusters, vector<Scalar> clustersCenters) {
	Mat dst_img(src_img.size(), src_img.type());

	for (int k = 0; k < n_cluster; k++) {
		clustersCenters[k].val[0] = rand() % 255;
		clustersCenters[k].val[1] = rand() % 255;
		clustersCenters[k].val[2] = rand() % 255;
		vector<Point>ptInCluster = ptInClusters[k];
		for (int j = 0; j < ptInCluster.size(); j++) {
			dst_img.at<Vec3b>(ptInCluster[j])[0] = clustersCenters[k].val[0];
			dst_img.at<Vec3b>(ptInCluster[j])[1] = clustersCenters[k].val[1];
			dst_img.at<Vec3b>(ptInCluster[j])[2] = clustersCenters[k].val[2];
		}
	}
	return dst_img;
}

double computeColorDistance(Scalar pixel, Scalar clusterPixel) {

	double diffBlue = pixel.val[0] - clusterPixel[0];
	double diffGreen = pixel.val[1] - clusterPixel[1];
	double diffRed = pixel.val[2] - clusterPixel[2];

	double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen, 2) + pow(diffRed, 2));

	return distance;

}

void findAssociatedCluster(Mat imgInput, int n_cluster, vector<Scalar> clustersCenters, vector<vector<Point>>& ptInClusters) {
	for (int r = 0; r < imgInput.rows; r++) {
		for (int c = 0; c < imgInput.cols; c++) {
			double minDistance = INFINITY;
			int closestClusterIndex = 0;
			Scalar pixel = imgInput.at<Vec3b>(r, c);

			for (int k = 0; k < n_cluster; k++) {
				Scalar clusterPixel = clustersCenters[k];
				double distance = computeColorDistance(pixel, clusterPixel);

				if (distance < minDistance) {
					minDistance = distance;
					closestClusterIndex = k;
				}
			}
			ptInClusters[closestClusterIndex].push_back(Point(c, r));
		}
	}
}

void createClustersInfo(Mat imgInput, int n_cluster, vector<Scalar>& clustersCenters,
	vector<vector<Point>>& ptInClusters) {

	RNG random(cv::getTickCount());

	for (int k = 0; k < n_cluster; k++) {
		Point centerKPoint;
		centerKPoint.x = random.uniform(0, imgInput.cols);
		centerKPoint.y = random.uniform(0, imgInput.rows);
		Scalar centerPixel = imgInput.at<Vec3b>(centerKPoint.y, centerKPoint.x);

		Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
		clustersCenters.push_back(centerK);

		vector<Point>ptInClustersK;
		ptInClusters.push_back(ptInClustersK);
	}
}

double adjustClusterCenters(Mat src_img, int n_cluster, vector<Scalar>& clustersCenters,
	vector<vector<Point>>ptInClusters, double& oldCenter, double newCenter) {
	double diffChange;

	for (int k = 0; k < n_cluster; k++) {

		vector<Point>ptInCluster = ptInClusters[k];
		double newBlue = 0;
		double newGreen = 0;
		double newRed = 0;
		for (int i = 0; i < ptInCluster.size(); i++) {
			Scalar pixel = src_img.at<Vec3b>(ptInCluster[i].y, ptInCluster[i].x);
			newBlue += pixel.val[0];
			newGreen += pixel.val[1];
			newRed += pixel.val[2];
		}
		newBlue /= ptInCluster.size();
		newGreen /= ptInCluster.size();
		newRed /= ptInCluster.size();

		Scalar newPixel(newBlue, newGreen, newRed);
		newCenter += computeColorDistance(newPixel, clustersCenters[k]);

		clustersCenters[k] = newPixel;
	}

	newCenter /= n_cluster;
	diffChange = abs(oldCenter - newCenter);

	oldCenter = newCenter;

	return diffChange;

}

Mat MyKmeans(Mat src_img, int n_cluster) {
	vector<Scalar>clustersCenters;
	vector<vector<Point>>ptInClusters;
	double threshold = 0.001;
	double oldCenter = INFINITY;
	double newCenter = 0;
	double diffChange = oldCenter - newCenter;

	createClustersInfo(src_img, n_cluster, clustersCenters, ptInClusters);

	while (diffChange > threshold) {

		newCenter = 0;
		for (int k = 0; k < n_cluster; k++) { ptInClusters[k].clear(); }

		findAssociatedCluster(src_img, n_cluster, clustersCenters, ptInClusters);

		diffChange = adjustClusterCenters(src_img, n_cluster, clustersCenters, ptInClusters, oldCenter, newCenter);


	}
	Mat dst_img = applyFinalClusterTolmage(src_img, n_cluster, ptInClusters, clustersCenters);

	//imshow("results", dst_img);
	//waitKey(0);


	return dst_img;
}



void hw1() {
	Mat src_img = imread("lab6/watermelon.jpg", 1);

	Mat dst_img = GetHSV(src_img);
	Mat rag_img = InRange(dst_img);

	Mat res_img;
	hconcat(src_img, rag_img, res_img);
	imshow("12204284 image", res_img);

	waitKey(0);
	destroyAllWindows;
}

void hw2() {
	Mat src_img = imread("lab6/fruit.jpg", 1);

	Mat dst_img = MyKmeans(src_img, 15);

	Mat res_img;
	hconcat(src_img, dst_img, res_img);
	imshow("12204284 image", res_img);

	waitKey(0);
	destroyAllWindows;
}

int main() {

	//hw1();
	hw2();

	return 0;
}