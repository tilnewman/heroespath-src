// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// setup-teardown.cpp
//
#include "setup-teardown.hpp"

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
    SetupTeardown::SetupTeardown(
        const std::string & TITLE,
        const SubsystemCollection SUBSYSTEMS,
        const std::vector<std::string> & ARGS)
    {
        m_subsystems = SUBSYSTEMS;

        m_name = "\"" + TITLE + "\" in configuration \"" + std::string(NAMEOF_ENUM(m_subsystems))
            + "\"";

        Setup(TITLE, ARGS);
    }

    SetupTeardown::~SetupTeardown() { Teardown(); }

    void SetupTeardown::Play() const { game::GameController::Instance()->PlayGame(); }

    void
        SetupTeardown::Setup(const std::string & TITLE, const std::vector<std::string> & ARGS) const
    {
        // the order of every line in this function is critical

        Setup_Log();

        M_HP_LOG(m_name << " Setup is starting...");

        Setup_PlatformDetection();
        Setup_RandomNumberGenerator();
        Setup_ConfigFile();
        Setup_FilesystemPaths();

        if (m_subsystems != SubsystemCollection::TestWithOnlyLogAndConfig)
        {
            Setup_ParseCommandLineArguments(ARGS);
            Setup_LoadItemDetails();
            Setup_Display(TITLE);
            Setup_SubsystemsAcquire();

            // SettingsFile::LoadAndRestore() must happen after Subsystem Acquires but before
            // Subsystem Initialization so that subsystems can use the settings saved from the last
            // run of the game during their initialization.
            if (m_subsystems == SubsystemCollection::All)
            {
                misc::SettingsFile::Instance()->LoadAndRestore();
            }
            else
            {
                M_HP_LOG("Since this is a unit test run, the settings file will be ignored.");
            }

            Setup_SubsystemsInitialize();
            Setup_HoldersFill();

            if (m_subsystems == SubsystemCollection::All)
            {
                // this causes the initial stage transition/creation so it must occur last
                GameController::Instance()->Initialize();
            }
            else
            {
                M_HP_LOG("Since this is a unit test run, a fake defaut party of characters will be "
                         "created that the test code can work with.");
                Game::Instance()->MakeNewForTestingAndSet();
            }
        }

        M_HP_LOG(m_name << " Setup has finished.");
    }

    void SetupTeardown::Setup_Log() const
    {
        misc::stringStreamHolder::init();
        misc::Log::Acquire();
    }

    void SetupTeardown::Setup_PlatformDetection() const
    {
        M_HP_LOG("Detected Platform \"" << misc::platform_name << "\"");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::platform_is_supported,
            std::string("This enviornment/operating_system/platform/whatever, called \"")
                .append(misc::platform_name)
                .append("\" is not supported."));
    }

    void SetupTeardown::Setup_RandomNumberGenerator() const
    {
        misc::helpers::MersenneTwister19937::Setup();
    }

    void SetupTeardown::Setup_ConfigFile() const
    {
        misc::ConfigFile::Acquire();
        misc::ConfigFile::Instance()->Initialize();
    }

    void SetupTeardown::Setup_ParseCommandLineArguments(const std::vector<std::string> & ARGS) const
    {
        for (std::size_t i(1); i < ARGS.size(); ++i)
        {
            const auto ARG = ARGS[i];

            if (1 == i)
            {
                GameController::SetStartupStage(ARG);
            }
            else
            {
                const std::string WARNING_STR { "Ignoring extra command line argument: \"" + ARG
                                                + "\"" };

                std::cout << WARNING_STR << std::endl;
                M_HP_LOG_WRN(WARNING_STR);
            }
        }
    }

    void SetupTeardown::Setup_LoadItemDetails() const
    {
        item::armor::ArmorDetailLoader::LoadFromGameDataFile();
        item::weapon::WeaponDetailLoader::LoadFromGameDataFile();
    }

    void SetupTeardown::Setup_Display(const std::string & TITLE) const
    {
        gui::Display::LogAllFullScreenVideoModes();
        gui::Display::LogAllSupportedFullScreenVideoModes();

        if ((m_subsystems == SubsystemCollection::All)
            || (m_subsystems == SubsystemCollection::TestAll))
        {
            gui::Display::Acquire(TITLE, sf::Style::Fullscreen, 0);
        }

        if (m_subsystems == SubsystemCollection::All)
        {
            gui::Display::Instance()->SetFrameRateLimit(
                static_cast<unsigned>(misc::ConfigFile::Instance()->ValueOrDefault<int>(
                    "system-window-frame-rate-limit")));

            gui::Display::Instance()->SetVerticalSync(
                misc::ConfigFile::Instance()->ValueOrDefault<bool>("system-window-sync"));
        }
        else
        {
            M_HP_LOG("Since this is a unit test run, the frame_rate_limit and "
                     "vertical_sync will both be disabled.");

            gui::Display::Instance()->SetFrameRateLimit(0);
            gui::Display::Instance()->SetVerticalSync(false);
        }
    }

    void SetupTeardown::Setup_FilesystemPaths() const
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

    void SetupTeardown::Setup_SubsystemsAcquire() const
    {
        // the order of every line in this function is critical

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

    void SetupTeardown::Setup_SubsystemsInitialize() const
    {
        // the order of every line in this function is critical

        gui::SoundManager::Instance()->Initialize();
        popup::PopupManager::Instance()->LoadAccentImagePaths();
        creature::nonplayer::ChanceFactory::Instance()->Initialize();

        if (misc::ConfigFile::Instance()->ValueAs<bool>(
                "system-items-will-create-items-at-startup", true))
        {
            item::ItemProfileWarehouse::Instance()->Initialize();
        }
    }

    void SetupTeardown::Setup_HoldersFill() const
    {
        creature::title::Holder::Fill();
        creature::condition::Holder::Fill();
        spell::Holder::Fill();
        song::Holder::Fill();
        combat::trap::Holder::Fill();
    }

    void SetupTeardown::Teardown() const
    {
        // the order of every line in this function is critical

        M_HP_LOG(m_name << " Teardown starting...");

        Teardown_SettingsFile();
        Teardown_CloseDisplay();
        Teardown_EmptyHolders();
        Teardown_ReleaseSubsystems();

        M_HP_LOG(
            m_name
            << " Teardown almost finished.  Only the Log Subsystem is left to teardown, which "
               "will end this logfile.");

        Teardown_Logger();
    }

    void SetupTeardown::Teardown_SettingsFile() const
    {
        if (m_subsystems != SubsystemCollection::All)
        {
            return;
        }

        try
        {
            misc::SettingsFile::Instance()->AcquireAndSave();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(m_name << " threw std::exception \"" << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(m_name << " threw an unknown (non-std) exception.");
        }
    }

    void SetupTeardown::Teardown_CloseDisplay() const
    {
        if ((m_subsystems != SubsystemCollection::All)
            && (m_subsystems != SubsystemCollection::TestAll))
        {
            return;
        }

        try
        {
            gui::Display::Instance()->Close();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(m_name << " threw std::exception \"" << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(m_name << " threw an unknown (non-std) exception.");
        }
    }

    void SetupTeardown::Teardown_EmptyHolders() const
    {
        if (m_subsystems == SubsystemCollection::TestWithOnlyLogAndConfig)
        {
            return;
        }

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
            M_HP_LOG_FAT(m_name << " threw std::exception \"" << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(m_name << " threw an unknown (non-std) exception.");
        }
    }

    void SetupTeardown::Teardown_ReleaseSubsystems() const
    {
        // no need to check m_subsystems in this function because calling Release() on a subsystm
        // singleton is always safe.

        try
        {
            // the order of every line in this try-block is critical

            // release the SettingsFile first so that it saves the settings file even if a
            // problem happens during shutdown
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

            // this will cause all party creatures/items/enchantments/etc to be
            // Warehouses::Free()'d
            Game::Release();

            // must occur after Game::Release() because game::GameState needs it
            game::NpcWarehouse::Release();

            // if this is an abnormal shutdown then the Encounter object will have some
            // Warehouse::Free() calls to make, so this must occur after Game::Release() but
            // before creature/item/enchantment/etc warehouses are released.
            combat::Encounter::Release();

            // there are dependencies between these Warehouses so this release order is critical
            creature::CreatureWarehouse::Release();
            item::ItemProfileWarehouse::Release();
            item::ItemWarehouse::Release();
            creature::EnchantmentWarehouse::Release();

            // the only constraint here is that it must occur after all the stages have been
            // released, so there is no reason for this to be one of the last things to be
            // released, but for some reason I liked the idea of the screen staying black while
            // the application shuts down
            gui::Display::Release();

            // these two are needed almost everywhere so release them last
            misc::ConfigFile::Release();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(m_name << " threw std::exception \"" << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(m_name << " threw an unknown (non-std) exception.");
        }
    }

    void SetupTeardown::Teardown_Logger() const
    {
        try
        {
            misc::Log::Release();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(m_name << " threw std::exception \"" << EXCEPTION.what() << "\"");
        }
        catch (...)
        {
            M_HP_LOG_FAT(m_name << " threw an unknown (non-std) exception.");
        }
    }

} // namespace game
} // namespace heroespath
