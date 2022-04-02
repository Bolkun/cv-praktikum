#include "Praktikum.h"

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace cv;
using namespace std;

// Geometrie und Textausgabe
static Scalar randomColor(RNG& rng) {
	int col = (unsigned)rng;
	return Scalar(col & 255, (col >> 8) & 255, (col >> 16) & 255);
}
int vorlesung2_seite36() {
	int width = 1000, height = 700;
	int x1 = -width / 2, x2 = width * 3 / 2;
	int y1 = -height / 2, y2 = height * 3 / 2;
	RNG rng(0xFFFFFFFF);	// white in RGB (255, 255, 255)
	Mat image = Mat::zeros(height, width, CV_8UC3); // CV_<bit-depth>{U|S|F}C(<number_of_channels>)
	imshow("Drawing Demo", image);
	waitKey(5);
	for (int i = 0; i < 100; i++) {
		Point pt1, pt2, center;
		pt1.x = rng.uniform(x1, x2); // returns uniformly distributed random number from [x1, y1) range
		pt1.y = rng.uniform(y1, y2);
		pt2.x = rng.uniform(x1, x2);
		pt2.y = rng.uniform(y1, y2);
		int thickness = rng.uniform(-3, 10);
		center.x = rng.uniform(x1, x2);
		center.y = rng.uniform(y1, y2);
		Size axes;
		axes.width = rng.uniform(0, 200);
		axes.height = rng.uniform(0, 200);
		double angle = rng.uniform(0, 180);
		line(image, pt1, pt2, randomColor(rng), rng.uniform(1, 10), LINE_AA);
		rectangle(image, pt1, pt2, randomColor(rng), MAX(thickness, -1), LINE_AA);
		circle(image, pt1, rng.uniform(0, 300), randomColor(rng), rng.uniform(-1, 9), LINE_AA);
		ellipse(image, center, axes, angle, angle - 100, angle + 200, randomColor(rng), rng.uniform(-1, 9), LINE_AA);
		putText(image, "Text", pt2, rng.uniform(0, 8), rng.uniform(0, 100) * 0.05 + 0.1, randomColor(rng), rng.uniform(1, 10), LINE_AA);

		imshow("Drawing Demo", image);
		if (waitKey(5) >= 0) return 0;
	}
	waitKey();
	return 0;
}

// Konturfindung
Mat img;
int threshval = 100;
void on_trackbar(int, void*) {
	Mat bw = threshval < 128 ? (img < threshval) : (img > threshval);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(bw, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	Mat dst = Mat::zeros(img.size(), CV_8UC3);
	if (!contours.empty() && !hierarchy.empty()) {
		int idx = 0;
		for (; idx >= 0; idx = hierarchy[idx][0]) {
			Scalar color((rand() & 255), (rand() & 255), (rand() & 255));
			drawContours(dst, contours, idx, color, FILLED, 8, hierarchy);
		}
	}
	imshow("Connected Components", dst);
}
int vorlesung2_seite37() {
	img = imread("OpenCV-01/Img01a.jpg", 0);
	namedWindow("Image", 1);
	imshow("Image", img);
	namedWindow("Connected Components", 1);
	createTrackbar("Threshold", "Connected Components", &threshval, 255, on_trackbar);
	on_trackbar(threshval, 0);
	waitKey(0);
	return 0;
}