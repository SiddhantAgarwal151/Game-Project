#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

int main()
{
    std::cout << "=== SFML FTP Demo ===" << std::endl;

    // Create FTP client
    sf::Ftp ftp;

    // Connect to FTP server (change this to your test server or localhost)
    std::cout << "\nConnecting to FTP server..." << std::endl;
    sf::Ftp::Response connectResponse = ftp.connect("ftp.dlptest.com", 21, sf::seconds(5));

    if (!connectResponse.isOk())
    {
        std::cerr << "❌ Connection failed: " << connectResponse.getMessage() << std::endl;
        return 1;
    }
    std::cout << "✅ Connected: " << connectResponse.getMessage() << std::endl;

    // Log in anonymously (dlptest.com allows anonymous FTP)
    std::cout << "Logging in anonymously..." << std::endl;
    sf::Ftp::Response loginResponse = ftp.login();

    if (!loginResponse.isOk())
    {
        std::cerr << "❌ Login failed: " << loginResponse.getMessage() << std::endl;
        return 1;
    }
    std::cout << "✅ Logged in: " << loginResponse.getMessage() << std::endl;

    // Get current working directory
    sf::Ftp::DirectoryResponse dirResponse = ftp.getWorkingDirectory();
    if (dirResponse.isOk())
        std::cout << "\n📂 Current directory: " << dirResponse.getDirectory() << std::endl;

    // Get directory listing
    sf::Ftp::ListingResponse listResponse = ftp.getDirectoryListing();
    if (listResponse.isOk())
    {
        std::cout << "\n📜 Directory contents:" << std::endl;
        for (const auto& item : listResponse.getListing())
            std::cout << " - " << item << std::endl;
    }

    // Example: Try to create a directory (will fail if you don’t have permission)
    std::cout << "\nCreating new directory 'sfml_demo_test'..." << std::endl;
    sf::Ftp::Response mkdirResponse = ftp.createDirectory("sfml_demo_test");
    std::cout << mkdirResponse.getMessage() << std::endl;

    // Example: Download a file (remote file name may differ per server)
    std::cout << "\nAttempting to download 'readme.txt'..." << std::endl;
    sf::Ftp::Response downloadResponse = ftp.download("readme.txt", ".", sf::Ftp::TransferMode::Ascii);
    std::cout << "Download status: " << downloadResponse.getMessage() << std::endl;

    // Keep connection alive
    ftp.keepAlive();

    // Disconnect
    std::cout << "\nDisconnecting..." << std::endl;
    ftp.disconnect();

    std::cout << "=== Demo Complete ===" << std::endl;
    return 0;
}