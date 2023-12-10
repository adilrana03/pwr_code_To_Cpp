
#include <iostream>
#include <string>
#include "Transaction.cpp"

class JoinTxn : public Transaction 
{
public:
    JoinTxn(int transactionSize, int positionInBlock, long transactionFee, const std::string& type,
            const std::string& from, const std::string& to, const std::string& nonceOrValidationHash,
            const std::string& hash)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(from) {}

    // Getters

    /**
     * @return the validator the user is delegating to
     */
    std::string getValidator() const {
        return validator;
    }

    // Override getValue() if needed
    // long getValue() override {}

private:
    std::string validator;
};

