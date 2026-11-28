#pragma once
#include <cstdint>
#include <string>

struct SignInfo {
    uint32_t hash;
    uint32_t hashMod;
    uint32_t sig;
    bool     valid;
};

size_t formStatement(const std::string& source, const std::string& dest, const std::string& group);

void signStatement(const std::string& stmtPath, const std::string& sigPath, SignInfo& info);
void verifyStatement(const std::string& stmtPath, const std::string& sigPath, SignInfo& info);
void tamperStatement(const std::string& stmtPath);
