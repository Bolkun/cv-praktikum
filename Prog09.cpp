#include "Praktikum.h"
#include "opencv2/calib3d.hpp"
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

// Aufgabe 1 (Muster Lösung von Prof)
Mat Image, image1, image2, image3;
Mat homography, rotatran = Mat::eye(3, 3, CV_64F), scal = Mat::eye(3, 3, CV_64F);
Mat points1(4, 1, CV_32FC2), points2(4, 1, CV_32FC2);
Point2f p1, p2;
int mi = 0, mj = 0;

void onMouse1(int event, int x, int y, int, void*) {
    switch (event) {
    case EVENT_LBUTTONUP:
        if (mi < 4) {
            points1.at<Point2f>(mi) = Point2f(x, y);
            printf("%f, %f\n", points1.at<Point2f>(mi).x, points1.at<Point2f>(mi).y);
            mi++;
        }
        else {
            Mat mp1(1, 1, CV_32FC2, Scalar(x, y));
            Mat mp2;
            perspectiveTransform(mp1, mp2, homography);
            p1 = mp1.at<Point2f>(0, 0);
            p2 = mp2.at<Point2f>(0, 0);
            circle(image1, p1, 5, Scalar(0, 255, 0));
            imshow("Image1", image1);
            circle(image2, p2, 5, Scalar(0, 0, 255));
            imshow("Image2", image2);
            circle(image3, p2, 5, Scalar(0, 0, 255));
            imshow("Image3", image3);
        }
        break;
    }
}

void onMouse2(int event, int x, int y, int, void*) {
    switch (event) {
    case EVENT_LBUTTONUP:
        if (mj < 4) {
            points2.at<Point2f>(mj) = Point2f(x, y);
            printf("%f, %f\n", points2.at<Point2f>(mj).x, points2.at<Point2f>(mj).y);
            mj++;
        }
        else {
            homography = findHomography(points1, points2);
            warpPerspective(image1, image3, homography, Size(image2.cols, image2.rows));
            imshow("Image3", image3);
            printf("%lf, %lf, %lf\n%lf, %lf, %lf\n%lf, %lf, %lf\n",
                homography.at<double>(0, 0), homography.at<double>(0, 1), homography.at<double>(0, 2),
                homography.at<double>(1, 0), homography.at<double>(1, 1), homography.at<double>(1, 2), 
                homography.at<double>(2, 0), homography.at<double>(2, 1), homography.at<double>(2, 2) );
        }
        break;
    }
}

int praktikum9_aufgabe1() {
	Image = imread("OpenCV-09/Img09a.jpg");
	if (!Image.data) {
		printf("Error: Couldn't open the image file.\n");
		return 1;
	}
    image1 = Image(Rect(0, 0, Image.cols / 2, Image.rows));
    image2 = Image(Rect(Image.cols / 2, 0, Image.cols/2, Image.rows));
	imshow("Image1", image1);
    imshow("Image2", image2);
    setMouseCallback("Image1", onMouse1, 0);
    setMouseCallback("Image2", onMouse2, 0);
	waitKey();
    destroyWindow("Image1");
    destroyWindow("Image2");
    return 0;
}

// Aufgabe 2
int praktikum9_aufgabe2() {
    // Defining the dimensions of checkerboard
    int CHECKERBOARD[2]{ 6,9 };

    // Creating vector to store vectors of 3D points for each checkerboard image
    std::vector<std::vector<cv::Point3f> > objpoints;

    // Creating vector to store vectors of 2D points for each checkerboard image
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
    {
        for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    // Extracting path of individual image stored in a given directory
    std::vector<cv::String> images;
    // Path of the folder containing checkerboard images
    std::string path = "OpenCV-09/*.jpg"; //"./images/*.jpg";

    cv::glob(path, images);

    cv::Mat frame, gray;
    // vector to store the pixel coordinates of detected checker board corners 
    std::vector<cv::Point2f> corner_pts;
    bool success;

    // Looping over all the images in the directory
    for (int i{ 0 }; i < images.size(); i++)
    {
        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Finding checker board corners
        // If desired number of corners are found in the image then success = true  
        success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

        /*
         * If desired number of corner are detected,
         * we refine the pixel coordinates and display
         * them on the images of checker board
        */
        if (success)
        {
            cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);

            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }

        cv::imshow("Image", frame);
        cv::waitKey(0);
    }

    cv::destroyAllWindows();

    cv::Mat cameraMatrix, distCoeffs, R, T;

    /*
     * Performing camera calibration by
     * passing the value of known 3D points (objpoints)
     * and corresponding pixel coordinates of the
     * detected corners (imgpoints)
    */
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    std::cout << "distCoeffs : " << distCoeffs << std::endl;
    std::cout << "Rotation vector : " << R << std::endl;
    std::cout << "Translation vector : " << T << std::endl;

    return 0;
}

// NOT Working!
int aufgabe02()
{
    Mat image, image1, image2, homography, map1, cameraMatrix, distCoeffs;
    int flag = 0;
    Size boardSize(7, 7);
    vector<Point3f> objectCorners;
    vector<Point2f> imageCorners, object4Corners(4), image4Corners(4);
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
    vector<Mat> rvecs, tvecs;

    for (int i = 0; i < boardSize.height; i++)
    {
        for (int j = 0; j < boardSize.width; j++)
        {
            objectCorners.push_back(Point3f(i, j, 0.0f));
        }
    }


    image = imread("Daten/Praktikum09/Img09c10.jpg");
    if (!image.data) {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }
    bool patternWasFound = findChessboardCorners(image, boardSize, imageCorners);
    if (patternWasFound)
    {
        printf("Pattern was found\n");
    }
    else
    {
        printf("Pattern was not found\n");
    }

    drawChessboardCorners(image, boardSize, imageCorners, patternWasFound);
    objectPoints.push_back(objectCorners);
    imagePoints.push_back(imageCorners);

    cv::imshow("Image", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    /*
    for (int i = 1; i < 7; i++)
    {
        char imgName[20] = "a";
        image = imread(imgName);
        findChessboardCorners(image, boardSize, imageCorners);
        drawChessboardCorners(image, boardSize, imageCorners, true);
        objectPoints.push_back(objectCorners);
        imagePoints.push_back(imageCorners);
    }
    calibrateCamera(objectPoints, imagePoints, image.size(), cameraMatrix, distCoeffs, rvecs, tvecs, flag);
    undistort(image, image1, cameraMatrix, distCoeffs, map1);
    object4Corners = objectCorners...;
    image4Corners = imageCorners...;
    homography = getPerspectiveTransform(image4Corners, object4Corners);
    warpPerspective(image, image2, homography, Size(image.cols * 1, image.rows * 2));
    int x = 0;
    int y = 0;
    Mat mp1(1, 1, CV_32FC2, Scalar(x, y)), mp2;
    perspectiveTransform(mp1, mp2, homography);
    x = mp2.at<Point2f>(0, 0)[0];
    y = mp2.at<Point2f>(0, 0)[1];
    */

    return 0;
}