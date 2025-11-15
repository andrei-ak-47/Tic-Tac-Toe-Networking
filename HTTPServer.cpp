// HTTPServer.cpp - FIXED VERSION
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <sstream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

class WinnerHTTPServer {
private:
    char lastWinner;
    bool running;
    std::vector<SOCKET> socketsToClose;

public:
    WinnerHTTPServer() : lastWinner(' '), running(true) {}

    void startServer() {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "WSAStartup failed\n";
            return;
        }

        // Create both sockets
        SOCKET httpSocket = socket(AF_INET, SOCK_STREAM, 0);
        SOCKET notifySocket = socket(AF_INET, SOCK_STREAM, 0);
        
        socketsToClose.push_back(httpSocket);
        socketsToClose.push_back(notifySocket);

        // Setup HTTP server (port 8081)
        sockaddr_in httpAddr{};
        httpAddr.sin_family = AF_INET;
        httpAddr.sin_port = htons(8081);
        httpAddr.sin_addr.s_addr = INADDR_ANY;
        bind(httpSocket, (sockaddr*)&httpAddr, sizeof(httpAddr));
        listen(httpSocket, 10);

        // Setup notification listener (port 9090)  
        sockaddr_in notifyAddr{};
        notifyAddr.sin_family = AF_INET;
        notifyAddr.sin_port = htons(9090);
        notifyAddr.sin_addr.s_addr = INADDR_ANY;
        bind(notifySocket, (sockaddr*)&notifyAddr, sizeof(notifyAddr));
        listen(notifySocket, 5);

        std::cout << "HTTP Server: http://localhost:8081\n";
        std::cout << "Notification listener: port 9090\n";
        std::cout << "Server running... Press Ctrl+C to stop\n";

        fd_set readfds;
        while (running) {
            FD_ZERO(&readfds);
            FD_SET(httpSocket, &readfds);
            FD_SET(notifySocket, &readfds);
            
            // Set timeout to make accept() non-blocking
            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int result = select(0, &readfds, NULL, NULL, &timeout);
            
            if (result > 0) {
                if (FD_ISSET(httpSocket, &readfds)) {
                    SOCKET clientSocket = accept(httpSocket, NULL, NULL);
                    if (clientSocket != INVALID_SOCKET) {
                        handleHTTPRequest(clientSocket);
                        closesocket(clientSocket);
                    }
                }
                if (FD_ISSET(notifySocket, &readfds)) {
                    SOCKET clientSocket = accept(notifySocket, NULL, NULL);
                    if (clientSocket != INVALID_SOCKET) {
                        char winner;
                        int bytes = recv(clientSocket, &winner, 1, 0);
                        if (bytes > 0) {
                            lastWinner = winner;
                            std::cout << "Received winner: " << winner << "\n";
                        }
                        closesocket(clientSocket);
                    }
                }
            }
        }

        // Cleanup
        for (SOCKET sock : socketsToClose) {
            closesocket(sock);
        }
        WSACleanup();
    }

    void handleHTTPRequest(SOCKET clientSocket) {
        char buffer[1024];
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes > 0) {
            std::string response = generateHTTPResponse();
            send(clientSocket, response.c_str(), (int)response.length(), 0);
        }
    }

    std::string generateHTTPResponse() {
        std::stringstream html;
        
        html << "HTTP/1.1 200 OK\r\n";
        html << "Content-Type: text/html\r\n";
        html << "Connection: close\r\n";
        html << "\r\n";
        
        html << "<!DOCTYPE html><html><head><title>Tic-Tac-Toe Results</title>";
        html << "<style>body{font-family:Arial;text-align:center;margin:50px;background:#667eea;color:white;}";
        html << ".container{background:rgba(255,255,255,0.1);padding:30px;border-radius:15px;max-width:500px;margin:0 auto;}";
        html << "h1{font-size:2.5em;}.result{font-size:1.8em;padding:20px;border-radius:10px;}";
        html << ".x-win{background:rgba(255,87,87,0.3);}.o-win{background:rgba(87,171,255,0.3);}";
        html << ".draw{background:rgba(255,193,87,0.3);}.waiting{background:rgba(200,200,200,0.3);}";
        html << "</style></head><body><div class='container'><h1> Tic-Tac-Toe Results</h1>";
        
        char winner = lastWinner;
        
        if (winner == ' ') {
            html << "<div class='result waiting'> Waiting for game results...</div>";
        } else if (winner == 'X') {
            html << "<div class='result x-win'> Player X Wins! 🏆</div>";
        } else if (winner == 'O') {
            html << "<div class='result o-win'> Player O Wins! 🏆</div>";
        } else if (winner == 'D') {
            html << "<div class='result draw'> It's a Draw!</div>";
        }
        
        html << "<p>Page auto-refreshes every 5 seconds</p>";
        html << "<script>setTimeout(()=>location.reload(),5000)</script>";
        html << "</div></body></html>";

        return html.str();
    }

    void stop() {
        running = false;
    }
};

int main() {
    WinnerHTTPServer server;
    server.startServer();
    return 0;
}