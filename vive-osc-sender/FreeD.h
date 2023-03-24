#pragma once

#include <cstdint>
#include <openvr.h>

class FreeD {
public:
    FreeD();
    ~FreeD();
    void setPosition(float x, float y, float z);
    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);
    void setPosZ(float posZ);
    void setPosX(float posX);
    void setPosY(float posY);
    void setZoom(int zoom);
    void setFocus(int focus);

    void setQuaternion(float w, float x, float y, float z);
    void createDataPacket();
    void sendDataPacket();

    void encode(uint8_t* buffer) const;

private:
    float m_pitch;
    float m_yaw;
    float m_roll;
    float m_posZ;
    float m_posX;
    float m_posY;
    int m_zoom;
    int m_focus;

    vr::HmdQuaternion_t m_quaternion;

    uint8_t calculateChecksum(const uint8_t* buffer, size_t length) const;
};
