#include <QApplication>
#include "loginwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginWindow window;  // Crear instancia de la ventana
    window.show();       // Mostrar la ventana

    return app.exec();   // Ejecutar la aplicación
}
