#pragma once

#include "observer/IObservable.h"

template<typename TEvent>
class ISubject : public IObservable<TEvent>
{
public:
    ISubject() = default;
    virtual ~ISubject() = default;

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    ISubject(const ISubject&) = delete;
    ISubject& operator=(const ISubject&) = delete;
    ISubject(ISubject&&) = delete;
    ISubject& operator=(ISubject&&) = delete;

    virtual void Notify(const TEvent& event) = 0;
};
