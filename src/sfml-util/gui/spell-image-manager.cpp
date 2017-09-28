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
// spell-image-manager.cpp
//
#include "spell-image-manager.hpp"

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

    std::unique_ptr<SpellImageManager> SpellImageManager::instanceUPtr_{ nullptr };
    std::string SpellImageManager::spellImagesDirectory_{ "" };
    const std::string SpellImageManager::filenameExtension_{ ".png" };


    SpellImageManager::SpellImageManager()
    {
        M_HP_LOG_DBG("Singleton Construction: SpellImageManager");
    }


    SpellImageManager::~SpellImageManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: SpellImageManager");
    }


    SpellImageManager * SpellImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): SpellImageManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void SpellImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<SpellImageManager>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: SpellImageManager");
        }
    }


    void SpellImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "game::SpellImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    bool SpellImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::SpellImageManager::Test() Starting Tests...");
        }

        auto simPtr{ SpellImageManager::Instance() };

        static auto spellIndex{ 0 };
        if (spellIndex < game::spell::Spells::Count)
        {
            auto const ENUM{ static_cast<game::spell::Spells::Enum>(spellIndex) };
            auto const ENUM_STR{ game::spell::Spells::ToString(ENUM) };
            sf::Texture texture;
            simPtr->Get(texture, ENUM);
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrAppend("SpellImageManager Tested " + ENUM_STR);
            ++spellIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::SpellImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    void SpellImageManager::Get(sf::Texture & texture, game::spell::Spells::Enum ENUM) const
    {
        sfml_util::LoadTexture(texture, MakeFilepath(ENUM).string());
    }


    const std::string SpellImageManager::MakeFilename(const game::spell::Spells::Enum ENUM) const
    {
        return boost::algorithm::to_lower_copy(game::spell::Spells::ToString(ENUM)) + filenameExtension_;
    }


    const boost::filesystem::path SpellImageManager::MakeFilepath(const game::spell::Spells::Enum ENUM) const
    {
        namespace bfs = boost::filesystem;
        return bfs::system_complete( bfs::path(spellImagesDirectory_) / bfs::path(MakeFilename(ENUM)) );
    }

}
}
