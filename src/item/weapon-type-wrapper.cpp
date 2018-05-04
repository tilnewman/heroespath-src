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
// weapon-type-wrapper.cpp
//
#include "weapon-type-wrapper.hpp"

#include "log/log-macros.hpp"
#include "misc/vector-map.hpp"

#include <exception>
#include <numeric>
#include <vector>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        const std::string WeaponTypeWrapper::DAGGER_NAME_{ "Dagger" };
        const std::string WeaponTypeWrapper::QUARTERSTAFF_NAME_{ "Quarterstaff" };
        const std::string WeaponTypeWrapper::BOW_GENERAL_NAME_{ "Bow" };

        WeaponTypeWrapper::WeaponTypeWrapper(
            const std::string & SYSTEM_NAME, const sfml_util::Size::Enum KNIFE_OR_DAGGER_SIZE)
            : generalName_("")
            , specificName_("")
            , systemName_(SYSTEM_NAME)
            , readableName_("")
            , type_(weapon_type::NotAWeapon)
            , isDagger_(false)
            , variant_()
            , elementTypes_()
        {
            // This is the default constructor and it needs to support construction with all default
            // parameters that leave the object in an invalid state, so this if/return allows for
            // that.
            if (SYSTEM_NAME.empty())
            {
                return;
            }

            auto const SYSTEM_NAME_LOWERCASE{ boost::algorithm::to_lower_copy(SYSTEM_NAME) };

            if (SYSTEM_NAME_LOWERCASE == boost::algorithm::to_lower_copy(QUARTERSTAFF_NAME_))
            {
                type_ = weapon_type::Staff;
                variant_ = true;
                SetNamesAndVerify("'name' constructor when name=Quarterstaff");
            }
            else if (
                SYSTEM_NAME_LOWERCASE
                == boost::algorithm::to_lower_copy(weapon_type::Name(weapon_type::Staff)))
            {
                type_ = weapon_type::Staff;
                variant_ = false;
                SetNamesAndVerify("'name' constructor when name=Staff");
            }
            else if (
                false == SetupWithKnifeOrDaggerName(SYSTEM_NAME_LOWERCASE, KNIFE_OR_DAGGER_SIZE))
            {
                if (false
                    == SetupWithSpecificTypeName<sword_type>(
                           SYSTEM_NAME_LOWERCASE, weapon_type::Sword))
                {
                    if (false
                        == SetupWithSpecificTypeName<axe_type>(
                               SYSTEM_NAME_LOWERCASE, weapon_type::Axe))
                    {
                        if (false
                            == SetupWithSpecificTypeName<club_type>(
                                   SYSTEM_NAME_LOWERCASE, weapon_type::Club))
                        {
                            if (false
                                == SetupWithSpecificTypeName<whip_type>(
                                       SYSTEM_NAME_LOWERCASE, weapon_type::Whip))
                            {
                                if (false == SetupWithBodypartName(SYSTEM_NAME_LOWERCASE))
                                {
                                    if (false
                                        == SetupWithSpecificTypeName<projectile_type>(
                                               SYSTEM_NAME_LOWERCASE, weapon_type::Projectile))
                                    {
                                        SetupWithSpecificTypeName<bladedstaff_type>(
                                            SYSTEM_NAME_LOWERCASE, weapon_type::BladedStaff);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            std::ostringstream errorSS;
            errorSS << "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(name=\"" << SYSTEM_NAME
                    << "\", size=\""
                    << ((KNIFE_OR_DAGGER_SIZE == sfml_util::Size::Count)
                            ? "Count"
                            : sfml_util::Size::ToString(KNIFE_OR_DAGGER_SIZE))
                    << "\") ";

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValidCompleteCheck()),
                errorSS.str() << " after this 'name' constructor the object was invalid: "
                              << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bool IS_DAGGER, const sfml_util::Size::Enum SIZE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Knife)
            , isDagger_(IS_DAGGER)
            , variant_(SIZE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (SIZE != sfml_util::Size::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(is_dagger="
                    << std::boolalpha << IS_DAGGER << ", size=Count) -size can't be Count");

            SetNamesAndVerify("after is_dagger constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bool IS_QUARTERSTAFF)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Staff)
            , isDagger_(false)
            , variant_(IS_QUARTERSTAFF)
            , elementTypes_()
        {
            SetNamesAndVerify("after is_quarterstaff constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const item::body_part::Enum BODY_PART)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(body_part::WeaponType(BODY_PART))
            , isDagger_(false)
            , variant_(BODY_PART)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                ((BODY_PART != body_part::Count) && (BODY_PART != body_part::Skin)),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(body_part="
                    << ((body_part::Count == BODY_PART) ? "Count" : body_part::ToString(BODY_PART))
                    << ") -that body_part is invalid.");

            SetNamesAndVerify("after body_part constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const sword_type::Enum SWORD_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Sword)
            , isDagger_(false)
            , variant_(SWORD_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (SWORD_TYPE != sword_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(sword_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after sword_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const axe_type::Enum AXE_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Axe)
            , isDagger_(false)
            , variant_(AXE_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (AXE_TYPE != axe_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(axe_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after axe_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const club_type::Enum CLUB_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Club)
            , isDagger_(false)
            , variant_(CLUB_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (CLUB_TYPE != club_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(club_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after club_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const whip_type::Enum WHIP_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Whip)
            , isDagger_(false)
            , variant_(WHIP_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (WHIP_TYPE != whip_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(whip_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after whip_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const projectile_type::Enum PROJ_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Projectile)
            , isDagger_(false)
            , variant_(PROJ_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (PROJ_TYPE != projectile_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(projectile_type=Count) -Count "
                "is invalid.");

            SetNamesAndVerify("after projectile_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bladedstaff_type::Enum BSTAFF_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::BladedStaff)
            , isDagger_(false)
            , variant_(BSTAFF_TYPE)
            , elementTypes_()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (BSTAFF_TYPE != bladedstaff_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(bladedstaff_type=Count) -Count "
                "is invalid.");

            SetNamesAndVerify("after bladedstaff_type constructor");
        }

        const std::string WeaponTypeWrapper::DetailsKeyName() const
        {
            return boost::algorithm::replace_all_copy(specificName_, " ", "");
        }

        const std::string WeaponTypeWrapper::ToString() const
        {
            if (*this == WeaponTypeWrapper())
            {
                return "(empty)";
            }

            misc::VectorMap<std::string, std::string> namesMap;
            namesMap[generalName_] += "generalName_\\";
            namesMap[specificName_] += "specificName_\\";
            namesMap[systemName_] += "systemName_\\";
            namesMap[readableName_] += "readableName_\\";

            std::ostringstream ss;
            for (auto const & VALUE_NAME_PAIR : namesMap)
            {
                ss << "\"" << VALUE_NAME_PAIR.first << "\"=" << VALUE_NAME_PAIR.second << ", ";
            }

            ss << "type_=" << weapon_type::ToString(type_, true) << ", is_dagger=" << std::boolalpha
               << isDagger_ << ", variant_.which()=" << variant_.which() << ","
               << ((IsQuarterstaff()) ? QUARTERSTAFF_NAME_ : "") << ",type=("
               << ((IsBite()) ? "bite" : "") << "," << ((IsClaws()) ? "claws" : "") << ","
               << ((IsFists()) ? "fists" : "") << "," << ((IsStaff()) ? "staff" : "") << ","
               << ((IsKnife()) ? "knife" : "") << "," << ((IsDagger()) ? "dagger" : "") << ","
               << ((IsTendrils()) ? "tendrils" : "") << "," << ((IsBreath()) ? "breath" : "") << ","
               << ((IsSword()) ? sword_type::ToString(SwordType()) : "") << ","
               << ((IsAxe()) ? axe_type::ToString(AxeType()) : "") << ","
               << ((IsClub()) ? club_type::ToString(ClubType()) : "") << ","
               << ((IsWhip()) ? whip_type::ToString(WhipType()) : "") << ","
               << ((IsProjectile()) ? projectile_type::ToString(ProjectileType()) : "") << ","
               << ((IsBladedStaff()) ? bladedstaff_type::ToString(BladedStaffType()) : "")
               << "), element_types={";

            if (elementTypes_.empty())
            {
                ss << "empty/invalid";
            }
            else
            {
                for (std::size_t i(0); i < elementTypes_.size(); ++i)
                {
                    if (i > 0)
                    {
                        ss << ",";
                    }

                    auto const ELEMENT_TYPE{ elementTypes_.at(i) };

                    if (ELEMENT_TYPE == element_type::None)
                    {
                        ss << "(None)";
                    }
                    else
                    {
                        ss << element_type::ToString(ELEMENT_TYPE, true, "&");
                    }
                }
            }

            ss << "}";

            return ss.str();
        }

        const std::string WeaponTypeWrapper::ImageFilename(
            const std::string & SEPARATOR, const std::string & EXTENSION) const
        {
            std::ostringstream ss;

            // remove any spaces from the generalName_
            auto const GENERAL_NAME_FILENAME_VERSION{ boost::algorithm::replace_all_copy(
                generalName_, " ", "") };

            ss << GENERAL_NAME_FILENAME_VERSION;

            if ((systemName_ != GENERAL_NAME_FILENAME_VERSION) && (systemName_ != "Bite"))
            {
                ss << SEPARATOR << systemName_;
            }

            ss << EXTENSION;
            return boost::algorithm::to_lower_copy(ss.str());
        }

        bool WeaponTypeWrapper::IsGeneralNameAlmostSpecificName() const
        {
            namespace ba = boost::algorithm;

            return (
                ba::to_lower_copy(ba::replace_all_copy(generalName_, " ", ""))
                == ba::to_lower_copy(specificName_));
        }

        const std::vector<WeaponTypeWrapper> WeaponTypeWrapper::MakeCompleteSet()
        {
            std::vector<WeaponTypeWrapper> wrappers;

            wrappers.emplace_back(WeaponTypeWrapper(false));
            wrappers.emplace_back(WeaponTypeWrapper(true));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Bite));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Claws));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Fists));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Tendrils));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Breath));

            for (int i(0); i < sfml_util::Size::Count; ++i)
            {
                auto const SIZE_ENUM{ static_cast<sfml_util::Size::Enum>(i) };
                wrappers.emplace_back(WeaponTypeWrapper(false, SIZE_ENUM));
                wrappers.emplace_back(WeaponTypeWrapper(true, SIZE_ENUM));
            }

            AppendSpecificSet<sword_type>(wrappers);
            AppendSpecificSet<axe_type>(wrappers);
            AppendSpecificSet<club_type>(wrappers);
            AppendSpecificSet<whip_type>(wrappers);
            AppendSpecificSet<projectile_type>(wrappers);
            AppendSpecificSet<bladedstaff_type>(wrappers);

            return wrappers;
        }

        bool WeaponTypeWrapper::SetupWithBodypartName(const std::string & SYSTEM_NAME_LOWERCASE)
        {
            for (int i(0); i < body_part::Count; ++i)
            {
                auto const BODY_PART_ENUM{ static_cast<body_part::Enum>(i) };

                if (boost::algorithm::to_lower_copy(body_part::ToString(BODY_PART_ENUM))
                    == SYSTEM_NAME_LOWERCASE)
                {
                    type_ = body_part::WeaponType(BODY_PART_ENUM);
                    variant_ = BODY_PART_ENUM;
                    SetNamesAndVerify("'name' constructor when name=BodyPart");
                    return true;
                }
            }

            return false;
        }

        bool WeaponTypeWrapper::SetupWithKnifeOrDaggerName(
            const std::string & SYSTEM_NAME_LOWERCASE, const sfml_util::Size::Enum SIZE)
        {
            if (SIZE == sfml_util::Size::Count)
            {
                return false;
            }

            auto const KNIFE_NAME{ weapon_type::ToString(weapon_type::Knife) };

            auto const IS_KNIFE{ SYSTEM_NAME_LOWERCASE
                                 == boost::algorithm::to_lower_copy(KNIFE_NAME) };

            auto const IS_DAGGER{ SYSTEM_NAME_LOWERCASE
                                  == boost::algorithm::to_lower_copy(DAGGER_NAME_) };

            if (IS_KNIFE || IS_DAGGER)
            {
                type_ = weapon_type::Knife;
                isDagger_ = IS_DAGGER;
                variant_ = SIZE;
                SetNamesAndVerify("'name' constructor when name=Knife/Dagger");
                return true;
            }
            else
            {
                return false;
            }
        }

        void WeaponTypeWrapper::SetNamesAndVerify(const std::string & CALLER_CONTEXT_DESCRIPTION)
        {
            elementTypes_.clear();

            switch (type_)
            {
                case weapon_type::Claws:
                case weapon_type::Bite:
                case weapon_type::Fists:
                case weapon_type::Tendrils:
                case weapon_type::Breath:
                {
                    generalName_ = weapon_type::Name(type_);
                    specificName_ = generalName_;
                    systemName_ = weapon_type::ToString(type_, false, false);
                    readableName_ = generalName_;
                    break;
                }

                case weapon_type::Staff:
                {
                    if (boost::get<bool>(variant_))
                    {
                        generalName_ = QUARTERSTAFF_NAME_;
                        specificName_ = QUARTERSTAFF_NAME_;
                        systemName_ = QUARTERSTAFF_NAME_;
                        readableName_ = QUARTERSTAFF_NAME_;
                        elementTypes_ = { element_type::Frost, element_type::Honor };
                    }
                    else
                    {
                        generalName_ = weapon_type::Name(weapon_type::Staff);
                        specificName_ = generalName_;
                        systemName_ = generalName_;
                        readableName_ = generalName_;
                        elementTypes_ = { element_type::Shadow, element_type::Honor };
                    }

                    break;
                }

                case weapon_type::Sword:
                {
                    auto const SWORD_TYPE{ SwordType() };

                    generalName_ = weapon_type::Name(type_);
                    specificName_ = sword_type::Name(SWORD_TYPE);
                    systemName_ = sword_type::ToString(SWORD_TYPE);

                    if ((SWORD_TYPE == sword_type::Broadsword)
                        || (SWORD_TYPE == sword_type::Longsword)
                        || (SWORD_TYPE == sword_type::Shortsword))
                    {
                        readableName_ = specificName_;
                    }
                    else
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }

                    switch (SWORD_TYPE)
                    {
                        case sword_type::Gladius:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Cutlass:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Falcata:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Rapier:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Frost | element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Saber:
                        {
                            elementTypes_ = { element_type::Shadow };
                            break;
                        }
                        case sword_type::Broadsword:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Longsword:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Frost | element_type::Fire);

                            break;
                        }
                        case sword_type::Knightlysword:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case sword_type::Flamberg:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Claymore:
                        {
                            elementTypes_ = element_type::AllCombinations();
                            break;
                        }
                        case sword_type::Shortsword:
                        case sword_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }

                case weapon_type::Axe:
                {
                    auto const AXE_TYPE{ AxeType() };

                    generalName_ = weapon_type::Name(type_);

                    // you would think this should be axe_type::Name() called here, but the names
                    // are so distinct that we use ToString() instead, intentionally.
                    specificName_ = axe_type::ToString(AXE_TYPE);

                    systemName_ = axe_type::ToString(AXE_TYPE);

                    if (AXE_TYPE == axe_type::Sickle)
                    {
                        readableName_ = specificName_;
                    }
                    else
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }

                    switch (AXE_TYPE)
                    {
                        case axe_type::Sickle:
                        {
                            elementTypes_ = { element_type::Shadow };
                            break;
                        }
                        case axe_type::Battleaxe:
                        {
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case axe_type::Waraxe:
                        {
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case axe_type::Handaxe:
                        case axe_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }

                case weapon_type::Whip:
                {
                    auto const WHIP_TYPE{ WhipType() };
                    generalName_ = weapon_type::Name(type_);
                    specificName_ = whip_type::Name(WHIP_TYPE);
                    systemName_ = whip_type::ToString(WHIP_TYPE);
                    readableName_ = specificName_;

                    switch (WHIP_TYPE)
                    {
                        case whip_type::Flail:
                        {
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case whip_type::MaceAndChain:
                        {
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case whip_type::Bullwhip:
                        case whip_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }

                case weapon_type::Club:
                {
                    auto const CLUB_TYPE{ ClubType() };

                    generalName_ = weapon_type::Name(type_);
                    specificName_ = club_type::ToString(ClubType());
                    systemName_ = specificName_;

                    if (CLUB_TYPE == club_type::Spiked)
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }
                    else
                    {
                        readableName_ = specificName_;
                    }

                    switch (CLUB_TYPE)
                    {
                        case club_type::Spiked:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case club_type::Mace:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case club_type::Warhammer:
                        {
                            elementTypes_ = element_type::AllCombinations();
                            break;
                        }
                        case club_type::Maul:
                        case club_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }

                case weapon_type::Projectile:
                {
                    auto const PROJECTILE_TYPE{ ProjectileType() };

                    switch (PROJECTILE_TYPE)
                    {
                        case projectile_type::Blowpipe:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            break;
                        }
                        case projectile_type::Sling:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            break;
                        }
                        case projectile_type::Shortbow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            break;
                        }
                        case projectile_type::Longbow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case projectile_type::Crossbow:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case projectile_type::CompositeBow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            elementTypes_ = element_type::AllCombinations();
                            break;
                        }
                        case projectile_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    specificName_ = projectile_type::Name(PROJECTILE_TYPE);
                    systemName_ = projectile_type::ToString(PROJECTILE_TYPE);
                    readableName_ = specificName_;
                    break;
                }

                case weapon_type::Knife:
                {
                    if (isDagger_)
                    {
                        generalName_ = DAGGER_NAME_;
                        specificName_ = DAGGER_NAME_;
                        systemName_ = DAGGER_NAME_;

                        auto const SIZE{ Size() };
                        if ((SIZE != sfml_util::Size::Count) && (SIZE != sfml_util::Size::Medium))
                        {
                            readableName_ = sfml_util::Size::ToString(SIZE) + " " + generalName_;
                        }
                        else
                        {
                            readableName_ = generalName_;
                        }

                        elementTypes_ = element_type::AllCombinations(
                            element_type::Fire | element_type::Frost | element_type::Honor);
                    }
                    else
                    {
                        generalName_ = weapon_type::Name(type_);
                        specificName_ = generalName_;
                        systemName_ = weapon_type::ToString(type_, false);

                        auto const SIZE{ Size() };
                        if ((SIZE != sfml_util::Size::Count) && (SIZE != sfml_util::Size::Medium))
                        {
                            readableName_ = sfml_util::Size::ToString(SIZE) + " " + generalName_;
                        }
                        else
                        {
                            readableName_ = generalName_;
                        }

                        elementTypes_ = element_type::AllCombinations(
                            element_type::Fire | element_type::Frost | element_type::Shadow);
                    }

                    break;
                }

                case weapon_type::Spear:
                case weapon_type::BladedStaff:
                {
                    auto const BLADEDSTAFF_TYPE{ BladedStaffType() };
                    generalName_ = weapon_type::Name(weapon_type::BladedStaff);
                    specificName_ = bladedstaff_type::Name(BLADEDSTAFF_TYPE);
                    systemName_ = bladedstaff_type::ToString(BLADEDSTAFF_TYPE);

                    // skip prepending the general name because most players will know these items
                    // by their specfificName_
                    readableName_ = specificName_;

                    switch (BLADEDSTAFF_TYPE)
                    {
                        case bladedstaff_type::Scythe:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case bladedstaff_type::Pike:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Partisan:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Halberd:
                        {
                            elementTypes_ = element_type::AllCombinations(
                                element_type::Fire | element_type::Frost | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Spear:
                        case bladedstaff_type::ShortSpear:
                        case bladedstaff_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }

                case weapon_type::NotAWeapon:
                case weapon_type::Melee:
                case weapon_type::Bladed:
                case weapon_type::Pointed:
                case weapon_type::Blowpipe:
                case weapon_type::Bow:
                case weapon_type::Crossbow:
                case weapon_type::Sling:
                default:
                {
                    break;
                }
            }

            elementTypes_.emplace_back(element_type::None);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValidCompleteCheck()),
                "item::weapon::WeaponTypeWrapper::SetNamesAndVerify("
                    << CALLER_CONTEXT_DESCRIPTION
                    << ") found the object was invalid when finished:  " << ToString());
        }

        bool WeaponTypeWrapper::IsValidCompleteCheck() const
        {
            if (elementTypes_.empty())
            {
                return false;
            }

            if (generalName_.empty() || specificName_.empty() || systemName_.empty()
                || readableName_.empty())
            {
                return false;
            }

            switch (type_)
            {
                case weapon_type::Claws:
                {
                    return (variant_.which() == BODY_PART_INDEX)
                        && (body_part::Claws == boost::get<body_part::Enum>(variant_));
                }
                case weapon_type::Bite:
                {
                    return (variant_.which() == BODY_PART_INDEX)
                        && (body_part::Bite == boost::get<body_part::Enum>(variant_));
                }
                case weapon_type::Fists:
                {
                    return (variant_.which() == BODY_PART_INDEX)
                        && (body_part::Fists == boost::get<body_part::Enum>(variant_));
                }
                case weapon_type::Tendrils:
                {
                    return (variant_.which() == BODY_PART_INDEX)
                        && (body_part::Tendrils == boost::get<body_part::Enum>(variant_));
                }
                case weapon_type::Breath:
                {
                    return (variant_.which() == BODY_PART_INDEX)
                        && (body_part::Breath == boost::get<body_part::Enum>(variant_));
                }
                case weapon_type::Sword:
                {
                    return (variant_.which() == SWORD_INDEX)
                        && (sword_type::Count != boost::get<sword_type::Enum>(variant_));
                }
                case weapon_type::Axe:
                {
                    return (variant_.which() == AXE_INDEX)
                        && (axe_type::Count != boost::get<axe_type::Enum>(variant_));
                }
                case weapon_type::Whip:
                {
                    return (variant_.which() == WHIP_INDEX)
                        && (whip_type::Count != boost::get<whip_type::Enum>(variant_));
                }
                case weapon_type::Club:
                {
                    return (variant_.which() == CLUB_INDEX)
                        && (club_type::Count != boost::get<club_type::Enum>(variant_));
                }
                case weapon_type::Projectile:
                case weapon_type::Blowpipe:
                case weapon_type::Bow:
                case weapon_type::Crossbow:
                case weapon_type::Sling:
                {
                    return (variant_.which() == PROJECTILE_INDEX)
                        && (projectile_type::Count != boost::get<projectile_type::Enum>(variant_));
                }
                case weapon_type::BladedStaff:
                case weapon_type::Spear:
                {
                    return (variant_.which() == BLADEDSTAFF_INDEX)
                        && (bladedstaff_type::Count
                            != boost::get<bladedstaff_type::Enum>(variant_));
                }
                case weapon_type::Knife:
                {
                    return (variant_.which() == SIZE_INDEX)
                        && (sfml_util::Size::Count != boost::get<sfml_util::Size::Enum>(variant_));
                }
                case weapon_type::Staff:
                {
                    return (variant_.which() == QUARTERSTAFF_INDEX);
                }
                case weapon_type::NotAWeapon:
                case weapon_type::Melee:
                case weapon_type::Bladed:
                case weapon_type::Pointed:
                default:
                {
                    return false;
                }
            }
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
