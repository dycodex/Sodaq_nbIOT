#include "Sodaq_nbIOT_Client.h"

Sodaq_nbIOT_Client::Sodaq_nbIOT_Client(Sodaq_nbIOT* nbiot):
 _nbiot(nbiot),
 _socket(-1),
 _remotePort(0),
 _connected(false),
 _shouldReconnect(false),
 _errorCount(0) {}

Sodaq_nbIOT_Client::~Sodaq_nbIOT_Client() {
    stop();
}

int Sodaq_nbIOT_Client::connect(IPAddress ip, uint16_t port) {
    _remoteIP = ip;
    _remotePort = port;

    bool isConnected = _socket > -1 &&  _connected;
    if (!isConnected) {
        _socket = _nbiot->createTCPSocket();
        if (_socket < 0) {
            _connected = false;
            return 0;
        }

        _nbiot->setSocketOptions(_socket, SOO_LEVEL_SOCKET, (int)SOO_SOCKET_KEEP_ALIVE, 1);

        if (!_nbiot->connectTCPSocket(_socket, _remoteIP.toString().c_str(), _remotePort)) {
            _connected = false;
            return 0;
        }

        _connected = true;
        _errorCount = 0;
    }

    _nbiot->enableHex();

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
    size_t toBeWritten = size;
    size_t chunkSize = 0;
    size_t written = 0;

    while (toBeWritten) {
        chunkSize = toBeWritten;
        if (chunkSize > 256) {
            chunkSize = 256;
        }

        char tempBuf[chunkSize] = {0};
        memcpy(tempBuf, buf + written, chunkSize);

        size_t writeTemp = _nbiot->writeTCPSocket(_socket, tempBuf, chunkSize);
        if (writeTemp < 0) {
            return written;
        }

        written += chunkSize;
        toBeWritten -= chunkSize;
    }

    return written;
}

int Sodaq_nbIOT_Client::available() {
    if (_socket > -1) {
        int pendingBytes = _nbiot->getPendingTCPBytes(_socket);
        Serial.printf("AVAILABLE BYTES: %d\n", pendingBytes);

        if (pendingBytes >= 0) {
            return pendingBytes;
        } else {
            incrementError();
        }
    }

    return -1;
}

int Sodaq_nbIOT_Client::read() {
    byte data;

    if (_peek != -1) {
        data = _peek;
        _peek = -1;

        return data;
    }

    if (read(&data, 1)  == 1) {
        return data;
    }

    return -1;
}

int Sodaq_nbIOT_Client::read(uint8_t* buf, size_t size) {
    uint8_t tempBuff[size];
    memset(tempBuff, 0, size);

    size_t toBeRead = size;
    size_t hasRead = 0;
    while (toBeRead) {
        size_t chunk = toBeRead;
        if (chunk > 60) {
            chunk = 60;
        }

        size_t nowRead = _nbiot->receiveBytesTCPSocket(_socket, tempBuff + hasRead, chunk);
        if (nowRead < 0) {
            incrementError();
            memcpy(buf, tempBuff, hasRead);

            return hasRead;
        }

        hasRead += nowRead;
        toBeRead -= nowRead;
    }

    memcpy(buf, tempBuff, hasRead);
    return hasRead;
}

void Sodaq_nbIOT_Client::flush() {

}

void Sodaq_nbIOT_Client::stop() {
    if (_socket < 0) {
        return;
    }

    Serial.println("Closing socket");
    _nbiot->closeSocket(_socket);

    _socket = -1;
    _connected = false;
}

uint8_t Sodaq_nbIOT_Client::connected()
{
    if (_socket < 0) {
        return 0;
    }

    SaraR4SocketStatus status = _nbiot->getTCPSocketStatus(_socket);
    uint8_t connected = (uint8_t)(status != SOCKET_INACTIVE && status != SOCKET_UNKNOWN_STATUS);
    _connected = (bool)connected;

    return connected;
    // return _connected;
}

int Sodaq_nbIOT_Client::peek() {
    if (_peek == -1) {
        _peek = read();
    }

    return _peek;
}


void Sodaq_nbIOT_Client::incrementError() {
    _errorCount++;
    if (_errorCount > 5) {
        _connected = false;
    }
}
