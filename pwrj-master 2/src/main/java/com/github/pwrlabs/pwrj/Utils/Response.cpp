#include <iostream>
#include <string>

class Response {
private:
    bool success;
    std::string txnHash;
    std::string error;

public:
    Response(bool success, const std::string& txnHash, const std::string& error)
        : success(success), txnHash(txnHash), error(error) {}

    /**
     * @return true if the operation was successful, false otherwise
     */
    bool isSuccess() const {
        return success;
    }

    /**
     * @return message returned if the operation was successful
     */
    const std::string& getTxnHash() const {
        return txnHash;
    }

    /**
     * @return error returned if the operation was not successful
     */
    const std::string& getError() const {
        return error;
    }
};

