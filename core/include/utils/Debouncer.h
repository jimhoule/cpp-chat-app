#pragma once

#include <chrono>
#include <string>

class Debouncer
{
public:
    Debouncer(std::chrono::milliseconds delay);

    /**
     * NOTES:
     *  - Called every frame
     *  - Returns once when value stopped changing for the delay and was not emitted yet
     */
    bool Update(const std::string& value);
    void Reset();

private:
    std::string m_lastValue = "";
    std::string m_lastEmittedValue = "";
    std::chrono::milliseconds m_delay = {};
    std::chrono::steady_clock::time_point m_changedAt = {};
};
