// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_IMAGE_OPTION_ENUM_HPP_INCLUDED
#define HEROESPATH_GUI_IMAGE_OPTION_ENUM_HPP_INCLUDED
//
// image-option-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    // Responsible for enumerating all possible alterations for a cached texture.
    struct ImageOpt : public EnumBaseBitField
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            Smooth = 1 << 0,
            Default = Smooth,
            FlipHoriz = 1 << 1,
            FlipVert = 1 << 2,
            Repeated = 1 << 3,
            Invert = 1 << 4,
            InvertAfterMask = 1 << 5,
            InvertIncludesAlpha = 1 << 6,
            ShadowMaskNormal = 1 << 7,
            ShadowMaskForShadowImage = 1 << 8,
            Last = ShadowMaskForShadowImage
        };

        static const std::string
            ToString(const Enum ENUM_VALUE, const EnumStringHow & HOW = EnumStringHow("/"));

        static const std::string ToStringPopulate(
            const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR = "/");
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_IMAGE_OPTION_ENUM_HPP_INCLUDED
