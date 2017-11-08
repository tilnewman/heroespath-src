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
#ifndef SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
#define SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
//
// song-image-manager.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/loop-manager.hpp"
#include "game/game-data-file.hpp"

#include <boost/type_index.hpp>//for boost::typeindex::type_id<T>().pretty_name()


namespace sfml_util
{
namespace gui
{

    //Responsible for implementing common operations of all image managers.
    template<typename T>
    class ImageManagerBase
    {
        ImageManagerBase(const ImageManagerBase &) = delete;
        ImageManagerBase & operator=(const ImageManagerBase &) = delete;

    public:
        ImageManagerBase()
        :
            pathToPlaceholderImage_(
                heroespath::game::GameDataFile::Instance()->GetMediaPath("media-images-placeholder") )
        {}


        virtual ~ImageManagerBase() {}


        static inline const std::string PathToImages() { return pathToImages_; }


        static inline void PathToImages(const std::string & PATH) { pathToImages_ = PATH; }


        //all images are square with this as the side length
        float Dimmension() const { return 256.0f; }


        //returns false if the placeholder was loaded instead of the desired image
        bool Get(
            sf::Texture & texture,
            const typename T::Enum ENUM_OF_IMAGE,
            const bool WILL_FLIP_HORIZ = false)
        {
            namespace bfs = boost::filesystem;

            auto path{ bfs::system_complete(
                bfs::path(pathToImages_) / bfs::path(T::ImageFilename(ENUM_OF_IMAGE)) ) };

            auto const WILL_USE_PLACEHOLDER{
                (bfs::exists(path) == false) ||
                (bfs::is_regular_file(path) == false) };

            if (WILL_USE_PLACEHOLDER)
            {
                M_HP_LOG_ERR("sfml_util::gui::ImageManagerBase<"
                    << boost::typeindex::type_id<T>().pretty_name()
                    << ">::Get(enum=" << ENUM_OF_IMAGE
                    << ", path=\"" << path.string() << "\", will_flip=" << std::boolalpha
                    << WILL_FLIP_HORIZ << ") that path does not exist or is not a regular file.");

                path = bfs::system_complete( bfs::path(pathToPlaceholderImage_) );
            }

            sfml_util::LoadTexture(texture, path.string());

            if (WILL_FLIP_HORIZ && (WILL_USE_PLACEHOLDER == false))
            {
                sfml_util::FlipHoriz(texture);
            }

            return ! WILL_USE_PLACEHOLDER;
        }

    private:
        static std::string pathToImages_;
        std::string pathToPlaceholderImage_;
    };


    template<typename T>
    std::string ImageManagerBase<T>::pathToImages_{ "" };

}
}

#endif //SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
