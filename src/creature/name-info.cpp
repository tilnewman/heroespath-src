// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// name-info.cpp
//
#include "name-info.hpp"

#include "log/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <sstream>
#include <string>

namespace heroespath
{
namespace creature
{

    const sfml_util::FontPtr_t NameInfo::DefaultFont() const
    {
        return sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System);
    }

    unsigned int NameInfo::DefaultSize() const
    {
        return sfml_util::FontManager::Instance()->Size_Normal();
    }

    const sf::Vector2f NameInfo::Size(const sfml_util::gui::TextInfo & TEXT_INFO) const
    {
        sfml_util::gui::TextRegion textRegion(
            "CreatureNameInfoSizeDeterminer", TEXT_INFO, sf::FloatRect());

        return sf::Vector2f(
            textRegion.GetEntityRegion().width, textRegion.GetEntityRegion().height);
    }

    const sf::Vector2f NameInfo::ConvertSizeToScreenRatio(const sf::Vector2f & SIZE_V) const
    {
        return sf::Vector2f(
            SIZE_V.x / sfml_util::Display::Instance()->GetWinWidth(),
            SIZE_V.y / sfml_util::Display::Instance()->GetWinHeight());
    }

} // namespace creature
} // namespace heroespath
