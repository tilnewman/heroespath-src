// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// ownership-profile.cpp
//
#include "ownership-profile.hpp"

#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "item/item.hpp"
#include "misc/handy-types.hpp"
#include "misc/random.hpp"
#include "stringutil/stringhelp.hpp"

#include "boost/lexical_cast.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace non_player
{
    namespace ownership
    {

        const std::string wealth_type::ToString(const wealth_type::Enum WEALTH_TYPE)
        {
            switch (WEALTH_TYPE)
            {
                case Destitute:
                {
                    return "Destitute";
                }
                case Poor:
                {
                    return "Poor";
                }
                case LowerMiddle:
                {
                    return "LowerMiddle";
                }
                case UpperMiddle:
                {
                    return "UpperMiddle";
                }
                case Rich:
                {
                    return "Rich";
                }
                case Lavish:
                {
                    return "Lavish";
                }
                case Royal:
                {
                    return "Royal";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(WEALTH_TYPE, "ToString");
                }
            }
        }

        wealth_type::Enum wealth_type::FromRankType(const creature::rank_class::Enum RANK_CLASS)
        {
            auto const RANK_CLASS_STR{ creature::rank_class::ToString(RANK_CLASS) };

            misc::VectorMap<wealth_type::Enum, float> wealthChanceMap;

            auto wealthTypeChanceRemaining{ wealth_type::Count };

            for (std::size_t i(0); i < wealth_type::Count; ++i)
            {
                auto const NEXT_WEALTH_TYPE{ static_cast<wealth_type::Enum>(i) };
                auto const NEXT_WEALTH_TYPE_NAME{ wealth_type::ToString(NEXT_WEALTH_TYPE) };

                std::ostringstream ss;
                ss << "heroespath-wealthtype-chance-" << RANK_CLASS_STR << "-"
                   << NEXT_WEALTH_TYPE_NAME << "-one-in";

                auto const NEXT_VALUE_STR{ game::GameDataFile::Instance()->GetCopyStr(ss.str()) };

                if (NEXT_VALUE_STR == "remaining")
                {
                    wealthTypeChanceRemaining = NEXT_WEALTH_TYPE;
                }

                auto nextChanceValue{ 0.0f };
                try
                {
                    nextChanceValue = boost::lexical_cast<float>(NEXT_VALUE_STR);
                }
                catch (...)
                {
                    nextChanceValue = -1.0f;
                }

                wealthChanceMap[NEXT_WEALTH_TYPE]
                    = ((nextChanceValue > 0.0f) ? (1.0f / nextChanceValue) : (0.0f));
            }

            auto cumulative{ 0.0f };
            auto const RAND{ misc::random::Float() };
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

        wealth_type::Enum wealth_type::FromRank(const Rank_t & RANK)
        {
            return FromRankType(creature::rank_class::FromRank(RANK));
        }

        wealth_type::Enum wealth_type::FromCreature(const creature::CreaturePtr_t CHARACTER_PTR)
        {
            return FromRank(CHARACTER_PTR->Rank());
        }

        void collector_type::ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR)
        {
            AppendNameIfBitIsSet(
                ss, ENUM_VALUE, collector_type::Minimalist, "Minimalist", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, collector_type::Practical, "Practical", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, collector_type::Collector, "Collector", SEPARATOR);
            AppendNameIfBitIsSet(ss, ENUM_VALUE, collector_type::Hoarder, "Hoarder", SEPARATOR);
        }

        collector_type::Enum
            collector_type::FromCreature(const creature::CreaturePtr_t CHARACTER_PTR)
        {
            auto const CHANCE_BASE(game::GameDataFile::Instance()->GetCopyFloat(
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-base"));

            // adjust for race
            auto const RACE_STR{ creature::race::ToString(CHARACTER_PTR->Race()) };

            auto const RACE_KEY{
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-race-"
                + RACE_STR
            };

            auto const RACE_COLLECTOR_PARTS_STR{ game::GameDataFile::Instance()->GetCopyStr(
                RACE_KEY) };

            misc::StrVec_t racePartsVec;
            appbase::stringhelp::SplitByChar(
                RACE_COLLECTOR_PARTS_STR, racePartsVec, ',', true, true);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (racePartsVec.size() == 4),
                "non_player::ownership::collector_type::FromCreature("
                    << CHARACTER_PTR->NameAndRaceAndRole()
                    << ") failed to read four values from the key=" << RACE_KEY);

            float chanceMinimalist(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[0]));
            float chancePractical(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[1]));
            float chanceCollector(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[2]));
            float chanceHoarder(CHANCE_BASE + ConvertStringToFloat(RACE_KEY, racePartsVec[3]));

            // adjust for roles
            auto const ROLE_STR{ creature::role::ToString(CHARACTER_PTR->Role()) };

            auto const ROLE_KEY{
                "heroespath-nonplayer-ownershipprofile-collectortype-chance-adjustment-role-"
                + ROLE_STR
            };

            auto const ROLE_COLLECTOR_PARTS_STR{ game::GameDataFile::Instance()->GetCopyStr(
                ROLE_KEY) };

            misc::StrVec_t rolePartsVec;
            appbase::stringhelp::SplitByChar(
                ROLE_COLLECTOR_PARTS_STR, rolePartsVec, ',', true, true);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (rolePartsVec.size() == 4),
                "non_player::ownership::collector_type::FromCreature("
                    << CHARACTER_PTR->NameAndRaceAndRole()
                    << ") failed to read four values from the key=" << ROLE_KEY);

            chanceMinimalist += ConvertStringToFloat(ROLE_KEY, rolePartsVec[0]);
            chancePractical += ConvertStringToFloat(ROLE_KEY, rolePartsVec[1]);
            chanceCollector += ConvertStringToFloat(ROLE_KEY, rolePartsVec[2]);
            chanceHoarder += ConvertStringToFloat(ROLE_KEY, rolePartsVec[3]);

            // enforce min
            {
                const float CHANCE_MIN(game::GameDataFile::Instance()->GetCopyFloat(
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

            // enforce max
            {
                const float CHANCE_MAX(game::GameDataFile::Instance()->GetCopyFloat(
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

            // determine
            collector_type::Enum collectorType{ None };
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

        const std::string owns_magic_type::ToString(const owns_magic_type::Enum OWNS_MAGIC_TYPE)
        {
            if (OWNS_MAGIC_TYPE == Rarely)
            {
                return "Rarely";
            }
            else if (OWNS_MAGIC_TYPE == Religious)
            {
                return "Religious";
            }
            else if (OWNS_MAGIC_TYPE == Magical)
            {
                return "Magical";
            }
            else
            {
                ThrowInvalidValueForFunction(OWNS_MAGIC_TYPE, "ToString");
            }
        }

        owns_magic_type::Enum
            owns_magic_type::FromCreature(const creature::CreaturePtr_t CHARACTER_PTR)
        {
            float chanceRarely(0.0f);
            float chanceReligious(0.0f);
            float chanceMagical(0.0f);

            // adjust for race
            {
                auto const RACE_STR(creature::race::ToString(CHARACTER_PTR->Race()));

                auto const RACE_KEY{
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-race-" + RACE_STR
                };

                auto const RACE_OWNSMAGIC_PARTS_STR{ game::GameDataFile::Instance()->GetCopyStr(
                    RACE_KEY) };

                misc::StrVec_t racePartsVec;
                appbase::stringhelp::SplitByChar(
                    RACE_OWNSMAGIC_PARTS_STR, racePartsVec, ',', true, true);

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (racePartsVec.size() == 3),
                    "non_player::ownership::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole()
                        << ") failed to read three values from the key=" << RACE_KEY);

                auto const RARELY_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[0]) };
                auto const RELIGIOUS_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[1]) };
                auto const MAGICAL_RACE_ADJ{ ConvertStringToFloat(RACE_KEY, racePartsVec[2]) };

                // Why do these values have to add up to one?
                // After the role adjustments below the values most definitely don't sum to 1.0...?
                // zTn -2017-7-14
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (misc::IsRealOne(RARELY_RACE_ADJ + RELIGIOUS_RACE_ADJ + MAGICAL_RACE_ADJ)),
                    "non_player::ownership::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole() << ") found key " << RACE_KEY << "="
                        << RACE_OWNSMAGIC_PARTS_STR << " -but those values do not add up to one.");

                chanceRarely += RARELY_RACE_ADJ;
                chanceReligious += RELIGIOUS_RACE_ADJ;
                chanceMagical += MAGICAL_RACE_ADJ;
            }

            // adjust for role
            {
                auto const ROLE_STR{ creature::role::ToString(CHARACTER_PTR->Role()) };

                std::ostringstream ss;
                ss << "heroespath-nonplayer-ownershipprofile-"
                   << "ownsmagictype-chance-adjustment-Rarely-role-" << ROLE_STR;

                auto const ROLE_KEY{
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-adjustment-role-"
                    + ROLE_STR
                };

                auto const ROLE_OWNSMAGIC_PARTS_STR{ game::GameDataFile::Instance()->GetCopyStr(
                    ROLE_KEY) };

                misc::StrVec_t rolePartsVec;
                appbase::stringhelp::SplitByChar(
                    ROLE_OWNSMAGIC_PARTS_STR, rolePartsVec, ',', true, true);

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (rolePartsVec.size() == 3),
                    "non_player::ownership::owns_magic_type::FromCreature("
                        << CHARACTER_PTR->NameAndRaceAndRole()
                        << ") failed to read three values from the key=" << ROLE_KEY);

                chanceRarely += ConvertStringToFloat(ROLE_KEY, rolePartsVec[0]);
                chanceReligious += ConvertStringToFloat(ROLE_KEY, rolePartsVec[1]);
                chanceMagical += ConvertStringToFloat(ROLE_KEY, rolePartsVec[2]);
            }

            // enforce min
            {
                const float MIN(game::GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-min"));

                if (chanceRarely < MIN)
                {
                    chanceRarely = MIN;
                }

                if (chanceReligious < MIN)
                {
                    chanceReligious = MIN;
                }

                if (chanceMagical < MIN)
                {
                    chanceMagical = MIN;
                }
            }

            // enforce max
            {
                const float MAX(game::GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-nonplayer-ownershipprofile-ownsmagictype-chance-max"));

                if (chanceRarely > MAX)
                {
                    chanceRarely = MAX;
                }

                if (chanceReligious > MAX)
                {
                    chanceReligious = MAX;
                }

                if (chanceMagical > MAX)
                {
                    chanceMagical = MAX;
                }
            }

            // determine
            auto const RAND(
                misc::random::Float(0.0f, (chanceRarely + chanceReligious + chanceRarely)));

            if (RAND < chanceMagical)
            {
                return Magical;
            }
            else
            {
                if (RAND < (chanceMagical + chanceReligious))
                {
                    return Religious;
                }
                else
                {
                    return Rarely;
                }
            }
        }

        const std::string complexity_type::ToString(const complexity_type::Enum COMPLEXITY_TYPE)
        {
            switch (COMPLEXITY_TYPE)
            {
                case Animal:
                {
                    return "Animal";
                }
                case Simple:
                {
                    return "Simple";
                }
                case Moderate:
                {
                    return "Moderate";
                }
                case Complex:
                {
                    return "Complex";
                }
                case Count:
                default:
                {
                    ThrowInvalidValueForFunction(COMPLEXITY_TYPE, "ToString");
                }
            }
        }

        complexity_type::Enum
            complexity_type::FromCreature(const creature::CreaturePtr_t CHARACTER_PTR)
        {
            auto const RACE_COMPLEXITY_STR{ game::GameDataFile::Instance()->GetCopyStr(
                "heroespath-nonplayer-ownershipprofile-complexitytype-race-"
                + creature::race::ToString(CHARACTER_PTR->Race())) };

            if (RACE_COMPLEXITY_STR == "based-on-role")
            {
                return static_cast<complexity_type::Enum>(
                    FromString(game::GameDataFile::Instance()->GetCopyStr(
                        "heroespath-nonplayer-ownershipprofile-complexitytype-role-"
                        + creature::role::ToString(CHARACTER_PTR->Role()))));
            }
            else
            {
                return static_cast<complexity_type::Enum>(FromString(RACE_COMPLEXITY_STR));
            }
        }

        Profile::Profile(
            const wealth_type::Enum WEALTH_TYPE,
            const collector_type::Enum COLLECTOR_TYPE,
            const owns_magic_type::Enum MAGIC_OWN_TYPE,
            const complexity_type::Enum COMPLEXITY_TYPE)
            : wealthType(WEALTH_TYPE)
            , collectorType(COLLECTOR_TYPE)
            , magicType(MAGIC_OWN_TYPE)
            , complexityType(COMPLEXITY_TYPE)
        {}

        const Profile Profile::Make_FromCreature(const creature::CreaturePtr_t CHARACTER_PTR)
        {
            return Profile(
                wealth_type::FromCreature(CHARACTER_PTR),
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
                ss << "non_player::ownership::ConvertStringToFloat(key=" << KEY
                   << ", STR_FLOAT=" << STR_FLOAT << " unable to convert that str to a float.";

                throw std::runtime_error(ss.str());
            }
        }

    } // namespace ownership
} // namespace non_player
} // namespace heroespath
