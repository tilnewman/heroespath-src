// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-manager.cpp
//
#include "interact/i-interaction.hpp"
#include "interaction-manager.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace interact
{

    InteractionManager::InteractionManager()
        : currentUPtr_()
        , nextUPtr_()
        , hasCurrentChanged_(true)
        , isRemoveCurrentPending_(false)
    {}

    InteractionManager::~InteractionManager() = default;

    const IInteractionPtrOpt_t InteractionManager::Current()
    {
        if (currentUPtr_)
        {
            return IInteractionPtr_t(currentUPtr_.get());
        }
        else
        {
            return boost::none;
        }
    }

    bool InteractionManager::HasCurrentChanged() const
    {
        if (hasCurrentChanged_)
        {
            hasCurrentChanged_ = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    void InteractionManager::RemoveCurrent() { isRemoveCurrentPending_ = true; }

    void InteractionManager::SetNext(InteractionUPtr_t NEXT_UPTR)
    {
        nextUPtr_ = std::move(NEXT_UPTR);
    }

    bool InteractionManager::Update()
    {
        if (isRemoveCurrentPending_)
        {
            isRemoveCurrentPending_ = false;
            currentUPtr_.reset();
            hasCurrentChanged_ = true;
        }

        if (nextUPtr_)
        {
            if (currentUPtr_)
            {
                currentUPtr_->PlayExitSfx();
            }

            currentUPtr_.swap(nextUPtr_);

            nextUPtr_.reset();
            currentUPtr_->PlayEnterSfx();
            hasCurrentChanged_ = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    void InteractionManager::Lock()
    {
        if (currentUPtr_)
        {
            currentUPtr_->Lock();
        }
    }

    void InteractionManager::Unlock()
    {
        if (currentUPtr_)
        {
            currentUPtr_->Unlock();
        }
    }

    bool InteractionManager::IsLocked() const
    {
        if (currentUPtr_)
        {
            return currentUPtr_->IsLocked();
        }
        else
        {
            return false;
        }
    }

} // namespace interact
} // namespace heroespath
