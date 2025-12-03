#ifndef ONLINE_ATTACKS_HPP
#define ONLINE_ATTACKS_HPP

#include <string>
#include <vector>

struct Credential {
    std::string username;
    std::string password;
};

// HTTP Login Brute Force
void runHTTPBruteForce(
    const std::string& url,
    const std::string& username,
    const std::string& wordlist,
    const std::string& usernameField = "username",
    const std::string& passwordField = "password",
    int delayMs = 1000
);

// Credential Stuffing
void runCredentialStuffing(
    const std::string& url,
    const std::string& credentialFile,
    const std::string& proxyUrl = "",
    int delayMs = 1000
);

// Password Spraying
void runPasswordSpraying(
    const std::string& url,
    const std::string& userlistFile,
    const std::string& password,
    int delayMs = 5000
);

// FTP Brute Force
void runFTPBruteForce(
    const std::string& host,
    int port,
    const std::string& username,
    const std::string& wordlist,
    int delayMs = 1000
);

#endif
