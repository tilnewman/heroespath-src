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
#include "config/configbase.hpp"
#include "config/settings-file.hpp"
#include "creature/condition-holder.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/enchantment-warehouse.hpp"
#include "creature/title-holder.hpp"
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "item/armor-details.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "item/weapon-details.hpp"
#include "log/logger.hpp"
#include "log/macros.hpp"
#include "misc/random.hpp"
#include "non-player/chance-factory.hpp"
#include "non-player/inventory-factory.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/combat-image-loader.hpp"
#include "sfml-util/gui/condition-image-loader.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/song-image-loader.hpp"
#include "sfml-util/gui/spell-image-loader.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/texture-cache.hpp"
#include "song/song-holder.hpp"
#include "spell/spell-holder.hpp"
#include "state/game-state-factory.hpp"
#include "state/npc-warehouse.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

namespace heroespath
{
namespace game
{

    StartupShutdown::StartupShutdown()
        : platform_()
    {}

    bool StartupShutdown::Setup(
        const std::string & APPLICATION_NAME, const int ARGC, char * argv[]) const
    {
        try
        {
            // this order is critical

            // initialize the log first so that all Setup() actions can be logged
            log::Logger::Acquire();

            platform_.Log();
            if (platform_.IsSupported() == false)
            {
                throw std::runtime_error(
                    "This system/platform is not supported.  See log for details.");
            }

            srand(static_cast<unsigned>(time(nullptr)));
            misc::random::MersenneTwister::Seed();

            Setup_ParseCommandLineArguments(ARGC, argv);

            game::GameDataFile::Acquire();
            game::GameDataFile::Instance()->Initialize();

            item::armor::ArmorDetailLoader::LoadFromGameDataFile();
            item::weapon::WeaponDetailLoader::LoadFromGameDataFile();

            sfml_util::gui::GuiElements::LoadTexture();
            Setup_Display(APPLICATION_NAME);
            Setup_ManagerClassResourcePaths();
            Setup_SubsystemsAcquire();
            Setup_SubsystemsInitialize();
            Setup_HoldersFill();

            // this causes the initial stage transition/creation so it must occur last
            LoopManager::Instance()->Initialize();

            return true;
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Setup() threw std::exception \"" << E.what() << "\"");
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Setup() threw unknown (non-std) exception.");
        }

        return false;
    }

    int StartupShutdown::Run() const
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
                "StartupShutdown::Run() threw std::exception \"" << E.what() << "\"");
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Run() threw an unknown (non-std) exception.");
        }

        return EXIT_FAILURE;
    }

    int StartupShutdown::Teardown() const
    {
        int exitCode{ EXIT_SUCCESS };

        Teardown_SettingsFile(exitCode);
        Teardown_CloseDisplay(exitCode);
        Teardown_ReleaseSubsystems(exitCode);
        Teardown_EmptyHolders(exitCode);
        Teardown_Logger(exitCode);

        return exitCode;
    }

    void StartupShutdown::Teardown_SettingsFile(int & exitCode_OutParam) const
    {
        try
        {
            config::SettingsFile::Instance()->AcquireAndSave();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_SettingsFile() threw std::exception \"" << E.what()
                                                                                   << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_SettingsFile() threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_CloseDisplay(int & exitCode_OutParam) const
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

    void StartupShutdown::Teardown_EmptyHolders(int & exitCode_OutParam) const
    {
        try
        {
            combat::trap::Holder::Empty();
            song::Holder::Empty();
            spell::Holder::Empty();
            creature::condition::Holder::Empty();
            creature::title::Holder::Empty();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_EmptyHolders() threw std::exception \"" << E.what()
                                                                                   << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_EmptyHolders() "
                    << "threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_ReleaseSubsystems(int & exitCode_OutParam) const
    {
        try
        {
            // this order is critical - be careful here...

            // release the SettingsFile first so that it saves the settings file even if a problem
            // happens during shutdown
            config::SettingsFile::Release();

            // factories should not be needed during shutdown, so release them early
            state::GameStateFactory::Release();
            non_player::ownership::ChanceFactory::Release();

            // release LoopManager early because it frees all the stages and their resources
            LoopManager::Release();

            // these must occur after all the stages have been released
            sfml_util::FontManager::Release(); // after NameInfo::Release()
            popup::PopupManager::Release();
            sfml_util::SoundManager::Release();
            sfml_util::TextureCache::Release();

            // this will cause all party creatures/items/enchantments/etc to be Warehouses::Free()'d
            Game::Release();

            // must occur after Game::Release() because state::GameState needs it
            state::NpcWarehouse::Release();

            // if this is an abnormal shutdown then the Encounter object will have some
            // Warehouse::Free() calls to make, so this must occur after Game::Release() but before
            // creature/item/enchantment/etc warehouses are released.
            combat::Encounter::Release();

            // there are dependancies between these Warehouses so this release order is critical
            creature::CreatureWarehouse::Release();
            item::ItemProfileWarehouse::Release();
            item::ItemWarehouse::Release();
            creature::EnchantmentWarehouse::Release();

            // the only constraint here is that it must occur after all the stages have been
            // released, so there is no reason for this to be one of the last things to be released,
            // but for some reason I liked the idea of the screen staying black while the
            // application shuts down
            sfml_util::Display::Release();

            // these two are needed almost everywhere so release them last
            GameDataFile::Release();
        }
        catch (const std::exception & E)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_ReleaseSubsystems() threw std::exception \"" << E.what()
                                                                                        << "\"");

            exitCode_OutParam = EXIT_FAILURE;
        }
        catch (...)
        {
            M_LOG_FAT(
                *log::Logger::Instance(),
                "StartupShutdown::Teardown_ReleaseSubsystems() "
                    << "threw an unknown (non-std) exception.");

            exitCode_OutParam = EXIT_FAILURE;
        }
    }

    void StartupShutdown::Teardown_Logger(int & exitCode_OutParam) const
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

    void StartupShutdown::Setup_ParseCommandLineArguments(const int ARGC, char * argv[]) const
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

    void StartupShutdown::Setup_Display(const std::string & APPLICATION_NAME) const
    {
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::Display::Acquire(APPLICATION_NAME, sf::Style::Fullscreen, 0);

        sfml_util::Display::Instance()->SetFrameRateLimit(static_cast<unsigned>(
            game::GameDataFile::Instance()->GetCopyInt("system-window-frame-rate-limit")));

        sfml_util::Display::Instance()->SetVerticalSync(
            game::GameDataFile::Instance()->GetCopyBool("system-window-sync"));
    }

    void StartupShutdown::Setup_ManagerClassResourcePaths() const
    {
        popup::PopupManager::SetTexturesDirectoryPaths(
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-popup-dir"),
            game::GameDataFile::Instance()->GetMediaPath("media-images-accents-dir"));

        sfml_util::SoundManager::SetSoundsDirectory(
            game::GameDataFile::Instance()->GetMediaPath("media-sounds-dir"),
            game::GameDataFile::Instance()->GetMediaPath("media-music-dir"));
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
        state::NpcWarehouse::Acquire();
        sfml_util::TextureCache::Acquire();
        creature::EnchantmentWarehouse::Acquire();
        item::ItemWarehouse::Acquire();
        creature::CreatureWarehouse::Acquire();
        sfml_util::SoundManager::Acquire();
        sfml_util::FontManager::Acquire();
        popup::PopupManager::Acquire();
        Game::Acquire();
        state::GameStateFactory::Acquire();
        combat::Encounter::Acquire();
        item::ItemProfileWarehouse::Acquire();
        non_player::ownership::ChanceFactory::Acquire();
        config::SettingsFile::Acquire();
        LoopManager::Acquire();
    }

    void StartupShutdown::Setup_SubsystemsInitialize() const
    {
        // SettingsFile::LoadAndRestore() must happen before initialization so that subsystems can
        // use the settings saved from the last run of the game.
        config::SettingsFile::Instance()->LoadAndRestore();

        sfml_util::SoundManager::Instance()->Initialize();
        popup::PopupManager::Instance()->LoadAccentImagePaths();
        non_player::ownership::ChanceFactory::Instance()->Initialize();
        item::ItemProfileWarehouse::Instance()->Initialize();
    }

} // namespace game
} // namespace heroespath
