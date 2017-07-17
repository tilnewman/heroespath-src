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
// ownership-profile.cpp
//
#include "ownership-profile.hpp"

#include "game/non-player/character.hpp"
#include "game/item/item.hpp"
#include "game/game-data-file.hpp"

#include "misc/random.hpp"
#include "misc/handy-types.hpp"
#include "stringutil/stringhelp.hpp"

#include "boost/lexical_cast.hpp"

#include <exception>
#include <sstream>
#include <string>
#include <map>


namespace game
{
namespace non_player
{
namespace ownership
{

    const std::string wealth_type::ToString(const wealth_type::Enum E)
    {
        switch (E)
        {
            case Destitute  : { return "Destitute"; }
            case Poor       : { return "Poor"; }
            case LowerMiddle: { return "LowerMiddle"; }
            case UpperMiddle: { return "UpperMiddle"; }
            case Rich       : { return "Rich"; }
            case Lavish     : { return "Lavish"; }
            case Royal      : { return "Royal"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::non_player::ownership::wealth_type::ToString("
                    << E << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    wealth_type::Enum wealth_type::FromRankType(const creature::rank_class::Enum RANK_TYPE)
    {
        const std::string RANK_TYPE_STR(creature::rank_class::ToString(RANK_TYPE));

        using WealthTypeChanceMap_t = std::map<wealth_type::Enum, float>;
        WealthTypeChanceMap_t wealthChanceMap;

        wealth_type::Enum wealthTypeChanceRemaining(wealth_type::Count);

        for (std::size_t i(0); i < wealth_type::Count; ++i)
        {
            const wealth_type::Enum NEXT_WEALTH_TYPE( static_cast<wealth_type::Enum>(i) );
            const std::string NEXT_WEALTH_TYPE_NAME( wealth_type::ToString(NEXT_WEALTH_TYPE) );

            std::ostringstream ss;
            ss << "heroespath-wealthtype-chance-" << RANK_TYPE_STR << "-" 
                << NEXT_WEALTH_TYPE_NAME << "-one-in";
            
            const std::string NEXT_VALUE_STR( GameDataFile::Instance()->GetCopyStr(ss.str()) );

            if (NEXT_VALUE_STR == "remaining")
            {
                wealthTypeChanceRemaining = NEXT_WEALTH_TYPE;
            }

            float nextChanceValue(0.0f);
            try
            {
                nextChanceValue = boost::lexical_cast<float>(NEXT_VALUE_STR);
            }
            catch(...)
            {
                nextChanceValue = -1.0f;
            }

            wealthChanceMap[NEXT_WEALTH_TYPE] = ((nextChanceValue > 0.0f) ?
                (1.0f / nextChanceValue) : (0.0f));
        }

        float cumulative(0.0f);
        const float RAND(misc::random::Float());
        for (auto const & NEXT_TYPECHANCE_PAIR : wealthChanceMap)
        {
            if (NEXT_TYPECHANCE_PAIR.first != wealthTypeChanceRemaining)
            {
                cumulative += NEXT_TYPECHANCE_PAIR.second;

                if (RAND < cumulative)
                {
                    return NEXT_TYPECHANCE_PAIR.first;
                }
            }
        }

        return wealthTypeChanceRemaining;
    }


    wealth_type::Enum wealth_type::FromRank(const stats::Rank_t RANK_VALUE)
    {
        return FromRankType( creature::rank_class::FromRank(RANK_VALUE) );
    }


    wealth_type::Enum wealth_type::FromCreature(const CharacterPtr_t CHARACTER_PTR)
    {
        return FromRank(CHARACTER_PTR->Rank());
    }


    const std::string collector_type::ToString(const collector_type::Enum E)
    {
        std::ostringstream ss;

        if (E == collector_type::NoPreference)
        {
            ss << "No Preference";
        }
        else
        {
            if (E & Minimalist) ss << "Minimalist";
            if (E & Practical)  ss << ((ss.str().empty()) ? "" : "/") << "Practical";
            if (E & Collector)  ss << ((ss.str().empty()) ? "" : "/") << "Collector";
            if (E & Hoarder)    ss << ((ss.str().empty()) ? "" : "/") << "Hoarder";
        }

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::non_player::ownership::collector_type::ToString("
                << E << ")_InvalidValueError";

            throw std::range_error(ssErr.str());
        }

        return ss.str();
    }


    collector_type::Enum collector_type::FromCreature(const CharacterPtr_t CHARACTER_PTR)
    {        
        auto const CHANCE_BASE( GameDataFile::Instance()->GetCopyFloat(
            "heroespath-nonplayer-ownershipprofile-collectortype-chance-base") );

        //adjust for race
        auto const RACE_STR{ creature::race::ToString(CHARACTER_PTR->Race().Which()) };

        auto const RACE_KEY{
            "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-race-" +
            RACE_STR };

        auto const RACE_COLLECTOR_PARTS_STR{ GameDataFile::Instance()->GetCopyStr(RACE_KEY) };

        misc::StrVec_t racePartsVec;
        appbase::stringhelp::SplitByChar(RACE_COLLECTOR_PARTS_STR, racePartsVec, ',', true, true);

        M_ASSERT_OR_LOGANDTHROW_SS((racePartsVec.size() == 4),
            "game::non_player::ownership::collector_type::FromCreature("
            << CHARACTER_PTR->NameAndRaceAndRole() << ") failed to read four values from the key="
            << RACE_KEY);
        
        float chanceMinimalist(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[0]));
        float chancePractical(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[1]));
        float chanceCollector(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[2]));
        float chanceHoarder(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[3]));

        //adjust for roles
        auto const ROLE_STR{ creature::role::ToString(CHARACTER_PTR->Role().Which()) };

        auto const ROLE_KEY{
            "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-role-" +
            ROLE_STR };
            
        auto const ROLE_COLLECTOR_PARTS_STR{ GameDataFile::Instance()->GetCopyStr(ROLE_KEY) };

        misc::StrVec_t rolePartsVec;
        appbase::stringhelp::SplitByChar(ROLE_COLLECTOR_PARTS_STR, rolePartsVec, ',', true, true);

        M_ASSERT_OR_LOGANDTHROW_SS((rolePartsVec.size() == 4),
            "game::non_player::ownership::collector_type::FromCreature("
            << CHARACTER_PTR->NameAndRaceAndRole() << ") failed to read four values from the key="
            << ROLE_KEY);

        chanceMinimalist += ConvertStringToFloat(ROLE_KEY, rolePartsVec[0]);
        chancePractical += ConvertStringToFloat(ROLE_KEY, rolePartsVec[1]);
        chanceCollector += ConvertStringToFloat(ROLE_KEY, rolePartsVec[2]);
        chanceHoarder += ConvertStringToFloat(ROLE_KEY, rolePartsVec[3]);

        //enforce min
        {
            const float CHANCE_MIN(GameDataFile::Instance()->GetCopyFloat(
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-minimum"));

            if (chanceMinimalist < CHANCE_MIN)
            {
                chanceMinimalist = CHANCE_MIN;
            }

            if (chancePractical < CHANCE_MIN)
            {
                chancePractical = CHANCE_MIN;
            }

            if (chanceCollector < CHANCE_MIN)
            {
                chanceCollector = CHANCE_MIN;
            }

            if (chanceHoarder < CHANCE_MIN)
            {
                chanceHoarder = CHANCE_MIN;
            }
        }

        //enforce max
        {
            const float CHANCE_MAX(GameDataFile::Instance()->GetCopyFloat(
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-maximum"));

            if (chanceMinimalist > CHANCE_MAX)
            {
                chanceMinimalist = CHANCE_MAX;
            }

            if (chancePractical > CHANCE_MAX)
            {
                chancePractical = CHANCE_MAX;
            }

            if (chanceCollector > CHANCE_MAX)
            {
                chanceCollector = CHANCE_MAX;
            }

            if (chanceHoarder > CHANCE_MAX)
            {
                chanceHoarder = CHANCE_MAX;
            }
        }

        //determine
        collector_type::Enum collectorType(NoPreference);
        if (misc::random::Float() < chancePractical)
        {
            collectorType = static_cast<collector_type::Enum>(collectorType | Practical);
        }

        if (misc::random::Float() < chanceCollector)
        {
            collectorType = static_cast<collector_type::Enum>(collectorType | Collector);
        }

        if (misc::random::Float() < chanceMinimalist)
        {
            collectorType = static_cast<collector_type::Enum>(collectorType | Minimalist);
        }

        if ((misc::random::Float() < chanceHoarder) && (0 == (collectorType & Minimalist)))
        {
            collectorType = static_cast<collector_type::Enum>(collectorType | Hoarder);
        }

        return collectorType;
    }


    owns_magic_type::Enum owns_magic_type::FromCreature(const CharacterPtr_t CHARACTER_PTR)
    {
        float chanceRarely(0.0f);
        float chanceReligous(0.0f);
        float chanceMagical(0.0f);

        //adjust for race
        {
            auto const RACE_STR( creature::race::ToString(CHARACTER_PTR->Race().Which()) );

            auto const RACE_KEY{
                "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-race-" +
                RACE_STR };

            auto const RACE_OWNSMAGIC_PARTS_STR{ GameDataFile::Instance()->GetCopyStr(RACE_KEY) };

            misc::StrVec_t racePartsVec;
            appbase::stringhelp::SplitByChar(RACE_OWNSMAGIC_PARTS_STR, racePartsVec, ',', true, true);

            M_ASSERT_OR_LOGANDTHROW_SS((racePartsVec.size() == 3),
                "game::non_player::ownership::owns_magic_type::FromCreature("
                << CHARACTER_PTR->NameAndRaceAndRole()
                << ") failed to read three values from the key="
                << RACE_KEY);

            auto const RARELY_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[0]) };
            auto const RELIGIOUS_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[1]) };
            auto const MAGICAL_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[2]) };

            //Why do these values have to add up to one?
            //After the role adjustments below the values most definitely don't sum to 1.0...?
            //zTn -2017-7-14
            M_ASSERT_OR_LOGANDTHROW_SS(
                (misc::IsRealOne(RARELY_RACE_ADJ + RELIGIOUS_RACE_ADJ + MAGICAL_RACE_ADJ)),
                "game::non_player::ownership::owns_magic_type::FromCreature("
                << CHARACTER_PTR->NameAndRaceAndRole()
                << ") found key "<< RACE_KEY << "=" << RACE_OWNSMAGIC_PARTS_STR
                << " -but those values do not add up to one.");

            chanceRarely += RARELY_RACE_ADJ;
            chanceReligous += RELIGIOUS_RACE_ADJ;
            chanceMagical += MAGICAL_RACE_ADJ;
        }

        //adjust for role
        {
            auto const ROLE_STR{ creature::role::ToString(CHARACTER_PTR->Role().Which()) };

            std::ostringstream ss;
            ss << "heroespath-nonplayer-ownershipprofile-"
                << "ownsmagictype-chance-adjustment-Rarely-role-" << ROLE_STR;

            auto const ROLE_KEY{
                "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-adjustment-role-" +
                ROLE_STR };

            auto const ROLE_OWNSMAGIC_PARTS_STR{ GameDataFile::Instance()->GetCopyStr(ROLE_KEY) };

            misc::StrVec_t rolePartsVec;
            appbase::stringhelp::SplitByChar(ROLE_OWNSMAGIC_PARTS_STR, rolePartsVec, ',', true, true);

            M_ASSERT_OR_LOGANDTHROW_SS((rolePartsVec.size() == 3),
                "game::non_player::ownership::owns_magic_type::FromCreature("
                << CHARACTER_PTR->NameAndRaceAndRole()
                << ") failed to read three values from the key="
                << ROLE_KEY);

            chanceRarely += ConvertStringToFloat(ROLE_KEY, rolePartsVec[0]);
            chanceReligous += ConvertStringToFloat(ROLE_KEY, rolePartsVec[1]);
            chanceMagical += ConvertStringToFloat(ROLE_KEY, rolePartsVec[2]);
        }

        //enforce min
        {
            const float MIN(GameDataFile::Instance()->GetCopyFloat(
                "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-min"));

            if (chanceRarely < MIN)
            {
                chanceRarely = MIN;
            }

            if (chanceReligous < MIN)
            {
                chanceReligous = MIN;
            }

            if (chanceMagical < MIN)
            {
                chanceMagical = MIN;
            }
        }

        //enforce max
        {
            const float MAX(GameDataFile::Instance()->GetCopyFloat(
                "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-max"));

            if (chanceRarely > MAX)
            {
                chanceRarely = MAX;
            }

            if (chanceReligous > MAX)
            {
                chanceReligous = MAX;
            }

            if (chanceMagical > MAX)
            {
                chanceMagical = MAX;
            }
        }

        //determine
        auto const RAND(misc::random::Float(0.0f, (chanceRarely + chanceReligous + chanceRarely)));

        if (RAND < chanceMagical)
        {
            return Magical;
        }
        else
        {
            if (RAND < (chanceMagical + chanceReligous))
            {
                return Religous;
            }
            else
            {
                return Rarely;
            }
        }
    }


    const std::string complexity_type::ToString(const complexity_type::Enum E)
    {
        switch (E)
        {
            case Animal:    { return "Animal"; }
            case Simple:    { return "Simple"; }
            case Moderate:  { return "Moderate"; }
            case Complex:   { return "Complex"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::non_player::ownership::complexity_type::ToString("
                    << E << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    complexity_type::Enum complexity_type::FromString(const std::string & NAME)
    {
        if      (NAME == ToString(Animal))   return Animal;
        else if (NAME == ToString(Simple))   return Simple;
        else if (NAME == ToString(Moderate)) return Moderate;
        else if (NAME == ToString(Complex))  return Complex;
        else
            return Count;
    }


    complexity_type::Enum complexity_type::FromCreature(const CharacterPtr_t CHARACTER_PTR)
    {
        const complexity_type::Enum COMPLEXITY_BASED_ON_RACE(
            FromString(GameDataFile::Instance()->GetCopyStr(
                "heroespath-nonplayer-ownershipprofile-complexitytype-race-" +
                creature::race::ToString(CHARACTER_PTR->Race().Which()))) );

        if (COMPLEXITY_BASED_ON_RACE != complexity_type::Count)
        {
            return COMPLEXITY_BASED_ON_RACE;
        }
        else
        {
            return FromString( GameDataFile::Instance()->GetCopyStr(
                "heroespath-nonplayer-ownershipprofile-complexitytype-role-" +
                creature::role::ToString(CHARACTER_PTR->Role().Which())) );
        }
    }


    Profile::Profile(const wealth_type::Enum     WEALTH_TYPE,
                     const collector_type::Enum  COLLECTOR_TYPE,
                     const owns_magic_type::Enum MAGIC_OWN_TYPE,
                     const complexity_type::Enum COMPLEXITY_TYPE)
    :
        wealthType      (WEALTH_TYPE),
        collectorType   (COLLECTOR_TYPE),
        magicType       (MAGIC_OWN_TYPE),
        complexityType  (COMPLEXITY_TYPE)
    {}


    const Profile Profile::Make_FromCreature(const CharacterPtr_t CHARACTER_PTR)
    {
        return Profile(wealth_type::FromCreature(CHARACTER_PTR),
                       collector_type::FromCreature(CHARACTER_PTR),
                       owns_magic_type::FromCreature(CHARACTER_PTR),
                       complexity_type::FromCreature(CHARACTER_PTR));
    }


    float ConvertStringToFloat(const std::string & KEY, const std::string & STR_FLOAT)
    {
        try
        {
            return boost::lexical_cast<float>(STR_FLOAT);
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "game::non_player::ownership::ConvertStringToFloat(key=" << KEY
               << ", STR_FLOAT=" << STR_FLOAT << " unable to convert that str to a float.";

            throw std::runtime_error(ss.str());
        }
    }


}
}
}
