// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// title-image-manager.cpp
//
#include "title-image-manager.hpp"

#include "sfml-util/loaders.hpp"

#include "game/log-macros.hpp"
#include "game/creature/title.hpp"

#include <boost/filesystem.hpp>
#include <sstream>
#include <exception>


namespace sfml_util
{
namespace gui
{

    std::string             TitleImageManager::imagesDirectoryPath_;
    TitleImageManagerSPtr_t TitleImageManager::instanceSPtr_;


    TitleImageManager::TitleImageManager()
    :
        placeHlderFilePathObj_( boost::filesystem::system_complete(boost::filesystem::path(imagesDirectoryPath_) / PlaceholderFileName()) )
    {
        if (boost::filesystem::exists(placeHlderFilePathObj_) == false)
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::TitleImageManager::Constructor() could not locate the placeholder image at: \"" << placeHlderFilePathObj_.string() << "\".";
            throw std::runtime_error(ss.str());
        }
    }


    TitleImageManager::~TitleImageManager()
    {}


    TitleImageManagerSPtr_t TitleImageManager::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new TitleImageManager );

        return instanceSPtr_;
    }


    void TitleImageManager::SetTitleImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    const TextureSPtr_t TitleImageManager::Get(game::creature::TitlePtr_t TITLE_PTR) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ( bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(TITLE_PTR->ImageFilename())) );
        bfs::path pathToUse(PATH_OBJ);

        if (bfs::exists(pathToUse) == false)
        {
            M_HP_LOG("sfml_util::gui::TitleImageManager::Get(title=\"" << TITLE_PTR->Name() << "\", title_filename=\"" << TITLE_PTR->ImageFilename() << "\") was unable to find the image for a title at: \"" << pathToUse.string() << "\".");
            pathToUse = placeHlderFilePathObj_;
        }

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, pathToUse.string());
        return textureSPtr;
    }

}
}
