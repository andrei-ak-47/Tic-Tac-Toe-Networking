// Server.cpp (game) - sends winner to HTTPServer on port 9090
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void FillSquare(char c, const char* numString, std::vector<std::vector<char>>& board, std::unordered_set<int>& UsedCells) {
    int num = std::stoi(numString);
    if(num < 1 || num > 9) return;
    if(UsedCells.find(num) != UsedCells.end()) return;
    UsedCells.insert(num);
    board[(num-1)/3][(num-1)%3] = c;
}

char checkWin(const std::vector<std::vector<char>>& board) {
    for(int i=0;i<3;i++){
        if(board[i][0] != ' ' && board[i][0]==board[i][1] && board[i][1]==board[i][2]) return board[i][0];
        if(board[0][i] != ' ' && board[0][i]==board[1][i] && board[1][i]==board[2][i]) return board[0][i];
    }
    if(board[1][1] != ' '){
        if(board[0][0]==board[1][1] && board[1][1]==board[2][2]) return board[1][1];
        if(board[0][2]==board[1][1] && board[1][1]==board[2][0]) return board[1][1];
    }
    return ' ';
}

void SendBoardToPlayers(const std::vector<std::vector<char>>& board, SOCKET PlayerSockets[2]){
    std::string boardStr;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++){ boardStr += board[i][j]; boardStr += ' '; }
    send(PlayerSockets[0], boardStr.c_str(), (int)boardStr.size(), 0);
    send(PlayerSockets[1], boardStr.c_str(), (int)boardStr.size(), 0);
}

void notifyHTTPServer(char winner) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed for notification\n";
        return;
    }

    SOCKET notify = socket(AF_INET, SOCK_STREAM, 0);
    if (notify == INVALID_SOCKET) {
        std::cerr << "Failed to create notification socket\n";
        WSACleanup();
        return;
    }

    sockaddr_in httpAddr{};
    httpAddr.sin_family = AF_INET;
    httpAddr.sin_port = htons(9090);
    inet_pton(AF_INET, "127.0.0.1", &httpAddr.sin_addr);

    // retry a few times if HTTP server not up yet
    bool connected = false;
    for(int attempt = 0; attempt < 10 && !connected; ++attempt) {
        if(connect(notify, (sockaddr*)&httpAddr, sizeof(httpAddr)) != SOCKET_ERROR) {
            connected = true;
            break;
        }
        std::cout << "Retrying connection to HTTP server... (" << attempt + 1 << "/10)\n";
        Sleep(500);
    }

    if(connected) {
        char out = (winner == ' ') ? 'D' : winner; // 'D' = draw
        send(notify, &out, 1, 0);
        std::cout << "Sent winner to HTTP server: " << out << "\n";
    } else {
        std::cerr << "Failed to connect to HTTP server on 127.0.0.1:9090\n";
    }

    closesocket(notify);
    WSACleanup();
}

int main(){
    WSAData wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        std::cerr << "WSAStartup failed\n"; return 1;
    }

    SOCKET MainSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(MainSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(MainSocket, 2);

    int numOfClients = 0;
    bool p1Role = static_cast<bool>(rand() % 2);
    SOCKET playerSockets[2];
    std::unordered_map<int,char> PlayerCharacter;

    while(numOfClients < 2){
        SOCKET clientSocket = accept(MainSocket, NULL, NULL);
        if(clientSocket == INVALID_SOCKET){ std::cerr << "accept failed\n"; continue; }
        playerSockets[numOfClients++] = clientSocket;
        std::cout << "Player connected (" << numOfClients << "/2)\n";
    }

    if(p1Role){
        send(playerSockets[0],"X",1,0);
        send(playerSockets[1],"O",1,0);
        PlayerCharacter[0]='X'; PlayerCharacter[1]='O';
    } else {
        send(playerSockets[0],"O",1,0);
        send(playerSockets[1],"X",1,0);
        PlayerCharacter[0]='O'; PlayerCharacter[1]='X';
    }

    std::vector<std::vector<char>> board(3, std::vector<char>(3,' '));
    std::unordered_set<int> UsedCells;
    bool running=true;
    char winner=' ';
    char CurrentPlayer='X';

    while(running){
        int CurrentPlayersIndex = (CurrentPlayer=='X')?0:1;
        int OppPlayerIndex = 1 - CurrentPlayersIndex;
        SendBoardToPlayers(board, playerSockets);
        char turnSignal='1';
        send(playerSockets[CurrentPlayersIndex], &turnSignal, 1, 0);
        turnSignal='0';
        send(playerSockets[OppPlayerIndex], &turnSignal, 1, 0);

        char buffer[16];
        int bytes = recv(playerSockets[CurrentPlayersIndex], buffer, sizeof(buffer)-1, 0);
        if(bytes > 0){
            buffer[bytes] = '\0';
            FillSquare(PlayerCharacter[CurrentPlayersIndex], buffer, board, UsedCells);
            winner = checkWin(board);
            if(winner != ' ' || UsedCells.size() == 9) running = false;
            CurrentPlayer = (CurrentPlayer=='X') ? 'O' : 'X';
        }
        Sleep(50);
    }

    std::string message = (winner != ' ') ? ("GAME OVER: Player " + std::string(1,winner) + " Wins!!\n")
                                         : "GAME OVER: It's a draw!";
    send(playerSockets[0], message.c_str(), (int)message.size(), 0);
    send(playerSockets[1], message.c_str(), (int)message.size(), 0);

    // Notify HTTP server
    notifyHTTPServer(winner);

    closesocket(playerSockets[0]);
    closesocket(playerSockets[1]);
    closesocket(MainSocket);
    WSACleanup();
    return 0;
}