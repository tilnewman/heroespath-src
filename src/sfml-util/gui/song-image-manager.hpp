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
#ifndef HEROESPATH_SFMLUTIL_GUI_SONGIMAGEMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_SONGIMAGEMANAGER_HPP_INCLUDED
//
// song-image-manager.hpp
//
#include "misc/not-null.hpp"
#include "sfml-util/gui/image-manager-base.hpp"
#include "song/song-enum.hpp"

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Loads images and delivers sf::Textures to them on demand.
        class SongImageManager : public ImageManagerBase<song::Songs>
        {
        public:
            SongImageManager(const SongImageManager &) = delete;
            SongImageManager(SongImageManager &&) = delete;
            SongImageManager & operator=(const SongImageManager &) = delete;
            SongImageManager & operator=(SongImageManager &&) = delete;

        public:
            SongImageManager();
            virtual ~SongImageManager();

            static misc::NotNull<SongImageManager *> Instance();
            static void Acquire();
            static void Release();

        private:
            static std::unique_ptr<SongImageManager> instanceUPtr_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_SONGIMAGEMANAGER_HPP_INCLUDED
