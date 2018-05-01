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

#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <numeric>
#include <sstream>
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
        {
            // This is the default constructor and it needs to support construction with all default
            // parameters that leave the object in an invalid state, so this if/return allows for
            // that.
            if (SYSTEM_NAME.empty())
            {
                return;
            }

            auto const SYSTEM_NAME_LOWERCASE{ boost::algorithm::to_lower_copy(SYSTEM_NAME) };

            if (generalName_.empty())
            {
                SetupWithBodypartName(SYSTEM_NAME_LOWERCASE);
            }

            if (generalName_.empty())
            {
                SetupWithKnifeOrDaggerName(SYSTEM_NAME_LOWERCASE, KNIFE_OR_DAGGER_SIZE);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<sword_type>(SYSTEM_NAME_LOWERCASE, weapon_type::Sword);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<axe_type>(SYSTEM_NAME_LOWERCASE, weapon_type::Axe);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<club_type>(SYSTEM_NAME_LOWERCASE, weapon_type::Club);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<whip_type>(SYSTEM_NAME_LOWERCASE, weapon_type::Whip);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<projectile_type>(
                    SYSTEM_NAME_LOWERCASE, weapon_type::Projectile);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<bladedstaff_type>(
                    SYSTEM_NAME_LOWERCASE, weapon_type::BladedStaff);
            }

            if (generalName_.empty()
                && (SYSTEM_NAME_LOWERCASE == boost::algorithm::to_lower_copy(QUARTERSTAFF_NAME_)))
            {
                type_ = weapon_type::Staff;
                variant_ = true;
                SetNames();
            }

            if (generalName_.empty()
                && (SYSTEM_NAME_LOWERCASE
                    == boost::algorithm::to_lower_copy(weapon_type::Name(weapon_type::Staff))))
            {
                type_ = weapon_type::Staff;
                variant_ = false;
                SetNames();
            }

            std::ostringstream ss;
            ss << "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(name=\"" << SYSTEM_NAME
               << "\", size=\""
               << ((KNIFE_OR_DAGGER_SIZE == sfml_util::Size::Count)
                       ? "Count"
                       : sfml_util::Size::ToString(KNIFE_OR_DAGGER_SIZE))
               << "\") ";

            M_ASSERT_OR_LOGANDTHROW_SS(
                (generalName_.empty() == false),
                ss.str() << "but that name failed to be recognized by any weapon category.");

            ss << "and that set weapon_type=" << weapon_type::ToString(type_)
               << " and isDagger_=" << std::boolalpha << isDagger_
               << ", variant_.which()=" << variant_.which() << ", ";

            if (IsValid() == false)
            {
                M_ASSERT_OR_LOGANDTHROW_SS((IsValid()), ss.str() << "but IsValid() return false.");
            }

            auto const ORIG_GENERAL_NAME{ generalName_ };
            auto const ORIG_SPECIFIC_NAME{ specificName_ };
            auto const ORIG_SYSTEM_NAME{ systemName_ };
            auto const ORIG_READABLE_NAME{ readableName_ };

            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ORIG_GENERAL_NAME == generalName_) && (ORIG_SPECIFIC_NAME == specificName_)
                 && (ORIG_SYSTEM_NAME == systemName_) && (ORIG_READABLE_NAME == readableName_)),
                ss.str() << "but the generated names did not match general=" << ORIG_GENERAL_NAME
                         << "/" << generalName_ << ", specific=" << ORIG_SPECIFIC_NAME << "/"
                         << specificName_ << ", system=" << ORIG_SYSTEM_NAME << "/" << systemName_
                         << ", readable=\"" << ORIG_READABLE_NAME << "\" \"" << readableName_
                         << "\".");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bool IS_DAGGER, const sfml_util::Size::Enum SIZE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Knife)
            , isDagger_(IS_DAGGER)
            , variant_(SIZE)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (SIZE != sfml_util::Size::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(is_dagger="
                    << std::boolalpha << IS_DAGGER
                    << ", size=Count) and that SIZE must be valid. (not Count)");

            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(is_dagger="
                    << std::boolalpha << IS_DAGGER << ", size="
                    << ((SIZE == sfml_util::Size::Count) ? "Count"
                                                         : sfml_util::Size::ToString(SIZE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bool IS_QUARTERSTAFF)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Staff)
            , isDagger_(false)
            , variant_(IS_QUARTERSTAFF)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(is_quarterstaff="
                    << std::boolalpha << IS_QUARTERSTAFF
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const item::body_part::Enum BODY_PART)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(body_part::WeaponType(BODY_PART))
            , isDagger_(false)
            , variant_(BODY_PART)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(body_part="
                    << ((body_part::Count == BODY_PART) ? "Count" : body_part::ToString(BODY_PART))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const sword_type::Enum SWORD_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Sword)
            , isDagger_(false)
            , variant_(SWORD_TYPE)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(sword_type="
                    << ((sword_type::Count == SWORD_TYPE) ? "Count"
                                                          : sword_type::ToString(SWORD_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const axe_type::Enum AXE_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Axe)
            , isDagger_(false)
            , variant_(AXE_TYPE)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(sword_type="
                    << ((axe_type::Count == AXE_TYPE) ? "Count" : axe_type::ToString(AXE_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const club_type::Enum CLUB_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Club)
            , isDagger_(false)
            , variant_(CLUB_TYPE)
        {
            variant_ = CLUB_TYPE;
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(club_type="
                    << ((club_type::Count == CLUB_TYPE) ? "Count" : club_type::ToString(CLUB_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const whip_type::Enum WHIP_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Whip)
            , isDagger_(false)
            , variant_(WHIP_TYPE)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(whip_type="
                    << ((whip_type::Count == WHIP_TYPE) ? "Count" : whip_type::ToString(WHIP_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const projectile_type::Enum PROJ_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Projectile)
            , isDagger_(false)
            , variant_(PROJ_TYPE)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(projectile_type="
                    << ((projectile_type::Count == PROJ_TYPE)
                            ? "Count"
                            : projectile_type::ToString(PROJ_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bladedstaff_type::Enum BSTAFF_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::BladedStaff)
            , isDagger_(false)
            , variant_(BSTAFF_TYPE)
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(bladedstaff_type="
                    << ((bladedstaff_type::Count == BSTAFF_TYPE)
                            ? "Count"
                            : bladedstaff_type::ToString(BSTAFF_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        bool WeaponTypeWrapper::IsValid() const
        {
            if (type_ == weapon_type::NotAWeapon)
            {
                return false;
            }
            else if (
                generalName_.empty() || specificName_.empty() || systemName_.empty()
                || readableName_.empty())
            {
                return false;
            }
            else if ((weapon_type::Knife != type_) && isDagger_)
            {
                return false;
            }
            else if ((weapon_type::Knife == type_) && (Size() == sfml_util::Size::Count))
            {
                return false;
            }
            else
            {
                const std::vector<bool> BOOLS{ IsQuarterstaff(), IsBite(),       IsClaws(),
                                               IsFists(),        IsStaff(),      IsKnife(),
                                               IsDagger(),       IsTendrils(),   IsBreath(),
                                               IsSword(),        IsAxe(),        IsClub(),
                                               IsWhip(),         IsProjectile(), IsBladedStaff() };

                const int COUNT{ std::accumulate(
                    std::begin(BOOLS),
                    std::end(BOOLS),
                    0,
                    [](auto const SUBTOTAL, auto const NEXT_BOOL) {
                        return SUBTOTAL + ((NEXT_BOOL) ? 1 : 0);
                    }) };

                return (COUNT == 1);
            }
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

            std::ostringstream ss;
            ss << "generalName_=\"" << generalName_ << "\", specificName_=\"" << specificName_
               << "\", systemName_=\"" << systemName_ << "\", readableName_=\"" << readableName_
               << "\", type_=" << weapon_type::ToString(type_, true)
               << ", is_dagger=" << std::boolalpha << isDagger_
               << ", variant_.which()=" << variant_.which() << ","
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
               << ((IsBladedStaff()) ? bladedstaff_type::ToString(BladedStaffType()) : "") << ")";

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

        void WeaponTypeWrapper::SetNames()
        {
            if (IsBite() || IsClaws() || IsFists() || IsStaff() || IsTendrils() || IsBreath())
            {
                generalName_ = weapon_type::Name(type_);
                specificName_ = generalName_;
                systemName_ = weapon_type::ToString(type_, false, false);
                readableName_ = generalName_;
            }
            else if (IsKnife())
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
            }
            else if (IsDagger())
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
            }
            else if (IsQuarterstaff())
            {
                generalName_ = QUARTERSTAFF_NAME_;
                specificName_ = QUARTERSTAFF_NAME_;
                systemName_ = QUARTERSTAFF_NAME_;
                readableName_ = QUARTERSTAFF_NAME_;
            }
            else if (IsSword())
            {
                auto const SWORD_TYPE{ SwordType() };

                generalName_ = weapon_type::Name(type_);
                specificName_ = sword_type::Name(SWORD_TYPE);
                systemName_ = sword_type::ToString(SWORD_TYPE);

                if ((SWORD_TYPE == sword_type::Broadsword) || (SWORD_TYPE == sword_type::Longsword)
                    || (SWORD_TYPE == sword_type::Shortsword))
                {
                    readableName_ = specificName_;
                }
                else
                {
                    readableName_ = specificName_ + " " + generalName_;
                }
            }
            else if (IsAxe())
            {
                auto const AXE_TYPE{ AxeType() };

                generalName_ = weapon_type::Name(type_);

                // you would think this should be axe_type::Name() called here, but the names are so
                // distinct that we use ToString() instead, intentionally.
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
            }
            else if (IsClub())
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
            }
            else if (IsWhip())
            {
                auto const WHIP_TYPE{ WhipType() };

                generalName_ = weapon_type::Name(type_);
                specificName_ = whip_type::Name(WHIP_TYPE);
                systemName_ = whip_type::ToString(WHIP_TYPE);
                readableName_ = specificName_;
            }
            else if (IsProjectile())
            {
                auto const PROJECTILE_TYPE{ ProjectileType() };

                if (PROJECTILE_TYPE == projectile_type::Blowpipe)
                {
                    generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                }
                else if (PROJECTILE_TYPE == projectile_type::Sling)
                {
                    generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                }
                else if (PROJECTILE_TYPE == projectile_type::Crossbow)
                {
                    generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                }
                else
                {
                    generalName_ = BOW_GENERAL_NAME_;
                }

                specificName_ = projectile_type::Name(PROJECTILE_TYPE);
                systemName_ = projectile_type::ToString(PROJECTILE_TYPE);
                readableName_ = specificName_;
            }
            else if (IsBladedStaff())
            {
                auto const BLADEDSTAFF_TYPE{ BladedStaffType() };
                generalName_ = weapon_type::Name(weapon_type::BladedStaff);
                specificName_ = bladedstaff_type::Name(BLADEDSTAFF_TYPE);
                systemName_ = bladedstaff_type::ToString(BLADEDSTAFF_TYPE);

                // skip prepending the general name because most players will know these items by
                // their specfificName_
                readableName_ = specificName_;
            }
        }

        bool WeaponTypeWrapper::SetupWithBodypartName(const std::string & SYSTEM_NAME_LOWERCASE)
        {
            for (int i(0); i < body_part::Count; ++i)
            {
                auto const BODY_PART_ENUM{ static_cast<body_part::Enum>(i) };

                if (boost::algorithm::to_lower_copy(body_part::ToString(BODY_PART_ENUM))
                    == SYSTEM_NAME_LOWERCASE)
                {
                    generalName_ = body_part::ToString(BODY_PART_ENUM);
                    specificName_ = generalName_;
                    systemName_ = generalName_;
                    readableName_ = generalName_;
                    type_ = body_part::WeaponType(BODY_PART_ENUM);
                    variant_ = BODY_PART_ENUM;
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
                auto const NAME_TO_USE{ ((IS_KNIFE) ? KNIFE_NAME : DAGGER_NAME_) };

                generalName_ = NAME_TO_USE;
                specificName_ = NAME_TO_USE;
                systemName_ = NAME_TO_USE;
                readableName_ = NAME_TO_USE;
                type_ = weapon_type::Knife;
                isDagger_ = IS_DAGGER;
                variant_ = SIZE;
                return true;
            }
            else
            {
                return false;
            }
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
