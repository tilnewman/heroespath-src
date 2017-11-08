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
// strategy-info.cpp
//
#include "strategy-info.hpp"

#include <sstream>
#include <tuple>
#include <string>


namespace heroespath
{
namespace combat
{
namespace strategy
{

    Info::Info(const SelectType::Enum    SELECT_TYPE,
               const RefineType::Enum    REFINE_TYPE,
               const AdvanceType::Enum   ADVANCE_TYPE,
               const RetreatType::Enum   RETREAT_TYPE,
               const std::size_t         OUTNUMBER_RETREAT_COUNT,
               const FrequencyType::Enum ROAR_FREQ_TYPE,
               const FrequencyType::Enum CAST_FREQ_TYPE,
               const FrequencyType::Enum FLY_FREQ_TYPE,
               const FrequencyType::Enum FLY_POUNCE_FREQ_TYPE,
               const FrequencyType::Enum STAND_POUNCE_FREQ_TYPE)
    :
        selectType_           (SELECT_TYPE),
        refineType_           (REFINE_TYPE),
        advanceType_          (ADVANCE_TYPE),
        retreatType_          (RETREAT_TYPE),
        outnumberRetreatCount_(OUTNUMBER_RETREAT_COUNT),
        roarFreqType_         (ROAR_FREQ_TYPE),
        castFreqType_         (CAST_FREQ_TYPE),
        flyFreqType_          (FLY_FREQ_TYPE),
        flyPounceFreqType_    (FLY_POUNCE_FREQ_TYPE),
        standPounceFreqType_  (STAND_POUNCE_FREQ_TYPE)
    {}


    const std::string Info::ToString() const
    {
        std::ostringstream ss;

        ss  << "StrategyInfo:"
            << " Select=" << SelectType::ToString(selectType_, true)
            << ", Refine=" << RefineType::ToString(refineType_, true)
            << ", Advance=" << AdvanceType::ToString(advanceType_)
            << ", Retreat=" << RetreatType::ToString(retreatType_)
            << ", RetreatCount=" << outnumberRetreatCount_
            << ", Roar=" << FrequencyType::ToString(roarFreqType_)
            << ", Cast=" << FrequencyType::ToString(castFreqType_)
            << ", Fly=" << FrequencyType::ToString(flyFreqType_)
            << ", FlyPounce=" << FrequencyType::ToString(flyPounceFreqType_)
            << ", StandPounce=" << FrequencyType::ToString(standPounceFreqType_);

        return ss.str();
    }


    bool operator<(const Info & L, const Info & R)
    {
        return std::tie(L.selectType_,
                        L.refineType_,
                        L.advanceType_,
                        L.retreatType_,
                        L.outnumberRetreatCount_,
                        L.roarFreqType_,
                        L.castFreqType_,
                        L.flyFreqType_,
                        L.flyPounceFreqType_,
                        L.standPounceFreqType_)
               <
               std::tie(R.selectType_,
                        R.refineType_,
                        R.advanceType_,
                        R.retreatType_,
                        R.outnumberRetreatCount_,
                        R.roarFreqType_,
                        R.castFreqType_,
                        R.flyFreqType_,
                        R.flyPounceFreqType_,
                        R.standPounceFreqType_);
    }


    bool operator==(const Info & L, const Info & R)
    {
        return std::tie(L.selectType_,
                        L.refineType_,
                        L.advanceType_,
                        L.retreatType_,
                        L.outnumberRetreatCount_,
                        L.roarFreqType_,
                        L.castFreqType_,
                        L.flyFreqType_,
                        L.flyPounceFreqType_,
                        L.standPounceFreqType_)
               ==
               std::tie(R.selectType_,
                        R.refineType_,
                        R.advanceType_,
                        R.retreatType_,
                        R.outnumberRetreatCount_,
                        R.roarFreqType_,
                        R.castFreqType_,
                        R.flyFreqType_,
                        R.flyPounceFreqType_,
                        R.standPounceFreqType_);
    }

}
}
}
