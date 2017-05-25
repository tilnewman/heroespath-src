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
// combat-image-manager.hpp
//
#include "combat-image-manager.hpp"

#include "game/loop-manager.hpp"

#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>


namespace sfml_util
{
namespace gui
{

    std::unique_ptr<CombatImageManager> CombatImageManager::instanceUPtr_{ nullptr };
    std::string CombatImageManager::imagesDirectoryPath_{ "" };


    CombatImageManager::CombatImageManager()
    {}


    CombatImageManager * CombatImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void CombatImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new CombatImageManager);
        }
    }


    void CombatImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::CombatImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    bool CombatImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CombatImageManager::Test()  Starting Tests...");
        }

        static auto willFlip{ true };
        static auto imageIndex{ 0 };
        if (imageIndex < CombatImageType::Count)
        {
            auto const ENUM{ static_cast<CombatImageType::Enum>(imageIndex) };
            auto const FILENAME{ CombatImageType::Filename(ENUM) };
            auto textureSPtr{ Get(ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((textureSPtr.get() != nullptr), "sfml_util::gui::CombatImageManager::Test()  Get(\"" << FILENAME << "\") returned a nullptr.");

            willFlip = ! willFlip;

            if (willFlip)
            {
                sfml_util::FlipHoriz( * textureSPtr);
                game::LoopManager::Instance()->TestingImageSet(textureSPtr);
                game::LoopManager::Instance()->TestingStrAppend("CombatImageManager Tested " + FILENAME);
            }
            else
            {
                game::LoopManager::Instance()->TestingImageSet(textureSPtr);
                game::LoopManager::Instance()->TestingStrAppend("CombatImageManager Tested " + FILENAME);
                return false;
            }

            ++imageIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CombatImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    TextureSPtr_t CombatImageManager::Get(const CombatImageType::Enum E, const bool WILL_FLIP_HORIZ) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(CombatImageType::Filename(E))));

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, PATH_OBJ.string());

        if (WILL_FLIP_HORIZ)
            sfml_util::FlipHoriz( * textureSPtr);

        return textureSPtr;
    }


}
}
