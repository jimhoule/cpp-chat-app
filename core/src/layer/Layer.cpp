#include "layer/Layer.h"

// **********
// * PUBLIC *
// **********
Layer::Layer(const std::string& ID, const std::shared_ptr<Logger>& Logger) : m_ID(ID), m_Logger(Logger)
{}

void Layer::OnAttach()
{}

void Layer::OnDetach()
{}

void Layer::OnUpdate()
{}

void Layer::OnSuspend()
{}

void Layer::OnUnsuspend()
{}

std::string& Layer::GetID()
{
    return m_ID;
}

bool Layer::IsSuspended() const
{
    return m_IsSuspended;
}

void Layer::Suspend()
{
    m_IsSuspended = true;
}

void Layer::Unsuspend()
{
    m_IsSuspended = false;
}
