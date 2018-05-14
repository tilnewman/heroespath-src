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
#ifndef HEROESPATH_SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
//
// image-manager-base.hpp
//
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "sfml-util/gui/image-dimmensions.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util.hpp"

#include <boost/type_index.hpp>

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for implementing common operations of all image managers that work on a
        // simple single enum.
        template <typename T>
        class ImageManagerBase
        {
        public:
            ImageManagerBase(const ImageManagerBase &) = delete;
            ImageManagerBase(ImageManagerBase &&) = delete;
            ImageManagerBase & operator=(const ImageManagerBase &) = delete;
            ImageManagerBase & operator=(ImageManagerBase &&) = delete;
            ImageManagerBase() = delete;

            static void SetImageDirectoryPath(const std::string & DIR_PATH)
            {
                pathToImagesDir_ = DIR_PATH;

                pathToPlaceholderImage_
                    = game::GameDataFile::Instance()->GetMediaPath("media-images-placeholder");
            }

            static float MaxDimmension() { return ImageDimmensions::ResourceStandardMax(); }

            // returns false if the placeholder was loaded instead of the desired image
            static bool
                Get(sf::Texture & texture,
                    const typename T::Enum ENUM_OF_IMAGE,
                    const bool WILL_FLIP_HORIZ = false)
            {
                namespace bfs = boost::filesystem;

                auto const IMAGE_FILE_PATH{ bfs::system_complete(
                    bfs::path(pathToImagesDir_) / bfs::path(T::ImageFilename(ENUM_OF_IMAGE))) };

                auto const WILL_USE_PLACEHOLDER{ (bfs::exists(IMAGE_FILE_PATH) == false)
                                                 || (bfs::is_regular_file(IMAGE_FILE_PATH)
                                                     == false) };

                if (WILL_USE_PLACEHOLDER)
                {
                    M_HP_LOG_ERR(
                        "sfml_util::gui::ImageManagerBase<"
                        << boost::typeindex::type_id<T>().pretty_name() << ">::Get(enum="
                        << ENUM_OF_IMAGE << ", path=\"" << IMAGE_FILE_PATH.string()
                        << "\", will_flip=" << std::boolalpha << WILL_FLIP_HORIZ
                        << ") that path does not exist or is not a regular file.  Will use the "
                           "placeholder image instead.");

                    sfml_util::LoadTexture(
                        texture, bfs::system_complete(bfs::path(pathToPlaceholderImage_)).string());
                }
                else
                {
                    sfml_util::LoadTexture(texture, IMAGE_FILE_PATH.string());

                    if (WILL_FLIP_HORIZ)
                    {
                        sfml_util::FlipHoriz(texture);
                    }
                }

                return !WILL_USE_PLACEHOLDER;
            }

            static bool Test()
            {
                static const std::string CLASS_NAME{ "sfml_util::gui::ImageManagerBase<"
                                                     + boost::typeindex::type_id<T>().pretty_name()
                                                     + ">" };

                static auto hasInitialPrompt{ false };
                if (false == hasInitialPrompt)
                {
                    hasInitialPrompt = true;

                    game::LoopManager::Instance()->TestingStrAppend(
                        CLASS_NAME + "  Starting Tests...");
                }

                static auto willFlip{ false };
                static misc::EnumUnderlying_t imageIndex{ 0 };
                if (imageIndex < T::Count)
                {
                    auto const ENUM{ static_cast<typename T::Enum>(imageIndex) };

                    sf::Texture texture;
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (Get(texture, ENUM, willFlip)),
                        "sfml_util::gui::"
                            << CLASS_NAME << "::Test() The call to Get(image_enum="
                            << T::ToString(ENUM) << ", will_flip=" << std::boolalpha << willFlip
                            << ") returned false, meaning the placeholder image was used.");

                    auto const DIMMENSION_U{ static_cast<unsigned>(MaxDimmension()) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        ((texture.getSize().x == DIMMENSION_U)
                         || (texture.getSize().y == DIMMENSION_U)),
                        "sfml_util::gui::"
                            << CLASS_NAME << "::Test() The call to Get(image_enum="
                            << T::ToString(ENUM) << ", will_flip=" << std::boolalpha << willFlip
                            << ") returned an image that was not of size=" << DIMMENSION_U << "x"
                            << DIMMENSION_U << ".  The actual size=" << texture.getSize().x << "x"
                            << texture.getSize().y << ".");

                    game::LoopManager::Instance()->TestingImageSet(texture);

                    game::LoopManager::Instance()->TestingStrAppend(
                        CLASS_NAME + " Tested " + T::ImageFilename(ENUM)
                        + ((willFlip) ? " HORIZ_FLIPPED" : ""));

                    if (willFlip)
                    {
                        ++imageIndex;
                    }

                    willFlip = !willFlip;
                    return false;
                }

                game::LoopManager::Instance()->TestingStrAppend(CLASS_NAME + "  All Test Passed");

                return true;
            }

        private:
            static std::string pathToImagesDir_;
            static std::string pathToPlaceholderImage_;
        };

        template <typename T>
        std::string ImageManagerBase<T>::pathToImagesDir_{ "" };

        template <typename T>
        std::string ImageManagerBase<T>::pathToPlaceholderImage_{ "" };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_IMAGEMANAGERBASE_HPP_INCLUDED
