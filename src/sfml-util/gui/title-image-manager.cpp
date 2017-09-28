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
#include "game/loop-manager.hpp"

#include <boost/filesystem.hpp>
#include <sstream>
#include <exception>


namespace sfml_util
{
namespace gui
{

    std::string TitleImageManager::imagesDirectoryPath_;
    std::unique_ptr<TitleImageManager> TitleImageManager::instanceUPtr_{ nullptr };


    TitleImageManager::TitleImageManager()
    :
        placeHlderFilePathObj_( boost::filesystem::system_complete(boost::filesystem::path(imagesDirectoryPath_) / PlaceholderFileName()) )
    {
        M_HP_LOG_DBG("Singleton Construction: TitleImageManager");
        if (boost::filesystem::exists(placeHlderFilePathObj_) == false)
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::TitleImageManager::Constructor() could not locate the placeholder image at: \"" << placeHlderFilePathObj_.string() << "\".";
            throw std::runtime_error(ss.str());
        }
    }


    TitleImageManager::~TitleImageManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: TitleImageManager");
    }


    TitleImageManager * TitleImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): TitleImageManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void TitleImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<TitleImageManager>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: TitleImageManager");
        }
    }


    void TitleImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::TitleImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void TitleImageManager::SetTitleImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    bool TitleImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::gui::TitleImageManager::Test() Starting Tests...");
        }

        auto timPtr{ TitleImageManager::Instance() };

        static auto titleIndex{ 0 };
        if (titleIndex < game::creature::Titles::Count)
        {
            auto const ENUM{ static_cast<game::creature::Titles::Enum>(titleIndex) };
            auto const ENUM_STR{ game::creature::Titles::ToString(ENUM) };
            sf::Texture texture;
            timPtr->Get(texture, ENUM);
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrAppend("TitleImageManager Tested " + ENUM_STR);
            ++titleIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "sfml_util::gui::TitleImageManager::Test()  ALL TESTS PASSED.");

        return true;
    }


    void TitleImageManager::Get(sf::Texture & texture, game::creature::TitlePtr_t TITLE_PTR) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ( bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(TITLE_PTR->ImageFilename())) );
        bfs::path pathToUse(PATH_OBJ);

        if (bfs::exists(pathToUse) == false)
        {
            M_HP_LOG_ERR("sfml_util::gui::TitleImageManager::Get(title=\"" << TITLE_PTR->Name() << "\", title_filename=\"" << TITLE_PTR->ImageFilename() << "\") was unable to find the image for a title at: \"" << pathToUse.string() << "\".");
            pathToUse = placeHlderFilePathObj_;
        }

        sfml_util::LoadTexture(texture, pathToUse.string());
    }

}
}
