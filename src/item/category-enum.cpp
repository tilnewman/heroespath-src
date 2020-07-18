// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// category-enum.cpp
//
#include "category-enum.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/enum-util.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace item
{

    const std::string Category::Name(const Enum ENUM, const EnumStringHow & HOW)
    {
        std::string str = EnumUtil<Category>::ToString(ENUM, HOW);
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(OneHanded), "one-handed");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(TwoHanded), "two-handed");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(ConsumedOnUse), "consumed upon use");
        boost::algorithm::ireplace_all(str, NAMEOF_ENUM(ShowsEnemyInfo), "shows enemy info");
        return str;
    }

} // namespace item
} // namespace heroespath
