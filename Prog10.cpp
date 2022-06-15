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

// Aufgabe 1 (VL Folie 152 vorlage)
int praktikum10_aufgabe1() {
    Mat image1, image2, image3, image4, h1, h2, m1i, m2i;   // 2 Bilder Stereomäßig aufgenommen, h1 = homography Mat
    vector<Vec3f> lines1, lines2;
    vector<Point2f> selPoints1, selPoints2;

    image1 = imread("OpenCV-10/Img10a01.png", IMREAD_GRAYSCALE);
    image2 = imread("OpenCV-10/Img10a02.png", IMREAD_GRAYSCALE);

    // Punkte müssen übereinstimmen
    int arX1[8] = { 190, 80, 216, 268, 235, 158, 199, 351};  // manuel aus photoshop ausgelesen
    int arY1[8] = { 159, 255, 254, 230, 95, 43, 72, 194};

    int arX2[8] = { 162, 57, 194, 251, 205, 148, 184, 336};  // manuel aus photoshop ausgelesen
    int arY2[8] = { 159, 255, 256, 230, 95, 43, 72, 194};

    for (int i = 0; i < 8; i++) {   // 8 Punkten in einen Bild und 8 Punkten in anderen Bild auswählen
        int x1 = arX1[i]; 
        int y1 = arY1[i];
        int x2 = arX2[i];
        int y2 = arY2[i];
        selPoints1.push_back(Point2f(x1, y1)); // Punkte in selPoints laden
        //selPoints2.push_back(Point2f(x2, y2));
        selPoints2.push_back(Point2f(x1 + 1 * i, y1 + 0 * i));    // horizontale linien
        //selPoints2.push_back(Point2f(x2 + 2 * i, y2 + 1 * i));    // diagonale linien
        //selPoints2.push_back(Point2f(x2 + y2, y2));                  // rechts nach links linien
    }

    Mat fundamental = findFundamentalMat(selPoints1, selPoints2, FM_RANSAC, 3.0, 0.99);
    computeCorrespondEpilines(selPoints1, 1, fundamental, lines1);  // Ergebnis = Weise Linien
    computeCorrespondEpilines(selPoints2, 2, fundamental, lines2);

    for (vector<Vec3f>::const_iterator it = lines1.begin(); it != lines1.end(); ++it) {
        line(image1, Point(0, -(*it)[2] / (*it)[1]), Point(image1.cols, -((*it)[2] + (*it)[0] * image1.cols) / (*it)[1]), Scalar(255, 255, 255));
    }
    for (vector<Vec3f>::const_iterator it = lines2.begin(); it != lines2.end(); ++it) {
        line(image2, Point(0, -(*it)[2] / (*it)[1]), Point(image2.cols, -((*it)[2] + (*it)[0] * image2.cols) / (*it)[1]), Scalar(255, 255, 255));
    }
    imshow("Image1-epilines", image1);
    imshow("Image2-epilines", image2);

    stereoRectifyUncalibrated(selPoints1, selPoints2, fundamental, image1.size(), h1, h2, 10);
    warpPerspective(image1, image3, h1, Size(image1.cols * 2, image1.rows * 2));
    warpPerspective(image2, image4, h2, Size(image2.cols * 2, image2.rows * 2));
    imshow("Image3", image3);
    imshow("Image4", image4);

    Mat disp, disp8;
    Ptr<StereoBM> bm = StereoBM::create(); // SGBM
    bm->setPreFilterCap(31);
    bm->setBlockSize(33);
    bm->setMinDisparity(1);
    bm->setNumDisparities(32);
    bm->setTextureThreshold(10);
    bm->setUniquenessRatio(15);
    bm->setSpeckleWindowSize(100);
    bm->setSpeckleRange(32);
    bm->setDisp12MaxDiff(11);
    bm->compute(image1, image2, disp);
    normalize(disp, disp, 0, 255, NORM_MINMAX);
    disp.convertTo(disp8, CV_8U);
    imshow("Disparity", disp8); // ukr: nerivnosti, Unterschiede, was vorne ist heller, was dunkler ist hinterer Backgraund - tiefen Bild

    Mat depth, depth8, repro, repro8;
    Mat q = (Mat_<float>(4, 4) << 1, 0, 0, 100,
        0, 1, 0, 100,
        0, 0, 1, 0,
        0, 0.001, 0.001, 1);
    q.at<float>(1, 1) = cos(-0.3);
    q.at<float>(2, 1) = -sin(-0.3);
    q.at<float>(1, 2) = sin(-0.3);
    q.at<float>(2, 2) = cos(-0.3);
    reprojectImageTo3D(disp8, depth, q, false, CV_32F);
    float x, y, z;
    repro = Mat(depth.rows, depth.cols, CV_32F, Scalar::all(0));
    for (int i = 0; i < depth.rows - 1; i++) {
        for (int j = 0; j < depth.cols - 1; j++) {
            x = depth.at<Vec3f>(i, j)[0];
            y = depth.at<Vec3f>(i, j)[1];
            z = depth.at<Vec3f>(i, j)[2];
            if (0 < x && x < repro.cols && 0 < y && y < repro.rows) {
                repro.at<float>(y, x) = z;
            }
        }
    }
    normalize(repro, repro8, 0, 255, NORM_MINMAX);
    repro8.convertTo(repro8, CV_8UC1);
    imshow("Reprojektion in 3D", repro8);

    waitKey(0);
}

// used https://datahacker.rs/stereo-geometry-code/ 
int praktikum10_aufgabe1b() {
    Mat image1, image2, image3, image4, h1, h2, m1i, m2i;   // 2 Bilder Stereomäßig aufgenommen, h1 = homography Mat
    vector<Vec3f> lines1, lines2;
    vector<Point2f> selPoints1, selPoints2;

    image1 = imread("OpenCV-10/Img10a01.png", IMREAD_GRAYSCALE);
    image2 = imread("OpenCV-10/Img10a02.png", IMREAD_GRAYSCALE);

    // 1. Detecting key points
    // Vector of keypoints
    vector<KeyPoint> keypoints1;
    vector<KeyPoint> keypoints2;

    // We will use orb feature detector
    // Construction of the orb feature detector
    Ptr<FeatureDetector> orb = ORB::create(5000);

    // Feature detection
    orb->detect(image1, keypoints1);
    orb->detect(image2, keypoints2);

    // Draw the kepoints
    Mat imageKeyPoints;
    drawKeypoints(image1, keypoints1, imageKeyPoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imshow("Image1-Keypoints", imageKeyPoints);

    drawKeypoints(image2, keypoints2, imageKeyPoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imshow("Image2-Keypoints", imageKeyPoints);

    // Extraction of the orb descriptors
    Mat descriptors1, descriptors2;
    orb->compute(image1, keypoints1, descriptors1);
    orb->compute(image2, keypoints2, descriptors2);

    // 2. Matching key points
    cv::FlannBasedMatcher matcher = cv::FlannBasedMatcher(cv::makePtr<cv::flann::LshIndexParams>(6, 12, 1), cv::makePtr<cv::flann::SearchParams>(50));
    std::vector< std::vector<cv::DMatch> > matches;
    matcher.knnMatch(descriptors1, descriptors2, matches, 2);

    std::cout << "matches size: " << matches.size() << std::endl;
    // Select few Matches
    std::vector<cv::DMatch> good;

    //Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i].size() >= 2) {
            if (matches[i][0].distance < ratio_thresh * matches[i][1].distance) {
                good.push_back(matches[i][0]);
            }
        }
    }

    cout << "Number of matched points: " << good.size() << endl;

    // 3. Computing a fundamental matrix
    // Convert one vector of keypoints into two vectors of Point2f
    std::vector<int> points1;
    std::vector<int> points2;
    for (auto iter = good.begin(); iter != good.end(); ++iter) {
        // Get the indexes of the selected matched keypoints
        points1.push_back(iter->queryIdx);
        points2.push_back(iter->trainIdx);
    }

    // Convert keypoints into Point2f
    cv::KeyPoint::convert(keypoints1, selPoints1, points1);
    cv::KeyPoint::convert(keypoints2, selPoints2, points2);

    // Compute F matrix from all good matches
    Mat fundemental = cv::findFundamentalMat(cv::Mat(selPoints1), cv::Mat(selPoints2), cv::FM_LMEDS);

    std::cout << "F-Matrix size= " << fundemental.rows << "," << fundemental.cols << std::endl;
    std::cout << "F-Matrix = \n" << fundemental << std::endl;

    // 4. Computing epipolar lines
    computeCorrespondEpilines(selPoints1, 1, fundemental, lines1);  // Ergebnis = Weise Linien
    computeCorrespondEpilines(selPoints2, 2, fundemental, lines2);

    for (vector<Vec3f>::const_iterator it = lines1.begin(); it != lines1.end(); ++it) {
        line(image1, Point(0, -(*it)[2] / (*it)[1]), Point(image1.cols, -((*it)[2] + (*it)[0] * image1.cols) / (*it)[1]), Scalar(255, 255, 255));
    }
    for (vector<Vec3f>::const_iterator it = lines2.begin(); it != lines2.end(); ++it) {
        line(image2, Point(0, -(*it)[2] / (*it)[1]), Point(image2.cols, -((*it)[2] + (*it)[0] * image2.cols) / (*it)[1]), Scalar(255, 255, 255));
    }
    imshow("Image1-epilines", image1);
    imshow("Image2-epilines", image2);

    Mat disp, disp8;
    Ptr<StereoBM> bm = StereoBM::create(); // SGBM
    bm->setPreFilterCap(31);
    bm->setBlockSize(33);
    bm->setMinDisparity(1);
    bm->setNumDisparities(32);
    bm->setTextureThreshold(10);
    bm->setUniquenessRatio(15);
    bm->setSpeckleWindowSize(100);
    bm->setSpeckleRange(32);
    bm->setDisp12MaxDiff(11);
    bm->compute(image1, image2, disp);
    normalize(disp, disp, 0, 255, NORM_MINMAX);
    disp.convertTo(disp8, CV_8U);
    imshow("Disparity", disp8); // ukr: nerivnosti, Unterschiede, was vorne ist heller, was dunkler ist hinterer Backgraund - tiefen Bild

    Mat depth, depth8, repro, repro8;
    Mat q = (Mat_<float>(4, 4) << 1, 0, 0, 100,
        0, 1, 0, 100,
        0, 0, 1, 0,
        0, 0.001, 0.001, 1);
    q.at<float>(1, 1) = cos(-0.3);
    q.at<float>(2, 1) = -sin(-0.3);
    q.at<float>(1, 2) = sin(-0.3);
    q.at<float>(2, 2) = cos(-0.3);
    reprojectImageTo3D(disp8, depth, q, false, CV_32F);
    float x, y, z;
    repro = Mat(depth.rows, depth.cols, CV_32F, Scalar::all(0));
    for (int i = 0; i < depth.rows - 1; i++) {
        for (int j = 0; j < depth.cols - 1; j++) {
            x = depth.at<Vec3f>(i, j)[0];
            y = depth.at<Vec3f>(i, j)[1];
            z = depth.at<Vec3f>(i, j)[2];
            if (0 < x && x < repro.cols && 0 < y && y < repro.rows) {
                repro.at<float>(y, x) = z;
            }
        }
    }
    normalize(repro, repro8, 0, 255, NORM_MINMAX);
    repro8.convertTo(repro8, CV_8UC1);
    imshow("Reprojektion in 3D", repro8);

    waitKey(0);
}

// Aufgabe 2
int praktikum10_aufgabe2() {
    Mat image1, image2, image3;

    image1 = imread("OpenCV-10/Img10d01.jpg", IMREAD_GRAYSCALE);  // linke teil vorne
    image2 = imread("OpenCV-10/Img10d02.jpg", IMREAD_GRAYSCALE);  // mittlere teil vorne
    image3 = imread("OpenCV-10/Img10d03.jpg", IMREAD_GRAYSCALE);  // rechte teil vorne

    Laplacian(image1, image1, image1.depth(), 5, 1, 100);
    //imshow("Image1-Laplacian", image1);
    pyrDown(image1, image1);    // wie Glättung + Skalierung runter gesetzt
    //imshow("Image1-pyrDown", image1);
    blur(image1, image1, Size(21, 21), Point(0, 0));
    //imshow("Image1-blur1", image1);
    blur(image1, image1, Size(21, 21), Point(0, 0));
    //imshow("Image1-blur2", image1);
    threshold(image1, image1, 130, 100, THRESH_BINARY); // Schranken Filter (Alles was über 130 wird auf 100 gesetzt, binär - tiefenwerte) - Kontrasteste Bereich am hellsten
    imshow("Image1-threshold", image1);

    Laplacian(image2, image2, image2.depth(), 5, 1, 100);
    pyrDown(image2, image2);
    blur(image2, image2, Size(21, 21), Point(0, 0));
    blur(image2, image2, Size(21, 21), Point(0, 0));
    threshold(image2, image2, 120, 175, THRESH_BINARY);
    imshow("Image2-threshold", image2);

    Laplacian(image3, image3, image3.depth(), 5, 1, 100);
    pyrDown(image3, image3);
    blur(image3, image3, Size(21, 21), Point(0, 0));
    blur(image3, image3, Size(21, 21), Point(0, 0));
    threshold(image3, image3, 125, 250, THRESH_BINARY);
    imshow("Image3-threshold", image3);

    image3 = max(image2, image3);   // Bilder über einander legen
    image3 = max(image1, image3);
    imshow("Image3-Verbindet mit image 1 & 2", image3);

    waitKey(0);
}