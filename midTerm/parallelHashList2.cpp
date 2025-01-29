#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // Para crypt
#include <chrono>   // Para medir tiempo
#include <omp.h>    // Para OpenMP
#include <cmath>    // Para std::pow

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 5;
const std::string KNOW_SALT = "1g";

bool generatePasswordsIterative(const std::string& targetHash, bool& foundPassword) {
    std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]);
    size_t totalCombinations = std::pow(CHARSET.size(), MAX_PASSWORD_LENGTH);

    #pragma omp parallel for schedule(dynamic) shared(foundPassword)
    for (size_t idx = 0; idx < totalCombinations; ++idx) {
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

bool crackHash(const std::string& targetHash, bool& foundPassword) {
    foundPassword = false;
    return generatePasswordsIterative(targetHash, foundPassword);
}

int main() {
    omp_set_num_threads(4); // Ajusta el número de hilos según necesidad

    std::ifstream inputFile("small_hashed_passwords2.txt");
    std::ofstream logFile("performanceParallell_log.txt");

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

    auto global_start = std::chrono::high_resolution_clock::now(); // Medición de tiempo total

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < hashes.size(); ++i) {
        const std::string& targetHash = hashes[i];
        std::cout << "Hilo " << omp_get_thread_num() << " comenzando a descifrar: " << targetHash << std::endl;

        bool found;
        crackHash(targetHash, found);

        #pragma omp critical
        {
            logFile << "Hash: " << targetHash
                    << (found ? " -> Descifrado." : " -> No descifrado.") << std::endl;
        }
    }

    auto global_end = std::chrono::high_resolution_clock::now();
    double global_duration = std::chrono::duration<double>(global_end - global_start).count();
    
    logFile << "Tiempo total de ejecución: " << global_duration << " segundos.\n";
    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performanceParallell_log.txt." << std::endl;
    std::cout << "Tiempo total de ejecución: " << global_duration << " segundos." << std::endl;

    return 0;
}
