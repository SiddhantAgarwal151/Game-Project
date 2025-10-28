#include <SFML/Network.hpp>
#include <cstdint>
#include <iostream>
#include <string>

// ------------------------------------------------------------
// Example struct for user-defined data type
// ------------------------------------------------------------
struct Character
{
    std::uint8_t age;
    std::string  name;
    float        weight;
};

// Overload the << operator to insert Character into a packet
sf::Packet& operator<<(sf::Packet& packet, const Character& character)
{
    return packet << character.age << character.name << character.weight;
}

// Overload the >> operator to extract Character from a packet
sf::Packet& operator>>(sf::Packet& packet, Character& character)
{
    return packet >> character.age >> character.name >> character.weight;
}

// ------------------------------------------------------------
// Custom packet that automatically compresses/decompresses data
// (Mock implementation for demonstration purposes)
// ------------------------------------------------------------
class ZipPacket : public sf::Packet
{
private:
    // Fake compression function
    const void* compressTheData(const void* data, std::size_t srcSize, std::size_t* dstSize)
    {
        // In a real scenario, you would compress here.
        // For demo, we simply return original data.
        *dstSize = srcSize;
        return data;
    }

    // Fake decompression function
    const void* uncompressTheData(const void* data, std::size_t srcSize, std::size_t* dstSize)
    {
        // In a real scenario, you would decompress here.
        // For demo, just return the input.
        *dstSize = srcSize;
        return data;
    }

protected:
    // Called before the packet data is sent
    const void* onSend(std::size_t& size) override
    {
        const void* srcData = getData();
        std::size_t srcSize = getDataSize();
        return compressTheData(srcData, srcSize, &size);
    }

    // Called after the packet data is received
    void onReceive(const void* data, std::size_t size) override
    {
        std::size_t dstSize;
        const void* dstData = uncompressTheData(data, size, &dstSize);
        append(dstData, dstSize);
    }
};

// ------------------------------------------------------------
// Example usage: sending and receiving packets
// ------------------------------------------------------------
int main()
{
    // Example 1: Using packets for primitive and string types
    {
        std::int16_t x = 10;
        std::string s = "hello";
        double d = 0.6;

        sf::Packet packet;
        packet << x << s << d;

        // Simulate network send/receive
        sf::Packet receivedPacket = packet;

        std::uint16_t rx;
        std::string rs;
        double rd;

        if (receivedPacket >> rx >> rs >> rd)
        {
            std::cout << "Received values:\n";
            std::cout << "x = " << rx << ", s = " << rs << ", d = " << rd << "\n";
        }
        else
        {
            std::cerr << "Failed to read data from packet.\n";
        }
    }

    // Example 2: Using packets with a custom struct (Character)
    {
        Character bob{25, "Bob", 72.5f};

        sf::Packet packet;
        packet << bob;

        Character received;
        packet >> received;

        std::cout << "\nCharacter received:\n";
        std::cout << "Age: " << static_cast<int>(received.age)
                  << ", Name: " << received.name
                  << ", Weight: " << received.weight << " kg\n";
    }

    // Example 3: Using a custom ZipPacket
    {
        ZipPacket zipPacket;

        Character alice{30, "Alice", 64.2f};
        std::int32_t score = 1200;

        zipPacket << alice << score;

        // Simulate transmission
        ZipPacket receivedZip = zipPacket;

        Character receivedAlice;
        std::int32_t receivedScore;

        receivedZip >> receivedAlice >> receivedScore;

        std::cout << "\nAfter decompression:\n";
        std::cout << "Name: " << receivedAlice.name
                  << ", Age: " << static_cast<int>(receivedAlice.age)
                  << ", Weight: " << receivedAlice.weight
                  << ", Score: " << receivedScore << "\n";
    }

    return 0;
}