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
#ifndef HEROESPATH_ITEM_WEAPONF_TYPE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONF_TYPE_WRAPPER_HPP_INCLUDED
//
// weapons-type-wrapper.hpp
//
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "sfml-util/size-enum.hpp"

#include <boost/type_index.hpp>
#include <boost/variant.hpp>

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        // Responsible generating all type and name information about a SINGLE weapon item.
        // Instances of this class can be constructed from either the required enum and size, or
        // from the unique name.  Unfortunately these classes are serialized so they require a
        // default constructor that leaves the object in an invalid state.
        class WeaponTypeWrapper
        {
        public:
            // use for default construction or to construct from a weapon's unqiue name ("Knife" and
            // "Dagger" must also specify the size)
            explicit WeaponTypeWrapper(
                const std::string & SYSTEM_NAME = "",
                const sfml_util::Size::Enum KNIFE_SIZE = sfml_util::Size::Count);

            // use to create knives and daggers
            WeaponTypeWrapper(const bool IS_DAGGER, const sfml_util::Size::Enum SIZE);

            // use to create staffs
            explicit WeaponTypeWrapper(const bool IS_QUARTERSTAFF);

            explicit WeaponTypeWrapper(const body_part::Enum);
            explicit WeaponTypeWrapper(const sword_type::Enum);
            explicit WeaponTypeWrapper(const axe_type::Enum);
            explicit WeaponTypeWrapper(const club_type::Enum);
            explicit WeaponTypeWrapper(const whip_type::Enum);
            explicit WeaponTypeWrapper(const projectile_type::Enum);
            explicit WeaponTypeWrapper(const bladedstaff_type::Enum);

            const std::string GeneralName() const { return generalName_; }
            const std::string SpecificName() const { return specificName_; }
            const std::string SystemName() const { return systemName_; }
            const std::string ReadableName() const { return readableName_; }
            const std::string DetailsKeyName() const;

            const std::string ToString() const;

            bool IsTypeValid() const { return (type_ != weapon_type::NotAWeapon); }

            weapon_type::Enum Type() const { return type_; }

            const ElementEnumVec_t ElementTypes() const { return elementTypes_; }

            bool IsStaff() const
            {
                return (weapon_type::Staff & type_) && (variant_.which() == QUARTERSTAFF_INDEX)
                    && (boost::get<bool>(variant_) == false);
            }

            bool IsQuarterstaff() const
            {
                return (weapon_type::Staff & type_) && (variant_.which() == QUARTERSTAFF_INDEX)
                    && boost::get<bool>(variant_);
            }

            bool IsBodyPart() const
            {
                return (IsBite() || IsClaws() || IsFists() || IsTendrils() || IsBreath());
            }

            bool IsBite() const
            {
                return (
                    GetSpecificTypeIfValid<body_part>(weapon_type::Bite, BODY_PART_INDEX)
                    != body_part::Count);
            }

            bool IsClaws() const
            {
                return (
                    GetSpecificTypeIfValid<body_part>(weapon_type::Claws, BODY_PART_INDEX)
                    != body_part::Count);
            }

            bool IsFists() const
            {
                return (
                    GetSpecificTypeIfValid<body_part>(weapon_type::Fists, BODY_PART_INDEX)
                    != body_part::Count);
            }

            bool IsTendrils() const
            {
                return (
                    GetSpecificTypeIfValid<body_part>(weapon_type::Tendrils, BODY_PART_INDEX)
                    != body_part::Count);
            }

            bool IsBreath() const
            {
                return (
                    GetSpecificTypeIfValid<body_part>(weapon_type::Breath, BODY_PART_INDEX)
                    != body_part::Count);
            }

            bool IsKnife() const
            {
                return (weapon_type::Knife & type_) && (Size() != sfml_util::Size::Count)
                    && (isDagger_ == false);
            }

            bool IsDagger() const
            {
                return (weapon_type::Knife & type_) && (Size() != sfml_util::Size::Count)
                    && isDagger_;
            }

            bool IsSword() const
            {
                return (weapon_type::Sword & type_) && (SwordType() != sword_type::Count);
            }

            bool IsAxe() const
            {
                return (weapon_type::Axe & type_) && (AxeType() != axe_type::Count);
            }

            bool IsClub() const
            {
                return (weapon_type::Club & type_) && (ClubType() != club_type::Count);
            }

            bool IsWhip() const
            {
                return (weapon_type::Whip & type_) && (WhipType() != whip_type::Count);
            }

            bool IsProjectile() const
            {
                return (weapon_type::Projectile & type_)
                    && (ProjectileType() != projectile_type::Count);
            }

            bool IsBladedStaff() const
            {
                return (weapon_type::BladedStaff & type_)
                    && (BladedStaffType() != bladedstaff_type::Count);
            }

            sfml_util::Size::Enum Size() const
            {
                return GetSpecificTypeIfValid<sfml_util::Size>(weapon_type::Knife, SIZE_INDEX);
            }

            sword_type::Enum SwordType() const
            {
                return GetSpecificTypeIfValid<sword_type>(weapon_type::Sword, SWORD_INDEX);
            }

            axe_type::Enum AxeType() const
            {
                return GetSpecificTypeIfValid<axe_type>(weapon_type::Axe, AXE_INDEX);
            }

            club_type::Enum ClubType() const
            {
                return GetSpecificTypeIfValid<club_type>(weapon_type::Club, CLUB_INDEX);
            }

            whip_type::Enum WhipType() const
            {
                return GetSpecificTypeIfValid<whip_type>(weapon_type::Whip, WHIP_INDEX);
            }

            projectile_type::Enum ProjectileType() const
            {
                return GetSpecificTypeIfValid<projectile_type>(
                    weapon_type::Projectile, PROJECTILE_INDEX);
            }

            bladedstaff_type::Enum BladedStaffType() const
            {
                return GetSpecificTypeIfValid<bladedstaff_type>(
                    weapon_type::BladedStaff, BLADEDSTAFF_INDEX);
            }

            const std::string
                ImageFilename(const std::string & SEPARATOR, const std::string & EXTENSION) const;

            bool IsGeneralNameAlmostSpecificName() const;

            static const std::vector<WeaponTypeWrapper> MakeCompleteSet();

            template <typename T>
            static const std::vector<WeaponTypeWrapper> MakeSpecificSet()
            {
                std::vector<WeaponTypeWrapper> wrappers;

                for (int i(0); i < T::Count; ++i)
                {
                    auto const SPECIFIC_WEAPON_ENUM{ static_cast<typename T::Enum>(i) };
                    wrappers.emplace_back(WeaponTypeWrapper(SPECIFIC_WEAPON_ENUM));
                }

                return wrappers;
            }

            template <typename T>
            static void AppendSpecificSet(std::vector<WeaponTypeWrapper> & wrappers)
            {
                for (auto const & WEAPON_TYPE_WRAPPER : MakeSpecificSet<T>())
                {
                    wrappers.emplace_back(WEAPON_TYPE_WRAPPER);
                }
            }

            friend bool operator==(const WeaponTypeWrapper & L, const WeaponTypeWrapper & R);
            friend bool operator<(const WeaponTypeWrapper & L, const WeaponTypeWrapper & R);

        private:
            template <typename T>
            typename T::Enum
                GetSpecificTypeIfValid(const weapon_type::Enum WEAPON_TYPE, const int INDEX) const
            {
                if ((WEAPON_TYPE & type_) && (variant_.which() == INDEX))
                {
                    return boost::get<typename T::Enum>(variant_);
                }
                else
                {
                    return T::Count;
                }
            }

            bool SetupWithBodypartName(const std::string & SYSTEM_NAME_LOWERCASE);

            bool SetupWithKnifeOrDaggerName(
                const std::string & SYSTEM_NAME_LOWERCASE, const sfml_util::Size::Enum SIZE);

            template <typename T>
            bool SetupWithSpecificTypeName(
                const std::string & SYSTEM_NAME_LOWERCASE, const weapon_type::Enum WEAPON_TYPE)
            {
                for (int i(0); i < T::Count; ++i)
                {
                    auto const SPECIFIC_WEAPON_ENUM{ static_cast<typename T::Enum>(i) };

                    auto const SPECIFIC_WEAPON_STR_LOWERCASE{ boost::algorithm::to_lower_copy(
                        T::ToString(SPECIFIC_WEAPON_ENUM)) };

                    if (SPECIFIC_WEAPON_STR_LOWERCASE == SYSTEM_NAME_LOWERCASE)
                    {
                        type_ = WEAPON_TYPE;
                        variant_ = SPECIFIC_WEAPON_ENUM;

                        std::ostringstream ss;
                        ss << "after SetupWithSpecificTypeName<"
                           << boost::typeindex::type_id<T>().pretty_name()
                           << "::Enum>(system_name_lowercase=" << SYSTEM_NAME_LOWERCASE
                           << ", weapon_type_tostring_lowercase=" << SPECIFIC_WEAPON_STR_LOWERCASE
                           << ")";

                        SetNamesAndVerify(ss.str());
                        return true;
                    }
                }

                return false;
            }

            void SetNamesAndVerify(const std::string & CALLER_CONTEXT_DESCRIPTION);
            bool IsValidCompleteCheck() const;

        public:
            static const std::string DAGGER_NAME_;
            static const std::string QUARTERSTAFF_NAME_;
            static const std::string BOW_GENERAL_NAME_;

        private:
            // a readable (gui visible) category kind of name, i.e. "Sword" when
            // systemName_="KnightlySword"
            std::string generalName_;

            // a readable (gui visible) unique name among all with a common generalName_, i.e.
            // "Knightly" when systemName_="KnightlySword"
            std::string specificName_;

            // a non-readable (contains no spaces) name that is unique system wide, not just unique
            // per generalName_, i.e. "KnightlySword" when specificlName_="Knightly"
            std::string systemName_;

            // a readable (gui visible) full name that is often specificName_ + " " + generalName_,
            // i.e. "Knightly Sword" when systemName_="KnightlySword"
            std::string readableName_;

            item::weapon_type::Enum type_;

            // since both knives and daggers will have type_ & weapon_type::Knife this is how to
            // tell the difference
            bool isDagger_;

            boost::variant<
                bool, // is quarterstaff
                sfml_util::Size::Enum, // if knife or dagger then must NOT be sfml_util::Size::Count
                sword_type::Enum,
                axe_type::Enum,
                club_type::Enum,
                whip_type::Enum,
                projectile_type::Enum,
                bladedstaff_type::Enum,
                body_part::Enum>
                variant_;

            // these are the "standard" element enchantments that are allowed for this weapon if not
            // also a set_type or named_type.
            ElementEnumVec_t elementTypes_;

            static const int QUARTERSTAFF_INDEX = 0;
            static const int SIZE_INDEX = 1;
            static const int SWORD_INDEX = 2;
            static const int AXE_INDEX = 3;
            static const int CLUB_INDEX = 4;
            static const int WHIP_INDEX = 5;
            static const int PROJECTILE_INDEX = 6;
            static const int BLADEDSTAFF_INDEX = 7;
            static const int BODY_PART_INDEX = 8;

        private:
            friend class boost::serialization::access;
            template <typename Archive>
            void serialize(Archive & ar, const unsigned int)
            {
                ar & generalName_;
                ar & specificName_;
                ar & systemName_;
                ar & readableName_;
                ar & type_;
                ar & isDagger_;
                ar & variant_;
                ar & elementTypes_;
            }
        };

        inline bool operator==(const WeaponTypeWrapper & L, const WeaponTypeWrapper & R)
        {
            // elementTypes_ intentionally not considered

            return std::tie(
                       L.generalName_,
                       L.specificName_,
                       L.systemName_,
                       L.readableName_,
                       L.type_,
                       L.isDagger_,
                       L.variant_)
                == std::tie(
                       R.generalName_,
                       R.specificName_,
                       R.systemName_,
                       R.readableName_,
                       R.type_,
                       R.isDagger_,
                       R.variant_);
        }

        inline bool operator!=(const WeaponTypeWrapper & L, const WeaponTypeWrapper & R)
        {
            return !(L == R);
        }

        inline bool operator<(const WeaponTypeWrapper & L, const WeaponTypeWrapper & R)
        {
            // elementTypes_ intentionally not considered

            return std::tie(
                       L.generalName_,
                       L.specificName_,
                       L.systemName_,
                       L.readableName_,
                       L.type_,
                       L.isDagger_,
                       L.variant_)
                < std::tie(
                       R.generalName_,
                       R.specificName_,
                       R.systemName_,
                       R.readableName_,
                       R.type_,
                       R.isDagger_,
                       R.variant_);
        }

        using WeaponTypeWrapperVec_t = std::vector<WeaponTypeWrapper>;
        using WeaponTypeWrapperVecVec_t = std::vector<WeaponTypeWrapperVec_t>;

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONF_TYPE_WRAPPER_HPP_INCLUDED
