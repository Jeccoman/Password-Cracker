#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "hash_utils.hpp"
using namespace std;
using namespace std::chrono;

void runDictionaryAttack(const string& target, const string& dictPath, bool useHash) {
    ifstream dictFile(dictPath);
    if (!dictFile.is_open()) {
        cerr << "[!] Failed to open dictionary file: " << dictPath << endl;
        return;
    }

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

    cout << "\n[+] Starting Dictionary Attack...\n";
    long long attempts = 0;
    bool found = false;
    string word;

    auto start = high_resolution_clock::now();

    while (getline(dictFile, word)) {
        attempts++;
        bool isMatch = false;

        if (useHash) {
            string hash = computeHash(word, hashType);
            isMatch = (hash == target);
        } else {
            isMatch = (word == target);
        }

        if (isMatch) {
            found = true;
            cout << "\n[!] Password cracked: " << word << endl;
            cout << "[!] Attempts: " << attempts << endl;
            break;
        }
    }

    dictFile.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    if (!found) {
        cout << "[!] Password not found in dictionary.\n";
    }

    cout << "[!] Time taken: " << duration.count() << " seconds\n";
}
