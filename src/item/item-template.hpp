// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_TEMPLATE_HPP_INCLUDE
#define HEROESPATH_ITEM_ITEM_TEMPLATE_HPP_INCLUDE
//
// item-template.hpp
//
#include "creature/complexity-type.hpp"
#include "item/armor-enum.hpp"
#include "item/element-enum.hpp"
#include "item/equipment-details.hpp"
#include "item/equipment-traits.hpp"
#include "item/misc-enum.hpp"
#include "item/weapon-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/nameof.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    // Contains the most fundamental information about an item, which is used to create many
    // similar items.  All items can fundamentally be defined by three enums: Major/Minor/Misc. This
    // fundamental information about an item is called the ItemTemplate, because thousands of
    // different items can be created from one ItemTemplate.  For example, if Major=Sword,
    // Minor=Falcata, and Misc=Count, there could be many varaitions with elemental enchantments,
    // Named enchantments, or belonging to a Set, etc.
    //
    // Fully complete items can be any combination of Weapon/Armor/Misc (except Weapon+Armor is not
    // allowed).  ItemTemplates are never a combination of any, they are only ever one of the three.
    class ItemTemplate
    {
        // These flag bits are just a way to pre-compute and cache details about the item that are
        // often queried, but keep in mind that all of these flags can be completely determined from
        // the three Major/Minor/Misc enums.  There are valid items that will have none of these
        // bits set.
        struct Flags : public EnumBaseBitField<>
        {
            enum Enum : EnumUnderlying_t
            {
                None = 0,
                Blowpipe = 1 << 0,
                Bow = 1 << 1,
                EitherSpear = 1 << 2,
                Bladed = 1 << 3,
                Pointed = 1 << 4,
                HasForm = 1 << 5,
                HasPixieVersion = 1 << 7,
                Weapon = 1 << 8,
                Invalid = 1 << 9
            };

            static constexpr Enum Last = Invalid;
        };

        // complete equipment constructor used by all equipment
        constexpr ItemTemplate(
            const bool IS_WEAPON,
            const EnumUnderlying_t MAJOR_VALUE,
            const EnumUnderlying_t MINOR_VALUE,
            const ElementList::Enum ELEMENTS_ALLOWED,
            const bool IS_MAJOR_VALID,
            const bool IS_MINOR_VALID,
            const bool HAS_PIXIE_VERSION,
            const bool HAS_FORM,
            const EquipmentDetails & DETAILS) noexcept
            : majorValue_(MAJOR_VALUE)
            , minorValue_(MINOR_VALUE)
            , misc_(Misc::Count)
            , equipmentDetails_(DETAILS)
            , elementsAllowedBase_(ELEMENTS_ALLOWED)
            , flags_(MakeFlagsEquipment(
                  IS_WEAPON,
                  MAJOR_VALUE,
                  MINOR_VALUE,
                  IS_MAJOR_VALID,
                  IS_MINOR_VALID,
                  HAS_PIXIE_VERSION,
                  HAS_FORM))
        {}

    public:
        // do not use, always creates an invalid object, only here for boost::serialization
        constexpr explicit ItemTemplate() noexcept
            : majorValue_(0)
            , minorValue_(0)
            , misc_(Misc::Count)
            , equipmentDetails_()
            , elementsAllowedBase_(ElementList::None)
            , flags_(Flags::None)
        {}

        template <typename T, typename = std::enable_if_t<is_equipment_minor_enum_v<T>>>
        constexpr explicit ItemTemplate(const T MINOR_ENUM) noexcept
            : ItemTemplate(
                equipment_traits<T>::is_weapon,
                equipment_traits<T>::major_value,
                MINOR_ENUM,
                ElementsAllowed<T>(MINOR_ENUM),
                is_major_valid_v<T>,
                IsMinorValid<T>(MINOR_ENUM),
                HasPixieVersion<T>(MINOR_ENUM),
                has_form_v<T>,
                FetchEquipmentDetails<T>(MINOR_ENUM))
        {}

        constexpr ItemTemplate(const Armor::Enum ARMOR_ENUM, const Forms::Enum FORM_ENUM) noexcept
            : ItemTemplate(
                false,
                ARMOR_ENUM,
                FORM_ENUM,
                ElementsAllowed<Armor::Enum, ARMOR_ENUM>(FORM_ENUM),
                is_major_valid_v<Armor::Enum, ARMOR_ENUM>,
                IsMinorValid<Armor::Enum, ARMOR_ENUM>(FORM_ENUM),
                HasPixieVersion<Armor::Enum, ARMOR_ENUM>(FORM_ENUM),
                has_form_v<Armor::Enum, ARMOR_ENUM>,
                FetchEquipmentDetails<Armor::Enum, ARMOR_ENUM>(MINOR_ENUM))
        {}

        constexpr explicit ItemTemplate(const Misc::Enum MISC) noexcept
            : majorValue_(0)
            , minorValue_(0)
            , misc_(MISC)
            , equipmentDetails_()
            , elementsAllowedBase_(Misc::ElementsAllowed(MISC))
            , flags_(MakeFlagsMisc(MISC))
        {}

        constexpr bool IsValid() const noexcept { return !(flags_ & Flags::Invalid); }

        constexpr bool IsOnlyMisc() const noexcept
        {
            return ((misc_ < Misc::Count) && !(flags_ & Flags::Weapon));
        }

        constexpr bool IsOnlyWeapon() const noexcept
        {
            return (!IsOnlyMisc() && (flags_ & Flags::Weapon));
        }

        constexpr bool IsOnlyArmor() const noexcept
        {
            return (!IsOnlyMisc() && !(flags_ & Flags::Weapon));
        }

        constexpr bool IsEquipment() const noexcept { return (IsOnlyWeapon() || IsOnlyArmor()); }

        constexpr Weapon::Enum WeaponType() const noexcept
        {
            return ((IsOnlyWeapon()) ? static_cast<Weapon::Enum>(majorValue_) : Weapon::Count);
        }

        constexpr Armor::Enum ArmorType() const noexcept
        {
            return ((IsOnlyArmor()) ? static_cast<Armor::Enum>(majorValue_) : Armor::Count);
        }

        constexpr Misc::Enum MiscType() const noexcept
        {
            return ((IsOnlyMisc()) ? misc_ : Misc::Count);
        }

        constexpr bool HasForm() const noexcept { return (flags_ & Flags::HasForm); }
        constexpr Forms::Enum FormEnum() const noexcept { return MinorAs<Forms::Enum>(); }

        constexpr EnumUnderlying_t MajorValue() const noexcept { return majorValue_; }
        constexpr EnumUnderlying_t MinorValue() const noexcept { return minorValue_; }

        template <typename T>
        constexpr bool IsMajor(const T MAJOR_ENUM) const noexcept
        {
            return (is_equipment_major_enum_v<T> && (MAJOR_ENUM == majorValue_));
        }

        template <typename T>
        constexpr auto MajorAs() const noexcept
        {
            return static_cast<typename equipment_traits<T>::major_enum_t>(majorValue_);
        }

        template <typename T>
        constexpr bool IsMinor(const T MINOR_ENUM) const noexcept
        {
            if (is_equipment_minor_enum_v<T>)
            {
                return (MINOR_ENUM == minorValue_);
            }
            else
            {
                return false
            }
        }

        template <typename T>
        constexpr auto MinorAs() const noexcept
        {
            if (is_equipment_minor_enum_v<T>)
            {
                return static_cast<typename equipment_traits<T>::minor_enum_t>(minorValue_);
            }
            else
            {
                return equipment_traits<T>::minor_wrapper_t::Count;
            }
        }

        constexpr bool IsSummoning() const noexcept { return Misc::IsSummoning(misc_); }

        constexpr const EquipmentDetails & GetEquipmentDetails() const noexcept
        {
            return equipmentDetails_;
        }

        constexpr bool IsBodyPartMisc() const noexcept { return Misc::IsBodyPart(misc_); }

        constexpr bool IsBodyPartEquipment() const noexcept
        {
            if (IsOnlyWeapon())
            {
                return (Weapon::BodyPart == majorValue_);
            }
            else if (IsOnlyArmor())
            {
                return (Armor::Skin == majorValue_);
            }
            else
            {
                return false;
            }
        }

        constexpr MaterialNameStyle::Enum GetMaterialNameStyle() const noexcept
        {
            if (IsOnlyMisc())
            {
                return MaterialNameStyle::Misc;
            }
            else
            {
                return equipmentDetails_.material_name_type;
            }
        }

        constexpr bool HasPixieVersion() const noexcept
        {
            return (flags_ & Flags::HasPixieVersion);
        }

        // allowed elemental enchantments for full Items also depends on Set::Enum and Named::Enum
        constexpr ElementList::Enum ElementsAllowedBase() const noexcept
        {
            return elementsAllowedBase_;
        }

        const std::string_view MajorString() const
        {
            if (IsOnlyWeapon())
            {
                return NAMEOF_ENUM(WeaponType());
            }
            else if (IsOnlyArmor())
            {
                return NAMEOF_ENUM(ArmorType());
            }
            else
            {
                return "";
            }
        }

        const std::string_view MajorName() const
        {
            if (IsOnlyWeapon())
            {
                return Weapon::Name(WeaponType());
            }
            else if (IsOnlyArmor())
            {
                return MajorString();
            }
            else
            {
                return "";
            }
        }

        const std::string MinorString() const
        {
            if (IsOnlyWeapon())
            {
                return std::string(MinorStringWeapon());
            }
            else if (IsOnlyArmor())
            {
                return std::string(MinorStringArmor());
            }
            else
            {
                return "";
            }
        }

        const std::string MinorName() const
        {
            if (IsOnlyWeapon())
            {
                return std::string(MinorNameWeapon());
            }
            else if (IsOnlyArmor())
            {
                return std::string(MinorNameArmor());
            }
            else
            {
                return "";
            }
        }

        const std::string SystemName() const
        {
            std::string str;

            if (IsEquipment())
            {
                str.reserve(32);
                str += MajorString();
                str += MinorString();
            }

            return str;
        }

        const std::string ReadableName() const
        {
            std::string str;

            if (IsEquipment())
            {
                str.reserve(32);

                if (HasForm())
                {
                    str += NAMEOF_ENUM(FormEnum());
                    str += " ";
                    str += MajorName();
                }
                else
                {
                    str += MinorName();
                }
            }

            return str;
        }

        const std::string ToString() const
        {
            std::string str;
            str.reserve(128);
            str += "ItemTemplate{";

            if (!IsValid())
            {
                str += "INVALID, ";
            }

            if (IsOnlyMisc())
            {
                str += "OnlyMisc=";
                str += NAMEOF_ENUM(misc_);
                str += ", ";
            }

            if (IsOnlyWeapon())
            {
                str += "OnlyWeapon, ";
            }

            if (IsOnlyArmor())
            {
                str += "OnlyArmor, ";
            }

            str += "major=";
            str += std::to_string(majorValue_);
            str += "/";
            str += MajorString();
            str += ", minor=";
            str += std::to_string(minorValue_);
            str += "/";
            str += MinorString();
            str += ", elements_allowed=";
            str += EnumUtil<ElementList>::ToString(elementsAllowed_, EnumStringHow(Wrap::Yes, "/"));
            str += ", flags=";
            str += EnumUtil<Flags>::ToString(flags_);
            str += "}";

            str += "}";
            return str;
        }

        const std::string SystemNameBase() const
        {
            if (IsOnlyWeapon())
            {
                return std::string(weapon_.SubTypeString());
            }
            else if (IsOnlyArmor())
            {
                return armor_.SubTypeString(true);
            }
            else
            {
                return std::string(NAMEOF_ENUM(misc_));
            }
        }

        const std::string ReadableNameBase() const
        {
            if (IsOnlyWeapon())
            {
                return std::string(weapon_.SubTypeName());
            }
            else if (IsOnlyArmor())
            {
                return armor_.SubTypeName();
            }
            else
            {
                return std::string(Misc::Name(misc_));
            }
        }

        constexpr const std::string_view EquipmentDescription() const noexcept
        {
            if (IsOnlyWeapon())
            {
                const auto SUB_TYPE_VALUE = weapon_.SubTypeValue();

                switch (weapon_.Type())
                {
                    case Weapon::Axe: return Axes::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Bladedstaff: return Bladedstaffs::descriptions[SUB_TYPE_VALUE];
                    case Weapon::BodyPart: return BodyPartWeapons::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Club: return Clubs::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Knife: return Knifes::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Projectile: return Projectiles::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Staff: return Staffs::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Sword: return Swords::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Whip: return Whips::descriptions[SUB_TYPE_VALUE];
                    case Weapon::Count:
                    default: return "";
                }
            }
            else if (IsOnlyArmor())
            {
                switch (armor_.Type())
                {
                    case Armor::Aventail:
                    case Armor::Boot:
                    case Armor::Bracer:
                    case Armor::Gauntlet:
                    case Armor::Pant:
                    case Armor::Shirt: return Forms::Description(armor_.Type(), armor_.Form());

                    case Armor::Skin: return Skins::descriptions[armor_.SubTypeValue()];
                    case Armor::Cover: return Covers::descriptions[armor_.SubTypeValue()];
                    case Armor::Helm: return Helms::descriptions[armor_.SubTypeValue()];
                    case Armor::Shield: return Shields::descriptions[armor_.SubTypeValue()];
                    case Armor::Count:
                    default: return "";
                }
            }
            else
            {
                return "";
            }
        }

        friend constexpr bool operator==(const ItemTemplate & L, const ItemTemplate & R) noexcept;
        friend constexpr bool operator<(const ItemTemplate & L, const ItemTemplate & R) noexcept;

    private:
        const std::string_view MinorStringWeapon() const
        {
            switch (WeaponType())
            {
                case Weapon::Axe: return NAMEOF_ENUM(MinorAs<Axes::Enum>());
                case Weapon::Bladedstaff: return NAMEOF_ENUM(MinorAs<Bladedstaffs::Enum>());
                case Weapon::BodyPart: return NAMEOF_ENUM(MinorAs<BodyPartWeapons::Enum>());
                case Weapon::Club: return NAMEOF_ENUM(MinorAs<Clubs::Enum>());
                case Weapon::Knife: return NAMEOF_ENUM(MinorAs<Knifes::Enum>());
                case Weapon::Projectile: return NAMEOF_ENUM(MinorAs<Projectiles::Enum>());
                case Weapon::Staff: return NAMEOF_ENUM(MinorAs<Staffs::Enum>());
                case Weapon::Sword: return NAMEOF_ENUM(MinorAs<Swords::Enum>());
                case Weapon::Whip: return NAMEOF_ENUM(MinorAs<Whips::Enum>());

                case Weapon::Count: return "Count";
                default: return "EquipmentTemplate::MinorStringWeapon()_weapon_enum_out_of_range";
            }
        }

        const std::string_view MinorStringArmor() const
        {
            switch (ArmorType())
            {
                case Armor::Aventail:
                case Armor::Boot:
                case Armor::Bracer:
                case Armor::Gauntlet:
                case Armor::Pant:
                case Armor::Shirt: return NAMEOF_ENUM(FormEnum());

                case Armor::Skin: return NAMEOF_ENUM(MinorAs<Skins::Enum>());
                case Armor::Helm: return NAMEOF_ENUM(MinorAs<Helms::Enum>());
                case Armor::Shield: return NAMEOF_ENUM(MinorAs<Shields::Enum>());
                case Armor::Cover: return NAMEOF_ENUM(MinorAs<Covers::Enum>());

                case Armor::Count: return "Count";
                default: return "EquipmentTemplate::MinorStringArmor()_armor_enum_was_out_of_range";
            }
        }

        const std::string_view MinorNameWeapon() const
        {
            switch (WeaponType())
            {
                case Weapon::Bladedstaff: return Bladedstaffs::Name(MinorAs<Bladedstaffs>());
                case Weapon::Club: return Clubs::Name(MinorAs<Clubs>());
                case Weapon::Whip: return Whips::Name(MinorAs<Whips>());

                case Weapon::Axe:
                case Weapon::BodyPart:
                case Weapon::Knife:
                case Weapon::Projectile:
                case Weapon::Staff:
                case Weapon::Sword: return MinorStringWeapon();

                case Weapon::Count: return "Count";
                default: return "EquipmentTemplate::MinorNameWeapon()_weapon_was_out_of_range";
            }
        }

        const std::string_view MinorNameArmor() const
        {
            switch (ArmorType())
            {
                case Armor::Aventail:
                case Armor::Boot:
                case Armor::Bracer:
                case Armor::Gauntlet:
                case Armor::Pant:
                case Armor::Shirt:
                case Armor::Skin:
                case Armor::Cover: return MinorStringArmor();

                case Armor::Helm: return Helms::Name(MinorAs<Helms::Enum>());
                case Armor::Shield: return Shields::Name(MinorAs<Shields::Enum>());

                case Armor::Count: return "Count";
                default: return "EquipmentTemplate::MinorNameArmor()_armor_enum_out_of_range";
            }
        }

        constexpr Flags::Enum MakeFlagsMisc(const Misc::Enum MISC) noexcept
        {
            Flags::Enum flags = Flags::None;

            if (Misc::HasPixieVersion(MISC))
            {
                flags |= Flags::HasPixieVersion;
            }

            if (MISC >= Misc::Count)
            {
                flags |= Flags::Invalid;
            }

            return flags;
        }

        constexpr Flags::Enum MakeFlagsEquipment(
            const bool IS_WEAPON,
            const EnumUnderlying_t MAJOR_VALUE,
            const EnumUnderlying_t MINOR_VALUE,
            const bool IS_MAJOR_VALID,
            const bool IS_MINOR_VALID,
            const bool HAS_PIXIE_VERSION,
            const bool HAS_FORM) noexcept
        {
            Flags::Enum flags = Flags::None;

            if (HAS_PIXIE_VERSION)
            {
                flags |= Flags::HasPixieVersion;
            }

            if (HAS_FORM && (MINOR_VALUE < Forms::Count))
            {
                flags |= Flags::HasForm;
            }

            if (!IS_MAJOR_VALID || !IS_MINOR_VALID)
            {
                flags |= Flags::Invalid;
            }
            else
            {
                if (IS_WEAPON)
                {
                    flags |= MakeFlagsWeapon(static_cast<Weapon::Enum>(MAJOR_VALUE), MINOR_VALUE);
                }
            }

            if ((flags & Flags::Weapon) && (flags & Flags::HasForm))
            {
                flags |= Flags::Invalid;
            }

            return flags;
        }

        constexpr Flags::Enum MakeFlagsWeapon(
            const Weapon::Enum WEAPON_ENUM, const EnumUnderlying_t MINOR_VALUE) noexcept
        {
            Flags::Enum flags = Flags::Weapon;

            if (WEAPON_ENUM == Weapon::Projectile)
            {
                if (MINOR_VALUE == Projectiles::Blowpipe)
                {
                    flags |= Flags::Blowpipe;
                }
                else if (
                    (MINOR_VALUE == Projectiles::Shortbow) || (MINOR_VALUE == Projectiles::Longbow)
                    || (MINOR_VALUE == Projectiles::Compositebow))
                {
                    flags |= Flags::Bow;
                }
            }

            if (WEAPON_ENUM == Weapon::Bladedstaff)
            {
                if (MINOR_VALUE != Bladedstaffs::Scythe)
                {
                    flags |= Flags::Pointed;
                }

                if ((MINOR_VALUE == Bladedstaffs::Spear)
                    || (MINOR_VALUE == Bladedstaffs::ShortSpear))
                {
                    flags |= Flags::EitherSpear;
                }
                else
                {
                    flags |= Flags::Bladed;
                }
            }

            if ((WEAPON_ENUM == Weapon::Knife) || (WEAPON_ENUM == Weapon::Sword)
                || (WEAPON_ENUM == Weapon::Axe))
            {
                flags |= Flags::Bladed;
            }

            if ((WEAPON_ENUM == Weapon::Knife) || (WEAPON_ENUM == Weapon::Sword))
            {
                flags |= Flags::Pointed;
            }

            if (misc::CountBits(flags & (Flags::Blowpipe | Flags::Bow | Flags::EitherSpear)) > 1)
            {
                flags |= Flags::Invalid;
            }

            return flags;
        }

    private:
        // these members are all that is needed to uniquely identify an template
        EnumUnderlying_t majorValue_;
        EnumUnderlying_t minorValue_;
        Misc::Enum misc_;

        // these members are completely generated from the members above
        // so there is no need to check these when comparing
        EquipmentDetails equipmentDetails_;
        ElementList::Enum elementsAllowedBase_;
        Flags::Enum flags_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & majorValue_;
            ar & minorValue_;
            ar & misc_;
            ar & equipmentDetails_;
            ar & elementsAllowedBase_;
            ar & flags_;
        }
    };

    // only major/minor/misc are considered, see comment above
    constexpr bool operator==(const ItemTemplate & L, const ItemTemplate & R) noexcept
    {
        return (
            (L.majorValue_ == R.majorValue_) && (L.minorValue_ == R.minorValue_)
            && (L.misc_ == R.misc_));
    }

    // only major/minor/misc are considered, see comment above
    constexpr bool operator!=(const ItemTemplate & L, const ItemTemplate & R) noexcept
    {
        return !(L == R);
    }

    // only major/minor/misc are considered, see comment above
    constexpr bool operator<(const ItemTemplate & L, const ItemTemplate & R) noexcept
    {
        if (L.majorValue_ != R.majorValue_)
        {
            return (L.majorValue_ < R.majorValue_);
        }
        else if (L.minorValue_ != R.minorValue_)
        {
            return (L.minorValue_ < R.minorValue_);
        }
        else
        {
            return (L.misc_ < R.misc_);
        }
    }

    using ItemTemplateVec_t = std::vector<ItemTemplate>;

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_TEMPLATE_HPP_INCLUDE
