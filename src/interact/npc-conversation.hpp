// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_NPC_CONVERSATION_HPP_INCLUDED
#define HEROESPATH_INTERACTION_NPC_CONVERSATION_HPP_INCLUDED
//
// npc-conversation.hpp
//
#include "interact/interaction-button-enum.hpp"
#include "interact/npc-conversation-point.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace heroespath
{
namespace interact
{

    // Responsible for storing all things said in a conversation and where in that conversation the
    // player is.
    class NpcConversation
    {
    public:
        // use to create a non-random specific conversation that is unique to an NPC
        explicit NpcConversation(const ConvPointVec_t & CONVERSATION_POINTS = ConvPointVec_t());

        // use to create a random conversation
        NpcConversation(const std::string & TEXT, const Buttons::Enum BUTTON);

        bool IsRandom() const { return isRandom_; }
        bool IsAtBeginning() const { return (1 == convPointsIndex_); }
        bool IsAtEnd() const { return (0 == convPointsIndex_); }

        const NpcConversationPoint & Current() const;

        // returns true if the conversation ended or restarted
        bool ApplyResponse(const Buttons::Enum);

        const std::string ToString() const;

    private:
        void LogIfInvalid() const;

        // true if this conversation was randomly created and should be replaced with another random
        // conversation when it ends.
        bool isRandom_;

        // an index into convPoints_ where the current state of the conversation is
        // always starts at 1 and will eventually end up at 0 where the conversation ends
        std::size_t convPointsIndex_;

        // a vector of conversation points
        // never empty, first value (at index zero) is always the (invalid) default
        // NpcConversationPoint() that represents the end of the conversation, the second value (at
        // index 1) represents the start of the conversation
        ConvPointVec_t convPoints_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & isRandom_;
            ar & convPointsIndex_;
            ar & convPoints_;
        }
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_NPC_CONVERSATION_HPP_INCLUDED
