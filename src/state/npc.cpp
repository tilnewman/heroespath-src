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
//
// npc.hpp
//
#include "npc.hpp"
#include "interact/conversation-factory.hpp"

namespace heroespath
{
namespace state
{

    Npc::Npc(
        const avatar::Avatar::Enum AVATAR,
        const interact::talk::CategoryVec_t & CONV_CATEGORIES,
        const interact::talk::Mood CONV_MOOD,
        const std::size_t WALK_BOUNDS_INDEX)
        : avatar_(AVATAR)
        , conversationCategories_(CONV_CATEGORIES)
        , conversationMood_(CONV_MOOD)
        , conversation_(MakeNewConversation())
        , walkBoundsIndex_(WALK_BOUNDS_INDEX)
    {}

    Npc::Npc(
        const avatar::Avatar::Enum AVATAR,
        const interact::Conversation & CONVERSATION,
        const std::size_t WALK_BOUNDS_INDEX)
        : avatar_(AVATAR)
        , conversationCategories_()
        , conversationMood_(interact::talk::Mood::Kind)
        , conversation_(CONVERSATION)
        , walkBoundsIndex_(WALK_BOUNDS_INDEX)
    {}

    void Npc::ApplyConversationResponse(const interact::Buttons::Enum BUTTON)
    {
        conversation_.ApplyResponse(BUTTON);

        if (conversation_.Current().IsValid() == false)
        {
            conversation_ = MakeNewConversation();
        }
    }

    const interact::Conversation Npc::MakeNewConversation()
    {
        return interact::ConversationFactory::Make(conversationMood_, conversationCategories_);
    }

    void Npc::HandleLevelLoad()
    {
        if (conversationCategories_.empty() == false)
        {
            conversation_ = MakeNewConversation();
        }
    }
}
}
