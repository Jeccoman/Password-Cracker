#include <iostream>
#include <string>
#include <chrono>
#include "hash_utils.hpp"
#include "rainbow_table.hpp"
using namespace std;
using namespace std::chrono;

void runRainbowAttack(const string& targetHash, const string& tablePath, const string& charset, int maxLen) {
    HashType hashType = detectHashType(targetHash);

    cout << "\n[+] Hash detected: ";
    switch(hashType) {
        case HashType::MD5: cout << "MD5"; break;
        case HashType::SHA1: cout << "SHA1"; break;
        case HashType::SHA256: cout << "SHA256"; break;
        case HashType::NTLM: cout << "NTLM"; break;
        case HashType::BCRYPT: cout << "BCRYPT"; break;
    }
    cout << endl;

    RainbowTable table(hashType);

    cout << "[+] Loading rainbow table from: " << tablePath << endl;
    table.load(tablePath);

    cout << "\n[+] Starting Rainbow Table Lookup...\n";

    auto start = high_resolution_clock::now();

    string password = table.lookup(targetHash, charset, maxLen);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    if (!password.empty()) {
        cout << "\n[!] Password cracked: " << password << endl;
        cout << "[!] Time taken: " << duration.count() << " ms\n";
    } else {
        cout << "[!] Password not found in rainbow table.\n";
        cout << "[!] Time taken: " << duration.count() << " ms\n";
    }
}

void runRainbowTableGeneration(const string& outputPath, const string& charset, int maxLen, int chainLength, int numChains, HashType hashType) {
    cout << "\n[+] Generating Rainbow Table...\n";
    cout << "[+] Hash type: ";
    switch(hashType) {
        case HashType::MD5: cout << "MD5"; break;
        case HashType::SHA1: cout << "SHA1"; break;
        case HashType::SHA256: cout << "SHA256"; break;
        case HashType::NTLM: cout << "NTLM"; break;
        case HashType::BCRYPT: cout << "BCRYPT"; break;
    }
    cout << endl;
    cout << "[+] Charset: " << charset << endl;
    cout << "[+] Max password length: " << maxLen << endl;

    auto start = high_resolution_clock::now();

    RainbowTable table(hashType, chainLength, numChains);
    table.build(charset, maxLen);
    table.save(outputPath);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    cout << "\n[+] Rainbow table generation complete!\n";
    cout << "[!] Time taken: " << duration.count() << " seconds\n";
}
