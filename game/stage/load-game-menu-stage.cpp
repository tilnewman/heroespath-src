//
// load-game-menu-stage.cpp
//
#include "load-game-menu-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "utilz/real.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/ouroboros.hpp"
#include "game/game-data-file.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/loop-manager.hpp"
#include "game/location/location.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"


namespace game
{
namespace stage
{

    LoadGameStage::LoadGameStage()
    :
        Stage                   ("LoadGameMenu"),
        SCREEN_WIDTH_           (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_          (sfml_util::Display::Instance()->GetWinHeight()),
        mainMenuTitle_          ("resume_button_normal.png"),
        backgroundImage_        ("media-images-backgrounds-tile-darkknot"),
        backButtonSPtr_         (),
        gsListBoxSPtr_          (),
        locTextRegionSPtr_      (),
        charTextRegionSVec_     (),
        charLabelTextRegionSPtr_(),
        gsListBoxPosLeft_       (0.0f),
        gsListBoxPosTop_        (0.0f),
        gsListBoxPosWidth_      (0.0f),
        gsListBoxPosHeight_     (0.0f),
        ouroborosSPtr_          (),
        bottomSymbol_           ()
    {}


    LoadGameStage::~LoadGameStage()
    {}


    bool LoadGameStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == backButtonSPtr_.get())
        {
            LoopManager::Instance()->Goto_MainMenu();
            return true;
        }

        return false;
    }


    bool LoadGameStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &)
    {
        SetupGameInfoDisplay();
        return true;
    }


    void LoadGameStage::Setup()
    {
        //ouroboros
        ouroborosSPtr_.reset( new Ouroboros("LoadGameStage's") );
        EntityAdd(ouroborosSPtr_);

        //back button
        const std::string BUTTONS_PATH(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"));
        backButtonSPtr_.reset( new sfml_util::gui::FourStateButton("Back",
                                                                   200.0f,
                                                                   SCREEN_HEIGHT_ - 100.0f,
                                                                   std::string(BUTTONS_PATH).append("back_button_normal.png"),
                                                                   "",
                                                                   std::string(BUTTONS_PATH).append("back_button_lit.png")) );

        backButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonSPtr_);

        //GameState ListBox
        //
        //determine position on screen
        gsListBoxPosLeft_  = ((SCREEN_WIDTH_ * 0.1f) - 85.0f) + sfml_util::MapByRes(0.0f, 800.0f);
        gsListBoxPosWidth_ = 610.0f;
        gsListBoxPosHeight_= SCREEN_HEIGHT_ * 0.5f;
        gsListBoxPosTop_   = ((SCREEN_HEIGHT_ * 0.5f) - (gsListBoxPosHeight_ * 0.5f)) + 50.0f;
        sf::FloatRect GS_LB_RECT(gsListBoxPosLeft_, gsListBoxPosTop_, gsListBoxPosWidth_, gsListBoxPosHeight_);
        //
        //hand all GameState objects to the ListBox
        state::GameStateSSet_t gamestateSSet( state::GameStateFactory::Instance()->LoadAllGames() );
        std::list<sfml_util::gui::ListBoxItemSPtr_t> listBoxItemSLst;
        std::size_t gameStateCount(0);
        for (auto const & NEXT_GAMESTATE_SPTR : gamestateSSet)
        {
            std::ostringstream ss;
            ss  << "Started "
                /*TODO MAP NAME INSTEAD OF STARTED DATE*/
                << NEXT_GAMESTATE_SPTR->DateTimeStarted().date.year << "-"
                << NEXT_GAMESTATE_SPTR->DateTimeStarted().date.month << "-"
                << NEXT_GAMESTATE_SPTR->DateTimeStarted().date.day
                << ", Saved "
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().date.year << "-"
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().date.month << "-"
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().date.day << "  "
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().time.hours << ":"
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().time.minutes << ":"
                << NEXT_GAMESTATE_SPTR->DateTimeOfLastSave().time.seconds;

            const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                     sfml_util::FontManager::Instance()->Font_Default2(),
                                                     28,
                                                     sf::Color::White,
                                                     sfml_util::Justified::Left);
            ss.str("");
            ss << ++gameStateCount;
            const sfml_util::gui::ListBoxItemSPtr_t LBI_SPTR( new sfml_util::gui::ListBoxItem(ss.str(), TEXT_INFO, NEXT_GAMESTATE_SPTR) );
            listBoxItemSLst.push_back(LBI_SPTR);
        }
        listBoxItemSLst.sort();
        //
        //establish the boxing options
        const sf::Color BG_COLOR(sfml_util::FontManager::Instance()->Color_Orange() - sf::Color(100, 100, 100, 220));
        const sfml_util::gui::BackgroundInfo BG_INFO(BG_COLOR);
        const sfml_util::gui::box::Info BOX_INFO(1,
                                                 true,
                                                 GS_LB_RECT,
                                                 sfml_util::gui::ColorSet(sfml_util::FontManager::Instance()->Color_Orange(),
                                                                          BG_COLOR,
                                                                          sfml_util::FontManager::Instance()->Color_Orange() - sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
                                                                          BG_COLOR - sf::Color(40,40,40,0)),
                                                 BG_INFO);
        //reate the ListBox
        gsListBoxSPtr_.reset( new sfml_util::gui::ListBox("GameStateToLoad",
                                                          GS_LB_RECT,
                                                          listBoxItemSLst,
                                                          this,
                                                          10.0f,
                                                          6.0f,
                                                          BOX_INFO,
                                                          sfml_util::FontManager::Instance()->Color_Orange(),
                                                          0,
                                                          this) );
        EntityAdd(gsListBoxSPtr_);

        SetupGameInfoDisplay();
    }


    void LoadGameStage::SetupGameInfoDisplay()
    {
        //free any existing TextRegion objects
        for (auto const & NEXT_TEXTREGION_SPTR : charTextRegionSVec_)
            EntityRemove(NEXT_TEXTREGION_SPTR);

        charTextRegionSVec_.clear();

        //establish which item is selected and get the player list from that GameState's Party object
        if (gsListBoxSPtr_->Empty())
            return;

        sfml_util::gui::ListBoxItemSPtr_t listBoxItemSPtr(gsListBoxSPtr_->GetSelected());
        M_ASSERT_OR_LOGANDTHROW_SS((listBoxItemSPtr.get() != nullptr), "LoadGameStage::SetupGameInfoDisplay() The ListBox was not empty but GetSelected() returned a nullptr.");

        state::GameStateSPtr_t gameStateSPtr(listBoxItemSPtr->gamestate_sptr);
        M_ASSERT_OR_LOGANDTHROW_SS((gameStateSPtr.get() != nullptr), "LoadGameStage::SetupGameInfoDisplay() The ListBox was not empty but GetSelected() returned a gamestate_sptr that was null.");

        sfml_util::gui::TextInfo descTextInfo("",
                                              sfml_util::FontManager::Instance()->Font_Default2(),
                                              26,
                                              sf::Color::White,
                                              sfml_util::Justified::Left);

        //establish positions
        const float CHAR_LIST_POS_LEFT(gsListBoxPosLeft_ + gsListBoxPosWidth_ + 75.0f);
        const float CHAR_LIST_POS_TOP(gsListBoxPosTop_ + 100.0f);

        //setup location text
        if (locTextRegionSPtr_.get() == nullptr)
        {
            locTextRegionSPtr_.reset(new sfml_util::gui::TextRegion("LoadGameLocation"));
            EntityAdd(locTextRegionSPtr_);
        }
        descTextInfo.text = std::string("Location:        ").append(gameStateSPtr->Location()->Name());
        const sf::FloatRect LOC_TEXT_RECT(CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 35.0f, 0.0f, 0.0f);
        locTextRegionSPtr_->Setup(descTextInfo, LOC_TEXT_RECT);

        //setup character list label text
        if (charLabelTextRegionSPtr_.get() == nullptr)
        {
            charLabelTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("CharacterListLabel") );
            EntityAdd(charLabelTextRegionSPtr_);
        }
        descTextInfo.text = "Characters:";
        const sf::FloatRect CHAR_TEXT_RECT(CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP - 5.0f, 0.0f, 0.0f);
        charLabelTextRegionSPtr_->Setup(descTextInfo, CHAR_TEXT_RECT);

        //setup characters list
        player::PartySPtr_t partySPtr(gameStateSPtr->Party());
        M_ASSERT_OR_LOGANDTHROW_SS((partySPtr.get() != nullptr), "LoadGameStage::SetupGameInfoDisplay() The ListBox was not empty but GetSelected() returned a null Party object.");

        const player::CharacterSVec_t CHAR_SVEC(partySPtr->Characters());

        const std::size_t NUM_CHARS(CHAR_SVEC.size());
        float posY(CHAR_LIST_POS_TOP + 30.0f);
        for (std::size_t i(0); i<NUM_CHARS; ++i)
        {
            std::ostringstream ss;
            ss << "CharList_" << i << "_" << CHAR_SVEC[i]->Name();
            const std::string TEXT_REGION_ENTITY_NAME(ss.str());

            ss.str("");
            ss  << CHAR_SVEC[i]->Name()
                << ", " << CHAR_SVEC[i]->Role().Name()
                << ", rank: " << CHAR_SVEC[i]->Rank();
            descTextInfo.text = ss.str();

            const sf::FloatRect RECT(CHAR_LIST_POS_LEFT + 25.0f, posY, 0.0f, 0.0f);

            sfml_util::gui::TextRegionSPtr_t textRegionSPtr( new sfml_util::gui::TextRegion(TEXT_REGION_ENTITY_NAME, descTextInfo, RECT) );
            charTextRegionSVec_.push_back(textRegionSPtr);
            EntityAdd(textRegionSPtr);

            posY += textRegionSPtr->GetEntityRegion().height - 25.0f;
        }
    }

    void LoadGameStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        bottomSymbol_.Draw(target, STATES);
        Stage::Draw(target, STATES);
    }


    bool LoadGameStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->SoundEffectsSet_Switch()->PlayRandom();
            LoopManager::Instance()->Goto_MainMenu();
            return true;
        }
        else
            return false;
    }

}
}
