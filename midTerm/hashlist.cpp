#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unistd.h> // Para crypt

const std::string SALT = "1g";

int main() {
    std::ifstream inputFile("common_passwords.txt");
    std::ofstream outputFile("small_hashed_passwords.txt");

    if (!inputFile || !outputFile) {
        std::cerr << "Error al abrir los archivos." << std::endl;
        return 1;
    }

    // Leer todas las líneas del archivo en un vector
    std::vector<std::string> passwords;
    std::string password;
    while (std::getline(inputFile, password)) {
        if (!password.empty()) {
            passwords.push_back(password);
        }
    }

    inputFile.close();

    // Configurar generador de números aleatorios
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const size_t numToSelect = 100;

    // Seleccionar aleatoriamente 100 contraseñas
    std::random_shuffle(passwords.begin(), passwords.end());
    std::vector<std::string> selectedPasswords(passwords.begin(), passwords.begin() + numToSelect);

    // Encriptar las contraseñas seleccionadas
    for (const auto& pwd : selectedPasswords) {
        char* hash = crypt(pwd.c_str(), SALT.c_str());
        outputFile << hash << std::endl;
    }

    outputFile.close();

    std::cout << "Hashing completado para " << numToSelect << " contraseñas seleccionadas aleatoriamente." << std::endl;

    return 0;
}