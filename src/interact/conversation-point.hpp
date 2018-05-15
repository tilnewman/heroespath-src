// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
#define HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
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
    struct ConversationPoint
    {
        // default construction means IsValid() == false.
        explicit ConversationPoint(
            const std::string & TEXT = "",
            const ButtonsVec_t & BUTTONS = ButtonsVec_t(),
            const TransitionMap_t & TRANSITIONS = TransitionMap_t())
            : text_(TEXT)
            , buttons_(BUTTONS)
            , transitionMap_(TRANSITIONS)
        {}

        ConversationPoint(const std::string & TEXT, const Buttons::Enum BUTTON)
            : text_(TEXT)
            , buttons_(1, BUTTON)
            , transitionMap_()
        {}

        const std::string Text() const { return text_; }
        const ButtonsVec_t Buttons() const { return buttons_; }

        std::size_t Transition(const Buttons::Enum BUTTON) const
        {
            std::size_t index{ 0 };
            transitionMap_.Find(BUTTON, index);
            return index;
        }

        bool IsValid() const { return (text_.empty() == false); }

    private:
        std::string text_;
        ButtonsVec_t buttons_;
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

    using ConvPointVec_t = std::vector<ConversationPoint>;
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
