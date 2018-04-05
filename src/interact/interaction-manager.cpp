// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// interaction-manager.cpp
//
#include "interaction-manager.hpp"
#include "interact/i-interaction.hpp"
#include "log/log-macros.hpp"

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
