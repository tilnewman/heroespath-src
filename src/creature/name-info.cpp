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

#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/text-region.hpp"
#include "misc/log-macros.hpp"

#include <sstream>
#include <string>

namespace heroespath
{
namespace creature
{

    float NameInfo::DefaultTextEntryBoxWidth() const
    {
        // The +1 is to accomodate the TextEntryBox's padding and margins
        return Size(gui::TextInfo(LargestName() + LargestLetter(), DefaultFont(), DefaultSize())).x;
    }

    gui::GuiFont::Enum NameInfo::DefaultFont() const { return gui::GuiFont::System; }

    unsigned int NameInfo::DefaultSize() const
    {
        return gui::FontManager::Instance()->Size_Normal();
    }

    const gui::TextInfo
        NameInfo::MakeTextInfo(const gui::GuiFont::Enum FONT, const unsigned int CHAR_SIZE) const
    {
        return gui::TextInfo(LargestName(), FONT, CHAR_SIZE);
    }

    const sf::Vector2f NameInfo::Size(const gui::TextInfo & TEXT_INFO) const
    {
        gui::TextRegion textRegion("CreatureNameInfoSizeDeterminer", TEXT_INFO, sf::FloatRect());

        return sf::Vector2f(
            textRegion.GetEntityRegion().width, textRegion.GetEntityRegion().height);
    }

    const sf::Vector2f NameInfo::ConvertSizeToScreenRatio(const sf::Vector2f & SIZE_V) const
    {
        return sf::Vector2f(
            SIZE_V.x / gui::Display::Instance()->GetWinWidth(),
            SIZE_V.y / gui::Display::Instance()->GetWinHeight());
    }

} // namespace creature
} // namespace heroespath
