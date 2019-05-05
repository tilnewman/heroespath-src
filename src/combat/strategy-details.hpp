// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
//
// strategy-details.hpp
//
#include "combat/strategy-chances.hpp"
#include "combat/strategy-enums.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "misc/enum-util.hpp"
#include "misc/strings.hpp"
#include "misc/vector-map.hpp"

#include <algorithm>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        using RaceRolePair_t = std::pair<creature::race::Enum, creature::role::Enum>;
        using RaceRoleChancesMap_t = misc::VectorMap<RaceRolePair_t, Chances>;

        // Responsible for loading creature strategy details from the GameDataFile, storing them,
        // and providing quick and easy access.
        class CreatureStrategies
        {
        public:
            CreatureStrategies(const CreatureStrategies &) = delete;
            CreatureStrategies(CreatureStrategies &&) = delete;
            CreatureStrategies & operator=(const CreatureStrategies &) = delete;
            CreatureStrategies & operator=(CreatureStrategies &&) = delete;

            CreatureStrategies();

            const Chances Get(const creature::race::Enum, const creature::role::Enum) const;

        private:
            void Setup();

            void ParseSubPartsSelect(const std::vector<std::string> &, SelectChanceMap_t &) const;
            void ParseSubPartsRefine(const std::vector<std::string> &, RefineChanceMap_t &) const;
            void ParseSubPartsAdvance(const std::vector<std::string> &, AdvanceChanceMap_t &) const;
            void ParseSubPartsRetreat(const std::vector<std::string> &, RetreatChanceMap_t &) const;

            void ParseSubPartsFrequency(
                const std::string &,
                const std::vector<std::string> &,
                FrequencyChanceMap_t &) const;

            void ParseSubPartsOutnumberRetreat(
                const std::vector<std::string> &, OutnumberRetreatChanceMap_t &) const;

            template <typename BaseType, typename EnumType>
            void ParseEnumColonChance(
                const std::string & FUNCTION_NAME,
                const std::string & SUB_STR,
                misc::VectorMap<EnumType, float> & OutParam_EnumChanceMap) const
            {
                const std::vector<std::string> ENUM_CHANCE_STR_VEC { misc::SplitByChars(
                    SUB_STR, misc::SplitHow(":")) };

                if (ENUM_CHANCE_STR_VEC.size() != 2)
                {
                    throw std::runtime_error(
                        "combat::strategy::CreatureStrategies::" + FUNCTION_NAME
                        + "() Found invalid subparts string: \"" + SUB_STR + "\".");
                }

                EnumType typeEnum;
                try
                {
                    typeEnum = EnumUtil<BaseType>::FromString(ENUM_CHANCE_STR_VEC.at(0));
                }
                catch (const std::exception & EX)
                {
                    throw std::runtime_error(
                        "combat::strategy::CreatureStrategies::" + FUNCTION_NAME
                        + "()  Threw exception calling T::FromString(\"" + ENUM_CHANCE_STR_VEC.at(0)
                        + "\")  This is probably just a typo in the gamedatafile.  Exception: ["
                        + EX.what() + "].");
                }

                const auto CHANCE { ParseChanceString(ENUM_CHANCE_STR_VEC.at(1)) };
                if ((CHANCE < 0.0f) || (CHANCE > 1.0f))
                {
                    throw std::runtime_error(
                        "combat::strategy::CreatureStrategies::" + FUNCTION_NAME
                        + "()  Failed to parse \"" + ENUM_CHANCE_STR_VEC.at(1)
                        + "\" into a valid 'enum:float'.  Resulting float (chance) was: "
                        + std::to_string(CHANCE) + ".");
                }

                OutParam_EnumChanceMap[typeEnum] = CHANCE;
            }

            float ParseChanceString(const std::string & FLOAT_STR) const;

        private:
            static const std::string SUBPART_TITLE_SELECT_;
            static const std::string SUBPART_TITLE_REFINE_;
            static const std::string SUBPART_TITLE_ADVANCE_;
            static const std::string SUBPART_TITLE_RETREAT_;
            static const std::string SUBPART_TITLE_OUTNUMBER_RETREAT_;
            static const std::string SUBPART_TITLE_FREQ_ROAR_;
            static const std::string SUBPART_TITLE_FREQ_CAST_;
            static const std::string SUBPART_TITLE_FREQ_FLY_;
            static const std::string SUBPART_TITLE_FREQ_FLYPOUNCE_;
            static const std::string SUBPART_TITLE_FREQ_STANDPOUNCE_;

            RaceRoleChancesMap_t raceRoleChancesMap_;
        };

        using CreatureStrategiesUPtr_t = std::unique_ptr<CreatureStrategies>;

    } // namespace strategy
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
