#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

const uint32_t FNV_OFFSET = 2166136261u;
const uint32_t FNV_PRIME  = 16777619u;

// Для ручного расчёта в контрольном примере (ТЗ 7.1) параметры выбраны небольшими:
// p = 251, q = 241, n = 60491, phi = 60000, e = 7, d = 17143 (e*d = 2*phi + 1)
const uint32_t RSA_N = 60491;
const uint32_t RSA_E = 7;
const uint32_t RSA_D = 17143;

uint32_t fnv1a(const void* data, size_t length, uint32_t hash = FNV_OFFSET);
uint32_t hashFile(const std::string& path);

uint32_t modPow(uint32_t base, uint32_t exp, uint32_t mod);
uint32_t rsaSign(uint32_t hash);
bool     rsaVerify(uint32_t hash, uint32_t signature);
