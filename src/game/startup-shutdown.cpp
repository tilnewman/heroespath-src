// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// startup-shutdown.cpp
//
#include "startup-shutdown.hpp"

#include "combat/encounter.hpp"
#include "combat/trap-holder.hpp"
#include "creature/condition-holder.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/enchantment-warehouse.hpp"
#include "creature/nonplayer-inventory-chances.hpp"
#include "creature/nonplayer-inventory-factory.hpp"
#include "creature/title-holder.hpp"
#include "game/game-controller.hpp"
#include "game/game-state-factory.hpp"
#include "game/game.hpp"
#include "game/loop.hpp"
#include "game/npc-warehouse.hpp"
#include "gui/content-images.hpp"
#include "gui/creature-image-paths.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/item-image-paths.hpp"
#include "gui/sound-manager.hpp"
#include "gui/texture-cache.hpp"
#include "item/armor-details.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "item/weapon-details.hpp"
#include "map/parser.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/log.hpp"
#include "misc/platform.hpp"
#include "misc/random.hpp"
#include "misc/settings-file.hpp"
#include "popup/popup-manager.hpp"
#include "song/song-holder.hpp"
#include "spell/spell-holder.hpp"

#include <SFML/Window/WindowStyle.hpp>

#include <iostream>
#include <stdexcept>

namespace heroespath
{
namespace game
{

    StartupShutdown::StartupShutdown(
        const std::string & APPLICATION_NAME,
        const int ARGC,
        char * argv[],
        const bool WILL_SETUP_FOR_TESTING)
    {
        Setup(APPLICATION_NAME, ARGC, argv, WILL_SETUP_FOR_TESTING);
    }

    void StartupShutdown::Setup(
        const std::string & APPLICATION_NAME,
        const int ARGC,
        char * argv[],
        const bool WILL_SETUP_FOR_TESTING) const
    {
        // this order is critical
        misc::stringStreamHolder::init();

        // initialize the log first so that all Setup() actions can be logged
        misc::Log::Acquire();
        // heroespath::LogMacroHelper::LogTimingStart();
        M_HP_LOG("game setup starting");

        if (WILL_SETUP_FOR_TESTING)
        {
            M_HP_LOG("game setup WILL be customized to run UNIT TESTING");
        }

        M_HP_LOG("Detected Platform \"" << misc::platform_name << "\"");
        if (!misc::platform_is_supported)
        {
            const auto ERROR_MESSAGE(
                std::string("This enviornment/operating_system/platform/whatever, called \"")
                    .append(misc::platform_name)
                    .append("\" is not supported."));

            M_HP_LOG_ERR(ERROR_MESSAGE);
            throw std::runtime_error(ERROR_MESSAGE);
        }

        misc::helpers::MersenneTwister19937::Setup();

        Setup_ParseCommandLineArguments(ARGC, argv);

        misc::ConfigFile::Acquire();
        misc::ConfigFile::Instance()->Initialize();

        item::armor::ArmorDetailLoader::LoadFromGameDataFile();
        item::weapon::WeaponDetailLoader::LoadFromGameDataFile();

        Setup_Display(APPLICATION_NAME, WILL_SETUP_FOR_TESTING);
        Setup_FilesystemPaths();
        Setup_SubsystemsAcquire();
        Setup_SubsystemsInitialize(WILL_SETUP_FOR_TESTING);
        Setup_HoldersFill();

        // this causes the initial stage transition/creation so it must occur last
        GameController::Instance()->Initialize();

        M_HP_LOG("game setup finished");
    }

    StartupShutdown::~StartupShutdown() { Teardown(); }

    void StartupShutdown::Run() const { game::GameController::Instance()->PlayGame(); }

    void StartupShutdown::Teardown() const
    {
        // heroespath::LogMacroHelper::LogTimingStop();
        M_HP_LOG("game teardown starting");
        Teardown_SettingsFile();
        Teardown_CloseDisplay();
        Teardown_ReleaseSubsystems();
        Teardown_EmptyHolders();
        M_HP_LOG("game teardown (almost) finished");
        Teardown_Logger();
    }

    void StartupShutdown::Teardown_SettingsFile() const
    {
        try
        {
            misc::SettingsFile::Instance()->AcquireAndSave();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_SettingsFile() threw std::exception \""
                << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_SettingsFile() threw an unknown (non-std) exception.");
        }
    }

    void StartupShutdown::Teardown_CloseDisplay() const
    {
        try
        {
            gui::Display::Instance()->Close();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_CloseDisplay() threw std::exception \""
                << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_CloseDisplay() threw an unknown (non-std) exception.");
        }
    }

    void StartupShutdown::Teardown_EmptyHolders() const
    {
        try
        {
            combat::trap::Holder::Empty();
            song::Holder::Empty();
            spell::Holder::Empty();
            creature::condition::Holder::Empty();
            creature::title::Holder::Empty();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_EmptyHolders() threw std::exception \""
                << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_EmptyHolders() "
                << "threw an unknown (non-std) exception.");
        }
    }

    void StartupShutdown::Teardown_ReleaseSubsystems() const
    {
        try
        {
            // this order is critical - be careful here...

            // release the SettingsFile first so that it saves the settings file even if a problem
            // happens during shutdown
            misc::SettingsFile::Release();

            // factories should not be needed during shutdown, so release them early
            game::GameStateFactory::Release();
            creature::nonplayer::ChanceFactory::Release();

            // release GameController early because it frees all the stages and their resources
            GameController::Release();

            // these must occur after all the stages have been released
            gui::FontManager::Release(); // after NameInfo::Release()
            popup::PopupManager::Release();
            gui::SoundManager::Release();
            gui::TextureCache::Release();

            // this will cause all party creatures/items/enchantments/etc to be Warehouses::Free()'d
            Game::Release();

            // must occur after Game::Release() because game::GameState needs it
            game::NpcWarehouse::Release();

            // if this is an abnormal shutdown then the Encounter object will have some
            // Warehouse::Free() calls to make, so this must occur after Game::Release() but before
            // creature/item/enchantment/etc warehouses are released.
            combat::Encounter::Release();

            // there are dependencies between these Warehouses so this release order is critical
            creature::CreatureWarehouse::Release();
            item::ItemProfileWarehouse::Release();
            item::ItemWarehouse::Release();
            creature::EnchantmentWarehouse::Release();

            // the only constraint here is that it must occur after all the stages have been
            // released, so there is no reason for this to be one of the last things to be released,
            // but for some reason I liked the idea of the screen staying black while the
            // application shuts down
            gui::Display::Release();

            // these two are needed almost everywhere so release them last
            misc::ConfigFile::Release();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_ReleaseSubsystems() threw std::exception \""
                << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_ReleaseSubsystems() "
                << "threw an unknown (non-std) exception.");
        }
    }

    void StartupShutdown::Teardown_Logger() const
    {
        try
        {
            misc::Log::Release();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_Logger() threw std::exception \"" << EXCEPTION.what()
                                                                             << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_Logger() threw an unknown (non-std) exception.");
        }
    }

    void StartupShutdown::Setup_ParseCommandLineArguments(const int ARGC, char * argv[]) const
    {
        if (ARGC >= 2)
        {
            GameController::SetStartupStage(argv[1]);

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

    void StartupShutdown::Setup_Display(
        const std::string & APPLICATION_NAME, const bool WILL_SETUP_FOR_TESTING) const
    {
        gui::Display::LogAllFullScreenVideoModes();
        gui::Display::LogAllSupportedFullScreenVideoModes();
        gui::Display::Acquire(APPLICATION_NAME, sf::Style::Fullscreen, 0, WILL_SETUP_FOR_TESTING);

        if (WILL_SETUP_FOR_TESTING)
        {
            M_HP_LOG("Since game is being setup for unit testing, the frame_rate_limit and "
                     "vertical_sync will both be disabled.");

            gui::Display::Instance()->SetFrameRateLimit(0);
            gui::Display::Instance()->SetVerticalSync(false);
        }
        else
        {
            gui::Display::Instance()->SetFrameRateLimit(
                static_cast<unsigned>(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                    "system-window-frame-rate-limit")));

            gui::Display::Instance()->SetVerticalSync(
                misc::ConfigFile::Instance()->ValueOrDefault<bool>("system-window-sync"));
        }
    }

    void StartupShutdown::Setup_FilesystemPaths() const
    {
        gui::ContentImage::SetupFilesystemPaths();

        map::Parser::SetupFilesystemPaths();

        popup::PopupManager::SetTexturesDirectoryPaths(
            misc::ConfigFile::Instance()->GetMediaPath("media-image-background-dir"),
            misc::ConfigFile::Instance()->GetMediaPath("media-image-accent-dir"));

        gui::SoundManager::SetSoundsDirectory(
            misc::ConfigFile::Instance()->GetMediaPath("media-sound-dir"),
            misc::ConfigFile::Instance()->GetMediaPath("media-music-dir"));

        gui::CreatureImagePaths::SetupFilesystemPaths();
        gui::ItemImagePaths::SetupFilesystemPaths();
    }

    void StartupShutdown::Setup_HoldersFill() const
    {
        creature::title::Holder::Fill();
        creature::condition::Holder::Fill();
        spell::Holder::Fill();
        song::Holder::Fill();
        combat::trap::Holder::Fill();
    }

    void StartupShutdown::Setup_SubsystemsAcquire() const
    {
        game::NpcWarehouse::Acquire();
        gui::TextureCache::Acquire();
        creature::EnchantmentWarehouse::Acquire();
        item::ItemWarehouse::Acquire();
        creature::CreatureWarehouse::Acquire();
        gui::SoundManager::Acquire();
        gui::FontManager::Acquire();
        popup::PopupManager::Acquire();
        Game::Acquire();
        game::GameStateFactory::Acquire();
        combat::Encounter::Acquire();
        item::ItemProfileWarehouse::Acquire();
        creature::nonplayer::ChanceFactory::Acquire();
        misc::SettingsFile::Acquire();
        GameController::Acquire();
    }

    void StartupShutdown::Setup_SubsystemsInitialize(const bool WILL_SETUP_FOR_TESTING) const
    {
        // SettingsFile::LoadAndRestore() must happen before initialization so that subsystems can
        // use the settings saved from the last run of the game.
        if (WILL_SETUP_FOR_TESTING)
        {
            M_HP_LOG(
                "Since game is being setup for unit testing, the settings file will be ignored.");
        }
        else
        {
            misc::SettingsFile::Instance()->LoadAndRestore();
        }

        gui::SoundManager::Instance()->Initialize();
        popup::PopupManager::Instance()->LoadAccentImagePaths();
        creature::nonplayer::ChanceFactory::Instance()->Initialize();

        if (misc::ConfigFile::Instance()->ValueAs<bool>(
                "system-items-will-create-items-at-startup", true))
        {
            item::ItemProfileWarehouse::Instance()->Initialize();
        }
    }

} // namespace game
} // namespace heroespath
