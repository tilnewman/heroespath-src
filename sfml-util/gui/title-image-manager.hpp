///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
        inline static const std::string FileNameExt()           { return ".png"; }

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
