#ifndef OBFUSCATION_H
#define OBFUSCATION_H

#include <string>
#include <cstdint>

namespace Obf {

    constexpr uint64_t PRNG_MUL = 0x5DEECE66DULL;
    constexpr uint64_t PRNG_ADD = 0xBULL;
    constexpr uint64_t PRNG_MOD = 0xFFFFFFFFFFFFULL;

    template <uint64_t Seed, size_t N>
    struct XorString {
        char data[N];

        constexpr XorString(const char(&str)[N]) : data{} {
            uint64_t s = Seed;
            for (size_t i = 0; i < N; ++i) {
                s = (s * PRNG_MUL + PRNG_ADD) & PRNG_MOD;
                char k = (char)(s >> 16);
                data[i] = str[i] ^ k;
            }
        }

        inline void decrypt(char* buf) const {
            uint64_t s = Seed;
            for (size_t i = 0; i < N; ++i) {
                s = (s * PRNG_MUL + PRNG_ADD) & PRNG_MOD;
                volatile char k = (char)(s >> 16);
                buf[i] = data[i] ^ k;
            }
            buf[N - 1] = '\0';
        }
    };
}

// کێشەکەت لێرە بوو، ئەم بەشە #endif ـی دەوێت
#ifdef __COUNTER__
#define OBF_SEED ((__COUNTER__ ^ 0x55AA55AA) + (__LINE__ * 1337))
#else
#define OBF_SEED ((__LINE__ ^ 0x55AA55AA) + (__LINE__ * 1337))
#endif 
// ---------------------------------------------

#define OBF(str) ([]() -> const char* { \
    constexpr size_t Size = sizeof(str) / sizeof(char); \
    constexpr auto encrypted = Obf::XorString<OBF_SEED, Size>(str); \
    static char buf[Size]; \
    encrypted.decrypt(buf); \
    return buf; \
})()

#define OBF_STR(str) ([]() -> std::string { \
    constexpr size_t Size = sizeof(str) / sizeof(char); \
    constexpr auto encrypted = Obf::XorString<OBF_SEED, Size>(str); \
    char buf[Size]; \
    encrypted.decrypt(buf); \
    return std::string(buf); \
})()

#define OBFUSCATE(str) OBF(str)

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;

constexpr uint32_t fnv1a(const char* const s, const uint32_t value = val_32_const) noexcept {
    return (s[0] == '\0') ? value : fnv1a(&s[1], (value ^ uint32_t(s[0])) * prime_32_const);
}

inline uint32_t fnv1a_rt(const char* s) {
    uint32_t hash = val_32_const;
    while (*s) {
        hash = (hash ^ (uint32_t)(*s++)) * prime_32_const;
    }
    return hash;
}

#endif
