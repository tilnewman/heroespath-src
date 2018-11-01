// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// mouse-text-info.cpp
//
#include "mouse-text-info.hpp"

#include "popup/popup-info.hpp"
#include "sfml-util/font-manager.hpp"

#include <tuple>

namespace heroespath
{
namespace gui
{

    MouseTextInfo::MouseTextInfo()
        : up()
        , down()
        , over()
        , disabled()
    {}

    MouseTextInfo::MouseTextInfo(const TextInfo & TEXT_INFO)
        : up(TEXT_INFO)
        , down(TEXT_INFO)
        , over(TEXT_INFO)
        , disabled(TEXT_INFO)
    {}

    MouseTextInfo::MouseTextInfo(
        const TextInfo & UP,
        const TextInfo & DOWN,
        const TextInfo & OVER,
        const TextInfo & DISABLED)
        : up(UP)
        , down(DOWN)
        , over(OVER)
        , disabled(DISABLED)
    {}

    MouseTextInfo::MouseTextInfo(
        const TextInfo & TEXT_INFO_UP,
        const sf::Color & COLOR_DOWN,
        const sf::Color & COLOR_OVER,
        const sf::Color & COLOR_DISABLED)
        : up(TEXT_INFO_UP)
        , down(TEXT_INFO_UP, TEXT_INFO_UP.text, COLOR_DOWN)
        , over(TEXT_INFO_UP, TEXT_INFO_UP.text, COLOR_OVER)
        , disabled(TEXT_INFO_UP, TEXT_INFO_UP.text, COLOR_DISABLED)
    {}

    MouseTextInfo::MouseTextInfo(
        const std::string & TEXT,
        const GuiFont::Enum FONT,
        const unsigned int SIZE,
        const sf::Color & COLOR_UP,
        const sf::Color & COLOR_DOWN,
        const sf::Color & COLOR_OVER,
        const sf::Color & COLOR_DISABLED)
        : up(TEXT, FONT, SIZE, COLOR_UP)
        , down(TEXT, FONT, SIZE, COLOR_DOWN)
        , over(TEXT, FONT, SIZE, COLOR_OVER)
        , disabled(TEXT, FONT, SIZE, COLOR_DISABLED)
    {}

    const MouseTextInfo MouseTextInfo::Make_PopupButtonSet(
        const std::string & NAME, const popup::PopupButtonColor::Enum COLOR)
    {
        return MouseTextInfo(
            NAME,
            gui::GuiFont::Handwriting,
            FontManager::Instance()->Size_Larger(),
            FontManager::Color_PopupButtonUp(COLOR),
            FontManager::Color_PopupButtonDown(COLOR),
            FontManager::Color_PopupButtonOver(COLOR),
            FontManager::Color_PopupButtonDisabled(COLOR));
    }

    const MouseTextInfo MouseTextInfo::Make_InteractionButtonSet(const std::string & NAME)
    {
        return MouseTextInfo(
            NAME,
            gui::GuiFont::DialogMedieval,
            FontManager::Instance()->Size_Large(),
            FontManager::Color_PopupButtonUp(popup::PopupButtonColor::Dark),
            FontManager::Color_PopupButtonDown(popup::PopupButtonColor::Dark),
            FontManager::Color_PopupButtonOver(popup::PopupButtonColor::Dark),
            FontManager::Color_PopupButtonDisabled(popup::PopupButtonColor::Dark));
    }

    const std::string MouseTextInfo::ToString(
        const bool WILL_PREFIX, const misc::Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        ss << "Up=" << up.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Down=" << down.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Over=" << over.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Disabled=" << disabled.ToString(false, misc::Wrap::Yes);

        const auto PARTS_STR { (
            (WILL_WRAP == misc::Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("MouseTextInfo").append((WILL_WRAP == misc::Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over, L.disabled)
            < std::tie(R.up, R.down, R.over, R.disabled);
    }

    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over, L.disabled)
            == std::tie(R.up, R.down, R.over, R.disabled);
    }

} // namespace gui
} // namespace heroespath
