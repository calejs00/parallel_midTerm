#include <iostream>
#include <unistd.h> // Para crypt
#include <string>
#include <chrono>   // Para medir el tiempo
#include <omp.h>    // Para OpenMP
#include <vector>
#include <fstream>

#define CHUNK_SIZE 5000
const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t FIXED_LENGTH = 8;

// Función para generar contraseñas y compararlas con el hash objetivo
bool generatePasswords(const std::string& prefix, const std::string& target_hash, const std::string& salt, long& attempts, bool& found) {
    if (found) return true;

    if (prefix.length() == FIXED_LENGTH) {
        std::string hash = crypt(prefix.c_str(), salt.c_str());
        #pragma omp atomic
        attempts++;

        if (hash == target_hash) {
            #pragma omp critical
            found = true;

            std::cout << "Password found: " << prefix << "\n";
            return true;
        }
        return false;
    }

    for (char c : CHARSET) {
        if (generatePasswords(prefix + c, target_hash, salt, attempts, found))
            return true;
    }

    return false;
}

// Función para leer contraseñas comunes de un archivo
std::vector<std::string> loadCommonPasswords(const std::string& filepath) {
    std::vector<std::string> passwords;
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        if (line.length() == FIXED_LENGTH) {
            passwords.push_back(line);
        }
    }
    file.close();
    return passwords;
}

int main() {
    std::string password, salt;

    std::cout << "Enter the password to hash: ";
    std::cin >> password;

    std::cout << "Enter the salt (2 characters): ";
    std::cin >> salt;

    if (salt.length() != 2) {
        std::cerr << "Salt must be exactly 2 characters long." << std::endl;
        return 1;
    }

    // Generar el hash de la contraseña introducida
    std::string target_hash = crypt(password.c_str(), salt.c_str());

    std::cout << "Generated hash: " << target_hash << "\n";

    // Intentar primero con una lista de contraseñas comunes
    std::vector<std::string> common_passwords = loadCommonPasswords("common_passwords.txt");
    long total_attempts = 0;
    bool found = false;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (const auto& common_pass : common_passwords) {
        std::string hash = crypt(common_pass.c_str(), salt.c_str());
        total_attempts++;

        if (hash == target_hash) {
            std::cout << "Password found in common list: " << common_pass << "\n";
            found = true;
            break;
        }
    }

    if (!found) {
        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic, CHUNK_SIZE)
            for (char c : CHARSET) {
                if (!found) {
                    generatePasswords(std::string(1, c), target_hash, salt, total_attempts, found);
                }
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    if (!found) {
        std::cout << "Password not found within the given constraints." << std::endl;
    }

    std::cout << "Total attempts: " << total_attempts << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " seconds" << std::endl;

    return 0;
}


//Para compilar g++ -fopenmp -O3 -march=native -o optimized2_password_cracker parallel2.cpp -lcrypt

