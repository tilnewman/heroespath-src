// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_ITEM_HPP_INCLUDED
#define HEROESPATH_ITEM_ITEM_HPP_INCLUDED
//
// item.hpp
//
#include "creature/enchantment.hpp"
#include "item/item-profile.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    class Item
    {
    public:
        Item() = default;
        Item(const std::string & NAME, const std::string & DESC, const ItemProfile & PROFILE);
        ~Item();

        Item(const Item &) = delete;
        Item(Item &&) = delete;
        Item & operator=(const Item &) = delete;
        Item & operator=(Item &&) = delete;

        const std::string Name() const;
        const std::string Desc() const;
        const std::string ShortName() const; // Name() without "Pixie" or materials
        const std::string BaseName() const;
        const std::string ImagePath() const { return imageFullPath_; }
        const std::string ToString() const;

        constexpr Category::Enum Category() const noexcept { return category_; }
        constexpr bool IsBroken() const noexcept { return HasCategory(Category::Broken); }
        constexpr bool IsUseable() const noexcept { return HasCategory(Category::Useable); }
        constexpr bool IsEquipable() const noexcept { return HasCategory(Category::Equipable); }
        constexpr bool IsWearable() const noexcept { return HasCategory(Category::Wearable); }
        constexpr bool IsOneHanded() const noexcept { return HasCategory(Category::OneHanded); }
        constexpr bool IsTwoHanded() const noexcept { return HasCategory(Category::TwoHanded); }

        constexpr bool HasCategory(const Category::Enum CATEGORY) const noexcept
        {
            return ((category_ & CATEGORY) > 0);
        }

        void AddCategory(const Category::Enum ENUM)
        {
            category_ = static_cast<Category::Enum>(category_ | ENUM);
        }

        constexpr bool IsRigid() const noexcept
        {
            return Material::IsRigid(profile_.MaterialPrimary());
        }

        constexpr bool IsBloody() const noexcept
        {
            return Material::IsBloody(profile_.MaterialPrimary(), profile_.MaterialSecondary());
        }

        constexpr float FireDamageRatio() const noexcept
        {
            return Material::FireDamageRatio(
                profile_.MaterialPrimary(), profile_.MaterialSecondary());
        }

        constexpr bool IsMetal() const noexcept
        {
            return Material::IsMetal(profile_.MaterialPrimary());
        }

        constexpr bool IsJeweled() const noexcept
        {
            return Material::IsFancyJewel(profile_.MaterialSecondary());
        }

        bool HasEnchantments() const { return !enchantmentsPVec_.empty(); }
        const creature::EnchantmentPVec_t & Enchantments() const { return enchantmentsPVec_; }

        bool IsMagical() const
        {
            return (!IsBroken() && (HasEnchantments() || profile_.IsMagical()));
        }

        constexpr bool IsPixie() const noexcept { return profile_.IsPixie(); }
        constexpr Set::Enum SetType() const noexcept { return profile_.SetType(); }
        constexpr Named::Enum NamedType() const noexcept { return profile_.NamedType(); }
        constexpr Element::Enum ElementType() const noexcept { return profile_.ElementType(); }
        constexpr Misc::Enum MiscType() const noexcept { return profile_.MiscType(); }
        constexpr bool IsQuest() const noexcept { return profile_.IsQuest(); }
        constexpr bool IsWeapon() const noexcept { return profile_.IsWeapon(); }
        constexpr bool IsArmor() const noexcept { return profile_.IsArmor(); }
        constexpr bool IsMisc() const noexcept { return profile_.IsMisc(); }
        constexpr bool IsUnique() const noexcept { return profile_.IsUnique(); }
        constexpr bool IsNamed() const noexcept { return profile_.IsNamed(); }
        constexpr bool IsSet() const noexcept { return profile_.IsSet(); }
        constexpr bool IsElemental() const noexcept { return profile_.IsElemental(); }
        constexpr bool IsBodyPart() const noexcept { return profile_.IsBodyPart(); }
        constexpr Armor_t ArmorRating() const noexcept { return profile_.ArmorRating(); }
        constexpr Coin_t Price() const noexcept { return profile_.Price(); }
        constexpr Weight_t Weight() const noexcept { return profile_.Weight(); }
        constexpr bool IsReligious() const noexcept { return profile_.IsReligious(); }
        constexpr Health_t DamageMin() const noexcept { return profile_.DamageMin(); }
        constexpr Health_t DamageMax() const noexcept { return profile_.DamageMax(); }
        constexpr Armor::Enum ArmorType() const noexcept { return profile_.ArmorType(); }
        constexpr Weapon::Enum WeaponType() const noexcept { return profile_.WeaponType(); }

        constexpr bool CanSummon() const noexcept { return profile_.CanSummon(); }

        constexpr bool HasElementType(const Element::Enum ENUM) const noexcept
        {
            return profile_.HasElementType(ENUM);
        }

        constexpr Material::Enum MaterialPrimary() const noexcept
        {
            return profile_.MaterialPrimary();
        }

        constexpr Material::Enum MaterialSecondary() const noexcept
        {
            return profile_.MaterialSecondary();
        }

        constexpr const creature::SummonInfo SummoningInfo() const noexcept
        {
            return profile_.SummoningInfo();
        }

        constexpr bool MustBePixieVersionForPixiesToEquip() const noexcept
        {
            return profile_.MustBePixieVersionForPixiesToEquip();
        }

        constexpr const WeaponTemplate & WeaponInfo() const noexcept
        {
            return profile_.WeaponInfo();
        }

        constexpr const ArmorTemplate & ArmorInfo() const noexcept { return profile_.ArmorInfo(); }

        constexpr MaterialNameStyle::Enum GetMaterialNameStyle() const noexcept
        {
            return profile_.GetMaterialNameStyle();
        }

        constexpr const ItemProfile & Profile() const noexcept { return profile_; }

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

        friend bool operator<(const Item & L, const Item & R);
        friend bool operator==(const Item & L, const Item & R);

    private:
        const std::string ReadableName() const;
        const std::string ComposeName(const std::string & ROOT_NAME) const;

    protected:
        std::string name_ = {};
        std::string desc_ = {};
        ItemProfile profile_ = {};
        Category::Enum category_ = Category::None;

        // The Item class owns the Enchantment objects and is responsible
        // for their lifetimes, but misc::NotNulls cannot be serialized so
        // a separate vector of raw pointers is needed.
        creature::EnchantmentPVec_t enchantmentsPVec_ = {};
        std::vector<creature::Enchantment *> enchantmentsToSerializePVec_ = {};

        // these are mutable so that we can set them when requested for the first time by a const
        // getter functions (lazy evaluation)
        mutable std::string imageFilename_ = {};
        mutable std::string imageFullPath_ = {};

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & desc_;
            ar & profile_;
            ar & category_;
            ar & enchantmentsToSerializePVec_;

            // don't save imageFullPath_ because that might change when a game is loaded after a
            // re-install, but the filename can be random so we want to keep that so each time
            // a game loads the player sees the image they are familiar with
            ar & imageFilename_;
        }
    };

    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
    using ItemUPtr_t = std::unique_ptr<Item>;
    using ItemUVec_t = std::vector<ItemUPtr_t>;

    // ignore name_/desc_/imageFilename_/imageFullPath_ since random or change betweeen load/save
    bool operator==(const Item & L, const Item & R);

    // ignore name_/desc_/imageFilename_/imageFullPath_ since random or change betweeen load/save
    inline bool operator!=(const Item & L, const Item & R) { return !(L == R); }

    // ignore name_/desc_/imageFilename_/imageFullPath_ since random or change betweeen load/save
    bool operator<(const Item & L, const Item & R);

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_HPP_INCLUDED
