#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <ctime>

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

// Mock function to simulate hashing (replace this with the actual Moodle implementation)
std::string mockHash(const std::string& input) {
    std::string hash;
    for (char c : input) {
        hash += (c + 3) % 127; // A basic hash simulation (not secure)
    }
    return hash;
}

// Load database from rockyou.txt
std::unordered_map<std::string, std::string> loadDatabase(const std::string& filename) {
    std::unordered_map<std::string, std::string> database;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return database;
    }

    std::string password;
    while (std::getline(file, password)) {
        if (!password.empty()) {
            database[password] = mockHash(password);
        }
    }

    file.close();
    return database;
}

// Check if the password is in the database
bool checkDatabase(const std::string& targetHash, const std::unordered_map<std::string, std::string>& database) {
    for (const auto& entry : database) {
        if (entry.second == targetHash) {
            std::cout << "Password found in database: " << entry.first << std::endl;
            return true;
        }
    }
    return false;
}

// Brute force password decryption
void bruteForceDecrypt(const std::string& targetHash, int maxPasswordLength) {
    std::cout << "Starting brute-force attack on hash." << std::endl;

    for (int length = 1; length <= maxPasswordLength; ++length) { // Try different lengths
        size_t totalCombinations = std::pow(CHARSET.size(), length);

        for (size_t attempt = 0; attempt < totalCombinations; ++attempt) {
            // Generate the current password
            std::string candidate;
            size_t temp = attempt;
            for (int i = 0; i < length; ++i) {
                candidate += CHARSET[temp % CHARSET.size()];
                temp /= CHARSET.size();
            }

            // Debugging progress
            if (attempt % 1000000 == 0) {
                std::cout << "Length " << length << ", Attempt " << attempt << "/" << totalCombinations << "... still working." << std::endl;
            }

            // Check if the hash matches
            if (mockHash(candidate) == targetHash) {
                std::cout << "Password found: " << candidate << std::endl;
                return;
            }
        }
    }

    std::cout << "Password not found in the given search space." << std::endl;
}

int main() {
    std::string targetPassword;
    std::cout << "Enter the target password: ";
    std::cin >> targetPassword;

    std::string targetHash = mockHash(targetPassword);

    // Load database from rockyou.txt
    std::unordered_map<std::string, std::string> passwordDatabase = loadDatabase("rockyou.txt");

    // Check database first
    if (!checkDatabase(targetHash, passwordDatabase)) {
        // Start brute force if not found in database
        clock_t start = clock();
        bruteForceDecrypt(targetHash, 10); // Test up to 10-character passwords
        clock_t end = clock();

        std::cout << "Time taken: " << (double)(end - start) / CLOCKS_PER_SEC << " seconds." << std::endl;
    } else {
        std::cout << "Password found in the database, brute force skipped." << std::endl;
    }

    return 0;
}
