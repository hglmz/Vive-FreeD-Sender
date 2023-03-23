#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Set up the message format
struct message {
    char messageType;
    char cameraID;
    float pitch;
    float yaw;
    float roll;
    float posZ;
    float posY;
    float posX;
    int zoom;
    int focus;
    short reserved;
    short checksum;
};

// Calculate the checksum for the message
short calculateChecksum(message m) {
    short sum = 0;
    char* ptr = (char*)&m;

    for (int i = 0; i < 28; i++) {
        sum += (short)*ptr;
        ptr++;
    }

    return sum;
}

int main() {
    // Set up the message to be sent
    message m;
    m.messageType = 0xD1;
    m.cameraID = 0x01;
    m.pitch = 45.0f;
    m.yaw = 30.0f;
    m.roll = 15.0f;
    m.posZ = 100.0f;
    m.posY = 50.0f;
    m.posX = 25.0f;
    m.zoom = 25;
    m.focus = 25;
    m.reserved = 0;
    m.checksum = calculateChecksum(m);

    // Set up the address to send to
    sockaddr_in si_other;
    int slen = sizeof(si_other);
    char* ip_address = "192.168.3.255";
    int port = 7004;

    // Initialize winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Failed. Error Code : " << WSAGetLastError();
        return 1;
    }

    // Create a socket
    SOCKET s;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        cout << "socket() failed with error code : " << WSAGetLastError();
        return 1;
    }

    // Set up the address to send to
    memset((char*)&si_other, 0, slen);
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    si_other.sin_addr.S_un.S_addr = inet_addr(ip_address);

    // Send the message
    if (sendto(s, (char*)&m, sizeof(m), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
        cout << "sendto() failed with error code : " << WSAGetLastError();
        return 1;
    }

    // Close the socket and cleanup winsock
    closesocket(s);
    WSACleanup();

}