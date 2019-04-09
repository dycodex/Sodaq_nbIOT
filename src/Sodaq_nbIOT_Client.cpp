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

    if (!_nbiot->connectTCPSocket(_socket, _remoteIP.toString().c_str(), _remotePort)) {
        return 0;
    }

    return 1;
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
    if (_socket > -1) {
        return _nbiot->getPendingTCPBytes(_socket);
    }

    return 0;
}

int Sodaq_nbIOT_Client::read() {
    byte data;

    if (_peek != 1) {
        data = _peek;
        _peek = -1;

        return data;
    }

    int res = read(&data, 1);
    if (res < 0) {
        return res;
    }

    return data;
}

int Sodaq_nbIOT_Client::read(uint8_t* buf, size_t size) {
    return _nbiot->receiveHexTCPSocket((char*)buf, size);
}

void Sodaq_nbIOT_Client::flush() {

}

void Sodaq_nbIOT_Client::stop() {
    if (_socket < 0) {
        return;
    }

    _nbiot->closeSocket(_socket);

    _socket = -1;
}

uint8_t Sodaq_nbIOT_Client::connected(){
    if (_socket < 0) {
        return 0;
    }

    SaraR4SocketStatus status = _nbiot->getTCPSocketStatus(_socket);

    return status != SOCKET_INACTIVE && status != SOCKET_UNKNOWN_STATUS;
}

int Sodaq_nbIOT_Client::peek() {
    if (_peek == -1) {
        _peek = read();
    }

    return _peek;
}
