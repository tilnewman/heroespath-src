// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SFXSETENUM_HPP_INCLUDED
#define HEROESPATH_GUI_SFXSETENUM_HPP_INCLUDED
//
// sfx-set-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct sound_effect_set : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Prompt = 0,
            Switch,
            TickOn,
            TickOff,
            Thock,
            Coin,
            Gem,
            MeteorShard,
            ItemGive,
            ItemDrop,
            BlowpipeShoot,
            BlowpipeHit,
            ArrowShoot,
            ArrowHit,
            ProjectileMiss,
            MeleeMiss,
            FistHit,
            TentacleHit,
            WhipHit,
            WhipMiss,
            MaterialHitMetal,
            MaterialHitMisc,
            ClawHit,
            Wind,
            SpellSelect,
            SpellBandage,
            DrumBlip,
            GuitarStrum,
            CombatWin,
            CombatLose,
            Count
        };

        static const std::string ToString(const Enum);
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SFXSETENUM_HPP_INCLUDED
