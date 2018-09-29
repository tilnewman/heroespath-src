// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// font.cpp
//
#include "font.hpp"

#include "misc/strings.hpp"

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Font & F)
{
    os << "(\"" << F.getInfo().family << "\")";
    return os;
}

} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::string ToString(const sf::Font & F, const misc::ToStringPrefix::Enum OPTIONS)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix(OPTIONS, "Font") << F;
        return ss.str();
    }

} // namespace sfutil
} // namespace heroespath
