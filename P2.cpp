#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#pragma comment(lib, "ws2_32.lib")

void DisplayBoard(char boardStr[10]){
    std::cout << "1.     |2.     |3.     \n";
    std::cout << "   " << boardStr[0] << "   |   " << boardStr[1] << "   |   " << boardStr[2] << "    \n";
    std::cout << "_______|_______|_______\n";
    std::cout << "4.     |5.     |6.     \n";
    std::cout << "   " << boardStr[3] << "   |   " << boardStr[4] << "   |   " << boardStr[5] << "    \n";
    std::cout << "_______|_______|_______\n";
    std::cout << "7.     |8.     |9.     \n";
    std::cout << "   " << boardStr[6] << "   |   " << boardStr[7] << "   |   " << boardStr[8] << "    \n";
    std::cout << "       |       |       \n";
}

int main(int argc, char* argv[]){
    // Handle command line arguments
    std::string serverIP = "127.0.0.1";  // Default to localhost
    
    if(argc > 1) {
        serverIP = argv[1];  // Use first command line argument as IP
        std::cout << "Connecting to server: " << serverIP << "\n";
    } else {
        // No IP provided - ask user
        std::cout << "No server IP provided.\n";
        std::cout << "Enter server IP (default: 127.0.0.1): ";
        std::string input;
        std::getline(std::cin, input);
        if(!input.empty()) {
            serverIP = input;
        }
        std::cout << "Connecting to: " << serverIP << "\n";
    }
    
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    
    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in ClientAddr;
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, serverIP.c_str(), &ClientAddr.sin_addr);  // Use dynamic IP
    
    // Rest of your existing client code...
    connect(ClientSocket, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
    
    bool running = true;
    char character;
    char boardStr[10] = "         ";
    
    recv(ClientSocket, &character, 1, 0);
    std::cout << "You are: " << character << '\n';
    
    char YourTurn;
    
    while(running){
        char data[100];
        int bytes = recv(ClientSocket, data, sizeof(data) - 1, 0);
        
        if(bytes > 0){
            data[bytes] = '\0';
            
            if(bytes == 18) {
                // Board data
                int boardIndex = 0;
                for(int i = 0; i < 18 && boardIndex < 9; i += 2) {
                    boardStr[boardIndex++] = data[i];
                }
                boardStr[9] = '\0';
                //std::cout << "🔄 UPDATED BOARD:\n";
                DisplayBoard(boardStr);
            }
            else if(bytes == 1) {
                // Turn signal
                YourTurn = data[0];
                if(YourTurn == '1'){
                    std::cout << "🎯 YOUR TURN! Enter move (1-9): ";
                    char ChosenSquare;
                    std::cin >> ChosenSquare;
                    send(ClientSocket, &ChosenSquare, 1, 0);
                }
                else if(YourTurn == '0'){
                    std::cout << "⏳ Waiting for opponent...\n";
                }
            }
            else if(bytes > 10 && std::string(data).find("GAME OVER") != std::string::npos) {
                std::cout << data << '\n';
                running = false;
            }
        }
    }
    
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}