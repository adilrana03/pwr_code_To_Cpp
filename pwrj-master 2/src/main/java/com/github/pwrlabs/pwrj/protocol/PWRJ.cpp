#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <iomanip>
#include <sstream>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class PWRJ {
private:
    static http_client client;
    static utility::string_t rpcNodeUrl;
    static long feePerByte;


public:
    static void setRpcNodeUrl(const utility::string_t& url) {
        rpcNodeUrl = url;
    }

    static utility::string_t getRpcNodeUrl() {
        return rpcNodeUrl;
    }

    static long getFeePerByte() {
        return feePerByte;
    }

    static int getNonceOfAddress(const utility::string_t& address) {
        try {
            uri_builder builder(rpcNodeUrl);
            builder.append_path(U("/nonceOfUser/"))
                   .append_query(U("userAddress"), address);

            http_response response = client.request(methods::GET, builder.to_string()).get();
            if (response.status_code() == status_codes::OK) {
                json::value jsonResponse = response.extract_json().get();
                return jsonResponse[U("nonce")].as_integer();
            } else if (response.status_code() == status_codes::BadRequext) {
                json::value jsonResponse = response.extract_json().get();
                throw std::runtime_error("Failed with HTTP error 400 and message: " + jsonResponse[U("message")].as_string());
            } else {
                throw std::runtime_error("Failed with HTTP error code : " + std::to_string(response.status_code()));
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 0;
        }
    }
};


