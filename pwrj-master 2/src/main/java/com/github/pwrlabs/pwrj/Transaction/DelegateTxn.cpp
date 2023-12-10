#include <iostream>
#include <string>
#include "Transaction.cpp"

class DelegateTxn : public Transaction 
{
public:
    DelegateTxn(int transactionSize, int positionInBlock, long transactionFee, const std::string& type,
                const std::string& from, const std::string& to, const std::string& nonceOrValidationHash,
                const std::string& hash, long amount)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(to), amount(amount) {}

    // Getters

    /**
     * @return the validator the user is delegating to
     */
    std::string getValidator() const {
        return validator;
    }

    /**
     * @return the amount of PWR the user is delegating
     */
    long getAmount() const {
        return amount;
    }

    // Override getValue() if needed
    // long getValue() override {}

private:
    std::string validator;
    long amount;
};

