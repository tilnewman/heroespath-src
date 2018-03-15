///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
#define HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
//
// conversation-point.hpp
//
#include "interact/interaction-button-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/vector-map.hpp"
#include <algorithm>
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
        ConversationPoint(
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
}
}

#endif // HEROESPATH_INTERACTION_CONVERSATION_POINT_HPP_INCLUDED
