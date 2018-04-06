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
#ifndef HEROESPATH_SFMLUTIL_GUI_TITLEIMAGEMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_TITLEIMAGEMANAGER_HPP_INCLUDED
//
// title-image-manager.hpp
//  Code that manages loading and lifetime of title images.
//
#include "creature/title-enum.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/image-manager-base.hpp"

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // A class that loads, stores, and distributes creature images.
        class TitleImageManager : public ImageManagerBase<creature::Titles>
        {
        public:
            TitleImageManager(const TitleImageManager &) = delete;
            TitleImageManager(TitleImageManager &&) = delete;
            TitleImageManager & operator=(const TitleImageManager &) = delete;
            TitleImageManager & operator=(TitleImageManager &&) = delete;

        public:
            TitleImageManager();
            virtual ~TitleImageManager();

            static misc::NotNull<TitleImageManager *> Instance();
            static void Acquire();
            static void Release();

        private:
            static std::unique_ptr<TitleImageManager> instanceUPtr_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_TITLEIMAGEMANAGER_HPP_INCLUDED
