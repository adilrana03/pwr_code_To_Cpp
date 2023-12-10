#include <iostream>
#include <string>
#include "Transaction.cpp"

class WithdrawTxn : public Transaction {
public:
    WithdrawTxn(int transactionSize, int positionInBlock, long transactionFee, const std::string& type,
                const std::string& from, const std::string& to, const std::string& nonceOrValidationHash,
                const std::string& hash, const std::string& validator, long shares)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(validator), shares(shares) {}

    // Getters

    /**
     * @return the validator the user is withdrawing from
     */
    const std::string& getValidator() const {
        return validator;
    }

    /**
     * @return the amount of PWR the user is withdrawing
     */
    long getShares() const {
        return shares;
    }

private:
    std::string validator;
    long shares;
};

