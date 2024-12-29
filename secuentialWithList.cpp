#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <unistd.h>

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

// Función para generar y probar contraseñas
bool generatePasswords(std::string& password, int pos, const std::string& targetHash) {
    if (pos == password.size()) {
        char* hash = crypt(password.c_str(), "$1$abcdefgh");
        if (hash == targetHash) {
            std::cout << "Contraseña encontrada: " << password << std::endl;
            return true;
        }
        return false;
    }

    for (size_t i = 0; i < CHARSET.size(); ++i) {
        password[pos] = CHARSET[i];
        if (generatePasswords(password, pos + 1, targetHash)) {
            return true;
        }
    }

    return false;
}

int main() {
    std::ifstream inputFile("hashed_passwords.txt");
    if (!inputFile) {
        std::cerr << "Error: No se pudo abrir el archivo hashed_passwords.txt." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        size_t delimiterPos = line.find(':');
        if (delimiterPos == std::string::npos) {
            std::cerr << "Error en el formato del archivo: " << line << std::endl;
            continue;
        }

        std::string plainText = line.substr(0, delimiterPos);
        std::string targetHash = line.substr(delimiterPos + 1);

        std::cout << "Descifrando hash: " << targetHash << std::endl;

        // Preparar la búsqueda
        std::string password(8, CHARSET[0]);
        auto start_time = std::chrono::high_resolution_clock::now();

        if (!generatePasswords(password, 0, targetHash)) {
            std::cout << "No se encontró la contraseña para el hash: " << targetHash << std::endl;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = end_time - start_time;
        std::cout << "Tiempo total: " << elapsed_time.count() << " segundos." << std::endl;
    }

    inputFile.close();
    return 0;
}
