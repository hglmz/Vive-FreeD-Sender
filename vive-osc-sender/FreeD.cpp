#include "FreeD.h"

FreeDPacket createFreeDPacket(uint8_t cameraID, float pitch, float yaw, float roll, float posZ, float posY, float posX, uint16_t zoom, uint16_t focus) {
    FreeDPacket packet;

    // set message type
    packet.messageType = 0xD1;

    // set camera ID
    packet.cameraID = cameraID;

    // set pitch, yaw, roll
    packet.pitch = pitch;
    packet.yaw = yaw;
    packet.roll = roll;

    // set position
    packet.posZ = posZ;
    packet.posY = posY;
    packet.posX = posX;

    // set zoom and focus
    packet.zoom = zoom;
    packet.focus = focus;

    // set reserved field
    packet.reserved = 0;

    // calculate and set checksum
    packet.checksum = 0;
    for (int i = 0; i < 28; i++) {
        packet.checksum += ((uint8_t*)&packet)[i];
    }

    return packet;
}
