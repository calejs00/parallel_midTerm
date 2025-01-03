#include <gtk/gtk.h>
#include <omp.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <iostream>
#include <vector>

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t FIXED_LENGTH = 8;

GtkWidget *entry_password;
GtkWidget *entry_salt;
GtkWidget *label_hash;
GtkWidget *label_status;
GtkWidget *window_main;
GtkWidget *window_threads;
std::vector<GtkWidget*> thread_labels;

bool found = false;
std::string found_password;

// Función para generar hashes
std::string generate_hash(const std::string &password, const std::string &salt) {
    return crypt(password.c_str(), salt.c_str());
}

// Función de descifrado para cada hilo
void crack_password(const std::string &target_hash, const std::string &salt, int thread_id) {
    long attempts = 0;
    std::string password(FIXED_LENGTH, 'a');

    while (!found) {
        for (char &c : password) {
            c = CHARSET[attempts % CHARSET.size()];
            attempts++;

            std::string hash = generate_hash(password, salt);
            gtk_label_set_text(GTK_LABEL(thread_labels[thread_id]), ("Thread " + std::to_string(thread_id) + ": " + password).c_str());

            if (hash == target_hash) {
                found = true;
                found_password = password;
                break;
            }
        }

        if (attempts >= CHARSET.size()) {
            break;
        }
    }

    gtk_label_set_text(GTK_LABEL(thread_labels[thread_id]), ("Thread " + std::to_string(thread_id) + " stopped.").c_str());
}

// Callback para el botón de inicio
extern "C" void on_button_start_clicked(GtkButton *button, gpointer user_data) {
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const char *salt = gtk_entry_get_text(GTK_ENTRY(entry_salt));

    if (strlen(salt) != 2) {
        gtk_label_set_text(GTK_LABEL(label_status), "Salt must be exactly 2 characters.");
        return;
    }

    std::string target_hash = generate_hash(password, salt);
    gtk_label_set_text(GTK_LABEL(label_hash), ("Generated Hash: " + target_hash).c_str());

    gtk_widget_show_all(window_threads);

    found = false;
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        threads.emplace_back(crack_password, target_hash, salt, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    gtk_label_set_text(GTK_LABEL(label_status), ("Password Found: " + found_password).c_str());
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Cargar la interfaz desde el archivo Glade
    GtkBuilder *builder = gtk_builder_new_from_file("interface.glade");

    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    window_threads = GTK_WIDGET(gtk_builder_get_object(builder, "window_threads"));
    entry_password = GTK_WIDGET(gtk_builder_get_object(builder, "entry_password"));
    entry_salt = GTK_WIDGET(gtk_builder_get_object(builder, "entry_salt"));
    label_hash = GTK_WIDGET(gtk_builder_get_object(builder, "label_hash"));
    label_status = GTK_WIDGET(gtk_builder_get_object(builder, "label_status"));

    for (int i = 0; i < 4; i++) {
        std::string label_name = "thread_label_" + std::to_string(i);
        thread_labels.push_back(GTK_WIDGET(gtk_builder_get_object(builder, label_name.c_str())));
    }

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);

    gtk_widget_show_all(window_main);
    gtk_main();

    return 0;
}

