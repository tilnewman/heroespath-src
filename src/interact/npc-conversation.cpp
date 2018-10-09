// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// npc-conversation.cpp
//
#include "npc-conversation.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>

namespace heroespath
{
namespace interact
{

    NpcConversation::NpcConversation(const ConvPointVec_t & CONVERSATION_POINTS)
        : isRandom_(false)
        , convPointsIndex_(1)
        , convPoints_(CONVERSATION_POINTS)
    {
        LogIfInvalid();
    }

    NpcConversation::NpcConversation(const std::string & TEXT, const Buttons::Enum BUTTON)
        : isRandom_(true)
        , convPointsIndex_(1)
        , convPoints_()
    {
        convPoints_.emplace_back(NpcConversationPoint());
        convPoints_.emplace_back(NpcConversationPoint(TEXT, BUTTON));
        LogIfInvalid();
    }

    const NpcConversationPoint & NpcConversation::Current() const
    {
        if (convPointsIndex_ >= convPoints_.size())
        {
            return convPoints_.at(0);
        }
        else
        {
            return convPoints_.at(convPointsIndex_);
        }
    }

    bool NpcConversation::ApplyResponse(const Buttons::Enum BUTTON)
    {
        const auto & CURRENT_COVERSATION_POINT { Current() };

        if (CURRENT_COVERSATION_POINT.IsValid())
        {
            convPointsIndex_ = CURRENT_COVERSATION_POINT.Transition(BUTTON);
            LogIfInvalid();

            return (IsAtBeginning() || IsAtEnd() || (convPointsIndex_ >= convPoints_.size()));
        }
        else
        {
            return true;
        }
    }

    const std::string NpcConversation::ToString() const
    {
        std::ostringstream ss;

        ss << "NpcConversation: index=" << convPointsIndex_ << ", is_random=" << std::boolalpha
           << isRandom_;

        std::size_t index { 0 };
        for (const auto & NPC_CONVERSATION_POINT : convPoints_)
        {
            ss << "\n[" << index++ << "]\t" << NPC_CONVERSATION_POINT.ToString();
        }

        return ss.str();
    }

    void NpcConversation::LogIfInvalid() const
    {
        if (convPointsIndex_ >= convPoints_.size())
        {
            M_HP_LOG_ERR(
                "interact::NpcConversation::LogIfInvalid() found convPointsIndex="
                << convPointsIndex_ << " out of range.  " << ToString());
        }

        if (convPoints_.size() < 2)
        {
            M_HP_LOG_ERR(
                "interact::NpcConversation::LogIfInvalid() found convPoints vector too small.  "
                << ToString());
        }
        else
        {
            if (convPoints_.at(0).IsValid() == true)
            {
                M_HP_LOG_ERR(
                    "interact::NpcConversation::LogIfInvalid() found convPoints_[0] was not the "
                    "empty (invalid) NpcConversationPoint object it should be.  "
                    << ToString());
            }

            if (convPoints_.at(1).IsValid() == false)
            {
                M_HP_LOG_ERR(
                    "interact::NpcConversation::LogIfInvalid() found convPoints_[1] was not the "
                    "valid NpcConversationPoint object it should be.  "
                    << ToString());
            }
        }
    }

} // namespace interact
} // namespace heroespath
