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
// trait-enum.cpp
//
#include "trait-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace stats
{

    const std::string Traits::ToString(const Enum E)
    {
        switch (E)
        {
            case Strength:
            {
                return "Strength";
            }
            case Accuracy:
            {
                return "Accuracy";
            }
            case Charm:
            {
                return "Charm";
            }
            case Luck:
            {
                return "Luck";
            }
            case Speed:
            {
                return "Speed";
            }
            case Intelligence:
            {
                return "Intelligence";
            }
            case Mana:
            {
                return "Mana";
            }
            case ArmorRating:
            {
                return "ArmorRating";
            }
            case MagicCast:
            {
                return "MagicCast";
            }
            case MagicEffect:
            {
                return "MagicEffect";
            }
            case MagicResist:
            {
                return "MagicResist";
            }
            case Encounter:
            {
                return "Encounter";
            }
            case Surprise:
            {
                return "Surprise";
            }
            case FindReligious:
            {
                return "FindReligious";
            }
            case FindMagic:
            {
                return "FindMagic";
            }
            case FindCoinsAmount:
            {
                return "FindCoinsAmount";
            }
            case Backstab:
            {
                return "Backstab";
            }
            case HealthGainMelee:
            {
                return "HealthGainMelee";
            }
            case HealthGainAll:
            {
                return "HealthGainAll";
            }
            case AnimalResist:
            {
                return "AnimalResist";
            }
            case DamageBonusFist:
            {
                return "DamageBonusFist";
            }
            case DamageBonusMelee:
            {
                return "DamageBonusMelee";
            }
            case DamageBonusProj:
            {
                return "DamageBonusProj";
            }
            case DamageBonusAll:
            {
                return "DamageBonusAll";
            }
            case PoisonOnMelee:
            {
                return "PoisonOnMelee";
            }
            case PoisonOnAll:
            {
                return "PoisonOnAll";
            }
            case PanicOnSpell:
            {
                return "PanicOnSpell";
            }
            case CurseCast:
            {
                return "CurseCast";
            }
            case CurseEffect:
            {
                return "CurseEffect";
            }
            case CurseResist:
            {
                return "CurseResist";
            }
            case CurseOnDamage:
            {
                return "CurseOnDamage";
            }
            case BlessCast:
            {
                return "BlessCast";
            }
            case BlessEffect:
            {
                return "BlessEffect";
            }
            case BlessResist:
            {
                return "BlessResist";
            }
            case HitPower:
            {
                return "HitPower";
            }
            case HitCritical:
            {
                return "HitCritical";
            }
            case FireDamage:
            {
                return "FireDamage";
            }
            case FireResist:
            {
                return "FireResist";
            }
            case FrostDamage:
            {
                return "FrostDamage";
            }
            case FrostResist:
            {
                return "FrostResist";
            }
            case HonorDamage:
            {
                return "HonorDamage";
            }
            case HonorResist:
            {
                return "HonorResist";
            }
            case ShadowDamage:
            {
                return "ShadowDamage";
            }
            case ShadowResist:
            {
                return "ShadowResist";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "stats::Traits::ToString(" << E << ")_InvalidValueError";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string Traits::Abbr(const Enum E)
    {
        switch (E)
        {
            case Strength:
            {
                return "Str";
            }
            case Accuracy:
            {
                return "Acc";
            }
            case Charm:
            {
                return "Cha";
            }
            case Luck:
            {
                return "Lck";
            }
            case Speed:
            {
                return "Spd";
            }
            case Intelligence:
            {
                return "Int";
            }
            case Mana:
            {
                return "Man";
            }
            case ArmorRating:
            {
                return "ArmRat";
            }
            case MagicCast:
            {
                return "MagCst";
            }
            case MagicEffect:
            {
                return "MagEff";
            }
            case MagicResist:
            {
                return "MagRes";
            }
            case Encounter:
            {
                return "Enc";
            }
            case Surprise:
            {
                return "Sur";
            }
            case FindReligious:
            {
                return "FindRel";
            }
            case FindMagic:
            {
                return "FindMag";
            }
            case FindCoinsAmount:
            {
                return "FindCoins";
            }
            case Backstab:
            {
                return "Backstab";
            }
            case HealthGainMelee:
            {
                return "HlthGainMel";
            }
            case HealthGainAll:
            {
                return "HlthGainAll";
            }
            case AnimalResist:
            {
                return "AnimalDmgRes";
            }
            case DamageBonusFist:
            {
                return "DmgFist";
            }
            case DamageBonusMelee:
            {
                return "DmgMel";
            }
            case DamageBonusProj:
            {
                return "DmgPrj";
            }
            case DamageBonusAll:
            {
                return "DmgAll";
            }
            case PoisonOnMelee:
            {
                return "PoiOnMel";
            }
            case PoisonOnAll:
            {
                return "PoiOnAll";
            }
            case PanicOnSpell:
            {
                return "PanicOnSpl";
            }
            case CurseCast:
            {
                return "CurCst";
            }
            case CurseEffect:
            {
                return "CurEff";
            }
            case CurseResist:
            {
                return "CurRes";
            }
            case CurseOnDamage:
            {
                return "CurOnDmg";
            }
            case BlessCast:
            {
                return "BlsCst";
            }
            case BlessEffect:
            {
                return "BlsEff";
            }
            case BlessResist:
            {
                return "BlsRes";
            }
            case HitPower:
            {
                return "HitPower";
            }
            case HitCritical:
            {
                return "HitCri";
            }
            case FireDamage:
            {
                return "FirDmg";
            }
            case FireResist:
            {
                return "FirRes";
            }
            case FrostDamage:
            {
                return "FrtDmg";
            }
            case FrostResist:
            {
                return "FrtRes";
            }
            case HonorDamage:
            {
                return "HnrDmg";
            }
            case HonorResist:
            {
                return "HnrRes";
            }
            case ShadowDamage:
            {
                return "ShaDmg";
            }
            case ShadowResist:
            {
                return "ShaRes";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "stats::Traits::Abbr(" << E << ")_InvalidValueError";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string Traits::Name(const Enum E)
    {
        switch (E)
        {
            case Strength:
            {
                return "Strength Bonus";
            }
            case Accuracy:
            {
                return "Accuracy Bonus";
            }
            case Charm:
            {
                return "Charm Bonus";
            }
            case Luck:
            {
                return "Luck Bonus";
            }
            case Speed:
            {
                return "Speed Bonus";
            }
            case Intelligence:
            {
                return "Intelligence Bonus";
            }
            case Mana:
            {
                return "Mana Bonus";
            }
            case ArmorRating:
            {
                return "Armor Rating Bonus";
            }
            case MagicCast:
            {
                return "Magic/Alchemy Chance";
            }
            case MagicEffect:
            {
                return "Magic Effect";
            }
            case MagicResist:
            {
                return "Magic Resistance";
            }
            case Encounter:
            {
                return "Encounter Chance";
            }
            case Surprise:
            {
                return "Surprise Chance";
            }
            case FindReligious:
            {
                return "Find Religious Items Chance";
            }
            case FindMagic:
            {
                return "Find Magic Items Chance";
            }
            case FindCoinsAmount:
            {
                return "Find Coins Bonus";
            }
            case Backstab:
            {
                return "Backstab Chance";
            }
            case HealthGainMelee:
            {
                return "Health Gained on Melee Damage";
            }
            case HealthGainAll:
            {
                return "Health Gained on All Damage";
            }
            case AnimalResist:
            {
                return "Animal Damage Resistance";
            }
            case DamageBonusFist:
            {
                return "Fist Damage Bonus";
            }
            case DamageBonusMelee:
            {
                return "Melee Damage Bonus";
            }
            case DamageBonusProj:
            {
                return "Projectile Damage Bonus";
            }
            case DamageBonusAll:
            {
                return "Damage Bonus";
            }
            case PoisonOnMelee:
            {
                return "Poison On Melee Hit Chance";
            }
            case PoisonOnAll:
            {
                return "Poison On Hit Chance";
            }
            case PanicOnSpell:
            {
                return "Panic On Spell Hit Chance";
            }
            case CurseCast:
            {
                return "Curse Cast Chance";
            }
            case CurseEffect:
            {
                return "Curse Cast Effect";
            }
            case CurseResist:
            {
                return "Curse Resistance";
            }
            case CurseOnDamage:
            {
                return "Curse On Hit Chance";
            }
            case BlessCast:
            {
                return "Bless Cast Chance";
            }
            case BlessEffect:
            {
                return "Bless Cast Effect";
            }
            case BlessResist:
            {
                return "Bless Resist";
            }
            case HitPower:
            {
                return "Power Hit Chance";
            }
            case HitCritical:
            {
                return "Critical Hit Chance";
            }
            case FireDamage:
            {
                return "Fire Damage Bonus";
            }
            case FireResist:
            {
                return "Fire Resistance";
            }
            case FrostDamage:
            {
                return "Frost Damage Bonus";
            }
            case FrostResist:
            {
                return "Frost Resistance";
            }
            case HonorDamage:
            {
                return "Honor Damage Bonus";
            }
            case HonorResist:
            {
                return "Honor Resistance";
            }
            case ShadowDamage:
            {
                return "Shadow Damage Bonus";
            }
            case ShadowResist:
            {
                return "Shadow Resistance";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "stats::Traits::Name(" << E << ")_InvalidValueError";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string Traits::Desc(const Enum E)
    {
        // TODO
        return Name(E);
    }
} // namespace stats
} // namespace heroespath
