// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enchantment.cpp
//
#include "enchantment.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/creature.hpp"
#include "item/item-score-helper.hpp"

#include <sstream>

namespace heroespath
{
namespace creature
{

    Enchantment::Enchantment(
        const EnchantmentType::Enum TYPE,
        const TraitSet & TRAIT_SET,
        const UseInfo & USE_INFO,
        const std::string & EFFECTS_STR,
        const Score_t SCORE,
        const UseEffectType EFFECT_TYPE)
        : type_(TYPE)
        , traitSet_(TRAIT_SET)
        , useInfo_(USE_INFO)
        , effectsStr_(EFFECTS_STR)
        , effectType_(EFFECT_TYPE)
        , score_(((SCORE == 0_score) ? CalcTreasureScore() : SCORE))
    {}

    const std::string Enchantment::EffectStr() const
    {
        if (effectsStr_.empty() == false)
        {
            return effectsStr_;
        }

        std::ostringstream ss;

        ss << EnchantmentType::ToString(type_);

        const auto SepIfNotEmpty { [](const std::string & S) {
            return ((S.empty()) ? "" : ", ");
        } };

        if (IsUseableEver())
        {
            const auto COUNT_REMAIN { useInfo_.CountRemaining() };
            if (COUNT_REMAIN > 0)
            {
                ss << SepIfNotEmpty(ss.str()) << COUNT_REMAIN << " uses left";
            }
            else if (COUNT_REMAIN == 0)
            {
                ss << SepIfNotEmpty(ss.str()) << "cannot be used again";
            }
            else
            {
                ss << SepIfNotEmpty(ss.str()) << "";
            }

            const auto SPELL { useInfo_.Spell() };
            if (SPELL != spell::Spells::Count)
            {
                ss << SepIfNotEmpty(ss.str()) << "casts the " << spell::Spells::Name(SPELL)
                   << " spell";
            }

            const auto CONDS_REMOVED_VEC { useInfo_.ConditionsRemoved() };
            if (CONDS_REMOVED_VEC.empty())
            {
                ss << SepIfNotEmpty(ss.str()) << "removes the conditions: "
                   << creature::condition::Algorithms::Names(
                          CONDS_REMOVED_VEC, misc::Vector::JoinOpt::And);
            }
        }

        if (useInfo_.RestrictedToPhase() != game::Phase::None)
        {
            ss << SepIfNotEmpty(ss.str()) << ", use only during "
               << game::Phase::ToString(useInfo_.RestrictedToPhase());
        }

        const auto TRAITS_STR { traitSet_.ToString(false, false, false, true) };
        if (TRAITS_STR.empty() == false)
        {
            ss << SepIfNotEmpty(ss.str()) << "Traits: " << TRAITS_STR;
        }

        return ss.str();
    }

    void Enchantment::UseEffect(const CreaturePtr_t)
    {
        switch (effectType_)
        {
            case UseEffectType::PixieBell:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::CrystalChimes:
            {
                /*TODO*/

                // const auto MAT_BONUS{ material::EnchantmentBonus(MATERIAL_PRIMARY,
                // MATERIAL_SECONDARY) }; above will range from 0-20 -add 10 to that and now you
                // have how much mana is restored when used. this enchantment will need to set its
                // own EFFECTS_STR based on these numbers

                break;
            }
            case UseEffectType::GoldenGong:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::DragonToothWhistle:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::DoveBloodVial:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::DriedEdible:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::ShamanRainmaker:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::SpecterChains:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::VultureGizzard:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::WarTrumpet:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::Blessed:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::Cursed:
            {
                /*TODO*/
                break;
            }
            case UseEffectType::None:
            default:
            {
                break;
            }
        }
    }

    const std::string Enchantment::ToString() const
    {
        std::ostringstream ss;
        ss << "Enchantment(name=" << creature::EnchantmentType::ToString(type_) << ", "
           << EffectStr() << ")";

        return ss.str();
    }

    Score_t Enchantment::CalcTreasureScore() const
    {
        item::ScoreHelper scoreHelper;

        auto score { scoreHelper.Score(traitSet_) };

        if (type_ & EnchantmentType::WhenHeld)
        {
            score += 350_score;
        }

        if (type_ & EnchantmentType::AllowsFlight)
        {
            score += 200_score;
        }

        if ((type_ & EnchantmentType::BlessWithoutItem)
            || (type_ & EnchantmentType::CurseWithoutItem))
        {
            score += 300_score;
        }

        return score;
    }

} // namespace creature
} // namespace heroespath
