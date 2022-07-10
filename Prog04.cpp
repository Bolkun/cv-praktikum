#include "Praktikum.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

// Aufgabe 1
int praktikum4_aufgabe1() {
	Mat imageLeft = imread("OpenCV-04/Img04a01.jpg");
	Mat imageRight = imread("OpenCV-04/Img04a02.jpg");

	imshow("Image left", imageLeft);
	imshow("Image right", imageRight);

	vector<Mat> images;
	vector<Mat>::iterator it;
	it = images.begin();
	it = images.insert(it, imageLeft);
	it = images.insert(it, imageRight);

	Mat panorama;

	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);

	Stitcher::Status status = stitcher->stitch(images, panorama);	// Input Array, Output Array

	if (status != Stitcher::OK)
	{
		printf("Can't stitch images, error code = %d\n", int(status));
		return 1;
	}

	imshow("Panorama", panorama);

	waitKey(0);

	return 0;
}

// Aufgabe 2
int praktikum4_aufgabe2() {
	Mat image1 = imread("OpenCV-04/Img04d01.jpeg");
	Mat image2 = imread("OpenCV-04/Img04d02.jpeg");

	imshow("firstImage", image1);
	imshow("secondImage", image2);

	Ptr<Feature2D> feature2D = BRISK::create(40);	// Feature2D ermittelt markante Punkte in einem Bild,
	Ptr<DescriptorMatcher> descriptorMatcher;		// und DescriptorMatcher setzt diese Bildpunkte in Bildpaaren miteinander in Beziehung.

	vector<DMatch> matches;
	vector<KeyPoint> keyImage1, keyImage2;

	Mat descImage1, descImage2, result1, result2, result;

	// Markante Punkte finden
	feature2D->detectAndCompute(image1, Mat(), keyImage1, descImage1, false);	// Detects keypoints and computes the descriptors
	feature2D->detectAndCompute(image2, Mat(), keyImage2, descImage2, false);	// (Input Mat, mask, vec KeyPoints, Output Mat, useProvidedKeyPoints)

	drawKeypoints(image1, keyImage1, result1);	// This function draws matches of keypoints from two images in the output image. Match is a line connecting two keypoints(circles)
	drawKeypoints(image2, keyImage2, result2);

	imshow("result1", result1);
	imshow("result2", result2);

	// Markante Punkte in beiden Bildern matchen
	descriptorMatcher = DescriptorMatcher::create(DescriptorMatcher::MatcherType::BRUTEFORCE);
	descriptorMatcher->match(descImage1, descImage2, matches, Mat());

	drawMatches(image1, keyImage1, image2, keyImage2, matches, result);

	imshow("Result", result);

	//// Beste Matches finden
	//Mat index;
	//int matchesCount = int(matches.size());
	//Mat tab(matchesCount, 1, CV_32F);

	//for (int i = 0; i < matchesCount; i++) {
	//	tab.at<float>(i, 0) = matches[i].distance;
	//}

	//sortIdx(tab, index, SORT_EVERY_COLUMN + SORT_ASCENDING);

	//vector<DMatch> bestMatches;

	//// hoechstens 30 Matches
	//for (int i = 0; i < min(30, matchesCount); i++) {
	//	bestMatches.push_back(matches[index.at<int>(i, 0)]);
	//}

	//drawMatches(image1, keyImage1, image2, keyImage2, bestMatches, result);
	//imshow("Result best matches", result);

	waitKey(0);

	return 0;
}

// Aufgabe 3
int praktikum4_aufgabe3() {
	Mat image, imageGray;

	image = imread("OpenCV-04/Img04b.jpg");
	image = imread("OpenCV-04/Img04c.png");
	cvtColor(image, imageGray, COLOR_BGR2GRAY);

	CascadeClassifier faceCascade, eyesCascade;	// CascadeClassifier kann in Verbindung mit im XML - Format gespeicherten Trainingsergebnissen zur Augen - und Gesichtserkennung verwendet werden.
	faceCascade.load("OpenCV-04/haarcascade_frontalface_alt.xml");
	eyesCascade.load("OpenCV-04/haarcascade_eye.xml");

	// Find faces
	vector<Rect> faces;
	faceCascade.detectMultiScale(imageGray, faces);

	Scalar red = Scalar(0, 0, 255);
	Scalar blue = Scalar(255, 0, 0);

	int countOfFaces = int(faces.size());

	for (int i = 0; i < countOfFaces; i++) {
		// Mark face with red circle
		Rect face = faces[i];

		float faceCenterX = face.x + face.width / 2;
		float faceCenterY = face.y + face.height / 2;
		Point center(faceCenterX, faceCenterY);

		ellipse(image, center, Size(face.width / 2, face.height / 2), 0, 0, 360, red, 4);

		// Find eyes inside face
		Mat faceROI = imageGray(face);
		vector<Rect> eyes;
		eyesCascade.detectMultiScale(faceROI, eyes);

		int countOfEyes = int(eyes.size());
		for (int j = 0; j < countOfEyes; j++) {
			// Mark eye with blue circle
			Rect eye = eyes[j];

			float eyeCenterX = face.x + eye.x + eye.width / 2;
			float eyeCenterY = face.y + eye.y + eye.height / 2;
			Point eyeCenter(eyeCenterX, eyeCenterY);
			int radius = cvRound((eye.width + eye.height) * 0.25);

			circle(image, eyeCenter, radius, blue, 4);
		}
	}

	imshow("Image", image);

	waitKey(0);

	return 0;
}