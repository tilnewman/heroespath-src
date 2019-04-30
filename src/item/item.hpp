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
//  A base class for all items.
//
#include "misc/boost-serialize-includes.hpp"

#include "creature/enchantment.hpp"
#include "creature/role-enum.hpp"
#include "creature/summon-info.hpp"
#include "game/strong-types.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-profile.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-type-wrapper.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    // Responsible for all state and operation of all items in the game.
    class Item
    {
    public:
        explicit Item(
            const std::string & NAME = "",
            const std::string & DESC = "",
            const category::Enum CATEGORY = category::None,
            const material::Enum MATERIAL_PRIMARY = material::Nothing,
            const material::Enum MATERIAL_SECONDARY = material::Nothing,
            const Coin_t & PRICE = 0_coin,
            const Weight_t & WEIGHT = 0_weight,
            const Health_t & DAMAGE_MIN = 0_health,
            const Health_t & DAMAGE_MAX = 0_health,
            const Armor_t & ARMOR_RATING = 0_armor,
            const TypeWrapper & TYPE_WRAPPER = TypeWrapper(),
            const weapon::WeaponTypeWrapper & WEAPON_INFO = weapon::WeaponTypeWrapper(),
            const armor::ArmorTypeWrapper & ARMOR_INFO = armor::ArmorTypeWrapper(),
            const bool IS_PIXIE_ITEM = false,
            const creature::role::Enum EXCLUSIVE_ROLE_BASED_ON_ITEM_TYPE = creature::role::Count);

        ~Item();

        Item(const Item &) = delete;
        Item(Item &&) = delete;
        Item & operator=(const Item &) = delete;
        Item & operator=(Item &&) = delete;

        const std::string Name() const;

        // similar to Name() but without "Pixie" or materials.
        const std::string ShortName() const;

        // the shortest answer to "what is it", with no unique/set/named type names
        const std::string BaseName() const;

        const std::string Desc() const;

        const std::string ImagePath() const;

        const weapon::WeaponTypeWrapper WeaponInfo() const { return weaponInfo_; }
        const armor::ArmorTypeWrapper ArmorInfo() const { return armorInfo_; }

        category::Enum Category() const { return category_; }
        material::Enum MaterialPrimary() const { return materialPri_; }
        material::Enum MaterialSecondary() const { return materialSec_; }
        weapon_type::Enum WeaponType() const { return weaponInfo_.Type(); }
        armor_type::Enum ArmorType() const { return armorInfo_.Type(); }
        misc_type::Enum MiscType() const { return miscType_; }
        set_type::Enum SetType() const { return setType_; }
        named_type::Enum NamedType() const { return namedType_; }
        element_type::Enum ElementType() const { return elementType_; }

        Coin_t Price() const { return price_; }
        Weight_t Weight() const { return weight_; }

        Health_t DamageMin() const { return damageMin_; }
        Health_t DamageMax() const { return damageMax_; }
        Armor_t ArmorRating() const { return armorRating_; }

        bool HasCategoryType(const category::Enum CATEGORY) const
        {
            return ((category_ & CATEGORY) > 0);
        }

        bool HasWeaponType(const weapon_type::Enum WEAPON_TYPE) const
        {
            return ((weaponInfo_.Type() & WEAPON_TYPE) > 0);
        }

        bool IsQuestItem() const { return misc_type::IsQuestItem(miscType_); }

        bool IsBroken() const { return (category_ & category::Broken); }
        bool IsWearable() const { return (category_ & category::Wearable); }
        bool IsOneHanded() const { return (category_ & category::OneHanded); }
        bool IsTwoHanded() const { return (category_ & category::TwoHanded); }
        bool IsBodypart() const { return (category_ & category::BodyPart); }
        bool IsArmor() const { return (armorInfo_.IsTypeValid()); }
        bool IsWeapon() const { return weaponInfo_.IsTypeValid(); }

        bool IsSet() const { return (set_type::Count != setType_) && (set_type::Not != setType_); }

        bool IsNamed() const
        {
            return (named_type::Count != namedType_) && (named_type::Not != namedType_);
        }

        bool IsMisc() const
        {
            return (misc_type::Count != miscType_) && (misc_type::Not != miscType_);
        }

        bool IsUnique() const { return misc_type::IsUnique(miscType_); }

        bool IsElemental() const { return (elementType_ != element_type::None); }

        bool IsRigid() const { return material::IsRigid(materialPri_); }

        bool IsBloody() const { return material::IsBloody(materialPri_, materialSec_); }

        float FireDamageRatio() const
        {
            return material::FireDamageRatio(materialPri_, materialSec_);
        }

        bool IsMetal() const { return material::IsMetal(materialPri_); }

        creature::role::Enum ExclusiveRole() const { return exclusiveToRole_; }

        bool IsJeweled() const { return material::IsFancyJewel(materialSec_); }

        bool IsPixie() const { return isPixie_; }

        bool MustBePixieVersionForPixiesToEquip() const;

        const creature::EnchantmentPVec_t & Enchantments() const { return enchantmentsPVec_; }

        void AddCategory(const category::Enum ENUM)
        {
            category_ = static_cast<category::Enum>(category_ | ENUM);
        }

        const creature::SummonInfo & SummonInfo() const { return summonInfo_; }

        bool HasEnchantments() const { return (enchantmentsPVec_.empty() == false); }

        bool IsMagical() const
        {
            return (
                (IsBroken() == false)
                && (HasEnchantments() || summonInfo_.CanSummon() || IsElemental() || IsSet()
                    || IsNamed() || IsUnique()));
        }

        const std::string ToString() const;

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

        friend bool operator<(const Item & L, const Item & R);
        friend bool operator==(const Item & L, const Item & R);

    private:
        const std::string ReadableName() const;
        const std::string ComposeName(const std::string & ROOT_NAME) const;

    protected:
        std::string name_;
        std::string desc_;
        Coin_t price_;
        Weight_t weight_;
        Health_t damageMin_;
        Health_t damageMax_;
        Armor_t armorRating_;
        creature::role::Enum exclusiveToRole_;
        category::Enum category_;
        misc_type::Enum miscType_;
        material::Enum materialPri_;
        material::Enum materialSec_;
        weapon::WeaponTypeWrapper weaponInfo_;
        armor::ArmorTypeWrapper armorInfo_;
        bool isPixie_;
        set_type::Enum setType_;
        named_type::Enum namedType_;
        element_type::Enum elementType_;
        creature::SummonInfo summonInfo_;

        // The Item class owns the Enchantment objects and is responsible
        // for their lifetimes, but misc::NotNulls cannot be serialized so
        // a separate vector of raw pointers is needed.
        creature::EnchantmentPVec_t enchantmentsPVec_;
        std::vector<creature::Enchantment *> enchantmentsToSerializePVec_;

        // these are mutable so that we can set them when requested for the first time by a const
        // getter functions (lazy evaluation)
        mutable std::string imageFilename_;
        mutable std::string imageFullPath_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & name_;
            ar & desc_;
            ar & price_;
            ar & weight_;
            ar & damageMin_;
            ar & damageMax_;
            ar & armorRating_;
            ar & exclusiveToRole_;
            ar & category_;
            ar & miscType_;
            ar & materialPri_;
            ar & materialSec_;

            // don't save imageFullPath_ because that might change when a game is loaded after a
            // re-install, but the filename can be random so we want to keep that so each time
            // a game loads the player sees the image they are familiar with
            ar & imageFilename_;

            ar & weaponInfo_;
            ar & armorInfo_;
            ar & isPixie_;
            ar & setType_;
            ar & namedType_;
            ar & elementType_;
            ar & summonInfo_;
            ar & enchantmentsToSerializePVec_;
        }
    };

    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
    using ItemUPtr_t = std::unique_ptr<Item>;
    using ItemUVec_t = std::vector<ItemUPtr_t>;

    bool operator<(const Item & L, const Item & R);

    bool operator==(const Item & L, const Item & R);

    inline bool operator!=(const Item & L, const Item & R) { return !(L == R); }

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ITEM_HPP_INCLUDED
