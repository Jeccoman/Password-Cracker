#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include <string>
#include "hash_utils.hpp"

void runBruteForce(const std::string& target, int maxLength, const std::string& charset, bool useHash = false);
void runDictionaryAttack(const std::string& target, const std::string& dictPath, bool useHash = false);
void runHybridAttack(const std::string& target, const std::string& dictPath, bool useHash = false, int maxDigits = 3);
void runRuleBasedAttack(const std::string& target, const std::string& dictPath, bool useHash = false);
void runRainbowAttack(const std::string& targetHash, const std::string& tablePath, const std::string& charset, int maxLen);
void runRainbowTableGeneration(const std::string& outputPath, const std::string& charset, int maxLen, int chainLength, int numChains, HashType hashType);

#endif
