// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// npc-conversation-factory.hpp
//
#include "npc-conversation-factory.hpp"

#include "interact/interaction-button-enum.hpp"
#include "interact/statement-factory.hpp"

namespace heroespath
{
namespace interact
{

    const NpcConversation NpcConversationFactory::MakeRandom(
        const talk::Mood MOOD, const talk::CategoryVec_t & TALK_CATEGORIES)
    {
        return NpcConversation(talk::Factory::Make(TALK_CATEGORIES, MOOD), Buttons::Goodbye);
    }

} // namespace interact
} // namespace heroespath
