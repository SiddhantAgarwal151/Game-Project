#include <SFML/Network.hpp>
#include <iostream>
#include <sstream>

int main() {
    std::cout << "=== SFML HTTP Local Demo ===" << std::endl;

    // Connect to local server (protocol excluded)
    sf::Http http("localhost", 8080);

    // -----------------------------
    // 1️⃣  GET request
    // -----------------------------
    sf::Http::Request getRequest("/");
    getRequest.setMethod(sf::Http::Request::Get);

    std::cout << "\n=== GET Request ===" << std::endl;
    sf::Http::Response getResponse = http.sendRequest(getRequest);

    std::cout << "Status: " << getResponse.getStatus() << std::endl;
    std::cout << "HTTP Version: " << getResponse.getMajorHttpVersion() << "."
              << getResponse.getMinorHttpVersion() << std::endl;
    std::cout << "Content-Type: " << getResponse.getField("Content-Type") << std::endl;
    std::cout << "Body:\n" << getResponse.getBody() << std::endl;

    // -----------------------------
    // 2️⃣  POST request
    // -----------------------------
    sf::Http::Request postRequest("/post", sf::Http::Request::Post);
    postRequest.setField("Content-Type", "application/x-www-form-urlencoded");

    std::ostringstream postBody;
    postBody << "name=Sid&score=99";
    postRequest.setBody(postBody.str());

    std::cout << "\n=== POST Request ===" << std::endl;
    sf::Http::Response postResponse = http.sendRequest(postRequest);

    std::cout << "Status: " << postResponse.getStatus() << std::endl;
    std::cout << "Content-Type: " << postResponse.getField("Content-Type") << std::endl;
    std::cout << "Body:\n" << postResponse.getBody() << std::endl;

    // -----------------------------
    // 3️⃣  HEAD request
    // -----------------------------
    sf::Http::Request headRequest("/", sf::Http::Request::Head);

    std::cout << "\n=== HEAD Request ===" << std::endl;
    sf::Http::Response headResponse = http.sendRequest(headRequest);

    std::cout << "Status: " << headResponse.getStatus() << std::endl;
    std::cout << "Server: " << headResponse.getField("Server") << std::endl;
    std::cout << "Content-Type: " << headResponse.getField("Content-Type") << std::endl;
    std::cout << "Content-Length: " << headResponse.getField("Content-Length") << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}
