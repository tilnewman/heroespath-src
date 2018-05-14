// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// title-enum.cpp
//
#include "title-enum.hpp"

#include "creature/title-holder.hpp"
#include "creature/title.hpp"
#include "game/game-data-file.hpp"
#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace creature
{

    const std::string Titles::ToString(const Titles::Enum E)
    {
        switch (E)
        {
            case ProtectorOfThornberry:
            {
                return "ProtectorOfThornberry";
            }
            //
            case Dodger:
            {
                return "Dodger";
            }
            case FastFoot:
            {
                return "FastFoot";
            }
            case NimbleFoot:
            {
                return "NimbleFoot";
            }
            case SwiftFoot:
            {
                return "SwiftFoot";
            }
            case MeleeMover:
            {
                return "MeleeMover";
            }
            case MeleeGrace:
            {
                return "MeleeGrace";
            }
            case MeleeDancer:
            {
                return "MeleeDancer";
            }
            case MeleeShadow:
            {
                return "MeleeShadow";
            }
            //
            case SkyDodger:
            {
                return "SkyDodger";
            }
            case FastWing:
            {
                return "FastWing";
            }
            case NimbleWing:
            {
                return "NimbleWing";
            }
            case SwiftWing:
            {
                return "SwiftWing";
            }
            case SkyMover:
            {
                return "SkyMover";
            }
            case GraceWing:
            {
                return "GraceWing";
            }
            case SkyDancer:
            {
                return "SkyDancer";
            }
            case SkyShadow:
            {
                return "SkyShadow";
            }
            //
            case Sneak:
            {
                return "Sneak";
            }
            case Stalker:
            {
                return "Stalker";
            }
            case Creeper:
            {
                return "Creeper";
            }
            case Savage:
            {
                return "Savage";
            }
            case Homicidal:
            {
                return "Homicidal";
            }
            case Bloodthirsty:
            {
                return "Bloodthirsty";
            }
            case ShadowWalker:
            {
                return "ShadowWalker";
            }
            case Assassin:
            {
                return "Assassin";
            }
            //
            case KnightOfTheClashingBlade:
            {
                return "KnightOfTheClashingBlade";
            }
            case KnightOfAHundredBattles:
            {
                return "KnightOfAHundredBattles";
            }
            case KnightOfThePiercingBlade:
            {
                return "KnightOfThePiercingBlade";
            }
            case KnightOfTheUndauntedBlade:
            {
                return "KnightOfTheUndauntedBlade";
            }
            case KnightOfAThousandBattles:
            {
                return "KnightOfAThousandBattles";
            }
            case KnightOfTheFearedBlade:
            {
                return "KnightOfTheFearedBlade";
            }
            case KnightOfTheRealm:
            {
                return "KnightOfTheRealm";
            }
            case KnightOfEtan:
            {
                return "KnightOfEtan";
            }
            //
            case Mender:
            {
                return "Mender";
            }
            case Healer:
            {
                return "Healer";
            }
            case BlessedHands:
            {
                return "BlessedHands";
            }
            case WitchDoctor:
            {
                return "WitchDoctor";
            }
            case LightOfTheCure:
            {
                return "LightOfTheCure";
            }
            case DivineTouch:
            {
                return "DivineTouch";
            }
            case OrderOfTheWhiteSage:
            {
                return "OrderOfTheWhiteSage";
            }
            case ClericOfTheCovenant:
            {
                return "ClericOfTheCovenant";
            }
            //
            case HandsOfCharity:
            {
                return "HandsOfCharity";
            }
            case HandsOfLove:
            {
                return "HandsOfLove";
            }
            case HandsOfDevotion:
            {
                return "HandsOfDevotion";
            }
            case HandsOfNobility:
            {
                return "HandsOfNobility";
            }
            case HandsOfSacrifice:
            {
                return "HandsOfSacrifice";
            }
            case TheSelflessHand:
            {
                return "TheSelflessHand";
            }
            case TheIncorruptibleHand:
            {
                return "TheIncorruptibleHand";
            }
            case OrderOfTheBleedingPalm:
            {
                return "OrderOfTheBleedingPalm";
            }
            //
            case HowlingStray:
            {
                return "HowlingStray";
            }
            case HowlingBully:
            {
                return "HowlingBully";
            }
            case HowlingWildling:
            {
                return "HowlingWildling";
            }
            case SnarlingCreature:
            {
                return "SnarlingCreature";
            }
            case SnarlingBeast:
            {
                return "SnarlingBeast";
            }
            case SnarlingFiend:
            {
                return "SnarlingFiend";
            }
            case RagingMonster:
            {
                return "RagingMonster";
            }
            case RagingHorror:
            {
                return "RagingHorror";
            }
            //
            case Prowler:
            {
                return "Prowler";
            }
            case PickPocket:
            {
                return "PickPocket";
            }
            case CatBurglar:
            {
                return "CatBurglar";
            }
            case Clincher:
            {
                return "Clincher";
            }
            case Picker:
            {
                return "Picker";
            }
            case SafeCracker:
            {
                return "SafeCracker";
            }
            case LockTickler:
            {
                return "LockTickler";
            }
            case LockBane:
            {
                return "LockBane";
            }
            //
            case Versifier:
            {
                return "Versifier";
            }
            case Player:
            {
                return "Player";
            }
            case Balladeer:
            {
                return "Balladeer";
            }
            case Minstrel:
            {
                return "Minstrel";
            }
            case Instrumentalist:
            {
                return "Instrumentalist";
            }
            case Soloist:
            {
                return "Soloist";
            }
            case Artiste:
            {
                return "Artiste";
            }
            case Virtuoso:
            {
                return "Virtuoso";
            }
            //
            case BardOfTheTrippingToes:
            {
                return "BardOfTheTrippingToes";
            }
            case BardOfTheMerryMelody:
            {
                return "BardOfTheMerryMelody";
            }
            case BardOfTheWhimsicalWord:
            {
                return "BardOfTheWhimsicalWord";
            }
            case BardOfTheCarolingChorus:
            {
                return "BardOfTheCarolingChorus";
            }
            case BardOfTheBouncingBallad:
            {
                return "BardOfTheBouncingBallad";
            }
            case BardOfTheDancingDitty:
            {
                return "BardOfTheDancingDitty";
            }
            case BardOfTheEnchantedEar:
            {
                return "BardOfTheEnchantedEar";
            }
            case BardOfTheAracneAria:
            {
                return "BardOfTheAracneAria";
            }
            //
            case Targeter:
            {
                return "Targeter";
            }
            case SteadfastFlyer:
            {
                return "SteadfastFlyer";
            }
            case DeadEye:
            {
                return "DeadEye";
            }
            case DeadCenter:
            {
                return "DeadCenter";
            }
            case SureSail:
            {
                return "SureSail";
            }
            case SureShot:
            {
                return "SureShot";
            }
            case PerfectHit:
            {
                return "PerfectHit";
            }
            case EnchantedAim:
            {
                return "EnchantedAim";
            }
            //
            case Magus:
            {
                return "Magus";
            }
            case Charmer:
            {
                return "Charmer";
            }
            case Diviner:
            {
                return "Diviner";
            }
            case Enchanter:
            {
                return "Enchanter";
            }
            case Conjurer:
            {
                return "Conjurer";
            }
            case Shaman:
            {
                return "Shaman";
            }
            case Wizard:
            {
                return "Wizard";
            }
            case Warlock:
            {
                return "Warlock";
            }
            //
            case Undaunted:
            {
                return "Undaunted";
            }
            case Daring:
            {
                return "Daring";
            }
            case Bold:
            {
                return "Bold";
            }
            case Brave:
            {
                return "Brave";
            }
            case Valorous:
            {
                return "Valorous";
            }
            case Valiant:
            {
                return "Valiant";
            }
            case Lionhearted:
            {
                return "Lionhearted";
            }
            case Heroic:
            {
                return "Heroic";
            }
            case Fearless:
            {
                return "Fearless";
            }
            //
            case PawOfTheLunarPup:
            {
                return "PawOfTheLunarPup";
            }
            case PawOfTheYoungHowler:
            {
                return "PawOfTheYoungHowler";
            }
            case PawOfTheNightSky:
            {
                return "PawOfTheNightSky";
            }
            case PawOfTheMidnightWail:
            {
                return "PawOfTheMidnightWail";
            }
            case PawOfTheTwilightMoon:
            {
                return "PawOfTheTwilightMoon";
            }
            case PawOfTheCelestialCry:
            {
                return "PawOfTheCelestialCry";
            }
            case PawOfTheGlowingOrb:
            {
                return "PawOfTheGlowingOrb";
            }
            case PawOfTheCrescentLegion:
            {
                return "PawOfTheCrescentLegion";
            }
            //
            case WindGlider:
            {
                return "WindGlider";
            }
            case LightFeather:
            {
                return "LightFeather";
            }
            case FreeFlyer:
            {
                return "FreeFlyer";
            }
            case SteadyWing:
            {
                return "SteadyWing";
            }
            case EverSoar:
            {
                return "EverSoar";
            }
            case SkySail:
            {
                return "SkySail";
            }
            case Aerialist:
            {
                return "Aerialist";
            }
            case FeatherDancer:
            {
                return "FeatherDancer";
            }
            //
            case PackFollower:
            {
                return "PackFollower";
            }
            case PackMember:
            {
                return "PackMember";
            }
            case PackTasker:
            {
                return "PackTasker";
            }
            case PackCharger:
            {
                return "PackCharger";
            }
            case PackNobel:
            {
                return "PackNobel";
            }
            case PackLeader:
            {
                return "PackLeader";
            }
            case PackAlpha:
            {
                return "PackAlpha";
            }
            case PackElder:
            {
                return "PackElder";
            }
            //
            case CritterClairvoyant:
            {
                return "CritterClairvoyant";
            }
            case AnimalAnimator:
            {
                return "AnimalAnimator";
            }
            case FriendOfTheFeral:
            {
                return "FriendOfTheFeral";
            }
            case WillOfTheWild:
            {
                return "WillOfTheWild";
            }
            case CreatureChanneler:
            {
                return "CreatureChanneler";
            }
            case BeastTaskmaster:
            {
                return "BeastTaskmaster";
            }
            case MonsterManipulator:
            {
                return "MonsterManipulator";
            }
            case MammalianMaster:
            {
                return "MammalianMaster";
            }
            //
            case Brawler:
            {
                return "Brawler";
            }
            case WildSwing:
            {
                return "WildSwing";
            }
            case HavocSmasher:
            {
                return "HavocSmasher";
            }
            case SureStroke:
            {
                return "SureStroke";
            }
            case Warrior:
            {
                return "Warrior";
            }
            case EverMark:
            {
                return "EverMark";
            }
            case MeleeMaster:
            {
                return "MeleeMaster";
            }
            case SteadyStriker:
            {
                return "SteadyStriker";
            }
            case OrderOfTheEverWarrior:
            {
                return "OrderOfTheEverWarrior";
            }
            //
            case DragonOfTheUnblinkingEye:
            {
                return "DragonOfTheUnblinkingEye";
            }
            case DragonOfTheCurlingClaw:
            {
                return "DragonOfTheCurlingClaw";
            }
            case DragonOfTheDaggerTooth:
            {
                return "DragonOfTheDaggerTooth";
            }
            case DragonOfTheForebodingGaze:
            {
                return "DragonOfTheForebodingGaze";
            }
            case DragonOfTheUnbreakableScale:
            {
                return "DragonOfTheUnbreakableScale";
            }
            case DragonOfTheDreadedBreath:
            {
                return "DragonOfTheDreadedBreath";
            }
            case DragonOfTheArcaneWing:
            {
                return "DragonOfTheArcaneWing";
            }
            case DragonOfTheNightmareSky:
            {
                return "DragonOfTheNightmareSky";
            }
            //
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string Titles::Name(const Titles::Enum E)
    {
        switch (E)
        {
            case ProtectorOfThornberry:
            {
                return "Protector Of Thornberry";
            }
            //
            case Dodger:
            {
                return "Dodger";
            }
            case FastFoot:
            {
                return "Fast Foot";
            }
            case NimbleFoot:
            {
                return "Nimble Foot";
            }
            case SwiftFoot:
            {
                return "Swift Foot";
            }
            case MeleeMover:
            {
                return "Melee Mover";
            }
            case MeleeGrace:
            {
                return "Melee Grace";
            }
            case MeleeDancer:
            {
                return "Melee Dancer";
            }
            case MeleeShadow:
            {
                return "Melee Shadow";
            }
            //
            case SkyDodger:
            {
                return "Sky Dodger";
            }
            case FastWing:
            {
                return "Fast Wing";
            }
            case NimbleWing:
            {
                return "Nimble Wing";
            }
            case SwiftWing:
            {
                return "Swift Wing";
            }
            case SkyMover:
            {
                return "Sky Mover";
            }
            case GraceWing:
            {
                return "Grace Wing";
            }
            case SkyDancer:
            {
                return "Sky Dancer";
            }
            case SkyShadow:
            {
                return "Sky Shadow";
            }
            //
            case Sneak:
            {
                return "Sneak";
            }
            case Stalker:
            {
                return "Stalker";
            }
            case Creeper:
            {
                return "Creeper";
            }
            case Savage:
            {
                return "Savage";
            }
            case Homicidal:
            {
                return "Homicidal";
            }
            case Bloodthirsty:
            {
                return "Bloodthirsty";
            }
            case ShadowWalker:
            {
                return "Shadow Walker";
            }
            case Assassin:
            {
                return "Assassin";
            }
            //
            case KnightOfTheClashingBlade:
            {
                return "Knight of the Clashing Blade";
            }
            case KnightOfAHundredBattles:
            {
                return "Knight of a Hundred Battles";
            }
            case KnightOfThePiercingBlade:
            {
                return "Knight of the Piercing Blade";
            }
            case KnightOfTheUndauntedBlade:
            {
                return "Knight of the Undaunted Blade";
            }
            case KnightOfAThousandBattles:
            {
                return "Knight of a Thousand Battles";
            }
            case KnightOfTheFearedBlade:
            {
                return "Knight of the Feared Blade";
            }
            case KnightOfTheRealm:
            {
                return "Knight of the Realm";
            }
            case KnightOfEtan:
            {
                return "Knight of Etan";
            }
            //
            case Mender:
            {
                return "Mender";
            }
            case Healer:
            {
                return "Healer";
            }
            case BlessedHands:
            {
                return "Blessed Hands";
            }
            case WitchDoctor:
            {
                return "Witch Doctor";
            }
            case LightOfTheCure:
            {
                return "Light of the Cure";
            }
            case DivineTouch:
            {
                return "Divine Touch";
            }
            case OrderOfTheWhiteSage:
            {
                return "Order of the White Sage";
            }
            case ClericOfTheCovenant:
            {
                return "Cleric of the Covenant";
            }
            //
            case HandsOfCharity:
            {
                return "Hands of Charity";
            }
            case HandsOfLove:
            {
                return "Hands of Love";
            }
            case HandsOfDevotion:
            {
                return "Hands of Devotion";
            }
            case HandsOfNobility:
            {
                return "Hands of Nobility";
            }
            case HandsOfSacrifice:
            {
                return "Hands of Sacrifice";
            }
            case TheSelflessHand:
            {
                return "The Selfless Hand";
            }
            case TheIncorruptibleHand:
            {
                return "The Incorruptible Hand";
            }
            case OrderOfTheBleedingPalm:
            {
                return "Order of the Bleeding Palm";
            }
            //
            case HowlingStray:
            {
                return "Howling Stray";
            }
            case HowlingBully:
            {
                return "Howling Bully";
            }
            case HowlingWildling:
            {
                return "Howling Wildling";
            }
            case SnarlingCreature:
            {
                return "Snarling Creature";
            }
            case SnarlingBeast:
            {
                return "Snarling Beast";
            }
            case SnarlingFiend:
            {
                return "Snarling Fiend";
            }
            case RagingMonster:
            {
                return "Raging Monster";
            }
            case RagingHorror:
            {
                return "Raging Horror";
            }
            //
            case Prowler:
            {
                return "Prowler";
            }
            case PickPocket:
            {
                return "Pick Pocket";
            }
            case CatBurglar:
            {
                return "Cat Burglar";
            }
            case Clincher:
            {
                return "Clincher";
            }
            case Picker:
            {
                return "Picker";
            }
            case SafeCracker:
            {
                return "Safe Cracker";
            }
            case LockTickler:
            {
                return "Lock Tickler";
            }
            case LockBane:
            {
                return "Lock Bane";
            }
            //
            case Versifier:
            {
                return "Versifier";
            }
            case Player:
            {
                return "Player";
            }
            case Balladeer:
            {
                return "Balladeer";
            }
            case Minstrel:
            {
                return "Minstrel";
            }
            case Instrumentalist:
            {
                return "Instrumentalist";
            }
            case Soloist:
            {
                return "Soloist";
            }
            case Artiste:
            {
                return "Artiste";
            }
            case Virtuoso:
            {
                return "Virtuoso";
            }
            //
            case BardOfTheTrippingToes:
            {
                return "Bard of the Tripping Toes";
            }
            case BardOfTheMerryMelody:
            {
                return "Bard of the Merry Melody";
            }
            case BardOfTheWhimsicalWord:
            {
                return "Bard of the Whimsical Word";
            }
            case BardOfTheCarolingChorus:
            {
                return "Bard of the Caroling Chorus";
            }
            case BardOfTheBouncingBallad:
            {
                return "Bard of the Bouncing Ballad";
            }
            case BardOfTheDancingDitty:
            {
                return "Bard of the Dancing Ditty";
            }
            case BardOfTheEnchantedEar:
            {
                return "Bard of the Enchanted Ear";
            }
            case BardOfTheAracneAria:
            {
                return "Bard of the Aracne Aria";
            }
            //
            case Targeter:
            {
                return "Targeter";
            }
            case SteadfastFlyer:
            {
                return "Steadfast Flyer";
            }
            case DeadEye:
            {
                return "Dead-Eye";
            }
            case DeadCenter:
            {
                return "Dead Center";
            }
            case SureSail:
            {
                return "Sure-Sail";
            }
            case SureShot:
            {
                return "Sure-Shot";
            }
            case PerfectHit:
            {
                return "Perfect Hit";
            }
            case EnchantedAim:
            {
                return "Enchanted Aim";
            }
            //
            case Magus:
            {
                return "Magus";
            }
            case Charmer:
            {
                return "Charmer";
            }
            case Diviner:
            {
                return "Diviner";
            }
            case Enchanter:
            {
                return "Enchanter";
            }
            case Conjurer:
            {
                return "Conjurer";
            }
            case Shaman:
            {
                return "Shaman";
            }
            case Wizard:
            {
                return "Wizard";
            }
            case Warlock:
            {
                return "Warlock";
            }
            //
            case Undaunted:
            {
                return "Undaunted";
            }
            case Daring:
            {
                return "Daring";
            }
            case Bold:
            {
                return "Bold";
            }
            case Brave:
            {
                return "Brave";
            }
            case Valorous:
            {
                return "Valorous";
            }
            case Valiant:
            {
                return "Valiant";
            }
            case Lionhearted:
            {
                return "Lionhearted";
            }
            case Heroic:
            {
                return "Heroic";
            }
            case Fearless:
            {
                return "Fearless";
            }
            //
            case PawOfTheLunarPup:
            {
                return "Paw of the Lunar Pup";
            }
            case PawOfTheYoungHowler:
            {
                return "Paw of the Young Howler";
            }
            case PawOfTheNightSky:
            {
                return "Paw of the Night Sky";
            }
            case PawOfTheMidnightWail:
            {
                return "Paw of the Midnight Wail";
            }
            case PawOfTheTwilightMoon:
            {
                return "Paw of the Twilight Moon";
            }
            case PawOfTheCelestialCry:
            {
                return "Paw of the Celestial Cry";
            }
            case PawOfTheGlowingOrb:
            {
                return "Paw of the Glowing Orb";
            }
            case PawOfTheCrescentLegion:
            {
                return "Paw of the Crescent Legion";
            }
            //
            case WindGlider:
            {
                return "Wind Glider";
            }
            case LightFeather:
            {
                return "Light Feather";
            }
            case FreeFlyer:
            {
                return "Free Flyer";
            }
            case SteadyWing:
            {
                return "Steady Wing";
            }
            case EverSoar:
            {
                return "Ever Soar";
            }
            case SkySail:
            {
                return "Sky Sail";
            }
            case Aerialist:
            {
                return "Aerialist";
            }
            case FeatherDancer:
            {
                return "Feather Dancer";
            }
            //
            case PackFollower:
            {
                return "Pack Follower";
            }
            case PackMember:
            {
                return "Pack Member";
            }
            case PackTasker:
            {
                return "Pack Tasker";
            }
            case PackCharger:
            {
                return "Pack Charger";
            }
            case PackNobel:
            {
                return "Pack Nobel";
            }
            case PackLeader:
            {
                return "Pack Leader";
            }
            case PackAlpha:
            {
                return "Pack Alpha";
            }
            case PackElder:
            {
                return "Pack Elder";
            }
            //
            case CritterClairvoyant:
            {
                return "Critter Clairvoyant";
            }
            case AnimalAnimator:
            {
                return "Animal Animator";
            }
            case FriendOfTheFeral:
            {
                return "Friend of the Feral";
            }
            case WillOfTheWild:
            {
                return "Will of the Wild";
            }
            case CreatureChanneler:
            {
                return "Creature Channeler";
            }
            case BeastTaskmaster:
            {
                return "Beast Taskmaster";
            }
            case MonsterManipulator:
            {
                return "Monster Manipulator";
            }
            case MammalianMaster:
            {
                return "Mammalian Master";
            }
            //
            case Brawler:
            {
                return "Brawler";
            }
            case WildSwing:
            {
                return "Wild Swing";
            }
            case HavocSmasher:
            {
                return "Havoc Smasher";
            }
            case SureStroke:
            {
                return "Sure Stroke";
            }
            case Warrior:
            {
                return "Warrior";
            }
            case EverMark:
            {
                return "Ever Mark";
            }
            case MeleeMaster:
            {
                return "Melee Master";
            }
            case SteadyStriker:
            {
                return "Steady Striker";
            }
            case OrderOfTheEverWarrior:
            {
                return "Order of the Ever Warrior";
            }
            //
            case DragonOfTheUnblinkingEye:
            {
                return "Dragon of the Unblinking Eye";
            }
            case DragonOfTheCurlingClaw:
            {
                return "Dragon of the Curling Claw";
            }
            case DragonOfTheDaggerTooth:
            {
                return "Dragon of the Dagger Tooth";
            }
            case DragonOfTheForebodingGaze:
            {
                return "Dragon of the Foreboding Gaze";
            }
            case DragonOfTheUnbreakableScale:
            {
                return "Dragon of the Unbreakable Scale";
            }
            case DragonOfTheDreadedBreath:
            {
                return "Dragon of the Dreaded Breath";
            }
            case DragonOfTheArcaneWing:
            {
                return "Dragon of the Arcane Wing";
            }
            case DragonOfTheNightmareSky:
            {
                return "Dragon of the Nightmare Sky";
            }
            //
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "Name");
            }
        }
    }

    const std::string Titles::Desc(const Titles::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-creature-title-" << ToString(E) << "-desc";
        return boost::algorithm::trim_copy(game::GameDataFile::Instance()->GetCopyStr(keySS.str()));
    }

    const std::string Titles::ImageFilename(const Titles::Enum E)
    {
        return title::Holder::Get(E)->ImageFilename();
    }

} // namespace creature
} // namespace heroespath
