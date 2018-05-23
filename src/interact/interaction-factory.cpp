// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-factory.cpp
//
#include "interaction-factory.hpp"

#include "interact/conversation.hpp"
#include "interact/locked-door.hpp"

#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace interact
{

    InteractionUPtr_t InteractionFactory::MakeLockedDoor(const map::Transition & TRANSITION)
    {
        return std::make_unique<LockedDoor>(TRANSITION);
    }

    InteractionUPtr_t InteractionFactory::MakeConversation(
        const game::NpcPtr_t NPC_PTR, const sf::Sprite & TALKING_NPC)
    {
        return std::make_unique<Conversation>(NPC_PTR, TALKING_NPC);
    }

} // namespace interact
} // namespace heroespath
