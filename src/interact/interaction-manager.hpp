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
#ifndef HEROESPATH_INTERACT_INTERACTION_MANAGER_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACTION_MANAGER_HPP_INCLUDED
//
// interaction-manager.hpp
//
#include <memory>

namespace heroespath
{
namespace interact
{

    struct IInteraction;
    using InteractionUPtr_t = std::unique_ptr<IInteraction>;

    // Responsible for the lifetimes of interactions.
    class InteractionManager
    {
    public:
        InteractionManager(const InteractionManager &) = delete;
        InteractionManager(const InteractionManager &&) = delete;
        InteractionManager & operator=(const InteractionManager &) = delete;
        InteractionManager & operator=(const InteractionManager &&) = delete;

    public:
        InteractionManager();
        ~InteractionManager();

        IInteraction * Current() { return currentUPtr_.get(); }
        bool HasCurrentChanged() const;
        void RemoveCurrent();
        void SetNext(InteractionUPtr_t);
        bool Update();
        void Lock();
        void Unlock();
        bool IsLocked() const;

    private:
        InteractionUPtr_t currentUPtr_;
        InteractionUPtr_t nextUPtr_;
        mutable bool hasCurrentChanged_;
        bool isRemoveCurrentPending_;
    };
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACTION_MANAGER_HPP_INCLUDED
