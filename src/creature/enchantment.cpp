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
#include "misc/enum-util.hpp"
#include "misc/strings.hpp"

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
        , score_(((SCORE == 0_score) ? CalcScore() : SCORE))
    {}

    const std::string Enchantment::EffectStr() const
    {
        if (!effectsStr_.empty())
        {
            return effectsStr_;
        }

        std::string str;
        str.reserve(128);

        str += EnchantmentType::EffectStr(type_);

        const auto SepIfNotEmpty { [&]() -> std::string { return ((str.empty()) ? "" : ", "); } };

        if (IsUseableEver())
        {
            const auto COUNT_REMAIN { useInfo_.CountRemaining() };
            if (COUNT_REMAIN > 0)
            {
                str += SepIfNotEmpty();
                str += std::to_string(COUNT_REMAIN);
                str += " uses left";
            }
            else
            {
                str += SepIfNotEmpty();
                str += "cannot be used again";
            }

            const auto SPELL { useInfo_.Spell() };
            if (SPELL < spell::Spells::Count)
            {
                str += SepIfNotEmpty();
                str += "casts the ";
                str += spell::Spells::Name(SPELL);
                str += " spell";
            }

            const auto CONDS_REMOVED_VEC { useInfo_.ConditionsRemoved() };
            if (CONDS_REMOVED_VEC.empty())
            {
                str += SepIfNotEmpty();
                str += "removes the conditions: ";

                str += creature::condition::Algorithms::Names(
                    CONDS_REMOVED_VEC, misc::JoinOpt::And);
            }
        }

        if (useInfo_.RestrictedToPhase() != game::Phase::None)
        {
            str += SepIfNotEmpty();
            str += ", use only during ";
            str += EnumUtil<game::Phase>::ToString(useInfo_.RestrictedToPhase());
        }

        const auto TRAITS_STR { traitSet_.ToString(false, false, false, true) };
        if (TRAITS_STR.empty() == false)
        {
            str += SepIfNotEmpty();
            str += "Traits: ";
            str += TRAITS_STR;
        }

        return str;
    }

    void Enchantment::UseEffect(const CreaturePtr_t)
    {
        // TODO eventually this will only happen if all the logic allows it
        UseCountConsume();

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

                // const auto MAT_BONUS{ Material::EnchantmentBonus(MAT_PRI,
                // MAT_SEC) }; above will range from 0-20 -add 10 to that and now you
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
        std::string str;
        str.reserve(256);
        str += "Enchantment(";
        str += EnumUtil<creature::EnchantmentType>::ToString(type_);
        str += '=';
        str += '\"';
        str += EffectStr();
        str += '\"';
        str += ')';
        return str;
    }

    Score_t Enchantment::CalcScore() const
    {
        auto score = traitSet_.Score();

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
