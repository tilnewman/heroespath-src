// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// color-set.cpp
//
#include "color-set.hpp"

#include "sfutil/color.hpp"
#include "sfutil/display.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace gui
{

    const sf::Uint8 FocusColors::DEFAULT_OFFSET_VALUE_(35);

    const sf::Color FocusColors::DEFAULT_OFFSET_COLOR_(
        DEFAULT_OFFSET_VALUE_, DEFAULT_OFFSET_VALUE_, DEFAULT_OFFSET_VALUE_, 0);

    FocusColors::FocusColors()
        : foreground_with(sf::Color::White)
        , background_with(sf::Color::White)
        , foreground_without(sf::Color::White - DEFAULT_OFFSET_COLOR_)
        , background_without(sf::Color::White - DEFAULT_OFFSET_COLOR_)
    {}

    FocusColors::FocusColors(
        const sf::Color & FG_WITH_FOCUS,
        const sf::Color & BG_WITH_FOCUS,
        const sf::Color & FG_WITHOUT_FOCUS,
        const sf::Color & BG_WITHOUT_FOCUS)
        : foreground_with(FG_WITH_FOCUS)
        , background_with(BG_WITH_FOCUS)
        , foreground_without(FG_WITHOUT_FOCUS)
        , background_without(BG_WITHOUT_FOCUS)
    {}

    FocusColors::FocusColors(const sf::Color & FG)
        : foreground_with(FG)
        , background_with(sf::Color::White)
        , foreground_without(FG)
        , background_without(sf::Color::White)
    {}

    FocusColors::FocusColors(const sf::Color & FG_WITH_FOCUS, const sf::Color & BG_WITH_FOCUS)
        : foreground_with(FG_WITH_FOCUS)
        , background_with(BG_WITH_FOCUS)
        , foreground_without(FG_WITH_FOCUS)
        , background_without(BG_WITH_FOCUS)
    {
        foreground_without -= DEFAULT_OFFSET_COLOR_;
        background_without -= DEFAULT_OFFSET_COLOR_;
    }

    void FocusColors::ForceWithoutFocusToMatchWithFocus()
    {
        foreground_without = foreground_with;
        background_without = background_with;
    }

    const std::string FocusColors::ToString(const bool WILL_WRAP) const
    {
        std::string str;
        str.reserve(64);

        if (WILL_WRAP)
        {
            str += '(';
        }

        str += "fg_wf=";
        str += misc::ToString(foreground_with);
        str += ", fg_wof=";
        str += misc::ToString(foreground_without);
        str += ", bg_wf=";
        str += misc::ToString(background_with);
        str += ", bg_wof=";
        str += misc::ToString(background_without);

        if (WILL_WRAP)
        {
            str += ')';
        }

        return str;
    }

    bool operator<(const FocusColors & L, const FocusColors & R)
    {
        return std::tie(
                   L.foreground_with, L.background_with, L.foreground_without, L.background_without)
            < std::tie(
                   R.foreground_with,
                   R.background_with,
                   R.foreground_without,
                   R.background_without);
    }

    bool operator==(const FocusColors & L, const FocusColors & R)
    {
        return std::tie(
                   L.foreground_with, L.background_with, L.foreground_without, L.background_without)
            == std::tie(
                   R.foreground_with,
                   R.background_with,
                   R.foreground_without,
                   R.background_without);
    }

} // namespace gui
} // namespace heroespath
