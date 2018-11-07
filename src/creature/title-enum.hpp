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
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct Titles : public misc::EnumBaseCounting<Titles, misc::EnumFirstValue::Valid>
    {
        // Note:  Keep order in sync with creature::title::Warehouse::Fill()
        enum Enum : misc::EnumUnderlying_t
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

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static const std::string Desc(const Enum);
        static const std::string ImageFilename(const Enum);
        static const std::string ImageDirectory();
        static const std::string ImagePath(const Enum);
    };

    using TitleEnumVec_t = std::vector<Titles::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TITLEENUM_HPP_INCLUDED
