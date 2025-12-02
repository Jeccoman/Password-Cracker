#include "hash_utils.hpp"
#include <openssl/md5.h>
#include <openssl/md4.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

std::string bytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; i++) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

std::string computeNTLM(const std::string& password) {
    std::string utf16le;
    for (char c : password) {
        utf16le += c;
        utf16le += '\0';
    }

    unsigned char hash[MD4_DIGEST_LENGTH];
    MD4(reinterpret_cast<const unsigned char*>(utf16le.c_str()), utf16le.length(), hash);

    return bytesToHex(hash, MD4_DIGEST_LENGTH);
}

std::string computeHash(const std::string& input, HashType hashType) {
    switch (hashType) {
        case HashType::MD5: {
            unsigned char hash[MD5_DIGEST_LENGTH];
            MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
            return bytesToHex(hash, MD5_DIGEST_LENGTH);
        }
        case HashType::SHA1: {
            unsigned char hash[SHA_DIGEST_LENGTH];
            SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
            return bytesToHex(hash, SHA_DIGEST_LENGTH);
        }
        case HashType::SHA256: {
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
            return bytesToHex(hash, SHA256_DIGEST_LENGTH);
        }
        case HashType::NTLM: {
            return computeNTLM(input);
        }
        case HashType::BCRYPT: {
            return "";
        }
        default:
            return "";
    }
}

HashType detectHashType(const std::string& hash) {
    size_t len = hash.length();

    if (hash.substr(0, 4) == "$2a$" || hash.substr(0, 4) == "$2b$" || hash.substr(0, 4) == "$2y$") {
        return HashType::BCRYPT;
    }

    if (len == 32) {
        return HashType::MD5;
    } else if (len == 40) {
        return HashType::SHA1;
    } else if (len == 64) {
        return HashType::SHA256;
    }

    return HashType::MD5;
}
