#include "Praktikum.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace std;
using namespace ml;

// Aufgabe 1
int praktikum13_aufgabe1() {
	const Mat all_leaves = imread("OpenCV-13/Img13a00.jpg");
	if (!all_leaves.data) {
		cerr << "Could not open image file" << endl;
		return EXIT_FAILURE;
	}

	Rect2d roi = selectROI(all_leaves);
	Mat templ = all_leaves(roi);
	imshow("ROI", templ);

	for (int i = 0; i < 9; i++) {
		string filename1 = "OpenCV-13/Img13a0";
		string filename2 = to_string(i + 1);
		string filename3 = ".jpg";
		Mat leaf = imread(filename1 + filename2 + filename3);

		int result_cols = leaf.cols - templ.cols + 1;
		int result_rows = leaf.rows - templ.rows + 1;
		Mat result;
		result.create(result_rows, result_cols, CV_32FC1);
		matchTemplate(leaf, templ, result, TM_SQDIFF);
		normalize(result, result, 0, 1, NORM_MINMAX);

		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		matchLoc = minLoc;
		rectangle(leaf, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

		imshow("Leaf", leaf);
		waitKey();
		destroyWindow("Leaf");
	}

	return EXIT_SUCCESS;
}

// Aufgabe 2 (Musterlösung von Prof.)
int praktikum13_aufgabe2() {
	float imgresp0, imgresp1, imgresp2, imgresp3, imgdata;
	Mat image0, templ;
	Ptr<ANN_MLP> mlp = ANN_MLP::create();
	int layer_sz[] = { 4, 5, 4 };
	Mat layer_sizes = Mat(1, 3, CV_32S, layer_sz);
	Mat train_data = Mat(3, 4, CV_32F);
	Mat train_resp = Mat(3, 4, CV_32F);
	mlp->setLayerSizes(layer_sizes);
	image0 = imread("OpenCV-13/Img13b01.jpg");
	imshow("Image0", image0);
	printf("Learning\n");
	for (int i = 0; i < 3; i++) {
		printf("Select Example % d:\n", i);
		Rect r1 = selectROI("Image0", image0, true);
		templ = image0(r1);
		imshow("Example", templ);
		printf("Select Class %d: ", i);
		scanf_s("%f %f %f %f", &imgresp0, &imgresp1, &imgresp2, &imgresp3);
		train_data.at<float>(i, 0) = templ.at<Vec3b>(0, 0)[0];
		train_data.at<float>(i, 1) = templ.at<Vec3b>(10, 0)[1];
		train_data.at<float>(i, 2) = templ.at<Vec3b>(10, 10)[2];
		train_data.at<float>(i, 3) = templ.at<Vec3b>(5, 5)[1];
		train_resp.at<float>(i, 0) = imgresp0;
		train_resp.at<float>(i, 1) = imgresp1;
		train_resp.at<float>(i, 2) = imgresp2;
		train_resp.at<float>(i, 3) = imgresp3;
	}
	mlp->setTermCriteria(TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.01));
	mlp->setTrainMethod(ANN_MLP::BACKPROP);
	mlp->setActivationFunction(ANN_MLP::SIGMOID_SYM);
	mlp->setBackpropMomentumScale(0.1);
	mlp->setBackpropWeightScale(0.1);
	Ptr<TrainData> traindata = TrainData::create(train_data, ROW_SAMPLE, train_resp);
	mlp->train(traindata);
	printf("Using\n");
	waitKey();
	for (int i = 1; i < 10; i++) {
		printf("Select Example %d:\n", i);
		Rect r1 = selectROI("Image0", image0, true);
		templ = image0(r1);
		imshow("Example", templ);
		train_data.at<float>(0, 0) = templ.at<Vec3b>(0, 0)[0];
		train_data.at<float>(0, 1) = templ.at<Vec3b>(10, 0)[1];
		train_data.at<float>(0, 2) = templ.at<Vec3b>(10, 10)[2];
		train_data.at<float>(0, 3) = templ.at<Vec3b>(5, 5)[1];
		mlp->predict(train_data, train_resp);
		imgresp0 = train_resp.at<float>(0, 0);
		imgresp1 = train_resp.at<float>(0, 1);
		imgresp2 = train_resp.at<float>(0, 2);
		imgresp3 = train_resp.at<float>(0, 3);
		printf("Class %f %f %f %f\n", imgresp0, imgresp1, imgresp2, imgresp3);
		waitKey();
	}
}