#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>
void FillSquare(char c, const char* numString, std::vector<std::vector<char>>& board, std::unordered_set<int>& UsedCells) {
    int num = std::stoi(numString);
    
    // Validate move
    if(num < 1 || num > 9) return;
    if(UsedCells.find(num) != UsedCells.end()) return;
    
    UsedCells.insert(num);
    int row = (num - 1) / 3;
    int col = (num - 1) % 3;
    board[row][col] = c;
}
void DisplayBoard(std::vector<std::vector<char>>& board){
    std::cout << "1.     |2.     |3.     \n";
    std::cout << "   " << board[0][0] << "   |   " << board[0][1] << "   |   " << board[0][2] << "    \n";
    std::cout << "_______|_______|_______\n";
    std::cout << "4.     |5.     |6.     \n";
    std::cout << "   " << board[1][0] << "   |   " << board[1][1] << "   |   " << board[1][2] << "    \n";
    std::cout << "_______|_______|_______\n";
    std::cout << "7.     |8.     |9.     \n";
    std::cout << "   " << board[2][0] << "   |   " << board[2][1] << "   |   " << board[2][2] << "    \n";
    std::cout << "       |       |       \n";
}
char checkWin(const std::vector<std::vector<char>>& board) {
    // Check rows and columns
    for(int i = 0; i < 3; i++) {
        // Rows
        if(board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        // Columns  
        if(board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    
    // Diagonals
    if(board[1][1] != ' ') {  // Center must be filled for any diagonal win
        if(board[0][0] == board[1][1] && board[1][1] == board[2][2])
            return board[1][1];
        if(board[0][2] == board[1][1] && board[1][1] == board[2][0])
            return board[1][1];
    }
    
    return ' ';  // No winner
}
void SendBoardToPlayers(const std::vector<std::vector<char>>& board, SOCKET PlayerSockets[2]){
    std::string boardStr = "";
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            boardStr += board[i][j];
            boardStr += ' ';
        }
    }
    send(PlayerSockets[0],boardStr.c_str(), boardStr.size(), 0);
    send(PlayerSockets[1],boardStr.c_str(), boardStr.size(), 0);
}
int main(){
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    //Made WSA
    SOCKET MainSocket = socket(AF_INET, SOCK_STREAM, 0);
    //Server Socket created, for connecting clients
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(MainSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(MainSocket, 2);
    std::cout << "CONNECTED!\n";
    int numOfClients = 0;
    bool p1Role = static_cast<bool>(rand() % 2);
    SOCKET playerSockets[2];
    std::unordered_map<int, char> PlayerCharacter;

    while(numOfClients < 2){
        SOCKET clientSocket = accept(MainSocket, NULL, NULL);
        playerSockets[numOfClients] = clientSocket;
        numOfClients++;
    }
    if(p1Role == true){
        send(playerSockets[0], "X", 1, 0);
        send(playerSockets[1], "O", 1, 0);
        PlayerCharacter[0] = 'X';
        PlayerCharacter[1] = 'O';
    }
    else{
        send(playerSockets[0], "O", 1, 0);
        send(playerSockets[1], "X", 1, 0);
        PlayerCharacter[0] = 'O';
        PlayerCharacter[1] = 'X';
    }
    std::cout << "Both Players Connected...\n";

    // ===BOTH PLAYERS CONNECTED AND READY TO PLAY NOW===
    //              ===GAME LOOP BELOW===

    std::vector<std::vector<char>> board(3, std::vector<char>(3, ' '));
    std::unordered_set<int> UsedCells;
    bool running = true;
    char winner = ' ';
    char CurrentPlayer = 'X';// X goes first
    //std::cout << "SENT INITIAL BOARD\n";
    //SendBoardToPlayers(board, playerSockets);
    //std::cout << "BOARD SENT\n";
    Sleep(2000);
    std::cout << "STARTING\n";
    while(running){
        DisplayBoard(board);
        int CurrentPlayersIndex = (CurrentPlayer == 'X') ? 0 : 1;
        int OppPlayerIndex = (CurrentPlayersIndex == 1) ? 0 : 1;
        SendBoardToPlayers(board, playerSockets);
        Sleep(200);
        //Send Curr and Opp Players if theyre allowed to go or not
        char turnSignal = '1';
        send(playerSockets[CurrentPlayersIndex], &turnSignal, 1, 0);
        std::cout << "Sent Player " << CurrentPlayersIndex + 1 << ": " << turnSignal << " (Turn Signal)\n";
        turnSignal = '0';  
        send(playerSockets[OppPlayerIndex], &turnSignal, 1, 0);
        std::cout << "Sent Player " << OppPlayerIndex + 1 << ": " << turnSignal << " (Turn Signal)\n";
        Sleep(150);
        std::cout << "Waiting for player " << CurrentPlayersIndex + 1 << "s move...\n";
        char buffer[10];
        int bytes = recv(playerSockets[CurrentPlayersIndex], buffer, sizeof(buffer) - 1, 0);
        if(bytes > 0){
            std::cout << "We got here...\n";
            buffer[bytes] = '\0';
            std::cout << buffer << '\n';
            FillSquare(PlayerCharacter[CurrentPlayersIndex], buffer, board, UsedCells);
            winner = checkWin(board);
            if(winner == ' '){
                if(UsedCells.size() == 9){
                    running = false;
                }
            }
            else running = false;
            CurrentPlayer = (CurrentPlayer == 'X') ? 'O' : 'X';
        }
    }
    // ===GAME ENDS, SOMEBODY WINS OR LOSSES, OR BOTH DRAW
    if(winner != ' '){
        std::string message = "GAME OVER: Player ";
        message += winner;
        message += " Wins!!\n";
        send(playerSockets[0], message.c_str(), message.size(), 0);
        send(playerSockets[1], message.c_str(), message.size(), 0);
    }
    else {
        std::string message = "GAME OVER: It's a draw!";
        send(playerSockets[0], message.c_str(), message.size(), 0);
        send(playerSockets[1], message.c_str(), message.size(), 0);
    }
    closesocket(playerSockets[0]);
    closesocket(playerSockets[1]);
    closesocket(MainSocket);
    WSACleanup();
    return 0;
}