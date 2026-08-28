#pragma once

#include "observer/IObserver.h"

template<typename TEvent, typename TOwner>
class Observer : public IObserver<TEvent>
{
public:
    using Method =  void (TOwner::*)(const TEvent&);

    Observer(TOwner& owner, Method method)
        : m_owner(owner)
        , m_method(method)
    {}

    void OnNotify(const TEvent& event) override
    {
        (m_owner.*m_method)(event);
    }

private:
    TOwner& m_owner;
    Method m_method = nullptr;
};
