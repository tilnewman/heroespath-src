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
        Conversation(const ConvPointVec_t & CONVERSATION_POINTS = ConvPointVec_t());

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
}
}

#endif // HEROESPATH_INTERACTION_CONVERSATION_HPP_INCLUDED
