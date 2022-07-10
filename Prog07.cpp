#include "Praktikum.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace cv;
using namespace std;

// Aufgabe 1
int prak07_A1() {
    Mat image0, image2, image3, image4, image5, image6, image7, image8, image9;
    Mat image1 = imread("OpenCV-07/Img07a.jpg");
    vector<cv::Mat> planes;
    split(image1, planes);
    printf("Size: %d", planes.size());  // 3 [0] - b, [1] - g, [2] - r
    //green channel bcs. most information in it
    image0 = planes[1];
    imshow("Original", image1);
    imshow("planes[0]", image0);
    imshow("planes[1]", image0);
    imshow("planes[2]", image0);
    waitKey();
    Mat kernel1 = (Mat_<double>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
    filter2D(image1, image2, image1.depth(), kernel1);  // inputArray, outputArray, ddepth, InputArray
    imshow("horizontal", image2);
    waitKey();
    Mat kernel2 = (Mat_<double>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    filter2D(image1, image3, image1.depth(), kernel2);
    imshow("vertikal", image3);
    waitKey();
    threshold(image1, image4, 200, 255, THRESH_BINARY); // inputArray, outputArray, thres, maxthres, type
    imshow("thresh", image4);
    waitKey();
    // The Sobel Operator is a discrete differentiation operator. It computes an approximation of the gradient of an image intensity function
    // The Sobel Operator combines Gaussian smoothing and differentiation.
    Sobel(image1, image5, image1.depth(), 1, 0, FILTER_SCHARR); // inputArray, outputArray, depth, x, y, size
    Sobel(image1, image6, image1.depth(), 2, 0, 3);
    Sobel(image1, image7, image1.depth(), 3, 0, 7);
    imshow("sobel", image5);
    waitKey();
    imshow("sobel", image6);
    waitKey();
    imshow("sobel", image7);
    waitKey();
    Laplacian(image1, image9, image1.depth(), 3, 1, 1); // inputArray, outputArray, depth, size, scale, delta
    imshow("Laplacian", image9);
    waitKey();
    cornerHarris(image0, image8, 1, 1, 1); // inputArray, outputArray, BlockSize, ksize (sobel operator), k
    image8 = 255 - image8;
    normalize(image8, image8, 0, 1, NORM_MINMAX); // inputArray, outputArray, alpha, betta, norm_type
    imshow("cornerHarris", image8);
    waitKey();

    return 1;
}

// Aufgabe 1
Mat sobel(Mat src, Mat kernel) {
    Mat dst;
    filter2D(src, dst, CV_16S, kernel);

    return dst;
}

Mat sobelHorizontal(Mat src) {
    return sobel(src, (Mat_<char>(3, 3) <<
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1));
}

Mat sobelVertical(Mat src) {
    return sobel(src, (Mat_<char>(3, 3) <<
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1));
}

int praktikum7_aufgabe1() {
    Mat image, canny, blurred, gray, grad, cornerImage, absGradX, absGradY;

    image = imread("OpenCV-07/Img07a.jpg");
    if (!image.data) {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }

    // Kantendetektor
    Canny(image, canny, 0, 30, 3);
    imshow("Canny", canny);

    // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
    Size kernelSize = Size(3, 3);
    double sigmaX = 0;
    GaussianBlur(image, blurred, kernelSize, sigmaX);
    // Convert the image to grayscale
    cvtColor(blurred, gray, COLOR_BGR2GRAY);

    Mat gradX = sobelHorizontal(gray);
    Mat gradY = sobelVertical(gray);

    // converting back to CV_8U
    convertScaleAbs(gradX, absGradX);
    convertScaleAbs(gradY, absGradY);
    addWeighted(absGradX, 0.5, absGradY, 0.5, 0, grad);

    cornerHarris(grad, cornerImage, 7, 5, 0.05);

    imshow("Original", image);
    imshow("Edges (sobel Vert. & Hor.)", grad);
    imshow("Corners (cornerHarris)", cornerImage);

    waitKey();
    destroyAllWindows();

    return 0;
}

// Aufgabe 2
MatND calculateHistForGrayscale(Mat image, int hbins, int sbins) {
    MatND hist;
    int histSize[] = { hbins, sbins };
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float sranges[] = { 0, 256 };
    const float* ranges[] = { hranges, sranges };

    int channels[] = { 0 };
    try {
        calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
    } catch (Exception e) {
        printf(e.msg.c_str());
    }

    return hist;
}

int praktikum7_aufgabe2() {
    Mat image1, image2;

    image1 = imread("OpenCV-07/Img07c.jpg", IMREAD_GRAYSCALE);
    if (!image1.data) {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }
    image2 = imread("OpenCV-07/Img07d.jpg", IMREAD_GRAYSCALE);
    if (!image2.data) {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }

    int rectWidth = 10, rectHeight = 10;
    Mat rect1 = Mat(image1, Rect(25, 25, rectWidth, rectHeight));
    Mat rect2 = Mat(image1, Rect(70, 70, rectWidth, rectHeight));
    int hbins = 25, sbins = 25;

    MatND lastHist, hist;
    Mat rect;
    double compareValue;
    string compareString;
    printf(" ");
    for (int x = 0; x < image1.size().width - rectWidth; x += rectWidth)
    {
        for (int y = 0; y < image1.size().height - rectHeight; y += rectHeight)
        {
            rect = Mat(image1, Rect(x, y, rectWidth, rectHeight));
            hist = calculateHistForGrayscale(rect, hbins, sbins);
            if (!lastHist.empty())
            {
                compareValue = compareHist(hist, lastHist, HISTCMP_CORREL);
                if (compareValue > 0.7) {
                    // ähnlich
                    compareString = " ";
                } else {
                    // unähnlich
                    compareString = "-";
                }
                printf("%s", compareString);
            }
            lastHist = hist;
        }
        printf("\n");
    }

    return 0;
}
