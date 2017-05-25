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
#ifndef SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
//
// spell-image-manager.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include "game/spell/spell-enum.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <memory>


namespace sfml_util
{
namespace gui
{

    //Loads images and delivers sfml_util::TextureSPtr_ts to them on demand.
    class SpellImageManager
    {
        //prevent copy construction
        SpellImageManager(const SpellImageManager &) =delete;

        //prevent copy assignment
        SpellImageManager & operator=(const SpellImageManager &) =delete;

        //prevent non-singleton construction
        SpellImageManager();

    public:
        static SpellImageManager * Instance();
        static void Acquire();
        static void Release();

        inline static void SetImagesDirectory(const std::string & S)    { spellImagesDirectory_ = S; }
        inline static float Dimmension()                                { return 256.0f; }
        static bool Test();

        sfml_util::TextureSPtr_t Get(const game::spell::Spells::Enum) const;

    private:
        const std::string MakeFilename(const game::spell::Spells::Enum) const;
        const boost::filesystem::path MakeFilepath(const game::spell::Spells::Enum) const;
    private:
        static std::unique_ptr<SpellImageManager> instanceUPtr_;
        static std::string spellImagesDirectory_;
        static const std::string filenameExtension_;
    };

}
}

#endif //SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
