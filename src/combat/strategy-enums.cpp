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
// strategy-enums.cpp
//
#include "strategy-enums.hpp"

#include "misc/boost-string-includes.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace combat
{
    namespace strategy
    {

        const std::string SelectType::ToString(const SelectType::Enum E, const bool WILL_WRAP)
        {
            std::ostringstream ss;

            if (E == SelectType::None)
            {
                ss << "None";
            }
            else
            {
                if (E & SelectType::Pixie)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Pixie";
                if (E & SelectType::Dragon)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Dragon";
                if (E & SelectType::Human)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Human";
                if (E & SelectType::Gnome)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Gnome";
                if (E & SelectType::Wolfen)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Wolfen";
                if (E & SelectType::Archer)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Archer";
                if (E & SelectType::Sorcerer)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Sorcerer";
                if (E & SelectType::Knight)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Knight";
                if (E & SelectType::Beastmaster)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Beastmaster";
                if (E & SelectType::Cleric)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Cleric";
                if (E & SelectType::Theif)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Theif";
                if (E & SelectType::Bard)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Bard";
                if (E & SelectType::FireBrand)
                    ss << ((ss.str().empty()) ? "" : ", ") << "FireBrand";
                if (E & SelectType::Sylavin)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Sylavin";
                if (E & SelectType::Projectile)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Projectile";
                if (E & SelectType::Caster)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Caster";
                if (E & SelectType::CanFly)
                    ss << ((ss.str().empty()) ? "" : ", ") << "CanFly";
                if (E & SelectType::CantFly)
                    ss << ((ss.str().empty()) ? "" : ", ") << "CantFly";
                if (E & SelectType::Beast)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Beast";
                if (E & SelectType::NotBeast)
                    ss << ((ss.str().empty()) ? "" : ", ") << "NotBeast";
            }

            if (ss.str().empty())
            {
                std::ostringstream exxceptionSS;
                exxceptionSS << "combat::strategy::SelectType::ToString(" << E
                             << ")_InvalidValueError.";
                throw std::runtime_error(exxceptionSS.str());
            }

            if (WILL_WRAP)
            {
                return "(" + ss.str() + ")";
            }
            else
            {
                return ss.str();
            }
        }

        SelectType::Enum SelectType::FromString(const std::string & S)
        {
            if (boost::algorithm::icontains(S, ToString(None)))
            {
                return None;
            }
            else
            {
                unsigned int i(0);
                if (boost::algorithm::icontains(S, ToString(Pixie)))
                    i = i | Pixie;
                if (boost::algorithm::icontains(S, ToString(Dragon)))
                    i = i | Dragon;
                if (boost::algorithm::icontains(S, ToString(Human)))
                    i = i | Human;
                if (boost::algorithm::icontains(S, ToString(Gnome)))
                    i = i | Gnome;
                if (boost::algorithm::icontains(S, ToString(Wolfen)))
                    i = i | Wolfen;
                if (boost::algorithm::icontains(S, ToString(Archer)))
                    i = i | Archer;
                if (boost::algorithm::icontains(S, ToString(Sorcerer)))
                    i = i | Sorcerer;
                if (boost::algorithm::icontains(S, ToString(Knight)))
                    i = i | Knight;
                if (boost::algorithm::icontains(S, ToString(Beastmaster)))
                    i = i | Beastmaster;
                if (boost::algorithm::icontains(S, ToString(Cleric)))
                    i = i | Cleric;
                if (boost::algorithm::icontains(S, ToString(Theif)))
                    i = i | Theif;
                if (boost::algorithm::icontains(S, ToString(Bard)))
                    i = i | Bard;
                if (boost::algorithm::icontains(S, ToString(FireBrand)))
                    i = i | FireBrand;
                if (boost::algorithm::icontains(S, ToString(Sylavin)))
                    i = i | Sylavin;
                if (boost::algorithm::icontains(S, ToString(Projectile)))
                    i = i | Projectile;
                if (boost::algorithm::icontains(S, ToString(Caster)))
                    i = i | Caster;
                if (boost::algorithm::icontains(S, ToString(CanFly)))
                    i = i | CanFly;
                if (boost::algorithm::icontains(S, ToString(CantFly)))
                    i = i | CantFly;
                if (boost::algorithm::icontains(S, ToString(Beast)))
                    i = i | Beast;
                if (boost::algorithm::icontains(S, ToString(NotBeast)))
                    i = i | NotBeast;

                if (0 == i)
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::SelectType::FromString(\"" << S
                       << "\")_InvalidValueError.";
                    throw std::runtime_error(ss.str());
                }
                else
                {
                    return static_cast<Enum>(i);
                }
            }
        }

        const std::string RefineType::ToString(const RefineType::Enum E, const bool WILL_WRAP)
        {
            std::ostringstream ss;

            if (E == None)
            {
                ss << "None";
            }
            else
            {
                if (E & Murderer)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Murderer";
                if (E & Bloodthirsty)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Bloodthirsty";
                if (E & Coward)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Coward";
                if (E & Hit)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Hit";
                if (E & Attack)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Attack";
                if (E & MusicMaker)
                    ss << ((ss.str().empty()) ? "" : ", ") << "MusicMaker";
                if (E & Caster)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Caster";
                if (E & Enchanted)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Enchanted";
                if (E & NotEnchanted)
                    ss << ((ss.str().empty()) ? "" : ", ") << "NotEnchanted";
                if (E & Steadfast)
                    ss << ((ss.str().empty()) ? "" : ", ") << "Steadfast";
                if (E & LastTo)
                    ss << ((ss.str().empty()) ? "" : ", ") << "LastTo";
                if (E & MostDamage)
                    ss << ((ss.str().empty()) ? "" : ", ") << "MostDamage";
            }

            if (ss.str().empty())
            {
                std::ostringstream exceptionSS;
                exceptionSS << "combat::strategy::RefineType::ToString(" << E
                            << ")_InvalidValueError.";
                throw std::runtime_error(exceptionSS.str());
            }

            if (WILL_WRAP)
            {
                return "(" + ss.str() + ")";
            }
            else
            {
                return ss.str();
            }
        }

        RefineType::Enum RefineType::FromString(const std::string & S)
        {
            if (boost::algorithm::icontains(S, ToString(None)))
            {
                return None;
            }
            else
            {
                unsigned int i(0);
                if (boost::algorithm::icontains(S, ToString(Murderer)))
                    i = i | Murderer;
                if (boost::algorithm::icontains(S, ToString(Bloodthirsty)))
                    i = i | Bloodthirsty;
                if (boost::algorithm::icontains(S, ToString(Coward)))
                    i = i | Coward;
                if (boost::algorithm::icontains(S, ToString(Hit)))
                    i = i | Hit;
                if (boost::algorithm::icontains(S, ToString(Attack)))
                    i = i | Attack;
                if (boost::algorithm::icontains(S, ToString(MusicMaker)))
                    i = i | MusicMaker;
                if (boost::algorithm::icontains(S, ToString(Caster)))
                    i = i | Caster;
                if (boost::algorithm::icontains(S, ToString(Enchanted)))
                    i = i | Enchanted;
                if (boost::algorithm::icontains(S, ToString(NotEnchanted)))
                    i = i | NotEnchanted;
                if (boost::algorithm::icontains(S, ToString(Steadfast)))
                    i = i | Steadfast;
                if (boost::algorithm::icontains(S, ToString(LastTo)))
                    i = i | LastTo;
                if (boost::algorithm::icontains(S, ToString(MostDamage)))
                    i = i | MostDamage;

                if (0 == i)
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::RefineType::FromString(\"" << S
                       << "\")_InvalidValueError.";
                    throw std::runtime_error(ss.str());
                }
                else
                {
                    return static_cast<Enum>(i);
                }
            }
        }

        const std::string AdvanceType::ToString(const AdvanceType::Enum E)
        {
            switch (E)
            {
                case None:
                {
                    return "None";
                }
                case Cowardly:
                {
                    return "Cowardly";
                }
                case Hesitant:
                {
                    return "Hesitant";
                }
                case Charger:
                {
                    return "Charger";
                }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::AdvanceType::ToString(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

        AdvanceType::Enum AdvanceType::FromString(const std::string & S)
        {
            if (boost::algorithm::iequals(S, ToString(None)))
                return None;
            else if (boost::algorithm::iequals(S, ToString(Cowardly)))
                return Cowardly;
            else if (boost::algorithm::iequals(S, ToString(Hesitant)))
                return Hesitant;
            else if (boost::algorithm::iequals(S, ToString(Charger)))
                return Charger;
            else
            {
                std::ostringstream ss;
                ss << "combat::strategy::AdvanceType::FromString(\"" << S
                   << "\")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }

        const std::string RetreatType::ToString(const Enum E)
        {
            switch (E)
            {
                case None:
                {
                    return "None";
                }
                case Wary:
                {
                    return "Wary";
                }
                case Coward:
                {
                    return "Coward";
                }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::RetreatType::ToString(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

        RetreatType::Enum RetreatType::FromString(const std::string & S)
        {
            if (boost::algorithm::iequals(S, ToString(None)))
                return None;
            else if (boost::algorithm::iequals(S, ToString(Wary)))
                return Wary;
            else if (boost::algorithm::iequals(S, ToString(Coward)))
                return Coward;
            else
            {
                std::ostringstream ss;
                ss << "combat::strategy::RetreatType::FromString(\"" << S
                   << "\")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }

        const std::string FrequencyType::ToString(const FrequencyType::Enum E)
        {
            switch (E)
            {
                case Never:
                {
                    return "Never";
                }
                case Once:
                {
                    return "Once";
                }
                case Twice:
                {
                    return "Twice";
                }
                case Thrice:
                {
                    return "Thrice";
                }
                case Rarely:
                {
                    return "Rarely";
                }
                case Seldom:
                {
                    return "Seldom";
                }
                case Neutral:
                {
                    return "Neutral";
                }
                case Commonly:
                {
                    return "Commonly";
                }
                case Often:
                {
                    return "Often";
                }
                case Always:
                {
                    return "Always";
                }
                case Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "combat::strategy::FrequencyType::ToString(" << E
                       << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

        FrequencyType::Enum FrequencyType::FromString(const std::string & S)
        {
            if (boost::algorithm::iequals(S, ToString(Never)))
                return Never;
            else if (boost::algorithm::iequals(S, ToString(Once)))
                return Once;
            else if (boost::algorithm::iequals(S, ToString(Twice)))
                return Twice;
            else if (boost::algorithm::iequals(S, ToString(Thrice)))
                return Thrice;
            else if (boost::algorithm::iequals(S, ToString(Rarely)))
                return Rarely;
            else if (boost::algorithm::iequals(S, ToString(Seldom)))
                return Seldom;
            else if (boost::algorithm::iequals(S, ToString(Neutral)))
                return Neutral;
            else if (boost::algorithm::iequals(S, ToString(Commonly)))
                return Commonly;
            else if (boost::algorithm::iequals(S, ToString(Often)))
                return Often;
            else if (boost::algorithm::iequals(S, ToString(Always)))
                return Always;
            else
            {
                std::ostringstream ss;
                ss << "combat::strategy::FrequencyType::FromString(\"" << S
                   << "\")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }
}
}
