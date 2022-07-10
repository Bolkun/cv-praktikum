#include "Praktikum.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// Aufgabe 1 (Musterlösung von Prof.)
int praktikum11_aufgabe1() {
	int num = 0;
	VideoCapture cap("OpenCV-11/Vid11d.avi");
	if (!cap.isOpened()) { printf("Frame Read Error.\n"); return 1; }
	Mat frame, frame0, frame1, flow, flowx, flowy;
	char code = 0;
	std::vector<cv::Mat> planes, planes1;
	cap >> frame;
	split(frame, planes);
	frame1 = planes[1];
	for(;;) {
		imshow("Frame (Optischer Fluess)", frame);
		frame1.copyTo(frame0);
		cap >> frame;
		if (frame.empty()) break;
		split(frame, planes);
		frame1 = planes[1];
		calcOpticalFlowFarneback(frame0, frame1, flow, 0.5, 2, 5, 5, 5, 1.1, OPTFLOW_FARNEBACK_GAUSSIAN);   // InputArray prev, InputArray next, computedFlowImage, imageScale, pyramidLevels, windowSize, flags
		split(flow, planes1);
		flowx = planes1[0];
		flowy = planes1[1];
		for (int i = 4; i < frame.rows - 4; i += 4) {
			for (int j = 4; j < frame.cols - 4; j += 4) {
				arrowedLine(frame, Point(j, i), Point(j + flowx.at<float>(i, j), i + flowy.at<float>(i, j)), Scalar(255, 0, 0), 1, 8, 0, 0.5);
			}
			code = waitKey(1);
			if (code > 0) break;
		}
	}
	cap.release();
}

// Aufgabe 2 (Musterlösung von Prof.) = Not working!
int praktikum11_aufgabe2() {
	KalmanFilter KF(4, 4, 0);
	Mat state(4, 1, CV_32F);
	Mat trans(4, 4, CV_32F);
	Mat processNoise(2, 1, CV_32F);
	Mat measurement = Mat::zeros(4, 1, CV_32F);
	char code = 0;
	VideoCapture cap("OpenCV-11/Vid11d.avi");
	if (!cap.isOpened()) { printf("Frame Read Error.\n"); return 1; }
	Mat frame, frame1, frame2, frame3,
		background, foreground, backimage, foreimage,
		mat1;
	vector<cv::Mat> planes, planes1;
	cap >> frame;
	split(frame, planes);
	frame1 = planes[1];
	frame1.copyTo(frame2);
	frame1.convertTo(background, CV_32F);
	vector<vector<Point>> contours;
	Mat frame4;
	cvtColor(frame2, frame4, COLOR_GRAY2BGR);
	state = (Mat_<float>(4, 1) << 100, 100, 10, -10);
	trans = (Mat_<float>(4, 4) <<
		1, 0, 1, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1);
	KF.transitionMatrix = trans;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(KF.errorCovPost, Scalar::all(1));
	setIdentity(KF.statePost, Scalar::all(0));
	for (;;) {
		imshow("Frame", frame);
		imshow("Frame4 (Kalman Filter)", frame4);
		frame1.copyTo(frame2);

		// Hier unten was fehlt
		float radius = 5;
		float objradius = 10;
		// Bis hier

		Point2f center, objcenter(0, 0);

		background.convertTo(backimage, CV_8U);
		absdiff(backimage, frame1, foreground);
		threshold(foreground, frame2, 15, 255, THRESH_BINARY);
		accumulateWeighted(frame1, background, 0.1);
		erode(frame2, frame2, Mat());
		frame2.copyTo(frame3);
		vector<vector<Point>> contours;
		findContours(frame3, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		int cs = contours.size();
		if (cs > 50) {
			cvtColor(frame2, frame4, COLOR_GRAY2BGR);
			int objposition = 0;
			for (int i = 0; i < cs; i++) {
				minEnclosingCircle(Mat(contours[i]), center, radius);
				if (radius > objradius) {
					objposition = i;
					objradius = radius;
					objcenter.x = center.x;
					objcenter.y = center.y;
				}
			}
		}
		circle(frame4, objcenter, objradius, Scalar(0, 255, 0), 1);
		circle(frame, objcenter, objradius, Scalar(0, 255, 0), 1);
		Mat prediction = KF.predict();
		Point predictPt = Point2f(prediction.at<float>(0), prediction.at<float>(1));
		Point predictVl = Point2f(prediction.at<float>(2), prediction.at<float>(3));
		circle(frame4, predictPt, 5, Scalar(0, 0, 255), 1);
		circle(frame, predictPt, 5, Scalar(0, 0, 255), 1);
		measurement = (Mat_<float>(4, 1) << objcenter.x, objcenter.y, 5, 0);
		if (cs > 50) {
			KF.correct(measurement);
		}
		code = waitKey(100);
		if (code > 0) break;
	}
	cap.release();
}


// Alternative zu 2 Aufgabe (https://www.myzhar.com/blog/tutorials/tutorial-opencv-ball-tracker-using-kalman-filter/)
// >>>>> Color to be tracked
#define MIN_H_BLUE 200
#define MAX_H_BLUE 300
// <<<<< Color to be tracked 

int praktikum11_aufgabe2a() {
    // Camera frame
    cv::Mat frame;

    // >>>> Kalman Filter
    int stateSize = 6;
    int measSize = 4;
    int contrSize = 0;

    unsigned int type = CV_32F;
    cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

    cv::Mat state(stateSize, 1, type);  // [x,y,v_x,v_y,w,h]
    cv::Mat meas(measSize, 1, type);    // [z_x,z_y,z_w,z_h]
    //cv::Mat procNoise(stateSize, 1, type)
    // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

    // Transition State Matrix A
    // Note: set dT at each processing step!
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    cv::setIdentity(kf.transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
    kf.measurementMatrix.at<float>(0) = 1.0f;
    kf.measurementMatrix.at<float>(7) = 1.0f;
    kf.measurementMatrix.at<float>(16) = 1.0f;
    kf.measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Q
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    kf.processNoiseCov.at<float>(0) = 1e-2;
    kf.processNoiseCov.at<float>(7) = 1e-2;
    kf.processNoiseCov.at<float>(14) = 5.0f;
    kf.processNoiseCov.at<float>(21) = 5.0f;
    kf.processNoiseCov.at<float>(28) = 1e-2;
    kf.processNoiseCov.at<float>(35) = 1e-2;

    // Measures Noise Covariance Matrix R
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));
    // <<<< Kalman Filter

    // Camera Index
    int idx = 0;

    // Camera Capture
    VideoCapture cap("OpenCV-11/Vid11d.avi");

    // >>>>> Camera Settings
    /*if (!cap.open(idx))
    {
        cout << "Webcam not connected.\n" << "Please verify\n";
        return EXIT_FAILURE;
    }*/

    //cap.set(cv2_CAP_PROP_FRAME_WIDTH, 1024);
    //cap.set(CV2_CAP_PROP_FRAME_HEIGHT, 768);
    // <<<<< Camera Settings

    //cout << "\nHit 'q' to exit...\n";

    char ch = 0;

    double ticks = 0;
    bool found = false;

    int notFoundCount = 0;

    // >>>>> Main loop
    while (ch != 'q' && ch != 'Q')
    {
        double precTick = ticks;
        ticks = (double)cv::getTickCount();

        double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds

        // Frame acquisition
        cap >> frame;

        cv::Mat res;
        frame.copyTo(res);

        if (found)
        {
            // >>>> Matrix A
            kf.transitionMatrix.at<float>(2) = dT;
            kf.transitionMatrix.at<float>(9) = dT;
            // <<<< Matrix A

            cout << "dT:" << endl << dT << endl;

            state = kf.predict();
            cout << "State post:" << endl << state << endl;

            cv::Rect predRect;
            predRect.width = state.at<float>(4);
            predRect.height = state.at<float>(5);
            predRect.x = state.at<float>(0) - predRect.width / 2;
            predRect.y = state.at<float>(1) - predRect.height / 2;

            cv::Point center;
            center.x = state.at<float>(0);
            center.y = state.at<float>(1);
            cv::circle(res, center, 2, CV_RGB(255, 0, 0), -1);

            cv::rectangle(res, predRect, CV_RGB(255, 0, 0), 2);
        }

        // >>>>> Noise smoothing
        cv::Mat blur;
        cv::GaussianBlur(frame, blur, cv::Size(5, 5), 3.0, 3.0);
        // <<<<< Noise smoothing

        // >>>>> HSV conversion
        cv::Mat frmHsv;
        cv::cvtColor(blur, frmHsv, COLOR_BGR2HSV);
        // <<<<< HSV conversion

        // >>>>> Color Thresholding
        // Note: change parameters for different colors
        cv::Mat rangeRes = cv::Mat::zeros(frame.size(), CV_8UC1);
        cv::inRange(frmHsv, cv::Scalar(MIN_H_BLUE / 2, 100, 80),
            cv::Scalar(MAX_H_BLUE / 2, 255, 255), rangeRes);
        // <<<<< Color Thresholding

        // >>>>> Improving the result
        cv::erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
        // <<<<< Improving the result

        // Thresholding viewing
        cv::imshow("Threshold", rangeRes);

        // >>>>> Contours detection
        vector<vector<cv::Point> > contours;
        cv::findContours(rangeRes, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        // <<<<< Contours detection

        // >>>>> Filtering
        vector<vector<cv::Point> > balls;
        vector<cv::Rect> ballsBox;
        for (size_t i = 0; i < contours.size(); i++)
        {
            cv::Rect bBox;
            bBox = cv::boundingRect(contours[i]);

            float ratio = (float)bBox.width / (float)bBox.height;
            if (ratio > 1.0f)
                ratio = 1.0f / ratio;

            // Searching for a bBox almost square
            if (ratio > 0.75 && bBox.area() >= 400)
            {
                balls.push_back(contours[i]);
                ballsBox.push_back(bBox);
            }
        }
        // <<<<< Filtering

        cout << "Balls found:" << ballsBox.size() << endl;

        // >>>>> Detection result
        for (size_t i = 0; i < balls.size(); i++)
        {
            cv::drawContours(res, balls, i, CV_RGB(20, 150, 20), 1);
            cv::rectangle(res, ballsBox[i], CV_RGB(0, 255, 0), 2);

            cv::Point center;
            center.x = ballsBox[i].x + ballsBox[i].width / 2;
            center.y = ballsBox[i].y + ballsBox[i].height / 2;
            cv::circle(res, center, 2, CV_RGB(20, 150, 20), -1);

            stringstream sstr;
            sstr << "(" << center.x << "," << center.y << ")";
            cv::putText(res, sstr.str(),
                cv::Point(center.x + 3, center.y - 3),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
        }
        // <<<<< Detection result

        // >>>>> Kalman Update
        if (balls.size() == 0)
        {
            notFoundCount++;
            cout << "notFoundCount:" << notFoundCount << endl;
            if (notFoundCount >= 100)
            {
                found = false;
            }
            /*else
                kf.statePost = state;*/
        }
        else
        {
            notFoundCount = 0;

            meas.at<float>(0) = ballsBox[0].x + ballsBox[0].width / 2;
            meas.at<float>(1) = ballsBox[0].y + ballsBox[0].height / 2;
            meas.at<float>(2) = (float)ballsBox[0].width;
            meas.at<float>(3) = (float)ballsBox[0].height;

            if (!found) // First detection!
            {
                // >>>> Initialization
                kf.errorCovPre.at<float>(0) = 1; // px
                kf.errorCovPre.at<float>(7) = 1; // px
                kf.errorCovPre.at<float>(14) = 1;
                kf.errorCovPre.at<float>(21) = 1;
                kf.errorCovPre.at<float>(28) = 1; // px
                kf.errorCovPre.at<float>(35) = 1; // px

                state.at<float>(0) = meas.at<float>(0);
                state.at<float>(1) = meas.at<float>(1);
                state.at<float>(2) = 0;
                state.at<float>(3) = 0;
                state.at<float>(4) = meas.at<float>(2);
                state.at<float>(5) = meas.at<float>(3);
                // <<<< Initialization

                kf.statePost = state;

                found = true;
            }
            else
                kf.correct(meas); // Kalman Correction

            cout << "Measure matrix:" << endl << meas << endl;
        }
        // <<<<< Kalman Update

        // Final result
        cv::imshow("Tracking", res);

        // User key
        ch = cv::waitKey(1);
    }
    // <<<<< Main loop

    return EXIT_SUCCESS;
}
