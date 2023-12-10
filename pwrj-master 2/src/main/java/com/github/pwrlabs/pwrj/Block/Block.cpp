#include<vector>
#include<iostream>

#include "Transaction/Transaction.cpp"
#include "Transaction/TransferTxn.cpp"


class Block {
private:
    int transactionCount;
    int size;
    long number;
    long reward;
    long timestamp;
    std::string hash;
    std::string submitter;
    bool success;
    std::vector<Transaction*> transactions;

public:
    Block(const std::string& blockJson) {
        // Parse blockJson (replace this with your actual JSON parsing logic)

        // Example initialization
        transactionCount = 0; // Replace with actual value
        size = 0; // Replace with actual value
        number = 0; // Replace with actual value
        reward = 0; // Replace with actual value
        timestamp = 0; // Replace with actual value
        hash = ""; // Replace with actual value
        submitter = ""; // Replace with actual value
        success = false; // Replace with actual value

        // Example transactions initialization
        transactions.push_back(new Transaction()); // Replace with actual transactions
    }

    Block(JSONObject blockJson) 
    {
        transactionCount = blockJson.getInt("transactionCount");
        size = blockJson.getInt("blockSize");
        number = blockJson.getLong("blockNumber");
        reward = blockJson.getLong("blockReward");
        timestamp = blockJson.getLong("timestamp");
        hash = blockJson.getString("blockHash");
        submitter = blockJson.getString("blockSubmitter");
        success = blockJson.getBoolean("success");

        JSONArray txns = blockJson.getJSONArray("transactions");
        transactions = new Transaction[txns.length()];

        for(int i = 0; i < txns.length(); i++) {
            JSONObject txnObject = txns.getJSONObject(i);
            Transaction txn = null;
            
            String txnType = txnObject.has("type") ? txnObject.getString("type") : "Unknown";
            if(txnType.equalsIgnoreCase("Transfer")) {
                txn = new TransferTxn(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"), txnObject.getLong("value"));
            } else if(txnType.equalsIgnoreCase("VM Data")) {
                txn = new VmDataTxn(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"), txnObject.getLong("vmId"), txnObject.getString("data"));
            } else if(txnType.equalsIgnoreCase("Delegate")) {
                txn = new DelegateTxn(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"), txnObject.getLong("value"));
            } else if(txnType.equalsIgnoreCase("Withdraw")) {
                txn = new WithdrawTxn(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"), txnObject.getLong("shares"));
            } else if(txnType.equalsIgnoreCase("Validator Join")) {
                txn = new JoinTxn(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"));
            } else {
                txn = new Transaction(txnObject.getInt("size"), txnObject.getInt("positionInTheBlock"), txnObject.getLong("fee"), txnType, txnObject.getString("from"), txnObject.getString("to"), txnObject.getString("nonceOrValidationHash"), txnObject.getString("hash"));
            }
            transactions[i] = txn;
        }
    }

    // Getters
    int getTransactionCount() const {
        return transactionCount;
    }

    int getSize() const {
        return size;
    }

    long getNumber() const {
        return number;
    }

    long getReward() const {
        return reward;
    }

    long getTimestamp() const {
        return timestamp;
    }

    const std::string& getHash() const {
        return hash;
    }

    const std::string& getSubmitter() const {
        return submitter;
    }

    bool isSuccess() const {
        return success;
    }

    const std::vector<Transaction*>& getTransactions() const {
        return transactions;
    }
};

int main()
{
    return 0;
}

