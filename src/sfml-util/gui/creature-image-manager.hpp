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
#ifndef HEROESPATH_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_HPP_INCLUDED
//
// creature-image-manager.hpp
//  Code that manages loading and lifetime of creature images.
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace sfml_util
{
    namespace gui
    {

        // A class that loads, stores, and distributes creature images.
        class CreatureImageManager
        {
        public:
            CreatureImageManager(const CreatureImageManager &) = delete;
            CreatureImageManager(CreatureImageManager &&) = delete;
            CreatureImageManager & operator=(const CreatureImageManager &) = delete;
            CreatureImageManager & operator=(CreatureImageManager &&) = delete;
            CreatureImageManager() = delete;

            static bool Test();

            static float Dimmension() { return 256.0f; }

            static void GetImage(sf::Texture & texture, const creature::CreaturePtr_t);

            static void GetImageFromFilename(
                sf::Texture & texture, const std::string & FILENAME, const bool WILL_FACE_RIGHT);

            static const std::string GetRandomFilename(const creature::CreaturePtr_t);

            static const std::vector<std::string> GetFilenames(
                const creature::race::Enum RACE,
                const creature::role::Enum ROLE,
                const creature::sex::Enum SEX,
                const creature::wolfen_class::Enum WOLFEN_CLASS = creature::wolfen_class::Pup,
                const creature::dragon_class::Enum DRAGON_CLASS
                = creature::dragon_class::Hatchling);

        private:
            static void LoadImage(
                sf::Texture & texture,
                const std::string & IMAGE_FILE_NAME,
                const bool WILL_FACE_RIGHT = false);
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_HPP_INCLUDED
