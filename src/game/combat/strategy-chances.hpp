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
#ifndef GAME_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
#define GAME_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
//
// strategy-chances.hpp
//
#include "misc/boost-string-includes.hpp"

#include "game/log-macros.hpp"
#include "game/combat/strategy-enums.hpp"
#include "game/combat/strategy-info.hpp"

#include "misc/random.hpp"

#include <string>
#include <map>
#include <vector>
#include <sstream>


namespace game
{
namespace combat
{
namespace strategy
{

    //All the information needed to construct a relatively random StrategyInfo object.
    //Note:  The default for frequency chance maps is 'Never', while the default for all other chance map types is 'Neutral'.
    class Chances
    {
    public:
        explicit Chances(const SelectChanceMap_t &           SELECT_CHANCES_MAP            = SelectChanceMap_t(),
                         const RefineChanceMap_t &           REFINE_CHANCES_MAP            = RefineChanceMap_t(),
                         const AdvanceChanceMap_t &          ADVANCE_CHANCES_MAP           = AdvanceChanceMap_t(),
                         const RetreatChanceMap_t &          RETREAT_CHANCES_MAP           = RetreatChanceMap_t(),
                         const OutnumberRetreatChanceMap_t & OUTNUMBER_RETREAT_CHANCES_MAP = OutnumberRetreatChanceMap_t(),
                         const FrequencyChanceMap_t &        ROAR_FREQ_CHANCES_MAP         = FrequencyChanceMap_t(),
                         const FrequencyChanceMap_t &        CAST_FREQ_CHANCES_MAP         = FrequencyChanceMap_t(),
                         const FrequencyChanceMap_t &        FLY_FREQ_CHANCES_MAP          = FrequencyChanceMap_t(),
                         const FrequencyChanceMap_t &        FLY_POUNCE_FREQ_CHANCES_MAP   = FrequencyChanceMap_t(),
                         const FrequencyChanceMap_t &        STAND_POUNCE_FREQ_CHANCES_MAP = FrequencyChanceMap_t());

        inline const SelectChanceMap_t           SelectChanceMapCopy() const           { return selectChanceMap_; }
        inline const RefineChanceMap_t           RefineChanceMapCopy() const           { return refineChanceMap_; }
        inline const AdvanceChanceMap_t          AdvanceChanceMapCopy() const          { return advanceChanceMap_; }
        inline const RetreatChanceMap_t          RetreatChanceMapCopy() const          { return retreatChanceMap_; }
        inline const OutnumberRetreatChanceMap_t OutnumberRetreatChanceMapCopy() const { return outnumberRetreatChanceMap_; }
        inline const FrequencyChanceMap_t        RoarFreqChanceMapCopy() const         { return roarFreqChanceMap_; }
        inline const FrequencyChanceMap_t        CastFreqChanceMapCopy() const         { return castFreqChanceMap_; }
        inline const FrequencyChanceMap_t        FlyFreqChanceMapCopy() const          { return flyFreqChanceMap_; }
        inline const FrequencyChanceMap_t        FlyPounceFreqChanceMapCopy() const    { return flyPounceFreqChanceMap_; }
        inline const FrequencyChanceMap_t        StandPounceFreqChanceMapCopy() const  { return standPounceFreqChanceMap_; }

        const Info Make() const;
        const Chances AdjustCopy(const Chances &) const;
        const std::string ToString() const;

        friend bool operator<(const Chances & L, const Chances & R);
        friend bool operator==(const Chances & L, const Chances & R);

    private:
        template<typename T>
        T SelectRandomTypeEnumDiscrete(const std::map<T, float> & ENUM_CHANCE_MAP) const
        {
            //handle case where the map is empty
            if (ENUM_CHANCE_MAP.empty())
                return T::None;

            T e{ T::None }; //None is always zero
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
                        e = static_cast<T>(static_cast<unsigned int>(e) | NEXT_ENUMCHANCE_PAIR.first);
                    }
                }
            }

            return e;
        }

        template<typename T>
        T SelectRandomTypeEnumContinuous(const std::map<T, float> & ENUM_CHANCE_MAP) const
        {
            if (ENUM_CHANCE_MAP.empty())
            {
                return T::None;
            }
            else if (ENUM_CHANCE_MAP.size() == 1)
            {
                return ENUM_CHANCE_MAP.begin()->first;
            }
            else
            {
                auto total{ 0.0f };
                for (auto const & NEXT_ENUMCHANCE_PAIR : ENUM_CHANCE_MAP)
                {
                    total += NEXT_ENUMCHANCE_PAIR.second;
                }

                auto const RAND{ misc::random::Float(0.0f, total) };

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
        const std::string ChanceMapToString(const std::map<EnumType, float> & CHANCE_MAP, const bool WILL_WRAP = false) const
        {
            const std::string SEP(", ");

            std::ostringstream ss;

            if (WILL_WRAP)
                ss << "(";

            for (auto const & NEXT_TYPECHANCE_PAIR : CHANCE_MAP)
                ss << StructType::ToString(NEXT_TYPECHANCE_PAIR.first) << ":" << NEXT_TYPECHANCE_PAIR.second << SEP;

            return boost::algorithm::erase_last_copy(ss.str(), SEP) + ((WILL_WRAP) ? ")" : "");
        }

    private:
        SelectChanceMap_t           selectChanceMap_;
        RefineChanceMap_t           refineChanceMap_;
        AdvanceChanceMap_t          advanceChanceMap_;
        RetreatChanceMap_t          retreatChanceMap_;
        OutnumberRetreatChanceMap_t outnumberRetreatChanceMap_;
        FrequencyChanceMap_t        roarFreqChanceMap_;
        FrequencyChanceMap_t        castFreqChanceMap_;
        FrequencyChanceMap_t        flyFreqChanceMap_;
        FrequencyChanceMap_t        flyPounceFreqChanceMap_;
        FrequencyChanceMap_t        standPounceFreqChanceMap_;
    };

    bool operator<(const Chances & L, const Chances & R);

    bool operator==(const Chances & L, const Chances & R);

    inline bool operator!=(const Chances & L, const Chances & R)
    {
        return ! (L == R);
    }

}
}
}

#endif //GAME_COMBAT_STRATEGYCHANCES_HPP_INCLUDED
