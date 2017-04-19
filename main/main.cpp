//
// main.cpp
//  Test SFML rendering to develop some gui classes.
//
#include "configbase/configbase.hpp"
#include "common/logmacros.hpp"

#include "sfml-util/platform.hpp"
#include "sfml-util/loop.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/random.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "heroespath/combat/strategy-details.hpp"
#include "heroespath/loop-manager.hpp"
#include "heroespath/logger.hpp"
#include "heroespath/game-data-file.hpp"
#include "heroespath/settings-file.hpp"
#include "heroespath/game.hpp"
#include "heroespath/creature/titles.hpp"


int main()
{
    std::cout << "SFML GUI Test Starting..." << std::endl;

    heroespath::LoggerSPtr_t logSPtr(heroespath::Logger::Instance());

    try
    {
        sfml_util::Platform::Instance()->DetectAndLog();
        if (sfml_util::Platform::Instance()->IsSupported() == false)
            throw std::runtime_error("This system (platform) is not supported.");

        //seed the random number generator
        sfml_util::rand::MersenneTwister::Seed();

        //keep an instance of various singleton classes here to prevent thrashing
        sfml_util::DisplaySPtr_t displaySPtr(sfml_util::Display::Instance());
        //
        M_LOG(*logSPtr, "Loading the Game Data file...");
        heroespath::GameDataFileSPtr_t gameDataFileSPtr(heroespath::GameDataFile::Instance());

        //set which stage will startup
        heroespath::LoopManager::SetStartupStage( gameDataFileSPtr->GetCopy<std::string>("system-startup-stage") );

        //setup the graphics display
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::WinSPtr_t winSPtr( sfml_util::Display::OpenRenderWindow("sfml-gui-demo", sf::Style::Fullscreen, 0/*default to antialiasing disabled*/) );
        winSPtr->setFramerateLimit( gameDataFileSPtr->GetCopyInt("system-window-frame-rate-limit", 0) );
        winSPtr->setVerticalSyncEnabled( gameDataFileSPtr->GetCopyBool("system-window-sync", true) );

        //set resource paths for manager classes
        sfml_util::FontManager::SetFontsDirectory(                      gameDataFileSPtr->GetMediaPath("media-fonts-dir"));
        sfml_util::gui::PopupManager::SetTexturesDirectoryPaths(        gameDataFileSPtr->GetMediaPath("media-images-backgrounds-popup-dir"), gameDataFileSPtr->GetMediaPath("media-images-accents-dir"));
        sfml_util::SoundManager::SetSoundsDirectory(                    gameDataFileSPtr->GetMediaPath("media-sounds-dir"), gameDataFileSPtr->GetMediaPath("media-music-dir"));
        sfml_util::gui::ItemImageManager::SetItemImageDirectory(        gameDataFileSPtr->GetMediaPath("media-images-items-dir"));
        sfml_util::gui::CreatureImageManager::SetCreatureImageDirectory(gameDataFileSPtr->GetMediaPath("media-images-creatures-dir") );
        sfml_util::gui::TitleImageManager::SetTitleImageDirectory(      gameDataFileSPtr->GetMediaPath("media-images-titles-dir") );
        sfml_util::gui::SpellImageManager::SetImagesDirectory(          gameDataFileSPtr->GetMediaPath("media-images-spells-dir") );
        sfml_util::gui::ConditionImageManager::SetImagesDirectory(      gameDataFileSPtr->GetMediaPath("media-images-conditions-dir") );
        sfml_util::gui::CombatImageManager::SetImagesDirectory(         gameDataFileSPtr->GetMediaPath("media-images-combat-dir") );

        //initialize manager classes
        heroespath::creature::title::TitleWarehouse::Setup();

        //keep an instance of various singleton classes here to prevent thrashing
        //TODO reduce game asset loading here and move below, see "load game assets"
        sfml_util::FontManagerSPtr_t                fontManagerSPtr          ( sfml_util::FontManager::Instance() );
        sfml_util::gui::PopupManagerSPtr_t          popupManagerSPtr         ( sfml_util::gui::PopupManager::Instance() );
        sfml_util::SoundManagerSPtr_t               soundManagerSPtr         ( sfml_util::SoundManager::Instance() );
        sfml_util::gui::GuiElementsSPtr_t           guiElementsSPtr          ( sfml_util::gui::GuiElements::Instance());
        sfml_util::gui::ItemImageManagerSPtr_t      itemImageManagerSPtr     ( sfml_util::gui::ItemImageManager::Instance() );
        sfml_util::gui::CreatureImageManagerSPtr_t  creatureImageManagerSPtr ( sfml_util::gui::CreatureImageManager::Instance() );
        sfml_util::gui::TitleImageManagerSPtr_t     titleImageManagerSPtr    ( sfml_util::gui::TitleImageManager::Instance() );
        sfml_util::gui::SpellImageManagerSPtr_t     spellImageManagerSPtr    ( sfml_util::gui::SpellImageManager::Instance() );
        sfml_util::gui::ConditionImageManagerSPtr_t conditionImageManagerSPtr( sfml_util::gui::ConditionImageManager::Instance() );
        sfml_util::gui::CombatImageManagerSPtr_t    combatImageManagerSPtr   ( sfml_util::gui::CombatImageManager::Instance() );
        heroespath::GameSPtr_t                      gameSPtr                 ( heroespath::Game::Instance() );

        try
        {
            //create/load/store the game settings file
            heroespath::SettingsFileSPtr_t settingsFileSPtr( heroespath::SettingsFile::Instance() );
            settingsFileSPtr->LoadAndRestore();

            //load game assets
            soundManagerSPtr->LoadStaticSoundSets(); //important to call this after settingsFileSPtr->LoadAndRestore() so the sound effects have the correct volume
            heroespath::combat::strategy::ChanceFactory::Instance()->Initialize();
            popupManagerSPtr->LoadAssets();

            //create the game state manager and run the game
            heroespath::LoopManagerSPtr_t gameStateSPtr(heroespath::LoopManager::Instance());
            gameStateSPtr->Execute();

            //save settings
            settingsFileSPtr->AcquireAndSave();
            settingsFileSPtr.reset();
        }
        catch (const std::exception & E)
        {
            std::cout << "heroespath::LoopManager::Execute() threw std::exception(\"" << E.what() << "\")" << std::endl;
            M_LOG(*logSPtr, "heroespath::LoopManager::Execute() threw std::exception(\"" << E.what() << "\")");
        }
        catch (...)
        {
            std::cout << "heroespath::LoopManager::Execute() threw an unknown non-std exception." << std::endl;
            M_LOG(*logSPtr, "heroespath::LoopManager::Execute() threw an unknown non-std exception.");
        }

        //ensure the window is closed before exiting
        if (winSPtr->isOpen())
            winSPtr->close();

        M_LOG( * logSPtr, "Reached the end of main within the try-catch.");
    }
    catch (const std::exception & E)
    {
        M_LOG( * logSPtr, "App threw std::exception \"" << E.what() << "\"");
        return EXIT_FAILURE;
    }
    catch (...)
    {
        M_LOG( * logSPtr, "App threw an unknown non-std exception." << std::endl);
        return EXIT_FAILURE;
    }

    std::cout << "SFML GUI Test Stopped." << std::endl;
    return EXIT_SUCCESS;
}
