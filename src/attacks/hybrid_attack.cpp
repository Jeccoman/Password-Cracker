#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include "hash_utils.hpp"
using namespace std;
using namespace std::chrono;

void runHybridAttack(const string& target, const string& dictPath, bool useHash, int maxDigits) {
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

    cout << "\n[+] Starting Hybrid Attack...\n";
    cout << "[+] Testing word variations with appended/prepended numbers (0-" << maxDigits << " digits)\n";

    long long attempts = 0;
    bool found = false;
    string foundPassword = "";
    vector<string> words;

    string word;
    while (getline(dictFile, word)) {
        words.push_back(word);
    }
    dictFile.close();

    auto start = high_resolution_clock::now();

    for (const string& baseWord : words) {
        if (found) break;

        for (int numDigits = 0; numDigits <= maxDigits; numDigits++) {
            if (found) break;

            int maxNum = 1;
            for (int i = 0; i < numDigits; i++) maxNum *= 10;

            for (int num = 0; num < maxNum; num++) {
                if (found) break;

                string numStr = (numDigits > 0) ? to_string(num) : "";
                while (numStr.length() < numDigits) {
                    numStr = "0" + numStr;
                }

                vector<string> variations;
                if (numDigits == 0) {
                    variations.push_back(baseWord);
                } else {
                    variations.push_back(baseWord + numStr);
                    variations.push_back(numStr + baseWord);
                }

                for (const string& candidate : variations) {
                    attempts++;
                    bool isMatch = false;

                    if (useHash) {
                        string hash = computeHash(candidate, hashType);
                        isMatch = (hash == target);
                    } else {
                        isMatch = (candidate == target);
                    }

                    if (isMatch) {
                        found = true;
                        foundPassword = candidate;
                        cout << "\n[!] Password cracked: " << candidate << endl;
                        cout << "[!] Attempts: " << attempts << endl;
                        break;
                    }
                }
            }
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    if (!found) {
        cout << "[!] Password not found with hybrid attack.\n";
    }

    cout << "[!] Time taken: " << duration.count() << " seconds\n";
}
