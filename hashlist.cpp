#include <iostream>
#include <fstream>
#include <unistd.h> // Para crypt

const std::string SALT = "$1$abcdefgh";

int main() {
    std::ifstream inputFile("rockyou.txt");
    std::ofstream outputFile("hashed_passwords.txt");

    if (!inputFile || !outputFile) {
        std::cerr << "Error al abrir los archivos." << std::endl;
        return 1;
    }

    std::string password;
    while (std::getline(inputFile, password)) {
        if (!password.empty()) {
            char* hash = crypt(password.c_str(), SALT.c_str());
            outputFile << password << ":" << hash << std::endl;
        }
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Hashing completado. Resultados guardados en hashed_passwords.txt." << std::endl;
    return 0;
}
