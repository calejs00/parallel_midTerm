#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>  // Usamos esta librería que está incluida en OpenCV estándar
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

vector<Mat> loadImages(const string& folderPath) {
    vector<Mat> images;
    vector<string> filenames;
    glob(folderPath + "/*.jpg", filenames);  // Asegúrate de usar la extensión correcta

    if (filenames.empty()) {
        cerr << "Error: No se encontraron imágenes en la carpeta." << endl;
    }

    for (const auto& filename : filenames) {
        Mat img = imread(filename);
        if (img.empty()) {
            cerr << "Error al cargar la imagen: " << filename << endl;
        } else {
            cout << "Imagen cargada: " << filename << " | Dimensiones: " << img.cols << "x" << img.rows << endl;
            images.push_back(img);
        }
    }


    cout << "Total de imágenes cargadas: " << images.size() << endl;

    return images;
}


void detectAndMatchFeatures(const Mat& img1, const Mat& img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, Mat& descriptors1, Mat& descriptors2, vector<DMatch>& matches) {
    Ptr<ORB> detector = ORB::create();  // Usamos ORB en lugar de características de contrib
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);
    
    BFMatcher matcher(NORM_HAMMING, true);  // Usamos BFMatcher con NORM_HAMMING para ORB
    matcher.match(descriptors1, descriptors2, matches);

    // Visualizar las correspondencias
    // Mat img_matches;
    // drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
    // imshow("Correspondencias", img_matches);  // Muestra las correspondencias entre las imágenes
    // waitKey(0);  // Espera a que se presione una tecla antes de continuar

}

Mat estimateHomography(const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, const vector<DMatch>& matches) {
    vector<Point2f> points1, points2;
    for (size_t i = 0; i < matches.size(); ++i) {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }
    Mat homography = findHomography(points1, points2, RANSAC);
    
    // // Imprimir la homografía para depuración
    // cout << "Homografía calculada: " << endl << homography << endl;

    return homography;
}

Mat warpImages(const Mat& img1, const Mat& img2, const Mat& homography) {
    // Se asegura de que la panorámica tenga el tamaño adecuado
    int width = img1.cols + img2.cols;
    int height = max(img1.rows, img2.rows);

    Mat result;
    warpPerspective(img2, result, homography, Size(width, height));

    // Copia la primera imagen en la panorámica resultante
    img1.copyTo(result(Rect(0, 0, img1.cols, img1.rows)));

    return result;
}

bool isVertical(const Mat& img) {
    return img.rows > img.cols;
}

Mat stitchImagesGroup(const vector<Mat>& group) {
    if (group.empty()) {
        cerr << "Error: Grupo de imágenes vacío." << endl;
        return Mat();
    }

    cout << "Procesando grupo de imágenes de tamaño: " << group.size() << endl;

    Mat panorama = group[0];
    if (panorama.empty()) {
        cerr << "Error: La primera imagen del grupo está vacía." << endl;
        return Mat();
    }

    for (size_t i = 1; i < group.size(); ++i) {
        if (group[i].empty()) {
            cerr << "Error: La imagen " << i << " en el grupo está vacía." << endl;
            return Mat();
        }
    }


    bool isVerticalPanorama = isVertical(group[0]);
    
    for (int i = 0; i < group.size() - 1; ++i) {
        vector<KeyPoint> keypoints1, keypoints2;
        Mat descriptors1, descriptors2;
        vector<DMatch> matches;
        detectAndMatchFeatures(panorama, group[i + 1], keypoints1, keypoints2, descriptors1, descriptors2, matches);
       
           imshow("Imagen 1", panorama);
        imshow("Imagen 2", group[i + 1]);
    
        // Si tienes más de dos imágenes, puedes mostrar la imagen intermedia también
        if (group.size() > 2 && i < group.size() - 2) {
            imshow("Imagen 3", group[i + 2]);  // Mostrar la tercera imagen si existe
        }

        waitKey(0);  // Espera por una tecla para continuar
       
       
       
        Mat homography = estimateHomography(keypoints1, keypoints2, matches);
        // Ajustar según si es una panorámica vertical o horizontal
        if (isVerticalPanorama) {
            Mat warpedImage = warpImages(panorama, group[i + 1], homography);
            panorama = warpedImage;
        } else {
            Mat warpedImage = warpImages(panorama, group[i + 1], homography);
            panorama = warpedImage;
        }
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
    // Cargar las imágenes desde una carpeta (por ejemplo, "dataset/"), reemplaza esta ruta con tu propia ruta
    vector<Mat> images = loadImages("/home/clara/Documentos/parallel_midTerm/finalTerm/SPHP_dataset/");

    if (images.size() < 23) {
        cerr << "Error: No hay suficientes imágenes en el dataset." << endl;
        return -1;
    }

    // Verificar antes de acceder
    if (images[22].empty()) {
        cerr << "Error: La imagen 22 está vacía." << endl;
        return -1;
    }

    cout << "Accediendo a images[22] correctamente." << endl;
    for (size_t i = 0; i < images.size(); i++) {
    if (images[i].empty()) {
        cerr << "Error: images[" << i << "] está vacío." << endl;
        return -1;
    }
}

    // Organizar las imágenes en grupos para las panorámicas
    vector<vector<Mat>> imageGroups = {
        {images[0], images[1], images[2]},  // Panorámica 1 (I1, I2, I3)
        {images[3], images[4], images[5]},  // Panorámica 2 (I4, I5, I6)
        {images[6], images[7]},  // Panorámica 3 (I7, I8, I9)
        {images[8], images[9]},            // Panorámica 4 (I10, I11)
        {images[10], images[11]},           // Panorámica 5 (I12, I13)
        {images[12], images[13]},           // Panorámica 6 (I14, I15)
        {images[14], images[15]},  // Panorámica 7 (I16, I17, I18)
        {images[16], images[17]},           // Panorámica 8 (I18, I19)
        {images[18], images[19], images[20]},           // Panorámica 9 (I20, I21)
        {images[21], images[22]}            // Panorámica 10 (I22, I23)
    };

    // Medir el tiempo de ejecución
    double start = (double)getTickCount();
    vector<Mat> panoramas = createAllPanoramas(imageGroups);  // Secuencial
    double end = (double)getTickCount();
    cout << "Tiempo de ejecución: " << (end - start) / getTickFrequency() << " segundos" << endl;

    // Mostrar las panorámicas generadas
    for (size_t i = 0; i < panoramas.size(); ++i) {
        Mat resized;
        resize(panoramas[i], resized, Size(), 2.0, 2.0);  // Escalamos x2
        imshow("Panorámica " + to_string(i + 1), resized);
    }

    waitKey(0);
    return 0;
}
