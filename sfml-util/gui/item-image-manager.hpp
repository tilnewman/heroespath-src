#ifndef SFMLUTIL_GUI_ITEMIMAGEMANAGER_INCLUDED
#define SFMLUTIL_GUI_ITEMIMAGEMANAGER_INCLUDED
//
// item-image-manager.hpp
//  Code that manages loading and lifetime of item images.
//
#include "sfml-util/sfml-graphics.hpp"

#include "game/item/weapon-info.hpp"
#include "game/item/armor-info.hpp"
#include "game/item/item.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    //types required by singleton implementation
    class ItemImageManager;
    using ItemImageManagerSPtr_t = std::shared_ptr<ItemImageManager>;


    //A class that loads, stores, and distributes creature images.
    class ItemImageManager
    {
        //prevent copy construction
        ItemImageManager(const ItemImageManager &);

        //prevent copy assignment
        ItemImageManager & operator=(const ItemImageManager &);

        //prevent non-singleton construction
        ItemImageManager();

    public:
        virtual ~ItemImageManager();
        static ItemImageManagerSPtr_t Instance();

        static bool Test();

        static void SetItemImageDirectory(const std::string & PATH);
        static inline float GetMaxDimmension() { return 256.0f; }

        TextureSPtr_t Load(const std::string & IMAGE_FILE_NAME) const;
        inline TextureSPtr_t Load(const game::item::ItemSPtr_t & ITEM_SPTR) const { return Load(ITEM_SPTR->ImageFilename() + EXT_); }
        TextureSPtr_t Load(const game::item::misc_type::Enum, const bool IS_JEWELED = false, const bool WILL_RANDOMIZE = true) const;

        const std::string GetImageFilename(const game::item::ItemSPtr_t & ITEM_SPTR, const bool WILL_RANDOMIZE = true) const;
        const std::string GetImageFilename(const game::item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED = false, const bool WILL_RANDOMIZE = true) const;
        const std::string GetImageFilename(const game::item::weapon::WeaponInfo & WEAPON_INFO, const bool IS_JEWELED = false, const bool WILL_RANDOMIZE = true) const;
        const std::string GetImageFilename(const game::item::armor::ArmorInfo & ARMOR_INFO, const bool IS_JEWELED = false, const bool WILL_RANDOMIZE = true) const;

    private:
        static std::string imagesDirectoryPath_;
        static ItemImageManagerSPtr_t instanceSPtr_;
        static const std::string EXT_;
    };

}
}
#endif //SFMLUTIL_GUI_ITEMIMAGEMANAGER_INCLUDED
