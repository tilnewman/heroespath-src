// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
//
// effect-type-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{

    struct EffectType : public misc::EnumBaseCounting<EffectType, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            CreatureHarmDamage = 0,
            CreatureHarmMisc,
            CreatureHelpHeal,
            CreatureHelpMisc,
            ItemHarmBreak,
            ItemHarmMisc,
            ItemHelpFix,
            ItemHelpMisc,
            Misc,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
    };

    using EffectTypeVec_t = std::vector<EffectType::Enum>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_EFFECTTYPEENUM_HPP_INCLUDED
