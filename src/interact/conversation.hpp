// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_CONVERSATION_HPP_INCLUDED
#define HEROESPATH_INTERACTION_CONVERSATION_HPP_INCLUDED
//
// conversation.hpp
//
#include "interact/conversation-point.hpp"
#include "interact/interaction-button-enum.hpp"
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
    class Conversation
    {
    public:
        explicit Conversation(const ConvPointVec_t & CONVERSATION_POINTS = ConvPointVec_t());

        Conversation(const std::string & TEXT, const Buttons::Enum BUTTON);

        const ConversationPoint & Current() const;

        void ApplyResponse(const Buttons::Enum);

    private:
        std::size_t pointIndex_;
        ConvPointVec_t convPoints_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & pointIndex_;
            ar & convPoints_;
        }
    };
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_CONVERSATION_HPP_INCLUDED
