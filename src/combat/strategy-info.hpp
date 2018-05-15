// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

            SelectType::Enum Select() const { return selectType_; }
            RefineType::Enum Refine() const { return refineType_; }
            AdvanceType::Enum Advance() const { return advanceType_; }
            RetreatType::Enum Retreat() const { return retreatType_; }
            std::size_t OutnumberedRetreatCount() const { return outnumberRetreatCount_; }
            FrequencyType::Enum RoarFreq() const { return roarFreqType_; }
            FrequencyType::Enum CastFreq() const { return castFreqType_; }
            FrequencyType::Enum FlyFreq() const { return flyFreqType_; }
            FrequencyType::Enum FlyPounceFreq() const { return flyPounceFreqType_; }
            FrequencyType::Enum StandPounceFreq() const { return standPounceFreqType_; }

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
    } // namespace strategy
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_STRATEGYINFO_HPP_INCLUDED
