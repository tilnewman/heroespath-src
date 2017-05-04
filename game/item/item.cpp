//
// item.cpp
//
#include "item.hpp"

#include "game/item/i-enchantment.hpp"

#include <tuple>


namespace game
{
namespace item
{

    Item::Item(const std::string &        NAME,
               const std::string &        DESC,
               const category::Enum       CATEGORY,
               const misc_type::Enum      MISC_TYPE,
               const weapon_type::Enum    WEAPON_TYPE,
               const armor_type::Enum     ARMOR_TYPE,
               const material::Enum       MATERIAL_PRIMARY,
               const material::Enum       MATERIAL_SECONDARY,
               const std::string &        IMAGE_FILENAME,
               const Coin_t               PRICE,
               const Weight_t             WEIGHT,
               const stats::Health_t      DAMAGE_MIN,
               const stats::Health_t      DAMAGE_MAX,
               const stats::Armor_t       ARMOR_RATING,
               const creature::role::Enum EXCLUSIVE_TO_ROLE,
               const weapon::WeaponInfo & WEAPON_INFO,
               const armor::ArmorInfo &   ARMOR_INFO,
               const IEnchantmentSVec_t & ENCHANTMENTS_SVEC,
               const bool                 IS_PIXIE_ITEM)
    :
        name_            (NAME),
        desc_            (DESC),
        price_           (PRICE),
        weight_          (WEIGHT),
        damageMin_       (DAMAGE_MIN),
        damageMax_       (DAMAGE_MAX),
        armorRating_     (ARMOR_RATING),
        enchantmentsSVec_(ENCHANTMENTS_SVEC),
        exclusiveToRole_ (EXCLUSIVE_TO_ROLE),
        armorType_       (ARMOR_TYPE),
        weaponType_      (WEAPON_TYPE),
        category_        (CATEGORY),
        miscType_        (MISC_TYPE),
        materialPri_     (MATERIAL_PRIMARY),
        materialSec_     (MATERIAL_SECONDARY),
        imageFilename_   (IMAGE_FILENAME),
        weaponInfo_      (WEAPON_INFO),
        armorInfo_       (ARMOR_INFO),
        isPixie_         (false)//see constructor
    {
        //apply enchantments to the item
        for (auto const & NEXT_ENCHANTMENT : enchantmentsSVec_)
            NEXT_ENCHANTMENT->ChangeItem(this);

        if (IS_PIXIE_ITEM)
            IsPixie(true, true);
    }


    Item::~Item()
    {}


    bool Item::IsMagical() const
    {
        return (category::IsMagical(category_)    ||
                material::IsMagical(materialPri_) ||
                material::IsMagical(materialSec_));
    }


    void Item::IsPixie(const bool IS_PIXIE, const bool WILL_ADJUST_WEIGHT)
    {
        if ((isPixie_ != IS_PIXIE) && WILL_ADJUST_WEIGHT)
        {
            weight_ /= 250;

            if (weight_ < 1)
                weight_ = 1;
        }

        isPixie_ = IS_PIXIE;
    }


    bool operator<(const Item & L, const Item & R)
    {
        return std::tie(L.name_,
                        L.desc_,
                        L.price_,
                        L.weight_,
                        L.damageMin_,
                        L.damageMax_,
                        L.armorRating_,
                        L.enchantmentsSVec_,
                        L.exclusiveToRole_,
                        L.armorType_,
                        L.weaponType_,
                        L.category_,
                        L.miscType_,
                        L.materialPri_,
                        L.materialSec_,
                        L.imageFilename_,
                        L.weaponInfo_,
                        L.armorInfo_,
                        L.isPixie_)
               <
               std::tie(R.name_,
                        R.desc_,
                        R.price_,
                        R.weight_,
                        R.damageMin_,
                        R.damageMax_,
                        R.armorRating_,
                        R.enchantmentsSVec_,
                        R.exclusiveToRole_,
                        R.armorType_,
                        R.weaponType_,
                        R.category_,
                        R.miscType_,
                        R.materialPri_,
                        R.materialSec_,
                        R.imageFilename_,
                        R.weaponInfo_,
                        R.armorInfo_,
                        R.isPixie_);
    }


    bool operator==(const Item & L, const Item & R)
    {
        return std::tie(L.name_,
                        L.desc_,
                        L.price_,
                        L.weight_,
                        L.damageMin_,
                        L.damageMax_,
                        L.armorRating_,
                        L.enchantmentsSVec_,
                        L.exclusiveToRole_,
                        L.armorType_,
                        L.weaponType_,
                        L.category_,
                        L.miscType_,
                        L.materialPri_,
                        L.materialSec_,
                        L.imageFilename_,
                        L.weaponInfo_,
                        L.armorInfo_,
                        L.isPixie_)
               ==
               std::tie(R.name_,
                        R.desc_,
                        R.price_,
                        R.weight_,
                        R.damageMin_,
                        R.damageMax_,
                        R.armorRating_,
                        R.enchantmentsSVec_,
                        R.exclusiveToRole_,
                        R.armorType_,
                        R.weaponType_,
                        R.category_,
                        R.miscType_,
                        R.materialPri_,
                        R.materialSec_,
                        R.imageFilename_,
                        R.weaponInfo_,
                        R.armorInfo_,
                        R.isPixie_);
    }


    bool operator!=(const Item & L, const Item & R)
    {
        return ! (L == R);
    }

}
}
