#define _USE_MATH_DEFINES

#include "Praktikum.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <math.h>

using namespace cv;
using namespace std;

// Aufgabe 1
RNG rng(12345);

int randColorPart() {
	return rng.uniform(0, 256);
}

Scalar randColor() {
	return Scalar(randColorPart(), randColorPart(), randColorPart());
}

Point centroid(vector<Point> contour) {
	Moments m = moments(contour);

	return Point(
		int(m.m10 / m.m00),
		int(m.m01 / m.m00)
	);
}

Mat buildStructuringElement(int size) {
	return getStructuringElement(MORPH_CROSS, Size(2 * size + 1, 2 * size + 1), Point(size, size));	//Quadrat
}

void dilate(Mat image, Mat& destination, int times) {
	int dilationSize = 1;
	Mat e = buildStructuringElement(dilationSize);
	dilate(image, destination, e, Point(-1, -1), times);
}

void erode(Mat image, Mat& destination, int times) {
	int erosionSize = 1;
	Mat e = buildStructuringElement(erosionSize);
	erode(image, destination, e, Point(-1, -1), times);
}

int praktikum8_aufgabe1() {
	Mat image = imread("OpenCV-08/Img08a.jpg", IMREAD_GRAYSCALE);
	if (!image.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}

	imshow("Image", image);

	vector<vector<Point>>contours;
	vector<Vec4i>hierarchy;
	Mat dst, edges;

	// ausdehnen
	dilate(image, dst, 3);	// Aus weiße pixeln werden quadren gebildet
	imshow("1. Dilate 1", dst);

	// auswaschen
	erode(dst, dst, 6);
	imshow("2. Erode", dst);	// weise pixeln verwaschen

	dilate(dst, dst, 3);
	imshow("3. Dilate", dst);

	int threshold1 = 100;
	int threshold2 = 2 * threshold1;
	Canny(dst, edges, threshold1, threshold2);	// Sucht Kanten von Objekten
	imshow("4. Canny", edges);

	findContours(edges, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	Mat contoursImage = Mat::zeros(edges.size(), CV_8UC3);
	Mat biggestContourImage = contoursImage.clone();
	int biggestContourIndex = 0;
	double maxArea = 0;
	double totalArea = 0;
	for (int i = 0; i >= 0; i = hierarchy[i][0]) {
		const vector<Point>& contour = contours[i];
		double area = fabs(contourArea(Mat(contour)));

		totalArea += area;

		if (area > maxArea) {
			maxArea = area;
			biggestContourIndex = i;
		}

		Point contourCentroid = centroid(contours[i]);
		Rect boundRect = boundingRect(contours[i]);

		drawContours(contoursImage, contours, (int)i, randColor(), 2, LINE_8, hierarchy);
		circle(contoursImage, contourCentroid, 5, randColor());
		rectangle(contoursImage, boundRect, randColor());
	}
	drawContours(biggestContourImage, contours, biggestContourIndex, randColor(), 2, LINE_8, hierarchy);

	imshow("Contours", contoursImage);
	imshow("Biggest contour", biggestContourImage);

	printf("Total area: %f\n", totalArea);

	waitKey();
}

// Aufgabe 2
int praktikum8_aufgabe2() {
	Mat image = imread("OpenCV-08/Img08c.jpg");
	if (!image.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}

	vector<Mat> planes;
	split(image, planes);
	Mat image0 = planes[1];

	Mat edges;
	int threshold1 = 120;
	int threshold2 = 350;
	Canny(image0, edges, threshold1, threshold2);
	imshow("Canny(Edges)", edges);

	vector<Vec2f> lines;
	double rho = 1;
	double theta = M_PI / 180;
	int threshold = 80;
	HoughLines(edges, lines, rho, theta, threshold);

	vector<Vec2f>::const_iterator it = lines.begin();
	while (it != lines.end()) {
		float rho = (*it)[0];
		float theta = (*it)[1];
		float angle = 90 - theta * 180.0 / M_PI;
		Point pt1(rho / cos(theta), 0);
		Point pt2((rho - image0.rows * sin(theta)) / cos(theta), image0.rows);
		line(image, pt1, pt2, randColor(), 1);
		++it;
	}

	imshow("Image with lines", image);

	waitKey(0);
}
