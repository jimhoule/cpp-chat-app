#pragma once

#include "observer/IObserver.h"

template<typename TEvent>
class IObservable
{
public:
    IObservable() = default;
    virtual ~IObservable() = default;

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    IObservable(const IObservable&) = delete;
    IObservable& operator=(const IObservable&) = delete;
    IObservable(IObservable&&) = delete;
    IObservable& operator=(IObservable&&) = delete;

    virtual void Subscribe(IObserver<TEvent>* observer) = 0;
    virtual void Unsubscribe(IObserver<TEvent>* observer) = 0;
};
