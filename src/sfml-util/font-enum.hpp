// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_FONT_ENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_FONT_ENUM_HPP_INCLUDED
//
// font-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    struct Font : public misc::EnumBaseCounting<Font, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            // Euler, slightly dolphin, nice and round with ample spacing
            Default = 0,

            // Modern-Antiqua, bold with sharp flavorful ends, bold and tall (like a bold Euler with
            // more flavor)
            DefaultBoldFlavor,

            // Gentium-Plus, normal, tall, straight, and slightly condensed (very similar to
            // GoudyBookletter but slightly wider)
            System,

            // Goudy-Bookletter, normal and wide but very condensed with ugly numbers (very similar
            // to GentiumPlus but slightly more narrow)
            SystemCondensed,

            // Quill-Sword, rigid caliggraphy with medieval tails, very condensed (good for signs)
            // (great flavorful numbers)
            SignThinTallNarrow,

            Number = SignThinTallNarrow,

            // Queen-Country, very bold and short with medieval flavor (good for signs)
            SignBoldShortWide,

            // Valley-Forge, casual handwriting, slightly bold (good for writing on scrolls)
            // used for popup window text buttons
            Handwriting,

            // Square-Antiqua
            DialogModern,

            // Mops-Antiqua
            DialogMedieval,

            Count
        };

        static const std::string ToString(const Font::Enum);
        static const std::string Name(const Font::Enum);
        static const std::string Path(const Font::Enum);
    };

    using FontEnumVec_t = std::vector<Font::Enum>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_FONT_ENUM_HPP_INCLUDED