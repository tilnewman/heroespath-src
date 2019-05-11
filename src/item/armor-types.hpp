// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
//
// armor-types.hpp
//  A set of enumerations that define a piece of armor.
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace item
{
    namespace armor
    {

        struct shield_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Buckler = 0,
                Kite,
                Heater,
                Pavis,
                Count
            };
        };

        struct helm_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Leather = 0,
                MailCoif,
                Kettle,
                Archers,
                Bascinet,
                Great,
                Count
            };

            static constexpr std::string_view const Name(const helm_type::Enum HELM_TYPE)
            {
                if (HELM_TYPE == MailCoif)
                {
                    return "Mail Coif";
                }
                else
                {
                    return NAMEOF_ENUM(HELM_TYPE);
                }
            }
        };

        struct base_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Plain = 0,
                Mail,
                Scale,
                Plate,
                Count
            };
        };

        struct cover_type : public EnumBaseCounting<EnumFirstValue::Valid>
        {
            enum Enum : EnumUnderlying_t
            {
                Cape = 0,
                Vest,
                Robe,
                Cloak,
                Count
            };
        };

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
