// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SOUNDEFFECTSENUM_HPP_INCLUDED
#define HEROESPATH_GUI_SOUNDEFFECTSENUM_HPP_INCLUDED
//
// sound-effects-enum.hpp
//  An enum defining the various sound effects
//
#include "misc/enum-common.hpp"
#include "misc/strings.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    struct Footstep : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Grass = 0,
            Gravel,
            Leaves,
            Solid,
            Wood,
            Count
        };
    };

    struct sound_effect : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            WindGust1 = 0,
            WindGust2,
            WindGust3,
            PromptGeneric,
            PromptQuestion,
            PromptWarn,
            Switch1,
            Switch2,
            Switch3,
            Thock1,
            Thock2,
            Thock3,
            Thock4,
            Thock5,
            Thock6,
            TickOff1,
            TickOff2,
            TickOn1,
            TickOn2,
            Coins1,
            Coins2,
            Coins3,
            Coins4,
            Coins5,
            Coins6,
            Coins7,
            Coins8,
            Coins9,
            Coins10,
            Coins11,
            Coins12,
            Coins13,
            Coins14,
            Coins15,
            Coins16,
            Gems,
            Shards,
            ItemGive,
            ItemDrop,
            BlowpipeShoot1,
            BlowpipeShoot2,
            BlowpipeShoot3,
            BlowpipeHit1,
            BlowpipeHit2,
            BlowpipeHit3,
            ArrowShoot1,
            ArrowShoot2,
            ArrowShoot3,
            ArrowShoot4,
            ArrowShoot5,
            ArrowShoot6,
            ArrowHit,
            ProjectileMiss1,
            ProjectileMiss2,
            ProjectileMiss3,
            ProjectileMiss4,
            ProjectileMiss5,
            ProjectileMiss6,
            ProjectileMiss7,
            ProjectileMiss8,
            ProjectileMiss9,
            ProjectileMiss10,
            ProjectileMiss11,
            ProjectileMiss12,
            ProjectileMiss13,
            MeleeMiss1,
            MeleeMiss2,
            MeleeMiss3,
            MeleeMiss4,
            MeleeMiss5,
            MeleeMiss6,
            MeleeMiss7,
            MeleeMiss8,
            MeleeMiss9,
            MeleeMiss10,
            MeleeMiss11,
            MeleeMiss12,
            MeleeMiss13,
            MeleeMiss14,
            MeleeMiss15,
            MeleeMiss16,
            MeleeMiss17,
            MeleeMiss18,
            MeleeMiss19,
            MeleeMiss20,
            MeleeMiss21,
            MeleeMiss22,
            MeleeMiss23,
            FistHit1,
            FistHit2,
            FistHit3,
            FistHit4,
            FistHit5,
            FistHit6,
            FistHit7,
            FistHit8,
            BreathHitFirebrand,
            BreathHitSylavin,
            TentacleHit1,
            TentacleHit2,
            TentacleHit3,
            TentacleHit4,
            WhipHit,
            WhipMiss1,
            WhipMiss2,
            WhipMiss3,
            WhipMiss4,
            WhipMiss5,
            WhipMiss6,
            WhipMiss7,
            WhipMiss8,
            WhipMiss9,
            WhipMiss10,
            MaterialHitMetal1,
            MaterialHitMetal2,
            MaterialHitMetal3,
            MaterialHitMetal4,
            MaterialHitMetal5,
            MaterialHitBone,
            MaterialHitCloth,
            MaterialHitFlesh,
            MaterialHitHide,
            MaterialHitHorn,
            MaterialHitLeather,
            MaterialHitLiquid,
            MaterialHitPlant,
            MaterialHitScale,
            MaterialHitStone,
            MaterialHitTooth,
            MaterialHitWood,
            MaterialHitMisc,
            ClawTear,
            Magic1,
            Trip,
            GuitarStrum1,
            GuitarStrum2,
            DrumBlip1,
            DrumBlip2,
            DrumBlip3,
            DrumBlip4,
            DrumBlip5,
            DrumBlip6,
            SpellSelect1,
            SpellSelect2,
            SpellSelect3,
            SpellSparks,
            SpellBandage1,
            SpellBandage2,
            SpellBandage3,
            SpellBandage4,
            SpellAwaken,
            SpellClearMind,
            SpellAntidote,
            SpellLift,
            SpellSleep,
            SpellTrip,
            SpellDaze,
            SpellPoison,
            SpellFrighten,
            SpellPoisonCloud,
            SongRallyDrum,
            SongSpiritResonance,
            SongRousingRhythm,
            SongTripBeat,
            SongPanicStrings,
            SongLullaby,
            CombatWin1,
            CombatWin2,
            CombatWin3,
            CombatLose1,
            CombatLose2,
            CombatLose3,
            CharacterDeath,
            RoarDragonFirebrandHatchling,
            RoarDragonFirebrandWhelp,
            RoarDragonFirebrandFledgling,
            RoarDragonFirebrandJuvenile,
            RoarDragonFirebrandAdult,
            RoarDragonFirebrandWyrm,
            RoarDragonFirebrandSkycaster,
            RoarDragonFirebrandElder,
            RoarDragonSylavinHatchling,
            RoarDragonSylavinWhelp,
            RoarDragonSylavinFledgling,
            RoarDragonSylavinJuvenile,
            RoarDragonSylavinAdult,
            RoarDragonSylavinWyrm,
            RoarDragonSylavinSkycaster,
            RoarDragonSylavinElder,
            RoarWolfenPup,
            RoarWolfenJuvenile,
            RoarWolfenAdult,
            RoarWolfenNoble,
            RoarWolfenHighborn,
            RoarWolfenElder,
            DeathBeast1,
            DeathBeast2,
            DeathBeast3,
            DeathBeast4,
            DeathBeast5,
            DeathBeast6,
            DeathBeast7,
            DeathBeast8aGroan,
            DeathBeast8bGroan,
            DeathBeastBig1,
            DeathBeastBig2,
            DeathBeastBig3,
            DeathBeastCaveTroll1,
            DeathBeastCaveTroll2,
            DeathBeastCaveTroll3,
            DeathBeastHuge1a,
            DeathBeastHuge1b,
            DeathBeastRough1,
            DeathBeastRough2,
            DeathBeastRough3,
            DeathBeastSmallRaspy1,
            DeathBeastSmallRaspy2,
            DeathBeastSmallRaspy3,
            DeathBeastSmallRaspy4,
            DeathBeastTalker1,
            DeathBeastTalker2Roar,
            DeathBeastWet,
            DeathCreepy1,
            DeathCreepy2,
            DeathCreepy3,
            DeathCreepy4,
            DeathCreepy5,
            DeathCreepy6,
            DeathDragon1a,
            DeathDragon1b,
            DeathGhostly1a,
            DeathGhostly1b,
            DeathGhostly1c,
            DeathHowlDemon,
            DeathHuman1,
            DeathHuman2,
            DeathHuman3,
            DeathHuman4,
            DeathHuman5,
            DeathHumanFemale1,
            DeathHumanFemale2,
            DeathHumanFemale3,
            DeathHumanMale1,
            DeathHumanMale2,
            DeathHumanMale3,
            DeathHumanMale4,
            DeathHumanMale5,
            DeathHumanMale6,
            DeathHumanMale7,
            DeathLion,
            DeathSnake1,
            DeathSnake2,
            DeathSnake3,
            DeathSnake4,
            DeathSnake5,
            DeathSnake6,
            DeathSpider1,
            DeathSpider2,
            DeathSpider3,
            DeathThing1,
            DeathThing2,
            DeathThing3,
            DeathWitch,
            DeathGriffin,
            DeathWolf,
            DeathBoar,
            DeathLionboar,
            Achievement,
            TreasureOpen1,
            TreasureOpen2,
            TreasureOpen3,
            TreasureOpen4,
            TreasureOpen5,
            LockPicking1,
            LockPicking2,
            LockPicking3,
            LockPicking4,
            LockPicking5,
            LockPicking6,
            LockPicking7,
            LockPickingFail,
            LockUnlock,
            TrapBang,
            TrapBoom,
            TrapChiaowahh,
            TrapChungSplutter,
            TrapFire,
            TrapFireball,
            TrapGasExhale,
            TrapGasLeak,
            TrapGhost1,
            TrapGhost2,
            TrapInferno,
            TrapMetalBang,
            TrapMetal,
            TrapPop,
            TrapSparksAhh,
            TrapSparksHiss,
            TrapSpiritShortHiss,
            TrapSplatDunk,
            TrapSplutter,
            DoorCommonClose1,
            DoorCommonClose2,
            DoorCommonClose3,
            DoorCommonOpen1,
            DoorCommonOpen2,
            DoorLocked,
            DoorOldClose1,
            DoorOldOpen1,
            DoorOldOpen2,
            DoorRattlyClose1,
            DoorRattlyClose2,
            DoorRattlyClose3,
            DoorRattlyOpen1,
            DoorSqueakyClose1,
            DoorSqueakyOpen1,
            DoorSqueakyOpen2,
            DoorSqueakyOpen3,
            Stairs,
            FootstepGrass,
            FootstepGravel,
            FootstepLeaves,
            FootstepSolid,
            FootstepWood,
            Count,
            None,
            Random
        };

        static constexpr std::string_view Filename(const sound_effect::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case WindGust1: return "WindGust1.ogg";
                case WindGust2: return "WindGust2.ogg";
                case WindGust3: return "WindGust3.ogg";
                case PromptGeneric: return "PromptGeneric.ogg";
                case PromptQuestion: return "PromptQuestion.ogg";
                case PromptWarn: return "PromptWarn.ogg";
                case Switch1: return "Switch1.ogg";
                case Switch2: return "Switch2.ogg";
                case Switch3: return "Switch3.ogg";
                case Thock1: return "Thock1.ogg";
                case Thock2: return "Thock2.ogg";
                case Thock3: return "Thock3.ogg";
                case Thock4: return "Thock4.ogg";
                case Thock5: return "Thock5.ogg";
                case Thock6: return "Thock6.ogg";
                case TickOff1: return "TickOff1.ogg";
                case TickOff2: return "TickOff2.ogg";
                case TickOn1: return "TickOn1.ogg";
                case TickOn2: return "TickOn2.ogg";
                case Coins1: return "coins-1.ogg";
                case Coins2: return "coins-2.ogg";
                case Coins3: return "coins-3.ogg";
                case Coins4: return "coins-4.ogg";
                case Coins5: return "coins-5.ogg";
                case Coins6: return "coins-6.ogg";
                case Coins7: return "coins-7.ogg";
                case Coins8: return "coins-8.ogg";
                case Coins9: return "coins-9.ogg";
                case Coins10: return "coins-10.ogg";
                case Coins11: return "coins-11.ogg";
                case Coins12: return "coins-12.ogg";
                case Coins13: return "coins-13.ogg";
                case Coins14: return "coins-14.ogg";
                case Coins15: return "coins-15.ogg";
                case Coins16: return "coins-16.ogg";
                case Gems: return "gems.ogg";
                case Shards: return "meteor_shards.ogg";
                case ItemGive: return "item-give.ogg";
                case ItemDrop: return "item-drop.ogg";
                case BlowpipeShoot1: return "blowpipe-shoot-1.ogg";
                case BlowpipeShoot2: return "blowpipe-shoot-2.ogg";
                case BlowpipeShoot3: return "blowpipe-shoot-3.ogg";
                case BlowpipeHit1: return "blowpipe-hit-1.ogg";
                case BlowpipeHit2: return "blowpipe-hit-2.ogg";
                case BlowpipeHit3: return "blowpipe-hit-3.ogg";
                case ArrowShoot1: return "arrow-shoot-1.ogg";
                case ArrowShoot2: return "arrow-shoot-2.ogg";
                case ArrowShoot3: return "arrow-shoot-3.ogg";
                case ArrowShoot4: return "arrow-shoot-4.ogg";
                case ArrowShoot5: return "arrow-shoot-5.ogg";
                case ArrowShoot6: return "arrow-shoot-6.ogg";
                case ArrowHit: return "arrow-hit.ogg";
                case ProjectileMiss1: return "projectile-miss-1.ogg";
                case ProjectileMiss2: return "projectile-miss-2.ogg";
                case ProjectileMiss3: return "projectile-miss-3.ogg";
                case ProjectileMiss4: return "projectile-miss-4.ogg";
                case ProjectileMiss5: return "projectile-miss-5.ogg";
                case ProjectileMiss6: return "projectile-miss-6.ogg";
                case ProjectileMiss7: return "projectile-miss-7.ogg";
                case ProjectileMiss8: return "projectile-miss-8.ogg";
                case ProjectileMiss9: return "projectile-miss-9.ogg";
                case ProjectileMiss10: return "projectile-miss-10.ogg";
                case ProjectileMiss11: return "projectile-miss-11.ogg";
                case ProjectileMiss12: return "projectile-miss-12.ogg";
                case ProjectileMiss13: return "projectile-miss-13.ogg";
                case MeleeMiss1: return "melee-miss-1.ogg";
                case MeleeMiss2: return "melee-miss-2.ogg";
                case MeleeMiss3: return "melee-miss-3.ogg";
                case MeleeMiss4: return "melee-miss-4.ogg";
                case MeleeMiss5: return "melee-miss-5.ogg";
                case MeleeMiss6: return "melee-miss-6.ogg";
                case MeleeMiss7: return "melee-miss-7.ogg";
                case MeleeMiss8: return "melee-miss-8.ogg";
                case MeleeMiss9: return "melee-miss-9.ogg";
                case MeleeMiss10: return "melee-miss-10.ogg";
                case MeleeMiss11: return "melee-miss-11.ogg";
                case MeleeMiss12: return "melee-miss-12.ogg";
                case MeleeMiss13: return "melee-miss-13.ogg";
                case MeleeMiss14: return "melee-miss-14.ogg";
                case MeleeMiss15: return "melee-miss-15.ogg";
                case MeleeMiss16: return "melee-miss-16.ogg";
                case MeleeMiss17: return "melee-miss-17.ogg";
                case MeleeMiss18: return "melee-miss-18.ogg";
                case MeleeMiss19: return "melee-miss-19.ogg";
                case MeleeMiss20: return "melee-miss-20.ogg";
                case MeleeMiss21: return "melee-miss-21.ogg";
                case MeleeMiss22: return "melee-miss-22.ogg";
                case MeleeMiss23: return "melee-miss-23.ogg";
                case FistHit1: return "fist-hit-1.ogg";
                case FistHit2: return "fist-hit-2.ogg";
                case FistHit3: return "fist-hit-3.ogg";
                case FistHit4: return "fist-hit-4.ogg";
                case FistHit5: return "fist-hit-5.ogg";
                case FistHit6: return "fist-hit-6.ogg";
                case FistHit7: return "fist-hit-7.ogg";
                case FistHit8: return "fist-hit-8.ogg";
                case BreathHitFirebrand: return "firebrand.ogg";
                case BreathHitSylavin: return "sylavin.ogg";
                case TentacleHit1: return "tentacle-hit-1.ogg";
                case TentacleHit2: return "tentacle-hit-2.ogg";
                case TentacleHit3: return "tentacle-hit-3.ogg";
                case TentacleHit4: return "tentacle-hit-4.ogg";
                case WhipHit: return "whip-hit.ogg";
                case WhipMiss1: return "whip-miss-1.ogg";
                case WhipMiss2: return "whip-miss-2.ogg";
                case WhipMiss3: return "whip-miss-3.ogg";
                case WhipMiss4: return "whip-miss-4.ogg";
                case WhipMiss5: return "whip-miss-5.ogg";
                case WhipMiss6: return "whip-miss-6.ogg";
                case WhipMiss7: return "whip-miss-7.ogg";
                case WhipMiss8: return "whip-miss-8.ogg";
                case WhipMiss9: return "whip-miss-9.ogg";
                case WhipMiss10: return "whip-miss-10.ogg";
                case MaterialHitMetal1: return "material-hit-metal-1.ogg";
                case MaterialHitMetal2: return "material-hit-metal-2.ogg";
                case MaterialHitMetal3: return "material-hit-metal-3.ogg";
                case MaterialHitMetal4: return "material-hit-metal-4.ogg";
                case MaterialHitMetal5: return "material-hit-metal-5.ogg";
                case MaterialHitBone: return "material-hit-bone.ogg";
                case MaterialHitCloth: return "material-hit-cloth.ogg";
                case MaterialHitFlesh: return "material-hit-flesh.ogg";
                case MaterialHitHide: return "material-hit-hide.ogg";
                case MaterialHitHorn: return "material-hit-horn.ogg";
                case MaterialHitLeather: return "material-hit-leather.ogg";
                case MaterialHitLiquid: return "material-hit-liquid.ogg";
                case MaterialHitPlant: return "material-hit-plant.ogg";
                case MaterialHitScale: return "material-hit-scale.ogg";
                case MaterialHitStone: return "material-hit-stone.ogg";
                case MaterialHitTooth: return "material-hit-tooth.ogg";
                case MaterialHitWood: return "material-hit-wood.ogg";
                case MaterialHitMisc: return "material-hit-misc.ogg";
                case ClawTear: return "claw-tear.ogg";
                case Magic1: return "magic1.ogg";
                case Trip: return "trip-and-fall.ogg";
                case GuitarStrum1: return "guitar-strum1.ogg";
                case GuitarStrum2: return "guitar-strum2.ogg";
                case DrumBlip1: return "drum-blip1.ogg";
                case DrumBlip2: return "drum-blip2.ogg";
                case DrumBlip3: return "drum-blip3.ogg";
                case DrumBlip4: return "drum-blip4.ogg";
                case DrumBlip5: return "drum-blip5.ogg";
                case DrumBlip6: return "drum-blip6.ogg";
                case SpellSelect1: return "select-1.ogg";
                case SpellSelect2: return "select-2.ogg";
                case SpellSelect3: return "select-3.ogg";
                case SpellSparks: return "sparks.ogg";
                case SpellBandage1: return "bandage-1.ogg";
                case SpellBandage2: return "bandage-2.ogg";
                case SpellBandage3: return "bandage-3.ogg";
                case SpellBandage4: return "bandage-4.ogg";
                case SpellAwaken: return "awaken.ogg";
                case SpellClearMind: return "clear-mind.ogg";
                case SpellAntidote: return "antidote.ogg";
                case SpellLift: return "lift.ogg";
                case SpellSleep: return "sleep.ogg";
                case SpellTrip: return "trip.ogg";
                case SpellDaze: return "daze.ogg";
                case SpellPoison: return "poison.ogg";
                case SpellFrighten: return "fright.ogg";
                case SpellPoisonCloud: return "poison-cloud.ogg";
                case SongRallyDrum: return "rally-drum.ogg";
                case SongSpiritResonance: return "spirit-resonance.ogg";
                case SongRousingRhythm: return "rousing-rhythm.ogg";
                case SongTripBeat: return "trip-beat.ogg";
                case SongPanicStrings: return "panic-strings.ogg";
                case SongLullaby: return "lullaby.ogg";
                case CombatWin1: return "win-1.ogg";
                case CombatWin2: return "win-2.ogg";
                case CombatWin3: return "win-3.ogg";
                case CombatLose1: return "lose-1.ogg";
                case CombatLose2: return "lose-2.ogg";
                case CombatLose3: return "lose-3.ogg";
                case CharacterDeath: return "character-death.ogg";
                case RoarDragonFirebrandHatchling: return "dragon-firebrand-hatchling.ogg";
                case RoarDragonFirebrandWhelp: return "dragon-firebrand-whelp.ogg";
                case RoarDragonFirebrandFledgling: return "dragon-firebrand-fledgling.ogg";
                case RoarDragonFirebrandJuvenile: return "dragon-firebrand-juvenile.ogg";
                case RoarDragonFirebrandAdult: return "dragon-firebrand-adult.ogg";
                case RoarDragonFirebrandWyrm: return "dragon-firebrand-wyrm.ogg";
                case RoarDragonFirebrandSkycaster: return "dragon-firebrand-skycaster.ogg";
                case RoarDragonFirebrandElder:
                    return "dragon-firebrand-elder.ogg";

                    // There is no sylavin whelp sfx yet, so re-use the hatchling sfx.  zTn 2017-7-6
                case RoarDragonSylavinWhelp:
                case RoarDragonSylavinHatchling: return "dragon-sylavin-hatchling.ogg";

                case RoarDragonSylavinFledgling: return "dragon-sylavin-fledgling.ogg";
                case RoarDragonSylavinJuvenile: return "dragon-sylavin-juvenile.ogg";
                case RoarDragonSylavinAdult: return "dragon-sylavin-adult.ogg";
                case RoarDragonSylavinWyrm: return "dragon-sylavin-wyrm.ogg";
                case RoarDragonSylavinSkycaster: return "dragon-sylavin-skycaster.ogg";
                case RoarDragonSylavinElder: return "dragon-sylavin-elder.ogg";
                case RoarWolfenPup: return "wolfen-pup.ogg";
                case RoarWolfenJuvenile: return "wolfen-juvenile.ogg";
                case RoarWolfenAdult: return "wolfen-adult.ogg";
                case RoarWolfenNoble: return "wolfen-noble.ogg";
                case RoarWolfenHighborn: return "wolfen-highborn.ogg";
                case RoarWolfenElder: return "wolfen-elder.ogg";
                case DeathBeast1: return "beast-1.ogg";
                case DeathBeast2: return "beast-2.ogg";
                case DeathBeast3: return "beast-3.ogg";
                case DeathBeast4: return "beast-4.ogg";
                case DeathBeast5: return "beast-5.ogg";
                case DeathBeast6: return "beast-6.ogg";
                case DeathBeast7: return "beast-7.ogg";
                case DeathBeast8aGroan: return "beast-8a-groan.ogg";
                case DeathBeast8bGroan: return "beast-8b-groan.ogg";
                case DeathBeastBig1: return "beast-big-1.ogg";
                case DeathBeastBig2: return "beast-big-2.ogg";
                case DeathBeastBig3: return "beast-big-3.ogg";
                case DeathBeastCaveTroll1: return "beast-cave-troll-1.ogg";
                case DeathBeastCaveTroll2: return "beast-cave-troll-2.ogg";
                case DeathBeastCaveTroll3: return "beast-cave-troll-3.ogg";
                case DeathBeastHuge1a: return "beast-huge-1a.ogg";
                case DeathBeastHuge1b: return "beast-huge-1b.ogg";
                case DeathBeastRough1: return "beast-rough-1.ogg";
                case DeathBeastRough2: return "beast-rough-2.ogg";
                case DeathBeastRough3: return "beast-rough-3.ogg";
                case DeathBeastSmallRaspy1: return "beast-small-raspy-1.ogg";
                case DeathBeastSmallRaspy2: return "beast-small-raspy-2.ogg";
                case DeathBeastSmallRaspy3: return "beast-small-raspy-3.ogg";
                case DeathBeastSmallRaspy4: return "beast-small-raspy-4.ogg";
                case DeathBeastTalker1: return "beast-talker-1.ogg";
                case DeathBeastTalker2Roar: return "beast-talker-2-roar.ogg";
                case DeathBeastWet: return "beast-wet.ogg";
                case DeathCreepy1: return "creepy-1.ogg";
                case DeathCreepy2: return "creepy-2.ogg";
                case DeathCreepy3: return "creepy-3.ogg";
                case DeathCreepy4: return "creepy-4.ogg";
                case DeathCreepy5: return "creepy-5.ogg";
                case DeathCreepy6: return "creepy-6.ogg";
                case DeathDragon1a: return "dragon-1a.ogg";
                case DeathDragon1b: return "dragon-1b.ogg";
                case DeathGhostly1a: return "ghostly-1a.ogg";
                case DeathGhostly1b: return "ghostly-1b.ogg";
                case DeathGhostly1c: return "ghostly-1c.ogg";
                case DeathHowlDemon: return "howl-demon.ogg";
                case DeathHuman1: return "human-1.ogg";
                case DeathHuman2: return "human-2.ogg";
                case DeathHuman3: return "human-3.ogg";
                case DeathHuman4: return "human-4.ogg";
                case DeathHuman5: return "human-5.ogg";
                case DeathHumanFemale1: return "human-female-1.ogg";
                case DeathHumanFemale2: return "human-female-2.ogg";
                case DeathHumanFemale3: return "human-female-3.ogg";
                case DeathHumanMale1: return "human-male-1.ogg";
                case DeathHumanMale2: return "human-male-2.ogg";
                case DeathHumanMale3: return "human-male-3.ogg";
                case DeathHumanMale4: return "human-male-4.ogg";
                case DeathHumanMale5: return "human-male-5.ogg";
                case DeathHumanMale6: return "human-male-6.ogg";
                case DeathHumanMale7: return "human-male-7.ogg";
                case DeathLion: return "lion.ogg";
                case DeathSnake1: return "snake-1.ogg";
                case DeathSnake2: return "snake-2.ogg";
                case DeathSnake3: return "snake-3.ogg";
                case DeathSnake4: return "snake-4.ogg";
                case DeathSnake5: return "snake-5.ogg";
                case DeathSnake6: return "snake-6.ogg";
                case DeathSpider1: return "spider-1.ogg";
                case DeathSpider2: return "spider-2.ogg";
                case DeathSpider3: return "spider-3.ogg";
                case DeathThing1: return "thing-1.ogg";
                case DeathThing2: return "thing-2.ogg";
                case DeathThing3: return "thing-3.ogg";
                case DeathWitch: return "witch.ogg";
                case DeathGriffin: return "griffin.ogg";
                case DeathWolf: return "wolf.ogg";
                case DeathBoar: return "boar.ogg";
                case DeathLionboar: return "lionboar.ogg";
                case Achievement: return "achievement.ogg";
                case TreasureOpen1: return "open-1.ogg";
                case TreasureOpen2: return "open-2.ogg";
                case TreasureOpen3: return "open-3.ogg";
                case TreasureOpen4: return "open-4.ogg";
                case TreasureOpen5: return "open-5.ogg";
                case LockPicking1: return "picking-1.ogg";
                case LockPicking2: return "picking-2.ogg";
                case LockPicking3: return "picking-3.ogg";
                case LockPicking4: return "picking-4.ogg";
                case LockPicking5: return "picking-5.ogg";
                case LockPicking6: return "picking-6.ogg";
                case LockPicking7: return "picking-7.ogg";
                case LockPickingFail: return "picking-fail.ogg";
                case LockUnlock: return "unlock.ogg";
                case TrapBang: return "trap-bang.ogg";
                case TrapBoom: return "trap-boom.ogg";
                case TrapChiaowahh: return "trap-chiaowahh.ogg";
                case TrapChungSplutter: return "trap-chung-splutter.ogg";
                case TrapFire: return "trap-fire.ogg";
                case TrapFireball: return "trap-fireball.ogg";
                case TrapGasExhale: return "trap-gas-exhale.ogg";
                case TrapGasLeak: return "trap-gas-leak.ogg";
                case TrapGhost1: return "trap-ghost-1.ogg";
                case TrapGhost2: return "trap-ghost-2.ogg";
                case TrapInferno: return "trap-inferno.ogg";
                case TrapMetalBang: return "trap-metal-bang.ogg";
                case TrapMetal: return "trap-metal.ogg";
                case TrapPop: return "trap-pop.ogg";
                case TrapSparksAhh: return "trap-sparks-ahh.ogg";
                case TrapSparksHiss: return "trap-sparks-hiss.ogg";
                case TrapSpiritShortHiss: return "trap-spirit-short-hiss.ogg";
                case TrapSplatDunk: return "trap-splat-dunk.ogg";
                case TrapSplutter: return "trap-splutter.ogg";
                case DoorCommonClose1: return "door-common-close-1.ogg";
                case DoorCommonClose2: return "door-common-close-2.ogg";
                case DoorCommonClose3: return "door-common-close-3.ogg";
                case DoorCommonOpen1: return "door-common-open-1.ogg";
                case DoorCommonOpen2: return "door-common-open-2.ogg";
                case DoorLocked: return "door-locked.ogg";
                case DoorOldClose1: return "door-old-close-1.ogg";
                case DoorOldOpen1: return "door-old-open-1.ogg";
                case DoorOldOpen2: return "door-old-open-2.ogg";
                case DoorRattlyClose1: return "door-rattly-close-1.ogg";
                case DoorRattlyClose2: return "door-rattly-close-2.ogg";
                case DoorRattlyClose3: return "door-rattly-close-3.ogg";
                case DoorRattlyOpen1: return "door-rattly-open-1.ogg";
                case DoorSqueakyClose1: return "door-squeaky-close-1.ogg";
                case DoorSqueakyOpen1: return "door-squeaky-open-1.ogg";
                case DoorSqueakyOpen2: return "door-squeaky-open-2.ogg";
                case DoorSqueakyOpen3: return "door-squeaky-open-3.ogg";
                case Stairs: return "stairs.ogg";
                case FootstepGrass: return "footstep-grass.ogg";
                case FootstepGravel: return "footstep-gravel.ogg";
                case FootstepLeaves: return "footstep-leaves.ogg";
                case FootstepSolid: return "footstep-solid.ogg";
                case FootstepWood: return "footstep-wood.ogg";

                case None:
                case Count:
                case Random: return "";

                default: return "gui::sound_effect::Filename(ENUM=out_of_range)";
            }
        }

        static constexpr std::string_view Directory(const sound_effect::Enum SFX_ENUM) noexcept
        {
            switch (SFX_ENUM)
            {
                case WindGust1:
                case WindGust2:
                case WindGust3: return "sound-effects/wind-gusts";

                case PromptGeneric:
                case PromptQuestion:
                case PromptWarn: return "sound-effects/prompts";

                case Switch1:
                case Switch2:
                case Switch3:
                case Thock1:
                case Thock2:
                case Thock3:
                case Thock4:
                case Thock5:
                case Thock6:
                case TickOff1:
                case TickOff2:
                case TickOn1:
                case TickOn2: return "sound-effects/user-interface";

                case Coins1:
                case Coins2:
                case Coins3:
                case Coins4:
                case Coins5:
                case Coins6:
                case Coins7:
                case Coins8:
                case Coins9:
                case Coins10:
                case Coins11:
                case Coins12:
                case Coins13:
                case Coins14:
                case Coins15:
                case Coins16: return "sound-effects/inventory/coins";

                case Gems: return "sound-effects/inventory/gems";

                case Shards: return "sound-effects/inventory/meteor_shards";

                case ItemGive:
                case ItemDrop: return "sound-effects/inventory/items";

                case BlowpipeShoot1:
                case BlowpipeShoot2:
                case BlowpipeShoot3: return "sound-effects/combat/blowpipe-shoot";

                case BlowpipeHit1:
                case BlowpipeHit2:
                case BlowpipeHit3: return "sound-effects/combat/blowpipe-hit";

                case ArrowShoot1:
                case ArrowShoot2:
                case ArrowShoot3:
                case ArrowShoot4:
                case ArrowShoot5:
                case ArrowShoot6: return "sound-effects/combat/arrow-shoot";

                case ArrowHit: return "sound-effects/combat/arrow-hit";

                case ProjectileMiss1:
                case ProjectileMiss2:
                case ProjectileMiss3:
                case ProjectileMiss4:
                case ProjectileMiss5:
                case ProjectileMiss6:
                case ProjectileMiss7:
                case ProjectileMiss8:
                case ProjectileMiss9:
                case ProjectileMiss10:
                case ProjectileMiss11:
                case ProjectileMiss12:
                case ProjectileMiss13: return "sound-effects/combat/projectile-miss";

                case MeleeMiss1:
                case MeleeMiss2:
                case MeleeMiss3:
                case MeleeMiss4:
                case MeleeMiss5:
                case MeleeMiss6:
                case MeleeMiss7:
                case MeleeMiss8:
                case MeleeMiss9:
                case MeleeMiss10:
                case MeleeMiss11:
                case MeleeMiss12:
                case MeleeMiss13:
                case MeleeMiss14:
                case MeleeMiss15:
                case MeleeMiss16:
                case MeleeMiss17:
                case MeleeMiss18:
                case MeleeMiss19:
                case MeleeMiss20:
                case MeleeMiss21:
                case MeleeMiss22:
                case MeleeMiss23: return "sound-effects/combat/melee-miss";

                case FistHit1:
                case FistHit2:
                case FistHit3:
                case FistHit4:
                case FistHit5:
                case FistHit6:
                case FistHit7:
                case FistHit8: return "sound-effects/combat/fist-hit";

                case BreathHitFirebrand:
                case BreathHitSylavin: return "sound-effects/combat/breath-hit";

                case TentacleHit1:
                case TentacleHit2:
                case TentacleHit3:
                case TentacleHit4: return "sound-effects/combat/tentacle-hit";

                case WhipHit: return "sound-effects/combat/whip-hit";

                case WhipMiss1:
                case WhipMiss2:
                case WhipMiss3:
                case WhipMiss4:
                case WhipMiss5:
                case WhipMiss6:
                case WhipMiss7:
                case WhipMiss8:
                case WhipMiss9:
                case WhipMiss10: return "sound-effects/combat/whip-miss";

                case MaterialHitMetal1:
                case MaterialHitMetal2:
                case MaterialHitMetal3:
                case MaterialHitMetal4:
                case MaterialHitMetal5:
                case MaterialHitBone:
                case MaterialHitCloth:
                case MaterialHitFlesh:
                case MaterialHitHide:
                case MaterialHitHorn:
                case MaterialHitLeather:
                case MaterialHitLiquid:
                case MaterialHitPlant:
                case MaterialHitScale:
                case MaterialHitStone:
                case MaterialHitTooth:
                case MaterialHitWood:
                case MaterialHitMisc: return "sound-effects/combat/material-hit";

                case ClawTear: return "sound-effects/combat/claw-tear";

                case Magic1: return "sound-effects/misc";

                case Trip: return "sound-effects/combat";

                case GuitarStrum1:
                case GuitarStrum2: return "sound-effects/misc";

                case DrumBlip1:
                case DrumBlip2:
                case DrumBlip3:
                case DrumBlip4:
                case DrumBlip5:
                case DrumBlip6: return "sound-effects/misc";

                case SpellSelect1:
                case SpellSelect2:
                case SpellSelect3:
                case SpellSparks:
                case SpellBandage1:
                case SpellBandage2:
                case SpellBandage3:
                case SpellBandage4:
                case SpellAwaken:
                case SpellClearMind:
                case SpellAntidote:
                case SpellLift:
                case SpellSleep:
                case SpellTrip:
                case SpellDaze:
                case SpellPoison:
                case SpellFrighten:
                case SpellPoisonCloud: return "sound-effects/spells";

                case SongRallyDrum:
                case SongSpiritResonance:
                case SongRousingRhythm:
                case SongTripBeat:
                case SongPanicStrings:
                case SongLullaby: return "sound-effects/songs";

                case CombatWin1:
                case CombatWin2:
                case CombatWin3:
                case CombatLose1:
                case CombatLose2:
                case CombatLose3:
                case CharacterDeath: return "sound-effects/combat";

                case RoarDragonFirebrandHatchling:
                case RoarDragonFirebrandWhelp:
                case RoarDragonFirebrandFledgling:
                case RoarDragonFirebrandJuvenile:
                case RoarDragonFirebrandAdult:
                case RoarDragonFirebrandWyrm:
                case RoarDragonFirebrandSkycaster:
                case RoarDragonFirebrandElder:
                case RoarDragonSylavinHatchling:
                case RoarDragonSylavinWhelp:
                case RoarDragonSylavinFledgling:
                case RoarDragonSylavinJuvenile:
                case RoarDragonSylavinAdult:
                case RoarDragonSylavinWyrm:
                case RoarDragonSylavinSkycaster:
                case RoarDragonSylavinElder:
                case RoarWolfenPup:
                case RoarWolfenJuvenile:
                case RoarWolfenAdult:
                case RoarWolfenNoble:
                case RoarWolfenHighborn:
                case RoarWolfenElder: return "sound-effects/combat/roar";

                case DeathBeast1:
                case DeathBeast2:
                case DeathBeast3:
                case DeathBeast4:
                case DeathBeast5:
                case DeathBeast6:
                case DeathBeast7:
                case DeathBeast8aGroan:
                case DeathBeast8bGroan:
                case DeathBeastBig1:
                case DeathBeastBig2:
                case DeathBeastBig3:
                case DeathBeastCaveTroll1:
                case DeathBeastCaveTroll2:
                case DeathBeastCaveTroll3:
                case DeathBeastHuge1a:
                case DeathBeastHuge1b:
                case DeathBeastRough1:
                case DeathBeastRough2:
                case DeathBeastRough3:
                case DeathBeastSmallRaspy1:
                case DeathBeastSmallRaspy2:
                case DeathBeastSmallRaspy3:
                case DeathBeastSmallRaspy4:
                case DeathBeastTalker1:
                case DeathBeastTalker2Roar:
                case DeathBeastWet:
                case DeathCreepy1:
                case DeathCreepy2:
                case DeathCreepy3:
                case DeathCreepy4:
                case DeathCreepy5:
                case DeathCreepy6:
                case DeathDragon1a:
                case DeathDragon1b:
                case DeathGhostly1a:
                case DeathGhostly1b:
                case DeathGhostly1c:
                case DeathHowlDemon:
                case DeathHuman1:
                case DeathHuman2:
                case DeathHuman3:
                case DeathHuman4:
                case DeathHuman5:
                case DeathHumanFemale1:
                case DeathHumanFemale2:
                case DeathHumanFemale3:
                case DeathHumanMale1:
                case DeathHumanMale2:
                case DeathHumanMale3:
                case DeathHumanMale4:
                case DeathHumanMale5:
                case DeathHumanMale6:
                case DeathHumanMale7:
                case DeathLion:
                case DeathSnake1:
                case DeathSnake2:
                case DeathSnake3:
                case DeathSnake4:
                case DeathSnake5:
                case DeathSnake6:
                case DeathSpider1:
                case DeathSpider2:
                case DeathSpider3:
                case DeathThing1:
                case DeathThing2:
                case DeathThing3:
                case DeathWitch:
                case DeathGriffin:
                case DeathWolf:
                case DeathBoar:
                case DeathLionboar: return "sound-effects/combat/death";

                case Achievement: return "sound-effects/misc";

                case TreasureOpen1:
                case TreasureOpen2:
                case TreasureOpen3:
                case TreasureOpen4:
                case TreasureOpen5: return "sound-effects/treasure";

                case LockPicking1:
                case LockPicking2:
                case LockPicking3:
                case LockPicking4:
                case LockPicking5:
                case LockPicking6:
                case LockPicking7:
                case LockPickingFail:
                case LockUnlock: return "sound-effects/lock";

                case TrapBang:
                case TrapBoom:
                case TrapChiaowahh:
                case TrapChungSplutter:
                case TrapFire:
                case TrapFireball:
                case TrapGasExhale:
                case TrapGasLeak:
                case TrapGhost1:
                case TrapGhost2:
                case TrapInferno:
                case TrapMetalBang:
                case TrapMetal:
                case TrapPop:
                case TrapSparksAhh:
                case TrapSparksHiss:
                case TrapSpiritShortHiss:
                case TrapSplatDunk:
                case TrapSplutter: return "sound-effects/trap";

                case DoorCommonClose1:
                case DoorCommonClose2:
                case DoorCommonClose3:
                case DoorCommonOpen1:
                case DoorCommonOpen2:
                case DoorLocked:
                case DoorOldClose1:
                case DoorOldOpen1:
                case DoorOldOpen2:
                case DoorRattlyClose1:
                case DoorRattlyClose2:
                case DoorRattlyClose3:
                case DoorRattlyOpen1:
                case DoorSqueakyClose1:
                case DoorSqueakyOpen1:
                case DoorSqueakyOpen2:
                case DoorSqueakyOpen3:
                case Stairs: return "sound-effects/map";

                case FootstepGrass:
                case FootstepGravel:
                case FootstepLeaves:
                case FootstepSolid:
                case FootstepWood: return "sound-effects/footstep";

                case Count: return "Count";
                case None: return "(None)";
                case Random: return "(Random)";

                default: return "gui::sound_effect::Directory(ENUM=out_of_range)";
            }
        }

        enum class DoorAction
        {
            Open,
            Close
        };

        enum class MapTransition
        {
            DoorCommon = 0,
            DoorOld,
            DoorRattly,
            DoorSquaeky,
            Stairs,
            Count
        };

        static constexpr std::string_view MapTransitionToString(const MapTransition TYPE) noexcept
        {
            switch (TYPE)
            {
                case MapTransition::DoorCommon: return "DoorCommon";
                case MapTransition::DoorOld: return "DoorOld";
                case MapTransition::DoorRattly: return "DoorRattly";
                case MapTransition::DoorSquaeky: return "DoorSquaeky";
                case MapTransition::Stairs: return "Stairs";
                case MapTransition::Count: return "Count";

                default:
                    return "gui::sound_effect::MapTransitionToString(MapTransition::Enum=out_of_"
                           "range)";
            }
        }

        static inline MapTransition MapTransitionFromString(const std::string_view NAME) noexcept
        {
            const auto MAP_TRANSITION_COUNT = static_cast<EnumUnderlying_t>(MapTransition::Count);

            for (EnumUnderlying_t i(0); i < MAP_TRANSITION_COUNT; ++i)
            {
                const auto ENUM { static_cast<MapTransition>(i) };

                if (misc::AreEqualCaseInsensitive(MapTransitionToString(ENUM), NAME))
                {
                    return ENUM;
                }
            }

            return MapTransition::Count;
        }

        static sound_effect::Enum RandomMapTransitionSfx(const MapTransition, const DoorAction);

        static constexpr Enum FootstepToSfx(const Footstep::Enum ENUM) noexcept
        {
            if (ENUM == Footstep::Grass)
            {
                return FootstepGrass;
            }
            else if (ENUM == Footstep::Gravel)
            {
                return FootstepGravel;
            }
            else if (ENUM == Footstep::Leaves)
            {
                return FootstepLeaves;
            }
            else if (ENUM == Footstep::Solid)
            {
                return FootstepSolid;
            }
            else if (ENUM == Footstep::Wood)
            {
                return FootstepWood;
            }
            else
            {
                return Count;
            }
        }
    };

    using SfxEnumVec_t = std::vector<sound_effect::Enum>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SOUNDEFFECTSENUM_HPP_INCLUDED
