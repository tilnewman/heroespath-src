// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
//
// trait-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{

    // Defines one effect an Enchantment can have.
    struct Traits : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            // Keep stats starting at zero and in order.
            Strength = 0,
            Accuracy,
            Charm,
            Luck,
            Speed,
            Intelligence,
            Mana,
            ArmorRating,
            MagicCast,
            MagicEffect,
            MagicResist,
            Encounter,
            Surprise,
            FindReligious,
            FindMagic,
            FindCoinsAmount,
            Backstab,
            HealthGainMelee,
            HealthGainAll,
            AnimalResist,
            DamageBonusFist,
            DamageBonusMelee,
            DamageBonusProj,
            DamageBonusAll,
            PoisonOnMelee,
            PoisonOnAll,
            PanicOnSpell,
            CurseCast,
            CurseEffect,
            CurseResist,
            CurseOnDamage,
            BlessCast,
            BlessEffect,
            BlessResist,
            HitPower,
            HitCritical,
            FireDamage,
            FireResist,
            FrostDamage,
            FrostResist,
            HonorDamage,
            HonorResist,
            ShadowDamage,
            ShadowResist,
            Count
        };

        static constexpr Enum StatCount = ArmorRating;

        static constexpr std::string_view Abbr(const Enum TRAIT) noexcept
        {
            switch (TRAIT)
            {
                case Strength: return "Str";
                case Accuracy: return "Acc";
                case Charm: return "Cha";
                case Luck: return "Lck";
                case Speed: return "Spd";
                case Intelligence: return "Int";
                case Mana: return "Man";
                case ArmorRating: return "ArmRat";
                case MagicCast: return "MagCst";
                case MagicEffect: return "MagEff";
                case MagicResist: return "MagRes";
                case Encounter: return "Enc";
                case Surprise: return "Sur";
                case FindReligious: return "FindRel";
                case FindMagic: return "FindMag";
                case FindCoinsAmount: return "FindCoins";
                case Backstab: return "Backstab";
                case HealthGainMelee: return "HlthGainMel";
                case HealthGainAll: return "HlthGainAll";
                case AnimalResist: return "AnimalDmgRes";
                case DamageBonusFist: return "DmgFist";
                case DamageBonusMelee: return "DmgMel";
                case DamageBonusProj: return "DmgPrj";
                case DamageBonusAll: return "DmgAll";
                case PoisonOnMelee: return "PoiOnMel";
                case PoisonOnAll: return "PoiOnAll";
                case PanicOnSpell: return "PanicOnSpl";
                case CurseCast: return "CurCst";
                case CurseEffect: return "CurEff";
                case CurseResist: return "CurRes";
                case CurseOnDamage: return "CurOnDmg";
                case BlessCast: return "BlsCst";
                case BlessEffect: return "BlsEff";
                case BlessResist: return "BlsRes";
                case HitPower: return "HitPower";
                case HitCritical: return "HitCri";
                case FireDamage: return "FirDmg";
                case FireResist: return "FirRes";
                case FrostDamage: return "FrtDmg";
                case FrostResist: return "FrtRes";
                case HonorDamage: return "HnrDmg";
                case HonorResist: return "HnrRes";
                case ShadowDamage: return "ShaDmg";
                case ShadowResist: return "ShaRes";
                case Count: return "Count";
                default: return "creature::Traits::Abbr(ENUM=out_of_bounds)";
            }
        }

        static constexpr std::string_view Name(const Enum TRAIT) noexcept
        {
            switch (TRAIT)
            {
                case Strength: return "Strength Bonus";
                case Accuracy: return "Accuracy Bonus";
                case Charm: return "Charm Bonus";
                case Luck: return "Luck Bonus";
                case Speed: return "Speed Bonus";
                case Intelligence: return "Intelligence Bonus";
                case Mana: return "Mana Bonus";
                case ArmorRating: return "Armor Rating Bonus";
                case MagicCast: return "Magic/Alchemy Chance";
                case MagicEffect: return "Magic Effect";
                case MagicResist: return "Magic Resistance";
                case Encounter: return "Encounter Chance";
                case Surprise: return "Surprise Chance";
                case FindReligious: return "Find Religious Items Chance";
                case FindMagic: return "Find Magic Items Chance";
                case FindCoinsAmount: return "Find Coins Bonus";
                case Backstab: return "Backstab Chance";
                case HealthGainMelee: return "Health Gained on Melee Damage";
                case HealthGainAll: return "Health Gained on All Damage";
                case AnimalResist: return "Animal Damage Resistance";
                case DamageBonusFist: return "Fist Damage Bonus";
                case DamageBonusMelee: return "Melee Damage Bonus";
                case DamageBonusProj: return "Projectile Damage Bonus";
                case DamageBonusAll: return "Damage Bonus";
                case PoisonOnMelee: return "Poison On Melee Hit Chance";
                case PoisonOnAll: return "Poison On Hit Chance";
                case PanicOnSpell: return "Panic On Spell Hit Chance";
                case CurseCast: return "Curse Cast Chance";
                case CurseEffect: return "Curse Cast Effect";
                case CurseResist: return "Curse Resistance";
                case CurseOnDamage: return "Curse On Hit Chance";
                case BlessCast: return "Bless Cast Chance";
                case BlessEffect: return "Bless Cast Effect";
                case BlessResist: return "Bless Resist";
                case HitPower: return "Power Hit Chance";
                case HitCritical: return "Critical Hit Chance";
                case FireDamage: return "Fire Damage Bonus";
                case FireResist: return "Fire Resistance";
                case FrostDamage: return "Frost Damage Bonus";
                case FrostResist: return "Frost Resistance";
                case HonorDamage: return "Honor Damage Bonus";
                case HonorResist: return "Honor Resistance";
                case ShadowDamage: return "Shadow Damage Bonus";
                case ShadowResist: return "Shadow Resistance";
                case Count: return "Count";
                default: return "creature::Traits::Name(ENUM=out_of_bounds)";
            }
        }

        static constexpr std::string_view Desc(const Enum) { return "TODO"; }
    };

    using TraitsVec_t = std::vector<Traits::Enum>;
    using TraitValuePair_t = std::pair<Traits::Enum, int>;
    using TraitValueVec_t = std::vector<TraitValuePair_t>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TRAIT_ENUM_HPP_INCLUDED
