#ifndef RAINBOW_TABLE_HPP
#define RAINBOW_TABLE_HPP

#include <string>
#include <unordered_map>
#include "hash_utils.hpp"

class RainbowTable {
private:
    std::unordered_map<std::string, std::string> table;
    HashType hashType;
    int chainLength;
    int numChains;

    std::string reduce(const std::string& hash, int round, const std::string& charset);
    std::string generateChain(const std::string& startPassword, const std::string& charset);

public:
    RainbowTable(HashType type = HashType::MD5, int chainLen = 1000, int chains = 10000);

    void build(const std::string& charset, int maxLen);
    void save(const std::string& filename);
    void load(const std::string& filename);

    std::string lookup(const std::string& hash, const std::string& charset, int maxLen);
};

#endif
