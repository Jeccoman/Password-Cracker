#include "rainbow_table.hpp"
#include "hash_utils.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>

RainbowTable::RainbowTable(HashType type, int chainLen, int chains)
    : hashType(type), chainLength(chainLen), numChains(chains) {}

std::string RainbowTable::reduce(const std::string& hash, int round, const std::string& charset) {
    unsigned long long value = 0;

    for (size_t i = 0; i < std::min(hash.length(), size_t(8)); i++) {
        value = (value * 256 + static_cast<unsigned char>(hash[i])) ^ round;
    }

    std::string password;
    int len = (value % 4) + 3;

    for (int i = 0; i < len; i++) {
        password += charset[value % charset.length()];
        value /= charset.length();
    }

    return password;
}

std::string RainbowTable::generateChain(const std::string& startPassword, const std::string& charset) {
    std::string current = startPassword;

    for (int i = 0; i < chainLength; i++) {
        std::string hash = computeHash(current, hashType);
        current = reduce(hash, i, charset);
    }

    return current;
}

void RainbowTable::build(const std::string& charset, int maxLen) {
    std::cout << "[+] Building rainbow table...\n";
    std::cout << "[+] Chain length: " << chainLength << "\n";
    std::cout << "[+] Number of chains: " << numChains << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> lenDist(3, maxLen);
    std::uniform_int_distribution<> charDist(0, charset.length() - 1);

    for (int chain = 0; chain < numChains; chain++) {
        int pwLen = lenDist(gen);
        std::string startPassword;

        for (int i = 0; i < pwLen; i++) {
            startPassword += charset[charDist(gen)];
        }

        std::string endPassword = generateChain(startPassword, charset);
        table[endPassword] = startPassword;

        if ((chain + 1) % 1000 == 0) {
            std::cout << "[+] Generated " << (chain + 1) << " chains...\n";
        }
    }

    std::cout << "[+] Rainbow table built with " << table.size() << " chains\n";
}

void RainbowTable::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[!] Failed to save rainbow table to: " << filename << std::endl;
        return;
    }

    int type = static_cast<int>(hashType);
    file.write(reinterpret_cast<char*>(&type), sizeof(type));
    file.write(reinterpret_cast<char*>(&chainLength), sizeof(chainLength));
    file.write(reinterpret_cast<char*>(&numChains), sizeof(numChains));

    size_t tableSize = table.size();
    file.write(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));

    for (const auto& entry : table) {
        size_t keyLen = entry.first.length();
        size_t valueLen = entry.second.length();

        file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        file.write(entry.first.c_str(), keyLen);

        file.write(reinterpret_cast<const char*>(&valueLen), sizeof(valueLen));
        file.write(entry.second.c_str(), valueLen);
    }

    file.close();
    std::cout << "[+] Rainbow table saved to: " << filename << std::endl;
}

void RainbowTable::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[!] Failed to load rainbow table from: " << filename << std::endl;
        return;
    }

    int type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));
    hashType = static_cast<HashType>(type);

    file.read(reinterpret_cast<char*>(&chainLength), sizeof(chainLength));
    file.read(reinterpret_cast<char*>(&numChains), sizeof(numChains));

    size_t tableSize;
    file.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));

    table.clear();

    for (size_t i = 0; i < tableSize; i++) {
        size_t keyLen, valueLen;

        file.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string key(keyLen, '\0');
        file.read(&key[0], keyLen);

        file.read(reinterpret_cast<char*>(&valueLen), sizeof(valueLen));
        std::string value(valueLen, '\0');
        file.read(&value[0], valueLen);

        table[key] = value;
    }

    file.close();
    std::cout << "[+] Rainbow table loaded with " << table.size() << " chains\n";
}

std::string RainbowTable::lookup(const std::string& hash, const std::string& charset, int maxLen) {
    for (int i = chainLength - 1; i >= 0; i--) {
        std::string current = hash;

        for (int j = i; j < chainLength; j++) {
            current = reduce(current, j, charset);
            current = computeHash(current, hashType);
        }

        current = reduce(current, chainLength - 1, charset);

        if (table.find(current) != table.end()) {
            std::string chainStart = table[current];
            std::string candidate = chainStart;

            for (int k = 0; k <= i; k++) {
                std::string candidateHash = computeHash(candidate, hashType);

                if (candidateHash == hash) {
                    return candidate;
                }

                candidate = reduce(candidateHash, k, charset);
            }
        }
    }

    return "";
}
