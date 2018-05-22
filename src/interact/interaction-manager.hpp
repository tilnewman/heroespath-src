// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_INTERACTION_MANAGER_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACTION_MANAGER_HPP_INCLUDED
//
// interaction-manager.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include <memory>

namespace heroespath
{
namespace interact
{

    struct IInteraction;
    using IInteractionPtr_t = misc::NotNull<IInteraction *>;
    using IInteractionPtrOpt_t = boost::optional<IInteractionPtr_t>;
    using InteractionUPtr_t = std::unique_ptr<IInteraction>;

    // Responsible for the lifetimes of interactions and for providing access to them.
    class InteractionManager
    {
    public:
        InteractionManager(const InteractionManager &) = delete;
        InteractionManager(InteractionManager &&) = delete;
        InteractionManager & operator=(const InteractionManager &) = delete;
        InteractionManager & operator=(InteractionManager &&) = delete;

    public:
        InteractionManager();
        ~InteractionManager();

        const IInteractionPtrOpt_t Current();
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
