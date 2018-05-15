// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// phase-enum.cpp
//
#include "phase-enum.hpp"

namespace heroespath
{
namespace game
{

    void Phase::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Phase::Combat, "Combat", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Phase::Exploring, "Exploring", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Phase::Conversation, "Conversation", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Phase::Quest, "Quest", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Phase::Inventory, "Inventory", SEPARATOR);
    }

} // namespace game
} // namespace heroespath
