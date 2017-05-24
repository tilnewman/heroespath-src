//
// strategy-details.cpp
//
#include "strategy-details.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"

#include <vector>
#include <map>


namespace game
{
namespace combat
{
namespace strategy
{

    const std::string ChanceFactory::SUBPART_TITLE_SELECT_           ("Select");
    const std::string ChanceFactory::SUBPART_TITLE_REFINE_           ("Refine");
    const std::string ChanceFactory::SUBPART_TITLE_ADVANCE_          ("Advance");
    const std::string ChanceFactory::SUBPART_TITLE_RETREAT_          ("Retreat");
    const std::string ChanceFactory::SUBPART_TITLE_OUTNUMBER_RETREAT_("OutnumberRetreat");
    const std::string ChanceFactory::SUBPART_TITLE_FREQ_ROAR_        ("RoarFreq");
    const std::string ChanceFactory::SUBPART_TITLE_FREQ_CAST_        ("CastFreq");
    const std::string ChanceFactory::SUBPART_TITLE_FREQ_FLY_         ("FlyFreq");
    const std::string ChanceFactory::SUBPART_TITLE_FREQ_FLYPOUNCE_   ("FlyPounceFreq");
    const std::string ChanceFactory::SUBPART_TITLE_FREQ_STANDPOUNCE_ ("StandingPounceFreq");
    //
    DetailsSPtr_t ChanceFactory::instance_(nullptr);


    ChanceFactory::ChanceFactory()
    :
        raceRoleChancesMap_()
    {}


    ChanceFactory::~ChanceFactory()
    {}


    DetailsSPtr_t ChanceFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset(new ChanceFactory());

        return instance_;
    }


    void ChanceFactory::Initialize()
    {
        std::map<creature::race::Enum, Chances> raceChancesMap;
        for (int i(0); i < creature::race::Count; ++i)
        {
            auto const RACE_ENUM( static_cast<creature::race::Enum>(i) );
            auto const RACE_STR( creature::race::ToString(RACE_ENUM) );

            std::ostringstream keySS;
            keySS << "heroespath-nonplayer-combat-strategy-race-" << RACE_STR;
            const std::string KEY(keySS.str());

            const std::string VALUE( GameDataFile::Instance()->GetCopyStr(KEY) );
            M_ASSERT_OR_LOGANDTHROW_SS((VALUE.empty() == false), "game::combat::strategy::ChanceFactory::Initialize()  (while parsing race=\"" << RACE_STR << "\")  failed to find gamedatafile value with key=\"" << KEY << "\"");

            std::vector<std::string> partsVec;
            appbase::stringhelp::SplitByChar(VALUE, partsVec, '|', true, true);

            SelectChanceMap_t    selectChanceMap;
            RefineChanceMap_t    refineChanceMap;
            AdvanceChanceMap_t   advanceChanceMap;
            RetreatChanceMap_t   retreatChanceMap;
            OutnumberRetreatChanceMap_t outnumberRetreatChanceMap;
            FrequencyChanceMap_t roarFreqChanceMap;
            FrequencyChanceMap_t castFreqChanceMap;
            FrequencyChanceMap_t flyFreqChanceMap;
            FrequencyChanceMap_t flyPounceFreqChanceMap;
            FrequencyChanceMap_t standPounceFreqChanceMap;

            for (auto const & NEXT_PART_STR : partsVec)
            {
                std::vector<std::string> subPartsVec;
                appbase::stringhelp::SplitByChar(NEXT_PART_STR, subPartsVec, ',', true, true);
                M_ASSERT_OR_LOGANDTHROW_SS((subPartsVec.empty() == false), "game::combat::strategy::ChanceFactory::Initialize()  (while parsing race=\"" << RACE_STR << "\")  Failed to parse \"" << NEXT_PART_STR << "\" into any sub strings sep by comma.");

                const std::string SUBPART_TITLE_STR(subPartsVec.at(0));
                if (SUBPART_TITLE_STR == SUBPART_TITLE_SELECT_)
                {
                    ParseSubPartsSelect(subPartsVec, selectChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_REFINE_)
                {
                    ParseSubPartsRefine(subPartsVec, refineChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_ADVANCE_)
                {
                    ParseSubPartsAdvance(subPartsVec, advanceChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_RETREAT_)
                {
                    ParseSubPartsRetreat(subPartsVec, retreatChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_OUTNUMBER_RETREAT_)
                {
                    ParseSubPartsOutnumberRetreat(subPartsVec, outnumberRetreatChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_ROAR_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_ROAR_, subPartsVec, roarFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_CAST_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_CAST_, subPartsVec, castFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_FLY_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_FLY_, subPartsVec, flyFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_FLYPOUNCE_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_FLYPOUNCE_, subPartsVec, flyPounceFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_STANDPOUNCE_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_STANDPOUNCE_, subPartsVec, standPounceFreqChanceMap);
                }
                else
                {
                    std::ostringstream ss;
                    ss << "game::combat::strategy::ChanceFactory::ParsePartsString() Didn't find a known subparts title.  Found \"" << subPartsVec.at(0) << "\" instead.";
                    throw std::runtime_error(ss.str());
                }
            }

            raceChancesMap[RACE_ENUM] = Chances(selectChanceMap,
                                                refineChanceMap,
                                                advanceChanceMap,
                                                retreatChanceMap,
                                                outnumberRetreatChanceMap,
                                                roarFreqChanceMap,
                                                castFreqChanceMap,
                                                flyFreqChanceMap,
                                                flyPounceFreqChanceMap,
                                                standPounceFreqChanceMap);
        }

        std::map<creature::role::Enum, Chances> roleChancesMap;
        for (int i(0); i < creature::role::Count; ++i)
        {
            auto const ROLE_ENUM( static_cast<creature::role::Enum>(i) );
            auto const ROLE_STR( creature::role::ToString(ROLE_ENUM) );

            std::ostringstream keySS;
            keySS << "heroespath-nonplayer-combat-strategy-adjustment-role-" << ROLE_STR;
            const std::string KEY(keySS.str());

            const std::string VALUE( GameDataFile::Instance()->GetCopyStr(KEY) );
            M_ASSERT_OR_LOGANDTHROW_SS((VALUE.empty() == false), "game::combat::strategy::ChanceFactory::Initialize()  (while parsing role=\"" << ROLE_STR << "\")  failed to find gamedatafile value with key=\"" << KEY << "\"");

            std::vector<std::string> partsVec;
            appbase::stringhelp::SplitByChar(VALUE, partsVec, '|', true, true);

            SelectChanceMap_t    selectChanceMap;
            RefineChanceMap_t    refineChanceMap;
            AdvanceChanceMap_t   advanceChanceMap;
            RetreatChanceMap_t    retreatChanceMap;
            OutnumberRetreatChanceMap_t outnumberRetreatChanceMap;
            FrequencyChanceMap_t roarFreqChanceMap;
            FrequencyChanceMap_t castFreqChanceMap;
            FrequencyChanceMap_t flyFreqChanceMap;
            FrequencyChanceMap_t flyPounceFreqChanceMap;
            FrequencyChanceMap_t standPounceFreqChanceMap;

            for (auto const & NEXT_PART_STR : partsVec)
            {
                std::vector<std::string> subPartsVec;
                appbase::stringhelp::SplitByChar(NEXT_PART_STR, subPartsVec, ',', true, true);
                M_ASSERT_OR_LOGANDTHROW_SS((subPartsVec.empty() == false), "game::combat::strategy::ChanceFactory::Initialize()  (while parsing role=\"" << ROLE_STR << "\")  Failed to parse \"" << NEXT_PART_STR << "\" into any sub strings sep by comma.");

                const std::string SUBPART_TITLE_STR(subPartsVec.at(0));
                if (SUBPART_TITLE_STR == SUBPART_TITLE_SELECT_)
                {
                    ParseSubPartsSelect(subPartsVec, selectChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_REFINE_)
                {
                    ParseSubPartsRefine(subPartsVec, refineChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_ADVANCE_)
                {
                    ParseSubPartsAdvance(subPartsVec, advanceChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_RETREAT_)
                {
                    ParseSubPartsRetreat(subPartsVec, retreatChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_OUTNUMBER_RETREAT_)
                {
                    ParseSubPartsOutnumberRetreat(subPartsVec, outnumberRetreatChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_ROAR_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_ROAR_, subPartsVec, roarFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_CAST_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_CAST_, subPartsVec, castFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_FLY_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_FLY_, subPartsVec, flyFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_FLYPOUNCE_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_FLYPOUNCE_, subPartsVec, flyPounceFreqChanceMap);
                }
                else if (SUBPART_TITLE_STR == SUBPART_TITLE_FREQ_STANDPOUNCE_)
                {
                    ParseSubPartsFrequency(SUBPART_TITLE_FREQ_STANDPOUNCE_, subPartsVec, standPounceFreqChanceMap);
                }
                else
                {
                    std::ostringstream ss;
                    ss << "game::combat::strategy::ChanceFactory::ParsePartsString() (while parsing role=\"" << ROLE_STR << "\") Didn't find a known subparts title.  Found \"" << subPartsVec.at(0) << "\" instead.";
                    throw std::runtime_error(ss.str());
                }
            }

            roleChancesMap[ROLE_ENUM] = Chances(selectChanceMap,
                                                refineChanceMap,
                                                advanceChanceMap,
                                                retreatChanceMap,
                                                outnumberRetreatChanceMap,
                                                roarFreqChanceMap,
                                                castFreqChanceMap,
                                                flyFreqChanceMap,
                                                flyPounceFreqChanceMap,
                                                standPounceFreqChanceMap);
        }

        for (auto const & NEXT_RACECHANCES_PAIR : raceChancesMap)
        {
            for (auto const & NEXT_ROLECHANCES_PAIR : roleChancesMap)
            {
                auto const CHANCES{ NEXT_RACECHANCES_PAIR.second.AdjustCopy(NEXT_ROLECHANCES_PAIR.second) };
                //M_HP_LOG("ChanceFactory::Initialize() race=" << creature::race::ToString(NEXT_RACECHANCES_PAIR.first) << ", role=" << creature::role::ToString(NEXT_ROLECHANCES_PAIR.first) << ", \t\t" << CHANCES.ToString());
                raceRoleChancesMap_[std::make_pair(NEXT_RACECHANCES_PAIR.first, NEXT_ROLECHANCES_PAIR.first)] = CHANCES;
            }
        }
    }


    const Chances ChanceFactory::Get(const creature::race::Enum RACE_ENUM, const creature::role::Enum ROLE_ENUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((raceRoleChancesMap_.empty() == false), "game::combat::strategy::ChanceFactory::Get(" << creature::race::ToString(RACE_ENUM) << ", " << creature::role::ToString(ROLE_ENUM) << ") called before Initialize() was called.");
        return raceRoleChancesMap_[std::make_pair(RACE_ENUM, ROLE_ENUM)];
    }


    void ChanceFactory::ParseSubPartsSelect(const std::vector<std::string> & PARTS_VEC, SelectChanceMap_t & OutParam_SelectChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : PARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_SELECT_)
            {
                continue;
            }
            else if (NEXT_SUBSTRING == "even_dist")
            {
                auto const EVEN_DIST_RATIO(1.0f / 18.0f);
                OutParam_SelectChanceMap[SelectType::Pixie]        = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Dragon]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Human]        = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Gnome]        = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Wolfen]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Archer]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Sorcerer]     = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Knight]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Beastmaster]  = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Cleric]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Theif]        = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Bard]         = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::FireBrand]    = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Sylavin]      = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Projectile]   = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Caster]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::CanFly]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Beast]        = EVEN_DIST_RATIO;
            }
            else if (NEXT_SUBSTRING == "even_dist_role")
            {
                auto const EVEN_DIST_RATIO(1.0f / 10.0f);
                OutParam_SelectChanceMap[SelectType::Wolfen]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Archer]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Sorcerer]     = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Knight]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Beastmaster]  = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Cleric]       = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Theif]        = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Bard]         = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::FireBrand]    = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Sylavin]      = EVEN_DIST_RATIO;
            }
            else if (NEXT_SUBSTRING == "even_dist_race")
            {
                auto const EVEN_DIST_RATIO(1.0f / 5.0f);
                OutParam_SelectChanceMap[SelectType::Pixie]  = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Dragon] = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Human]  = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Gnome]  = EVEN_DIST_RATIO;
                OutParam_SelectChanceMap[SelectType::Wolfen] = EVEN_DIST_RATIO;
            }
            else
            {
                ParseEnumColonChance<SelectType>("ParseSubPartsSelect", NEXT_SUBSTRING, OutParam_SelectChanceMap);
            }
        }
    }


    void ChanceFactory::ParseSubPartsRefine(const std::vector<std::string> & PARTS_VEC, RefineChanceMap_t & OutParam_refineChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : PARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_REFINE_)
            {
                continue;
            }
            else if (NEXT_SUBSTRING == "even_dist")
            {
                auto const CHANCE(1.0f / 12.0f);
                OutParam_refineChanceMap[RefineType::Murderer    ] = CHANCE;
                OutParam_refineChanceMap[RefineType::Bloodthirsty] = CHANCE;
                OutParam_refineChanceMap[RefineType::Coward      ] = CHANCE;
                OutParam_refineChanceMap[RefineType::Hit         ] = CHANCE;
                OutParam_refineChanceMap[RefineType::Attack      ] = CHANCE;
                OutParam_refineChanceMap[RefineType::MusicMaker  ] = CHANCE;
                OutParam_refineChanceMap[RefineType::Caster	     ] = CHANCE;
                OutParam_refineChanceMap[RefineType::Enchanted   ] = CHANCE;
                OutParam_refineChanceMap[RefineType::NotEnchanted] = CHANCE;
                OutParam_refineChanceMap[RefineType::Steadfast   ] = CHANCE;
                OutParam_refineChanceMap[RefineType::LastTo      ] = CHANCE;
                OutParam_refineChanceMap[RefineType::MostDamage  ] = CHANCE;
            }
            else
            {
                ParseEnumColonChance<RefineType>("ParseSubPartsRefine", NEXT_SUBSTRING, OutParam_refineChanceMap);
            }
        }
    }


    void ChanceFactory::ParseSubPartsAdvance(const std::vector<std::string> & PARTS_VEC, AdvanceChanceMap_t & OutParam_AdvanceChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : PARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_ADVANCE_)
            {
                continue;
            }
            else if (NEXT_SUBSTRING == "even_dist")
            {
                auto const CHANCE(1.0f / 4.0f);
                OutParam_AdvanceChanceMap[AdvanceType::None]     = CHANCE;
                OutParam_AdvanceChanceMap[AdvanceType::Cowardly] = CHANCE;
                OutParam_AdvanceChanceMap[AdvanceType::Hesitant] = CHANCE;
                OutParam_AdvanceChanceMap[AdvanceType::Charger]  = CHANCE;
            }
            else
            {
                ParseEnumColonChance<AdvanceType>("ParseSubPartsAdvance", NEXT_SUBSTRING, OutParam_AdvanceChanceMap);
            }
        }
    }


    void ChanceFactory::ParseSubPartsRetreat(const std::vector<std::string> & PARTS_VEC, RetreatChanceMap_t & OutParam_RetreatChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : PARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_RETREAT_)
            {
                continue;
            }
            else if (NEXT_SUBSTRING == "even_dist")
            {
                auto const CHANCE(1.0f / 3.0f);
                OutParam_RetreatChanceMap[RetreatType::None]   = CHANCE;
                OutParam_RetreatChanceMap[RetreatType::Wary]   = CHANCE;
                OutParam_RetreatChanceMap[RetreatType::Coward] = CHANCE;
            }
            else
            {
                ParseEnumColonChance<RetreatType>("ParseSubPartsRetreat", NEXT_SUBSTRING, OutParam_RetreatChanceMap);
            }
        }
    }


    void ChanceFactory::ParseSubPartsFrequency(const std::string &              SUBPART_TITLE_STR,
                                               const std::vector<std::string> & SUBPARTS_VEC,
                                               FrequencyChanceMap_t &           OutParam_FreqChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : SUBPARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_STR)
            {
                continue;
            }
            else if (NEXT_SUBSTRING == "even_dist")
            {
                auto const CHANCE(1.0f / 7.0f);
                OutParam_FreqChanceMap[FrequencyType::Never]    = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Rarely]   = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Seldom]   = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Neutral]  = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Commonly] = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Often]    = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Always]   = CHANCE;
            }
            else if (NEXT_SUBSTRING == "even_dist_no_abs")
            {
                auto const CHANCE(1.0f / 5.0f);
                OutParam_FreqChanceMap[FrequencyType::Rarely]   = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Seldom]   = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Neutral]  = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Commonly] = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Often]    = CHANCE;
            }
            else if (NEXT_SUBSTRING == "even_dist_coun")
            {
                auto const CHANCE(1.0f / 3.0f);
                OutParam_FreqChanceMap[FrequencyType::Once]   = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Twice]  = CHANCE;
                OutParam_FreqChanceMap[FrequencyType::Thrice] = CHANCE;
            }
            else
            {
                ParseEnumColonChance<FrequencyType>("ParseSubPartsFrequency", NEXT_SUBSTRING, OutParam_FreqChanceMap);
            }
        }
    }


    void ChanceFactory::ParseSubPartsOutnumberRetreat(const std::vector<std::string> & SUBPARTS_VEC, OutnumberRetreatChanceMap_t & OutParam_OutnumberRetreatChanceMap) const
    {
        for (auto const & NEXT_SUBSTRING : SUBPARTS_VEC)
        {
            if (NEXT_SUBSTRING == SUBPART_TITLE_OUTNUMBER_RETREAT_)
            {
                continue;
            }
            else
            {
                std::vector<std::string> countCountColonChanceStrVec;
                appbase::stringhelp::SplitByChar(NEXT_SUBSTRING, countCountColonChanceStrVec, ':', true, true);
                M_ASSERT_OR_LOGANDTHROW_SS(((countCountColonChanceStrVec.size() == 2) || (countCountColonChanceStrVec.size() == 3)), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid substring: \"" << NEXT_SUBSTRING << "\".");

                //parse the chance first, which should be the last
                auto const CHANCE{ ParseChanceString(countCountColonChanceStrVec.at(countCountColonChanceStrVec.size() - 1)) };

                if (boost::algorithm::contains(countCountColonChanceStrVec.at(0), "-"))
                {
                    //handle case "range_start-range_end:(chance)" or "value:(chance)"

                    //split the number range into two numbers
                    std::vector<std::string> rangeNumberStrVec;
                    appbase::stringhelp::SplitByChar(countCountColonChanceStrVec.at(0), rangeNumberStrVec, '-', true, true);
                    M_ASSERT_OR_LOGANDTHROW_SS(((rangeNumberStrVec.size() == 1) || (rangeNumberStrVec.size() == 2)), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid range string: \"" << countCountColonChanceStrVec.at(0) << "\".  This should either be a whole counting number or two of them separated by a dash.");

                    //parse the second number second, if any, a zero means there is no range
                    auto rangeEnd = int{ 0 };
                    if (rangeNumberStrVec.size() == 2)
                    {
                        try
                        {
                            rangeEnd = boost::lexical_cast<int>(rangeNumberStrVec.at(1));
                        }
                        catch (...)
                        {
                            rangeEnd = -1;
                        }
                        M_ASSERT_OR_LOGANDTHROW_SS((rangeEnd >= 0), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid range end string: \"" << rangeNumberStrVec.at(1) << "\".  This should be a whole counting number greater than or equal to zero.");
                    }

                    //parse the first number third
                    auto rangeStart = int{ 0 };
                    try
                    {
                        rangeStart = boost::lexical_cast<int>(rangeNumberStrVec.at(0));
                    }
                    catch (...)
                    {
                        rangeStart = -1;
                    }
                    M_ASSERT_OR_LOGANDTHROW_SS((rangeStart >= 0), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid range start string: \"" << rangeNumberStrVec.at(0) << "\".  This should be a whole counting number greater than or equal to zero.");

                    if (rangeEnd > 0)
                        M_ASSERT_OR_LOGANDTHROW_SS((rangeStart < rangeEnd), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid range string: \"" << countCountColonChanceStrVec.at(0) << "\".  The start was not less than the end.");

                    if (0 == rangeEnd)
                    {
                        //case where there is no range, just a number with a chance
                        OutParam_OutnumberRetreatChanceMap[static_cast<std::size_t>(rangeStart)] = CHANCE;
                    }
                    else
                    {
                        //case where there is a valid range
                        for (int i(rangeStart); i <= rangeEnd; ++i)
                            OutParam_OutnumberRetreatChanceMap[static_cast<std::size_t>(i)] = CHANCE;
                    }
                }
                else
                {
                    //handle case "dist:(initial_count):(initial_chance)
                    M_ASSERT_OR_LOGANDTHROW_SS((countCountColonChanceStrVec.at(0) == "dist"), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid initial:count:chance string: \"" << countCountColonChanceStrVec.at(0) << "\".");

                    auto initialCount = int{ 0 };
                    try
                    {
                        initialCount = boost::lexical_cast<int>(countCountColonChanceStrVec.at(1));
                    }
                    catch (...)
                    {
                        initialCount = -1;
                    }
                    M_ASSERT_OR_LOGANDTHROW_SS((initialCount >= 0), "game::combat::strategy::ParseSubPartsOutnumberRetreat() Found invalid initial count string: \"" << countCountColonChanceStrVec.at(1) << "\".");

                    auto initialChance{ CHANCE };
                    for (int i(initialCount); i < 99; ++i)
                    {
                        OutParam_OutnumberRetreatChanceMap[static_cast<std::size_t>(i)] = initialChance;
                        initialChance = initialChance * 2.0f;
                        if (initialChance > 1.0f)
                            initialChance = 1.0f;
                    }
                }
            }
        }
    }


    float ChanceFactory::ParseChanceString(const std::string & FLOAT_STR) const
    {
        try
        {
            return boost::lexical_cast<float>(FLOAT_STR);
        }
        catch (...)
        {
            return -1.0f;
        }
    }

}
}
}
