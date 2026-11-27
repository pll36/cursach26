#include "crypto.h"
#include "common.h"
#include <fstream>

using namespace std;

uint32_t fnv1a(const void* data, size_t length, uint32_t hash) {
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

uint32_t hashFile(const string& path) {
    ifstream file(path, ios::binary);
    if (!file) throw FileError(openErrorText(path));
    uint32_t hash = FNV_OFFSET;
    char symbol;
    while (file.get(symbol)) {
        hash ^= static_cast<unsigned char>(symbol);
        hash *= FNV_PRIME;
    }
    if (file.bad()) throw FileError("ошибка чтения файла " + path);
    return hash;
}

uint32_t modPow(uint32_t base, uint32_t exp, uint32_t mod) {
    uint32_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

uint32_t rsaSign(uint32_t hash) {
    return modPow(hash % RSA_N, RSA_D, RSA_N);
}

bool rsaVerify(uint32_t hash, uint32_t signature) {
    if (signature >= RSA_N) return false;
    return modPow(signature, RSA_E, RSA_N) == hash % RSA_N;
}
