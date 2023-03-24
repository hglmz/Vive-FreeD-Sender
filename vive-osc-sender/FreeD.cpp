#include "FreeD.h"
#include <cstring>

FreeD::FreeD()
    : m_pitch(0.0f), m_yaw(0.0f), m_roll(0.0f),
    m_posZ(0.0f), m_posX(0.0f), m_posY(0.0f),
    m_zoom(0), m_focus(0) {}

FreeD::~FreeD() {}

void FreeD::setPitch(float pitch) { m_pitch = pitch; }
void FreeD::setYaw(float yaw) { m_yaw = yaw; }
void FreeD::setRoll(float roll) { m_roll = roll; }
void FreeD::setPosZ(float posZ) { m_posZ = posZ; }
void FreeD::setPosX(float posX) { m_posX = posX; }
void FreeD::setPosY(float posY) { m_posY = posY; }
void FreeD::setZoom(int zoom) { m_zoom = zoom; }
void FreeD::setFocus(int focus) { m_focus = focus; }


void FreeD::setQuaternion(float w, float x, float y, float z) {
    m_quaternion.w = w;
    m_quaternion.x = x;
    m_quaternion.y = y;
    m_quaternion.z = z;
}


// Yeni eklenen setPosition fonksiyonu
void FreeD::setPosition(float x, float y, float z) {
    m_posX = x;
    m_posY = y;
    m_posZ = z;
}

void FreeD::encode(uint8_t* buffer) const {
    buffer[0] = 0xD1;
    // ID deðerini ayarlamak için buffer[1] = ... kullanýn
    memcpy(buffer + 2, &m_pitch, sizeof(float));
    memcpy(buffer + 6, &m_yaw, sizeof(float));
    memcpy(buffer + 10, &m_roll, sizeof(float));
    memcpy(buffer + 14, &m_posZ, sizeof(float));
    memcpy(buffer + 18, &m_posX, sizeof(float));
    memcpy(buffer + 22, &m_posY, sizeof(float));
    memcpy(buffer + 26, &m_zoom, sizeof(int16_t));
    memcpy(buffer + 28, &m_focus, sizeof(int16_t));

    uint8_t checksum = calculateChecksum(buffer, 28);
    buffer[28] = checksum;
}

uint8_t FreeD::calculateChecksum(const uint8_t* buffer, size_t length) const {
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum += buffer[i];
    }
    return checksum;
}
