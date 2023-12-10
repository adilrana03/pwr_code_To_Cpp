#include <iostream>
#include <string>

class Delegator {
private:
    std::string address;
    std::string validatorAddress;
    long shares;
    long delegatedPWR;

public:
    Delegator(const std::string& address, const std::string& validatorAddress, long shares, long delegatedPWR)
        : address(address), validatorAddress(validatorAddress), shares(shares), delegatedPWR(delegatedPWR) {}

    // Getters
    const std::string& getAddress() const {
        return address;
    }

    const std::string& getValidatorAddress() const {
        return validatorAddress;
    }

    long getShares() const {
        return shares;
    }

    long getDelegatedPWR() const {
        return delegatedPWR;
    }
};

