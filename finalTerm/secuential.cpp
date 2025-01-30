#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <chrono>  // Para medir el tiempo
#include <omp.h>   // Para usar OpenMP

using namespace std;
using namespace std::chrono;

struct Point {
    double x, y;
};

double euclidean_distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Función para K-means secuencial
void kmeans(const vector<Point>& data, vector<Point>& centroids, int k, int max_iters = 100) {
    int n = data.size();
    vector<int> labels(n);
    vector<Point> new_centroids(k);

    for (int iter = 0; iter < max_iters; ++iter) {
        for (int i = 0; i < n; ++i) {
            double min_dist = euclidean_distance(data[i], centroids[0]);
            int closest_centroid = 0;
            for (int j = 1; j < k; ++j) {
                double dist = euclidean_distance(data[i], centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = j;
                }
            }
            labels[i] = closest_centroid;
        }

        vector<int> count(k, 0);
        new_centroids = vector<Point>(k, {0.0, 0.0});
        for (int i = 0; i < n; ++i) {
            int label = labels[i];
            new_centroids[label].x += data[i].x;
            new_centroids[label].y += data[i].y;
            count[label]++;
        }

        for (int i = 0; i < k; ++i) {
            if (count[i] > 0) {
                new_centroids[i].x /= count[i];
                new_centroids[i].y /= count[i];
            }
        }

        bool converged = true;
        for (int i = 0; i < k; ++i) {
            if (euclidean_distance(centroids[i], new_centroids[i]) > 1e-4) {
                converged = false;
                break;
            }
        }

        centroids = new_centroids;
        if (converged) break;
    }
}

// Función para K-means paralelo
void kmeans_parallel(const vector<Point>& data, vector<Point>& centroids, int k, int max_iters = 100) {
    int n = data.size();
    vector<int> labels(n);
    vector<Point> new_centroids(k);

    for (int iter = 0; iter < max_iters; ++iter) {
        #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            double min_dist = euclidean_distance(data[i], centroids[0]);
            int closest_centroid = 0;
            for (int j = 1; j < k; ++j) {
                double dist = euclidean_distance(data[i], centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = j;
                }
            }
            labels[i] = closest_centroid;
        }

        vector<int> count(k, 0);
        new_centroids = vector<Point>(k, {0.0, 0.0});

        #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            int label = labels[i];
            #pragma omp atomic
            new_centroids[label].x += data[i].x;
            #pragma omp atomic
            new_centroids[label].y += data[i].y;
            #pragma omp atomic
            count[label]++;
        }

        for (int i = 0; i < k; ++i) {
            if (count[i] > 0) {
                new_centroids[i].x /= count[i];
                new_centroids[i].y /= count[i];
            }
        }

        bool converged = true;
        for (int i = 0; i < k; ++i) {
            if (euclidean_distance(centroids[i], new_centroids[i]) > 1e-4) {
                converged = false;
                break;
            }
        }

        centroids = new_centroids;
        if (converged) break;
    }
}

int main() {
    srand(time(0));

    int k = 3;  // Número de clusters
    int n = 100000; // Número de puntos en el conjunto de datos

    // Generar puntos aleatorios
    vector<Point> data(n);
    for (int i = 0; i < n; ++i) {
        data[i].x = (rand() % 1000) / 1000.0;
        data[i].y = (rand() % 1000) / 1000.0;
    }

    vector<Point> centroids(k);

    // Inicializar los centros aleatoriamente
    for (int i = 0; i < k; ++i) {
        centroids[i] = data[rand() % data.size()];
    }

    // Ejecutar pruebas con diferentes números de hilos
    for (int threads = 1; threads <= 8; threads *= 2) {
        omp_set_num_threads(threads); // Establecer el número de hilos

        // Medir el tiempo de ejecución de la versión secuencial
        auto start = high_resolution_clock::now();
        if (threads == 1) { // Solo ejecuta la versión secuencial cuando threads == 1
            kmeans(data, centroids, k);
        } else {
            kmeans_parallel(data, centroids, k);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Tiempo de ejecución con " << threads << " hilos: " << duration.count() << " ms" << endl;
    }

    return 0;
}
