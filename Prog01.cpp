#include "Praktikum.h"

#include <opencv2/opencv.hpp>

using namespace cv; // Vec3b
using namespace std;

int praktikum1_aufgabe2()
{
	Mat image;
	image = imread("OpenCV-01/Img01a.jpg"); // loads an image from the specified file and returns it
	if (!image.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}
	namedWindow("Image:");	 // creates a window that can be used as a placeholder for images and trackbars.created windows are referred to by their names
	imshow("Image:", image); // displays an image in the specified window
	waitKey(0);				 // waits for a key event infinitely or for delay milliseconds
	destroyWindow("Image:"); // destroys the window with the given name
	return 0;
}

int praktikum1_aufgabe3()
{
	Mat originalImage = imread("OpenCV-01/Img01a.jpg");
	if (!originalImage.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}

	Mat modifiedImage = originalImage.clone();

	map<string, int> constantBrightnessRectangle {
		{ "top", 20 },
		{ "left", 20 },
		{ "width", 500 },
		{ "height", 200 }
	};
	int brightness = 100;
	for (int y = constantBrightnessRectangle["top"]; y < constantBrightnessRectangle["top"] + constantBrightnessRectangle["height"]; y++) {
		for (int x = constantBrightnessRectangle["left"]; x < constantBrightnessRectangle["left"] + constantBrightnessRectangle["width"]; x++) {
			// Färbe jeweils 3 Kanälle
			for (int c = 0; c < originalImage.channels(); c++) {
				int old = modifiedImage.at<Vec3b>(y, x)[c];
				modifiedImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(old + brightness); // conversion from one primitive type to another
			}
		}
	}

	map<string, int> averageRectangle {
		{ "top", constantBrightnessRectangle["top"] + 200 + 20 },
		{ "left", 20 },
		{ "width", 500 },
		{ "height", 200 }
	};
	for (int y = averageRectangle["top"]; y < averageRectangle["top"] + averageRectangle["height"]; y++) {
		for (int x = averageRectangle["left"]; x < averageRectangle["left"] + averageRectangle["width"]; x++) {
			for (int c = 0; c < originalImage.channels(); c++) {
				int average = (int)(originalImage.at<Vec3b>(y, x - 1)[c] + originalImage.at<Vec3b>(y, x)[c]) / 2;
				modifiedImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(average);
			}
		}
	}

	map<string, int> differenceRectangle {
		{ "top", averageRectangle["top"] + 200 + 20 },
		{ "left", 20 },
		{ "width", 500 },
		{ "height", 200 }
	};
	for (int y = differenceRectangle["top"]; y < differenceRectangle["top"] + differenceRectangle["height"]; y++) {
		for (int x = differenceRectangle["left"]; x < differenceRectangle["left"] + differenceRectangle["width"]; x++) {
			for (int c = 0; c < originalImage.channels(); c++) {
				int average = (originalImage.at<Vec3b>(y, x - 1)[c] - originalImage.at<Vec3b>(y, x)[c]);
				modifiedImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(average);
			}
		}
	}

	imshow("Original Image:", originalImage);
	imshow("Modified image:", modifiedImage);
	cv::waitKey(0);

	return 0;
}
