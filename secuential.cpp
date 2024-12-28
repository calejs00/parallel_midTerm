#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <omp.h>

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

// Genera todas las combinaciones posibles de una contraseña de longitud 8
void generatePasswords(std::string& password, int pos) {
    if (pos == password.size()) {
        // Probar la contraseña generada
        char* hash = crypt(password.c_str(), "$1$abcdefgh");
        std::cout << "Probando: " << password << " -> Hash: " << hash << std::endl;
        return;
    }

    for (size_t i = 0; i < CHARSET.size(); ++i) {
        password[pos] = CHARSET[i];
        generatePasswords(password, pos + 1);
    }
}

int main() {
    std::string targetHash = "$1$abcdefgh$";  // Aquí deberías poner el hash que estás tratando de descifrar
    std::string password(8, CHARSET[0]);  // Inicia la contraseña con 8 caracteres

    // Comienza a generar contraseñas
    double start_time = omp_get_wtime();  // Tiempo de inicio

    // Llamada a la función para generar y probar contraseñas
    generatePasswords(password, 0, targetHash);

    double end_time = omp_get_wtime();  // Tiempo de finalización
    std::cout << "Tiempo total: " << end_time - start_time << " segundos." << std::endl;

    return 0;
}