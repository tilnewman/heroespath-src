// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// texture-options-enum.cpp
//
#include "texture-options-enum.hpp"

namespace heroespath
{
namespace sfml_util
{

    void TextureOpt::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, TextureOpt::Smooth, "Smooth", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, TextureOpt::FlipHoriz, "FlipHoriz", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, TextureOpt::FlipVert, "FlipVert", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, TextureOpt::Invert, "Invert", SEPARATOR);
    }

} // namespace sfml_util
} // namespace heroespath
