// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// trait.cpp
//
#include "trait.hpp"

#include <sstream>

namespace heroespath
{
namespace creature
{

    Trait::Trait(const Traits::Enum TYPE, const Trait_t NORMAL)
        : type_(TYPE)
        , normal_(NORMAL)
        , current_(NORMAL)
    {}

    const std::string Trait::ToString(const bool WILL_PREPEND_PLUS) const
    {
        std::string str;
        str.reserve(8);

        if (WILL_PREPEND_PLUS && (current_ > 0))
        {
            str += '+';
        }

        str += std::to_string(current_);

        return str;
    }

} // namespace creature
} // namespace heroespath
