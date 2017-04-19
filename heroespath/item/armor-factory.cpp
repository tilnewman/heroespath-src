//
// armor-factory.cpp
//
#include "armor-factory.hpp"
#include "heroespath/item/item.hpp"
#include "heroespath/item/armor-details.hpp"
#include "heroespath/item/weapon-info.hpp"


namespace heroespath
{
namespace item
{
namespace armor
{

    ArmorFactorySPtr_t ArmorFactory::instance_(nullptr);


    ArmorFactory::ArmorFactory()
    {}


    ArmorFactory::~ArmorFactory()
    {}


    ArmorFactorySPtr_t ArmorFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new ArmorFactory );

        return instance_;
    }


    ItemSPtr_t ArmorFactory::Make_Shield(const shield_type::Enum SHIELD_TYPE,
                                          const material::Enum    MATERIAL_PRI,
                                          const material::Enum    MATERIAL_SEC)
    {
        ArmorInfo armorInfo(armor_type::Sheild);
        armorInfo.shield = SHIELD_TYPE;

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (SHIELD_TYPE == shield_type::Pavis)
            exclusiveRole = creature::role::Knight;

        const ArmorDetails DETAILS(ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::shield_type::ToString(SHIELD_TYPE)));

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, false);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        std::ostringstream ssName;
        ssName << material::ToReadableString(MATERIAL_PRI) << " ";
        if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
        {
            if (material::IsRigid(MATERIAL_SEC))
            {
                if (material::IsJewel(MATERIAL_SEC))
                    ssName << "jeweled ";
                else
                    ssName << material::ToReadableString(MATERIAL_SEC) << " reinforced ";
            }
            else if (material::IsLiquid(MATERIAL_SEC))
                ssName << " and " << material::ToReadableString(MATERIAL_SEC) << " covered ";
            else
                ssName << " and " << material::ToReadableString(MATERIAL_SEC) << " ";

        }
        ssName << DETAILS.name;

        std::ostringstream ssDesc;
        ssDesc << DETAILS.description << " made of " << material::ToReadableString(MATERIAL_PRI);
        if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
        {
            if (material::IsRigid(MATERIAL_SEC))
            {
                if (material::IsJewel(MATERIAL_SEC))
                    ssDesc << " and jeweled with ";
                else
                    ssDesc << " and reinforced with ";
            }
            else if (material::IsLiquid(MATERIAL_SEC))
                ssDesc << " and is covered in ";
            else
                ssDesc << " and ";

            ssDesc << material::ToReadableString(MATERIAL_SEC);
        }
        ssDesc << ".";

        return ItemSPtr_t( new Item(ssName.str(),
                                     ssDesc.str(),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::OneHanded),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Sheild,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo) );
    }


    ItemSPtr_t ArmorFactory::Make_Helm(const helm_type::Enum HELM_TYPE,
                                        const material::Enum  MATERIAL_PRI,
                                        const material::Enum  MATERIAL_SEC)
    {
        ArmorInfo armorInfo(armor_type::Helm);
        armorInfo.helm = HELM_TYPE;

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::helm_type::ToString(HELM_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        material::Enum materialPri(MATERIAL_PRI);
        material::Enum materialSec(MATERIAL_SEC);
        if (HELM_TYPE == helm_type::Leather)
        {
            materialPri = material::HardLeather;
            materialSec = material::Nothing;
        }

        AdjustPrice(price, materialPri, materialSec);
        AdjustWeight(weight, materialPri, materialSec);
        AdjustArmorRating(armorRating, materialPri, materialSec);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (HELM_TYPE == helm_type::Great)
            exclusiveRole = creature::role::Knight;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, materialPri, materialSec),
                                     Make_Desc(DETAILS.description, materialPri, materialSec),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Helm,
                                     materialPri,
                                     materialSec,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo) );
    }


    ItemSPtr_t ArmorFactory::Make_Gauntlets(const base_type::Enum TYPE,
                                             const material::Enum  MATERIAL_PRI,
                                             const material::Enum  MATERIAL_SEC,
                                             const bool            IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Gauntlets);
        armorInfo.is_gauntlets = true;
        armorInfo.base = TYPE;

        std::string gauntletName("gloves");
        if (TYPE != item::armor::base_type::Plain)
            gauntletName = item::armor::base_type::ToString(TYPE) + "Gauntlets";

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(gauntletName) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Gauntlets,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ArmorFactory::Make_Pants(const base_type::Enum TYPE,
                                         const material::Enum  MATERIAL_SEC,
                                         const bool            IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Pants);
        armorInfo.is_pants = true;
        armorInfo.base = TYPE;

        const material::Enum MATERIAL_PRI(material::Cloth);

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::base_type::ToString(TYPE) + "Pants") );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        category::Enum armorCategory(category::Armor);
        if (TYPE == base_type::Plain)
            armorCategory = category::Wearable;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_DescClasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(armorCategory | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Pants,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ArmorFactory::Make_Boots(const base_type::Enum TYPE,
                                         const material::Enum  MATERIAL_PRI,
                                         const material::Enum  MATERIAL_SEC,
                                         const bool            IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Boots);
        armorInfo.is_boots = true;
        armorInfo.base = TYPE;

        const std::string TYPE_NAME_STR(item::armor::base_type::ToString(TYPE));
        const ArmorDetails DETAILS(ArmorDetailLoader::Instance()->LookupArmorDetails(TYPE_NAME_STR + "Boots"));

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        std::string imageFilename(DETAILS.image_filename);
        if ((material::IsClothOrLeather(MATERIAL_PRI)) && (material::IsClothOrLeather(MATERIAL_SEC)))
            imageFilename = "boots-leather";

        category::Enum armorCategory(category::Armor);
        if (TYPE == base_type::Plain)
            armorCategory = category::Wearable;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(armorCategory | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Boots,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     imageFilename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ArmorFactory::Make_Shirt(const base_type::Enum TYPE,
                                         const material::Enum  MATERIAL_SEC,
                                         const bool            IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Shirt);
        armorInfo.is_shirt = true;
        armorInfo.base = TYPE;

        const material::Enum MATERIAL_PRI(material::Cloth);
        const std::string TYPE_NAME_STR(item::armor::base_type::ToString(TYPE));
        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(TYPE_NAME_STR + "Shirt") );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        category::Enum armorCategory(category::Armor);
        if (TYPE == base_type::Plain)
            armorCategory = category::Wearable;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_DescClasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(armorCategory | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Shirt,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ArmorFactory::Make_Bracer(const base_type::Enum TYPE,
                                          const material::Enum  MATERIAL_PRI,
                                          const material::Enum  MATERIAL_SEC,
                                          const bool            IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Bracer);
        armorInfo.is_bracer = true;
        armorInfo.base = TYPE;

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::base_type::ToString(TYPE) + "Bracers") );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Bracer,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ArmorFactory::Make_Aventail(const base_type::Enum TYPE,
                                            const material::Enum  MATERIAL_PRI,
                                            const material::Enum  MATERIAL_SEC)
    {
        ArmorInfo armorInfo(armor_type::Aventail);
        armorInfo.is_aventail = true;
        armorInfo.base = TYPE;

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::base_type::ToString(TYPE) + "Aventail") );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        creature::role::Enum exclusiveRole(creature::role::Count);
        if (TYPE == base_type::Plate)
            exclusiveRole = creature::role::Knight;

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC),
                                     Make_DescClasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Aventail,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     exclusiveRole,
                                     weapon::WeaponInfo(),
                                     armorInfo) );
    }


    ItemSPtr_t ArmorFactory::Make_Cover(const cover_type::Enum COVER_TYPE,
                                         const material::Enum   MATERIAL_PRI,
                                         const material::Enum   MATERIAL_SEC,
                                         const bool             IS_PIXIE_ITEM)
    {
        ArmorInfo armorInfo(armor_type::Covering);
        armorInfo.cover = COVER_TYPE;

        const ArmorDetails DETAILS( ArmorDetailLoader::Instance()->LookupArmorDetails(item::armor::cover_type::ToString(COVER_TYPE)) );

        Coin_t price(DETAILS.price);
        Weight_t weight(DETAILS.weight);
        stats::Armor_t armorRating(DETAILS.armor_rating);

        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);
        AdjustArmorRating(armorRating, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t( new Item(Make_Name(DETAILS.name, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_DescClasped(DETAILS.description, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Armor | category::Equippable | category::Wearable),
                                     misc_type::NotMisc,
                                     weapon_type::NotAWeapon,
                                     armor_type::Covering,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     DETAILS.image_filename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     armorRating,
                                     creature::role::Count,
                                     weapon::WeaponInfo(),
                                     armorInfo,
                                     item::IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }

}
}
}
