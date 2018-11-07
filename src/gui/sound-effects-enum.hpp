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
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    struct Footstep : public misc::EnumBaseCounting<Footstep, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Grass = 0,
            Gravel,
            Leaves,
            Solid,
            Wood,
            Count
        };

        static const std::string ToString(const Enum);
    };

    struct sound_effect : public misc::EnumBaseCounting<sound_effect, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
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
            MeteorShards,
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
            DeathBeastRaugh1,
            DeathBeastRaugh2,
            DeathBeastRaugh3,
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

        static const std::string ToString(const sound_effect::Enum);
        static const std::string Filename(const sound_effect::Enum);
        static const std::string Directory(const sound_effect::Enum);

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

        static const std::string MapTransitionToString(const MapTransition);
        static MapTransition MapTransitionFromString(const std::string &);
        static sound_effect::Enum RandomMapTransitionSfx(const MapTransition, const DoorAction);
        static Enum FootstepToSfx(const Footstep::Enum);
    };

    using SfxEnumVec_t = std::vector<sound_effect::Enum>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SOUNDEFFECTSENUM_HPP_INCLUDED
