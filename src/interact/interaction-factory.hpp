// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_INTERACTION_FACTORY_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACTION_FACTORY_HPP_INCLUDED
//
// interaction-factory.hpp
//
#include "interact/i-interaction.hpp"
#include "interact/interaction-text-enum.hpp"
#include "map/transition.hpp"

namespace heroespath
{
namespace interact
{

    // Responsible for the construction of all Interaction objects.
    struct InteractionFactory
    {
        static InteractionUPtr_t MakeLockedDoor(const map::Transition &);
    };
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
