// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_LOCKED_DOOR_HPP_INCLUDED
#define HEROESPATH_INTERACT_LOCKED_DOOR_HPP_INCLUDED
//
// locked-door.hpp
//
#include "interact/interaction-base.hpp"
#include "map/transition.hpp"

namespace heroespath
{
namespace interact
{

    // Responsible for interactions with locked doors.
    class LockedDoor : public InteractionBase
    {
    public:
        explicit LockedDoor(const map::Transition &);

        virtual ~LockedDoor() = default;

        map::Level::Enum ToLevel() const { return transition_.WhichLevel(); }

        bool OnSuccess(const stage::InteractStagePtr_t) final;

    private:
        bool OnInteraction(const stage::InteractStagePtr_t, const Button &) final;

    private:
        map::Transition transition_;
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_LOCKED_DOOR_HPP_INCLUDED
