#include <iostream>
#include <unistd.h> // Para crypt
#include <string>
#include <chrono>   // Para medir el tiempo
#include <omp.h>    // Para OpenMP

#define CHUNK_SIZE 100

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

// Función para generar contraseñas y compararlas con el hash objetivo
bool generatePasswords(const std::string& prefix, size_t length, const std::string& targetHash, bool& found) {
    std::string password = prefix;
    password.resize(length, CHARSET[0]); // Completar con el primer carácter

    while (!found) {
        char* hash = crypt(password.c_str(), "$1$abcdefgh");
        #pragma omp critical
        {
            std::cout << "Probing password: " << password << " -> Hash: " << hash << std::endl;
        }

        if (std::string(hash) == targetHash) {
            #pragma omp critical
            {
                if (!found) {
                    found = true;
                    std::cout << "Contraseña encontrada: " << password << std::endl;
                }
            }
            #pragma omp flush(found)
            return true;
        }

        // Incrementar la contraseña al siguiente valor
        for (int i = length - 1; i >= 0; --i) {
            if (password[i] == CHARSET.back()) {
                password[i] = CHARSET[0];
            } else {
                password[i] = CHARSET[CHARSET.find(password[i]) + 1];
                break;
            }
        }

        if (password == prefix + std::string(length - prefix.size(), CHARSET[0])) {
            break; // Volvimos al prefijo inicial
        }
    }

    return false;
}

int main() {
    std::string inputPassword;
    std::cout << "Introduce la contraseña a encriptar (máximo 8 caracteres): ";
    std::cin >> inputPassword;

    if (inputPassword.size() > 8) {
        std::cerr << "Error: La contraseña no puede tener más de 8 caracteres." << std::endl;
        return 1;
    }

    std::string targetHash = crypt(inputPassword.c_str(), "$1$abcdefgh");
    std::cout << "Hash objetivo: " << targetHash << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    bool found = false;
    size_t passwordLength = inputPassword.size();

    // Paralelización: dividir el espacio de búsqueda inicial
    #pragma omp parallel for schedule(dynamic, CHUNK_SIZE) shared(found)
    for (size_t i = 0; i < CHARSET.size(); ++i) {
        if (found) continue;

        // Prefijo inicial basado en el hilo
        std::string prefix(1, CHARSET[i]);
        generatePasswords(prefix, passwordLength, targetHash, found);
    }

    if (!found) {
        std::cout << "No se encontró la contraseña en el espacio de búsqueda." << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Tiempo total: " << elapsed_time.count() << " segundos." << std::endl;

    return 0;
}
