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
// strategy-chances.cpp
//
#include "strategy-chances.hpp"

#include "misc/real.hpp"

#include <tuple>
#include <utility>


namespace game
{
namespace combat
{
namespace strategy
{

    Chances::Chances(const SelectChanceMap_t &           SELECT_CHANCES_MAP,
                     const RefineChanceMap_t &           REFINE_CHANCES_MAP,
                     const AdvanceChanceMap_t &          ADVANCE_CHANCES_MAP,
                     const RetreatChanceMap_t &          RETREAT_CHANCES_MAP,
                     const OutnumberRetreatChanceMap_t & OUTNUMBER_RETREAT_CHANCES_MAP,
                     const FrequencyChanceMap_t &        ROAR_FREQ_CHANCES_MAP,
                     const FrequencyChanceMap_t &        CAST_FREQ_CHANCES_MAP,
                     const FrequencyChanceMap_t &        FLY_FREQ_CHANCES_MAP,
                     const FrequencyChanceMap_t &        FLY_POUNCE_FREQ_CHANCES_MAP,
                     const FrequencyChanceMap_t &        STAND_POUNCE_FREQ_CHANCES_MAP)
    :
        selectChanceMap_          (SELECT_CHANCES_MAP),
        refineChanceMap_          (REFINE_CHANCES_MAP),
        advanceChanceMap_         (ADVANCE_CHANCES_MAP),
        retreatChanceMap_         (RETREAT_CHANCES_MAP),
        outnumberRetreatChanceMap_(OUTNUMBER_RETREAT_CHANCES_MAP),
        roarFreqChanceMap_        (ROAR_FREQ_CHANCES_MAP),
        castFreqChanceMap_        (CAST_FREQ_CHANCES_MAP),
        flyFreqChanceMap_         (FLY_FREQ_CHANCES_MAP),
        flyPounceFreqChanceMap_   (FLY_POUNCE_FREQ_CHANCES_MAP),
        standPounceFreqChanceMap_ (STAND_POUNCE_FREQ_CHANCES_MAP)
    {}


    const Info Chances::Make() const
    {
        return Info(SelectRandomTypeEnumDiscrete(selectChanceMap_),
                    SelectRandomTypeEnumDiscrete(refineChanceMap_),
                    SelectRandomTypeEnumContinuous(advanceChanceMap_),
                    SelectRandomTypeEnumContinuous(retreatChanceMap_),
                    SelectRandomNumberContinuous(outnumberRetreatChanceMap_),
                    SelectRandomFrequencyType(roarFreqChanceMap_),
                    SelectRandomFrequencyType(castFreqChanceMap_),
                    SelectRandomFrequencyType(flyFreqChanceMap_),
                    SelectRandomFrequencyType(flyPounceFreqChanceMap_),
                    SelectRandomFrequencyType(standPounceFreqChanceMap_));
    }


    const Chances Chances::AdjustCopy(const Chances & ADJUSTER) const
    {
        Chances chanceCopy(* this);

        SelectChanceMap_t adjSelectChanceMap(ADJUSTER.SelectChanceMapCopy());
        for (auto & nextSelectChancePair : chanceCopy.selectChanceMap_)
        {
            nextSelectChancePair.second += adjSelectChanceMap[nextSelectChancePair.first];
            if (nextSelectChancePair.second < 0.0f)
                nextSelectChancePair.second = 0.0f;
        }

        RefineChanceMap_t adjRefineChanceMap(ADJUSTER.RefineChanceMapCopy());
        for (auto & nextRefineChancePair : chanceCopy.refineChanceMap_)
        {
            nextRefineChancePair.second += adjRefineChanceMap[nextRefineChancePair.first];
            if (nextRefineChancePair.second < 0.0f)
                nextRefineChancePair.second = 0.0f;
        }

        AdvanceChanceMap_t adjAdvanceChanceMap(ADJUSTER.AdvanceChanceMapCopy());
        for (auto & nextAdvanceChancePair : chanceCopy.advanceChanceMap_)
        {
            nextAdvanceChancePair.second += adjAdvanceChanceMap[nextAdvanceChancePair.first];
            if (nextAdvanceChancePair.second < 0.0f)
                nextAdvanceChancePair.second = 0.0f;
        }

        RetreatChanceMap_t adjRetreatChanceMap(ADJUSTER.RetreatChanceMapCopy());
        for (auto & nextRetreatChancePair : chanceCopy.retreatChanceMap_)
        {
            nextRetreatChancePair.second += adjRetreatChanceMap[nextRetreatChancePair.first];
            if (nextRetreatChancePair.second < 0.0f)
                nextRetreatChancePair.second = 0.0f;
        }

        OutnumberRetreatChanceMap_t adjOutnumberRetreatChanceMap(ADJUSTER.OutnumberRetreatChanceMapCopy());
        for (auto & nextOutnumberRetreatCountChancePair : chanceCopy.retreatChanceMap_)
        {
            nextOutnumberRetreatCountChancePair.second += adjOutnumberRetreatChanceMap[nextOutnumberRetreatCountChancePair.first];
            if (nextOutnumberRetreatCountChancePair.second < 0.0f)
                nextOutnumberRetreatCountChancePair.second = 0.0f;
        }

        FrequencyChanceMap_t adjFrequencyChanceMap(ADJUSTER.RoarFreqChanceMapCopy());
        for (auto & nextRoarFreqChancePair : chanceCopy.roarFreqChanceMap_)
        {
            nextRoarFreqChancePair.second += adjFrequencyChanceMap[nextRoarFreqChancePair.first];
            if (nextRoarFreqChancePair.second < 0.0f)
                nextRoarFreqChancePair.second = 0.0f;
        }

        FrequencyChanceMap_t adjCastFrequencyChanceMap(ADJUSTER.CastFreqChanceMapCopy());
        for (auto & nextCastFreqChancePair : chanceCopy.castFreqChanceMap_)
        {
            nextCastFreqChancePair.second += adjCastFrequencyChanceMap[nextCastFreqChancePair.first];
            if (nextCastFreqChancePair.second < 0.0f)
                nextCastFreqChancePair.second = 0.0f;
        }

        FrequencyChanceMap_t adjFlyFrequencyChanceMap(ADJUSTER.FlyFreqChanceMapCopy());
        for (auto & nextFlyFreqChancePair : chanceCopy.flyFreqChanceMap_)
        {
            nextFlyFreqChancePair.second += adjFlyFrequencyChanceMap[nextFlyFreqChancePair.first];
            if (nextFlyFreqChancePair.second < 0.0f)
                nextFlyFreqChancePair.second = 0.0f;
        }

        FrequencyChanceMap_t adjFlyPounceFrequencyChanceMap(ADJUSTER.FlyPounceFreqChanceMapCopy());
        for (auto & nextFlyPounceFreqChancePair : chanceCopy.flyPounceFreqChanceMap_)
        {
            nextFlyPounceFreqChancePair.second += adjFlyPounceFrequencyChanceMap[nextFlyPounceFreqChancePair.first];
            if (nextFlyPounceFreqChancePair.second < 0.0f)
                nextFlyPounceFreqChancePair.second = 0.0f;
        }

        FrequencyChanceMap_t adjStandPounceFrequencyChanceMap(ADJUSTER.StandPounceFreqChanceMapCopy());
        for (auto & nextStandPounceFreqChancePair : chanceCopy.standPounceFreqChanceMap_)
        {
            nextStandPounceFreqChancePair.second += adjStandPounceFrequencyChanceMap[nextStandPounceFreqChancePair.first];
            if (nextStandPounceFreqChancePair.second < 0.0f)
                nextStandPounceFreqChancePair.second = 0.0f;
        }

        return chanceCopy;
    }


    const std::string Chances::ToString() const
    {
        std::ostringstream ss;
        ss << "StrategyChances:"
            << "  Select=" << ChanceMapToString<SelectType, SelectType::Enum>(selectChanceMap_, true)
            << "  |  Refine=" << ChanceMapToString<RefineType, RefineType::Enum>(refineChanceMap_, true)
            << "  |  Advance=" << ChanceMapToString<AdvanceType, AdvanceType::Enum>(advanceChanceMap_, true)
            << "  |  Retreat=" << ChanceMapToString<RetreatType, RetreatType::Enum>(retreatChanceMap_, true)
            << "  |  Roar=" << ChanceMapToString<FrequencyType, FrequencyType::Enum>(roarFreqChanceMap_, false)
            << "  |  Cast=" << ChanceMapToString<FrequencyType, FrequencyType::Enum>(castFreqChanceMap_, false)
            << "  |  Fly=" << ChanceMapToString<FrequencyType, FrequencyType::Enum>(flyFreqChanceMap_, false)
            << "  |  FlyPounce=" << ChanceMapToString<FrequencyType, FrequencyType::Enum>(flyPounceFreqChanceMap_, false)
            << "  |  StandPounce=" << ChanceMapToString<FrequencyType, FrequencyType::Enum>(standPounceFreqChanceMap_, false)
            << "  |  RetreatCounts=(";

        std::size_t count{ outnumberRetreatChanceMap_.size() };

        if (count > 10)
            count = 10;

        if (0 == count)
        {
            ss << "0";
        }
        else
        {
            const std::string SEP{ ", " };
            for (std::size_t i(0); i < count; ++i)
            {
                ss << i << ":";

                auto const COUNTCHANCE_ITER{ outnumberRetreatChanceMap_.find(i) };
                if (COUNTCHANCE_ITER == outnumberRetreatChanceMap_.end())
                    ss << "none";
                else
                    ss << COUNTCHANCE_ITER->second;

                ss << SEP;
            }
        }

        ss << ")";
        return ss.str();
    }


    FrequencyType::Enum Chances::SelectRandomFrequencyType(const FrequencyChanceMap_t & FREQ_CHANCE_MAP) const
    {
        //Handle case where the map is empty
        //Handle case where there are multiple FrequencyTypes with certain or higher than certain chance.
        //Handle case where chances do not add up exactly to one, or 100%.

        if (FREQ_CHANCE_MAP.empty())
            return FrequencyType::Never;

        //Find chance total of all FrequencyTypes, and make a vec of all that have a certain (one) or higher than certain chance.
        auto totalOfAllFreqTypes{0.0f};
        auto cretainOrHigherChanceTotal{0.0f};
        std::vector<std::pair<FrequencyType::Enum, float>> cretainOrHigherFreqTypeVec;
        for (auto const & NEXT_FREQCHANCE_PAIR : FREQ_CHANCE_MAP)
        {
            totalOfAllFreqTypes += NEXT_FREQCHANCE_PAIR.second;
            if ((NEXT_FREQCHANCE_PAIR.second > 1.0f) || misc::IsRealOne(NEXT_FREQCHANCE_PAIR.second))
            {
                cretainOrHigherChanceTotal += NEXT_FREQCHANCE_PAIR.second;
                cretainOrHigherFreqTypeVec.push_back(NEXT_FREQCHANCE_PAIR);
            }
        }

        if (cretainOrHigherFreqTypeVec.size() == 1)
        {
            //if there is only one FrequencyType that is certain or higher then automatically choose it
            return cretainOrHigherFreqTypeVec[0].first;
        }
        else if (cretainOrHigherFreqTypeVec.size() > 1)
        {
            //if there are multipe FrequencyTypes that are certain or higher then choose randomly amoung them
            auto const RAND( misc::random::Float(0.0f, cretainOrHigherChanceTotal) );
            auto subtotal{ 0.0f };
            for (auto const & NEXT_FREQCHANCE_PAIR : cretainOrHigherFreqTypeVec)
            {
                subtotal += NEXT_FREQCHANCE_PAIR.second;
                if (RAND < subtotal)
                    return NEXT_FREQCHANCE_PAIR.first;
            }

            return cretainOrHigherFreqTypeVec[cretainOrHigherFreqTypeVec.size() - 1].first;
        }
        else
        {
            auto const DO_CHANCES_TOTAL_ONE_OR_LESS{ ((totalOfAllFreqTypes < 1.0f) || misc::IsRealOne(totalOfAllFreqTypes)) };

            //if there are no FrequencyTypes with certain or higher chance,
            //then random select from any with a non-zero chance...
            auto subtotal{ 0.0f };
            auto const RAND{ misc::random::Float(0.0f, ((DO_CHANCES_TOTAL_ONE_OR_LESS) ? 1.0f : totalOfAllFreqTypes)) };

            for (auto const & NEXT_FREQCHANCE_PAIR : FREQ_CHANCE_MAP)
            {
                subtotal += NEXT_FREQCHANCE_PAIR.second;
                if (RAND < subtotal)
                    return NEXT_FREQCHANCE_PAIR.first;
            }

            if (DO_CHANCES_TOTAL_ONE_OR_LESS)
            {
                //...defaulting to 'Never' if none is randomly selected.
                return FrequencyType::Never;
            }
            else
            {
                //...default to first FrequencyType in the list
                return FREQ_CHANCE_MAP.begin()->first;
            }
        }
    }


    std::size_t Chances::SelectRandomNumberContinuous(const OutnumberRetreatChanceMap_t & OUTNUMBER_RETREAT_CHANCE_MAP) const
    {
        if (OUTNUMBER_RETREAT_CHANCE_MAP.empty())
        {
            return 0;
        }
        else if (OUTNUMBER_RETREAT_CHANCE_MAP.size() == 1)
        {
            return OUTNUMBER_RETREAT_CHANCE_MAP.begin()->first;
        }
        else
        {
            //Since the "dist" option can set up to a hundred chances equal to one,
            //restrict the one chance to only occurring only once, the first time.
            auto alreadyFoundChanceOfOne{ false };

            auto total{ 0.0f };
            for (auto const & NEXT_COUNTCHANCE_PAIR : OUTNUMBER_RETREAT_CHANCE_MAP)
            {
                if (misc::IsRealOne(NEXT_COUNTCHANCE_PAIR.second))
                {
                    if (alreadyFoundChanceOfOne)
                    {
                        continue;
                    }
                    else
                    {
                        alreadyFoundChanceOfOne = true;
                    }
                }

                total += NEXT_COUNTCHANCE_PAIR.second;
            }

            auto const RAND{ misc::random::Float(0.0f, total) };

            auto subtotal{ 0.0f };
            for (auto const & NEXT_COUNTCHANCE_PAIR : OUTNUMBER_RETREAT_CHANCE_MAP)
            {
                subtotal += NEXT_COUNTCHANCE_PAIR.second;
                if (RAND < subtotal)
                    return NEXT_COUNTCHANCE_PAIR.first;
            }

            return OUTNUMBER_RETREAT_CHANCE_MAP.begin()->first;
        }
    }


    bool operator<(const Chances & L, const Chances & R)
    {
        return std::tie(L.selectChanceMap_,
                        L.refineChanceMap_,
                        L.advanceChanceMap_,
                        L.roarFreqChanceMap_,
                        L.castFreqChanceMap_,
                        L.flyFreqChanceMap_,
                        L.flyPounceFreqChanceMap_,
                        L.standPounceFreqChanceMap_)
               <
               std::tie(R.selectChanceMap_,
                        R.refineChanceMap_,
                        R.advanceChanceMap_,
                        R.roarFreqChanceMap_,
                        R.castFreqChanceMap_,
                        R.flyFreqChanceMap_,
                        R.flyPounceFreqChanceMap_,
                        R.standPounceFreqChanceMap_);
    }


    bool operator==(const Chances & L, const Chances & R)
    {
        return std::tie(L.selectChanceMap_,
                        L.refineChanceMap_,
                        L.advanceChanceMap_,
                        L.roarFreqChanceMap_,
                        L.castFreqChanceMap_,
                        L.flyFreqChanceMap_,
                        L.flyPounceFreqChanceMap_,
                        L.standPounceFreqChanceMap_)
               ==
               std::tie(R.selectChanceMap_,
                        R.refineChanceMap_,
                        R.advanceChanceMap_,
                        R.roarFreqChanceMap_,
                        R.castFreqChanceMap_,
                        R.flyFreqChanceMap_,
                        R.flyPounceFreqChanceMap_,
                        R.standPounceFreqChanceMap_);
    }


    bool operator!=(const Chances & L, const Chances & R)
    {
        return ! (L == R);
    }
}
}
}