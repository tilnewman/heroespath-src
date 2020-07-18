// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile.cpp
//
#include "item-profile.hpp"

#include "misc/enum-util.hpp"

namespace heroespath
{
namespace item
{

    const std::string ItemProfile::ToString() const
    {
        std::string str;
        str.reserve(128);

        str += "ItemProfile{";

        if (!IsValid())
        {
            str += "INVALID, ";
        }

        str += thinProfile_.ToString();
        str += "category=";
        str += EnumUtil<item::Category>::ToString(category_, EnumStringHow(Wrap::Yes));
        str += ", ";

        if (isPixie_)
        {
            str += ", PIXIE";
        }

        str += ", mat=";
        str += NAMEOF_ENUM(matPri_);
        str += "/";
        str += NAMEOF_ENUM(matSec_);

        if (set_ < Set::Count)
        {
            str += ", set=";
            str += NAMEOF_ENUM(set_);
        }

        if (named_ < Named::Count)
        {
            str += ", named=";
            str += NAMEOF_ENUM(named_);
        }

        if (element_ != Element::None)
        {
            str += ", element=";
            str += EnumUtil<Element>::ToString(element_, EnumStringHow(Wrap::Yes));
        }

        if (summonInfo_.IsValid())
        {
            str += ", summon=";
            str += summonInfo_.ToString();
        }

        return str;
    }

} // namespace item
} // namespace heroespath
