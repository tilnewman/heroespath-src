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
#ifndef SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
#define SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
//
// creature-image-manager.hpp
//  Code that manages loading and lifetime of creature images.
//
#include "sfml-util/sfml-graphics.hpp"

#include "game/creature/role-enum.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/wolfen-class-enum.hpp"
#include "game/creature/dragon-class-enum.hpp"

#include <memory>
#include <map>
#include <tuple>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    class CreatureImageManager;
    using CreatureImageManagerSPtr_t = std::shared_ptr<CreatureImageManager>;


    //A class that loads, stores, and distributes creature images.
    class CreatureImageManager
    {
        //prevent copy construction
        CreatureImageManager(const CreatureImageManager &) =delete;

        //prevent copy assignment
        CreatureImageManager & operator=(const CreatureImageManager &) =delete;

        //prevent non-singleton construction
        CreatureImageManager();

    public:
        virtual ~CreatureImageManager();

        static CreatureImageManagerSPtr_t Instance();
        static void SetCreatureImageDirectory(const std::string & PATH);

        static bool Test();

        inline static float DimmensionMax() { return 256.0f; }

        const std::string GetFilename(const game::creature::race::Enum         RACE,
                                      const game::creature::role::Enum         ROLE,
                                      const game::creature::sex::Enum          SEX,
                                      const bool                                     WILL_PICK_RANDOM = true,
                                      const game::creature::wolfen_class::Enum WOLFEN_CLASS     = game::creature::wolfen_class::Pup,
                                      const game::creature::dragon_class::Enum DRAGON_CLASS     = game::creature::dragon_class::Hatchling) const;

        const TextureSPtr_t GetImage(const game::creature::race::Enum         RACE,
                                     const game::creature::role::Enum         ROLE,
                                     const game::creature::sex::Enum          SEX              = game::creature::sex::Male,
                                     const bool                                     WILL_PICK_RANDOM = true,
                                     const bool                                     WILL_FACE_RIGHT  = false,
                                     const game::creature::wolfen_class::Enum WOLFEN_CLASS     = game::creature::wolfen_class::Pup,
                                     const game::creature::dragon_class::Enum DRAGON_CLASS     = game::creature::dragon_class::Hatchling) const;

        const TextureSPtr_t GetImage(const std::string & FILENAME,
                                     const bool          WILL_FACE_RIGHT = false) const;

        void GetFilenames(std::vector<std::string> &                     outputVec,
                          const game::creature::race::Enum         RACE,
                          const game::creature::role::Enum         ROLE,
                          const game::creature::sex::Enum          SEX,
                          const game::creature::wolfen_class::Enum WOLFEN_CLASS = game::creature::wolfen_class::Pup,
                          const game::creature::dragon_class::Enum DRAGON_CLASS = game::creature::dragon_class::Hatchling) const;

    private:
        //assumes there is no matching entry in textureMap_
        TextureSPtr_t LoadImage(const std::string & IMAGE_FILE_NAME,
                                const bool          WILL_FACE_RIGHT = false) const;

    private:
        static std::string imagesDirectoryPath_;
        static CreatureImageManagerSPtr_t instanceSPtr_;
    };

}
}
#endif //SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
