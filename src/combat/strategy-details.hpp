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
#ifndef HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
//
// strategy-details.hpp
//
#include "combat/strategy-chances.hpp"
#include "combat/strategy-details.hpp"
#include "combat/strategy-enums.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"

#include "stringutil/stringhelp.hpp"

#include <algorithm>
#include <exception>
#include <map>
#include <memory>
#include <sstream>
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
        using RaceRoleChancesMap_t = std::map<RaceRolePair_t, Chances>;

        // forward declarations for the singleton implementation
        class ChanceFactory;
        using DetailsSPtr_t = std::shared_ptr<ChanceFactory>;

        // loads strategy details from GameDataFile and stores them per race and role
        class ChanceFactory
        {
            ChanceFactory(const ChanceFactory &) = delete;
            ChanceFactory(const ChanceFactory &&) = delete;
            ChanceFactory & operator=(const ChanceFactory &) = delete;
            ChanceFactory & operator=(const ChanceFactory &&) = delete;

        public:
            ChanceFactory();
            virtual ~ChanceFactory();
            static DetailsSPtr_t Instance();
            void Initialize();
            const Chances Get(const creature::race::Enum, const creature::role::Enum);

        private:
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
                std::map<EnumType, float> & OutParam_EnumChanceMap) const
            {
                std::vector<std::string> enumColonChanceStrVec;
                appbase::stringhelp::SplitByChar(SUB_STR, enumColonChanceStrVec, ':', true, true);

                if (enumColonChanceStrVec.size() != 2)
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::ChanceFactory::" << FUNCTION_NAME
                       << "() Found invalid subparts string: \"" << SUB_STR << "\".";
                    throw std::runtime_error(ss.str());
                }

                EnumType typeEnum;
                try
                {
                    typeEnum = BaseType::FromString(enumColonChanceStrVec.at(0));
                }
                catch (const std::exception & EX)
                {
                    std::ostringstream exceptionSS;
                    exceptionSS
                        << "combat::strategy::ChanceFactory::" << FUNCTION_NAME
                        << "()  Threw exceptionn calling T::FromString(\""
                        << enumColonChanceStrVec.at(0)
                        << "\")  This is probably just a typo in the gamedatafile.  Exception: ["
                        << EX.what() << "].";
                    throw std::runtime_error(exceptionSS.str());
                }

                auto const CHANCE{ ParseChanceString(enumColonChanceStrVec.at(1)) };
                if ((CHANCE < 0.0f) || (CHANCE > 1.0f))
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::ChanceFactory::" << FUNCTION_NAME
                       << "()  Failed to parse \"" << enumColonChanceStrVec.at(1)
                       << "\" into a valid 'enum:float'.  Resulting float (chance) was: " << CHANCE
                       << ".";
                    throw std::runtime_error(ss.str());
                }

                OutParam_EnumChanceMap[typeEnum] = CHANCE;
            }

            float ParseChanceString(const std::string & FLOAT_STR) const;

        public:
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

        private:
            static DetailsSPtr_t instance_;
            //
            RaceRoleChancesMap_t raceRoleChancesMap_;
        };
    } // namespace strategy
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
