#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> 
#include <chrono>   
#include <algorithm> 

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 5; 
const std::string KNOW_SALT = "1g";     

bool generatePasswords(std::string& password, int pos, const std::string& targetHash) {
    if (pos == password.size()) {
        char* generatedHash = crypt(password.c_str(), KNOW_SALT.c_str());
        if (targetHash == generatedHash) {
            std::cout << "Contraseña encontrada: " << password << " con SALT: " << KNOW_SALT << std::endl;
            return true;
        }
        return false;
    }

    for (char c : CHARSET) {
        password[pos] = c;
        if (generatePasswords(password, pos + 1, targetHash)) {
            return true;
        }
    }

    return false;
}

bool crackHash(const std::string& targetHash, std::chrono::duration<double>& duration) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]); 
    if (generatePasswords(password, 0, targetHash)) {
        auto end_time = std::chrono::high_resolution_clock::now();
        duration = end_time - start_time;
        return true;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    return false;
}

int main() {
    std::ifstream inputFile("small_hashed_passwords2.txt");
    std::ofstream logFile("performance_log.txt");

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

    for (const auto& targetHash : hashes) {
        std::cout << "Intentando descifrar: " << targetHash << std::endl;

        std::chrono::duration<double> duration;
        if (crackHash(targetHash, duration)) {
            logFile << "Hash: " << targetHash 
                    << " -> Descifrado en " << duration.count() 
                    << " segundos.\n";
        } else {
            logFile << "Hash: " << targetHash 
                    << " -> No descifrado (Tiempo total: " << duration.count() 
                    << " segundos.\n";
        }
    }

    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performance_log.txt." << std::endl;
    return 0;
}
