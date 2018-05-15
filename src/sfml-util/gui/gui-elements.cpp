// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// gui-elements.cpp
//
#include "gui-elements.hpp"

#include "game/game-data-file.hpp"
#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        sf::Texture GuiElements::elementsTexture_;

        void GuiElements::LoadTexture()
        {
            sfml_util::LoadTexture(
                elementsTexture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-gui-elements"));
        }

        /*sfml_util::LoadTexture(
            darkKnotBgTexture_,
            game::GameDataFile::Instance()->GetMediaPath(
                "media-images-backgrounds-tile-darkknot"));*/

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
