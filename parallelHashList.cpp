#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // Para crypt
#include <chrono>   // Para medir tiempo
#include <algorithm> // Para std::next_permutation
#include <omp.h>    // Para OpenMP
#include <cmath>    // Para std::pow

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 5; // Longitud de las contraseñas (ahora 5)
const std::string KNOW_SALT = "1g";     // SALT conocido

// Función para generar contraseñas de manera iterativa
bool generatePasswordsIterative(const std::string& targetHash, bool& foundPassword, int& globalIndex) {
    std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]);

    size_t totalCombinations = std::pow(CHARSET.size(), MAX_PASSWORD_LENGTH); // Total de combinaciones posibles

    // Cada hilo maneja un rango de combinaciones
    #pragma omp for
    for (size_t idx = globalIndex; idx < totalCombinations; ++idx) {
        size_t temp = idx;
        for (size_t i = 0; i < MAX_PASSWORD_LENGTH; ++i) {
            password[i] = CHARSET[temp % CHARSET.size()];
            temp /= CHARSET.size();
        }

        // Comprobar si el hash coincide
        char* generatedHash = crypt(password.c_str(), KNOW_SALT.c_str());

        // Si encontramos la contraseña, actualizamos la variable compartida
        if (targetHash == generatedHash) {
            #pragma omp critical
            {
                if (!foundPassword) {
                    std::cout << "Contraseña encontrada: " << password << " con SALT: " << KNOW_SALT << std::endl;
                    foundPassword = true; // Marcar que se encontró la contraseña
                }
            }
        }
    }

    return foundPassword;
}

// Función para probar las contraseñas con el SALT conocido
bool crackHash(const std::string& targetHash, std::chrono::duration<double>& duration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    bool foundPassword = false;
    int globalIndex = 0;  // Índice global para las contraseñas

    #pragma omp parallel shared(foundPassword, globalIndex)
    {
        generatePasswordsIterative(targetHash, foundPassword, globalIndex);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    return foundPassword;
}

int main() {
    omp_set_num_threads(4);  // Establecer el número de hilos que deseas usar

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

    // Paralelizar la búsqueda de contraseñas
    #pragma omp parallel for
    for (size_t i = 0; i < hashes.size(); ++i) {
        const std::string& targetHash = hashes[i];
        std::cout << "Hilo " << omp_get_thread_num() << " comenzando a descifrar: " << targetHash << std::endl;

        std::chrono::duration<double> duration;
        bool found = crackHash(targetHash, duration);
        if (found) {
            #pragma omp critical
            {
                logFile << "Hash: " << targetHash 
                        << " -> Descifrado en " << duration.count() 
                        << " segundos.\n";
            }
        } else {
            #pragma omp critical
            {
                logFile << "Hash: " << targetHash 
                        << " -> No descifrado (Tiempo total: " << duration.count() 
                        << " segundos).\n";
            }
        }
    }

    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performance_log.txt." << std::endl;
    return 0;
}
