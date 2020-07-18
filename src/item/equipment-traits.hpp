// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_EQUIPMENT_TRAITS_HPP_INCLUDED
#define HEROESPATH_ITEM_EQUIPMENT_TRAITS_HPP_INCLUDED
//
// equipment-traits.hpp
//
#include "equipment-details.hpp"
#include "item/armor-enum.hpp"
#include "item/weapon-enum.hpp"

namespace heroespath
{
namespace item
{

    // Equipment Items are only weapons or armor.  All Equipment items can be completely
    // defined with only two enums: The Major Enum and the Minor Enum.  (i.e. Sword/Rapier or
    // Bracer/Plate)  Each of those enums are wrapped with structs, just like all other enums in the
    // game.  So every enum has three parts: Wrapper Type (Wrapper_t), Enum Type (Wrapper_t::Enum),
    // and the value of that Enum.  (i.e. Swords/Swords::Enum/Swords::Falcata, and
    // Skins/Skins::Enum/Skins::Flesh)
    //
    //  So all equipment items have:
    //      - major_wrapper_t
    //      - major_enum_t          (which is really justmajor_wrapper_t::Enum)
    //      - major_enum_value
    //      - minor_wrapper_t
    //      - minor_enum_t          (which is really just minor_wrapper_t::Enum)
    //      - minor_enum_value
    //
    // The problem is that sometimes you only know one of the types and need the others.  Solving
    // that problem is the purpose of this file.  Below are helpful tools that allow you to quickly
    // and easily get everything you need to know about an equipment item, even if you only know one
    // or two of the enum types or values.

    template <typename T = void, Armor::Enum ARMOR_TYPE = Armor::Count>
    struct equipment_traits
    {
        equipment_traits() = delete;
    };

    template <>
    struct equipment_traits<Weapon>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = void;
        using minor_enum_t = void;
        static constexpr const major_enum_t major_value = major_wrapper_t::Count;
    };

    template <>
    struct equipment_traits<Weapon::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = void;
        using minor_enum_t = void;
        static constexpr const major_enum_t major_value = major_wrapper_t::Count;
    };

    //

    template <>
    struct equipment_traits<Axes>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Axes;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Axe;
    };

    template <>
    struct equipment_traits<Axes::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Axes;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Axe;
    };

    template <>
    struct equipment_traits<Bladedstaffs>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Bladedstaffs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Bladedstaff;
    };

    template <>
    struct equipment_traits<Bladedstaffs::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Bladedstaffs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Bladedstaff;
    };

    template <>
    struct equipment_traits<BodyPartWeapons>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = BodyPartWeapons;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::BodyPart;
    };

    template <>
    struct equipment_traits<BodyPartWeapons::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = BodyPartWeapons;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::BodyPart;
    };

    template <>
    struct equipment_traits<Clubs>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Clubs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Club;
    };

    template <>
    struct equipment_traits<Clubs::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Clubs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Club;
    };

    template <>
    struct equipment_traits<Knifes>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Knifes;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Knife;
    };

    template <>
    struct equipment_traits<Knifes::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Knifes;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Knife;
    };

    template <>
    struct equipment_traits<Projectiles>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Projectiles;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Projectile;
    };

    template <>
    struct equipment_traits<Projectiles::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Projectiles;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Projectile;
    };

    template <>
    struct equipment_traits<Staffs>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Staffs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Staff;
    };

    template <>
    struct equipment_traits<Staffs::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Staffs;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Staff;
    };

    template <>
    struct equipment_traits<Swords>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Swords;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Sword;
    };

    template <>
    struct equipment_traits<Swords::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Swords;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Sword;
    };

    template <>
    struct equipment_traits<Whips>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Whips;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Whip;
    };

    template <>
    struct equipment_traits<Whips::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = true;
        using major_wrapper_t = Weapon;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Whips;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Whip;
    };

    //

    template <>
    struct equipment_traits<Armor>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = void;
        using minor_enum_t = void;
        static constexpr const major_enum_t major_value = major_wrapper_t::Count;
    };

    template <>
    struct equipment_traits<Armor::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = void;
        using minor_enum_t = void;
        static constexpr const major_enum_t major_value = major_wrapper_t::Count;
    };

    //

    template <>
    struct equipment_traits<Armor::Enum, Armor::Aventail>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Aventail;
    };

    template <>
    struct equipment_traits<Armor::Enum, Armor::Boot>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Boot;
    };

    template <>
    struct equipment_traits<Armor::Enum, Armor::Bracer>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Bracer;
    };

    template <>
    struct equipment_traits<Armor::Enum, Armor::Gauntlet>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Gauntlet;
    };

    template <>
    struct equipment_traits<Armor::Enum, Armor::Pant>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Pant;
    };

    template <>
    struct equipment_traits<Armor::Enum, Armor::Shirt>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Shirt;
    };

    //
    template <>
    struct equipment_traits<Skins>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Skins;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Skin;
    };

    template <>
    struct equipment_traits<Skins::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Skins;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Skin;
    };

    template <>
    struct equipment_traits<Covers>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Covers;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Cover;
    };

    template <>
    struct equipment_traits<Covers::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Covers;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Cover;
    };

    template <>
    struct equipment_traits<Helms>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Helms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Helm;
    };

    template <>
    struct equipment_traits<Helms::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Helms;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Helm;
    };

    template <>
    struct equipment_traits<Shields>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Shields;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Shield;
    };

    template <>
    struct equipment_traits<Shields::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        using major_wrapper_t = Armor;
        using major_enum_t = major_wrapper_t::Enum;
        using minor_wrapper_t = Shields;
        using minor_enum_t = minor_wrapper_t::Enum;
        static constexpr const major_enum_t major_value = major_wrapper_t::Shield;
    };

    //

    template <>
    struct equipment_traits<Forms>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        // using major_wrapper_t = void;
        // using major_enum_t = void;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        // static constexpr const major_enum_t major_value = major_wrapper_t::Shield;
    };

    template <>
    struct equipment_traits<Forms::Enum>
    {
        equipment_traits() = delete;
        static constexpr const bool is_weapon = false;
        // using major_wrapper_t = void;
        // using major_enum_t = void;
        using minor_wrapper_t = Forms;
        using minor_enum_t = minor_wrapper_t::Enum;
        // static constexpr const major_enum_t major_value = major_wrapper_t::Shield;
    };

    //

    template <typename T>
    constexpr const bool is_equipment_major_enum_v
        = (misc::are_enum_v<T> && misc::are_same_v<T, equipment_traits<T>::major_enum_t>);

    template <typename T>
    constexpr const bool is_weapon_major_enum_v
        = (equipment_traits<T>::is_weapon && is_equipment_major_enum_v<T>);

    template <typename T>
    constexpr const bool is_armor_major_enum_v
        = (!equipment_traits<T>::is_weapon && is_equipment_major_enum_v<T>);

    template <typename T>
    constexpr const bool is_equipment_minor_enum_v
        = (misc::are_enum_v<T> && misc::are_same_v<T, equipment_traits<T>::minor_enum_t>);

    template <typename T>
    constexpr const bool is_weapon_minor_enum_v
        = (equipment_traits<T>::is_weapon && is_equipment_minor_enum_v<T>);

    template <typename T>
    constexpr const bool is_armor_minor_enum_v
        = (!equipment_traits<T>::is_weapon && is_equipment_minor_enum_v<T>);

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr const bool has_form_v
        = misc::are_same_v < equipment_traits<T, ARMOR_ENUM>::minor_enum_t;

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr const bool is_major_valid_v
        = (equipment_traits<T, ARMOR_ENUM>::major_value
           < equipment_traits<T, ARMOR_ENUM>::major_wrapper_t::Count);

    // IsMinorValid()

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr std::enable_if_t<!has_form_v<T>, bool> IsMinorValid(const T MINOR_ENUM)
    {
        return (MINOR_ENUM < equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::Count);
    }

    template <typename T, Armor::Enum ARMOR_ENUM>
    constexpr std::enable_if_t<has_form_v<T, ARMOR_ENUM>, bool>
        IsMinorValid(const Forms::Enum MINOR_ENUM)
    {
        return (MINOR_ENUM < equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::Count);
    }

    // ElementsAllowed()

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr std::enable_if_t<!has_form_v<T, ARMOR_ENUM>, ElementList::Enum>
        ElementsAllowed(const T MINOR_ENUM) noexcept
    {
        return equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::ElementsAllowed(MINOR_ENUM);
    }

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr std::enable_if_t<has_form_v<T, ARMOR_ENUM>, ElementList::Enum>
        ElementsAllowed(const Forms::Enum MINOR_ENUM = Forms::Count) noexcept
    {
        return Forms::ElementsAllowed(ARMOR_ENUM, MINOR_ENUM);
    }

    // HasPixieVersion()

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr std::enable_if_t<!has_form_v<T>, ElementList::Enum>
        HasPixieVersion(const T MINOR_ENUM) noexcept
    {
        return equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::HasPixieVersion(MINOR_ENUM);
    }

    template <typename T, Armor::Enum ARMOR_ENUM>
    constexpr std::enable_if_t<has_form_v<T, ARMOR_ENUM>, ElementList::Enum>
        HasPixieVersion(const Forms::Enum MINOR_ENUM) noexcept
    {
        return Forms::HasPixieVersion(ARMOR_ENUM, MINOR_ENUM);
    }

    // FetchEquipmentDetails()

    template <typename T, Armor::Enum ARMOR_ENUM = Armor::Count>
    constexpr std::enable_if_t<!has_form_v<T>, const EquipmentDetails &>
        FetchEquipmentDetails(const T MINOR_ENUM)
    {
        return equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::details[MINOR_ENUM];
    }

    template <typename T, Armor::Enum ARMOR_ENUM>
    constexpr std::enable_if_t<has_form_v<T, ARMOR_ENUM>, const EquipmentDetails &>
        FetchEquipmentDetails(const Forms::Enum MINOR_ENUM)
    {
        return equipment_traits<T, ARMOR_ENUM>::minor_wrapper_t::Details(ARMOR_ENUM, MINOR_ENUM);
    }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_EQUIPMENT_TRAITS_HPP_INCLUDED
