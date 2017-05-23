#ifndef GAME_ITEM_ITEM_INCLUDED
#define GAME_ITEM_ITEM_INCLUDED
//
// item.hpp
//  A base class for all items.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/stats/types.hpp"
#include "game/item/types.hpp"
#include "game/item/item-type-enum.hpp"
#include "game/item/weapon-info.hpp"
#include "game/item/armor-info.hpp"
#include "game/creature/role-enum.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
namespace item
{

    //A base class for all items.
    class Item
    {
        //prevent copy construction
        Item(const Item &) =delete;

        //prevent copy assignment
        Item & operator=(const Item &) =delete;

    public:
        explicit Item(const std::string &        NAME               = "no_name_error",
                      const std::string &        DESC               = "no-desc_error",
                      const category::Enum       CATEGORY           = category::Broken,
                      const misc_type::Enum      MISC_TYPE          = misc_type::NotMisc,
                      const weapon_type::Enum    WEAPON_TYPE        = weapon_type::NotAWeapon,
                      const armor_type::Enum     ARMOR_TYPE         = armor_type::NotArmor,
                      const material::Enum       MATERIAL_PRIMARY   = material::Nothing,
                      const material::Enum       MATERIAL_SECONDARY = material::Nothing,
                      const std::string &        IMAGE_FILENAME     = "",
                      const Coin_t               PRICE              = Coin_t(0),
                      const Weight_t             WEIGHT             = Weight_t(0),
                      const stats::Health_t      DAMAGE_MIN         = 0,
                      const stats::Health_t      DAMAGE_MAX         = 0,
                      const stats::Armor_t       ARMOR_RATING       = 0,
                      const creature::role::Enum EXCLUSIVE_TO_ROLE  = creature::role::Count,
                      const weapon::WeaponInfo & WEAPON_INFO        = weapon::WeaponInfo(),
                      const armor::ArmorInfo &   ARMOR_INFO         = armor::ArmorInfo(),
                      const bool                 IS_PIXIE_ITEM      = false);

        virtual ~Item();

        inline const std::string Name() const                   { return name_; }
        inline const std::string Desc() const                   { return desc_; }

        inline const std::string ImageFilename() const          { return imageFilename_; }

        inline const weapon::WeaponInfo Weapon_Info() const     { return weaponInfo_; }
        inline const armor::ArmorInfo Armor_Info() const        { return armorInfo_; }

        inline category::Enum Category() const                  { return category_; }
        inline material::Enum MaterialPrimary() const           { return materialPri_; }
        inline material::Enum MaterialSecondary() const         { return materialSec_; }
        inline weapon_type::Enum WeaponType() const             { return weaponType_; }
        inline armor_type::Enum ArmorType() const               { return armorType_; }
        inline misc_type::Enum MiscType() const                 { return miscType_; }

        inline bool IsCategoryType(const category::Enum E) const    { return (category_ & E); }
        inline bool IsWeaponType(const weapon_type::Enum E) const   { return (weaponType_ & E); }
        inline bool IsArmorType(const armor_type::Enum E) const     { return (armorType_ & E); }
        inline bool IsMiscType(const misc_type::Enum E) const       { return (miscType_ & E); }

        inline bool IsBroken() const                            { return (category_ == category::Broken); }
        inline bool IsArmor() const                             { return (category_ & category::Armor); }
        inline bool IsWeapon() const                            { return (category_ & category::Weapon); }
        inline bool IsWearable() const                          { return (category_ & category::Wearable); }
        inline bool IsOneHanded() const                         { return (category_ & category::OneHanded); }
        inline bool IsTwoHanded() const                         { return (category_ & category::TwoHanded); }
        inline bool WillLEnchant() const                        { return ((category_ & category::EnchantsWhenHeld) || (category_ & category::EnchantsOnlyWhenEquipped)); }
        inline bool IsBodypart() const                          { return (category_ & category::BodyPart); }

        bool IsMagical() const;
        inline bool IsRigid() const                             { return material::IsRigid(materialPri_); }
        inline bool ContainsEther() const                       { return material::ContainsEther(materialPri_, materialSec_); }
        inline bool IsBloody() const                            { return material::IsBloody(materialPri_, materialSec_); }
        inline bool ContainsFlesh() const                       { return material::ContainsFlesh(materialPri_, materialSec_); }
        inline float FireDamageRatio() const                    { return material::FireDamageRatio(materialPri_, materialSec_); }
        inline bool ContainsMetal() const                       { return material::ContainsMetal(materialPri_, materialSec_); }
        inline bool IsMetal() const                             { return material::IsMetal(materialPri_); }
        inline bool IsStone() const                             { return material::IsStone(materialPri_); }
        inline bool ContiansPrecious() const                    { return material::ContiansPrecious(materialPri_, materialSec_); }

        inline Coin_t Price() const                             { return price_; }
        inline Weight_t Weight() const                          { return weight_; }

        inline stats::Health_t DamageMin() const                { return damageMin_; }
        inline stats::Health_t DamageMax() const                { return damageMax_; }
        inline stats::Armor_t  ArmorRating() const              { return armorRating_; }

        inline virtual creature::role::Enum ExclusiveRole() const       { return exclusiveToRole_; }

        inline bool IsJeweled() const                           { return material::IsJewel(materialSec_); }
        inline bool IsJeweledRing() const                       { return ((miscType_ == misc_type::Ring) && IsJeweled()); }

        void IsPixie(const bool IS_PIXIE, const bool WILL_ADJUST_WEIGHT);
        inline bool IsPixie() const                             { return isPixie_; }

        friend bool operator<(const Item & L, const Item & R);
        friend bool operator==(const Item & L, const Item & R);

    protected:
        std::string          name_;
        std::string          desc_;
        Coin_t               price_;
        Weight_t             weight_;
        stats::Health_t      damageMin_;
        stats::Health_t      damageMax_;
        stats::Armor_t       armorRating_;\
        creature::role::Enum exclusiveToRole_;
        armor_type::Enum     armorType_;
        weapon_type::Enum    weaponType_;
        category::Enum       category_;
        misc_type::Enum      miscType_;
        material::Enum       materialPri_;
        material::Enum       materialSec_;
        std::string          imageFilename_;
        weapon::WeaponInfo   weaponInfo_;
        armor::ArmorInfo     armorInfo_;
        bool                 isPixie_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
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
            ar & armorType_;
            ar & weaponType_;
            ar & category_;
            ar & miscType_;
            ar & materialPri_;
            ar & materialSec_;
            ar & imageFilename_;
            ar & weaponInfo_;
            ar & armorInfo_;
            ar & isPixie_;
        }
    };


    using ItemSPtr_t = std::shared_ptr<Item>;
    using ItemSVec_t = std::vector<ItemSPtr_t>;
    using ItemSVecVec_t = std::vector<ItemSVec_t>;

    using ItemSVecIter_t = ItemSVec_t::iterator;
    using ItemSVecCIter_t = ItemSVec_t::const_iterator;


    bool operator<(const Item & L, const Item & R);

    bool operator==(const Item & L, const Item & R);

    bool operator!=(const Item & L, const Item & R);

}
}
#endif //GAME_ITEM_ITEM_INCLUDED
