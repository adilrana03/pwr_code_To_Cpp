#include <iostream>
#include <string>
#include "Transaction.cpp"

class TransferTxn : public Transaction {
public:
    TransferTxn(int transactionSize, int positionInBlock, long transactionFee, const std::string& type,
                const std::string& from, const std::string& to, const std::string& nonceOrValidationHash,
                const std::string& hash, long value)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          value(value) {}

    // Getters

    /**
     * @return the value of the transaction
     */
    long getValue() const override {
        return value;
    }

private:
    long value;
};

