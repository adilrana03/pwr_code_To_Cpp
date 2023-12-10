#include <iostream>
#include "protocol/PWRJ.cpp"

int main() {
    pwrj::PWRJ::setRpcNodeUrl("http://localhost:8085/");
    pwrj::PWRWallet wallet("48157030754737918552913355043337580418007638602253380155554472945119041505152");

    std::cout << wallet.getAddress() << std::endl;

    pwrj::Response r = wallet.claimActiveNodeSpot();

    std::cout << "Success: " << r.isSuccess() << std::endl;
    std::cout << "Transaction Hash: " << r.getTxnHash() << std::endl;
    std::cout << "Error: " << r.getError() << std::endl;

    return 0;
}
