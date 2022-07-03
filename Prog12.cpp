#include "Praktikum.h"
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

// Aufgabe 1 & 2 (Musterlösung von Prof.)
int praktikum12_aufgabe1UND2() {
	Mat image, image0, image1, image2,
		imagergb, imagebgr0, imagebgr1, imagebgr2,
		image3, image4, image5, image6, image7, image8,
		imagehls, imagehls0, imagehls1, imagehls2;
	vector<Mat> planes;

	image = imread("OpenCV-12/Img12a.jpg");
	imshow("Image", image);
	split(image, planes);
	imagebgr0 = planes[0];
	imagebgr1 = planes[1];
	imagebgr2 = planes[2];
	imshow("Imagebgr0 (Blau Kanal)", imagebgr0);
	imshow("Imagebgr1 (Grün Kanal)", imagebgr1);
	imshow("Imagebgr2 (Rot Kanal)", imagebgr2);
	waitKey();
	cvtColor(image, imagehls, COLOR_BGR2HLS);
	imshow("Imagehls", imagehls);
	split(imagehls, planes);
	imagehls0 = planes[0];
	imagehls1 = planes[1];
	imagehls2 = planes[2];
	imshow("Imagehls0 (Hue)", imagehls0);
	imshow("Imagehls1 (Lightness)", imagehls1);
	imshow("Imagehls2 (Saturation)", imagehls2);
	waitKey();
	threshold(imagehls0, image0, 100, 0, THRESH_TOZERO);
	threshold(image0, image0, 130, 0, THRESH_TOZERO_INV);
	normalize(image0, image0, 0, 255, NORM_MINMAX);
	threshold(image0, image0, 150, 250, THRESH_BINARY);
	erode(image0, image3, Mat(), Point(-1, -1), 2);
	dilate(image3, image3, Mat(), Point(-1, -1), 2);
	threshold(imagehls0, image1, 35, 0, THRESH_TOZERO);
	threshold(image1, image1, 90, 0, THRESH_TOZERO_INV);
	normalize(image1, image1, 0, 255, NORM_MINMAX);
	threshold(image1, image1, 35, 170, THRESH_BINARY);
	erode(image1, image4, Mat(), Point(-1, -1), 2);
	dilate(image4, image4, Mat(), Point(-1, -1), 2);
	threshold(imagehls0, image2, 0, 0, THRESH_TOZERO);
	threshold(image2, image2, 25, 0, THRESH_TOZERO_INV);
	normalize(image2, image2, 0, 255, NORM_MINMAX);
	threshold(image2, image2, 5, 100, THRESH_BINARY);
	erode(image2, image5, Mat(), Point(-1, -1), 2);
	dilate(image5, image5, Mat(), Point(-1, -1), 2);
	image6 = max(image0, image1);
	image6 = max(image6, image2);
	image7 = max(image3, image4);
	image7 = max(image7, image5);
	distanceTransform(image7, image7, DIST_L1, 3);
	image7.convertTo(image7, CV_8U);
	normalize(image7, image7, 0, 255, NORM_MINMAX);
	imshow("Image", image);
	imshow("Image1", image1);
	imshow("Image2", image2);
	imshow("Image3", image3);
	imshow("Image4", image4);
	imshow("Image5", image5);
	imshow("Image6", image6);
	imshow("Image7", image7);
	waitKey();
	int sz[] = { 5,5 };
	Mat mat1(2, sz, CV_8U, Scalar::all(1));
	erode(image6, image8, mat1);
	erode(image8, image8, mat1);
	dilate(image8, image8, mat1);
	imshow("Image8", image8);
	waitKey();
	vector<vector<Point>> contours;
	findContours(image8, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	Mat image9(image8.size(), CV_8UC3, Scalar(255, 255, 255));
	drawContours(image9, contours, -1, Scalar(0, 0, 0), 1);
	imshow("Image9", image9);
	waitKey();
	vector<vector<Point>>::const_iterator itc = contours.begin();
	int object[10000][6]; //Identifier, Xposition, YPosition, Size, Color, Class
	int relation[10000][6]; //Identifier1, Identifier2, Distance, HDirection, VDirection, Class
	int k = 0;
	while (itc != contours.end()) {
		Moments mom = moments(Mat(*itc++));
		if (mom.m00 >= 100) {
			object[k][0] = k;
			object[k][1] = mom.m10 / mom.m00;
			object[k][2] = mom.m01 / mom.m00;
			object[k][3] = mom.m00;
			object[k][4] = imagehls0.at<uchar>(mom.m01 / mom.m00, mom.m10 / mom.m00);
			object[k][5] = image8.at<uchar>(mom.m01 / mom.m00, mom.m10 / mom.m00);
			printf("Obj (%d,%d,%d,%d,%d,%d)\n", object[k][0], object[k][1], object[k][2], object[k][3], object[k][4], object[k][5]);
			circle(image9, Point(object[k][1], object[k][2]), 2, Scalar(0, 0, 255), 2);
			char h[3]; sprintf_s(h, "%d", k);
			putText(image9, h, Point(object[k][1], object[k][2]), 0, 0.4, Scalar(0, 0, 255), 1);
			k++;
		}
	}
	imshow("Image9", image9);
	waitKey();
	int n = 0;
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < k; j++) {
			if (abs(object[i][1] - object[j][1]) < 100 && abs(object[i][2] - object[j][2]) < 100 &&
				(abs(object[i][1] - object[j][1]) > 1 || abs(object[i][2] - object[j][2]) > 1)) {
				relation[n][0] = i;
				relation[n][1] = j;
				relation[n][2] = (int)sqrt(pow(object[i][1] - object[j][1], 2) + pow(object[i][2] - object[j][2], 2));
				if (object[i][1] - object[j][1] > 0) relation[n][3] = -1; else relation[n][3] = 1;
				if (object[i][2] - object[j][2] > 0) relation[n][4] = -1; else relation[n][4] = 1;
				relation[n][5] = (relation[n][3] * relation[n][4]);
				printf("Rel (%d,%d,%d,%d,%d,%d)\n", relation[n][0], relation[n][1], relation[n][2], relation[n][3], relation[n][4], relation[n][5]);
				line(image9, Point(object[i][1], object[i][2]), Point(object[j][1], object[j][2]), Scalar(255, 0, 0), 1);
				char h[3]; sprintf_s(h, "%d", relation[n][5]);
				putText(image9, h, Point((object[i][1] + object[j][1]) / 2, (object[i][2] + object[j][2]) / 2), 0, 0.3, Scalar(255, 0, 0), 1);
				n++;
			}
		}
	}
	imshow("Image9", image9);
	waitKey();
	return 0;
}