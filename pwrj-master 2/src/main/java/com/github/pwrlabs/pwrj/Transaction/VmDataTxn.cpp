#include <iostream>
#include <string>
#include "Transaction.cpp"

class VmDataTxn : public Transaction {
public:
    VmDataTxn(int transactionSize, int positionInBlock, long transactionFee, const std::string& type,
              const std::string& from, const std::string& to, const std::string& nonceOrValidationHash,
              const std::string& hash, long vmId, const std::string& data)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          vmId(vmId), data(data) {}

    // Getters

    /**
     * @return the id of the VM that this transaction is for
     */
    long getVmId() const {
        return vmId;
    }

    /**
     * @return the data of the transaction
     */
    const std::string& getData() const {
        return data;
    }

private:
    long vmId;
    std::string data;
};

