#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <iostream>

class Signature {
public:
    static EC_GROUP* CURVE_GROUP;
    static BIGNUM* HALF_CURVE_ORDER;

    static EC_KEY* generateKeyPair();
    static unsigned char* signMessage(const unsigned char* message, size_t messageLen, EC_KEY* privateKey);
    static EC_KEY* recoverFromSignature(int recId, ECDSA_SIG* sig, const unsigned char* message, size_t messageLen);
    static ECDSA_SIG* sign(EC_KEY* privateKey, const unsigned char* transactionHash, size_t hashLen);
    static EC_POINT* decompressKey(BIGNUM* xBN, bool yBit);
    static BIGNUM* publicKeyFromPrivate(BIGNUM* privKey);
    static EC_POINT* publicPointFromPrivate(BIGNUM* privKey);
    static unsigned char* toBytesPadded(BIGNUM* value, int length);
    static void verifyPrecondition(bool assertionResult, const char* errorMessage);

    public:
        EC_GROUP* Signature::CURVE_GROUP;
        BIGNUM* Signature::HALF_CURVE_ORDER;
};

void initializeCurveParams() {
    CURVE_GROUP = EC_GROUP_new_by_curve_name(NID_secp256k1);
    HALF_CURVE_ORDER = BN_new();
    BN_rshift1(HALF_CURVE_ORDER, EC_GROUP_get0_order(CURVE_GROUP));
}

EC_KEY* Signature::generateKeyPair() {
    EC_KEY* keyPair = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(keyPair);
    return keyPair;
}

unsigned char* Signature::signMessage(const unsigned char* message, size_t messageLen, EC_KEY* privateKey) {
    BIGNUM* privateKeyBN = EC_KEY_get0_private_key(privateKey);
    BIGNUM* publicKeyBN = publicKeyFromPrivate(privateKeyBN);
    unsigned char* messageHash = SHA3_256(message, messageLen);

    ECDSA_SIG* sig = sign(privateKey, messageHash, 32);
    int recId = -1;

    for (int headerByte = 0; headerByte < 4; ++headerByte) {
        EC_KEY* recoveredKey = recoverFromSignature(headerByte, sig, messageHash, 32);
        BIGNUM* recoveredKeyBN = publicKeyFromPrivate(EC_KEY_get0_private_key(recoveredKey));
        if (BN_cmp(publicKeyBN, recoveredKeyBN) == 0) {
            recId = headerByte;
            EC_KEY_free(recoveredKey);
            break;
        }
        EC_KEY_free(recoveredKey);
    }

    if (recId == -1) {
        throw std::runtime_error("Could not construct a recoverable key. Are your credentials valid?");
    } else {
        unsigned char* signature = new unsigned char[65];
        BIGNUM* r = BN_new();
        BIGNUM* s = BN_new();
        ECDSA_SIG_get0(sig, &r, &s);

        // r, s, v
        BN_bn2bin(r, signature);
        BN_bn2bin(s, signature + 32);
        signature[64] = static_cast<unsigned char>(recId + 27);

        ECDSA_SIG_free(sig);
        BN_free(r);
        BN_free(s);

        return signature;
    }
}

EC_KEY* Signature::recoverFromSignature(int recId, ECDSA_SIG* sig, const unsigned char* message, size_t messageLen) {
    verifyPrecondition(recId >= 0, "recId must be positive");
    BIGNUM* r = BN_new();
    BIGNUM* s = BN_new();
    ECDSA_SIG_get0(sig, &r, &s);

    verifyPrecondition(BN_cmp(r, BN_value_one()) >= 0, "r must be positive");
    verifyPrecondition(BN_cmp(s, BN_value_one()) >= 0, "s must be positive");
    verifyPrecondition(message != nullptr, "message cannot be null");

    const BIGNUM* n = EC_GROUP_get0_order(CURVE_GROUP);
    BIGNUM* i = BN_new();
    BN_set_word(i, static_cast<unsigned long>(recId / 2));
    BIGNUM* x = BN_new();
    BN_mod_add(x, r, BN_mod_mul(i, i, n, nullptr), n, nullptr);

    const BIGNUM* prime = EC_GROUP_get0_prime(CURVE_GROUP);
    if (BN_cmp(x, prime) >= 0) {
        BN_free(i);
        BN_free(x);
        return nullptr;
    }

    EC_POINT* R = decompressKey(x, recId & 1);
    EC_POINT* G = EC_GROUP_get0_generator(CURVE_GROUP);
    if (!EC_POINT_mul(CURVE_GROUP, R, nullptr, R, n, nullptr) || !EC_POINT_is_at_infinity(CURVE_GROUP, R)) {
        BN_free(i);
        BN_free(x);
        EC_POINT_free(R);
        return nullptr;
    }

    BIGNUM* e = BN_bin2bn(message, static_cast<int>(messageLen), nullptr);
    BIGNUM* eInv = BN_new();
    BN_sub(eInv, BN_value_one(), e);
    BN_set_negative(eInv, 1);

    BIGNUM* rInv = BN_mod_inverse(nullptr, r, n, nullptr);
    BIGNUM* srInv = BN_new();
    BN_mod_mul(srInv, s, rInv, n, nullptr);
    BIGNUM* eInvrInv = BN_new();
    BN_mod_mul(eInvrInv, rInv, eInv, n, nullptr);

    EC_POINT* q = EC_POINT_new(CURVE_GROUP);
    EC_POINT_mul(CURVE_GROUP, q, nullptr, G, eInvrInv, R);
    unsigned char* qBytes = new unsigned char[65];
    EC_POINT_point2oct(CURVE_GROUP, q, POINT_CONVERSION_COMPRESSED, qBytes, 65, nullptr);

    BN_free(i);
    BN_free(x);
    BN_free(e);
    BN_free(eInv);
    BN_free(rInv);
    BN_free(srInv);
    BN_free(eInvrInv);
    EC_POINT_free(R);
    EC_POINT_free(q);

    return EC_KEY_new_by_curve_name(NID_secp256k1);
}

ECDSA_SIG* Signature::sign(EC_KEY* privateKey, const unsigned char* transactionHash, size_t hashLen) {
    ECDSA_SIG* sig = ECDSA_do_sign(transactionHash, static_cast<int>(hashLen), privateKey);
    return sig;
}

EC_POINT* Signature::decompressKey(BIGNUM* xBN, bool yBit) {
    EC_POINT* point = EC_POINT_new(CURVE_GROUP);
    BN_CTX* ctx = BN_CTX_new();
    EC_GROUP_get_curve_GFp(CURVE_GROUP, nullptr, nullptr, ctx);

    BIGNUM* yBN = BN_new();
    EC_POINT_set_compressed_coordinates_GFp(CURVE_GROUP, point, xBN, yBit ? 1 : 0, ctx);
    BN_CTX_free(ctx);
    BN_free(yBN);

    return point;
}

BIGNUM* Signature::publicKeyFromPrivate(BIGNUM* privKey) {
    EC_POINT* point = publicPointFromPrivate(privKey);
    BIGNUM* xBN = BN_new();
    BIGNUM* yBN = BN_new();
    EC_POINT_get_affine_coordinates_GFp(CURVE_GROUP, point, xBN, yBN, nullptr);
    BIGNUM* publicKeyBN = BN_new();
    BN_set_negative(publicKeyBN, 0);
    BN_copy(publicKeyBN, xBN);

    BN_free(xBN);
    BN_free(yBN);
    EC_POINT_free(point);

    return publicKeyBN;
}

EC_POINT* Signature::publicPointFromPrivate(BIGNUM* privKey) {
    if (BN_cmp(privKey, EC_GROUP_get0_order(CURVE_GROUP)) >= 0) {
        BN_mod(privKey, privKey, EC_GROUP_get0_order(CURVE_GROUP), nullptr);
    }

    EC_POINT* point = EC_POINT_new(CURVE_GROUP);
    EC_POINT_mul(CURVE_GROUP, point, privKey, nullptr, nullptr, nullptr);

    return point;
}

unsigned char* Signature::toBytesPadded(BIGNUM* value, int length) {
    unsigned char* result = new unsigned char[length];
    size_t bytesLength = BN_num_bytes(value);
    const unsigned char* bytes = BN_bn2bin(value, result + length - bytesLength);

    if (bytes[0] == 0) {
        bytesLength--;
    }

    if (bytesLength > static_cast<size_t>(length)) {
        throw std::runtime_error("Input is too large to put in byte array of size " + std::to_string(length));
    }

    return result;
}

void Signature::verifyPrecondition(bool assertionResult, const char* errorMessage) {
    if (!assertionResult) {
        throw std::runtime_error(errorMessage);
    }
}
