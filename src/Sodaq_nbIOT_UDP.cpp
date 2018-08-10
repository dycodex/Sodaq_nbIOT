#include "Sodaq_nbIOT_UDP.h"

Sodaq_nbIOT_UDP::Sodaq_nbIOT_UDP(Sodaq_nbIOT& nbiot):
 _nbiot(&nbiot),
 _socket(-1),
 _remotePort(0),
 _txBuffer(0),
 _txBufferLength(0),
 _rxBuffer(0) {}

Sodaq_nbIOT_UDP::~Sodaq_nbIOT_UDP() {
    stop();
}

uint8_t Sodaq_nbIOT_UDP::begin(uint16_t port) {
    // UDP server is not supported yet
    return 0;
}

void Sodaq_nbIOT_UDP::stop() {
    if (_txBuffer) {
        delete _txBuffer;
        _txBuffer = NULL;
    }

    if (_rxBuffer) {
        delete _rxBuffer;
        _rxBuffer = NULL;
    }

    if (_nbiot->closeSocket(_socket)) {
        _socket = -1;
    }
}

int Sodaq_nbIOT_UDP::beginPacket() {
    if (_remotePort == 0) {
        return 0;
    }

    if (!_txBuffer) {
        _txBuffer = new char[512];
        if (!_txBuffer) {
            return 0;
        }
    }

    _txBufferLength = 0;
    if (_socket == -1) {
        _socket = _nbiot->createSocket();
        if (_socket == -1) {
            return 0;
        }
    }

    return 1;
}

int Sodaq_nbIOT_UDP::beginPacket(IPAddress ip, uint16_t port) {
    _remoteAddress = ip;
    _remotePort = port;

    return beginPacket();
}

int Sodaq_nbIOT_UDP::beginPacket(const char* host, uint16_t port) {
    IPAddress ip;
    if (ip.fromString(host)) {
        _remoteAddress = ip;
        _remotePort = port;

        return beginPacket();
    }

    return 0;
}

int Sodaq_nbIOT_UDP::endPacket() {
    int sent = _nbiot->socketSend(
        _socket,
        _remoteAddress.toString().c_str(),
        _remotePort,
        _txBuffer,
        _txBufferLength
    );

    if (sent < 0) {
        return 0;
    }

    return 1;
}

size_t Sodaq_nbIOT_UDP::write(uint8_t value) {
    if (_txBufferLength == 512) {
        endPacket();
        _txBufferLength = 0;
    }

    _txBuffer[_txBufferLength++] = value;

    return 1;
}

size_t Sodaq_nbIOT_UDP::write(const uint8_t* buffer, size_t size) {
    size_t i;
    for (i = 0; i < size; i++) {
        write(buffer[i]);
    }

    return i;
}

int Sodaq_nbIOT_UDP::parsePacket() {
    if (_rxBuffer) {
        return 0;
    }

    uint8_t buffer[512] = {0};
    int readLength = _nbiot->socketReceiveBytes(buffer, 512);
    if (readLength == -1) {
        return 0;
    }

    _rxBuffer = new cbuf(readLength);
    _rxBuffer->write((const char*)buffer, readLength);

    return readLength;
}

int Sodaq_nbIOT_UDP::available() {
    if (!_rxBuffer) {
        return 0;
    }

    return _rxBuffer->available();
}

int Sodaq_nbIOT_UDP::read() {
    if (!_rxBuffer) {
        return -1;
    }

    int out = _rxBuffer->read();
    if (!_rxBuffer->available()) {
        cbuf* b = _rxBuffer;
        _rxBuffer = 0;
        delete b;
    }

    return out;
}

int Sodaq_nbIOT_UDP::read(unsigned char* buffer, size_t length) {
    return read((char*)buffer, length);
}

int Sodaq_nbIOT_UDP::read(char* buffer, size_t length) {
    if (!_rxBuffer) {
        return 0;
    }

    int out = _rxBuffer->read(buffer, length);
    if (!_rxBuffer->available()) {
        cbuf *b = _rxBuffer;
        _rxBuffer = 0;
        delete b;
    }

    return out;
}

int Sodaq_nbIOT_UDP::peek() {
    if (!_rxBuffer) {
        return -1;
    }

    return _rxBuffer->peek();
}

void Sodaq_nbIOT_UDP::flush() {
    cbuf* b = _rxBuffer;
    _rxBuffer = 0;
    delete b;
}

IPAddress Sodaq_nbIOT_UDP::remoteIP() {
    return _remoteAddress;
}

uint16_t Sodaq_nbIOT_UDP::remotePort() {
    return _remotePort;
}
