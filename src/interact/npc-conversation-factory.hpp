// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_NPC_CONVERSATION_FACTORY_HPP_INCLUDED
#define HEROESPATH_INTERACTION_NPC_CONVERSATION_FACTORY_HPP_INCLUDED
//
// npc-conversation-factory.hpp
//
#include "interact/npc-conversation.hpp"
#include "interact/statement.hpp"

namespace heroespath
{
namespace interact
{

    struct NpcConversationFactory
    {
        static const NpcConversation MakeRandom(const talk::Mood, const talk::CategoryVec_t &);
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_NPC_CONVERSATION_FACTORY_HPP_INCLUDED
