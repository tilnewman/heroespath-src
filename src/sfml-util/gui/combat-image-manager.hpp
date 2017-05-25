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
#ifndef SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
//
// combat-image-manager.hpp
//
#include <sfml-util/gui/combat-image-enum.hpp>

#include <string>
#include <memory>


namespace sf
{
    class Texture;
}
namespace sfml_util
{
    using TextureSPtr_t = std::shared_ptr<sf::Texture>;

namespace gui
{

    //singleton responsible for loading and dispensing combat image TextureSPtr_t
    class CombatImageManager
    {
        //prevent copy assignment
        CombatImageManager & operator=(const CombatImageManager &) =delete;

        //prevent copy construction
        CombatImageManager(const CombatImageManager &) =delete;

        //prevent non-singleton construction
        CombatImageManager();

    public:
        static CombatImageManager * Instance();
        static void Acquire();
        static void Release();

        inline static void SetImagesDirectory(const std::string & S) { imagesDirectoryPath_ = S; }

        bool Test();

        TextureSPtr_t Get(const CombatImageType::Enum, const bool WILL_FLIP_HORIZ = false) const;

    private:
        static std::unique_ptr<CombatImageManager> instanceUPtr_;
        static std::string imagesDirectoryPath_;
    };

}
}

#endif //SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED