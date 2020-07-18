// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// element-enum.cpp
//
#include "element-enum.hpp"

namespace heroespath
{
namespace item
{

    const std::string
        Element::Name(const Enum ELEMENT_TYPE, const EnumStringHow &, const bool INCLUDE_OF)
    {
        std::string str;
        str.reserve(64);

        auto appendIfBitIsSet { [&](const Element::Enum BIT) {
            if (ELEMENT_TYPE & BIT)
            {
                if (str.empty())
                {
                    if (INCLUDE_OF)
                    {
                        str += "of ";
                    }
                }
                else
                {
                    str += " and ";
                }

                str += NAMEOF_ENUM(BIT);
            }
        } };

        appendIfBitIsSet(Element::Fire);
        appendIfBitIsSet(Element::Frost);
        appendIfBitIsSet(Element::Honor);
        appendIfBitIsSet(Element::Shadow);

        return str;
    }

} // namespace item
} // namespace heroespath
