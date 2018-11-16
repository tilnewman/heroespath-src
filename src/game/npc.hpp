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
#include "interact/npc-conversation-point.hpp"
#include "interact/npc-conversation.hpp"
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
            const std::size_t WALK_BOUNDS_SET_INDEX = 0);

        Npc(const avatar::Avatar::Enum,
            const interact::NpcConversation &,
            const std::size_t WALK_BOUNDS_SET_INDEX);

        avatar::Avatar::Enum AvatarImage() const { return avatar_; }

        const interact::NpcConversationPoint ConversationPoint() { return conversation_.Current(); }

        // returns true if the conversation ended or restarted
        bool ApplyConversationResponse(const interact::Buttons::Enum);

        std::size_t WalkBoundsSetIndex() const { return walkBoundsSetIndex_; }

        void RemakeConversationIfRandom();

        const std::string ToString() const;

    private:
        const interact::NpcConversation MakeNewRandomConversation();

        bool CanMakeNewRandomConversation() const
        {
            return (conversation_.IsRandom() && (conversationCategories_.empty() == false));
        }

    private:
        avatar::Avatar::Enum avatar_;
        interact::talk::CategoryVec_t conversationCategories_;
        interact::talk::Mood conversationMood_;
        interact::NpcConversation conversation_;
        std::size_t walkBoundsSetIndex_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & avatar_;
            ar & conversationCategories_;
            ar & conversation_;
            ar & conversationMood_;
            ar & walkBoundsSetIndex_;
        }
    };

    using NpcPtr_t = misc::NotNull<Npc *>;
    using NpcPVec_t = std::vector<NpcPtr_t>;

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_NPC_HPP_INCLUDED
