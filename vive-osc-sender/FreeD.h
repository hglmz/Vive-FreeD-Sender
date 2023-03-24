#ifndef __FREED_H__
#define __FREED_H__

#include <cstdint>

struct FreeDPacket {
    uint8_t messageType; // 1 byte
    uint8_t cameraID; // 1 byte
    float pitch; // 4 bytes
    float yaw; // 4 bytes
    float roll; // 4 bytes
    float posZ; // 4 bytes
    float posY; // 4 bytes
    float posX; // 4 bytes
    uint16_t zoom; // 2 bytes
    uint16_t focus; // 2 bytes
    uint16_t reserved; // 2 bytes
    uint8_t checksum; // 1 byte
};

FreeDPacket createFreeDPacket(uint8_t cameraID, float pitch, float yaw, float roll, float posZ, float posY, float posX, uint16_t zoom, uint16_t focus);

int sendFreeDPacket(const char* serverIP, int serverPort, const FreeDPacket& packet);

#endif // __FREED_H__
