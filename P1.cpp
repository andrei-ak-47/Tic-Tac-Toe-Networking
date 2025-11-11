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

int main(){
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    
    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in ClientAddr;
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.0.39", &ClientAddr.sin_addr);
    
    connect(ClientSocket, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
    std::cout << "CONNECTED!";
    
    bool running = true;
    char character;
    char boardStr[10] = "         ";  // Initialize with spaces
    
    recv(ClientSocket, &character, 1, 0);  // Get character (X/O)
    std::cout << "You are: " << character << '\n';
    
    char YourTurn;
    
    while(running){
        char data[100];
        
        // Receive any data from server
        int bytes = recv(ClientSocket, data, sizeof(data) - 1, 0);
        
        if(bytes > 0){
            data[bytes] = '\0';
            
            // DEBUG: Show what we received
            std::cout << "DEBUG: Received " << bytes << " bytes: [";
            for(int i = 0; i < bytes && i < 20; i++){
                if(data[i] == ' ') std::cout << '_';
                else std::cout << data[i];
            }
            std::cout << "]\n";
            
            // Check if this is BOARD data (18 bytes: 9 chars + 9 spaces)
            if(bytes == 18) {
                // Extract board characters (skip the spaces)
                int boardIndex = 0;
                for(int i = 0; i < 18 && boardIndex < 9; i += 2) {
                    boardStr[boardIndex++] = data[i];
                }
                boardStr[9] = '\0';
                std::cout << "UPDATED BOARD:\n";
                DisplayBoard(boardStr);
            }
            // Check if this is TURN signal (1 byte)
            else if(bytes == 1) {
                YourTurn = data[0];
                if(YourTurn == '1'){
                    std::cout << "YOUR TURN! Enter move (1-9): ";
                    char ChosenSquare;
                    std::cin >> ChosenSquare;
                    send(ClientSocket, &ChosenSquare, 1, 0);
                    std::cout << "Sent move: " << ChosenSquare << "\n";
                }
                else if(YourTurn == '0'){
                    std::cout << "Waiting for opponent...\n";
                }
            }
            // Check for game over message
            else if(bytes > 10 && std::string(data).find("GAME OVER") != std::string::npos) {
                DisplayBoard(boardStr);
                std::cout << data << '\n';
                running = false;
            }
        }
    }
    
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}