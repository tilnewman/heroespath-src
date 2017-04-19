//
// misc-item-factory.cpp
//
#include "misc-item-factory.hpp"

#include "sfml-util/boost-string-includes.hpp"
#include "sfml-util/gui/item-image-manager.hpp"

#include "heroespath/item/item.hpp"

#include <exception>
#include <sstream>


namespace heroespath
{
namespace item
{
namespace misc
{

    ItemFactorySPtr_t ItemFactory::instance_(nullptr);


    ItemFactory::ItemFactory()
    {}


    ItemFactory::~ItemFactory()
    {}


    ItemFactorySPtr_t ItemFactory::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new ItemFactory );

        return instance_;
    }


    ItemSPtr_t ItemFactory::Make_Ring(const material::Enum MATERIAL_PRI,
                                       const material::Enum MATERIAL_SEC,
                                       const bool           IS_PIXIE_ITEM)
    {
        Coin_t price(5);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);

        Weight_t weight(20);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        std::string imageFilename("ring-plain");
        if (material::IsPrecious(MATERIAL_SEC))
            imageFilename = "ring-jeweled";

        return ItemSPtr_t( new Item(Make_Name("Ring", MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc("A ring", MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Equippable | category::Wearable),
                                     misc_type::Ring,
                                     weapon_type::NotAWeapon,
                                     armor_type::NotArmor,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     imageFilename,
                                     price,
                                     weight,
                                     0,
                                     0,
                                     0,
                                     creature::role::Count,
                                     weapon::WeaponInfo(),
                                     armor::ArmorInfo(),
                                     IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ItemFactory::Make_Wand(const material::Enum MATERIAL_PRI,
                                       const material::Enum MATERIAL_SEC,
                                       const bool           IS_PIXIE_ITEM)
    {
        Coin_t price(437);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);

        Weight_t weight(40);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemSPtr_t( new Item(Make_Name("Wand", MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
                                     Make_Desc("A wand", MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
                                     static_cast<category::Enum>(category::Equippable | category::Magical | category::EnchantsOnlyWhenEquipped),
                                     misc_type::Wand,
                                     weapon_type::NotAWeapon,
                                     armor_type::NotArmor,
                                     MATERIAL_PRI,
                                     MATERIAL_SEC,
                                     boost::algorithm::erase_all_copy(sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(item::misc_type::Wand, material::IsJewel(MATERIAL_SEC), true), ".gif"),
                                     price,
                                     weight,
                                     0,
                                     0,
                                     0,
                                     creature::role::Count,
                                     weapon::WeaponInfo(),
                                     armor::ArmorInfo(),
                                     IEnchantmentSVec_t(),
                                     IS_PIXIE_ITEM) );
    }


    ItemSPtr_t ItemFactory::Make_Instrument(const instrument_type::Enum TYPE, const bool IS_PIXIE_ITEM)
    {
        Coin_t price(0);
        Weight_t weight(0);
        std::string name("");
        std::string desc("");
        misc_type::Enum miscType(misc_type::Count);
        std::string imageFilename("");
        category::Enum categoryHanded(category::OneHanded);

        switch (TYPE)
        {
            case instrument_type::Panflute:
            {
                name = "Panflute";
                desc = "A set of pipes at varying length bound together";
                price = 100;
                weight = 150;
                categoryHanded = category::OneHanded;
                imageFilename = "panflute";
                miscType = misc_type::Panflute;
                break;
            }
            case instrument_type::Flute:
            {
                name = "Flute";
                desc = "A single hollow length";
                price = 500;
                weight = 200;
                categoryHanded = category::TwoHanded;
                imageFilename = "flute";
                miscType = misc_type::Flute;
                break;
            }
            case instrument_type::Lute:
            {
                name = "Lute";
                desc = "A type of guitar with a round body";
                price = 1000;
                weight = 350;
                categoryHanded = category::TwoHanded;
                imageFilename = "lute";
                miscType = misc_type::Lute;
                break;
            }
            case instrument_type::Sitar:
            {
                name = "Sitar";
                desc = "A type of guitar with a long neck";
                price = 2000;
                weight = 400;
                categoryHanded = category::TwoHanded;
                imageFilename = "sitar";
                miscType = misc_type::Sitar;
                break;
            }
            case instrument_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "herospath::item::ItemFactory::Make_Instrument(" << TYPE << ") -but that type is invalid.";
                throw std::range_error(ss.str());
            }
        }

        AdjustPrice(price, material::Wood, material::Rope, IS_PIXIE_ITEM);
        AdjustWeight(weight, material::Wood, material::Rope);

        return ItemSPtr_t(new Item(Make_Name(name, material::Wood, material::Rope, IS_PIXIE_ITEM),
                                    Make_Desc(desc, material::Wood, material::Rope, "", IS_PIXIE_ITEM),
                                    static_cast<category::Enum>(category::Equippable | category::Useable | categoryHanded),
                                    miscType,
                                    weapon_type::NotAWeapon,
                                    armor_type::NotArmor,
                                    material::Wood,
                                    material::Rope,
                                    imageFilename,
                                    price,
                                    weight,
                                    0,
                                    0,
                                    0,
                                    creature::role::Bard,
                                    weapon::WeaponInfo(),
                                    armor::ArmorInfo(),
                                    IEnchantmentSVec_t(),
                                    IS_PIXIE_ITEM));
    }

}
}
}
