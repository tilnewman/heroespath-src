// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_NPC_HPP_INCLUDED
#define HEROESPATH_GAME_NPC_HPP_INCLUDED
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
namespace game
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

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_NPC_HPP_INCLUDED
