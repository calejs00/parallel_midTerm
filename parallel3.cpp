đ#include <iostream>
#include <unistd.h> // Para crypt
#include <string>
#include <chrono>   // Para medir el tiempo
#include <omp.h>    // Para OpenMP
#include <vector>
#include <fstream>
#include <atomic>

#define CHUNK_SIZE 5000
const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t FIXED_LENGTH = 8;

// Variable compartida para detener todos los hilos cuando se encuentra la contraseña
std::atomic<bool> found(false);

// Función para cargar una lista de contraseñas comunes desde un archivo
std::vector<std::string> loadCommonPasswords(const std::string& filename) {
    std::vector<std::string> passwords;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            passwords.push_back(line);
        }
        file.close();
    }
    return passwords;
}

// Función para generar contraseñas y compararlas con el hash objetivo
void generatePasswords(const std::string& target_hash, const std::string& salt, long& total_attempts, std::atomic<bool>& found) {
    size_t total_combinations = 1;
    for (size_t i = 0; i < FIXED_LENGTH; ++i) {
        total_combinations *= CHARSET.size();
    }

    #pragma omp parallel
    {
        size_t thread_id = omp_get_thread_num();
        size_t num_threads = omp_get_num_threads();

        for (size_t idx = thread_id; idx < total_combinations && !found.load(); idx += num_threads) {
            std::string password(FIXED_LENGTH, CHARSET[0]);
            size_t temp_idx = idx;
            for (size_t pos = 0; pos < FIXED_LENGTH; ++pos) {
                password[pos] = CHARSET[temp_idx % CHARSET.size()];
                temp_idx /= CHARSET.size();
            }

            // Mostrar el progreso de cada hilo
            #pragma omp critical
            {
                std::cout << "thread" << thread_id << ": " << password << std::endl;
            }

            // Comparar con el hash objetivo
            std::string hashed = crypt(password.c_str(), salt.c_str());
            #pragma omp atomic
            total_attempts++;
            if (hashed == target_hash) {
                found.store(true);
                #pragma omp critical
                {
                    std::cout << "Thread " << thread_id << " found the password: " << password << std::endl;
                }
                break;
            }
        }
    }
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
    bool found_common = false;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (const auto& common_pass : common_passwords) {
        std::string hash = crypt(common_pass.c_str(), salt.c_str());
        total_attempts++;

        if (hash == target_hash) {
            std::cout << "Password found in common list: " << common_pass << "\n";
            found_common = true;
            break;
        }
    }

    if (!found_common) {
        generatePasswords(target_hash, salt, total_attempts, found);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    if (!found.load()) {
        std::cout << "Password not found within the given constraints." << std::endl;
    }

    std::cout << "Total attempts: " << total_attempts << "\n";
    std::cout << "Execution time: " << elapsed_time.count() << " seconds." << std::endl;

    return 0;
}

//Para compilar g++ -fopenmp -O3 -march=native -o optimized3_password_cracker parallel3.cpp -lcrypt
