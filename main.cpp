#include <iostream>
#include <vector>
#include "headers/menu-commands.h"
#include "headers/execute-commands.h"
#include "headers/file-system.h"
#include "headers/cryptography.h"
#include "headers/string-utilities.h"

void initializeFileSystem() {
    std::filesystem::create_directories("public_keys");
    std::filesystem::create_directories("filesystem/");
}

bool authenticateUser(const std::string &username) {
    std::string publicKeyPath="public_keys/" + username + "_pub.pem";
    if (!std::filesystem::exists(publicKeyPath)){
        return false;
    } else {
        std::string message = "Hello";
        std::string result;
        if (decrypt_ciphertext(encrypt_plaintext(message, username), username) == message) {
            return true;
        }
    }
    return false;
}

void processCommand(const std::string& command, Filesystem &fs, bool _isAdmin, const std::string &username) {
    if(command == "exit"){
        std::cout << "Exiting... Thank you!" << std::endl;
        exit(1);
    } else {
        fs.processUserCommand(command, _isAdmin, username);
    }
}

bool isAdmin(const std::string& username) {
    return username.find("admin") != std::string::npos;
}

int main(int argc, char** argv) {
    if(argc != 2){
        std::cerr << "Usage: ./fileserver <username_keyfile>" << std::endl;
        return 1;
    }

    std::string username_keyfile=argv[1];
    std::string username = splitText(username_keyfile,'_')[0];
    bool adminStatus = isAdmin(username);

    initializeFileSystem();

    if(!authenticateUser(username)) {
        if(adminStatus) {
            Filesystem fs(username,adminStatus,1);
            Filesystem::addUser(username,adminStatus);
        } else {
            std::cerr << "Authentication failed" << std::endl;
            return 1;
        }
    } else {
        // If the user is successfully authenticated:
        Filesystem fs(username, adminStatus);
        // Rest of the code to interact with the file system
        std::cout << "Logged in as " + username<<std::endl;
        std::cout << std::endl;
        available_commands(adminStatus);

        std::string command;
        while(true) {
            std::cout << username << "@" << fs.getCurrentWorkingDirectory();
            std::getline(std::cin,command);
            processCommand(command, fs , adminStatus, username);
        }
    }
}
