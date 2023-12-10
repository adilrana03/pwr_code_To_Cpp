#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "Delegator/Delegator.cpp" 

class Validator {
private:
    std::string address;
    std::string ip;
    bool badActor;
    long votingPower;
    long shares;
    int delegatorsCount;
    std::string status;

public:
    Validator(const std::string& address, const std::string& ip, bool badActor, long votingPower, long shares, int delegatorsCount, const std::string& status)
        : address(address), ip(ip), badActor(badActor), votingPower(votingPower), shares(shares), delegatorsCount(delegatorsCount), status(status) {}

    // Getters

    /**
     * @return the address of the validator
     */
    std::string getAddress() const {
        return address;
    }

    /**
     * @return the ip of the validator
     */
    std::string getIp() const {
        return ip;
    }

    /**
     * @return true if the validator is a bad actor, false otherwise
     */
    bool isBadActor() const {
        return badActor;
    }

    /**
     * @return the voting power of the validator
     */
    long getVotingPower() const {
        return votingPower;
    }

    /**
     * @return the shares of the validator
     */
    long getShares() const {
        return shares;
    }

    /**
     * @return the number of delegators of the validator
     */
    int getDelegatorsCount() const {
        return delegatorsCount;
    }

    /**
     * @return the status of the validator
     */
    std::string getStatus() const {
        return status;
    }

    std::vector<Delegator> getDelegators() const {
        try {
            CURL* curl = curl_easy_init();

            if (curl) {
                std::string url = PWRJ::getRpcNodeUrl() + "/validator/delegatorsOfValidator/?validatorAddress=" + address;

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

                // Response data will be stored here
                std::string response_data;

                // Callback function to write received data to a string
                auto write_callback = [](char* data, size_t size, size_t nmemb, std::string* writer_data) -> size_t {
                    size_t total_size = size * nmemb;
                    writer_data->append(data, total_size);
                    return total_size;
                };

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

                // Perform the HTTP request
                CURLcode res = curl_easy_perform(curl);

                // Check for errors
                if (res != CURLE_OK) {
                    std::cerr << "Curl request failed: " << curl_easy_strerror(res) << std::endl;
                    curl_easy_cleanup(curl);
                    return {};
                }

                // Parse JSON response
                nlohmann::json object = nlohmann::json::parse(response_data);

                if (object.find("delegators") != object.end()) {
                    const auto& delegators = object["delegators"];
                    std::vector<Delegator> delegatorsList;

                    for (const auto& [delegatorAddress, shares] : delegators.items()) {
                        long delegatedPWR = static_cast<long>(shares.get<int64_t>()) * votingPower / shares;
                        Delegator d("0x" + delegatorAddress, address, shares, delegatedPWR);
                        delegatorsList.push_back(d);
                    }

                    curl_easy_cleanup(curl);
                    return delegatorsList;
                } else {
                    std::cerr << "Failed to parse JSON response." << std::endl;
                    curl_easy_cleanup(curl);
                    return {};
                }
            } else {
                std::cerr << "Failed to initialize CURL." << std::endl;
                return {};
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception occurred: " << e.what() << std::endl;
            return {};
        }
    }
};
