// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_PROFILE_TEMPLATE_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_PROFILE_TEMPLATE_HPP_INCLUDED
//
// item-template-factory.hpp
//
#include "item/armor-enum.hpp"
#include "item/item-template.hpp"
#include "item/misc-enum.hpp"
#include "item/named-enum.hpp"
#include "item/set-enum.hpp"
#include "item/weapon-enum.hpp"
#include "misc/type-helpers.hpp"
#include "misc/vector-map.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace heroespath
{
namespace item
{

    struct ItemTemplateFactory
    {
    public:
        ItemTemplateFactory();
        ~ItemTemplateFactory();

        ItemTemplateFactory(const ItemTemplateFactory &) = delete;
        ItemTemplateFactory(ItemTemplateFactory &&) = delete;
        ItemTemplateFactory & operator=(const ItemTemplateFactory &) = delete;
        ItemTemplateFactory & operator=(ItemTemplateFactory &&) = delete;

        static misc::NotNull<ItemTemplateFactory *> Instance();
        static void Create();
        static void Destroy();

        void Initialize();

        template <typename WeaponSubEnum_t>
        const ItemTemplate & MakeWeapon(const WeaponSubEnum_t SUB_TYPE_ENUM) const
        {
            return weapons_.at(WeaponIndex(SUB_TYPE_ENUM));
        }

        inline const ItemTemplateVec_t & MakeAllWeapons() const { return weapons_; }

        inline const ItemTemplate &
            MakeArmor(const Armor::Enum TYPE, const Forms::Enum FORM = Forms::Count) const
        {
            return armor_.at(ArmorIndexByType(TYPE, FORM));
        }

        template <typename ArmorSubEnum_t>
        const ItemTemplate & MakeArmor(const ArmorSubEnum_t SUB_TYPE_ENUM) const
        {
            return armor_.at(ArmorIndexBySubType(SUB_TYPE_ENUM));
        }

        inline const ItemTemplateVec_t & MakeAllArmor() const { return armor_; }

        inline const ItemTemplate & MakeMisc(const Misc::Enum MISC) const { return misc_.at(MISC); }

        inline const ItemTemplateVec_t & MakeAllMisc() const { return misc_; }

        template <typename SubEnum_t>
        std::enable_if_t<is_weapon_sub_type_v<SubEnum_t>, const ItemTemplate &>
            MakeFromSubType(const SubEnum_t SUB_TYPE_ENUM) const
        {
            return MakeWeapon(SUB_TYPE_ENUM);
        }

        template <typename SubEnum_t>
        std::enable_if_t<is_armorSpecifics_sub_type_v<SubEnum_t>, const ItemTemplate &>
            MakeFromSubType(const SubEnum_t SUB_TYPE_ENUM) const
        {
            return MakeArmor(SUB_TYPE_ENUM);
        }

        template <typename SubEnum_t>
        std::enable_if_t<misc::are_same_v<SubEnum_t, Misc::Enum>, const ItemTemplate &>
            MakeFromSubType(const SubEnum_t SUB_TYPE_ENUM) const
        {
            return MakeMisc(SUB_TYPE_ENUM);
        }

        const misc::VectorMap<Named::Enum, ItemTemplateVec_t> & MakeAllNamed() const
        {
            return namedToTemplates_;
        }

        const misc::VectorMap<Set::Enum, ItemTemplateVec_t> & MakeAllSet() const
        {
            return setToTemplates_;
        }

    private:
        template <typename EnumWrapper_t>
        void MakeAndAppendWeaponSpecific(std::vector<WeaponTemplate> & templates) const
        {
            for (EnumUnderlying_t i(0); i < EnumWrapper_t::Count; ++i)
            {
                const auto ENUM = static_cast<typename EnumWrapper_t::Enum>(i);
                templates.push_back(WeaponTemplate(ENUM));
            }
        }

        template <typename CompareWrapper_t, typename OrigSubEnum_t>
        bool WeaponIndexHelper(const OrigSubEnum_t SUB_ENUM_ORIG, std::size_t & index) const
            noexcept
        {
            if constexpr (misc::are_same_v<OrigSubEnum_t, typename CompareWrapper_t::Enum>)
            {
                index += SUB_ENUM_ORIG;
                return true;
            }
            else
            {
                index += CompareWrapper_t::Count;
                return false;
            }
        }

        template <typename SubEnum_t>
        std::size_t WeaponIndex(const SubEnum_t SUB_ENUM) const
        {
            std::size_t index = 0;

            // clang-format off
            if (WeaponIndexHelper<Axes>(SUB_ENUM, index))            { return index; }
            if (WeaponIndexHelper<Bladedstaffs>(SUB_ENUM, index))    { return index; }
            if (WeaponIndexHelper<BodyPartWeapons>(SUB_ENUM, index)) { return index; }
            if (WeaponIndexHelper<Clubs>(SUB_ENUM, index))           { return index; }
            if (WeaponIndexHelper<Knifes>(SUB_ENUM, index))          { return index; }
            if (WeaponIndexHelper<Projectiles>(SUB_ENUM, index))     { return index; }
            if (WeaponIndexHelper<Staffs>(SUB_ENUM, index))          { return index; }
            if (WeaponIndexHelper<Swords>(SUB_ENUM, index))          { return index; }
            if (WeaponIndexHelper<Whips>(SUB_ENUM, index))           { return index; }
            // clang-format on

            // should never get here
            return 0;
        }

        template <typename Function_t>
        ItemTemplateVec_t MakeAllWeaponsIf(Function_t function) const
        {
            ItemTemplateVec_t profiles;
            profiles.reserve(16);

            for (const auto & WEAPON_SPECIFIC_PROFILE : weaponSpecifics_)
            {
                if (function(WEAPON_SPECIFIC_PROFILE))
                {
                    profiles.emplace_back(ItemTemplate(WEAPON_SPECIFIC_PROFILE));
                }
            }

            return profiles;
        }

        ItemTemplateVec_t MakeAllWeaponsOfType(const Weapon::Enum TYPE_ENUM) const;

        void MakeAndAppendArmorSpecificWithForm(
            const Armor::Enum ARMOR_TYPE, std::vector<ArmorTemplate> & templates) const;

        template <typename EnumWrapper_t>
        void MakeAndAppendArmorSpecificWithoutForm(std::vector<ArmorTemplate> & templates) const
        {
            for (EnumUnderlying_t i(0); i < EnumWrapper_t::Count; ++i)
            {
                const auto ENUM = static_cast<typename EnumWrapper_t::Enum>(i);
                templates.push_back(ArmorTemplate(ENUM));
            }
        }

        std::size_t
            ArmorIndexByType(const Armor::Enum TYPE, const Forms::Enum FORM = Forms::Count) const;

        inline std::size_t ArmorIndexBySubType(const Skins::Enum SKIN) const
        {
            return (
                ((static_cast<std::size_t>(Armor::Shirt) + 1)
                 * static_cast<std::size_t>(Forms::Count))
                + static_cast<std::size_t>(SKIN));
        }

        inline std::size_t ArmorIndexBySubType(const Covers::Enum COVER) const
        {
            return (ArmorIndexBySubType(Skins::Count) + COVER);
        }

        inline std::size_t ArmorIndexBySubType(const Helms::Enum HELM) const
        {
            return (ArmorIndexBySubType(Covers::Count) + HELM);
        }

        inline std::size_t ArmorIndexBySubType(const Shields::Enum SHIELD) const
        {
            return (ArmorIndexBySubType(Helms::Count) + SHIELD);
        }

        template <typename Function_t>
        ItemTemplateVec_t MakeAllArmorIf(Function_t function) const
        {
            ItemTemplateVec_t profiles;
            profiles.reserve(16);

            for (const auto & ARMOR_PROFILE : armorSpecifics_)
            {
                if (function(ARMOR_PROFILE))
                {
                    profiles.emplace_back(ItemTemplate(ARMOR_PROFILE));
                }
            }

            return profiles;
        }

        ItemTemplateVec_t MakeAllArmorOfType(const Armor::Enum ARMOR_TYPE) const;
        ItemTemplateVec_t MakeAllArmorOfForm(const Forms::Enum FORM) const;

        const ItemTemplateVec_t MakeNamed(const Named::Enum NAMED_TYPE) const;
        const ItemTemplateVec_t MakeSet(const Set::Enum SET_TYPE) const;

    private:
        static std::unique_ptr<ItemTemplateFactory> instanceUPtr_;

        std::vector<WeaponTemplate> weaponSpecifics_;
        std::vector<ArmorTemplate> armorSpecifics_;

        std::vector<ItemTemplate> weapons_;
        std::vector<ItemTemplate> armor_;
        std::vector<ItemTemplate> misc_;

        misc::VectorMap<Named::Enum, ItemTemplateVec_t> namedToTemplates_;
        misc::VectorMap<Set::Enum, ItemTemplateVec_t> setToTemplates_;
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_PROFILE_TEMPLATE_HPP_INCLUDED
