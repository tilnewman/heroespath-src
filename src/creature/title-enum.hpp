// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TITLEENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_TITLEENUM_HPP_INCLUDED
//
// title-enum.hpp
//  An enumeration defining each type of Title.
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct Titles : public EnumBaseCounting<>
    {
        // Note:  Keep order in sync with creature::title::Warehouse::Fill()
        enum Enum : EnumUnderlying_t
        {
            ProtectorOfThornberry = 0,
            //
            Dodger,
            FastFoot,
            NimbleFoot,
            SwiftFoot,
            MeleeMover,
            MeleeGrace,
            MeleeDancer,
            MeleeShadow,
            //
            SkyDodger,
            FastWing,
            NimbleWing,
            SwiftWing,
            SkyMover,
            GraceWing,
            SkyDancer,
            SkyShadow,
            //
            Sneak,
            Stalker,
            Creeper,
            Savage,
            Homicidal,
            Bloodthirsty,
            ShadowWalker,
            Assassin,
            //
            KnightOfTheClashingBlade,
            KnightOfAHundredBattles,
            KnightOfThePiercingBlade,
            KnightOfTheUndauntedBlade,
            KnightOfAThousandBattles,
            KnightOfTheFearedBlade,
            KnightOfTheRealm,
            KnightOfEtan,
            //
            Mender,
            Healer,
            BlessedHands,
            WitchDoctor,
            LightOfTheCure,
            DivineTouch,
            OrderOfTheWhiteSage,
            ClericOfTheCovenant,
            //
            HandsOfCharity,
            HandsOfLove,
            HandsOfDevotion,
            HandsOfNobility,
            HandsOfSacrifice,
            TheSelflessHand,
            TheIncorruptibleHand,
            OrderOfTheBleedingPalm,
            //
            HowlingStray,
            HowlingBully,
            HowlingWildling,
            SnarlingCreature,
            SnarlingBeast,
            SnarlingFiend,
            RagingMonster,
            RagingHorror,
            //
            Prowler,
            PickPocket,
            CatBurglar,
            Clincher,
            Picker,
            SafeCracker,
            LockTickler,
            LockBane,
            //
            Versifier,
            Player,
            Balladeer,
            Minstrel,
            Instrumentalist,
            Soloist,
            Artiste,
            Virtuoso,
            //
            BardOfTheTrippingToes,
            BardOfTheMerryMelody,
            BardOfTheWhimsicalWord,
            BardOfTheCarolingChorus,
            BardOfTheBouncingBallad,
            BardOfTheDancingDitty,
            BardOfTheEnchantedEar,
            BardOfTheAracneAria,
            //
            Targeter,
            SteadfastFlyer,
            DeadEye,
            DeadCenter,
            SureSail,
            SureShot,
            PerfectHit,
            EnchantedAim,
            //
            Magus,
            Charmer,
            Diviner,
            Enchanter,
            Conjurer,
            Shaman,
            Wizard,
            Warlock,
            //
            Undaunted,
            Daring,
            Bold,
            Brave,
            Valorous,
            Valiant,
            Lionhearted,
            Heroic,
            Fearless,
            //
            PawOfTheLunarPup,
            PawOfTheYoungHowler,
            PawOfTheNightSky,
            PawOfTheMidnightWail,
            PawOfTheTwilightMoon,
            PawOfTheCelestialCry,
            PawOfTheGlowingOrb,
            PawOfTheCrescentLegion,
            //
            WindGlider,
            LightFeather,
            FreeFlyer,
            SteadyWing,
            EverSoar,
            SkySail,
            Aerialist,
            FeatherDancer,
            //
            PackFollower,
            PackMember,
            PackTasker,
            PackCharger,
            PackNobel,
            PackLeader,
            PackAlpha,
            PackElder,
            //
            CritterClairvoyant,
            AnimalAnimator,
            FriendOfTheFeral,
            WillOfTheWild,
            CreatureChanneler,
            BeastTaskmaster,
            MonsterManipulator,
            MammalianMaster,
            //
            Brawler,
            WildSwing,
            HavocSmasher,
            SureStroke,
            Warrior,
            EverMark,
            MeleeMaster,
            SteadyStriker,
            OrderOfTheEverWarrior,
            //
            DragonOfTheUnblinkingEye,
            DragonOfTheCurlingClaw,
            DragonOfTheDaggerTooth,
            DragonOfTheForebodingGaze,
            DragonOfTheUnbreakableScale,
            DragonOfTheDreadedBreath,
            DragonOfTheArcaneWing,
            DragonOfTheNightmareSky,
            //
            Count
        };

        static constexpr std::string_view Name(const Titles::Enum ENUM)
        {
            switch (ENUM)
            {
                case ProtectorOfThornberry: return "Protector Of Thornberry";
                //
                case Dodger: return "Dodger";
                case FastFoot: return "Fast Foot";
                case NimbleFoot: return "Nimble Foot";
                case SwiftFoot: return "Swift Foot";
                case MeleeMover: return "Melee Mover";
                case MeleeGrace: return "Melee Grace";
                case MeleeDancer: return "Melee Dancer";
                case MeleeShadow: return "Melee Shadow";
                //
                case SkyDodger: return "Sky Dodger";
                case FastWing: return "Fast Wing";
                case NimbleWing: return "Nimble Wing";
                case SwiftWing: return "Swift Wing";
                case SkyMover: return "Sky Mover";
                case GraceWing: return "Grace Wing";
                case SkyDancer: return "Sky Dancer";
                case SkyShadow: return "Sky Shadow";
                //
                case Sneak: return "Sneak";
                case Stalker: return "Stalker";
                case Creeper: return "Creeper";
                case Savage: return "Savage";
                case Homicidal: return "Homicidal";
                case Bloodthirsty: return "Bloodthirsty";
                case ShadowWalker: return "Shadow Walker";
                case Assassin: return "Assassin";
                //
                case KnightOfTheClashingBlade: return "Knight of the Clashing Blade";
                case KnightOfAHundredBattles: return "Knight of a Hundred Battles";
                case KnightOfThePiercingBlade: return "Knight of the Piercing Blade";
                case KnightOfTheUndauntedBlade: return "Knight of the Undaunted Blade";
                case KnightOfAThousandBattles: return "Knight of a Thousand Battles";
                case KnightOfTheFearedBlade: return "Knight of the Feared Blade";
                case KnightOfTheRealm: return "Knight of the Realm";
                case KnightOfEtan: return "Knight of Etan";
                //
                case Mender: return "Mender";
                case Healer: return "Healer";
                case BlessedHands: return "Blessed Hands";
                case WitchDoctor: return "Witch Doctor";
                case LightOfTheCure: return "Light of the Cure";
                case DivineTouch: return "Divine Touch";
                case OrderOfTheWhiteSage: return "Order of the White Sage";
                case ClericOfTheCovenant: return "Cleric of the Covenant";
                //
                case HandsOfCharity: return "Hands of Charity";
                case HandsOfLove: return "Hands of Love";
                case HandsOfDevotion: return "Hands of Devotion";
                case HandsOfNobility: return "Hands of Nobility";
                case HandsOfSacrifice: return "Hands of Sacrifice";
                case TheSelflessHand: return "The Selfless Hand";
                case TheIncorruptibleHand: return "The Incorruptible Hand";
                case OrderOfTheBleedingPalm: return "Order of the Bleeding Palm";
                //
                case HowlingStray: return "Howling Stray";
                case HowlingBully: return "Howling Bully";
                case HowlingWildling: return "Howling Wildling";
                case SnarlingCreature: return "Snarling Creature";
                case SnarlingBeast: return "Snarling Beast";
                case SnarlingFiend: return "Snarling Fiend";
                case RagingMonster: return "Raging Monster";
                case RagingHorror: return "Raging Horror";
                //
                case Prowler: return "Prowler";
                case PickPocket: return "Pick Pocket";
                case CatBurglar: return "Cat Burglar";
                case Clincher: return "Clincher";
                case Picker: return "Picker";
                case SafeCracker: return "Safe Cracker";
                case LockTickler: return "Lock Tickler";
                case LockBane: return "Lock Bane";
                //
                case Versifier: return "Versifier";
                case Player: return "Player";
                case Balladeer: return "Balladeer";
                case Minstrel: return "Minstrel";
                case Instrumentalist: return "Instrumentalist";
                case Soloist: return "Soloist";
                case Artiste: return "Artiste";
                case Virtuoso: return "Virtuoso";
                //
                case BardOfTheTrippingToes: return "Bard of the Tripping Toes";
                case BardOfTheMerryMelody: return "Bard of the Merry Melody";
                case BardOfTheWhimsicalWord: return "Bard of the Whimsical Word";
                case BardOfTheCarolingChorus: return "Bard of the Caroling Chorus";
                case BardOfTheBouncingBallad: return "Bard of the Bouncing Ballad";
                case BardOfTheDancingDitty: return "Bard of the Dancing Ditty";
                case BardOfTheEnchantedEar: return "Bard of the Enchanted Ear";
                case BardOfTheAracneAria: return "Bard of the Arcane Aria";
                //
                case Targeter: return "Targeter";
                case SteadfastFlyer: return "Steadfast Flyer";
                case DeadEye: return "Dead-Eye";
                case DeadCenter: return "Dead Center";
                case SureSail: return "Sure-Sail";
                case SureShot: return "Sure-Shot";
                case PerfectHit: return "Perfect Hit";
                case EnchantedAim: return "Enchanted Aim";
                //
                case Magus: return "Magus";
                case Charmer: return "Charmer";
                case Diviner: return "Diviner";
                case Enchanter: return "Enchanter";
                case Conjurer: return "Conjurer";
                case Shaman: return "Shaman";
                case Wizard: return "Wizard";
                case Warlock: return "Warlock";
                //
                case Undaunted: return "Undaunted";
                case Daring: return "Daring";
                case Bold: return "Bold";
                case Brave: return "Brave";
                case Valorous: return "Valorous";
                case Valiant: return "Valiant";
                case Lionhearted: return "Lionhearted";
                case Heroic: return "Heroic";
                case Fearless: return "Fearless";
                //
                case PawOfTheLunarPup: return "Paw of the Lunar Pup";
                case PawOfTheYoungHowler: return "Paw of the Young Howler";
                case PawOfTheNightSky: return "Paw of the Night Sky";
                case PawOfTheMidnightWail: return "Paw of the Midnight Wail";
                case PawOfTheTwilightMoon: return "Paw of the Twilight Moon";
                case PawOfTheCelestialCry: return "Paw of the Celestial Cry";
                case PawOfTheGlowingOrb: return "Paw of the Glowing Orb";
                case PawOfTheCrescentLegion: return "Paw of the Crescent Legion";
                //
                case WindGlider: return "Wind Glider";
                case LightFeather: return "Light Feather";
                case FreeFlyer: return "Free Flyer";
                case SteadyWing: return "Steady Wing";
                case EverSoar: return "Ever Soar";
                case SkySail: return "Sky Sail";
                case Aerialist: return "Aerialist";
                case FeatherDancer: return "Feather Dancer";
                //
                case PackFollower: return "Pack Follower";
                case PackMember: return "Pack Member";
                case PackTasker: return "Pack Tasker";
                case PackCharger: return "Pack Charger";
                case PackNobel: return "Pack Nobel";
                case PackLeader: return "Pack Leader";
                case PackAlpha: return "Pack Alpha";
                case PackElder: return "Pack Elder";
                //
                case CritterClairvoyant: return "Critter Clairvoyant";
                case AnimalAnimator: return "Animal Animator";
                case FriendOfTheFeral: return "Friend of the Feral";
                case WillOfTheWild: return "Will of the Wild";
                case CreatureChanneler: return "Creature Channeler";
                case BeastTaskmaster: return "Beast Taskmaster";
                case MonsterManipulator: return "Monster Manipulator";
                case MammalianMaster: return "Mammalian Master";
                //
                case Brawler: return "Brawler";
                case WildSwing: return "Wild Swing";
                case HavocSmasher: return "Havoc Smasher";
                case SureStroke: return "Sure Stroke";
                case Warrior: return "Warrior";
                case EverMark: return "Ever Mark";
                case MeleeMaster: return "Melee Master";
                case SteadyStriker: return "Steady Striker";
                case OrderOfTheEverWarrior: return "Order of the Ever Warrior";
                //
                case DragonOfTheUnblinkingEye: return "Dragon of the Unblinking Eye";
                case DragonOfTheCurlingClaw: return "Dragon of the Curling Claw";
                case DragonOfTheDaggerTooth: return "Dragon of the Dagger Tooth";
                case DragonOfTheForebodingGaze: return "Dragon of the Foreboding Gaze";
                case DragonOfTheUnbreakableScale: return "Dragon of the Unbreakable Scale";
                case DragonOfTheDreadedBreath: return "Dragon of the Dreaded Breath";
                case DragonOfTheArcaneWing: return "Dragon of the Arcane Wing";
                case DragonOfTheNightmareSky: return "Dragon of the Nightmare Sky";
                //
                case Count: return "Count";
                default: return "creature::Titles::Name(ENUM=out_of_range)";
            }
        }

        static const std::string Desc(const Enum);
        static const std::string ImageFilename(const Enum);
        static const std::string ImageDirectory();
        static const std::string ImagePath(const Enum);
    };

    using TitleEnumVec_t = std::vector<Titles::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TITLEENUM_HPP_INCLUDED
