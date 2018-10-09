// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// npc.hpp
//
#include "interact/npc-conversation-factory.hpp"
#include "npc.hpp"

#include <sstream>

namespace heroespath
{
namespace game
{

    Npc::Npc(
        const avatar::Avatar::Enum AVATAR,
        const interact::talk::CategoryVec_t & CONV_CATEGORIES,
        const interact::talk::Mood CONV_MOOD,
        const std::size_t WALK_BOUNDS_INDEX)
        : avatar_(AVATAR)
        , conversationCategories_(CONV_CATEGORIES)
        , conversationMood_(CONV_MOOD)
        , conversation_(MakeNewRandomConversation())
        , walkBoundsIndex_(WALK_BOUNDS_INDEX)
    {}

    Npc::Npc(
        const avatar::Avatar::Enum AVATAR,
        const interact::NpcConversation & CONVERSATION,
        const std::size_t WALK_BOUNDS_INDEX)
        : avatar_(AVATAR)
        , conversationCategories_()
        , conversationMood_(interact::talk::Mood::Kind)
        , conversation_(CONVERSATION)
        , walkBoundsIndex_(WALK_BOUNDS_INDEX)
    {}

    bool Npc::ApplyConversationResponse(const interact::Buttons::Enum BUTTON)
    {
        if (conversation_.ApplyResponse(BUTTON))
        {
            // at this point we know the conversation either ended or restarted
            RemakeConversationIfRandom();
            return true;
        }
        else
        {
            return false;
        }
    }

    const interact::NpcConversation Npc::MakeNewRandomConversation()
    {
        return interact::NpcConversationFactory::MakeRandom(
            conversationMood_, conversationCategories_);
    }

    void Npc::RemakeConversationIfRandom()
    {
        if (CanMakeNewRandomConversation())
        {
            conversation_ = MakeNewRandomConversation();
        }
    }

    const std::string Npc::ToString() const
    {
        std::ostringstream ss;

        ss << "Npc={avatar=" << avatar::Avatar::ToString(avatar_) << ", conv_mood="
           << ((conversationMood_ == interact::talk::Mood::Kind) ? "kind" : "mean")
           << ", walk_bounds_index=" << walkBoundsIndex_ << ", conv_categories=";

        for (const auto CONV_CAT : conversationCategories_)
        {
            ss << interact::talk::CategoryToString(CONV_CAT) << ", ";
        }

        ss << "}";

        return ss.str();
    }

} // namespace game
} // namespace heroespath
