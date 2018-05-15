// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COMBAT_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COMBAT_IMAGE_LOADER_HPP_INCLUDED
//
// combat-image-loader.hpp
//
#include "game/game-data-file.hpp"
#include "sfml-util/gui/combat-image-enum.hpp"
#include "sfml-util/gui/enum-image-loader.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for loading combat images.
        class CombatImageLoader : public EnumImageLoader<CombatImageType>
        {
        public:
            CombatImageLoader & operator=(const CombatImageLoader &) = delete;
            CombatImageLoader & operator=(CombatImageLoader &&) = delete;
            CombatImageLoader(const CombatImageLoader &) = delete;
            CombatImageLoader(CombatImageLoader &&) = delete;

            CombatImageLoader()
                : EnumImageLoader<CombatImageType>(
                      game::GameDataFile::Instance()->GetMediaPath("media-images-combat-dir"))
            {}
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COMBAT_IMAGE_LOADER_HPP_INCLUDED
