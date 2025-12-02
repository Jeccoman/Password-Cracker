#include <iostream>
#include <string>
#include <chrono>
#include "hash_utils.hpp"
using namespace std;
using namespace std::chrono;

bool bruteForceFound = false;
long long bruteForceAttempts = 0;
string crackedPassword = "";

void recursiveBrute(string current, const string& charset, int maxLength, const string& target, bool useHash, HashType hashType) {
    if (bruteForceFound || current.length() > maxLength) return;

    if (!current.empty()) {
        bruteForceAttempts++;
        bool isMatch = false;

        if (useHash) {
            string hash = computeHash(current, hashType);
            isMatch = (hash == target);
        } else {
            isMatch = (current == target);
        }

        if (isMatch) {
            bruteForceFound = true;
            crackedPassword = current;
            cout << "\n[!] Password cracked: " << current << endl;
            cout << "[!] Attempts: " << bruteForceAttempts << endl;
            return;
        }
    }

    for (char c : charset) {
        recursiveBrute(current + c, charset, maxLength, target, useHash, hashType);
        if (bruteForceFound) break;
    }
}

void runBruteForce(const string& target, int maxLength, const string& charset, bool useHash) {
    bruteForceFound = false;
    bruteForceAttempts = 0;
    crackedPassword = "";

    HashType hashType = HashType::MD5;
    if (useHash) {
        hashType = detectHashType(target);
        cout << "\n[+] Hash detected: ";
        switch(hashType) {
            case HashType::MD5: cout << "MD5"; break;
            case HashType::SHA1: cout << "SHA1"; break;
            case HashType::SHA256: cout << "SHA256"; break;
            case HashType::NTLM: cout << "NTLM"; break;
            case HashType::BCRYPT: cout << "BCRYPT"; break;
        }
        cout << endl;
    }

    cout << "\n[+] Starting Brute Force Attack...\n";
    auto start = high_resolution_clock::now();

    recursiveBrute("", charset, maxLength, target, useHash, hashType);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    if (!bruteForceFound) {
        cout << "[!] Password not found in given length and charset.\n";
    }

    cout << "[!] Time taken: " << duration.count() << " seconds\n";
}
