// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sound-effects-enum.cpp
//
#include "sound-effects-enum.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace gui
{

    const std::string Footstep::ToString(const Footstep::Enum ENUM)
    {
        switch (ENUM)
        {
            case Grass:
            {
                return "Grass";
            }
            case Gravel:
            {
                return "Gravel";
            }
            case Leaves:
            {
                return "Leaves";
            }
            case Solid:
            {
                return "Solid";
            }
            case Wood:
            {
                return "Wood";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string sound_effect::ToString(const sound_effect::Enum ENUM)
    {
        switch (ENUM)
        {
            case WindGust1:
            {
                return "WindGust1";
            }
            case WindGust2:
            {
                return "WindGust2";
            }
            case WindGust3:
            {
                return "WindGust3";
            }
            case PromptGeneric:
            {
                return "PromptGeneric";
            }
            case PromptQuestion:
            {
                return "PromptQuestion";
            }
            case PromptWarn:
            {
                return "PromptWarn";
            }
            case Switch1:
            {
                return "Switch1";
            }
            case Switch2:
            {
                return "Switch2";
            }
            case Switch3:
            {
                return "Switch3";
            }
            case Thock1:
            {
                return "Thock1";
            }
            case Thock2:
            {
                return "Thock2";
            }
            case Thock3:
            {
                return "Thock3";
            }
            case Thock4:
            {
                return "Thock4";
            }
            case Thock5:
            {
                return "Thock5";
            }
            case Thock6:
            {
                return "Thock6";
            }
            case TickOff1:
            {
                return "TickOff1";
            }
            case TickOff2:
            {
                return "TickOff2";
            }
            case TickOn1:
            {
                return "TickOn1";
            }
            case TickOn2:
            {
                return "TickOn2";
            }
            case Coins1:
            {
                return "coins-1";
            }
            case Coins2:
            {
                return "coins-2";
            }
            case Coins3:
            {
                return "coins-3";
            }
            case Coins4:
            {
                return "coins-4";
            }
            case Coins5:
            {
                return "coins-5";
            }
            case Coins6:
            {
                return "coins-6";
            }
            case Coins7:
            {
                return "coins-7";
            }
            case Coins8:
            {
                return "coins-8";
            }
            case Coins9:
            {
                return "coins-9";
            }
            case Coins10:
            {
                return "coins-10";
            }
            case Coins11:
            {
                return "coins-11";
            }
            case Coins12:
            {
                return "coins-12";
            }
            case Coins13:
            {
                return "coins-13";
            }
            case Coins14:
            {
                return "coins-14";
            }
            case Coins15:
            {
                return "coins-15";
            }
            case Coins16:
            {
                return "coins-16";
            }
            case Gems:
            {
                return "gems";
            }
            case MeteorShards:
            {
                return "meteor_shards";
            }
            case ItemGive:
            {
                return "item-give";
            }
            case ItemDrop:
            {
                return "item-drop";
            }
            case BlowpipeShoot1:
            {
                return "blowpipe-shoot-1";
            }
            case BlowpipeShoot2:
            {
                return "blowpipe-shoot-2";
            }
            case BlowpipeShoot3:
            {
                return "blowpipe-shoot-3";
            }
            case BlowpipeHit1:
            {
                return "blowpipe-hit-1";
            }
            case BlowpipeHit2:
            {
                return "blowpipe-hit-2";
            }
            case BlowpipeHit3:
            {
                return "blowpipe-hit-3";
            }
            case ArrowShoot1:
            {
                return "arrow-shoot-1";
            }
            case ArrowShoot2:
            {
                return "arrow-shoot-2";
            }
            case ArrowShoot3:
            {
                return "arrow-shoot-3";
            }
            case ArrowShoot4:
            {
                return "arrow-shoot-4";
            }
            case ArrowShoot5:
            {
                return "arrow-shoot-5";
            }
            case ArrowShoot6:
            {
                return "arrow-shoot-6";
            }
            case ArrowHit:
            {
                return "arrow-hit";
            }
            case ProjectileMiss1:
            {
                return "projectile-miss-1";
            }
            case ProjectileMiss2:
            {
                return "projectile-miss-2";
            }
            case ProjectileMiss3:
            {
                return "projectile-miss-3";
            }
            case ProjectileMiss4:
            {
                return "projectile-miss-4";
            }
            case ProjectileMiss5:
            {
                return "projectile-miss-5";
            }
            case ProjectileMiss6:
            {
                return "projectile-miss-6";
            }
            case ProjectileMiss7:
            {
                return "projectile-miss-7";
            }
            case ProjectileMiss8:
            {
                return "projectile-miss-8";
            }
            case ProjectileMiss9:
            {
                return "projectile-miss-9";
            }
            case ProjectileMiss10:
            {
                return "projectile-miss-10";
            }
            case ProjectileMiss11:
            {
                return "projectile-miss-11";
            }
            case ProjectileMiss12:
            {
                return "projectile-miss-12";
            }
            case ProjectileMiss13:
            {
                return "projectile-miss-13";
            }
            case MeleeMiss1:
            {
                return "melee-miss-1";
            }
            case MeleeMiss2:
            {
                return "melee-miss-2";
            }
            case MeleeMiss3:
            {
                return "melee-miss-3";
            }
            case MeleeMiss4:
            {
                return "melee-miss-4";
            }
            case MeleeMiss5:
            {
                return "melee-miss-5";
            }
            case MeleeMiss6:
            {
                return "melee-miss-6";
            }
            case MeleeMiss7:
            {
                return "melee-miss-7";
            }
            case MeleeMiss8:
            {
                return "melee-miss-8";
            }
            case MeleeMiss9:
            {
                return "melee-miss-9";
            }
            case MeleeMiss10:
            {
                return "melee-miss-10";
            }
            case MeleeMiss11:
            {
                return "melee-miss-11";
            }
            case MeleeMiss12:
            {
                return "melee-miss-12";
            }
            case MeleeMiss13:
            {
                return "melee-miss-13";
            }
            case MeleeMiss14:
            {
                return "melee-miss-14";
            }
            case MeleeMiss15:
            {
                return "melee-miss-15";
            }
            case MeleeMiss16:
            {
                return "melee-miss-16";
            }
            case MeleeMiss17:
            {
                return "melee-miss-17";
            }
            case MeleeMiss18:
            {
                return "melee-miss-18";
            }
            case MeleeMiss19:
            {
                return "melee-miss-19";
            }
            case MeleeMiss20:
            {
                return "melee-miss-20";
            }
            case MeleeMiss21:
            {
                return "melee-miss-21";
            }
            case MeleeMiss22:
            {
                return "melee-miss-22";
            }
            case MeleeMiss23:
            {
                return "melee-miss-23";
            }
            case FistHit1:
            {
                return "fist-hit-1";
            }
            case FistHit2:
            {
                return "fist-hit-2";
            }
            case FistHit3:
            {
                return "fist-hit-3";
            }
            case FistHit4:
            {
                return "fist-hit-4";
            }
            case FistHit5:
            {
                return "fist-hit-5";
            }
            case FistHit6:
            {
                return "fist-hit-6";
            }
            case FistHit7:
            {
                return "fist-hit-7";
            }
            case FistHit8:
            {
                return "fist-hit-8";
            }
            case BreathHitFirebrand:
            {
                return "firebrand";
            }
            case BreathHitSylavin:
            {
                return "sylavin";
            }
            case TentacleHit1:
            {
                return "tentacle-hit-1";
            }
            case TentacleHit2:
            {
                return "tentacle-hit-2";
            }
            case TentacleHit3:
            {
                return "tentacle-hit-3";
            }
            case TentacleHit4:
            {
                return "tentacle-hit-4";
            }
            case WhipHit:
            {
                return "whip-hit";
            }
            case WhipMiss1:
            {
                return "whip-miss-1";
            }
            case WhipMiss2:
            {
                return "whip-miss-2";
            }
            case WhipMiss3:
            {
                return "whip-miss-3";
            }
            case WhipMiss4:
            {
                return "whip-miss-4";
            }
            case WhipMiss5:
            {
                return "whip-miss-5";
            }
            case WhipMiss6:
            {
                return "whip-miss-6";
            }
            case WhipMiss7:
            {
                return "whip-miss-7";
            }
            case WhipMiss8:
            {
                return "whip-miss-8";
            }
            case WhipMiss9:
            {
                return "whip-miss-9";
            }
            case WhipMiss10:
            {
                return "whip-miss-10";
            }
            case MaterialHitMetal1:
            {
                return "material-hit-metal-1";
            }
            case MaterialHitMetal2:
            {
                return "material-hit-metal-2";
            }
            case MaterialHitMetal3:
            {
                return "material-hit-metal-3";
            }
            case MaterialHitMetal4:
            {
                return "material-hit-metal-4";
            }
            case MaterialHitMetal5:
            {
                return "material-hit-metal-5";
            }
            case MaterialHitBone:
            {
                return "material-hit-bone";
            }
            case MaterialHitCloth:
            {
                return "material-hit-cloth";
            }
            case MaterialHitFlesh:
            {
                return "material-hit-flesh";
            }
            case MaterialHitHide:
            {
                return "material-hit-hide";
            }
            case MaterialHitHorn:
            {
                return "material-hit-horn";
            }
            case MaterialHitLeather:
            {
                return "material-hit-leather";
            }
            case MaterialHitLiquid:
            {
                return "material-hit-liquid";
            }
            case MaterialHitPlant:
            {
                return "material-hit-plant";
            }
            case MaterialHitScale:
            {
                return "material-hit-scale";
            }
            case MaterialHitStone:
            {
                return "material-hit-stone";
            }
            case MaterialHitTooth:
            {
                return "material-hit-tooth";
            }
            case MaterialHitWood:
            {
                return "material-hit-wood";
            }
            case MaterialHitMisc:
            {
                return "material-hit-misc";
            }
            case ClawTear:
            {
                return "claw-tear";
            }
            case Magic1:
            {
                return "magic1";
            }
            case Trip:
            {
                return "trip-and-fall";
            }
            case GuitarStrum1:
            {
                return "guitar-strum1";
            }
            case GuitarStrum2:
            {
                return "guitar-strum2";
            }
            case DrumBlip1:
            {
                return "drum-blip1";
            }
            case DrumBlip2:
            {
                return "drum-blip2";
            }
            case DrumBlip3:
            {
                return "drum-blip3";
            }
            case DrumBlip4:
            {
                return "drum-blip4";
            }
            case DrumBlip5:
            {
                return "drum-blip5";
            }
            case DrumBlip6:
            {
                return "drum-blip6";
            }
            case SpellSelect1:
            {
                return "select-1";
            }
            case SpellSelect2:
            {
                return "select-2";
            }
            case SpellSelect3:
            {
                return "select-3";
            }
            case SpellSparks:
            {
                return "sparks";
            }
            case SpellBandage1:
            {
                return "bandage-1";
            }
            case SpellBandage2:
            {
                return "bandage-2";
            }
            case SpellBandage3:
            {
                return "bandage-3";
            }
            case SpellBandage4:
            {
                return "bandage-4";
            }
            case SpellAwaken:
            {
                return "awaken";
            }
            case SpellClearMind:
            {
                return "clear-mind";
            }
            case SpellAntidote:
            {
                return "antidote";
            }
            case SpellLift:
            {
                return "lift";
            }
            case SpellSleep:
            {
                return "sleep";
            }
            case SpellTrip:
            {
                return "trip";
            }
            case SpellDaze:
            {
                return "daze";
            }
            case SpellPoison:
            {
                return "poison";
            }
            case SpellFrighten:
            {
                return "fright";
            }
            case SpellPoisonCloud:
            {
                return "poison-cloud";
            }
            case SongRallyDrum:
            {
                return "rally-drum";
            }
            case SongSpiritResonance:
            {
                return "spirit-resonance";
            }
            case SongRousingRhythm:
            {
                return "rousing-rhythm";
            }
            case SongTripBeat:
            {
                return "trip-beat";
            }
            case SongPanicStrings:
            {
                return "panic-strings";
            }
            case SongLullaby:
            {
                return "lullaby";
            }
            case CombatWin1:
            {
                return "win-1";
            }
            case CombatWin2:
            {
                return "win-2";
            }
            case CombatWin3:
            {
                return "win-3";
            }
            case CombatLose1:
            {
                return "lose-1";
            }
            case CombatLose2:
            {
                return "lose-2";
            }
            case CombatLose3:
            {
                return "lose-3";
            }
            case CharacterDeath:
            {
                return "character-death";
            }
            case RoarDragonFirebrandHatchling:
            {
                return "dragon-firebrand-hatchling";
            }
            case RoarDragonFirebrandWhelp:
            {
                return "dragon-firebrand-whelp";
            }
            case RoarDragonFirebrandFledgling:
            {
                return "dragon-firebrand-fledgling";
            }
            case RoarDragonFirebrandJuvenile:
            {
                return "dragon-firebrand-juvenile";
            }
            case RoarDragonFirebrandAdult:
            {
                return "dragon-firebrand-adult";
            }
            case RoarDragonFirebrandWyrm:
            {
                return "dragon-firebrand-wyrm";
            }
            case RoarDragonFirebrandSkycaster:
            {
                return "dragon-firebrand-skycaster";
            }
            case RoarDragonFirebrandElder:
            {
                return "dragon-firebrand-elder";
            }
            case RoarDragonSylavinHatchling:
            {
                return "dragon-sylavin-hatchling";
            }
            case RoarDragonSylavinWhelp:
            {
                return "dragon-sylavin-whelp";
            }
            case RoarDragonSylavinFledgling:
            {
                return "dragon-sylavin-fledgling";
            }
            case RoarDragonSylavinJuvenile:
            {
                return "dragon-sylavin-juvenile";
            }
            case RoarDragonSylavinAdult:
            {
                return "dragon-sylavin-adult";
            }
            case RoarDragonSylavinWyrm:
            {
                return "dragon-sylavin-wyrm";
            }
            case RoarDragonSylavinSkycaster:
            {
                return "dragon-sylavin-skycaster";
            }
            case RoarDragonSylavinElder:
            {
                return "dragon-sylavin-elder";
            }
            case RoarWolfenPup:
            {
                return "wolfen-pup";
            }
            case RoarWolfenJuvenile:
            {
                return "wolfen-juvenile";
            }
            case RoarWolfenAdult:
            {
                return "wolfen-adult";
            }
            case RoarWolfenNoble:
            {
                return "wolfen-noble";
            }
            case RoarWolfenHighborn:
            {
                return "wolfen-highborn";
            }
            case RoarWolfenElder:
            {
                return "wolfen-elder";
            }
            case DeathBeast1:
            {
                return "beast-1";
            }
            case DeathBeast2:
            {
                return "beast-2";
            }
            case DeathBeast3:
            {
                return "beast-3";
            }
            case DeathBeast4:
            {
                return "beast-4";
            }
            case DeathBeast5:
            {
                return "beast-5";
            }
            case DeathBeast6:
            {
                return "beast-6";
            }
            case DeathBeast7:
            {
                return "beast-7";
            }
            case DeathBeast8aGroan:
            {
                return "beast-8a-groan";
            }
            case DeathBeast8bGroan:
            {
                return "beast-8b-groan";
            }
            case DeathBeastBig1:
            {
                return "beast-big-1";
            }
            case DeathBeastBig2:
            {
                return "beast-big-2";
            }
            case DeathBeastBig3:
            {
                return "beast-big-3";
            }
            case DeathBeastCaveTroll1:
            {
                return "beast-cave-troll-1";
            }
            case DeathBeastCaveTroll2:
            {
                return "beast-cave-troll-2";
            }
            case DeathBeastCaveTroll3:
            {
                return "beast-cave-troll-3";
            }
            case DeathBeastHuge1a:
            {
                return "beast-huge-1a";
            }
            case DeathBeastHuge1b:
            {
                return "beast-huge-1b";
            }
            case DeathBeastRough1:
            {
                return "beast-rough-1";
            }
            case DeathBeastRough2:
            {
                return "beast-rough-2";
            }
            case DeathBeastRough3:
            {
                return "beast-rough-3";
            }
            case DeathBeastSmallRaspy1:
            {
                return "beast-small-raspy-1";
            }
            case DeathBeastSmallRaspy2:
            {
                return "beast-small-raspy-2";
            }
            case DeathBeastSmallRaspy3:
            {
                return "beast-small-raspy-3";
            }
            case DeathBeastSmallRaspy4:
            {
                return "beast-small-raspy-4";
            }
            case DeathBeastTalker1:
            {
                return "beast-talker-1";
            }
            case DeathBeastTalker2Roar:
            {
                return "beast-talker-2-roar";
            }
            case DeathBeastWet:
            {
                return "beast-wet";
            }
            case DeathCreepy1:
            {
                return "creepy-1";
            }
            case DeathCreepy2:
            {
                return "creepy-2";
            }
            case DeathCreepy3:
            {
                return "creepy-3";
            }
            case DeathCreepy4:
            {
                return "creepy-4";
            }
            case DeathCreepy5:
            {
                return "creepy-5";
            }
            case DeathCreepy6:
            {
                return "creepy-6";
            }
            case DeathDragon1a:
            {
                return "dragon-1a";
            }
            case DeathDragon1b:
            {
                return "dragon-1b";
            }
            case DeathGhostly1a:
            {
                return "ghostly-1a";
            }
            case DeathGhostly1b:
            {
                return "ghostly-1b";
            }
            case DeathGhostly1c:
            {
                return "ghostly-1c";
            }
            case DeathHowlDemon:
            {
                return "howl-demon";
            }
            case DeathHuman1:
            {
                return "human-1";
            }
            case DeathHuman2:
            {
                return "human-2";
            }
            case DeathHuman3:
            {
                return "human-3";
            }
            case DeathHuman4:
            {
                return "human-4";
            }
            case DeathHuman5:
            {
                return "human-5";
            }
            case DeathHumanFemale1:
            {
                return "human-female-1";
            }
            case DeathHumanFemale2:
            {
                return "human-female-2";
            }
            case DeathHumanFemale3:
            {
                return "human-female-3";
            }
            case DeathHumanMale1:
            {
                return "human-male-1";
            }
            case DeathHumanMale2:
            {
                return "human-male-2";
            }
            case DeathHumanMale3:
            {
                return "human-male-3";
            }
            case DeathHumanMale4:
            {
                return "human-male-4";
            }
            case DeathHumanMale5:
            {
                return "human-male-5";
            }
            case DeathHumanMale6:
            {
                return "human-male-6";
            }
            case DeathHumanMale7:
            {
                return "human-male-7";
            }
            case DeathLion:
            {
                return "lion";
            }
            case DeathSnake1:
            {
                return "snake-1";
            }
            case DeathSnake2:
            {
                return "snake-2";
            }
            case DeathSnake3:
            {
                return "snake-3";
            }
            case DeathSnake4:
            {
                return "snake-4";
            }
            case DeathSnake5:
            {
                return "snake-5";
            }
            case DeathSnake6:
            {
                return "snake-6";
            }
            case DeathSpider1:
            {
                return "spider-1";
            }
            case DeathSpider2:
            {
                return "spider-2";
            }
            case DeathSpider3:
            {
                return "spider-3";
            }
            case DeathThing1:
            {
                return "thing-1";
            }
            case DeathThing2:
            {
                return "thing-2";
            }
            case DeathThing3:
            {
                return "thing-3";
            }
            case DeathWitch:
            {
                return "witch";
            }
            case DeathGriffin:
            {
                return "griffin";
            }
            case DeathWolf:
            {
                return "wolf";
            }
            case DeathBoar:
            {
                return "boar";
            }
            case DeathLionboar:
            {
                return "lionboar";
            }
            case Achievement:
            {
                return "achievement";
            }
            case TreasureOpen1:
            {
                return "open-1";
            }
            case TreasureOpen2:
            {
                return "open-2";
            }
            case TreasureOpen3:
            {
                return "open-3";
            }
            case TreasureOpen4:
            {
                return "open-4";
            }
            case TreasureOpen5:
            {
                return "open-5";
            }
            case LockPicking1:
            {
                return "picking-1";
            }
            case LockPicking2:
            {
                return "picking-2";
            }
            case LockPicking3:
            {
                return "picking-3";
            }
            case LockPicking4:
            {
                return "picking-4";
            }
            case LockPicking5:
            {
                return "picking-5";
            }
            case LockPicking6:
            {
                return "picking-6";
            }
            case LockPicking7:
            {
                return "picking-7";
            }
            case LockPickingFail:
            {
                return "picking-fail";
            }
            case LockUnlock:
            {
                return "unlock";
            }
            case TrapBang:
            {
                return "trap-bang";
            }
            case TrapBoom:
            {
                return "trap-boom";
            }
            case TrapChiaowahh:
            {
                return "trap-chiaowahh";
            }
            case TrapChungSplutter:
            {
                return "trap-chung-splutter";
            }
            case TrapFire:
            {
                return "trap-fire";
            }
            case TrapFireball:
            {
                return "trap-fireball";
            }
            case TrapGasExhale:
            {
                return "trap-gas-exhale";
            }
            case TrapGasLeak:
            {
                return "trap-gas-leak";
            }
            case TrapGhost1:
            {
                return "trap-ghost-1";
            }
            case TrapGhost2:
            {
                return "trap-ghost-2";
            }
            case TrapInferno:
            {
                return "trap-inferno";
            }
            case TrapMetalBang:
            {
                return "trap-metal-bang";
            }
            case TrapMetal:
            {
                return "trap-metal";
            }
            case TrapPop:
            {
                return "trap-pop";
            }
            case TrapSparksAhh:
            {
                return "trap-sparks-ahh";
            }
            case TrapSparksHiss:
            {
                return "trap-sparks-hiss";
            }
            case TrapSpiritShortHiss:
            {
                return "trap-spirit-short-hiss";
            }
            case TrapSplatDunk:
            {
                return "trap-splat-dunk";
            }
            case TrapSplutter:
            {
                return "trap-splutter";
            }
            case DoorCommonClose1:
            {
                return "door-common-close-1";
            }
            case DoorCommonClose2:
            {
                return "door-common-close-2";
            }
            case DoorCommonClose3:
            {
                return "door-common-close-3";
            }
            case DoorCommonOpen1:
            {
                return "door-common-open-1";
            }
            case DoorCommonOpen2:
            {
                return "door-common-open-2";
            }
            case DoorLocked:
            {
                return "door-locked";
            }
            case DoorOldClose1:
            {
                return "door-old-close-1";
            }
            case DoorOldOpen1:
            {
                return "door-old-open-1";
            }
            case DoorOldOpen2:
            {
                return "door-old-open-2";
            }
            case DoorRattlyClose1:
            {
                return "door-rattly-close-1";
            }
            case DoorRattlyClose2:
            {
                return "door-rattly-close-2";
            }
            case DoorRattlyClose3:
            {
                return "door-rattly-close-3";
            }
            case DoorRattlyOpen1:
            {
                return "door-rattly-open-1";
            }
            case DoorSqueakyClose1:
            {
                return "door-squeaky-close-1";
            }
            case DoorSqueakyOpen1:
            {
                return "door-squeaky-open-1";
            }
            case DoorSqueakyOpen2:
            {
                return "door-squeaky-open-2";
            }
            case DoorSqueakyOpen3:
            {
                return "door-squeaky-open-3";
            }
            case Stairs:
            {
                return "stairs";
            }
            case FootstepGrass:
            {
                return "footstep-grass";
            }
            case FootstepGravel:
            {
                return "footstep-gravel";
            }
            case FootstepLeaves:
            {
                return "footstep-leaves";
            }
            case FootstepSolid:
            {
                return "footstep-solid";
            }
            case FootstepWood:
            {
                return "footstep-wood";
            }
            case None:
            {
                return "None";
            }
            case Count:
            {
                return "(Count)";
            }
            case Random:
            {
                return "(Random)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string sound_effect::Filename(const sound_effect::Enum ENUM)
    {
        if (ENUM == RoarDragonSylavinWhelp)
        {
            // There is no sylavin whelp sfx yet, so re-use the hatchling sfx.  zTn 2017-7-6
            return ToString(RoarDragonSylavinHatchling) + ".ogg";
        }
        else
        {
            return ToString(ENUM) + ".ogg";
        }
    }

    const std::string sound_effect::Directory(const sound_effect::Enum SFX_ENUM)
    {
        switch (SFX_ENUM)
        {
            case WindGust1:
            case WindGust2:
            case WindGust3:
            {
                return "sound-effects/wind-gusts";
            }
            case PromptGeneric:
            case PromptQuestion:
            case PromptWarn:
            {
                return "sound-effects/prompts";
            }
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
            case TickOn2:
            {
                return "sound-effects/user-interface";
            }
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
            case Coins16:
            {
                return "sound-effects/inventory/coins";
            }
            case Gems:
            {
                return "sound-effects/inventory/gems";
            }
            case MeteorShards:
            {
                return "sound-effects/inventory/meteor_shards";
            }
            case ItemGive:
            case ItemDrop:
            {
                return "sound-effects/inventory/items";
            }
            case BlowpipeShoot1:
            case BlowpipeShoot2:
            case BlowpipeShoot3:
            {
                return "sound-effects/combat/blowpipe-shoot";
            }
            case BlowpipeHit1:
            case BlowpipeHit2:
            case BlowpipeHit3:
            {
                return "sound-effects/combat/blowpipe-hit";
            }
            case ArrowShoot1:
            case ArrowShoot2:
            case ArrowShoot3:
            case ArrowShoot4:
            case ArrowShoot5:
            case ArrowShoot6:
            {
                return "sound-effects/combat/arrow-shoot";
            }
            case ArrowHit:
            {
                return "sound-effects/combat/arrow-hit";
            }
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
            case ProjectileMiss13:
            {
                return "sound-effects/combat/projectile-miss";
            }
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
            case MeleeMiss23:
            {
                return "sound-effects/combat/melee-miss";
            }
            case FistHit1:
            case FistHit2:
            case FistHit3:
            case FistHit4:
            case FistHit5:
            case FistHit6:
            case FistHit7:
            case FistHit8:
            {
                return "sound-effects/combat/fist-hit";
            }
            case BreathHitFirebrand:
            case BreathHitSylavin:
            {
                return "sound-effects/combat/breath-hit";
            }
            case TentacleHit1:
            case TentacleHit2:
            case TentacleHit3:
            case TentacleHit4:
            {
                return "sound-effects/combat/tentacle-hit";
            }
            case WhipHit:
            {
                return "sound-effects/combat/whip-hit";
            }
            case WhipMiss1:
            case WhipMiss2:
            case WhipMiss3:
            case WhipMiss4:
            case WhipMiss5:
            case WhipMiss6:
            case WhipMiss7:
            case WhipMiss8:
            case WhipMiss9:
            case WhipMiss10:
            {
                return "sound-effects/combat/whip-miss";
            }
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
            case MaterialHitMisc:
            {
                return "sound-effects/combat/material-hit";
            }
            case ClawTear:
            {
                return "sound-effects/combat/claw-tear";
            }
            case Magic1:
            {
                return "sound-effects/misc";
            }
            case Trip:
            {
                return "sound-effects/combat";
            }
            case GuitarStrum1:
            case GuitarStrum2:
            {
                return "sound-effects/misc";
            }
            case DrumBlip1:
            case DrumBlip2:
            case DrumBlip3:
            case DrumBlip4:
            case DrumBlip5:
            case DrumBlip6:
            {
                return "sound-effects/misc";
            }
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
            case SpellPoisonCloud:
            {
                return "sound-effects/spells";
            }
            case SongRallyDrum:
            case SongSpiritResonance:
            case SongRousingRhythm:
            case SongTripBeat:
            case SongPanicStrings:
            case SongLullaby:
            {
                return "sound-effects/songs";
            }
            case CombatWin1:
            case CombatWin2:
            case CombatWin3:
            case CombatLose1:
            case CombatLose2:
            case CombatLose3:
            case CharacterDeath:
            {
                return "sound-effects/combat";
            }
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
            case RoarWolfenElder:
            {
                return "sound-effects/combat/roar";
            }
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
            case DeathLionboar:
            {
                return "sound-effects/combat/death";
            }
            case Achievement:
            {
                return "sound-effects/misc";
            }
            case TreasureOpen1:
            case TreasureOpen2:
            case TreasureOpen3:
            case TreasureOpen4:
            case TreasureOpen5:
            {
                return "sound-effects/treasure";
            }
            case LockPicking1:
            case LockPicking2:
            case LockPicking3:
            case LockPicking4:
            case LockPicking5:
            case LockPicking6:
            case LockPicking7:
            case LockPickingFail:
            case LockUnlock:
            {
                return "sound-effects/lock";
            }
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
            case TrapSplutter:
            {
                return "sound-effects/trap";
            }
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
            case Stairs:
            {
                return "sound-effects/map";
            }
            case FootstepGrass:
            case FootstepGravel:
            case FootstepLeaves:
            case FootstepSolid:
            case FootstepWood:
            {
                return "sound-effects/footstep";
            }
            case Count:
            {
                return "(Count)";
            }
            case None:
            {
                return "(None)";
            }
            case Random:
            {
                return "(Random)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(SFX_ENUM)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    sound_effect::Enum
        sound_effect::RandomMapTransitionSfx(const MapTransition TYPE, const DoorAction ACTION)
    {
        SfxEnumVec_t vec;

        switch (TYPE)
        {
            case MapTransition::DoorCommon:
            {
                if (ACTION == DoorAction::Open)
                {
                    vec = { sound_effect::DoorCommonOpen1, sound_effect::DoorCommonOpen2 };
                    break;
                }
                else
                {
                    vec = { sound_effect::DoorCommonClose1,
                            sound_effect::DoorCommonClose2,
                            sound_effect::DoorCommonClose3 };
                    break;
                }
            }
            case MapTransition::DoorOld:
            {
                if (ACTION == DoorAction::Open)
                {
                    vec = { sound_effect::DoorOldOpen1, sound_effect::DoorOldOpen2 };
                    break;
                }
                else
                {
                    vec = { sound_effect::DoorOldClose1 };
                    break;
                }
            }
            case MapTransition::DoorRattly:
            {
                if (ACTION == DoorAction::Open)
                {
                    vec = { sound_effect::DoorRattlyOpen1 };
                    break;
                }
                else
                {
                    vec = { sound_effect::DoorRattlyClose1,
                            sound_effect::DoorRattlyClose2,
                            sound_effect::DoorRattlyClose3 };
                    break;
                }
            }
            case MapTransition::DoorSquaeky:
            {
                if (ACTION == DoorAction::Open)
                {
                    vec = { sound_effect::DoorSqueakyOpen1,
                            sound_effect::DoorSqueakyOpen2,
                            sound_effect::DoorSqueakyOpen3 };
                    break;
                }
                else
                {
                    vec = { sound_effect::DoorSqueakyClose1 };
                    break;
                }
            }
            case MapTransition::Stairs:
            {
                vec = { sound_effect::Stairs };
                break;
            }
            case MapTransition::Count:
            {
                M_HP_LOG_ERR(
                    "type=Count, action=" << static_cast<int>(ACTION)
                                          << ") but that type is invalid");

                return sound_effect::Count;
            }
            default:
            {
                M_HP_LOG_ERR(
                    "type=" << static_cast<int>(TYPE) << ", action=" << static_cast<int>(ACTION)
                            << ") but that type is invalid");

                return sound_effect::Count;
            }
        }

        return misc::RandomSelect(vec);
    }

    const std::string sound_effect::MapTransitionToString(const MapTransition TYPE)
    {
        switch (TYPE)
        {
            case MapTransition::DoorCommon:
            {
                return "DoorCommon";
            }
            case MapTransition::DoorOld:
            {
                return "DoorOld";
            }
            case MapTransition::DoorRattly:
            {
                return "DoorRattly";
            }
            case MapTransition::DoorSquaeky:
            {
                return "DoorSquaeky";
            }
            case MapTransition::Stairs:
            {
                return "Stairs";
            }
            case MapTransition::Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR("type=" << static_cast<int>(TYPE) << ") but that type is invalid");
                return "";
            }
        }
    }

    sound_effect::MapTransition sound_effect::MapTransitionFromString(const std::string & NAME)
    {
        for (int i(0); i < static_cast<int>(sound_effect::MapTransition::Count); ++i)
        {
            const auto ENUM { static_cast<sound_effect::MapTransition>(i) };

            if (misc::ToLowerCopy(MapTransitionToString(ENUM)) == misc::ToLowerCopy(NAME))
            {
                return ENUM;
            }
        }

        M_HP_LOG_ERR(
            "The name \"" << NAME << "\" failed to match any sound_effect::MapTransitions.");

        return sound_effect::MapTransition::Count;
    }

    sound_effect::Enum sound_effect::FootstepToSfx(const Footstep::Enum ENUM)
    {
        if (ENUM == Footstep::Grass)
        {
            return sound_effect::FootstepGrass;
        }
        else if (ENUM == Footstep::Gravel)
        {
            return sound_effect::FootstepGravel;
        }
        else if (ENUM == Footstep::Leaves)
        {
            return sound_effect::FootstepLeaves;
        }
        else if (ENUM == Footstep::Solid)
        {
            return sound_effect::FootstepSolid;
        }
        else if (ENUM == Footstep::Wood)
        {
            return sound_effect::FootstepWood;
        }
        else
        {
            return sound_effect::Count;
        }
    }

} // namespace gui
} // namespace heroespath
