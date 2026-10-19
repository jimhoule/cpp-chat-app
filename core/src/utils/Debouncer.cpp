#include "utils/Debouncer.h"

// **********
// * PUBLIC *
// **********
Debouncer::Debouncer(std::chrono::milliseconds delay)
    : m_delay(delay)
{}

bool Debouncer::Update(const std::string& value)
{
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // Restarts delay every time value changes
    if (value != m_lastValue)
    {
        m_lastValue = value;
        m_changedAt = now;
    }

    // Waits until value stopped changing for the whole delay
    const bool isDelayElapsed = now - m_changedAt >= m_delay;
    if (!isDelayElapsed)
    {
        return false;
    }

    // Emits only once per stable value
    if (value == m_lastEmittedValue)
    {
        return false;
    }

    m_lastEmittedValue = value;

    return true;
}

void Debouncer::Reset()
{
    m_lastValue = "";
    m_lastEmittedValue = "";
    m_changedAt = {};
}
