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
#include "npc.hpp"
#include "interact/conversation-factory.hpp"

#include <sstream>

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

    void Npc::ResetConversation()
    {
        if (conversationCategories_.empty() == false)
        {
            conversation_ = MakeNewConversation();
        }
    }

    const std::string Npc::ToString() const
    {
        std::ostringstream ss;

        ss << "state::Npc={avatar=" << avatar::Avatar::ToString(avatar_) << ", conv_mood="
           << ((conversationMood_ == interact::talk::Mood::Kind) ? "kind" : "mean")
           << ", walk_bounds_index=" << walkBoundsIndex_ << ", conv_categories=";

        for (auto const CONV_CAT : conversationCategories_)
        {
            ss << interact::talk::CategoryToString(CONV_CAT) << ", ";
        }

        ss << "}";

        return ss.str();
    }

} // namespace state
} // namespace heroespath
