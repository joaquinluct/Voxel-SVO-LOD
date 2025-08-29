#pragma once

// Clase base para todos los eventos
template <typename TEventType>
class Event {
public:
    virtual ~Event() = default;
    virtual TEventType GetType() const = 0;
};