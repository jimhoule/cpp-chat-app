#pragma once

#include "observer/ISubject.h"

#include <algorithm>
#include <vector>

template<typename TEvent>
class Subject : public ISubject<TEvent>
{
public: 
    Subject() = default;
    ~Subject() override = default;

    void Subscribe(IObserver<TEvent>* observer) override
    {
        if (observer == nullptr)
        {
            return;
        }

        // NOTE: A layer reattached without a matching detach must not be notified twice
        ObserverIterator observerIterator = std::find(m_observers.begin(), m_observers.end(), observer);
        if (observerIterator != m_observers.end())
        {
            return;
        }

        m_observers.push_back(observer);
    }

    void Unsubscribe(IObserver<TEvent>* observer) override
    {
        ObserverIterator observerIterator = std::find(m_observers.begin(), m_observers.end(), observer);
        if (observerIterator == m_observers.end())
        {
            return;
        }

        // Tombstones instead of erasing, Notify() may be iterating this m_observers right now
        *observerIterator = nullptr;
        m_hasDetachedObservers = true;

        if (m_notifyDepth == 0)
        {
            CompactObservers();
        }
    }

    void Notify(const TEvent& event) override
    {
        // Snapshots the size so an observer attaching from insinde OnNotify is not called for the event alrady in flight
        const std::size_t observersSize = m_observers.size();
        m_notifyDepth++;

        for (std::size_t index = 0; index < observersSize; index++)
        {
            IObserver<TEvent>* observer = m_observers[index];
            if (observer != nullptr)
            {
                observer->OnNotify(event);
            }
        }

        m_notifyDepth--;
        if (m_notifyDepth == 0)
        {
            CompactObservers();
        }
    }

private:
    using Observers = std::vector<IObserver<TEvent>*>;
    using ObserverIterator = typename Observers::iterator;

    Observers m_observers = {};
    std::size_t m_notifyDepth = 0;
    bool m_hasDetachedObservers = false;

    void CompactObservers()
    {
        if (!m_hasDetachedObservers)
        {
            return;
        }

        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), nullptr), m_observers.end());
        m_hasDetachedObservers = false;
    }
};
