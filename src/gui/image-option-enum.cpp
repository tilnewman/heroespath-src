// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-option-enum.cpp
//
#include "image-option-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string
        ImageOpt::ToStringPopulate(const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        AppendNameIfBitIsSet(str, ENUM_VALUE, ImageOpt::Smooth, "Smooth", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, ImageOpt::FlipHoriz, "FlipHoriz", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, ImageOpt::FlipVert, "FlipVert", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, ImageOpt::Repeated, "Repeated", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, ImageOpt::Invert, "Invert", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, ImageOpt::InvertAfterMask, "InvertAfterMask", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, ImageOpt::InvertIncludesAlpha, "InvertIncludesAlpha", SEPARATOR);

        return str;
    }

} // namespace gui
} // namespace heroespath
