#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // Para crypt
#include <chrono>   // Para medir tiempo
#include <algorithm> // Para std::next_permutation

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t MAX_PASSWORD_LENGTH = 8; // Longitud máxima de las contraseñas
const size_t MAX_SALT_LENGTH = 2;     // Supongamos un SALT máximo de 4 caracteres

// Función para generar contraseñas recursivamente
bool generatePasswords(std::string& password, int pos, const std::string& targetHash, const std::string& salt) {
    if (pos == password.size()) {
        char* generatedHash = crypt(password.c_str(), salt.c_str());
        if (targetHash == generatedHash) {
            std::cout << "Contraseña encontrada: " << password << " con SALT: " << salt << std::endl;
            return true;
        }
        return false;
    }

    for (char c : CHARSET) {
        password[pos] = c;
        if (generatePasswords(password, pos + 1, targetHash, salt)) {
            return true;
        }
    }

    return false;
}

// Función para probar todas las combinaciones de SALT de longitud variable
bool crackHash(const std::string& targetHash, std::chrono::duration<double>& duration) {
    auto start_time = std::chrono::high_resolution_clock::now();

    for (size_t saltLength = 1; saltLength <= MAX_SALT_LENGTH; ++saltLength) {
        std::string salt(saltLength, CHARSET[0]); // SALT inicial de longitud 'saltLength'

        // Probar todas las combinaciones del SALT de longitud actual
        do {
            std::string password(MAX_PASSWORD_LENGTH, CHARSET[0]); // Contraseña inicial
            if (generatePasswords(password, 0, targetHash, salt)) {
                auto end_time = std::chrono::high_resolution_clock::now();
                duration = end_time - start_time;
                return true;
            }
        } while (std::next_permutation(salt.begin(), salt.end()));
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    return false;
}

int main() {
    std::ifstream inputFile("small_hashed_passwords.txt");
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
                    << " -> Descifrado en " << duration.count() << " segundos.\n";
        } else {
            logFile << "Hash: " << targetHash 
                    << " -> No descifrado (Tiempo total: " << duration.count() << " segundos).\n";
        }
    }

    logFile.close();

    std::cout << "Análisis de rendimiento guardado en performance_log.txt." << std::endl;
    return 0;
}
