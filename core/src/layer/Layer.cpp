#include "layer/Layer.h"

// **********
// * PUBLIC *
// **********
Layer::Layer(const std::string& id, Logger& logger)
    : m_id(id)
    , m_logger(logger)
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

const std::string& Layer::GetId() const
{
    return m_id;
}

bool Layer::IsSuspended() const
{
    return m_isSuspended;
}

void Layer::Suspend()
{
    m_isSuspended = true;
}

void Layer::Unsuspend()
{
    m_isSuspended = false;
}
