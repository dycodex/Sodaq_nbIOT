#ifndef SODAQ_NBIOT_CLIENT_H
#define SODAQ_NBIOT_CLIENT_H

#include <Client.h>
#include <cbuf.h>
#include "Sodaq_nbIOT.h"

class Sodaq_nbIOT_Client: public Client {
public:
    Sodaq_nbIOT_Client(Sodaq_nbIOT* nbiot);
    ~Sodaq_nbIOT_Client();
    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int available();
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    
    operator bool() {
        return connected();
    }

private:
    Sodaq_nbIOT* _nbiot;
    int _socket;
    IPAddress _remoteIP;
    uint16_t _remotePort;
    bool _connected;

    int _peek;
};

#endif
