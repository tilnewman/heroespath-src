#ifndef GAME_COMBAT_STRATEGYINFO_HPP_INCLUDED
#define GAME_COMBAT_STRATEGYINFO_HPP_INCLUDED
//
// strategy-info.hpp
//
#include "strategy-enums.hpp"

#include <string>


namespace game
{
namespace combat
{
namespace strategy
{

    class Info
    {
    public:
        explicit Info(const SelectType::Enum    SELECT_TYPE              = SelectType::None,
                      const RefineType::Enum    REFINE_TYPE              = RefineType::None,
                      const AdvanceType::Enum   ADVANCE_TYPE             = AdvanceType::None,
                      const RetreatType::Enum   RETREAT_TYPE             = RetreatType::None,
                      const std::size_t         OUTNUMBER_RETREAT_COUNT  = 0, //zero means never retreat when melee outnumbered
                      const FrequencyType::Enum ROAR_FREQ_TYPE           = FrequencyType::Never,
                      const FrequencyType::Enum CAST_FREQ_TYPE           = FrequencyType::Never,
                      const FrequencyType::Enum FLY_FREQ_TYPE            = FrequencyType::Never,
                      const FrequencyType::Enum FLY_POUNCE_FREQ_TYPE     = FrequencyType::Never,
                      const FrequencyType::Enum STAND_POUNCE_FREQ_TYPE   = FrequencyType::Never);

        inline SelectType::Enum Select() const              { return selectType_; }
        inline RefineType::Enum Refine() const              { return refineType_; }
        inline AdvanceType::Enum Advance() const            { return advanceType_; }
        inline RetreatType::Enum Retreat() const            { return retreatType_; }
        inline std::size_t OutnumberedRetreatCount() const  { return outnumberRetreatCount_; }
        inline FrequencyType::Enum RoarFreq() const         { return roarFreqType_; }
        inline FrequencyType::Enum CastFreq() const         { return castFreqType_; }
        inline FrequencyType::Enum FlyFreq() const          { return flyFreqType_; }
        inline FrequencyType::Enum FlyPounceFreq() const    { return flyPounceFreqType_; }
        inline FrequencyType::Enum StandPounceFreq() const  { return standPounceFreqType_; }

        const std::string ToString() const;

        friend bool operator<(const Info & L, const Info & R);
        friend bool operator==(const Info & L, const Info & R);

    private:
        SelectType::Enum    selectType_;
        RefineType::Enum    refineType_;
        AdvanceType::Enum   advanceType_;
        RetreatType::Enum   retreatType_;
        std::size_t         outnumberRetreatCount_;
        FrequencyType::Enum roarFreqType_;
        FrequencyType::Enum castFreqType_;
        FrequencyType::Enum flyFreqType_;
        FrequencyType::Enum flyPounceFreqType_;
        FrequencyType::Enum standPounceFreqType_;
    };


    bool operator<(const Info & L, const Info & R);
    bool operator==(const Info & L, const Info & R);
    bool operator!=(const Info & L, const Info & R);

}
}
}

#endif //GAME_COMBAT_STRATEGYINFO_HPP_INCLUDED
