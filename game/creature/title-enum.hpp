#ifndef GAME_CREATURE_TITLEENUM_INCLUDED
#define GAME_CREATURE_TITLEENUM_INCLUDED
//
// title-enum.hpp
//  An enumeration defining each type of Title.
//
#include <string>
#include <vector>


namespace game
{
namespace creature
{

    namespace Titles
    {
        enum Enum
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
            OrderOfTheSteadyBlade,
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

        const std::string ToString(const Enum);//straight name with no spaces that matches the enum
        const std::string Name(const Enum);//human readable
        const std::string Desc(const Enum);//pulled from the game data file
    }

    using TitleEnumVec_t = std::vector<Titles::Enum>;

}
}
#endif //GAME_CREATURE_TITLEENUM_INCLUDED
