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
#ifndef HEROESPATH_STATE_NPC_HPP_INCLUDED
#define HEROESPATH_STATE_NPC_HPP_INCLUDED
//
// npc.hpp
//
#include "avatar/avatar-enum.hpp"
#include "interact/conversation-point.hpp"
#include "interact/conversation.hpp"
#include "interact/statement.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace state
{

    // Responsible for wrapping all the state needed to represent a
    // non-player-character in the game.
    class Npc
    {
    public:
        explicit Npc(
            const avatar::Avatar::Enum AVATAR = avatar::Avatar::Count,
            const interact::talk::CategoryVec_t & CONV_CATEGORIES = interact::talk::CategoryVec_t(),
            const interact::talk::Mood CONV_MOOD = interact::talk::Mood::Kind,
            const std::size_t WALK_BOUNDS_INDEX = 0);

        Npc(const avatar::Avatar::Enum,
            const interact::Conversation &,
            const std::size_t WALK_BOUNDS_INDEX);

        avatar::Avatar::Enum AvatarImage() const { return avatar_; }

        const interact::ConversationPoint ConversationPoint() { return conversation_.Current(); }

        void ApplyConversationResponse(const interact::Buttons::Enum);

        std::size_t WalkBoundsIndex() const { return walkBoundsIndex_; }

        void ResetConversation();

        const std::string ToString() const;

    private:
        const interact::Conversation MakeNewConversation();

    private:
        avatar::Avatar::Enum avatar_;
        interact::talk::CategoryVec_t conversationCategories_;
        interact::talk::Mood conversationMood_;
        interact::Conversation conversation_;
        std::size_t walkBoundsIndex_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & avatar_;
            ar & conversationCategories_;
            ar & conversation_;
            ar & conversationMood_;
            ar & walkBoundsIndex_;
        }
    };

    using NpcPtr_t = misc::NotNull<Npc *>;
    using NpcPVec_t = std::vector<NpcPtr_t>;

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_NPC_HPP_INCLUDED
