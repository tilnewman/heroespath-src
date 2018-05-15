// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// conversation.cpp
//
#include "conversation.hpp"
#include <string>

namespace heroespath
{
namespace interact
{

    Conversation::Conversation(const ConvPointVec_t & CONVERSATION_POINTS)
        : pointIndex_(1)
        , convPoints_(CONVERSATION_POINTS)
    {}

    Conversation::Conversation(const std::string & TEXT, const Buttons::Enum BUTTON)
        : pointIndex_(1)
        , convPoints_()
    {
        convPoints_.emplace_back(ConversationPoint());
        convPoints_.emplace_back(ConversationPoint(TEXT, BUTTON));
    }

    const ConversationPoint & Conversation::Current() const
    {
        if (pointIndex_ >= convPoints_.size())
        {
            return convPoints_.at(0);
        }
        else
        {
            return convPoints_.at(pointIndex_);
        }
    }

    void Conversation::ApplyResponse(const Buttons::Enum BUTTON)
    {
        auto & convPoint{ Current() };

        if (convPoint.IsValid())
        {
            pointIndex_ = convPoint.Transition(BUTTON);
        }
    }
} // namespace interact
} // namespace heroespath
