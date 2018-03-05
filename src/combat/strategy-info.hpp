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
#ifndef HEROESPATH_COMBAT_STRATEGYINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYINFO_HPP_INCLUDED
//
// strategy-info.hpp
//
#include "strategy-enums.hpp"

#include <string>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        class Info
        {
        public:
            explicit Info(
                const SelectType::Enum SELECT_TYPE = SelectType::None,
                const RefineType::Enum REFINE_TYPE = RefineType::None,
                const AdvanceType::Enum ADVANCE_TYPE = AdvanceType::None,
                const RetreatType::Enum RETREAT_TYPE = RetreatType::None,
                const std::size_t OUTNUMBER_RETREAT_COUNT
                = 0, // zero means never retreat when melee outnumbered
                const FrequencyType::Enum ROAR_FREQ_TYPE = FrequencyType::Never,
                const FrequencyType::Enum CAST_FREQ_TYPE = FrequencyType::Never,
                const FrequencyType::Enum FLY_FREQ_TYPE = FrequencyType::Never,
                const FrequencyType::Enum FLY_POUNCE_FREQ_TYPE = FrequencyType::Never,
                const FrequencyType::Enum STAND_POUNCE_FREQ_TYPE = FrequencyType::Never);

            inline SelectType::Enum Select() const { return selectType_; }
            inline RefineType::Enum Refine() const { return refineType_; }
            inline AdvanceType::Enum Advance() const { return advanceType_; }
            inline RetreatType::Enum Retreat() const { return retreatType_; }
            inline std::size_t OutnumberedRetreatCount() const { return outnumberRetreatCount_; }
            inline FrequencyType::Enum RoarFreq() const { return roarFreqType_; }
            inline FrequencyType::Enum CastFreq() const { return castFreqType_; }
            inline FrequencyType::Enum FlyFreq() const { return flyFreqType_; }
            inline FrequencyType::Enum FlyPounceFreq() const { return flyPounceFreqType_; }
            inline FrequencyType::Enum StandPounceFreq() const { return standPounceFreqType_; }

            const std::string ToString() const;

            friend bool operator<(const Info & L, const Info & R);
            friend bool operator==(const Info & L, const Info & R);

        private:
            SelectType::Enum selectType_;
            RefineType::Enum refineType_;
            AdvanceType::Enum advanceType_;
            RetreatType::Enum retreatType_;
            std::size_t outnumberRetreatCount_;
            FrequencyType::Enum roarFreqType_;
            FrequencyType::Enum castFreqType_;
            FrequencyType::Enum flyFreqType_;
            FrequencyType::Enum flyPounceFreqType_;
            FrequencyType::Enum standPounceFreqType_;
        };

        bool operator<(const Info & L, const Info & R);

        bool operator==(const Info & L, const Info & R);

        inline bool operator!=(const Info & L, const Info & R) { return !(L == R); }
    }
}
}

#endif // HEROESPATH_COMBAT_STRATEGYINFO_HPP_INCLUDED
