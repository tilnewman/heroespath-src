#ifndef SFMLUTIL_GUI_TITLEIMAGEMANAGER_INCLUDED
#define SFMLUTIL_GUI_TITLEIMAGEMANAGER_INCLUDED
//
// title-image-manager.hpp
//  Code that manages loading and lifetime of title images.
//
#include "sfml-util/sfml-graphics.hpp"

#include "game/creature/title-warehouse.hpp"

#include <boost/filesystem/path.hpp>

#include <string>


namespace game
{
namespace creature
{
    class Title;
    using TitleCPtr_t = const Title *;
}
}
namespace sfml_util
{
namespace gui
{

    class TitleImageManager;
    using TitleImageManagerSPtr_t = std::shared_ptr<TitleImageManager>;


    //A class that loads, stores, and distributes creature images.
    class TitleImageManager
    {
        //prevent copy construction
        TitleImageManager(const TitleImageManager &) =delete;

        //prevent copy assignment
        TitleImageManager & operator=(const TitleImageManager &) =delete;

        //prevent non-singleton construction
        TitleImageManager();

    public:
        virtual ~TitleImageManager();

        static TitleImageManagerSPtr_t Instance();
        static void SetTitleImageDirectory(const std::string & PATH);

        inline static const std::string PlaceholderFileName()   { return "placeholder" + FileNameExt(); }
        inline static float DimmensionMax()                     { return 256.0f; }
        inline static const std::string FileNameExt()           { return ".gif"; }

        const TextureSPtr_t Get(game::creature::TitlePtr_t TITLE_PTR) const;

        //this function will throw if the given enum is invalid
        inline const TextureSPtr_t Get(const game::creature::Titles::Enum E) const { return Get(game::creature::title::Warehouse::Get(E)); }

    private:
        static std::string imagesDirectoryPath_;
        static TitleImageManagerSPtr_t instanceSPtr_;
        //
        boost::filesystem::path placeHlderFilePathObj_;
    };

}
}
#endif //SFMLUTIL_GUI_TITLEIMAGEMANAGER_INCLUDED
