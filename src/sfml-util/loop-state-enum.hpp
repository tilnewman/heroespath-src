// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_LOOPSTATE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOPSTATE_HPP_INCLUDED
//
// loop-state-enum.hpp
//  An enum describing the game loop state.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{

    struct LoopState : public misc::EnumBaseCounting<LoopState, misc::EnumFirstValueNone>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Intro,
            MainMenu,
            LoadGameMenu,
            CharacterCreation,
            PartyCreation,
            Adventure,
            Combat,
            Camp,
            Inn,
            Query,
            Popup,
            Credits,
            Settings,
            Exit,
            Inventory,
            Treasure,
            Test,
            Count
        };

        static const std::string ToString(const LoopState::Enum);

        static bool TestHelper() { return EnumBaseCounting::Test(); }
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOPSTATE_HPP_INCLUDED
