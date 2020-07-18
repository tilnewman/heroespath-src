// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ELEMENTAL_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_ELEMENTAL_ENUM_HPP_INCLUDED
//
// element-enum.hpp
//
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace item
{

    struct Element : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            Honor = 1 << 0,
            Shadow = 1 << 1,
            Fire = 1 << 2,
            Frost = 1 << 3
        };

        static constexpr Enum Last = Frost;

        static constexpr bool IsValid(const Enum ENUM) noexcept
        {
            return !(((ENUM & Fire) && (ENUM & Frost)) || ((ENUM & Honor) && (ENUM & Shadow)));
        }

        static const std::string Name(
            const Enum, const EnumStringHow & HOW = EnumStringHow(), const bool INCLUDE_OF = true);
    };

    struct ElementList : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            Honor = 1 << 0,
            Shadow = 1 << 1,
            Fire = 1 << 2,
            Frost = 1 << 3,
            HonorFire = 1 << 4,
            HonorFrost = 1 << 5,
            ShadowFire = 1 << 6,
            ShadowFrost = 1 << 7
        };

        static constexpr Enum Last = ShadowFrost;

        static constexpr Enum All
            = (Honor | Shadow | Fire | Frost | HonorFire | HonorFrost | ShadowFire | ShadowFrost);

        static constexpr std::size_t Size(const Enum ENUM) noexcept
        {
            return misc::CountBits(ENUM);
        }

        static constexpr Enum MakeCombinationsList(const EnumUnderlying_t ENUM_VALUE) noexcept
        {
            switch (ENUM_VALUE)
            {
                case (Element::Honor): return Honor;
                case (Element::Shadow): return Shadow;

                case (Element::Honor | Element::Shadow): return (Honor | Shadow);

                case (Element::Fire): return (Fire);

                case (Element::Honor | Element::Fire): return (Honor | Fire | HonorFire);

                case (Element::Shadow | Element::Fire): return (Shadow | Fire | ShadowFire);

                case (Element::Honor | Element::Shadow | Element::Fire):
                    return (Honor | Shadow | Fire | HonorFire | ShadowFire);

                case (Element::Frost): return (Frost);

                case (Element::Honor | Element::Frost): return (Honor | Frost | HonorFrost);

                case (Element::Shadow | Element::Frost): return (Shadow | Frost | ShadowFrost);

                case (Element::Honor | Element::Shadow | Element::Frost):
                    return (Honor | Shadow | Frost | HonorFrost | ShadowFrost);

                case (Element::Fire | Element::Frost): return (Fire | Frost);

                case (Element::Honor | Element::Fire | Element::Frost):
                    return (Honor | Fire | Frost | HonorFire | HonorFrost);

                case (Element::Shadow | Element::Fire | Element::Frost):
                    return (Shadow | Fire | Frost | ShadowFire | ShadowFrost);

                case (Element::Honor | Element::Shadow | Element::Fire | Element::Frost):
                    return (
                        Honor | Shadow | Fire | Frost | HonorFire | HonorFrost | ShadowFire
                        | ShadowFrost);

                default: return None;
            };
        }

        // fails if multiple bits are high at once
        static constexpr Element::Enum
            FromListBitFlag(const ElementList::Enum LIST_BIT_FLAG) noexcept
        {
            switch (LIST_BIT_FLAG)
            {
                case Honor: return Element::Honor;
                case Shadow: return Element::Shadow;
                case Fire: return Element::Fire;
                case Frost: return Element::Frost;
                case HonorFire: return (Element::Honor | Element::Fire);
                case HonorFrost: return (Element::Honor | Element::Frost);
                case ShadowFire: return (Element::Shadow | Element::Fire);
                case ShadowFrost: return (Element::Shadow | Element::Frost);

                case None:
                default: return Element::None;
            }
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ELEMENTAL_ENUM_HPP_INCLUDED
