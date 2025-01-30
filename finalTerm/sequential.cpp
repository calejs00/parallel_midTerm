#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

vector<Mat> loadImages(const string& folderPath) {
    vector<Mat> images;
    vector<string> filenames;
    glob(folderPath, filenames);  // Cargar todas las imágenes de la carpeta
    for (const auto& filename : filenames) {
        images.push_back(imread(filename));
    }
    return images;
}

void detectAndMatchFeatures(const Mat& img1, const Mat& img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, Mat& descriptors1, Mat& descriptors2, vector<DMatch>& matches) {
    Ptr<ORB> detector = ORB::create();
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);
    
    BFMatcher matcher(NORM_HAMMING, true);
    matcher.match(descriptors1, descriptors2, matches);
}

Mat estimateHomography(const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, const vector<DMatch>& matches) {
    vector<Point2f> points1, points2;
    for (size_t i = 0; i < matches.size(); ++i) {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }
    return findHomography(points1, points2, RANSAC);
}

Mat warpImages(const Mat& img1, const Mat& img2, const Mat& homography) {
    Mat result;
    warpPerspective(img2, result, homography, Size(img1.cols + img2.cols, img1.rows));
    img1.copyTo(result(Rect(0, 0, img1.cols, img1.rows)));
    return result;
}

Mat stitchImagesGroup(const vector<Mat>& group) {
    Mat panorama = group[0];  // Empezamos con la primera imagen del grupo
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    vector<DMatch> matches;
    
    // Si hay más de 1 imagen, emparejamos secuencialmente
    for (int i = 0; i < group.size() - 1; ++i) {
        detectAndMatchFeatures(panorama, group[i + 1], keypoints1, keypoints2, descriptors1, descriptors2, matches);
        Mat homography = estimateHomography(keypoints1, keypoints2, matches);
        Mat warpedImage = warpImages(panorama, group[i + 1], homography);
        panorama = warpedImage;  // La panorámica se actualiza con la imagen emparejada
    }

    return panorama;
}

vector<Mat> createAllPanoramas(const vector<vector<Mat>>& imageGroups) {
    vector<Mat> panoramas;
    
    for (int i = 0; i < imageGroups.size(); ++i) {
        Mat panorama = stitchImagesGroup(imageGroups[i]);
        panoramas.push_back(panorama);
    }

    return panoramas;
}

int main() {
    // Cargar las imágenes desde una carpeta (por ejemplo, "dataset/")
    vector<Mat> images = loadImages("C:/home/clara/Documentos/parallel_midTerm/finalTerm");

    // Organizar las imágenes en grupos para las panorámicas
    vector<vector<Mat>> imageGroups = {
        {images[0], images[1], images[2]},  // Panorámica 1 (I1, I2, I3)
        {images[3], images[4], images[5]},  // Panorámica 2 (I4, I5, I6)
        {images[6], images[7], images[8]},  // Panorámica 3 (I7, I8, I9)
        {images[9], images[10]},            // Panorámica 4 (I10, I11)
        {images[11], images[12]},           // Panorámica 5 (I12, I13)
        {images[13], images[14]},           // Panorámica 6 (I14, I15)
        {images[15], images[16], images[17]},  // Panorámica 7 (I16, I17, I18)
        {images[18], images[19]},           // Panorámica 8 (I18, I19)
        {images[20], images[21]},           // Panorámica 9 (I20, I21)
        {images[22], images[23]}            // Panorámica 10 (I22, I23)
    };

    // Medir el tiempo de ejecución
    double start = (double)getTickCount();
    vector<Mat> panoramas = createAllPanoramas(imageGroups);  // Secuencial
    double end = (double)getTickCount();
    cout << "Tiempo de ejecución: " << (end - start) / getTickFrequency() << " segundos" << endl;

    // Mostrar las panorámicas generadas
    for (size_t i = 0; i < panoramas.size(); ++i) {
        imshow("Panorámica " + to_string(i + 1), panoramas[i]);
    }

    waitKey(0);
    return 0;
}
