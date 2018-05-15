// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_INTERACTION_FACTORY_HPP_INCLUDED
#define HEROESPATH_INTERACTION_INTERACTION_FACTORY_HPP_INCLUDED
//
// conversation-factory.hpp
//
#include "interact/conversation.hpp"
#include "interact/statement.hpp"

namespace heroespath
{
namespace interact
{

    struct ConversationFactory
    {
        static const Conversation Make(const talk::Mood, const talk::CategoryVec_t &);
    };
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_INTERACTION_FACTORY_HPP_INCLUDED
