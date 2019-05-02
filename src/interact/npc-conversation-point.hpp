// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_NPC_CONVERSATION_POINT_HPP_INCLUDED
#define HEROESPATH_INTERACTION_NPC_CONVERSATION_POINT_HPP_INCLUDED
//
// conversation-point.hpp
//
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

    using TransitionMap_t = misc::VectorMap<Buttons::Enum, std::size_t>;

    // Responsible for wrapping a place in a conversation.
    struct NpcConversationPoint
    {
        // default construction means IsValid() == false.
        explicit NpcConversationPoint(
            const std::string & TEXT = "",
            const ButtonEnumVec_t & BUTTONS = ButtonEnumVec_t(),
            const TransitionMap_t & TRANSITIONS = TransitionMap_t())
            : text_(TEXT)
            , buttons_(BUTTONS)
            , transitionMap_(TRANSITIONS)
        {}

        NpcConversationPoint(const std::string & TEXT, const Buttons::Enum BUTTON)
            : text_(TEXT)
            , buttons_(1, BUTTON)
            , transitionMap_()
        {}

        NpcConversationPoint(const NpcConversationPoint &) = default;
        NpcConversationPoint(NpcConversationPoint &&) = default;
        NpcConversationPoint & operator=(const NpcConversationPoint &) = default;
        NpcConversationPoint & operator=(NpcConversationPoint &&) = default;

        const std::string Text() const { return text_; }
        const ButtonEnumVec_t Buttons() const { return buttons_; }

        std::size_t Transition(const Buttons::Enum BUTTON) const
        {
            std::size_t index { 0 };
            transitionMap_.Find(BUTTON, index);
            return index;
        }

        bool IsValid() const { return (text_.empty() == false) && (buttons_.empty() == false); }

        const std::string ToString() const
        {
            std::string str("NpcConversationPoint: \"" + text_ + "\", buttons=(");

            for (const auto BUTTON : buttons_)
            {
                str += Buttons::ToString(BUTTON) + ",";
            }

            str += "), transition_map=(";

            for (const auto & BUTTON_INDEX_PAIR : transitionMap_)
            {
                str += Buttons::ToString(BUTTON_INDEX_PAIR.first) + "="
                    + std::to_string(BUTTON_INDEX_PAIR.second) + ",";
            }

            str += ")";

            return str;
        }

    private:
        std::string text_;
        ButtonEnumVec_t buttons_;
        TransitionMap_t transitionMap_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & text_;
            ar & buttons_;
            ar & transitionMap_;
        }
    };

    using ConvPointVec_t = std::vector<NpcConversationPoint>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_NPC_CONVERSATION_POINT_HPP_INCLUDED
