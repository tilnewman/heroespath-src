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
            throw std::runtime_error("This system (platform) is not supported.");

        //seed the random number generator
        utilz::random::MersenneTwister::Seed();

        //keep an instance of various singleton classes here to prevent thrashing
        sfml_util::DisplaySPtr_t displaySPtr(sfml_util::Display::Instance());
        //
        M_LOG(*logSPtr, "Loading the Game Data file...");
        game::GameDataFileSPtr_t gameDataFileSPtr(game::GameDataFile::Instance());

        //set which stage will startup
        game::LoopManager::SetStartupStage( gameDataFileSPtr->GetCopy<std::string>("system-startup-stage") );

        //setup the graphics display
        sfml_util::Display::LogAllFullScreenVideoModes();
        sfml_util::Display::LogAllSupportedFullScreenVideoModes();
        sfml_util::WinSPtr_t winSPtr{ sfml_util::Display::OpenRenderWindow("Heroes' Path", sf::Style::Fullscreen, 0/*default to antialiasing disabled*/) };
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

        //setup manager classes
        game::creature::title::Warehouse::Setup();
        game::creature::condition::Warehouse::Setup();
        game::spell::Warehouse::Setup();

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
        game::GameSPtr_t                            gameSPtr                 ( game::Game::Instance() );

        try
        {
            //create/load/store the game settings file
            game::SettingsFileSPtr_t settingsFileSPtr( game::SettingsFile::Instance() );
            settingsFileSPtr->LoadAndRestore();

            //load game assets
            soundManagerSPtr->LoadStaticSoundSets(); //important to call this after settingsFileSPtr->LoadAndRestore() so the sound effects have the correct volume
            game::combat::strategy::ChanceFactory::Instance()->Initialize();
            popupManagerSPtr->LoadAssets();

            //create the game state manager and run the game
            game::LoopManagerSPtr_t gameStateSPtr(game::LoopManager::Instance());
            gameStateSPtr->Execute();

            //save settings
            settingsFileSPtr->AcquireAndSave();
            settingsFileSPtr.reset();
        }
        catch (const std::exception & E)
        {
            std::cout << "game::LoopManager::Execute() threw std::exception(\"" << E.what() << "\")" << std::endl;
            M_LOG(*logSPtr, "game::LoopManager::Execute() threw std::exception(\"" << E.what() << "\")");
        }
        catch (...)
        {
            std::cout << "game::LoopManager::Execute() threw an unknown non-std exception." << std::endl;
            M_LOG(*logSPtr, "game::LoopManager::Execute() threw an unknown non-std exception.");
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

    std::cout << "...Stopping Heroes Path." << std::endl;
    return EXIT_SUCCESS;
}
