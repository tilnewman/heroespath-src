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
// startup-shutdown.cpp
//
#include "startup-shutdown.hpp"

#include "combat/encounter.hpp"
#include "combat/strategy-details.hpp"
#include "combat/trap-warehouse.hpp"
#include "config/configbase.hpp"
#include "config/settings-file.hpp"
#include "creature/condition-warehouse.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/enchantment-factory.hpp"
#include "creature/name-info.hpp"
#include "creature/title-warehouse.hpp"
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "item/armor-details.hpp"
#include "item/armor-factory.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-factory.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-warehouse.hpp"
#include "item/misc-item-factory.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-factory.hpp"
#include "log/logger.hpp"
#include "log/macros.hpp"
#include "misc/platform.hpp"
#include "misc/random.hpp"
#include "non-player/chance-factory.hpp"
#include "non-player/inventory-factory.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/texture-cache.hpp"
#include "song/song-warehouse.hpp"
#include "spell/spell-warehouse.hpp"
#include "state/game-state-factory.hpp"
#include "state/npc-warehouse.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

namespace heroespath
{
namespace game
{

    bool StartupShutdown::Setup(const std::string & APPLICATION_NAME, const int ARGC, char * argv[])
    {
        try
        {
            // initialize the log first so that all Setup() actions can be logged
            log::Logger::Acquire();

            DetectLogAndCheckPlatform();

            srand(static_cast<unsigned>(time(nullptr)));
            misc::random::MersenneTwister::Seed();

            ParseCommandLineArguments(ARGC, argv);

            // GameDataFile is used everywhere so Acquire and Initialize it before everything else
            game::GameDataFile::Acquire();
            game::GameDataFile::Instance()->Initialize();

            // this order is critical
            SetupDisplay(APPLICATION_NAME);
            SetManagerClassResourcePaths();
            SingletonsAcquire();
            SingletonsInitialize();
            WarehousesFill();

            // this causes the initial stage transition/creation so it must occur last
            LoopManager::Instance()->Initialize();

            return true;
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "Appication Setup Framework threw std::exception \"" << E.what() << "\"");
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "Appication Setup Framework threw unknown (non-std) exception.");
        }

        return false;
    }

    int StartupShutdown::Run()
    {
        try
        {
            if (game::LoopManager::Instance()->Execute())
            {
                return EXIT_SUCCESS;
            }
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "Application threw std::exception \"" << E.what() << "\"");
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(), "Application threw an unknown (non-std) exception.");
        }

        return EXIT_FAILURE;
    }

    int StartupShutdown::Teardown()
    {
        int exitCode{ EXIT_SUCCESS };

        Teardown_SettingsFile(exitCode);
        Teardown_CloseDisplay(exitCode);
        Teardown_EmptyWarehouses(exitCode);
        Teardown_ReleaseSingletons(exitCode);
        Teardown_Logger(exitCode);

        return exitCode;
    }

    void StartupShutdown::Teardown_SettingsFile(int & exitCode_OutParam)
    {
        try
        {
            config::SettingsFile::Instance()->AcquireAndSave();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "SettingsFile::Teardown_SettingsFile() threw std::exception \"" << E.what()
                                                                                << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "SettingsFile::Teardown_SettingsFile() threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_CloseDisplay(int & exitCode_OutParam)
    {
        try
        {
            if (sfml_util::Display::Instance()->IsOpen())
            {
                sfml_util::Display::Instance()->Close();
            }
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_CloseDisplay() threw std::exception \"" << E.what()
                                                                                   << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_CloseDisplay() threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_EmptyWarehouses(int & exitCode_OutParam)
    {
        try
        {
            WarehousesEmpty();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_EmptyWarehouses() threw std::exception \"" << E.what()
                                                                                      << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_EmptyWarehouses() "
                    << "threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_ReleaseSingletons(int & exitCode_OutParam)
    {
        try
        {
            SingletonsRelease();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_ReleaseSingletons() threw std::exception \"" << E.what()
                                                                                        << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_ReleaseSingletons() "
                    << "threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_Logger(int & exitCode_OutParam)
    {
        try
        {
            log::Logger::Release();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_Logger() threw std::exception \"" << E.what() << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_Logger() threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::ParseCommandLineArguments(const int ARGC, char * argv[])
    {
        if (ARGC >= 2)
        {
            std::cout << "Will attempt to start in stage: \"" << argv[1] << "\"" << std::endl;
            LoopManager::SetStartupStage(argv[1]);

            if (ARGC > 2)
            {
                for (int i(2); i < ARGC; ++i)
                {
                    std::cout << "Ignoring extra command line argument: \"" << argv[i] << "\""
                              << std::endl;
                }
            }
        }
    }

    void StartupShutdown::DetectLogAndCheckPlatform()
    {
        misc::Platform::Acquire();
        misc::Platform::Instance()->DetectAndLog();

        if (misc::Platform::Instance()->IsSupported() == false)
        {
            throw std::runtime_error(
                "This system (platform) is not supported.  See log for details.");
        }
    }

    void StartupShutdown::SetupDisplay(const std::string & APPLICATION_NAME)
    {
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::Display::Acquire(APPLICATION_NAME, sf::Style::Fullscreen, 0);

        sfml_util::Display::Instance()->SetFrameRateLimit(static_cast<unsigned>(
            game::GameDataFile::Instance()->GetCopyInt("system-window-frame-rate-limit")));

        sfml_util::Display::Instance()->SetVerticalSync(
            game::GameDataFile::Instance()->GetCopyBool("system-window-sync"));
    }

    void StartupShutdown::SetManagerClassResourcePaths()
    {
        sfml_util::FontManager::SetFontsDirectory(
            game::GameDataFile::Instance()->GetMediaPath("media-fonts-dir"));

        popup::PopupManager::SetTexturesDirectoryPaths(
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-popup-dir"),
            game::GameDataFile::Instance()->GetMediaPath("media-images-accents-dir"));

        sfml_util::SoundManager::SetSoundsDirectory(
            game::GameDataFile::Instance()->GetMediaPath("media-sounds-dir"),
            game::GameDataFile::Instance()->GetMediaPath("media-music-dir"));

        sfml_util::gui::ItemImageManager::SetItemImageDirectory(
            game::GameDataFile::Instance()->GetMediaPath("media-images-items-dir"));

        sfml_util::gui::CreatureImageManager::SetCreatureImageDirectory(
            game::GameDataFile::Instance()->GetMediaPath("media-images-creatures-dir"));

        sfml_util::gui::TitleImageManager::PathToImages(
            game::GameDataFile::Instance()->GetMediaPath("media-images-titles-dir"));

        sfml_util::gui::SpellImageManager::PathToImages(
            game::GameDataFile::Instance()->GetMediaPath("media-images-spells-dir"));

        sfml_util::gui::SongImageManager::PathToImages(
            game::GameDataFile::Instance()->GetMediaPath("media-images-songs-dir"));

        sfml_util::gui::ConditionImageManager::PathToImages(
            game::GameDataFile::Instance()->GetMediaPath("media-images-conditions-dir"));

        sfml_util::gui::CombatImageManager::PathToImages(
            game::GameDataFile::Instance()->GetMediaPath("media-images-combat-dir"));
    }

    void StartupShutdown::WarehousesFill()
    {
        sfml_util::FontManager::Fill();
        creature::title::Warehouse::Fill();
        creature::condition::Warehouse::Fill();
        spell::Warehouse::Fill();
        song::Warehouse::Fill();
        combat::trap::Warehouse::Fill();
    }

    void StartupShutdown::WarehousesEmpty()
    {
        combat::trap::Warehouse::Empty();
        song::Warehouse::Empty();
        spell::Warehouse::Empty();
        creature::condition::Warehouse::Empty();
        creature::title::Warehouse::Empty();
        sfml_util::FontManager::Empty();
    }

    void StartupShutdown::SingletonsAcquire()
    {
        state::NpcWarehouse::Acquire();
        sfml_util::TextureCache::Acquire();
        creature::EnchantmentFactory::Acquire();
        item::ItemWarehouse::Acquire();
        creature::CreatureWarehouse::Acquire();
        item::weapon::WeaponFactory::Acquire();
        item::MiscItemFactory::Acquire();
        sfml_util::SoundManager::Acquire();
        sfml_util::FontManager::Acquire();
        popup::PopupManager::Acquire();
        sfml_util::gui::GuiElements::Acquire();
        sfml_util::gui::ItemImageManager::Acquire();
        sfml_util::gui::CreatureImageManager::Acquire();
        sfml_util::gui::TitleImageManager::Acquire();
        sfml_util::gui::SpellImageManager::Acquire();
        sfml_util::gui::ConditionImageManager::Acquire();
        sfml_util::gui::CombatImageManager::Acquire();
        sfml_util::gui::SongImageManager::Acquire();
        Game::Acquire();
        state::GameStateFactory::Acquire();
        creature::NameInfo::Acquire();
        item::armor::ArmorDetailLoader::Acquire();
        item::armor::ArmorFactory::Acquire();
        item::weapon::WeaponDetailLoader::Acquire();
        non_player::ownership::InventoryFactory::Acquire();
        combat::Encounter::Acquire();
        item::ArmorRatings::Acquire();
        item::ItemProfileWarehouse::Acquire();
        item::ItemFactory::Acquire();
        non_player::ownership::ChanceFactory::Acquire();
        config::SettingsFile::Acquire();
        LoopManager::Acquire();
    }

    void StartupShutdown::SingletonsInitialize()
    {
        // SettingsFile::LoadAndRestore() must happen before initialization so that subsystems can
        // use the settings saved from the last run of the game.
        config::SettingsFile::Instance()->LoadAndRestore();

        sfml_util::SoundManager::Instance()->Initialize();
        combat::strategy::ChanceFactory::Instance()->Initialize();
        popup::PopupManager::Instance()->LoadAccentImagePaths();
        item::ArmorRatings::Instance()->Setup();
        non_player::ownership::ChanceFactory::Instance()->Initialize();
    }

    void StartupShutdown::SingletonsRelease()
    {
        LoopManager::Release();
        config::SettingsFile::Release();
        item::ItemFactory::Release();
        item::ItemProfileWarehouse::Release();
        item::ArmorRatings::Release();
        combat::Encounter::Release();
        non_player::ownership::InventoryFactory::Release();
        item::weapon::WeaponDetailLoader::Release();
        item::armor::ArmorFactory::Release();
        item::armor::ArmorDetailLoader::Release();
        creature::NameInfo::Release();
        state::GameStateFactory::Release();
        Game::Release();
        sfml_util::gui::SongImageManager::Release();
        sfml_util::gui::CombatImageManager::Release();
        sfml_util::gui::ConditionImageManager::Release();
        sfml_util::gui::SpellImageManager::Release();
        sfml_util::gui::TitleImageManager::Release();
        sfml_util::gui::CreatureImageManager::Release();
        sfml_util::gui::ItemImageManager::Release();
        sfml_util::gui::GuiElements::Release();
        sfml_util::FontManager::Release();
        popup::PopupManager::Release();
        sfml_util::SoundManager::Release();
        item::MiscItemFactory::Release();
        item::weapon::WeaponFactory::Release();
        creature::CreatureWarehouse::Release();
        item::ItemWarehouse::Release();
        creature::EnchantmentFactory::Release();
        non_player::ownership::ChanceFactory::Release();
        sfml_util::Display::Release();
        misc::Platform::Release();
        sfml_util::TextureCache::Release();
        state::NpcWarehouse::Release();
        GameDataFile::Release();
    }

} // namespace game
} // namespace heroespath
