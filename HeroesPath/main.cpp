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
        std::cout << "\t1" << std::endl;
        utilz::Platform::Instance()->DetectAndLog();
        if (utilz::Platform::Instance()->IsSupported() == false)
        {
            throw std::runtime_error("This system (platform) is not supported.");
        }
        std::cout << "\t2" << std::endl;
        //seed the random number generator
        utilz::random::MersenneTwister::Seed();
        std::cout << "\t3" << std::endl;
        //keep an instance of various singleton classes here to prevent thrashing
        sfml_util::DisplaySPtr_t displaySPtr(sfml_util::Display::Instance());
        std::cout << "\t4" << std::endl;
        //
        M_LOG(*logSPtr, "Loading the Game Data file...");
        game::GameDataFile::Acquire();
        auto dataFilePtr(game::GameDataFile::Instance());
        std::cout << "\t5" << std::endl;
        //set which stage will startup
        game::LoopManager::SetStartupStage( dataFilePtr->GetCopy<std::string>("system-startup-stage") );
        std::cout << "\t6" << std::endl;
        //setup the graphics display
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::WinSPtr_t winSPtr{ sfml_util::Display::OpenRenderWindow("Heroes' Path", sf::Style::Fullscreen, 0/*default to antialiasing disabled*/) };
        winSPtr->setFramerateLimit(static_cast<unsigned int>(dataFilePtr->GetCopyInt("system-window-frame-rate-limit", 0)) );
        winSPtr->setVerticalSyncEnabled( dataFilePtr->GetCopyBool("system-window-sync", true) );
        std::cout << "\t7" << std::endl;
        //set resource paths for manager classes
        sfml_util::FontManager::SetFontsDirectory(                      dataFilePtr->GetMediaPath("media-fonts-dir"));
        sfml_util::gui::PopupManager::SetTexturesDirectoryPaths(        dataFilePtr->GetMediaPath("media-images-backgrounds-popup-dir"), dataFilePtr->GetMediaPath("media-images-accents-dir"));
        sfml_util::SoundManager::SetSoundsDirectory(                    dataFilePtr->GetMediaPath("media-sounds-dir"), dataFilePtr->GetMediaPath("media-music-dir"));
        sfml_util::gui::ItemImageManager::SetItemImageDirectory(        dataFilePtr->GetMediaPath("media-images-items-dir"));
        sfml_util::gui::CreatureImageManager::SetCreatureImageDirectory(dataFilePtr->GetMediaPath("media-images-creatures-dir") );
        sfml_util::gui::TitleImageManager::SetTitleImageDirectory(      dataFilePtr->GetMediaPath("media-images-titles-dir") );
        sfml_util::gui::SpellImageManager::SetImagesDirectory(          dataFilePtr->GetMediaPath("media-images-spells-dir") );
        sfml_util::gui::ConditionImageManager::SetImagesDirectory(      dataFilePtr->GetMediaPath("media-images-conditions-dir") );
        sfml_util::gui::CombatImageManager::SetImagesDirectory(         dataFilePtr->GetMediaPath("media-images-combat-dir") );
        std::cout << "\t8" << std::endl;

        //load game assets Stage 1
        game::creature::title::Warehouse::Setup();
        game::creature::condition::Warehouse::Setup();
        game::spell::Warehouse::Setup();

        //Note:  FontManager is actually acquired at static init time,
        //       but for completeness the Acquire() call is also here.
        sfml_util::FontManager::Acquire();
        sfml_util::gui::PopupManager::Acquire();
        sfml_util::SoundManager::Acquire();
        auto guiElementsSPtr          { sfml_util::gui::GuiElements::Instance() };
        auto itemImageManagerSPtr     { sfml_util::gui::ItemImageManager::Instance() };
        auto creatureImageManagerSPtr { sfml_util::gui::CreatureImageManager::Instance() };
        auto titleImageManagerSPtr    { sfml_util::gui::TitleImageManager::Instance() };
        auto spellImageManagerSPtr    { sfml_util::gui::SpellImageManager::Instance() };
        auto conditionImageManagerSPtr{ sfml_util::gui::ConditionImageManager::Instance() };
        auto combatImageManagerSPtr   { sfml_util::gui::CombatImageManager::Instance() };
        game::Game::Acquire();

        try
        {
            //create/load/store the game settings file
            auto settingsFileSPtr( game::SettingsFile::Instance() );
            settingsFileSPtr->LoadAndRestore();

            //load game assets Stage 3
            //NOTE:  SoundManager::Instance()->LoadSoundSets() must occur after
            //       SettingsFile::LoadAndRestore() so that the sound effects
            //       created here have the correct volume loaded from the
            //       settings file.
            sfml_util::SoundManager::Instance()->LoadSoundSets();
            game::combat::strategy::ChanceFactory::Instance()->Initialize();
            sfml_util::gui::PopupManager::Instance()->LoadAssets();

            //create the game state manager and run the game
            auto gameStateSPtr(game::LoopManager::Instance());
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
        game::Game::Release();
        sfml_util::SoundManager::Release();
        sfml_util::gui::PopupManager::Release();
        game::GameDataFile::Release();
        sfml_util::FontManager::Release();

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
