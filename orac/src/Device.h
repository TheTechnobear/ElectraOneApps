#pragma once

#include <stdint.h>

#include "Message.h"

class Device {
public:
    Device() : id(0), port(0), channel(0), rate(0), tsLastMessage(0) {}

    Device(uint8_t newId, uint8_t newPort, uint8_t newChannel, uint16_t newRate)
        : id(newId), port(newPort), channel(newChannel), rate(newRate),
          tsLastMessage(0) {}

    uint8_t getId(void) const { return (id); }

    void setPort(uint8_t newPort) { port = newPort; }

    uint8_t getPort(void) const { return (port); }

    void setChannel(uint8_t newChannel) { channel = newChannel; }

    uint8_t getChannel(void) const { return (channel); }

    uint16_t getRate(void) const { return (rate); }

    void setTsLastMessage(void) { tsLastMessage = millis(); }

    bool isReady(void) {
        return (((millis() - tsLastMessage) > rate) ? true : false);
    }

private:
    uint8_t id;
    uint8_t port;
    uint8_t channel;
    uint16_t rate;
    uint32_t tsLastMessage;
};
