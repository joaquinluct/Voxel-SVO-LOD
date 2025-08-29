#pragma once

#include <IEvent.h>

template <typename TEventType>
class IEventListener {
public:
    virtual ~IEventListener() = default;
    // El método que recibirá el evento
    virtual void OnEvent(const IEvent& event) = 0;
};