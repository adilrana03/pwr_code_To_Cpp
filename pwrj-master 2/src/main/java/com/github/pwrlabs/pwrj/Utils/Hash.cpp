#include <iostream>
#include <cstring>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/blake2.h>
#include <botan/hash.h>
#include <botan/botan_all.h>

class Hash {
private:
    Hash() {}

public:
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& input, const std::string& algorithm) {
        if (algorithm == "SHA-256") {
            return sha256(input);
        } else if (algorithm == "SHA3-256") {
            return sha3(input);
        } else if (algorithm == "HmacSHA512") {
            // Replace with Bouncy Castle HMac implementation
            // Note: Bouncy Castle's HMac class is not directly available in the current OpenSSL/Botan setup
            // You may need to explore other alternatives or use a dedicated library for HMac-SHA512.
            throw std::runtime_error("HmacSHA512 is not supported in this example");
        } else if (algorithm == "RIPEMD160") {
            return ripemd160(input);
        } else if (algorithm == "Blake2b256") {
            return blake2b256(input);
        } else {
            throw std::runtime_error("Unsupported hash algorithm: " + algorithm);
        }
    }

    static std::vector<uint8_t> sha3(const std::vector<uint8_t>& input) {
        Botan::Keccak_1600 keccak(256);
        keccak.update(input);
        return keccak.final();
    }

    static std::vector<uint8_t> sha256(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> result(SHA256_DIGEST_LENGTH);
        SHA256(input.data(), input.size(), result.data());
        return result;
    }

    static std::vector<uint8_t> ripemd160(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> result(RIPEMD160_DIGEST_LENGTH);
        RIPEMD160(input.data(), input.size(), result.data());
        return result;
    }

    static std::vector<uint8_t> blake2b256(const std::vector<uint8_t>& input) {
        std::vector<uint8_t> result(BLAKE2B256_OUTBYTES);
        blake2b(result.data(), input.data(), nullptr, BLAKE2B256_OUTBYTES, input.size(), 0);
        return result;
    }
};

