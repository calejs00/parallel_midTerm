#include <iostream>
#include <unistd.h> // Para crypt
#include <string>
#include <chrono> // Para medir el tiempo

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyz0123456789./";

// Función para generar y probar contraseñas
bool generatePasswords(std::string& password, int pos, const std::string& targetHash) {
    if (pos == password.size()) {
        // Generar el hash de la contraseña actual
        char* hash = crypt(password.c_str(), "$1$abcdefgh");
        if (hash == targetHash) { // Comparar el hash generado con el hash objetivo
            std::cout << "Contraseña encontrada: " << password << std::endl;
            return true;
        }
        return false;
    }

    for (size_t i = 0; i < CHARSET.size(); ++i) {
        password[pos] = CHARSET[i];
        if (generatePasswords(password, pos + 1, targetHash)) {
            return true; // Si se encuentra la contraseña, detener la búsqueda
        }
    }

    return false;
}

int main() {
    std::string inputPassword;
    std::cout << "Introduce la contraseña a encriptar (8 caracteres): ";
    std::cin >> inputPassword;

    // Generar el hash de la contraseña introducida
    if (inputPassword.size() > 8) {
        std::cerr << "Error: La contraseña no puede tener más de 8 caracteres." << std::endl;
        return 1;
    }

    if (inputPassword.size() < 8) {
        std::cerr << "Error: La contraseña no puede tener menos de 8 caracteres." << std::endl;
        return 1;
    }

    std::string targetHash = crypt(inputPassword.c_str(), "$1$abcdefgh");
    std::cout << "Hash objetivo: " << targetHash << std::endl;

    // Preparar la búsqueda
    std::string password(inputPassword.size(), CHARSET[0]); // Contraseña inicial de la misma longitud
    auto start_time = std::chrono::high_resolution_clock::now();

    // Generar combinaciones para descifrar la contraseña
    if (!generatePasswords(password, 0, targetHash)) {
        std::cout << "No se encontró la contraseña en el espacio de búsqueda." << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Tiempo total: " << elapsed_time.count() << " segundos." << std::endl;

    return 0;
}
