#pragma once

template<typename TEvent>
class IObserver
{
public:
    IObserver() = default;
    virtual ~IObserver() = default;

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    IObserver(const IObserver&) = delete;
    IObserver& operator=(const IObserver&) = delete;
    IObserver(IObserver&&) = delete;
    IObserver& operator=(IObserver&&) = delete;

    virtual void OnNotify(const TEvent& event) = 0;

};
