#ifndef HASH_UTILS_HPP
#define HASH_UTILS_HPP

#include <string>

enum class HashType {
    MD5,
    SHA1,
    SHA256,
    NTLM,
    BCRYPT
};

std::string computeHash(const std::string& input, HashType hashType);
HashType detectHashType(const std::string& hash);
std::string computeNTLM(const std::string& password);

#endif
