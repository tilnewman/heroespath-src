#ifndef GAME_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
#define GAME_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
//
// strategy-details.hpp
//
#include "game/combat/strategy-enums.hpp"
#include "game/combat/strategy-details.hpp"
#include "game/combat/strategy-chances.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"

#include "stringutil/stringhelp.hpp"

#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <exception>
#include <algorithm>
#include <vector>
#include <map>


namespace game
{
namespace combat
{
namespace strategy
{

    using RaceRolePair_t = std::pair<creature::race::Enum, creature::role::Enum>;
    using RaceRoleChancesMap_t = std::map<RaceRolePair_t, Chances>;


    //forward declarations for the singleton implementation
    class ChanceFactory;
    using DetailsSPtr_t = std::shared_ptr<ChanceFactory>;


    //loads strategy details from GameDataFile and stores them per race and role
    class ChanceFactory
    {
        //prevent copy construction
        ChanceFactory(const ChanceFactory &) =delete;

        //prevent copy assignment
        ChanceFactory & operator=(const ChanceFactory &) =delete;

        //prevent non-singleton construction
        ChanceFactory();

    public:
        virtual ~ChanceFactory();
        static DetailsSPtr_t Instance();
        void Initialize();
        const Chances Get(const creature::race::Enum, const creature::role::Enum);

    private:
        void ParseSubPartsSelect(const std::vector<std::string> &, SelectChanceMap_t &) const;
        void ParseSubPartsRefine(const std::vector<std::string> &, RefineChanceMap_t &) const;
        void ParseSubPartsAdvance(const std::vector<std::string> &, AdvanceChanceMap_t &) const;
        void ParseSubPartsRetreat(const std::vector<std::string> &, RetreatChanceMap_t &) const;
        void ParseSubPartsFrequency(const std::string &, const std::vector<std::string> &, FrequencyChanceMap_t &) const;
        void ParseSubPartsOutnumberRetreat(const std::vector<std::string> &, OutnumberRetreatChanceMap_t &) const;

        template<typename BaseType, typename EnumType>
        void ParseEnumColonChance(const std::string &         FUNCTION_NAME,
                                  const std::string &         SUB_STR,
                                  std::map<EnumType, float> & OutParam_EnumChanceMap) const
        {
            std::vector<std::string> enumColonChanceStrVec;
            appbase::stringhelp::SplitByChar(SUB_STR, enumColonChanceStrVec, ':', true, true);

            if (enumColonChanceStrVec.size() != 2)
            {
                std::ostringstream ss;
                ss << "game::combat::strategy::ChanceFactory::" << FUNCTION_NAME << "() Found invalid subparts string: \"" << SUB_STR << "\".";
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
                exceptionSS << "game::combat::strategy::ChanceFactory::" << FUNCTION_NAME << "()  Threw exceptionn calling T::FromString(\"" << enumColonChanceStrVec.at(0) << "\")  This is probably just a typo in the gamedatafile.  Exception: [" << EX.what() << "].";
                throw std::runtime_error(exceptionSS.str());
            }

            auto const CHANCE{ ParseChanceString(enumColonChanceStrVec.at(1)) };
            if ((CHANCE < 0.0f) || (CHANCE > 1.0f))
            {
                std::ostringstream ss;
                ss << "game::combat::strategy::ChanceFactory::" << FUNCTION_NAME << "()  Failed to parse \"" << enumColonChanceStrVec.at(1) << "\" into a valid 'enum:float'.  Resulting float (chance) was: " << CHANCE << ".";
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

}
}
}


#endif //GAME_COMBAT_STRATEGYDETAILS_HPP_INCLUDED
