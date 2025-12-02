#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cctype>
#include "hash_utils.hpp"
using namespace std;
using namespace std::chrono;

string capitalize(const string& word) {
    if (word.empty()) return word;
    string result = word;
    result[0] = toupper(result[0]);
    return result;
}

string allUpperCase(const string& word) {
    string result = word;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string allLowerCase(const string& word) {
    string result = word;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string toLeetSpeak(const string& word) {
    string result = word;
    for (size_t i = 0; i < result.length(); i++) {
        char c = tolower(result[i]);
        switch(c) {
            case 'a': result[i] = '4'; break;
            case 'e': result[i] = '3'; break;
            case 'i': result[i] = '1'; break;
            case 'o': result[i] = '0'; break;
            case 's': result[i] = '5'; break;
            case 't': result[i] = '7'; break;
            case 'l': result[i] = '1'; break;
            case 'g': result[i] = '9'; break;
            case 'b': result[i] = '8'; break;
        }
    }
    return result;
}

string reverseString(const string& word) {
    string result = word;
    reverse(result.begin(), result.end());
    return result;
}

vector<string> applyRules(const string& word) {
    vector<string> variations;

    variations.push_back(word);
    variations.push_back(capitalize(word));
    variations.push_back(allUpperCase(word));
    variations.push_back(allLowerCase(word));
    variations.push_back(toLeetSpeak(word));
    variations.push_back(capitalize(toLeetSpeak(word)));
    variations.push_back(reverseString(word));
    variations.push_back(capitalize(reverseString(word)));

    for (const string& base : vector<string>{word, capitalize(word), toLeetSpeak(word)}) {
        variations.push_back(base + "!");
        variations.push_back(base + "!!");
        variations.push_back(base + "123");
        variations.push_back(base + "1");
        variations.push_back(base + "12");
        variations.push_back("!" + base);
    }

    return variations;
}

void runRuleBasedAttack(const string& target, const string& dictPath, bool useHash) {
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

    cout << "\n[+] Starting Rule-Based Attack...\n";
    cout << "[+] Applying rules: capitalization, leet speak, reverse, common suffixes\n";

    long long attempts = 0;
    bool found = false;
    string foundPassword = "";
    string word;

    auto start = high_resolution_clock::now();

    while (getline(dictFile, word) && !found) {
        vector<string> variations = applyRules(word);

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
                cout << "[!] Base word: " << word << endl;
                cout << "[!] Attempts: " << attempts << endl;
                break;
            }
        }
    }

    dictFile.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    if (!found) {
        cout << "[!] Password not found with rule-based attack.\n";
    }

    cout << "[!] Time taken: " << duration.count() << " seconds\n";
}
