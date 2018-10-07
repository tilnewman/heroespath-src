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
#include "game/game-state-factory.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "game/npc-warehouse.hpp"
#include "item/armor-details.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-profile-warehouse.hpp"
#include "item/item-type-enum.hpp"
#include "item/item-warehouse.hpp"
#include "item/weapon-details.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/log.hpp"
#include "misc/platform.hpp"
#include "misc/random.hpp"
#include "misc/settings-file.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/combat-image-loader.hpp"
#include "sfml-util/condition-image-loader.hpp"
#include "sfml-util/creature-image-loader.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/song-image-loader.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/spell-image-loader.hpp"
#include "sfml-util/texture-cache.hpp"
#include "sfml-util/title-image-loader.hpp"
#include "song/song-holder.hpp"
#include "spell/spell-holder.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

namespace heroespath
{
namespace game
{

    StartupShutdown::StartupShutdown(
        const std::string & APPLICATION_NAME, const int ARGC, char * argv[])
        : platform_()
    {
        Setup(APPLICATION_NAME, ARGC, argv);
    }

    void StartupShutdown::Setup(
        const std::string & APPLICATION_NAME, const int ARGC, char * argv[]) const
    {
        // this order is critical

        // initialize the log first so that all Setup() actions can be logged
        misc::Log::Acquire();

        platform_.Log();
        if (platform_.IsSupported() == false)
        {
            throw std::runtime_error(
                "This system/platform is not supported.  See log for details.");
        }

        srand(static_cast<unsigned>(time(nullptr)));
        misc::random::MersenneTwister::Seed();

        Setup_ParseCommandLineArguments(ARGC, argv);

        misc::ConfigFile::Acquire();
        misc::ConfigFile::Instance()->Initialize();

        item::armor::ArmorDetailLoader::LoadFromGameDataFile();
        item::weapon::WeaponDetailLoader::LoadFromGameDataFile();

        Setup_Display(APPLICATION_NAME);
        Setup_ManagerClassResourcePaths();
        Setup_SubsystemsAcquire();
        Setup_SubsystemsInitialize();
        Setup_HoldersFill();

        // this causes the initial stage transition/creation so it must occur last
        LoopManager::Instance()->Initialize();
    }

    StartupShutdown::~StartupShutdown() { Teardown(); }

    void StartupShutdown::Run() const { game::LoopManager::Instance()->Execute(); }

    void StartupShutdown::Teardown() const
    {
        Teardown_SettingsFile();
        Teardown_CloseDisplay();
        Teardown_ReleaseSubsystems();
        Teardown_EmptyHolders();
        Teardown_Logger();
    }

    void StartupShutdown::Teardown_SettingsFile() const
    {
        try
        {
            misc::SettingsFile::Instance()->AcquireAndSave();
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_SettingsFile() threw std::exception \"" << E.what()
                                                                                   << "\"");
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
            if (sfml_util::Display::Instance()->IsOpen())
            {
                sfml_util::Display::Instance()->Close();
            }
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_CloseDisplay() threw std::exception \"" << E.what()
                                                                                   << "\"");
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
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_EmptyHolders() threw std::exception \"" << E.what()
                                                                                   << "\"");
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

            // release LoopManager early because it frees all the stages and their resources
            LoopManager::Release();

            // these must occur after all the stages have been released
            sfml_util::FontManager::Release(); // after NameInfo::Release()
            popup::PopupManager::Release();
            sfml_util::SoundManager::Release();
            sfml_util::TextureCache::Release();

            // this will cause all party creatures/items/enchantments/etc to be Warehouses::Free()'d
            Game::Release();

            // must occur after Game::Release() because game::GameState needs it
            game::NpcWarehouse::Release();

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
            misc::ConfigFile::Release();
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_ReleaseSubsystems() threw std::exception \"" << E.what()
                                                                                        << "\"");
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
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT(
                "StartupShutdown::Teardown_Logger() threw std::exception \"" << E.what() << "\"");
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
            misc::ConfigFile::Instance()->ValueOrDefault<int>("system-window-frame-rate-limit")));

        sfml_util::Display::Instance()->SetVerticalSync(
            misc::ConfigFile::Instance()->ValueOrDefault<bool>("system-window-sync"));
    }

    void StartupShutdown::Setup_ManagerClassResourcePaths() const
    {
        popup::PopupManager::SetTexturesDirectoryPaths(
            misc::ConfigFile::Instance()->GetMediaPath("media-images-backgrounds-popup-dir"),
            misc::ConfigFile::Instance()->GetMediaPath("media-images-accents-dir"));

        sfml_util::SoundManager::SetSoundsDirectory(
            misc::ConfigFile::Instance()->GetMediaPath("media-sounds-dir"),
            misc::ConfigFile::Instance()->GetMediaPath("media-music-dir"));
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
        sfml_util::TextureCache::Acquire();
        creature::EnchantmentWarehouse::Acquire();
        item::ItemWarehouse::Acquire();
        creature::CreatureWarehouse::Acquire();
        sfml_util::SoundManager::Acquire();
        sfml_util::FontManager::Acquire();
        popup::PopupManager::Acquire();
        Game::Acquire();
        game::GameStateFactory::Acquire();
        combat::Encounter::Acquire();
        item::ItemProfileWarehouse::Acquire();
        creature::nonplayer::ChanceFactory::Acquire();
        misc::SettingsFile::Acquire();
        LoopManager::Acquire();
    }

    void StartupShutdown::Setup_SubsystemsInitialize() const
    {
        // SettingsFile::LoadAndRestore() must happen before initialization so that subsystems can
        // use the settings saved from the last run of the game.
        misc::SettingsFile::Instance()->LoadAndRestore();

        sfml_util::SoundManager::Instance()->Initialize();
        popup::PopupManager::Instance()->LoadAccentImagePaths();
        creature::nonplayer::ChanceFactory::Instance()->Initialize();
        item::ItemProfileWarehouse::Instance()->Initialize();
    }

} // namespace game
} // namespace heroespath
