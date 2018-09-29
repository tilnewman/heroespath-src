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
#include "sfml-util/text-region.hpp"

#include <sstream>
#include <string>

namespace heroespath
{
namespace creature
{

    float NameInfo::DefaultTextEntryBoxWidth() const
    {
        // The +1 is to accomodate the TextEntryBox's padding and margins
        return Size(sfml_util::TextInfo(
                        LargestName() + LargestLetter(), DefaultFont(), DefaultSize()))
            .x;
    }

    sfml_util::GuiFont::Enum NameInfo::DefaultFont() const { return sfml_util::GuiFont::System; }

    unsigned int NameInfo::DefaultSize() const
    {
        return sfml_util::FontManager::Instance()->Size_Normal();
    }

    const sfml_util::TextInfo NameInfo::MakeTextInfo(
        const sfml_util::GuiFont::Enum FONT, const unsigned int CHAR_SIZE) const
    {
        return sfml_util::TextInfo(LargestName(), FONT, CHAR_SIZE);
    }

    const sf::Vector2f NameInfo::Size(const sfml_util::TextInfo & TEXT_INFO) const
    {
        sfml_util::TextRegion textRegion(
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
