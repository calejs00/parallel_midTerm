#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // Para crypt
#include <chrono>   // Para medir tiempo
#include <cmath>    // Para std::pow

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 5;
const std::string KNOW_SALT = "1g";

// Genera todas las contraseñas de manera secuencial
bool generatePasswordsSequential(const std::string& targetHash, std::string& foundPassword) {
    std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]);
    size_t totalCombinations = std::pow(CHARSET.size(), MAX_PASSWORD_LENGTH);

    for (size_t idx = 0; idx < totalCombinations; ++idx) {
        // Generar la contraseña actual
        size_t temp = idx;
        for (size_t i = 0; i < MAX_PASSWORD_LENGTH; ++i) {
            password[i] = CHARSET[temp % CHARSET.size()];
            temp /= CHARSET.size();
        }

        // Generar el hash y comparar
        char* generatedHash = crypt(password.c_str(), KNOW_SALT.c_str());
        if (targetHash == generatedHash) {
            foundPassword = password;
            return true;  // Termina temprano si encuentra la contraseña
        }
    }
    return false;
}

// Función para medir el tiempo de desencriptación
bool crackHashSequential(const std::string& targetHash, double& duration, std::string& foundPassword) {
    auto start_time = std::chrono::high_resolution_clock::now();
    bool success = generatePasswordsSequential(targetHash, foundPassword);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration<double>(end_time - start_time).count();
    return success;
}

int main() {
    std::ifstream inputFile("small_hashed_passwords2.txt");
    std::ofstream logFile("performanceSequential_log.txt");

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

    auto global_start = std::chrono::high_resolution_clock::now(); // Tiempo total

    for (const auto& targetHash : hashes) {
        std::string foundPassword;
        double duration;
        bool found = crackHashSequential(targetHash, duration, foundPassword);

        if (found) {
            logFile << "Hash: " << targetHash
                    << " -> Contraseña encontrada: " << foundPassword
                    << " en " << duration << " segundos.\n";
        } else {
            logFile << "Hash: " << targetHash
                    << " -> No descifrado (Tiempo total: " << duration << " segundos).\n";
        }
    }

    auto global_end = std::chrono::high_resolution_clock::now();
    double global_duration = std::chrono::duration<double>(global_end - global_start).count();
    
    logFile << "Tiempo total de ejecución: " << global_duration << " segundos.\n";
    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performanceSequential_log.txt." << std::endl;
    return 0;
}
