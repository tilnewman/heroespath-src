// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TITLE_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TITLE_IMAGE_LOADER_HPP_INCLUDED
//
// title-image-loader.hpp
//
#include "creature/title-enum.hpp"
#include "game/game-data-file.hpp"
#include "sfml-util/enum-image-loader.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Responsible for loading Title images.
    class TitleImageLoader : public EnumImageLoader<creature::Titles>
    {
    public:
        TitleImageLoader(const TitleImageLoader &) = delete;
        TitleImageLoader(TitleImageLoader &&) = delete;
        TitleImageLoader & operator=(const TitleImageLoader &) = delete;
        TitleImageLoader & operator=(TitleImageLoader &&) = delete;

        TitleImageLoader()
            : EnumImageLoader<creature::Titles>(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-titles-dir"))
        {}
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TITLE_IMAGE_LOADER_HPP_INCLUDED
