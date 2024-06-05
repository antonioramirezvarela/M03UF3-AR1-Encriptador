#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

// Clave de encriptación simétrica
const std::string KEY = "my_secret_key";

// Función para encriptar/desencriptar un mensaje
std::string encryptDecrypt(const std::string& message) {
    std::string result = message;
    for (size_t i = 0; i < message.size(); ++i) {
        result[i] = message[i] ^ KEY[i % KEY.size()];
    }
    return result;
}

// Función simple para calcular un "checksum" usando una suma de caracteres
std::string calculateChecksum(const std::string& data) {
    unsigned long long sum = 0;
    for (char c : data) {
        sum += static_cast<unsigned char>(c);
    }
    std::ostringstream checksum;
    checksum << std::hex << sum;
    return checksum.str();
}

// Función para leer el archivo de mensajes
bool readMessagesFromFile(const std::string& filename, std::vector<std::string>& messages, std::string& storedChecksum) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    // Leer el checksum almacenado
    std::getline(file, storedChecksum);

    //Leer y desencriptar los mensajes
    std::string encryptedMessage;
    while (std::getline(file, encryptedMessage)) {
        messages.push_back(encryptDecrypt(encryptedMessage));
    }
    file.close();
    return true;
}

// Función para escribir el archivo de mensajes
void writeMessagesToFile(const std::string& filename, const std::vector<std::string>& messages) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo para escribir." << std::endl;
        return;
    }

    std::ostringstream dataStream;
    for (const auto& message : messages) {
        dataStream << encryptDecrypt(message) << "\n";
    }
    std::string data = dataStream.str();
    std::string checksum = calculateChecksum(data);

    // Escribir el checksum y los mensajes encriptados en el archivo
    file << checksum << "\n" << data;

    file.close();
}

int main() {
    const std::string filename = "messages.txt";
    std::vector<std::string> messages;
    std::string storedChecksum;
    bool fileExists = readMessagesFromFile(filename, messages, storedChecksum);

    if (fileExists) {
        std::string choice;
        std::cout << "Historial de mensajes encontrado. ¿Desea mostrar los mensajes anteriores? (s/n): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpiar el buffer de entrada

        if (choice == "s" || choice == "S") {
            std::string currentData;
            for (const auto& msg : messages) {
                currentData += msg + "\n";
            }
            std::string currentChecksum = calculateChecksum(currentData);
            if (currentChecksum != storedChecksum) {
                std::cout << "Advertencia: El archivo ha sido alterado." << std::endl;
            }

            std::cout << "Mensajes anteriores:\n";
            for (const auto& msg : messages) {
                std::cout << msg << std::endl;
            }
        }
        else {
            messages.clear(); // Descartar los mensajes anteriores
        }
    }

    std::cout << "Ingrese sus mensajes. Escriba 'exit' para finalizar y guardar los mensajes." << std::endl;
    std::string inputMessage;
    while (true) {
        std::getline(std::cin, inputMessage);
        if (inputMessage == "exit") {
            break;
        }
        messages.push_back(inputMessage);
    }

    // Guardar los mensajes en el archivo
    writeMessagesToFile(filename, messages);

    return 0;
}