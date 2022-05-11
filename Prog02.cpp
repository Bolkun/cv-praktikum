#define _CRT_SECURE_NO_WARNINGS
#include "Praktikum.h"
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

// Aufgabe 1
void toggleNegative(Mat image) {
	const int keyQ = 113;
	const int keyW = 119;

	while (true) {
		int key = waitKey(0);
		switch (key) {
		case keyQ:	// Back
			return;
		case keyW: // Change to negativ
			image = 255 - image; // Negative Image Formel
			imshow("Image", image);
			break;
		}
	}
}

int praktikum2_aufgabe1() {
	Mat image = imread("OpenCV-02/Img02a.jpg", IMREAD_GRAYSCALE);	// convert image to the single channel grayscale image (codec internal conversion)
	if (!image.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}
	namedWindow("Image:");
	imshow("Image:", image);

	printf("Zeilen: %d, Spalten: %d\n", image.rows, image.cols);	// (rows) 482-height, (cols) 720-width

	double minimum, maximum;
	minMaxLoc(image, &minimum, &maximum);
	printf("Minimum: %f, Maximum: %f\n", minimum, maximum);	// min 1, max 255

	Scalar meanOfImage, meanStdDevOfImage;
	meanStdDev(image, meanOfImage, meanStdDevOfImage);	// berechnet Mittelwert der angegebenen Array in 'meanOfArray' und Standardabweichung in 'meanStdDevOfImage'
	printf("Mittelwert: %f\n", meanOfImage[0]);
	printf("Standardabweichung: %f\n", meanStdDevOfImage[0]);

	toggleNegative(image);

	destroyWindow("Image:");
	return 0;
}

// Aufgabe 2
int contrast = 1;
int brightness = 1;
Point startPoint, endPoint;
Mat image = imread("OpenCV-02/Img02a.jpg");

void onMouse(int event, int x, int y, int flags, void* userdata) {
	if (event == MouseEventTypes::EVENT_LBUTTONDOWN) {
		printf("x: %d, y: %d\n", x, y);

		Vec3b position = image.at<Vec3b>(y, x);
		
		printf("Farbwerte: (%d, %d, %d, %d)\n", position[0], position[1], position[2], position[3]);
		
		circle(image, Point(x, y), 10, Scalar(0, 0, 255));
		imshow("Image", image);
		
		char output[15];
		sprintf(output, "x: %d, y: %d", x, y);
		Mat control = Mat(image.rows, image.cols, CV_8UC1, Scalar(70));
		putText(control, output, Point(10, 30), FONT_HERSHEY_TRIPLEX, 1, Scalar(255, 255, 255));
		imshow("Control", control);
	}
}

void onContrastChange(int pos, void* userdata) {
	if (pos == 0) {
		contrast = 1;
		return;
	}

	contrast = pos;

	Mat newImage;
	image.convertTo(newImage, -1, contrast, brightness);
	imshow("Image", newImage);
}

void onBrightnessChange(int pos, void* userdata) {
	brightness = pos;
	Mat newImage;
	image.convertTo(newImage, -1, contrast, brightness);
	imshow("Image", newImage);
}

int praktikum2_aufgabe2() {
	imshow("Image", image);

	Mat control = Mat(image.rows, image.cols, CV_8UC1, Scalar(70));
	imshow("Control", control); // Grey bg window
	
	setMouseCallback("Image", onMouse, &image);

	createTrackbar("Contrast", "Control", &contrast, 10, onContrastChange);
	createTrackbar("Brightness", "Control", &brightness, 255, onBrightnessChange);

	waitKey(0);

	Rect roi = selectROI(image); // press 'Space' or 'Enter' button
	imshow("ROI", image(roi));

	waitKey(0);

	destroyWindow("Image");
	destroyWindow("Control");

	return 0;
}