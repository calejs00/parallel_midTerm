#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // Para crypt
#include <chrono>   // Para medir tiempo
#include <omp.h>    // Para OpenMP
#include <cmath>    // Para std::pow

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 5; // Longitud de las contraseñas (ahora 5)
const std::string KNOW_SALT = "1g";   // SALT conocido

bool generatePasswordsIterative(const std::string& targetHash, bool& foundPassword) {
    std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]);
    size_t totalCombinations = std::pow(CHARSET.size(), MAX_PASSWORD_LENGTH);

    // Cada hilo maneja un rango dinámico de combinaciones
    #pragma omp parallel for schedule(dynamic) shared(foundPassword)
    for (size_t idx = 0; idx < totalCombinations; ++idx) {
        #pragma omp flush(foundPassword)
        if (foundPassword) continue; // Si ya se encontró la contraseña, terminar

        size_t temp = idx;
        for (size_t i = 0; i < MAX_PASSWORD_LENGTH; ++i) {
            password[i] = CHARSET[temp % CHARSET.size()];
            temp /= CHARSET.size();
        }

        char* generatedHash = crypt(password.c_str(), KNOW_SALT.c_str());

        if (targetHash == generatedHash) {
            #pragma omp critical
            {
                if (!foundPassword) {
                    foundPassword = true;
                    std::cout << "Contraseña encontrada: " << password << " con SALT: " << KNOW_SALT << std::endl;
                }
            }
        }
    }

    return foundPassword;
}

bool crackHash(const std::string& targetHash, std::chrono::duration<double>& duration) {
    auto start_time = std::chrono::high_resolution_clock::now();

    bool foundPassword = false;
    generatePasswordsIterative(targetHash, foundPassword);

    auto end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double>(end_time - start_time).count();
    return foundPassword;
}

int main() {
    omp_set_num_threads(4); // Establecer el número de hilos que deseas usar

    std::ifstream inputFile("small_hashed_passwords2.txt");
    if (!inputFile || !logFile) {
        std::cerr << "Error al abrir los archivos necesarios." << std::endl;
        return 1;
    }
    std::vector<std::string> hashes;
    std::string hash;
    while (std::getline(inputFile, hash)) {
        if (!hash.empty()) {
            hashes.push_back(hash);
        }
    }

    inputFile.close();
    std::vector<std::stringstream> threadLogs(omp_get_max_threads());
    auto global_start = std::chrono::high_resolution_clock::now(); // Medición global




    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < hashes.size(); ++i) {
        const std::string& targetHash = hashes[i];
        double duration;
        std::cout << "Hilo " << omp_get_thread_num() << " comenzando a descifrar: " << targetHash << std::endl;
        int thread_id = omp_get_thread_num();
        bool found = crackHash(targetHash, duration);
        threadLogs[thread_id] << "Hilo " << thread_id << " - Hash: " << targetHash
                              << " -> Descifrado en " << duration << " segundos.\n";
    }

    auto global_end = std::chrono::high_resolution_clock::now();
    double global_duration = std::chrono::duration<double>(global_end - global_start).count();

    std::ofstream logFile("performanceParallell_log.txt");
    for (const auto& log : threadLogs) {
        logFile << log.str();
    }
    logFile << "Tiempo total de ejecución: " << global_duration << " segundos.\n";
    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performanceParallel_log.txt." << std::endl;
    return 0;
}