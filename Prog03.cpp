#include "Praktikum.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

// Aufgabe 1
tuple<Mat, Mat, Mat> singleChannels(Mat image) {
	Mat first, second, third;

	first = Mat(image.size(), CV_8U);
	second = first.clone();
	third = first.clone();

	for (int y = 0; y < image.size().height; y++) {
		for (int x = 0; x < image.size().width; x++) {
			first.at<uchar>(y, x) = image.at<Vec3b>(y, x)[0];
			second.at<uchar>(y, x) = image.at<Vec3b>(y, x)[1];
			third.at<uchar>(y, x) = image.at<Vec3b>(y, x)[2];
		}
	}

	return make_tuple(first, second, third);
}

Mat averageGray(Mat image) {
	Mat modified = image.clone();

	for (int y = 0; y < image.size().height; y++) {
		for (int x = 0; x < image.size().width; x++) {
			int sum = 0;
			for (int c = 0; c < image.channels(); c++) {
				sum += image.at<Vec3b>(y, x)[c];
			}
			int average = sum / image.channels();

			for (int c = 0; c < modified.channels(); c++) {
				modified.at<Vec3b>(y, x)[c] = average;
			}
		}
	}

	return modified;
}

int shift(int number, int min, int max) {
	int shifted = number + 1;
	if (shifted > max) {
		shifted = min;
	}

	return shifted;
}

Mat shiftChannels(Mat image) {
	Mat modified = image.clone();

	for (int y = 0; y < image.size().height; y++) {
		for (int x = 0; x < image.size().width; x++) {
			for (int c = 0; c < image.channels(); c++) {
				modified.at<Vec3b>(y, x)[c] = image.at<Vec3b>(y, x)[shift(c, 0, image.channels() - 1)];
			}
		}
	}

	return modified;
}

int praktikum3_aufgabe1() {
	Mat image = imread("OpenCV-03/Img03a.jpg");

	// Variante 1
	imshow("Image", image);
	imshow("Changed channel mapping", shiftChannels(image));
	imshow("Average gray", averageGray(image));

	// Variante 2
	Mat modified_image;
	modified_image = image.clone();
	cvtColor(image, modified_image, COLOR_BGR2GRAY);
	imshow("Gray", modified_image);

	tuple<Mat, Mat, Mat> singleChannelImages = singleChannels(image);
	imshow("Single gray 0", get<0>(singleChannelImages));
	imshow("Single gray 1", get<1>(singleChannelImages));
	imshow("Single gray 2", get<2>(singleChannelImages));

	imwrite("OpenCV-03/build/first_channel_gray.png", get<0>(singleChannelImages));
	imwrite("OpenCV-03/build/second_channel_gray.jpg", get<1>(singleChannelImages));
	imwrite("OpenCV-03/build/third_channel_gray.bmp", get<2>(singleChannelImages));

	waitKey(0);

	return 0;
}

// Aufgabe 2
int praktikum3_aufgabe2() {
	VideoCapture videoCapture("OpenCV-03/Vid03a.mov");
	Mat frame;

	/*while (true) {
		videoCapture >> frame;
		if (frame.empty()) break;
		imshow("Frame", frame);
		waitKey(2);
	}*/

	Mat modifiedFrame;
	/*while (true) {
		videoCapture >> frame;
		if (frame.empty()) break;
		modifiedFrame = shiftChannels(frame);
		imshow("Modified video", modifiedFrame);
		waitKey(2);
	}*/

	/*while (true) {
		videoCapture >> frame;
		if (frame.empty()) break;
		modifiedFrame = shiftChannels(frame);
		imshow("Frame", frame);
		imshow("Modified video", modifiedFrame);
		waitKey(2);
	}*/

	int width = videoCapture.get(CAP_PROP_FRAME_WIDTH);
	int height = videoCapture.get(CAP_PROP_FRAME_HEIGHT);

	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');	// constructs the fourcc code of the codec to be used in the constructor
	VideoWriter videoWriter("OpenCV-03/build/video.avi", fourcc, 15, Size(width, height));	// fps=15

	while (true) {
		videoCapture >> frame;
		if (frame.empty()) break;
		modifiedFrame = shiftChannels(frame);

		imshow("Video", frame);
		imshow("Modified video", modifiedFrame);

		videoWriter.write(modifiedFrame);

		waitKey(2);
	}

	videoCapture.release();
	videoWriter.release();

	waitKey(0);

	return 0;
}

