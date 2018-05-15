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
namespace sfml_util
{
    namespace gui
    {

        MouseTextInfo::MouseTextInfo()
            : up()
            , down()
            , over()
        {}

        MouseTextInfo::MouseTextInfo(const TextInfo & TEXT_INFO)
            : up(TEXT_INFO)
            , down(TEXT_INFO)
            , over(TEXT_INFO)
        {}

        MouseTextInfo::MouseTextInfo(
            const TextInfo & UP, const TextInfo & DOWN, const TextInfo & OVER)
            : up(UP)
            , down(DOWN)
            , over(OVER)
        {}

        MouseTextInfo::MouseTextInfo(
            const TextInfo & TEXT_INFO_UP,
            const sf::Color & COLOR_DOWN,
            const sf::Color & COLOR_OVER)
            : up(TEXT_INFO_UP)
            , down(TEXT_INFO_UP)
            , over(TEXT_INFO_UP)
        {
            down.color = COLOR_DOWN;
            over.color = COLOR_OVER;
        }

        MouseTextInfo::MouseTextInfo(
            const std::string & TEXT,
            const FontPtr_t FONT_PTR,
            const unsigned int SIZE,
            const sf::Color & COLOR_UP,
            const sf::Color & COLOR_DOWN,
            const sf::Color & COLOR_OVER)
            : up(TEXT, FONT_PTR, SIZE, COLOR_UP)
            , down(TEXT, FONT_PTR, SIZE, COLOR_DOWN)
            , over(TEXT, FONT_PTR, SIZE, COLOR_OVER)
        {}

        const MouseTextInfo MouseTextInfo::Make_PopupButtonSet(
            const std::string & NAME, const popup::PopupButtonColor::Enum COLOR)
        {
            return MouseTextInfo(
                NAME,
                FontManager::Instance()->Font_PopupButton(),
                FontManager::Instance()->Size_Larger(),
                FontManager::Color_PopupButtonUp(COLOR),
                FontManager::Color_PopupButtonDown(COLOR),
                FontManager::Color_PopupButtonOver(COLOR));
        }

        const MouseTextInfo MouseTextInfo::Make_InteractionButtonSet(const std::string & NAME)
        {
            return MouseTextInfo(
                NAME,
                FontManager::Instance()->Font_PopupButton(),
                FontManager::Instance()->Size_Large(),
                FontManager::Color_PopupButtonUp(popup::PopupButtonColor::Dark),
                FontManager::Color_PopupButtonDown(popup::PopupButtonColor::Dark),
                FontManager::Color_PopupButtonOver(popup::PopupButtonColor::Dark));
        }

        bool operator<(const MouseTextInfo & L, const MouseTextInfo & R)
        {
            return std::tie(L.up, L.down, L.over) < std::tie(R.up, R.down, R.over);
        }

        bool operator==(const MouseTextInfo & L, const MouseTextInfo & R)
        {
            return std::tie(L.up, L.down, L.over) == std::tie(R.up, R.down, R.over);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
