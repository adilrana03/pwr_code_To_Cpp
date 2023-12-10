#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/keccak.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/secblock.h>

#include "Utils/Hash.h" // Assuming Hash class is defined in Utils.h
#include "Response.h" // Assuming Response class is defined in Response.h
#include "PWRJ.h" // Assuming PWRJ class is defined in PWRJ.h
#include "Signature.h" // Assuming Signature class is defined in Signature.h

class PWRWallet {
private:
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::Integer privateKey;

public:
    PWRWallet(const std::string& privateKeyHex) : privateKey(CryptoPP::Integer(privateKeyHex.c_str())) {}

    PWRWallet(const std::vector<byte>& privateKeyBytes) : privateKey(CryptoPP::Integer(privateKeyBytes.data(), privateKeyBytes.size())) {}

    PWRWallet(const CryptoPP::Integer& privateKey) : privateKey(privateKey) {}

    PWRWallet() : privateKey(CryptoPP::Integer(rng, 256)) {}

    std::string getAddress() const {
        return publicKeyToAddress(publicKeyFromPrivate(privateKey));
    }

    static std::string publicKeyToAddress(const CryptoPP::Integer& publicKey) {
        CryptoPP::SecByteBlock publicKeyBytes(publicKey.MinEncodedSize());
        publicKey.Encode(publicKeyBytes, publicKeyBytes.size());

        if (publicKeyBytes[0] == 0x04) {
            publicKeyBytes = CryptoPP::SecByteBlock(publicKeyBytes.data() + 1, publicKeyBytes.size() - 1);
        }

        CryptoPP::Keccak_256 keccak;
        keccak.Update(publicKeyBytes, publicKeyBytes.size());

        CryptoPP::SecByteBlock addressBytes(20);
        keccak.Final(addressBytes);

        std::string addressHex;
        CryptoPP::HexEncoder hexEncoder(new CryptoPP::StringSink(addressHex), false);
        hexEncoder.Put(addressBytes, addressBytes.size());
        hexEncoder.MessageEnd();

        return "0x" + addressHex.substr(addressHex.size() - 40);
    }

    long getBalance() {
        return PWRJ::getBalanceOfAddress(getAddress());
    }

    int getNonce() {
        return PWRJ::getNonceOfAddress(getAddress());
    }

    CryptoPP::Integer getPrivateKey() const {
        return privateKey;
    }

    Response transferPWR(const std::string& to, long amount, int nonce) {
        if (to.size() != 42) {
            throw std::runtime_error("Invalid address");
        }
        if (amount < 0) {
            throw std::runtime_error("Amount cannot be negative");
        }
        if (nonce < 0) {
            throw std::runtime_error("Nonce cannot be negative");
        }
        if (amount + (98 * PWRJ::getFeePerByte()) > getBalance()) {
            throw std::runtime_error("Insufficient balance");
        }
        if (nonce < getNonce()) {
            throw std::runtime_error("Nonce is too low");
        }

        CryptoPP::byte_buffer buffer;
        buffer.push_back(0x00);
        buffer.push_back((nonce >> 24) & 0xFF);
        buffer.push_back((nonce >> 16) & 0xFF);
        buffer.push_back((nonce >> 8) & 0xFF);
        buffer.push_back(nonce & 0xFF);
        buffer.push_back((amount >> 56) & 0xFF);
        buffer.push_back((amount >> 48) & 0xFF);
        buffer.push_back((amount >> 40) & 0xFF);
        buffer.push_back((amount >> 32) & 0xFF);
        buffer.push_back((amount >> 24) & 0xFF);
        buffer.push_back((amount >> 16) & 0xFF);
        buffer.push_back((amount >> 8) & 0xFF);
        buffer.push_back(amount & 0xFF);
        CryptoPP::SecByteBlock toBytes;
        CryptoPP::StringSource(to.substr(2), true, new CryptoPP::HexDecoder(new CryptoPP::ArraySink(toBytes, toBytes.size())));
        buffer.insert(buffer.end(), toBytes.begin(), toBytes.end());

        CryptoPP::byte_buffer signature = Signature::signMessage(buffer, privateKey);

        buffer.insert(buffer.end(), signature.begin(), signature.end());

        return PWRJ::broadcastTxn(buffer);
    }

    Response transferPWR(const std::string& to, long amount) {
        return transferPWR(to, amount, getNonce());
    }

    // Other member functions...

private:
    static CryptoPP::Integer publicKeyFromPrivate(const CryptoPP::Integer& privateKey) {
        CryptoPP::ECP::Point point = publicPointFromPrivate(privateKey);
        CryptoPP::SecByteBlock encoded(point.EncodedSize(true));
        point.EncodePoint(encoded, true);
        return CryptoPP::Integer(encoded.data(), encoded.size());
    }

    static CryptoPP::ECP::Point publicPointFromPrivate(const CryptoPP::Integer& privateKey) {
        CryptoPP::ECP::Point base = CryptoPP::ECP::Point::Multiplication(Signature::getCurve().GetBasePrecomputation(), privateKey);
        return CryptoPP::ECP::Point::Multiplication(Signature::getCurve().GetBasePrecomputation(), privateKey);
    }
};

