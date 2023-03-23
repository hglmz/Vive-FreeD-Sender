#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <array>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <iterator>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <openvr.h>

constexpr uint8_t MESSAGE_IDENTIFIER = 0xD1;
constexpr size_t MESSAGE_SIZE = 29;

std::array<uint8_t, MESSAGE_SIZE> createMessage(float pitch, float yaw, float roll, float x, float y, float z)
{
    std::array<uint8_t, MESSAGE_SIZE> message{};
    message[0] = MESSAGE_IDENTIFIER;
    message[1] = 0x00; // Camera ID

    auto floatToBytes = [](float value) {
        uint32_t intValue = *reinterpret_cast<uint32_t*>(&value);
        return std::array<uint8_t, 4>{static_cast<uint8_t>(intValue),
            static_cast<uint8_t>(intValue >> 8),
            static_cast<uint8_t>(intValue >> 16),
            static_cast<uint8_t>(intValue >> 24)};
    };

    auto pitchBytes = floatToBytes(pitch);
    auto yawBytes = floatToBytes(yaw);
    auto rollBytes = floatToBytes(roll);
    auto xBytes = floatToBytes(x);
    auto yBytes = floatToBytes(y);
    auto zBytes = floatToBytes(z);

    std::copy(pitchBytes.begin(), pitchBytes.end(), message.begin() + 2);
    std::copy(yawBytes.begin(), yawBytes.end(), message.begin() + 6);
    std::copy(rollBytes.begin(), rollBytes.end(), message.begin() + 10);
    std::copy(zBytes.begin(), zBytes.end(), message.begin() + 14);
    std::copy(yBytes.begin(), yBytes.end(), message.begin() + 18);
    std::copy(xBytes.begin(), xBytes.end(), message.begin() + 22);

    // Zoom and focus not used
    message[26] = 0x00;
    message[27] = 0x00;
    message[28] = 0x00;

    uint8_t checksum = 0;
    for (size_t i = 0; i < MESSAGE_SIZE - 1; ++i)
    {
        checksum += message[i];
    }
    message[28] = checksum;

    return message;
}

int main()
{
    // Konfig dosyasý adý
    const std::string CONFIG_FILE_NAME = "config.txt";

    std::string ipAddress;
    uint16_t port;

    // Konfig dosyasýndan okuma
    std::ifstream configFile(CONFIG_FILE_NAME);
    if (configFile.is_open())
    {
        configFile >> ipAddress >> port;
        configFile.close();
    }
    else
    {
        std::cerr << "Konfig dosyasý açýlamadý!" << std::endl;
        std::cout << "IP adresi: ";
        std::cin >> ipAddress;
        std::cout << "Port numarasý: ";
        std::cin >> port;

        // Kullanýcýnýn girdiði IP adresi ve port numarasýný kaydet
        std::ofstream newConfigFile(CONFIG_FILE_NAME);
        if (newConfigFile.is_open())
        {
            newConfigFile << ipAddress << std::endl << port << std::endl;
            newConfigFile.close();
        }
        else
        {
            std::cerr << "Konfig dosyasi olusturulamadi!" << std::endl;
        }
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;

        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    // Initialize OpenVR
    vr::EVRInitError initError = vr::VRInitError_None;
    if (!vr::VR_IsRuntimeInstalled() || !vr::VR_IsHmdPresent())
    {
        std::cerr << "No VR headset found" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    vr::IVRSystem* vrSystem = vr::VR_Init(&initError, vr::VRApplication_Background);
    if (initError != vr::VRInitError_None)
    {
        std::cerr << "Failed to initialize VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(initError) << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        vr::VRControllerState_t controllerState;
        vr::TrackedDevicePose_t trackedDevicePose;
        vr::ETrackedDeviceClass trackedDeviceClass;

        for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
        {
            trackedDeviceClass = vrSystem->GetTrackedDeviceClass(deviceIndex);
            if (trackedDeviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker)
            {
                vrSystem->GetControllerStateWithPose(vr::TrackingUniverseStanding, deviceIndex, &controllerState, sizeof(controllerState), &trackedDevicePose);
                if (trackedDevicePose.bPoseIsValid)
                {
                    vr::HmdMatrix34_t pose = trackedDevicePose.mDeviceToAbsoluteTracking;
                    float pitch = atan2f(pose.m[2][0], pose.m[2][2]);
                    float yaw = atan2f(pose.m[0][2], pose.m[2][2]);
                    float roll = atan2f(pose.m[1][0], pose.m[1][1]);

                    float x = pose.m[0][3];
                    float y = pose.m[1][3];
                    float z = pose.m[2][3];

                    // Anlýk konumu ekrana yazdýrma
                    std::cout << "X: " << x << " Y: " << y << " Z: " << z << std::endl;

                    auto message = createMessage(pitch, yaw, roll, x, y, z);
                    sendto(sock, reinterpret_cast<char*>(message.data()), message.size(), 0, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    vr::VR_Shutdown();
    closesocket(sock);
    WSACleanup();

    //return 0
// std::cout << "Çýkmak için bir tuþa basýn...\n";
// std::cin.get();
//return 0;

    system("pause");
    return 0;
}