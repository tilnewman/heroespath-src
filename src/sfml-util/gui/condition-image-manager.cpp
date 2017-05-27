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
// condition-image-manager.cpp
//
#include "condition-image-manager.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"

#include "sfml-util/loaders.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/filesystem.hpp>


namespace sfml_util
{
namespace gui
{

    std::unique_ptr<ConditionImageManager> ConditionImageManager::instanceUPtr_{ nullptr };
    std::string ConditionImageManager::conditionImagesDirectory_{ "" };
    const std::string ConditionImageManager::filenameExtension_{ ".png" };


    ConditionImageManager::ConditionImageManager()
    {}


    ConditionImageManager * ConditionImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ConditionImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ConditionImageManager);
        }
    }


    void ConditionImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::ConditionImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    bool ConditionImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ConditionImageManager::Test()  Starting Tests...");
        }

        auto cimPtr{ ConditionImageManager::Instance() };

        static auto condIndex{ 0 };
        if (condIndex < game::creature::Conditions::Count)
        {
            auto const ENUM{ static_cast<game::creature::Conditions::Enum>(condIndex) };
            auto const ENUM_STR{ game::creature::Conditions::ToString(ENUM) };
            sf::Texture texture;
            cimPtr->Get(texture, ENUM);
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrAppend("ConditionImageManager Tested " + ENUM_STR);
            ++condIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ConditionImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    void ConditionImageManager::Get(sf::Texture & texture, game::creature::Conditions::Enum ENUM) const
    {
        sfml_util::LoadImageOrTexture(texture, MakeFilepath(ENUM).string());
    }


    const std::string ConditionImageManager::MakeFilename(const game::creature::Conditions::Enum ENUM) const
    {
        return boost::algorithm::to_lower_copy(game::creature::Conditions::ToString(ENUM)) + filenameExtension_;
    }


    const boost::filesystem::path ConditionImageManager::MakeFilepath(const game::creature::Conditions::Enum ENUM) const
    {
        namespace bfs = boost::filesystem;
        return bfs::system_complete( bfs::path(conditionImagesDirectory_) / bfs::path(MakeFilename(ENUM)) );
    }

}
}
