#include <iostream>
#include <string>

class Transaction {
public:
    Transaction(int size, int positionInTheBlock, long fee, const std::string& type, const std::string& from,
                const std::string& to, const std::string& nonceOrValidationHash, const std::string& hash)
        : size(size), positionInTheBlock(positionInTheBlock), fee(fee), type(type), from(from), to(to),
          nonceOrValidationHash(nonceOrValidationHash), hash(hash) {}

    // Getters

    /**
     * @return the size of the transaction
     */
    int getSize() const {
        return size;
    }

    /**
     * @return the position of the transaction in the block
     */
    int getPositionInTheBlock() const {
        return positionInTheBlock;
    }

    /**
     * @return the fee of the transaction
     */
    long getFee() const {
        return fee;
    }

    /**
     * @return the type of the transaction
     */
    const std::string& getType() const {
        return type;
    }

    /**
     * @return the address of the sender of the transaction
     */
    const std::string& getFrom() const {
        return from;
    }

    /**
     * @return the address of the receiver of the transaction
     */
    const std::string& getTo() const {
        return to;
    }

    /**
     * @return the nonce of the transaction if it is an ECDSA transaction,
     *         or the validation hash if it is an MHBS transaction
     */
    const std::string& getNonceOrValidationHash() const {
        return nonceOrValidationHash;
    }

    /**
     * @return the hash of the transaction
     */
    const std::string& getHash() const {
        return hash;
    }

    // Abstract Methods
    virtual long getValue() const {
        return 0;
    }

private:
    int size;
    int positionInTheBlock;
    long fee;
    std::string type;
    std::string from;
    std::string to;
    std::string nonceOrValidationHash;
    std::string hash;
};

