// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_WEAPONF_TYPE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONF_TYPE_WRAPPER_HPP_INCLUDED
//
// weapons-type-wrapper.hpp
//
#include "item/item-type-enum.hpp"
#include "item/weapon-types.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/nameof.hpp"
#include "misc/strings.hpp"

#include <boost/variant.hpp>

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
            struct KnifeOrDagger
            {};

            struct StaffOrQuarterstaff
            {};

            // use for default construction or to construct from a weapon's unique name ("Knife" and
            // "Dagger" must also specify the size)
            explicit WeaponTypeWrapper(const std::string & SYSTEM_NAME = "");

            // use to create knives and daggers
            WeaponTypeWrapper(const KnifeOrDagger, const bool IS_DAGGER);

            // use to create staffs
            explicit WeaponTypeWrapper(const StaffOrQuarterstaff, const bool IS_QUARTERSTAFF);

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

            // IsBodyPart(), IsBlowpipe(), HasHandle(), IsPole() are complete and
            // mutually exclusive

            bool IsBodyPart() const
            {
                const auto BODY_PART_TYPE { BodyPartType() };
                return (BODY_PART_TYPE != body_part::Count) && (BODY_PART_TYPE != body_part::Skin);
            }

            bool IsBlowpipe() const { return (ProjectileType() == projectile_type::Blowpipe); }

            bool HasHandle() const
            {
                return (IsBodyPart() == false) && (ProjectileType() != projectile_type::Blowpipe)
                    && ((type_ & weapon_type::Staff) == 0) && (IsBladedStaff() == false);
            }

            bool IsPole() const { return ((type_ & weapon_type::Staff) || IsBladedStaff()); }

            bool CanBeReinforced() const
            {
                return (IsBodyPart() == false) && (IsSpear() || (type_ & weapon_type::Staff));
            }

            bool WillBeDescribedAsOneSolidMaterial() const
            {
                return (
                    (type_ & weapon_type::Knife) || IsSpear() || (type_ & weapon_type::Staff)
                    || (SwordType() == sword_type::Falcata) || (ClubType() == club_type::Mace)
                    || (ProjectileType() == projectile_type::Blowpipe));
            }

            bool IsTypeValid() const { return (type_ != weapon_type::None); }

            weapon_type::Enum Type() const { return type_; }

            const ElementEnumVec_t ElementTypes() const { return elementTypes_; }

            bool IsBow() const
            {
                return (ProjectileType() == projectile_type::Shortbow)
                    || (ProjectileType() == projectile_type::Longbow)
                    || (ProjectileType() == projectile_type::CompositeBow);
            }

            bool IsSpear() const
            {
                return (
                    (BladedStaffType() == bladedstaff_type::Spear)
                    || (BladedStaffType() == bladedstaff_type::ShortSpear));
            }

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

            bool IsBite() const { return (BodyPartType() == body_part::Bite); }

            bool IsClaws() const { return (BodyPartType() == body_part::Claws); }

            bool IsFists() const { return (BodyPartType() == body_part::Fists); }

            bool IsTentacles() const { return (BodyPartType() == body_part::Tentacles); }

            bool IsBreath() const { return (BodyPartType() == body_part::Breath); }

            bool IsKnife() const { return ((weapon_type::Knife & type_) && (isDagger_ == false)); }

            bool IsDagger() const { return ((weapon_type::Knife & type_) && isDagger_); }

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

            body_part::Enum BodyPartType() const
            {
                return GetSpecificTypeIfValid<body_part>(weapon_type::BodyPart, BODY_PART_INDEX);
            }

            const std::string
                ImageFilename(const std::string & SEPARATOR, const std::string & EXTENSION) const;

            bool IsGeneralNameAlmostSpecificName() const;

            static const std::vector<WeaponTypeWrapper> MakeCompleteSet();

            template <typename T>
            static const std::vector<WeaponTypeWrapper> MakeSpecificSet()
            {
                std::vector<WeaponTypeWrapper> wrappers;

                for (EnumUnderlying_t i(0); i < T::Count; ++i)
                {
                    const auto SPECIFIC_WEAPON_ENUM { static_cast<typename T::Enum>(i) };
                    wrappers.emplace_back(WeaponTypeWrapper(SPECIFIC_WEAPON_ENUM));
                }

                return wrappers;
            }

            template <typename T>
            static void AppendSpecificSet(std::vector<WeaponTypeWrapper> & wrappers)
            {
                for (const auto & WEAPON_TYPE_WRAPPER : MakeSpecificSet<T>())
                {
                    wrappers.emplace_back(WEAPON_TYPE_WRAPPER);
                }
            }

            name_material_type::Enum NameMaterialType() const;

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

            bool SetupWithKnifeOrDaggerName(const std::string & SYSTEM_NAME_LOWERCASE);

            template <typename T>
            bool SetupWithSpecificTypeName(
                const std::string & SYSTEM_NAME_LOWERCASE, const weapon_type::Enum WEAPON_TYPE)
            {
                for (EnumUnderlying_t i(0); i < T::Count; ++i)
                {
                    const auto SPECIFIC_WEAPON_ENUM { static_cast<typename T::Enum>(i) };

                    const auto SPECIFIC_WEAPON_STR_LOWERCASE { misc::ToLowerCopy(
                        T::ToString(SPECIFIC_WEAPON_ENUM)) };

                    if (SPECIFIC_WEAPON_STR_LOWERCASE == SYSTEM_NAME_LOWERCASE)
                    {
                        type_ = WEAPON_TYPE;
                        variant_ = SPECIFIC_WEAPON_ENUM;

                        const std::string CONTEXT_STR(
                            "after SetupWithSpecificTypeName<" + std::string(NAMEOF_TYPE_T(T))
                            + "::Enum>(system_name_lowercase=" + SYSTEM_NAME_LOWERCASE
                            + ", weapon_type_tostring_lowercase=" + SPECIFIC_WEAPON_STR_LOWERCASE
                            + ")");

                        SetNamesAndVerify(CONTEXT_STR);
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
            static const int SWORD_INDEX = 1;
            static const int AXE_INDEX = 2;
            static const int CLUB_INDEX = 3;
            static const int WHIP_INDEX = 4;
            static const int PROJECTILE_INDEX = 5;
            static const int BLADEDSTAFF_INDEX = 6;
            static const int BODY_PART_INDEX = 7;

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
