#pragma once


class IUICallback {
public:
    virtual void touchDown(TouchEvent &touchEvent) = 0;
    virtual void touchUp(TouchEvent &touchEvent) = 0;
    virtual void touchHold(TouchEvent &touchEvent) = 0;
    virtual void touchLongHold(TouchEvent &touchEvent) = 0;
    virtual void touchClick(TouchEvent &touchEvent) = 0;
    virtual void touchDoubleClick(TouchEvent &touchEvent) = 0;

    virtual void potTouchDown(uint8_t potId) = 0;
    virtual void potTouchUp(uint8_t potId) = 0;
    virtual void potMove(uint8_t potId, int16_t relativeChange) = 0;

    virtual void buttonUp(uint8_t buttonId) = 0;
    virtual void buttonDown(uint8_t buttonId) = 0;
    virtual void buttonLongHold(uint8_t buttonId) = 0;
protected:
    virtual ~IUICallback() = default;
};


