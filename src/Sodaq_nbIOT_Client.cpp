#include "Sodaq_nbIOT_Client.h"

Sodaq_nbIOT_Client::Sodaq_nbIOT_Client(Sodaq_nbIOT* nbiot):
 _nbiot(nbiot),
 _socket(-1),
 _remotePort(0),
 _connected(false) {}

Sodaq_nbIOT_Client::~Sodaq_nbIOT_Client() {
    stop();
}

int Sodaq_nbIOT_Client::connect(IPAddress ip, uint16_t port) {
    _remoteIP = ip;
    _remotePort = port;

    _socket = _nbiot->createTCPSocket();
    if (_socket < 0) {
        return 0;
    }

    if (_nbiot->connectTCPSocket(_socket, _remoteIP.toString().c_str(), _remotePort)) {
        _connected = true;

        return 1;
    }

    return 0;
}

int Sodaq_nbIOT_Client::connect(const char* host, uint16_t port) {
    IPAddress ip;
    ip.fromString(host);

    return connect(ip, port);
}

size_t Sodaq_nbIOT_Client::write(uint8_t data) {
    return write(&data, 1);
}

size_t Sodaq_nbIOT_Client::write(const uint8_t *buf, size_t size) {
    return _nbiot->writeTCPSocket(_socket, (char*)buf, size);
}

int Sodaq_nbIOT_Client::available() {
    if (_nbiot->hasPendingTCPBytes()) {
        return (int)_nbiot->getPendingTCPBytes();
    }

    return 0;
}

int Sodaq_nbIOT_Client::read() {
    uint8_t data;
    int res = _nbiot->receiveBytesTCPSocket(&data, 1);

    if (res < 0) {
        return res;
    }

    return data;
}

int Sodaq_nbIOT_Client::read(uint8_t* buf, size_t size) {
    return _nbiot->receiveBytesTCPSocket(buf, size);
}

void Sodaq_nbIOT_Client::flush() {

}

void Sodaq_nbIOT_Client::stop() {

}

uint8_t Sodaq_nbIOT_Client::connected() {
    return (uint8_t)_connected;
}

int Sodaq_nbIOT_Client::peek() {
    return available();
}
