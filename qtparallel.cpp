#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QThread>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <unistd.h>
#include <omp.h>
#include "ui_password_cracker.h"


#define CHUNK_SIZE 5000
const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
const size_t FIXED_LENGTH = 8;

public:
    PasswordCracker(QWidget *parent = nullptr) : QWidget(parent) {
        setupUi(this);  // Configura la interfaz
        connect(pushButton_start, &QPushButton::clicked, this, &PasswordCracker::startCracking);
    }
    ~PasswordCracker() override = default;


private slots:
    void startCracking();

private:
    QLineEdit *passwordInput;
    QLineEdit *saltInput;
    QLabel *hashLabel;
    QTextEdit *threadStatus;
    QLabel *statusLabel;

    bool found;
    std::string foundPassword;
    std::mutex foundMutex;

    std::string generateHash(const std::string &password, const std::string &salt);
    void crackPassword(const std::string &targetHash, const std::string &salt, int threadId, long &attempts);
    std::vector<std::string> loadCommonPasswords(const std::string &filepath);
};

PasswordCracker::PasswordCracker(QWidget *parent)
    : QWidget(parent), found(false) {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGridLayout *inputLayout = new QGridLayout();
    inputLayout->addWidget(new QLabel("Password:"), 0, 0);
    passwordInput = new QLineEdit(this);
    inputLayout->addWidget(passwordInput, 0, 1);

    inputLayout->addWidget(new QLabel("Salt (2 characters):"), 1, 0);
    saltInput = new QLineEdit(this);
    saltInput->setMaxLength(2);
    inputLayout->addWidget(saltInput, 1, 1);

    mainLayout->addLayout(inputLayout);

    hashLabel = new QLabel("Generated Hash:");
    mainLayout->addWidget(hashLabel);

    threadStatus = new QTextEdit(this);
    threadStatus->setReadOnly(true);
    mainLayout->addWidget(threadStatus);

    statusLabel = new QLabel("Status:");
    mainLayout->addWidget(statusLabel);

    QPushButton *startButton = new QPushButton("Start Cracking", this);
    connect(startButton, &QPushButton::clicked, this, &PasswordCracker::startCracking);
    mainLayout->addWidget(startButton);
}

PasswordCracker::~PasswordCracker() {
    // Limpieza (si es necesaria)
}

std::string PasswordCracker::generateHash(const std::string &password, const std::string &salt) {
    return crypt(password.c_str(), salt.c_str());
}

std::vector<std::string> PasswordCracker::loadCommonPasswords(const std::string &filepath) {
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

void PasswordCracker::crackPassword(const std::string &targetHash, const std::string &salt, int threadId, long &attempts) {
    std::string password(FIXED_LENGTH, 'a');
    password[0] = CHARSET[threadId % CHARSET.size()];

    while (!found) {
        for (size_t i = 0; i < CHARSET.size(); ++i) {
            password[1] = CHARSET[i];
            for (size_t j = 0; j < CHARSET.size(); ++j) {
                password[2] = CHARSET[j];
                std::string hash = generateHash(password, salt);

                {
                    std::lock_guard<std::mutex> lock(foundMutex);
                    if (found) return;
                    attempts++;
                }

                QMetaObject::invokeMethod(threadStatus, [=]() {
                    threadStatus->append(QString("Thread %1: Attempting %2").arg(threadId).arg(QString::fromStdString(password)));
                }, Qt::QueuedConnection);

                if (hash == targetHash) {
                    {
                        std::lock_guard<std::mutex> lock(foundMutex);
                        found = true;
                        foundPassword = password;
                    }
                    return;
                }
            }
        }
    }
}

void PasswordCracker::startCracking() {
    const std::string password = passwordInput->text().toStdString();
    const std::string salt = saltInput->text().toStdString();

    if (salt.length() != 2) {
        statusLabel->setText("Salt must be exactly 2 characters.");
        return;
    }

    if (password.length() > FIXED_LENGTH) {
        statusLabel->setText("Password length exceeds 8 characters.");
        return;
    }

    const std::string targetHash = generateHash(password, salt);
    hashLabel->setText(QString("Generated Hash: %1").arg(QString::fromStdString(targetHash)));

    found = false;
    threadStatus->clear();

    std::vector<std::string> commonPasswords = loadCommonPasswords("common_passwords.txt");

    for (const auto &commonPass : commonPasswords) {
        std::string hash = generateHash(commonPass, salt);
        if (hash == targetHash) {
            found = true;
            foundPassword = commonPass;
            statusLabel->setText(QString("Password Found: %1").arg(QString::fromStdString(foundPassword)));
            return;
        }
    }

    long totalAttempts = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(&PasswordCracker::crackPassword, this, targetHash, salt, i, std::ref(totalAttempts));
    }

    for (auto &t : threads) {
        t.join();
    }

    if (found) {
        statusLabel->setText(QString("Password Found: %1").arg(QString::fromStdString(foundPassword)));
    } else {
        statusLabel->setText("Password not found.");
    }

    statusLabel->setText(statusLabel->text() + QString(" Total Attempts: %1").arg(totalAttempts));
}

#include "moc_qtparallel.cpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PasswordCracker window;
    window.setWindowTitle("Password Cracker");
    window.show();

    return app.exec();
}

