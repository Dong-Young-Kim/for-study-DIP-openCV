//#include <iostream>
//#include <ctime>
//#include <algorithm>
//#include "opencv2/core/core.hpp"			// Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
//#include "opencv2/highgui/highgui.hpp"		// GUI와 관련된 요소를 포함하는 헤더(imshow 등)
//#include "opencv2/imgproc/imgproc.hpp"		// 각종 이미지 처리 함수를 포함하는 헤더
//using namespace cv;
//using namespace std;
//
//double computeColorDistance(Scalar pixel, Scalar clusterPixel) {
//
//	double diffBlue = pixel.val[0] - clusterPixel[0];
//	double diffGreen = pixel.val[1] - clusterPixel[1];
//	double diffRed = pixel.val[2] - clusterPixel[2];
//
//	double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen, 2) + pow(diffRed, 2));
//
//	return distance;
//
//}
//
//void findAssociatedCluster(Mat imgInput, int n_cluster, vector<Scalar> clustersCenters, vector<vector<Point>>& ptInClusters) {
//	for (int r = 0; r < imgInput.rows; r++) {
//		for (int c = 0; c < imgInput.cols; c++) {
//			double minDistance = INFINITY;
//			int closestClusterIndex = 0;
//			Scalar pixel = imgInput.at<Vec3b>(r, c);
//
//			for (int k = 0; k < n_cluster; k++) {
//				Scalar clusterPixel = clustersCenters[k];
//				double distance = computeColorDistance(pixel, clusterPixel);
//
//				if (distance < minDistance) {
//					minDistance = distance;
//					closestClusterIndex = k;
//				}
//			}
//			ptInClusters[closestClusterIndex].push_back(Point(c, r));
//		}
//	}
//}
//
//Mat applyFinalClusterTolmage(Mat src_img, int n_cluster, vector<vector<Point>>& ptInClusters, vector<Scalar> clustersCenters) {
//	Mat dst_img(src_img.size(), src_img.type());
//
//	for (int k = 0; k < n_cluster; k++) {
//		vector<Point>ptInCluster = ptInClusters[k];
//		for (int j = 0; j < ptInCluster.size(); j++) {
//			dst_img.at<Vec3b>(ptInCluster[j])[0] = clustersCenters[k].val[0];
//			dst_img.at<Vec3b>(ptInCluster[j])[1] = clustersCenters[k].val[1];
//			dst_img.at<Vec3b>(ptInCluster[j])[2] = clustersCenters[k].val[2];
//		}
//	}
//	return dst_img;
//}
//
//void createClustersInfo(Mat imgInput, int n_cluster, vector<Scalar>& clustersCenters,
//	vector<vector<Point>>& ptInClusters) {
//
//	RNG random(cv::getTickCount());
//
//	for (int k = 0; k < n_cluster; k++) {
//		Point centerKPoint;
//		centerKPoint.x = random.uniform(0, imgInput.cols);
//		centerKPoint.y = random.uniform(0, imgInput.rows);
//		Scalar centerPixel = imgInput.at<Vec3b>(centerKPoint.y, centerKPoint.x);
//
//		Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
//		clustersCenters.push_back(centerK);
//
//		vector<Point>ptInClustersK;
//		ptInClusters.push_back(ptInClustersK);
//	}
//}
//
//double adjustClusterCenters(Mat src_img, int n_cluster, vector<Scalar>& clustersCenters,
//	vector<vector<Point>>ptInClusters, double& oldCenter, double newCenter) {
//	double diffChange;
//
//	for (int k = 0; k < n_cluster; k++) {
//
//		vector<Point>ptInCluster = ptInClusters[k];
//		double newBlue = 0;
//		double newGreen = 0;
//		double newRed = 0;
//		for (int i = 0; i < ptInCluster.size(); i++) {
//			Scalar pixel = src_img.at<Vec3b>(ptInCluster[i].y, ptInCluster[i].x);
//			newBlue += pixel.val[0];
//			newGreen += pixel.val[1];
//			newRed += pixel.val[2];
//		}
//		newBlue /= ptInCluster.size();
//		newGreen /= ptInCluster.size();
//		newRed /= ptInCluster.size();
//
//		Scalar newPixel(newBlue, newGreen, newRed);
//		newCenter += computeColorDistance(newPixel, clustersCenters[k]);
//
//		clustersCenters[k] = newPixel;
//	}
//
//	newCenter /= n_cluster;
//	diffChange = abs(oldCenter - newCenter);
//
//	oldCenter = newCenter;
//
//	return diffChange;
//
//}
//
//Mat MyKmeans(Mat src_img, int n_cluster) {
//	vector<Scalar>clustersCenters;
//	vector<vector<Point>>ptInClusters;
//	double threshold = 0.001;
//	double oldCenter = INFINITY;
//	double newCenter = 0;
//	double diffChange = oldCenter - newCenter;
//
//	createClustersInfo(src_img, n_cluster, clustersCenters, ptInClusters);
//
//	while (diffChange > threshold) {
//
//		newCenter = 0;
//		for (int k = 0; k < n_cluster; k++) { ptInClusters[k].clear(); }
//
//		findAssociatedCluster(src_img, n_cluster, clustersCenters, ptInClusters);
//
//		diffChange = adjustClusterCenters(src_img, n_cluster, clustersCenters, ptInClusters, oldCenter, newCenter);
//
//
//	}
//	Mat dst_img = applyFinalClusterTolmage(src_img, n_cluster, ptInClusters, clustersCenters);
//
//	//imshow("results", dst_img);
//	//waitKey(0);
//
//
//	return dst_img;
//}
//
////Mat CvKmeans(Mat src_img, int k) {
////
////	//2차원 영상 -> 1차원 벡터
////	Mat samples(src_img.rows * src_img.cols, src_img.channels(), CV_32F);
////	for (int y = 0; y < src_img.rows; y++) {
////		for (int x = 0; x < src_img.cols; x++) {
////			if (src_img.channels() == 3) {
////				for (int z = 0; z < src_img.channels(); z++) {
////					samples.at<float>(y + x * src_img.rows, z) = (float)src_img.at<Vec3b>(y, x)[z];
////				}
////			}
////			else {
////				samples.at<float>(y + x + src_img.rows) = (float)src_img.at<uchar>(y, x);
////			}
////		}
////	}
////
////	//opencv k-means 수행
////	Mat labels;
////	Mat centers;
////	int attemps = 5;
////
////	kmeans(samples, k, labels,
////		TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001),
////		attemps, KMEANS_PP_CENTERS, centers);
////
////	//1차원 벡터 => 2차원 영상
////	Mat dst_img(src_img.size(), src_img.type());
////	for (int y = 0; y < src_img.rows; y++) {
////		for (int x = 0; x < src_img.cols; x++) {
////			int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
////			if (src_img.channels() == 3) {
////				for (int z = 0; z < src_img.channels(); z++) {
////					dst_img.at<Vec3b>(y, x)[
////
////
////				}
////			}
////		}
////	}
////}
//
//
//
//void ex1() {
//	Mat src_img = imread("lab6/fruit.jpg", 1);
//
//	Mat dst_img = MyKmeans(src_img, 3);
//
//	Mat res_img;
//	hconcat(src_img, dst_img, res_img);
//	imshow("12204284 image", res_img);
//
//	waitKey(0);
//	destroyAllWindows;
//}
//
//int main() {
//
//	ex1();
//
//	return 0;
//}