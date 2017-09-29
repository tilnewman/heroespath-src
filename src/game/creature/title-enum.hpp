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

    struct Titles
    {
        //Note:  Keep order in sync with game::creature::title::Warehouse::Fill()
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

        //returns the straight name with no spaces that matches the enum name
        static const std::string ToString(const Enum);

        //returns the human readable (GUI) version with spaces
        static const std::string Name(const Enum);

        //returns the value from the game data file
        static const std::string Desc(const Enum);

        //returns the filename and extension only, not a complete path
        static const std::string ImageFilename(const Enum);
    };

    using TitleEnumVec_t = std::vector<Titles::Enum>;

}
}
#endif //GAME_CREATURE_TITLEENUM_INCLUDED
