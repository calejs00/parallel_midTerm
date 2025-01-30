#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <chrono>  // Para medir el tiempo

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
    vector<Point> data = {{1.0, 2.0}, {1.5, 1.8}, {5.0, 8.0}, {8.0, 8.0}, {1.0, 0.6}, {9.0, 11.0}};
    vector<Point> centroids(k);

    // Inicializar los centros aleatoriamente
    for (int i = 0; i < k; ++i) {
        centroids[i] = data[rand() % data.size()];
    }

    // Medir el tiempo de ejecución de la versión secuencial
    auto start = high_resolution_clock::now();
    kmeans(data, centroids, k);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución secuencial: " << duration.count() << " ms" << endl;

    // Medir el tiempo de ejecución de la versión paralela
    start = high_resolution_clock::now();
    kmeans_parallel(data, centroids, k);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Tiempo de ejecución paralelo: " << duration.count() << " ms" << endl;

    return 0;
}
