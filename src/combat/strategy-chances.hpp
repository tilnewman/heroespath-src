// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
#define HEROESPATH_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
//
// strategy-chances.hpp
//
#include "combat/strategy-enums.hpp"
#include "combat/strategy-info.hpp"
#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"
#include "misc/vector-map.hpp"

#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        // All the information needed to construct a relatively random StrategyInfo object.
        // Note:  The default for frequency chance maps is 'Never', while the default for all other
        // chance map types is 'Neutral'.
        class Chances
        {
        public:
            explicit Chances(
                const SelectChanceMap_t & SELECT_CHANCES_MAP = SelectChanceMap_t(),
                const RefineChanceMap_t & REFINE_CHANCES_MAP = RefineChanceMap_t(),
                const AdvanceChanceMap_t & ADVANCE_CHANCES_MAP = AdvanceChanceMap_t(),
                const RetreatChanceMap_t & RETREAT_CHANCES_MAP = RetreatChanceMap_t(),
                const OutnumberRetreatChanceMap_t & OUTNUMBER_RETREAT_CHANCES_MAP
                = OutnumberRetreatChanceMap_t(),
                const FrequencyChanceMap_t & ROAR_FREQ_CHANCES_MAP = FrequencyChanceMap_t(),
                const FrequencyChanceMap_t & CAST_FREQ_CHANCES_MAP = FrequencyChanceMap_t(),
                const FrequencyChanceMap_t & FLY_FREQ_CHANCES_MAP = FrequencyChanceMap_t(),
                const FrequencyChanceMap_t & FLY_POUNCE_FREQ_CHANCES_MAP = FrequencyChanceMap_t(),
                const FrequencyChanceMap_t & STAND_POUNCE_FREQ_CHANCES_MAP
                = FrequencyChanceMap_t());

            const SelectChanceMap_t SelectChanceMapCopy() const { return selectChanceMap_; }
            const RefineChanceMap_t RefineChanceMapCopy() const { return refineChanceMap_; }
            const AdvanceChanceMap_t AdvanceChanceMapCopy() const { return advanceChanceMap_; }
            const RetreatChanceMap_t RetreatChanceMapCopy() const { return retreatChanceMap_; }
            const OutnumberRetreatChanceMap_t OutnumberRetreatChanceMapCopy() const
            {
                return outnumberRetreatChanceMap_;
            }
            const FrequencyChanceMap_t RoarFreqChanceMapCopy() const { return roarFreqChanceMap_; }
            const FrequencyChanceMap_t CastFreqChanceMapCopy() const { return castFreqChanceMap_; }
            const FrequencyChanceMap_t FlyFreqChanceMapCopy() const { return flyFreqChanceMap_; }
            const FrequencyChanceMap_t FlyPounceFreqChanceMapCopy() const
            {
                return flyPounceFreqChanceMap_;
            }
            const FrequencyChanceMap_t StandPounceFreqChanceMapCopy() const
            {
                return standPounceFreqChanceMap_;
            }

            const Info Make() const;
            const Chances AdjustCopy(const Chances &) const;
            const std::string ToString() const;

            friend bool operator<(const Chances & L, const Chances & R);
            friend bool operator==(const Chances & L, const Chances & R);

        private:
            template <typename T>
            T SelectRandomTypeEnumDiscrete(const misc::VectorMap<T, float> & ENUM_CHANCE_MAP) const
            {
                // handle case where the map is empty
                if (ENUM_CHANCE_MAP.Empty())
                {
                    return T::None;
                }

                T e{ T::None }; // None is always zero
                for (auto const & NEXT_ENUMCHANCE_PAIR : ENUM_CHANCE_MAP)
                {
                    if (misc::random::Float(0.0f, 1.0f) < NEXT_ENUMCHANCE_PAIR.second)
                    {
                        if (NEXT_ENUMCHANCE_PAIR.first == T::None)
                        {
                            return T::None;
                        }
                        else
                        {
                            e = static_cast<T>(
                                static_cast<unsigned int>(e) | NEXT_ENUMCHANCE_PAIR.first);
                        }
                    }
                }

                return e;
            }

            template <typename T>
            T SelectRandomTypeEnumContinuous(
                const misc::VectorMap<T, float> & ENUM_CHANCE_MAP) const
            {
                if (ENUM_CHANCE_MAP.Empty())
                {
                    return T::None;
                }
                else if (ENUM_CHANCE_MAP.Size() == 1)
                {
                    return ENUM_CHANCE_MAP.begin()->first;
                }
                else
                {
                    auto const TOTAL{ std::accumulate(
                        std::begin(ENUM_CHANCE_MAP),
                        std::end(ENUM_CHANCE_MAP),
                        0.0f,
                        [](auto const SUBTOTAL, auto const & PAIR) {
                            return SUBTOTAL + PAIR.second;
                        }) };

                    auto const RAND{ misc::random::Float(0.0f, TOTAL) };

                    auto subtotal{ 0.0f };
                    for (auto const & NEXT_ENUMCHANCE_PAIR : ENUM_CHANCE_MAP)
                    {
                        subtotal += NEXT_ENUMCHANCE_PAIR.second;
                        if (RAND < subtotal)
                        {
                            return NEXT_ENUMCHANCE_PAIR.first;
                        }
                    }

                    return ENUM_CHANCE_MAP.begin()->first;
                }
            }

            FrequencyType::Enum SelectRandomFrequencyType(const FrequencyChanceMap_t &) const;

            std::size_t SelectRandomNumberContinuous(const OutnumberRetreatChanceMap_t &) const;

            template <typename StructType, typename EnumType>
            const std::string ChanceMapToString(
                const misc::VectorMap<EnumType, float> & CHANCE_MAP,
                const bool WILL_WRAP = false) const
            {
                const std::string SEP(", ");

                std::ostringstream ss;

                if (WILL_WRAP)
                {
                    ss << "(";
                }

                for (auto const & NEXT_TYPECHANCE_PAIR : CHANCE_MAP)
                {
                    ss << StructType::ToString(NEXT_TYPECHANCE_PAIR.first) << ":"
                       << NEXT_TYPECHANCE_PAIR.second << SEP;
                }

                return boost::algorithm::erase_last_copy(ss.str(), SEP) + ((WILL_WRAP) ? ")" : "");
            }

        private:
            SelectChanceMap_t selectChanceMap_;
            RefineChanceMap_t refineChanceMap_;
            AdvanceChanceMap_t advanceChanceMap_;
            RetreatChanceMap_t retreatChanceMap_;
            OutnumberRetreatChanceMap_t outnumberRetreatChanceMap_;
            FrequencyChanceMap_t roarFreqChanceMap_;
            FrequencyChanceMap_t castFreqChanceMap_;
            FrequencyChanceMap_t flyFreqChanceMap_;
            FrequencyChanceMap_t flyPounceFreqChanceMap_;
            FrequencyChanceMap_t standPounceFreqChanceMap_;
        };

        bool operator<(const Chances & L, const Chances & R);

        bool operator==(const Chances & L, const Chances & R);

        inline bool operator!=(const Chances & L, const Chances & R) { return !(L == R); }

    } // namespace strategy
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
