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
// main.cpp
//  Test SFML rendering to develop some gui classes.
//
#include "configbase/configbase.hpp"
#include "common/logmacros.hpp"

#include "utilz/platform.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "game/combat/strategy-details.hpp"
#include "game/loop-manager.hpp"
#include "game/logger.hpp"
#include "game/game-data-file.hpp"
#include "game/settings-file.hpp"
#include "game/game.hpp"
#include "game/creature/title-warehouse.hpp"
#include "game/creature/condition-warehouse.hpp"
#include "game/spell/spell-warehouse.hpp"

#include "utilz/random.hpp"


int main()
{
    std::cout << "Starting Heroes Path..." << std::endl;

    game::LoggerSPtr_t logSPtr(game::Logger::Instance());

    try
    {
        utilz::Platform::Instance()->DetectAndLog();
        if (utilz::Platform::Instance()->IsSupported() == false)
        {
            throw std::runtime_error("This system (platform) is not supported.");
        }

        //seed the random number generator
        utilz::random::MersenneTwister::Seed();

        //keep an instance of various singleton classes here to prevent thrashing
        sfml_util::DisplaySPtr_t displaySPtr(sfml_util::Display::Instance());
        //
        M_LOG(*logSPtr, "Loading the Game Data file...");
        auto gameDataFilePtr(game::GameDataFile::Instance());

        //set which stage will startup
        game::LoopManager::SetStartupStage( gameDataFilePtr->GetCopy<std::string>("system-startup-stage") );

        //setup the graphics display
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::WinSPtr_t winSPtr{ sfml_util::Display::OpenRenderWindow("Heroes' Path", sf::Style::Fullscreen, 0/*default to antialiasing disabled*/) };
        winSPtr->setFramerateLimit(static_cast<unsigned int>(gameDataFilePtr->GetCopyInt("system-window-frame-rate-limit", 0)) );
        winSPtr->setVerticalSyncEnabled( gameDataFilePtr->GetCopyBool("system-window-sync", true) );

        //set resource paths for manager classes
        sfml_util::FontManager::SetFontsDirectory(                      gameDataFilePtr->GetMediaPath("media-fonts-dir"));
        sfml_util::gui::PopupManager::SetTexturesDirectoryPaths(        gameDataFilePtr->GetMediaPath("media-images-backgrounds-popup-dir"), gameDataFilePtr->GetMediaPath("media-images-accents-dir"));
        sfml_util::SoundManager::SetSoundsDirectory(                    gameDataFilePtr->GetMediaPath("media-sounds-dir"), gameDataFilePtr->GetMediaPath("media-music-dir"));
        sfml_util::gui::ItemImageManager::SetItemImageDirectory(        gameDataFilePtr->GetMediaPath("media-images-items-dir"));
        sfml_util::gui::CreatureImageManager::SetCreatureImageDirectory(gameDataFilePtr->GetMediaPath("media-images-creatures-dir") );
        sfml_util::gui::TitleImageManager::SetTitleImageDirectory(      gameDataFilePtr->GetMediaPath("media-images-titles-dir") );
        sfml_util::gui::SpellImageManager::SetImagesDirectory(          gameDataFilePtr->GetMediaPath("media-images-spells-dir") );
        sfml_util::gui::ConditionImageManager::SetImagesDirectory(      gameDataFilePtr->GetMediaPath("media-images-conditions-dir") );
        sfml_util::gui::CombatImageManager::SetImagesDirectory(         gameDataFilePtr->GetMediaPath("media-images-combat-dir") );
        
        try
        {
            //load game assets Stage 1
            game::creature::title::Warehouse::Setup();
            game::creature::condition::Warehouse::Setup();
            game::spell::Warehouse::Setup();

            //Call up an instance of the singleton classes here ensure they load
            //now during startup and not at some other random time during play.
            //Also, see below for the InstanceRelease() calls.
            sfml_util::FontManager::Instance();
            sfml_util::gui::PopupManager::Instance();
            auto soundManagerSPtr         { sfml_util::SoundManager::Instance() };
            auto guiElementsSPtr          { sfml_util::gui::GuiElements::Instance() };
            auto itemImageManagerSPtr     { sfml_util::gui::ItemImageManager::Instance() };
            auto creatureImageManagerSPtr { sfml_util::gui::CreatureImageManager::Instance() };
            auto titleImageManagerSPtr    { sfml_util::gui::TitleImageManager::Instance() };
            auto spellImageManagerSPtr    { sfml_util::gui::SpellImageManager::Instance() };
            auto conditionImageManagerSPtr{ sfml_util::gui::ConditionImageManager::Instance() };
            auto combatImageManagerSPtr   { sfml_util::gui::CombatImageManager::Instance() };
            game::Game::Instance();

            //create/load/store the game settings file
            auto settingsFileSPtr( game::SettingsFile::Instance() );
            settingsFileSPtr->LoadAndRestore();

            //load game assets Stage 3
            //NOTE:  This must occur after SettingsFile::LoadAndRestore()
            //       so that the sound effects created here have the correct
            //       volume loaded from the settings file.
            soundManagerSPtr->LoadSoundSets(); 
            game::combat::strategy::ChanceFactory::Instance()->Initialize();
            sfml_util::gui::PopupManager::Instance()->LoadAssets();

            //create the game state manager and run the game
            game::LoopManagerSPtr_t gameStateSPtr(game::LoopManager::Instance());
            gameStateSPtr->Execute();

            //save settings
            settingsFileSPtr->AcquireAndSave();
            settingsFileSPtr.reset();
        }
        catch (const std::exception & E)
        {
            std::cout << "game threw std::exception(\"" << E.what() << "\")" << std::endl;
            M_LOG(*logSPtr, "game threw std::exception(\"" << E.what() << "\")");
        }
        catch (...)
        {
            std::cout << "game threw an unknown non-std exception." << std::endl;
            M_LOG(*logSPtr, "game threw an unknown non-std exception.");
        }

        //ensure the window is closed before exiting
        if (winSPtr->isOpen())
        {
            winSPtr->close();
        }

        //release singleton/manager instances
        game::Game::InstanceRelease();
        sfml_util::FontManager::InstanceRelease();
        sfml_util::gui::PopupManager::InstanceRelease();
        game::GameDataFile::InstanceRelease();

        M_LOG( * logSPtr, "Reached the end of main within the try-catch.");
    }
    catch (const std::exception & E)
    {
        M_LOG( * logSPtr, "Appication framework threw std::exception \"" << E.what() << "\"");
        return EXIT_FAILURE;
    }
    catch (...)
    {
        M_LOG( * logSPtr, "Appication framework threw an unknown non-std exception." << std::endl);
        return EXIT_FAILURE;
    }

    std::cout << "...Stopping Heroes Path." << std::endl;
    return EXIT_SUCCESS;
}
