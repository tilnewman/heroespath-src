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
// character-stage.cpp
//
#include "character-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gradient-info.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/box.hpp"

#include "game/log-macros.hpp"
#include "game/game-data-file.hpp"
#include "game/ouroboros.hpp"
#include "game/loop-manager.hpp"
#include "game/creature/body-type.hpp"
#include "game/creature/race.hpp"
#include "game/creature/role.hpp"
#include "game/creature/race-stats.hpp"
#include "game/creature/role-stats.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/title.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/name-info.hpp"
#include "game/item/inventory.hpp"
#include "game/player/initial.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/state/game-state-factory.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"

#include <sstream>
#include <memory>


namespace game
{
namespace stage
{

    MenuButton::MenuButton(const std::string & NAME,
                           const std::string & IMAGE_FILENAME_UP,
                           const std::string & IMAGE_FILENAME_OVER)
    :
        FourStateButton(std::string(NAME).append("_MenuButton")),
        stagePtr_      ()
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(0.0f,
                               0.0f,
                               std::string(BUTTONS_PATH).append(IMAGE_FILENAME_UP),
                               "",
                               std::string(BUTTONS_PATH).append(IMAGE_FILENAME_OVER),
                               "",
                               sfml_util::gui::MouseTextInfo(),
                               sfml_util::gui::TextInfo(),
                               false);
    }

    bool MenuButton::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_STATE_CHANGE(GuiEntity::UpdateMousePos(MOUSE_POS_V));

        if (DID_STATE_CHANGE)
        {
            if (GetMouseState() == sfml_util::MouseState::Over)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOn).PlayRandom();
            }
            else
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOff).PlayRandom();
            }

            if (stagePtr_ == nullptr)
            {
                Reset();
            }
            else
            {
                if (stagePtr_->AreAnyAnimNumStillMoving() == false)
                    Reset();
            }
        }

        return DID_STATE_CHANGE;
    }

    void MenuButton::OnClick(const sf::Vector2f &)
    {
        if (stagePtr_ != nullptr)
            stagePtr_->HandleCallback(this);
    }


    const sfml_util::gui::ColorSet  CharacterStage::GUI_DEFAULT_COLORSET_(sf::Color(220, 220, 220), sf::Color(220, 220, 220), sf::Color(180, 180, 180), sf::Color(180, 180, 180));
    const sf::Color                 CharacterStage::LIGHT_TEXT_COLOR_(sfml_util::FontManager::Color_Light());
    const sf::Color                 CharacterStage::DESC_TEXT_COLOR_(sfml_util::FontManager::Color_Orange());
    //
    const std::string               CharacterStage::POPUP_NAME_NONAMEERROR_("CreateNoNameError");
    const std::string               CharacterStage::POPUP_NAME_MISSINGATTRIBS_("CreateMissingAttributesError");
    const std::string               CharacterStage::POPUP_NAME_CREATECONFIRM_("CreateConfirm");
    const std::string               CharacterStage::POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_("BackButtonLeaveScreenComfirm");
    const std::string               CharacterStage::POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_("NextButtonLeaveScreenComfirm");
    const std::string               CharacterStage::POPUP_NAME_HELP_1_("HelpMessage1");
    const std::string               CharacterStage::POPUP_NAME_HELP_2_("HelpMessage2");
    const std::string               CharacterStage::POPUP_NAME_HELP_3_("HelpMessage3");
    const std::string               CharacterStage::POPUP_NAME_IMAGE_SELECTION_("ImageSelection");


    CharacterStage::CharacterStage()
    :
        Stage                   ("CharacterCreation"),
        SCREEN_WIDTH_           (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_          (sfml_util::Display::Instance()->GetWinHeight()),
        SMALL_FONT_SIZE_        (sfml_util::FontManager::Instance()->Size_Small()),
        RADIO_BOX_TEXT_SIZE_    (sfml_util::FontManager::Instance()->Size_Largeish()),
        STATBOX_WIDTH_          (286.0f),
        STATBOX_HEIGHT_         (290.0f),
        STATBOX_POS_LEFT_       ((SCREEN_WIDTH_ * 0.5f) - (STATBOX_WIDTH_ * 0.5f)),
        STATS_POS_LEFT_         (STATBOX_POS_LEFT_ + 10.0f),
        ouroborosUPtr_          (),
        mainMenuTitle_          ("create_char_button_normal.png"),
        attribVertOffset1_      (0.0f),
        attribVertOffset2_      (0.0f),
        smokeAnimmaxX_          (sfml_util::Display::Instance()->GetWinWidth() - (sfml_util::Display::Instance()->GetWinWidth() * 0.25f)),
        smokeAnimDrifterX_      (smokeAnimmaxX_, sfml_util::Display::Instance()->GetWinWidth() - 200.0f, 0.05f, 0.5f),
        smokeAnimDrifterY_      (0.0f, 300.0f, 0.05f, 0.5f),
        backgroundImage_        ("media-images-backgrounds-tile-darkknot"),
        smokeAnimUPtr_          (),
        backButtonSPtr_         ( std::make_shared<MenuButton>("Back", "back_button_normal.png", "back_button_lit.png") ),
        saveButtonSPtr_         ( std::make_shared<MenuButton>("Save", "save_button_normal.png", "save_button_lit.png") ),
        helpButtonSPtr_         ( std::make_shared<MenuButton>("Help", "help_button_normal.png", "help_button_lit.png") ),
        nextButtonSPtr_         ( std::make_shared<MenuButton>("Next", "next_button_normal.png", "next_button_lit.png") ),
        statSetBase_            (),
        statSetRace_            (),
        statSetRole_            (),
        statSetFixedAnim_       (),
        statModifierTextVec_    (),
        willDrawStatModText_    (false),
        strLabelTextRegion_     ("Strength"),
        accLabelTextRegion_     ("Accuracy"),
        chaLabelTextRegion_     ("Charm"),
        lckLabelTextRegion_     ("Luck"),
        spdLabelTextRegion_     ("Speed"),
        intLabelTextRegion_     ("Intelligence"),
        statsLineLength_        (235.0f),
        statsLine1PosTop_       (0.0f),
        statsLine2PosTop_       (0.0f),
        statsLine3PosTop_       (0.0f),
        statsLine4PosTop_       (0.0f),
        statsLine5PosTop_       (0.0f),
        statsLine6PosTop_       (0.0f),
        statsLineVertPosDiff_   (0.0f),
        statsFirstNumPosLeft_   (0.0f),
        statsStrPosTop_         (0.0f),
        statsAccPosTop_         (0.0f),
        statsChaPosTop_         (0.0f),
        statsLckPosTop_         (0.0f),
        statsSpdPosTop_         (0.0f),
        statsIntPosTop_         (0.0f),
        statsBoxUPtr_           (),
        isAnimStats_            (false),
        isWaitingForStats_      (false),
        animStatsDelayPerSec_   (0.0f),
        animStatsTimeCounterSec_(0.0f),
        animStatsSVec_          (),
        fixedStatsSVec_         (),
        initialRollCounter_     (0),
        dragStartY_             (-1.0f),
        closestDragStat_        (stats::stat::Count),
        raceRadioButtonSPtr_    (),
        racetDescTextRegionUPtr_(),
        roleRadioButtonSPtr_    (),
        roletDescTextRegionUPtr_(),
        sexRadioButtonSPtr_     (),
        nameTextEntryBoxSPtr_   (),
        attrDescTextRegionUPtr_ (),
        sbInsTextRegionUPtr_    (),
        sbInsTextSlider_        (150, 255, 4.0f, static_cast<sf::Uint8>(misc::random::Int(150, 255))),
        nInsTextRegionUPtr_     (),
        nInsTextSlider_         (150, 255, 4.0f, static_cast<sf::Uint8>(misc::random::Int(150, 255))),
        bottomSymbol_           (),
        selectedImageIndex_     (0)
    {}


    CharacterStage::~CharacterStage()
    {
        ClearAllEntities();
    }


    bool CharacterStage::HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t & RADIOBUTTON_WRAPPER)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((RADIOBUTTON_WRAPPER.PTR_ != nullptr), "game::CharacterStage::HandleCallback() given a null RADIOBUTTON_PACKAGE.PTR_.");

        //was change to race
        if (raceRadioButtonSPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        {
            const creature::race::Enum WHICH_RACE( static_cast<creature::race::Enum>(RADIOBUTTON_WRAPPER.PTR_->GetSelectedNumber()) );

            //remove stats modifiers since they will change based on race AND role
            UndoAndClearStatModifierChanges();

            //update description box
            SetupRaceDescriptionBox();

            //change to race may have change the role as well
            AdjustRoleRadioButtonsForRace(WHICH_RACE);
            SetupRoleDescriptionBox();

            HandleChangedStatModifiers("after race change");
            return true;
        }

        //was change to role
        if (roleRadioButtonSPtr_.get() == RADIOBUTTON_WRAPPER.PTR_)
        {
            //remove stats modifiers since they will change based on race AND role
            UndoAndClearStatModifierChanges();

            //update description box
            SetupRoleDescriptionBox();

            HandleChangedStatModifiers("after role change");
            return true;
        }

        return false;
    }


    bool CharacterStage::HandleCallback(const game::callback::PopupResponse & POPUP_RESPONSE)
    {
        if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_CREATECONFIRM_) &&
            (sfml_util::Response::IsAffirmative(POPUP_RESPONSE.Response())))
        {
            const std::string           NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));
            const creature::sex::Enum   SEX_ENUM(static_cast<creature::sex::Enum>(sexRadioButtonSPtr_->GetSelectedNumber() + 1));
            const creature::race::Enum  RACE_ENUM(static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()));
            const creature::role::Enum  ROLE_ENUM(((RACE_ENUM == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())));

            //setup the final StatSet for the character
            stats::StatSet statSetFinal(statSetBase_);
            statSetFinal.ResetCurrentAndActualToNormal();

            //get all image filenames possible for the character
            std::vector<std::string> characterImageFilenamesVec;
            sfml_util::gui::CreatureImageManager::Instance()->GetFilenames(characterImageFilenamesVec, RACE_ENUM, ROLE_ENUM, SEX_ENUM);

            //create the new character
            auto newCharacterUPtr = std::make_unique<player::Character>(
                NAME,
                SEX_ENUM,
                creature::BodyType::Make_FromRaceAndRole(RACE_ENUM, ROLE_ENUM),
                creature::Race(RACE_ENUM),
                creature::Role(ROLE_ENUM),
                statSetFinal,
                0,
                1,
                0,
                game::creature::CondEnumVec_t(),
                game::creature::TitleEnumVec_t(),
                game::item::Inventory(),
                sfml_util::DateTime::CurrentDateTime(),
                characterImageFilenamesVec[selectedImageIndex_]);

            //setup the new character with initial health and inventory
            player::Initial::Setup(newCharacterUPtr.get());

            //save the character to disc
            game::state::GameStateFactory::Instance()->SaveCharacter(newCharacterUPtr.get());

            //confirm popup
            std::ostringstream sss;
            sss << "\n"
                << newCharacterUPtr->Name()
                << " the " << creature::sex::ToString(newCharacterUPtr->Sex())
                << " " << newCharacterUPtr->Race().Name();

            if (creature::race::Wolfen != newCharacterUPtr->Race().Which())
            {
                sss << " " << newCharacterUPtr->Role().Name();
            }

            sss  << " saved.";

            LoopManager::Instance()->PopupWaitBegin(
                this, sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    "Test After", sss.str()));

            ResetForNewCharacterCreation();
            return true;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_)
        {
            if (sfml_util::Response::IsAffirmative(POPUP_RESPONSE.Response()))
            {
                LoopManager::Instance()->SetTransitionBeforeFade(LoopState::MainMenu);
            }

            return true;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_)
        {
            if (sfml_util::Response::IsAffirmative(POPUP_RESPONSE.Response()))
            {
                LoopManager::Instance()->SetTransitionBeforeFade(LoopState::PartyCreation);
            }

            return true;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_HELP_1_)
        {
            LoopManager::Instance()->PopupWaitBegin(this,
                                                    sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                                                        POPUP_NAME_HELP_2_,
                                                        std::string("Classic Set:\n\nKnight\nArcher\nBard\nThief\nCleric\nSorcerer") +
                                                            "\n\n\nPet Set:\n\nKnight\nBeastmaster\nDragon or Wolfen\nBard\nThief\nCleric" +
                                                            "\n\n\nWing Set:\n\nBeastmaster\nFirebrand Dragon\nSylavin Dragon\nSorcerer or Bard\nThief\nCleric" +
                                                            "\n\n\nHowler Set:\n\nBeastmaster\nWolfen\nWolfen\nWolfen\nThief\nCleric" +
                                                            "\n\n\n" +
                                                            "Note that one of your characters must have the role of Beastmaster " +
                                                            "if you want a Dragon or Wolfen in your party.",
                                                        sfml_util::PopupButtons::Continue,
                                                        sfml_util::PopupImage::Large));
            return false;
        }
        else if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_HELP_2_)
        {
            LoopManager::Instance()->PopupWaitBegin(this,
                                                    sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                                                        POPUP_NAME_HELP_3_,
                                                        std::string("To make a character, first select the Race and Role.  Use the text descriptions ") +
                                                            "to guide you.  Then hold down the space bar to summon a random set of attributes.  " +
                                                            "\n\nDrag the attributes up and down to customize them for the character you are crafting.  " +
                                                            "Use the attribute description text on the right side of the screen to guide you.  " +
                                                            "If you don't like the attribute values you have, simply hold down the space bar " +
                                                            "again to randomly summon more.\n\nFinally, give your character a name and then click " +
                                                            "the Save button.  Once you have created all the characters you need, click the Next " +
                                                            "button.  You can always return and make more characters.",
                                                        sfml_util::PopupButtons::Okay,
                                                        sfml_util::PopupImage::Large));
            return true;
        }
        else if ((POPUP_RESPONSE.Info().Name() == POPUP_NAME_IMAGE_SELECTION_) &&
                 (POPUP_RESPONSE.Response() != sfml_util::Response::Cancel))
        {
            selectedImageIndex_ = POPUP_RESPONSE.Selection();

            const std::string CHARACTER_NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));
            const bool IS_MALE(sexRadioButtonSPtr_->GetSelectedNumber() == 0);
            const creature::race::Enum RACE(static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()));
            const creature::role::Enum ROLE(((RACE == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())));

            //setup the final StatSet for the character
            stats::StatSet statSetFinal;
            for (int i(0); i < stats::stat::Count; ++i)
            {
                auto const NEXT_ENUM{ static_cast<stats::stat::Enum>(i) };
                statSetFinal.Get(NEXT_ENUM).ResetAll(fixedStatsSVec_[NEXT_ENUM]->Value());
            }

            std::ostringstream ss;
            ss << "\n\nCreate " << CHARACTER_NAME << "?\nA "
                << ((IS_MALE) ? "male " : "female ");

            if (RACE == creature::race::Dragon)
            {
                ss << creature::role::Name(ROLE) << " " << creature::race::Name(RACE);
            }
            else
            {
                ss << creature::race::Name(RACE) << " ";

                if (RACE != creature::race::Wolfen)
                {
                    ss << creature::role::Name(ROLE);
                }
            }

            //const std::string PAD(" ");
            ss << " with:\n\n"
               << "   Strength="     << statSetFinal.Get(stats::stat::Strength).Normal() << "\n"
               << "   Accuracy="     << statSetFinal.Get(stats::stat::Accuracy).Normal() << "\n"
               << "   Charm="        << statSetFinal.Get(stats::stat::Charm).Normal() << "\n"
               << "   Luck="         << statSetFinal.Get(stats::stat::Luck).Normal() << "\n"
               << "   Speed="        << statSetFinal.Get(stats::stat::Speed).Normal() << "\n"
               << "   Intelligence=" << statSetFinal.Get(stats::stat::Intelligence).Normal() << "\n";

            LoopManager::Instance()->PopupWaitBegin(this, sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_CREATECONFIRM_,
                                                                                                                    ss.str(),
                                                                                                                    sfml_util::PopupButtons::YesNo,
                                                                                                                    sfml_util::PopupImage::Large,
                                                                                                                    sfml_util::Justified::Center,
                                                                                                                    sfml_util::sound_effect::PromptQuestion));
            return false;
        }

        //reset Loop's callback handler pointer in the default case
        return true;
    }


    bool CharacterStage::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &)
    {
        return false;
    }


    bool CharacterStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == helpButtonSPtr_.get())
            return HandleCallback_HelpButton();

        if (PACKAGE.PTR_ == backButtonSPtr_.get())
            return HandleCallback_BackButton();

        if (PACKAGE.PTR_ == saveButtonSPtr_.get())
            return HandleCallback_SaveButton();

        if (PACKAGE.PTR_ == nextButtonSPtr_.get())
            return HandleCallback_NextButton();

        return false;
    }


    bool CharacterStage::HandleCallback_SaveButton()
    {
        //verify not animating
        if (AreAnyAnimNumStillMoving())
            return false;

        //verify all stats are ready
        if (AreAnyStatsIgnored())
        {
            std::ostringstream ss;
            ss << "The following attributes are blank: ";

            bool foundAnyToIgnore(false);
            for (std::size_t i(0); i < stats::stat::Count; ++i)
            {
                if (fixedStatsSVec_[i]->IgnoreMe())
                {
                    ss << ((false == foundAnyToIgnore) ? "" : ", ") << stats::stat::Name(static_cast<stats::stat::Enum>(i));
                    foundAnyToIgnore = true;
                }
            }

            ss << ".  All attributes must have a value before your character can be created.  Hold down the spacebar until all attributes have values.";

            const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_MISSINGATTRIBS_,
                                                                                                 ss.str(),
                                                                                                 sfml_util::PopupButtons::Okay,
                                                                                                 sfml_util::PopupImage::RegularSidebar,
                                                                                                 sfml_util::Justified::Left,
                                                                                                 sfml_util::sound_effect::PromptQuestion));

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);

            return false;
        }

        //verify name is not blank/empty
        const std::string CHARACTER_NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));
        if (CHARACTER_NAME.empty())
        {
            LoopManager::Instance()->PopupWaitBegin(this, sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_NONAMEERROR_,
                                                                                                                    "The name box is empty.  You must name your character to continue.",
                                                                                                                    sfml_util::PopupButtons::Okay,
                                                                                                                    sfml_util::PopupImage::Banner,
                                                                                                                    sfml_util::Justified::Center,
                                                                                                                    sfml_util::sound_effect::PromptQuestion));
            return false;
        }

        //establish character details
        const creature::sex::Enum SEX((sexRadioButtonSPtr_->GetSelectedNumber() == 0) ? creature::sex::Male : creature::sex::Female);
        const creature::race::Enum RACE(static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()));
        const creature::role::Enum ROLE(((RACE == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())));

        //get all image filenames possible for the character
        std::vector<std::string> characterImageFilenamesVec;
        sfml_util::gui::CreatureImageManager::Instance()->GetFilenames(characterImageFilenamesVec, RACE, ROLE, SEX);

        //load images
        sfml_util::TextureVec_t characterTextureVec;
        for (auto const & NEXT_FILENAME_STR : characterImageFilenamesVec)
        {
            sf::Texture texture;
            sfml_util::gui::CreatureImageManager::Instance()->GetImage(texture, NEXT_FILENAME_STR, true);
            characterTextureVec.push_back(texture);
        }

        std::ostringstream ss;
        ss << "Choose an image for \"" << CHARACTER_NAME << "\"";
        LoopManager::Instance()->PopupWaitBegin(this, sfml_util::gui::PopupManager::Instance()->
            CreatePopupInfo(POPUP_NAME_IMAGE_SELECTION_,
                            ss.str(),
                            characterTextureVec,
                            true));

        return true;
    }


    bool CharacterStage::HandleCallback_BackButton()
    {
        //verify all stats are ready
        if (AreAnyAnimNumStillMoving())
            return false;

        const std::string NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));

        //if it seems the user has been working on a character, prompt before losing the data
        if ((false == AreAnyStatsIgnored()) || (false == NAME.empty()))
        {
            std::ostringstream ss;
            ss << "Leaving will cause ";

            if (NAME.empty())
                ss << "this character ";
            else
                ss << "the character \"" << NAME << "\" ";

            ss << "to be lost.  Are you sure?";

            LoopManager::Instance()->PopupWaitBegin(this,
                                                    sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_,
                                                                                                              ss.str(),
                                                                                                              sfml_util::PopupButtons::YesNo,
                                                                                                              sfml_util::PopupImage::Banner,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::PromptWarn));

            return true;
        }
        else
            LoopManager::Instance()->Goto_MainMenu();

        return true;
    }


    bool CharacterStage::HandleCallback_HelpButton()
    {
        LoopManager::Instance()->PopupWaitBegin(this,
                                                sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                                                    POPUP_NAME_HELP_1_,
                                                    std::string("You play Heroes' Path with a group of six characters called a party.  ") +
                                                        "This is where you create those characters one at a time. " +
                                                        "The first thing to do is decide what Roles you want those characters to have.  " +
                                                        "Role is simply a character's occupation, or type.  You should choose the " +
                                                        "combination of Roles in your party carefully, so they work well together as " +
                                                        "a team." +
                                                        "\n\n" +
                                                        "If you are new to the game, there are four recommended sets of " +
                                                        "Roles that are known to be effective and fun to play.  Click continue below to see them listed.",
                                                    sfml_util::PopupButtons::Continue,
                                                    sfml_util::PopupImage::Regular));
        return true;
    }


    bool CharacterStage::HandleCallback_NextButton()
    {
        //verify all stats are ready
        if (AreAnyAnimNumStillMoving())
            return false;

        const std::string NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));

        //if it seems the user has been working on a character, prompt before losing the data
        if ((false == AreAnyStatsIgnored()) || (false == NAME.empty()))
        {
            std::ostringstream ss;
            ss << "Leaving will cause ";

            if (NAME.empty())
                ss << "this character ";
            else
                ss << "the character \"" << NAME << "\" ";

            ss << "to be lost.  Are you sure?";

            LoopManager::Instance()->PopupWaitBegin(this,
                                                    sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_,
                                                                                                              ss.str(),
                                                                                                              sfml_util::PopupButtons::YesNo,
                                                                                                              sfml_util::PopupImage::Banner,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::PromptWarn));

            return true;
        }
        else
            LoopManager::Instance()->Goto_PartyCreation();

        return true;
    }


    void CharacterStage::AdjustRoleRadioButtonsForRace(const game::creature::race::Enum WHICH_RACE)
    {
        using namespace creature;

        std::vector<role::Enum> temp;
        if (WHICH_RACE == creature::race::Dragon)
            temp = { role::Archer, role::Bard, role::Beastmaster, role::Cleric, role::Knight, role::Sorcerer, role::Thief };
        else
        {
            if (WHICH_RACE == creature::race::Pixie)
                temp = { role::Sylavin, role::Firebrand, role::Knight, role::Archer };
            else
            {
                if (WHICH_RACE == creature::race::Wolfen)
                    temp = { role::Archer, role::Bard, role::Beastmaster, role::Sylavin, role::Cleric, role::Firebrand, role::Knight, role::Sorcerer, role::Thief };
                else
                    temp = { role::Sylavin, role::Firebrand };
            }
        }

        misc::SizetVec_t invalidSelectionsVec;
        for (auto const NEXT_ENUM : temp)
            invalidSelectionsVec.push_back( static_cast<std::size_t>(NEXT_ENUM) );

        roleRadioButtonSPtr_->SetInvalidSelections(invalidSelectionsVec);
    }


    void CharacterStage::Setup()
    {
        ouroborosUPtr_ = std::make_unique<Ouroboros>("CharacterStage's");
        EntityAdd(ouroborosUPtr_.get());

        //help button
        helpButtonSPtr_->SetScaleToRes();
        helpButtonSPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) - helpButtonSPtr_->GetEntityRegion().width - 180.0f);
        EntityAdd(helpButtonSPtr_.get());
        helpButtonSPtr_->SetOwningCharacterStage(this);

        //create button
        saveButtonSPtr_->SetScaleToRes();
        saveButtonSPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) + 180.0f);
        EntityAdd(saveButtonSPtr_.get());
        saveButtonSPtr_->SetOwningCharacterStage(this);

        //back button
        backButtonSPtr_->SetScaleToRes();
        backButtonSPtr_->SetVertPositionToBottomOfScreenByRes((((SCREEN_WIDTH_ * 0.5f) - backButtonSPtr_->GetEntityRegion().width) - 400.0f) - sfml_util::MapByRes(0.0f, 400.0f));
        EntityAdd(backButtonSPtr_.get());
        backButtonSPtr_->SetOwningCharacterStage(this);

        //Next button
        nextButtonSPtr_->SetScaleToRes();
        nextButtonSPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) + 400.0f + sfml_util::MapByRes(0.0f, 400.0f));
        EntityAdd(nextButtonSPtr_.get());
        nextButtonSPtr_->SetOwningCharacterStage(this);

        //race selection RadioButtonSet
        {
            sfml_util::gui::TextInfo raceRadioButtonSetTextInfo(" ",
                                                                sfml_util::FontManager::Instance()->Font_Default2(),
                                                                RADIO_BOX_TEXT_SIZE_,
                                                                LIGHT_TEXT_COLOR_,
                                                                sfml_util::Justified::Left);

            std::vector<std::string> raceNameVec;
            for (std::size_t i(0); i < creature::race::Count_PlayerRaces; ++i)
                raceNameVec.push_back(creature::race::Name(static_cast<creature::race::Enum>(i)));

            const sf::FloatRect REGION(sfml_util::MapByRes(40.0f, 1525.0f), mainMenuTitle_.LowerPosition(), 0.0f, 0.0f);
            const sfml_util::gui::BackgroundInfo BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), REGION);
            const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

            raceRadioButtonSPtr_ = std::make_shared<sfml_util::gui::RadioButtonSet>(
                "RaceSelection",
                REGION.left,
                REGION.top,
                raceRadioButtonSetTextInfo,
                raceNameVec,
                0,
                sfml_util::Brightness::Bright,
                misc::SizetVec_t(),
                BOX_INFO,
                sfml_util::gui::RadioButtonSet::BETWEEN_PAD_DEFAULT_,
                0.0f);

            raceRadioButtonSPtr_->CallbackHandlerAdd(this);
            EntityAdd(raceRadioButtonSPtr_.get());
        }

        //role selection RadioButtonSet
        {
            sfml_util::gui::TextInfo roleRadioButtonSetTextInfo(" ",
                                                                sfml_util::FontManager::Instance()->Font_Default2(),
                                                                RADIO_BOX_TEXT_SIZE_,
                                                                LIGHT_TEXT_COLOR_,
                                                                sfml_util::Justified::Left);

            std::vector<std::string> roleNameVec;
            for (std::size_t i(0); i < creature::role::PlayerRoleCount-1; ++i)//exclude Wolfen
                roleNameVec.push_back(creature::role::Name(static_cast<creature::role::Enum>(i)));

            const float POS_TOP(raceRadioButtonSPtr_->GetEntityRegion().top + raceRadioButtonSPtr_->GetEntityRegion().height + sfml_util::MapByRes(35.0f, 75.0f));
            const sf::FloatRect REGION(sfml_util::MapByRes(15.0f, 1300.0f), POS_TOP, 0.0f, 0.0f);
            const sfml_util::gui::BackgroundInfo BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), REGION);
            const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

            roleRadioButtonSPtr_ = std::make_shared<sfml_util::gui::RadioButtonSet>(
                "RoleSelection",
                REGION.left,
                REGION.top,
                roleRadioButtonSetTextInfo,
                roleNameVec,
                0,
                sfml_util::Brightness::Bright,
                misc::SizetVec_t(),
                BOX_INFO,
                sfml_util::gui::RadioButtonSet::BETWEEN_PAD_DEFAULT_,
                -5.0f);

            roleRadioButtonSPtr_->CallbackHandlerAdd(this);
            EntityAdd(roleRadioButtonSPtr_.get());
        }

        SetupRaceDescriptionBox();
        SetupRoleDescriptionBox();
        SetupAttrDescriptionBox();

        //name label
        {
            const sf::FloatRect REGION((SCREEN_WIDTH_ * 0.5f) - 150.0f,
                mainMenuTitle_.LowerPosition() - 20.0f, 0.0f, 0.0f);

            const sfml_util::gui::TextInfo NAME_LABEL_TEXT_INFO(
                "(name your character here)",
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Small(),
                LIGHT_TEXT_COLOR_,
                sf::BlendAlpha,
                sf::Text::Italic);

            nInsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "NameLabel",
                NAME_LABEL_TEXT_INFO,
                REGION);

            nInsTextRegionUPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (nInsTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 45.0f,
                                              nInsTextRegionUPtr_->GetEntityPos().y);

            EntityAdd(nInsTextRegionUPtr_.get());
        }

        //name text entry box
        {
            const float WIDTH(creature::NameInfo::Instance()->TextEntryBoxWidth());

            const sf::FloatRect REGION((SCREEN_WIDTH_ * 0.5f) - (WIDTH * 0.5f),
                                       nInsTextRegionUPtr_->GetEntityRegion().top + nInsTextRegionUPtr_->GetEntityRegion().height,
                                       WIDTH,
                                       55.0f);

            const sfml_util::gui::BackgroundInfo BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), REGION);
            const sfml_util::gui::box::Info BOX_INFO(true, REGION, GUI_DEFAULT_COLORSET_, BG_INFO);
            
            nameTextEntryBoxSPtr_ = std::make_shared<sfml_util::gui::TextEntryBox>(
                "CharacterName",
                REGION,
                creature::NameInfo::Instance()->MakeTextInfo(" ", DESC_TEXT_COLOR_),
                LIGHT_TEXT_COLOR_,
                BOX_INFO);

            EntityAdd(nameTextEntryBoxSPtr_.get());
        }

        //sex selection RadioButtonSet
        {
            sfml_util::gui::TextInfo sexRadioButtonSetTextInfo(" ",
                                                               sfml_util::FontManager::Instance()->Font_Default2(),
                                                               RADIO_BOX_TEXT_SIZE_,
                                                               LIGHT_TEXT_COLOR_,
                                                               sfml_util::Justified::Left);

            std::vector<std::string> sexNameVec;
            sexNameVec.push_back("Male");
            sexNameVec.push_back("Female");

            const sf::FloatRect TEMP_REGION;
            const sfml_util::gui::BackgroundInfo BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), TEMP_REGION);
            const sfml_util::gui::box::Info BOX_INFO(true, TEMP_REGION, GUI_DEFAULT_COLORSET_, BG_INFO);

            sexRadioButtonSPtr_ = std::make_shared<sfml_util::gui::RadioButtonSet>(
                "SexSelection",
                0.0f,
                0.0f,
                sexRadioButtonSetTextInfo,
                sexNameVec,
                0,
                sfml_util::Brightness::Bright,
                misc::SizetVec_t(),
                BOX_INFO,
                5.0f,
                0.0f);

            sexRadioButtonSPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (sexRadioButtonSPtr_->GetEntityRegion().width * 0.5f),
                                              nameTextEntryBoxSPtr_->GetEntityPos().y + nameTextEntryBoxSPtr_->GetEntityRegion().height + sfml_util::MapByRes(25.0f, 70.0f));

            EntityAdd(sexRadioButtonSPtr_.get());
        }

        //spacebar instruction text
        {
            sfml_util::gui::TextInfo insTextInfo("(click the box below then hold the spacebar to change attributes)\n(drag numbers up and down to customize)                ",
                                                 sfml_util::FontManager::Instance()->Font_Typical(),
                                                 sfml_util::FontManager::Instance()->Size_Small(),
                                                 LIGHT_TEXT_COLOR_,
                                                 sf::BlendAlpha,
                                                 sf::Text::Italic,
                                                 sfml_util::Justified::Center);

            sbInsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SpacebarInstructions",
                insTextInfo,
                sf::FloatRect());

            sbInsTextRegionUPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (sbInsTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 100.0f,
                                               sexRadioButtonSPtr_->GetEntityRegion().top + sexRadioButtonSPtr_->GetEntityRegion().height + sfml_util::MapByRes(30.0f, 90.0f));
            
            EntityAdd(sbInsTextRegionUPtr_.get());
        }

        //Stat BackgroundBox
        const float STATBOX_POS_TOP(sbInsTextRegionUPtr_->GetEntityRegion().top + sbInsTextRegionUPtr_->GetEntityRegion().height + 8.0f);
        {
            const sf::FloatRect RECT(STATBOX_POS_LEFT_,
                                     STATBOX_POS_TOP,
                                     STATBOX_WIDTH_,
                                     STATBOX_HEIGHT_);

            const sfml_util::gui::BackgroundInfo BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), RECT);

            sfml_util::gui::box::Info boxInfo(true,
                                              RECT,
                                              sfml_util::gui::ColorSet(sf::Color(220, 220, 220),
                                                                       sf::Color(220, 220, 220),
                                                                       sf::Color(180, 180, 180),
                                                                       sf::Color(180, 180, 180)),
                                              BG_INFO);

            statsBoxUPtr_ = std::make_unique<sfml_util::gui::box::Box>(
                "CharacterStageStats", boxInfo);

            EntityAdd(statsBoxUPtr_.get());
        }

        //Stat Labels

        sfml_util::gui::TextInfo statTextInfo(statSetBase_.Str().Name(),
                                              sfml_util::FontManager::Instance()->Font_Typical(),
                                              38,
                                              LIGHT_TEXT_COLOR_,
                                              sfml_util::Justified::Left);

        {
            const float STATS_TEXT_VERT_OFFSET(-13.0f);
            const float STATS_LINE_VERT_OFFSET(-7.0f);

            sf::FloatRect rect(STATS_POS_LEFT_, STATBOX_POS_TOP, 0.0f, 0.0f);
            strLabelTextRegion_.Setup(statTextInfo, rect);

            statTextInfo.text = statSetBase_.Acc().Name();
            rect.top += strLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine1PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
            accLabelTextRegion_.Setup(statTextInfo, rect);

            statTextInfo.text = statSetBase_.Cha().Name();
            rect.top += accLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine2PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
            chaLabelTextRegion_.Setup(statTextInfo, rect);

            statTextInfo.text = statSetBase_.Lck().Name();
            rect.top += chaLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine3PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
            lckLabelTextRegion_.Setup(statTextInfo, rect);

            statTextInfo.text = statSetBase_.Spd().Name();
            rect.top += lckLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine4PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
            spdLabelTextRegion_.Setup(statTextInfo, rect);

            statTextInfo.text = statSetBase_.Int().Name();
            rect.top += spdLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine5PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
            intLabelTextRegion_.Setup(statTextInfo, rect);

            rect.top += intLabelTextRegion_.GetEntityRegion().height + STATS_TEXT_VERT_OFFSET;
            statsLine6PosTop_ = rect.top + STATS_LINE_VERT_OFFSET;
        }

        //stat numbers placement
        {
            //establish positions and offsets
            const float STAT_NUM_HORIZ_OFFSET(statsLineLength_ - 40.0f);
            statsFirstNumPosLeft_ = STATS_POS_LEFT_ + STAT_NUM_HORIZ_OFFSET;
            attribVertOffset1_ = -40.0f;
            attribVertOffset2_ = (statsLine2PosTop_ - statsLine1PosTop_);

            //establish actual attribute number positions on screen
            statsStrPosTop_ = GetAttributeNumPosTop(stats::stat::Strength);
            statsAccPosTop_ = GetAttributeNumPosTop(stats::stat::Accuracy);
            statsChaPosTop_ = GetAttributeNumPosTop(stats::stat::Charm);
            statsLckPosTop_ = GetAttributeNumPosTop(stats::stat::Luck);
            statsSpdPosTop_ = GetAttributeNumPosTop(stats::stat::Speed);
            statsIntPosTop_ = GetAttributeNumPosTop(stats::stat::Intelligence);

            statsLineVertPosDiff_ = statsIntPosTop_ - statsSpdPosTop_;
        }

        //fill the fixed stats vector with dummy values initially...
        const std::size_t NUM_STATS(stats::stat::Count);
        for (std::size_t i(0); i < NUM_STATS; ++i)
            fixedStatsSVec_.push_back( AnimNumSPtr_t() );

        //...then assign valid AnimNumSPtr_t objects with a value of zero
        fixedStatsSVec_[stats::stat::Strength]      = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Strength,     statsFirstNumPosLeft_, statsStrPosTop_, statsFirstNumPosLeft_, statsStrPosTop_, statTextInfo);
        fixedStatsSVec_[stats::stat::Accuracy]      = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Accuracy,     statsFirstNumPosLeft_, statsAccPosTop_, statsFirstNumPosLeft_, statsAccPosTop_, statTextInfo);
        fixedStatsSVec_[stats::stat::Charm]         = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Charm,        statsFirstNumPosLeft_, statsChaPosTop_, statsFirstNumPosLeft_, statsChaPosTop_, statTextInfo);
        fixedStatsSVec_[stats::stat::Luck]          = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Luck,         statsFirstNumPosLeft_, statsLckPosTop_, statsFirstNumPosLeft_, statsLckPosTop_, statTextInfo);
        fixedStatsSVec_[stats::stat::Speed]         = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Speed,        statsFirstNumPosLeft_, statsSpdPosTop_, statsFirstNumPosLeft_, statsSpdPosTop_, statTextInfo);
        fixedStatsSVec_[stats::stat::Intelligence]  = std::make_shared<AnimNum>(stats::Stat::VAL_INVALID_, stats::stat::Intelligence, statsFirstNumPosLeft_, statsIntPosTop_, statsFirstNumPosLeft_, statsIntPosTop_, statTextInfo);

        //smoke animation
        //Note:  Keep this last to be added to the enitySVec_ in this function
        smokeAnimUPtr_ = sfml_util::AnimationFactory::Make(sfml_util::Animations::SmokeSwirl,
                                                           sfml_util::MapByRes(1.0f, 3.0f),
                                                           0.035f,
                                                           sf::Color::White,
                                                           sf::BlendAlpha);

        EntityAdd(smokeAnimUPtr_.get());

        //setup initial config of radio buttons
        AdjustRoleRadioButtonsForRace(static_cast<game::creature::race::Enum>(0));

        SetupRoleDescriptionBox();
    }


    float CharacterStage::GetAttributeNumPosTop(const stats::stat::Enum STAT)
    {
        switch (STAT)
        {
            case stats::stat::Strength:     { return statsLine1PosTop_ + attribVertOffset1_ + (0 * attribVertOffset2_); }
            case stats::stat::Accuracy:     { return statsLine1PosTop_ + attribVertOffset1_ + (1 * attribVertOffset2_); }
            case stats::stat::Charm:        { return statsLine1PosTop_ + attribVertOffset1_ + (2 * attribVertOffset2_); }
            case stats::stat::Luck:         { return statsLine1PosTop_ + attribVertOffset1_ + (3 * attribVertOffset2_); }
            case stats::stat::Speed:        { return statsLine1PosTop_ + attribVertOffset1_ + (4 * attribVertOffset2_); }
            case stats::stat::Intelligence:
            case stats::stat::Count:
            default:                        { return statsLine1PosTop_ + attribVertOffset1_ + (5 * attribVertOffset2_); }
        }
    }


    void CharacterStage::SetupRaceDescriptionBox()
    {
        const creature::race::Enum RACE_ENUM( static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()) );
        const std::string RACE_DESC(creature::race::Desc(RACE_ENUM));

        const sf::FloatRect REGION(raceRadioButtonSPtr_->GetEntityRegion().left + raceRadioButtonSPtr_->GetEntityRegion().width + 15.0f,
                                   raceRadioButtonSPtr_->GetEntityPos().y + sfml_util::MapByRes(15.0f, 45.0f),
                                   sfml_util::MapByRes(232.0f, 1500.0f),
                                   raceRadioButtonSPtr_->GetEntityRegion().height - 30.0f);

        sfml_util::gui::TextInfo raceDescTextInfo(  RACE_DESC,
                                                    sfml_util::FontManager::Instance()->Font_Default2(),
                                                    30,
                                                    DESC_TEXT_COLOR_,
                                                    sfml_util::Justified::Left );

        if (racetDescTextRegionUPtr_.get() == nullptr)
        {
            racetDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "RaceDescription",
                raceDescTextInfo,
                REGION,
                this,
                SMALL_FONT_SIZE_);

            EntityAdd(racetDescTextRegionUPtr_.get());
        }
        else
        {
            racetDescTextRegionUPtr_->Setup(raceDescTextInfo,
                                            REGION,
                                            this,
                                            SMALL_FONT_SIZE_);
        }
    }


    void CharacterStage::SetupRoleDescriptionBox()
    {
        const creature::role::Enum ROLE_ENUM( ((static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()) == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())));
        const std::string ROLE_DESC( creature::role::Desc(ROLE_ENUM) );

        auto const ROLE_RADIOBUTTON_REGION{ roleRadioButtonSPtr_->GetEntityRegion() };
        const sf::FloatRect REGION(ROLE_RADIOBUTTON_REGION.left + ROLE_RADIOBUTTON_REGION.width + 15.0f,
                                   ROLE_RADIOBUTTON_REGION.top + sfml_util::MapByRes(25.0f, 75.0f),
                                   sfml_util::MapByRes(240.0f, 1750.0f),
                                   roleRadioButtonSPtr_->GetEntityRegion().height - 30.0f);

        sfml_util::gui::TextInfo roleDescTextInfo(ROLE_DESC,
                                                  sfml_util::FontManager::Instance()->Font_Dialog2(),
                                                  30,
                                                  DESC_TEXT_COLOR_,
                                                  sfml_util::Justified::Left);

        if (roletDescTextRegionUPtr_.get() == nullptr)
        {
            roletDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "RoleDescription",
                roleDescTextInfo,
                REGION,
                this,
                SMALL_FONT_SIZE_);

            EntityAdd(roletDescTextRegionUPtr_.get());
        }
        else
        {
            roletDescTextRegionUPtr_->Setup(roleDescTextInfo,
                                            REGION,
                                            this,
                                            SMALL_FONT_SIZE_);
        }
    }


    void CharacterStage::SetupAttrDescriptionBox()
    {
        //establish horiz position
        const float ATTR_DESC_WIDTH(sfml_util::MapByRes(335.0f, 3000.0f));
        const float ATTR_DESC_HEIGHT(sfml_util::MapByRes(310.0f, 2100.0f));
        const float ATTR_DESC_POS_LEFT((SCREEN_WIDTH_ - ATTR_DESC_WIDTH) - sfml_util::MapByRes(15.0f, 300.0f));
        const float ATTR_DESC_POS_TOP(sfml_util::MapByRes(450.0f, 1600.0f));

        //establish region that the attribute description text will be drawn to
        const sf::FloatRect REGION(ATTR_DESC_POS_LEFT,
                                   ATTR_DESC_POS_TOP,
                                   ATTR_DESC_WIDTH,
                                   ATTR_DESC_HEIGHT);

        const std::string PAD(" - ");
        std::ostringstream ss;
        sfml_util::gui::TextInfo descTextInfo(ss.str(),
                                              sfml_util::FontManager::Instance()->Font_Default2(),
                                              SMALL_FONT_SIZE_,
                                              DESC_TEXT_COLOR_,
                                              sfml_util::Justified::Center);

        sf::Color helpStrColor;
        sfml_util::gui::TextInfo helpTextInfo(descTextInfo);
        sfml_util::gui::TextRegionUVec_t textRegionUVec;

        //strength
        ss.str("");
        ss << stats::stat::Name(stats::stat::Strength)
           << PAD
           << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Strength")
           << "\n\n";

        //see below, will add explicitly
        auto const STRENGTH_BASE_TEXT{ ss.str() };
        
        //strength help text
        GetStatHelpText(stats::stat::Strength, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "StrAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //Accuracy
        ss.str("");
        ss << stats::stat::Name(stats::stat::Accuracy)
            << PAD
            << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Accuracy")
            << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
            "AccStatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()) );

        //Accuracy help text
        GetStatHelpText(stats::stat::Accuracy, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "AccAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //Charm
        ss.str("");
        ss << stats::stat::Name(stats::stat::Charm)
            << PAD
            << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Charm")
            << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
            "ChaStatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()) );

        //charm help text
        GetStatHelpText(stats::stat::Charm, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {   
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "ChaAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //Luck
        ss.str("");
        ss << stats::stat::Name(stats::stat::Luck)
            << PAD
            << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Luck")
            << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
            "LckStatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()) );

        //Luck help text
        GetStatHelpText(stats::stat::Luck, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "LuckAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //Speed
        ss.str("");
        ss << stats::stat::Name(stats::stat::Speed)
            << PAD
            << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Speed")
            << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
            "SpdStatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()) );

        //Speed help text
        GetStatHelpText(stats::stat::Speed, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "SpdAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //Intelligence
        ss.str("");
        ss << stats::stat::Name(stats::stat::Intelligence)
            << PAD
            << GameDataFile::Instance()->GetCopyStr("heroespath-stats-stat-desc_Intelligence")
            << "\n\n";

        descTextInfo.text = ss.str();
        textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
            "IntStatDesc",
            descTextInfo,
            REGION,
            sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
            sfml_util::gui::box::Info(),
            sfml_util::Margins()) );

        //Intelligence help text
        GetStatHelpText(stats::stat::Intelligence, helpTextInfo);
        if (false == helpTextInfo.text.empty())
        {
            textRegionUVec.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "IntAttrDesc",
                helpTextInfo,
                REGION,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info(),
                sfml_util::Margins()) );
        }

        //setup the final TextRegion that will be drawn to the screen
        descTextInfo.text = STRENGTH_BASE_TEXT;
        if (attrDescTextRegionUPtr_.get() == nullptr)
        {
            attrDescTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "AttributeDescription",
                descTextInfo,
                REGION,
                this);

            EntityAdd(attrDescTextRegionUPtr_.get());
        }
        else
        {
            attrDescTextRegionUPtr_->Setup(descTextInfo, REGION, this);
        }

        for (auto const & NEXT_TR_UPTR : textRegionUVec)
        {
            attrDescTextRegionUPtr_->Append( * NEXT_TR_UPTR );
        }
    }


    bool CharacterStage::GetStatHelpText(const stats::stat::Enum    WHICH_STAT,
                                         sfml_util::gui::TextInfo & textInfo) const
    {
        //bail if no values yet
        if ((static_cast<std::size_t>(WHICH_STAT) >= fixedStatsSVec_.size()) ||
            (AreAnyAnimNumStillMoving()) ||
            (AreAnyStatsIgnored()))
        {
            textInfo.text.clear();
            textInfo.color = DESC_TEXT_COLOR_;
            return false;
        }

        const std::string RECC_NORMAL_STR("It would be a good idea to increase this attribute if you can.");
        const std::string RECC_STRONG_STR("It is strongly reccomended that you increase this attribute.");

        sf::Color reccNormalColor(DESC_TEXT_COLOR_);
        reccNormalColor.g = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.g) + 20);
        reccNormalColor.b = static_cast<sf::Uint8>(static_cast<int>(reccNormalColor.b) + 40);

        sf::Color reccStrongColor(255, 200, 181);

        const creature::role::Enum WHICH_ROLE( ((static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()) == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())));

        const stats::Stat_t STAT_VALUE(fixedStatsSVec_[static_cast<std::size_t>(WHICH_STAT)]->Value());

        std::ostringstream sss;
        sss << "Your current " << stats::stat::ToString(WHICH_STAT) << " is only " << STAT_VALUE << ".";
        const std::string IS_ONLY_STR(sss.str());

        const game::stats::Stat_t STAT_VALUE_LOW      { 10 };
        const game::stats::Stat_t STAT_VALUE_MED      { 15 };
        const game::stats::Stat_t STAT_VALUE_HIGH     { 20 };
        const game::stats::Stat_t STAT_VALUE_HIGHEST  { 25 };

        std::ostringstream ss;
        if (WHICH_ROLE == creature::role::Archer)
        {
            if (WHICH_STAT == stats::stat::Accuracy)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "Archers need Accuracy the most, and your current Accuracy is only " << STAT_VALUE << ".  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << "Archers need Accuracy the most, and your current Accuracy is only " << STAT_VALUE << ".  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Strength)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "An Archer is primarily a fighter, so the Strength attribute is important.  Your current Strength is only " << STAT_VALUE << ".  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << "An Archer is primarily a fighter, so the Strength attribute is important.  Your current Strength is only " << STAT_VALUE << ".  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Speed)
            {
                if (STAT_VALUE < STAT_VALUE_LOW)
                {
                    ss << "An Archer is primarily a fighter, so the Speed attribute is important for dodging attacks.  Your current Speed is only " << STAT_VALUE << ".  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << "An Archer is primarily a fighter, so the Speed attribute is important for dodging attacks.  Your current Speed is only " << STAT_VALUE << ".  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Bard)
        {
            if (WHICH_STAT == stats::stat::Charm)
            {
                const std::string REASON_STR("Charm is important to Bards for magical song.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGHEST)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Intelligence)
            {
                const std::string REASON_STR("Inteflligence is important to Bards for magical song.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGHEST)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Strength)
            {
                const std::string REASON_STR("Strength is sometimes important to Bards if you want them to also be good fighters.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccStrongColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Beastmaster)
        {
            if (WHICH_STAT == stats::stat::Intelligence)
            {
                const std::string REASON_STR("A Beastmaster's primary attribute is Intelligence, for the magical bond they have to their animals.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Strength)
            {
                const std::string REASON_STR("Strength is often important to Beastmasters because they can be good fighters.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccStrongColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Cleric)
        {
            if (WHICH_STAT == stats::stat::Charm)
            {
                const std::string REASON_STR("A Cleric's primary attribute is Charm so they can heal and be good diplomats.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Intelligence)
            {
                const std::string REASON_STR("A Cleric is a kind of spell caster, so Intelligence is very important.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if ((WHICH_ROLE == creature::role::Firebrand) || (WHICH_ROLE == creature::role::Sylavin))
        {
            if (WHICH_STAT == stats::stat::Strength)
            {
                const std::string REASON_STR("Dragons may be magical creatures, but make no mistake, they are fighters.  Strength is important to all types of Dragons.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Intelligence)
            {
                const std::string REASON_STR("Dragons are magical creatures so Intelligence is important.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Knight)
        {
            const std::string REASON_STR("Knights are fighters, so they need Strength, Accuracy, and Speed -in that order.");

            if (WHICH_STAT == stats::stat::Strength)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Speed)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Accuracy)
            {
                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Sorcerer)
        {
            if (WHICH_STAT == stats::stat::Intelligence)
            {
                const std::string REASON_STR("A Sorcerer is first and foremost a spell caster, so Intelligence is very important.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Speed)
            {
                const std::string REASON_STR("Sorcerers are typically physically weaker than the other adventurers.  Speed will be important for dodging attacks.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Thief)
        {
            if (WHICH_STAT == stats::stat::Speed)
            {
                const std::string REASON_STR("Thieves are typically physically weaker than the other adventurers.  Speed will be important for dodging attacks and for having fast fingers.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Luck)
            {
                const std::string REASON_STR("Thieves need Luck more than any other attribute.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else if (WHICH_ROLE == creature::role::Wolfen)
        {
            if (WHICH_STAT == stats::stat::Strength)
            {
                const std::string REASON_STR("Wolfens are not in your party of adventurers for their looks, they are there to defense.  Strength for fighting is most important.");

                if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < 25)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Accuracy)
            {
                const std::string REASON_STR("A Wolfen's strong jaws are useless if they can't bite anything with them.  Accuracy is important.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }

            if (WHICH_STAT == stats::stat::Speed)
            {
                const std::string REASON_STR("Wolfen's are fast for a reason, they need to avoid being hit with attacks.  Speed will be of primary importance for those on the front line of a fight.");

                if (STAT_VALUE < STAT_VALUE_MED)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_STRONG_STR;
                    textInfo.color = reccStrongColor;
                }
                else if (STAT_VALUE < STAT_VALUE_HIGH)
                {
                    ss << REASON_STR << "  " << IS_ONLY_STR << "  " << RECC_NORMAL_STR;
                    textInfo.color = reccNormalColor;
                }
            }
        }
        else
        {
            ss.clear();
            textInfo.color = DESC_TEXT_COLOR_;
        }

        if (ss.str().empty() == false)
            ss << "\n\n";

        textInfo.text = ss.str();
        return ! textInfo.text.empty();
    }


    void CharacterStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        //oscillate the spacebar instruction text's color to help players know what to do initially
        if (AreAnyStatsIgnored() && (false == AreAnyAnimNumStillMoving()))
        {
            const sf::Uint8 NEW_COLOR_VAL( static_cast<sf::Uint8>(sbInsTextSlider_.Update(ELAPSED_TIME_SECONDS)) );

            sf::Color color(sbInsTextRegionUPtr_->GetEntityColorForeground());
            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            sbInsTextRegionUPtr_->SetEntityColorFgBoth(color);
        }
        else
        {
            sbInsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::White);
        }

        //oscillate the spacebar instruction text's color to help players know what to do initially
        const std::string CHARACTER_NAME(boost::algorithm::trim_copy(nameTextEntryBoxSPtr_->GetText()));
        if (CHARACTER_NAME.empty())
        {
            const sf::Uint8 NEW_COLOR_VAL(static_cast<sf::Uint8>(nInsTextSlider_.Update(ELAPSED_TIME_SECONDS)));

            sf::Color color(nInsTextRegionUPtr_->GetEntityColorForeground());
            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            nInsTextRegionUPtr_->SetEntityColorFgBoth(color);
        }
        else
        {
            nInsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::White);
        }

        //drift the position of the smoke anim
        smokeAnimUPtr_->SetEntityPos(smokeAnimDrifterX_.Update(ELAPSED_TIME_SECONDS),
            smokeAnimDrifterY_.Update(ELAPSED_TIME_SECONDS));

        //ramp the smoke anim speed up and down when the spacebar is held
        if (isAnimStats_)
        {
            if (smokeAnimUPtr_->TimePerFrame() >= 0.01f)
            {
                smokeAnimUPtr_->TimePerFrameAdj(ELAPSED_TIME_SECONDS * 0.02f * -1.0f);
            }
        }
        else
        {
            if (smokeAnimUPtr_->TimePerFrame() <= 0.05f)
            {
                smokeAnimUPtr_->TimePerFrameAdj(ELAPSED_TIME_SECONDS * 0.01f);
            }
        }

        ProduceAnimatingDigits(ELAPSED_TIME_SECONDS);

        //draw the stat modifier text as soon as the number animations stop
        if ((false == AreAnyAnimNumStillMoving()) &&
            (false == willDrawStatModText_)       &&
            (false == AreAnyStatsIgnored()))
        {
            willDrawStatModText_ = true;
            HandleChangedStatModifiers("after anim stopped");
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
        }
        else
        {
            if (AreAnyAnimNumStillMoving())
            {
                willDrawStatModText_ = false;
            }
        }

        HandleStuckAnims(ELAPSED_TIME_SECONDS);
    }


    void CharacterStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        bottomSymbol_.Draw(target, STATES);

        Stage::Draw(target, STATES);

        //Don't draw statBox_.  That is in entitySSet_.

        target.draw(strLabelTextRegion_, STATES);
        target.draw(accLabelTextRegion_, STATES);
        target.draw(chaLabelTextRegion_, STATES);
        target.draw(lckLabelTextRegion_, STATES);
        target.draw(spdLabelTextRegion_, STATES);
        target.draw(intLabelTextRegion_, STATES);

        //draw lines between stats
        sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine1PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine1PosTop_)) };
        sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine2PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine2PosTop_)) };
        sf::Vertex line3[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine3PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine3PosTop_)) };
        sf::Vertex line4[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine4PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine4PosTop_)) };
        sf::Vertex line5[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine5PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine5PosTop_)) };
        sf::Vertex line6[] = { sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 0.0f, statsLine6PosTop_)), sf::Vertex(sf::Vector2f(STATS_POS_LEFT_ + 10.0f + statsLineLength_, statsLine6PosTop_)) };
        //
        const sf::Color LINE_TO_COLOR(255, 255, 255, 20);
        line1[1].color = LINE_TO_COLOR;
        line2[1].color = LINE_TO_COLOR;
        line3[1].color = LINE_TO_COLOR;
        line4[1].color = LINE_TO_COLOR;
        line5[1].color = LINE_TO_COLOR;
        line6[1].color = LINE_TO_COLOR;
        //
        target.draw(line1, 2, sf::Lines);
        target.draw(line2, 2, sf::Lines);
        target.draw(line3, 2, sf::Lines);
        target.draw(line4, 2, sf::Lines);
        target.draw(line5, 2, sf::Lines);
        target.draw(line6, 2, sf::Lines);

        //draw stat modifier texts
        const std::size_t NUM_STAT_MODS(statModifierTextVec_.size());
        for (std::size_t i(0); i < NUM_STAT_MODS; ++i)
            target.draw(statModifierTextVec_[i], STATES);

        //draw animating digits
        const std::size_t NUM_DIGITS(animStatsSVec_.size());
        for (std::size_t i(0); i < NUM_DIGITS; ++i)
            if (false == animStatsSVec_[i]->IgnoreMe())
                target.draw( * animStatsSVec_[i], STATES);

        for (std::size_t i(0); i < stats::stat::Count; ++i)
            if (false == fixedStatsSVec_[i]->IgnoreMe())
                target.draw( * fixedStatsSVec_[i], STATES);

        /*
        //draw the rectangle defining the smoke anim drift boundary
        const sf::FloatRect R(smokeAnimmaxX_,
                              0.0f,
                              (sfml_util::Display::Instance()->GetWinWidth() - 150.0f) - smokeAnimmaxX_,
                              400.0f);
        sfml_util::DrawRectangle(target, STATES, R, sf::Color::Red);
        */
    }


    bool CharacterStage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        const bool RESULT( Stage::KeyPress(KE) );

        if ((KE.code == sf::Keyboard::Space) &&
            (false == isAnimStats_) &&
            (false == nameTextEntryBoxSPtr_->HasFocus()))
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Wind).PlayRandom();
            animStatsTimeCounterSec_ = 0.0f;
            animStatsDelayPerSec_ = 0.01f;//any fraction of a second will work here
            isAnimStats_ = true;
            isWaitingForStats_ = false;
            UndoAndClearStatModifierChanges();
            return true;
        }
        else
            return RESULT;
    }


    bool CharacterStage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        if (Stage::KeyRelease(KE))
        {
            return true;
        }

        if (nameTextEntryBoxSPtr_->HasFocus())
        {
            if ((KE.code == sf::Keyboard::Return) || (KE.code == sf::Keyboard::Tab))
            {
                nameTextEntryBoxSPtr_->SetHasFocus(false);
                statsBoxUPtr_->SetHasFocus(true);
                Stage::SetFocus(statsBoxUPtr_.get());
            }

            return true;
        }

        if (KE.code == sf::Keyboard::Space)
        {
            isAnimStats_ = false;
            isWaitingForStats_ = true;
            return true;
        }
        else
        {
            return HandleMenuNavigationKeyRelease(KE);
        }
    }


    void CharacterStage::UndoAndClearStatModifierChanges()
    {
        //undo modifier changes to statSetBase
        for (std::size_t s(0); s < stats::stat::Count; ++s)
        {
            const stats::stat::Enum NEXT_STAT_ENUM(static_cast<stats::stat::Enum>(s));

            const std::size_t NUM_MODIFIERS(statModifierTextVec_.size());

            for (std::size_t m(0); m < NUM_MODIFIERS; ++m)
            {
                if (NEXT_STAT_ENUM == statModifierTextVec_[m].stat)
                {
                    statSetBase_.Get(NEXT_STAT_ENUM).ModifyNormal(-1 * statModifierTextVec_[m].value);
                    statSetBase_.Get(NEXT_STAT_ENUM).ResetCurrentAndActualToNormal();
                }
            }
        }

        //eliminate all modifiers
        statModifierTextVec_.clear();
    }


    void CharacterStage::SetVisibleStatsToStatSetBase()
    {
        for (std::size_t i(0); i < stats::stat::Count; ++i)
        {
            const stats::stat::Enum NEXT_STAT_ENUM(static_cast<stats::stat::Enum>(i));
            const stats::Stat_t     NEW_VAL       (statSetBase_.Get(NEXT_STAT_ENUM).Normal());

            //prevent visible stats from showing negative numbers, but keep them in statSetBase_
            if (NEW_VAL < 0)
                fixedStatsSVec_[i]->ValueSet(0);
            else
                fixedStatsSVec_[i]->ValueSet(NEW_VAL);

            fixedStatsSVec_[i]->CreateNewTextRegion();
        }
    }


    void CharacterStage::HandleChangedStatModifiers(const std::string &)
    {
        //undo race and role changes to base and eliminate all stat modifiers
        // -already done before this is called

        CreateStatModifers();
        ApplyStatModifiersToStatSetBase();
        SetVisibleStatsToStatSetBase();
        SetupAttrDescriptionBox();
    }


    void CharacterStage::CreateStatModifers()
    {
        statModifierTextVec_.clear();

        const creature::race::Enum RACE( static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()) );

        statSetRace_ = creature::RaceStatModifier::Get(RACE);

        const float HORIZ_OFFSET(sfml_util::MapByRes(90.0f, 150.0f));
        const float VERT_OFFSET(5.0f);

        const std::string RACE_NAME_ABBR(creature::race::Abbr(RACE));

        std::vector<stats::stat::Enum> preExistingStatVec;

        for (std::size_t i(0); i<stats::stat::Count; ++i)
        {
            const stats::stat::Enum NEXT_STAT_ENUM(static_cast<stats::stat::Enum>(i));
            const stats::Stat_t NEXT_STAT_VAL(statSetRace_.Get(NEXT_STAT_ENUM).Normal());

            if ((NEXT_STAT_VAL != 0) &&
                (false == fixedStatsSVec_[NEXT_STAT_ENUM]->IgnoreMe()) &&
                (false == fixedStatsSVec_[NEXT_STAT_ENUM]->IsHeldDown()) &&
                (initialRollCounter_ >= 6))
            {
                preExistingStatVec.push_back(NEXT_STAT_ENUM);
                statModifierTextVec_.push_back( StatModText(NEXT_STAT_ENUM,
                                                            RACE_NAME_ABBR,
                                                            NEXT_STAT_VAL,
                                                            statsFirstNumPosLeft_ + HORIZ_OFFSET,
                                                            GetAttributeNumPosTop(NEXT_STAT_ENUM) + VERT_OFFSET) );
            }
        }

        const creature::role::Enum ROLE_ENUM( ((static_cast<creature::race::Enum>(raceRadioButtonSPtr_->GetSelectedNumber()) == creature::race::Wolfen) ? creature::role::Wolfen : static_cast<creature::role::Enum>(roleRadioButtonSPtr_->GetSelectedNumber())) );
        const std::string ROLE_NAME_ABBR( creature::role::Abbr(ROLE_ENUM));

        statSetRole_ = creature::RoleStatModifier::Get(ROLE_ENUM);

        for (std::size_t i(0); i<stats::stat::Count; ++i)
        {
            const stats::stat::Enum NEXT_STAT_ENUM(static_cast<stats::stat::Enum>(i));
            const stats::Stat_t NEXT_STAT_VAL(statSetRole_.Get(NEXT_STAT_ENUM).Normal());

            if ((NEXT_STAT_VAL != 0) &&
                (false == fixedStatsSVec_[NEXT_STAT_ENUM]->IgnoreMe()) &&
                (false == fixedStatsSVec_[NEXT_STAT_ENUM]->IsHeldDown()) &&
                (initialRollCounter_ >= 6))
            {
                float extraHorizOffset(0.0f);

                if (std::find(preExistingStatVec.begin(), preExistingStatVec.end(), NEXT_STAT_ENUM) == preExistingStatVec.end())
                    extraHorizOffset = 0.0f;
                else
                    extraHorizOffset = sfml_util::MapByRes(60.0f, 200.0f);

                statModifierTextVec_.push_back( StatModText(NEXT_STAT_ENUM,
                                                            ROLE_NAME_ABBR,
                                                            NEXT_STAT_VAL,
                                                            statsFirstNumPosLeft_ + HORIZ_OFFSET + extraHorizOffset,
                                                            GetAttributeNumPosTop(NEXT_STAT_ENUM) + VERT_OFFSET) );
            }
        }
    }


    void CharacterStage::ApplyStatModifiersToStatSetBase()
    {
        const std::size_t NUM_MODIFIERS(statModifierTextVec_.size());
        for (std::size_t i(0); i < NUM_MODIFIERS; ++i)
        {
            statSetBase_.Get(statModifierTextVec_[i].stat).ModifyNormal(statModifierTextVec_[i].value);
        }
    }


    stats::stat::Enum CharacterStage::GetHeldDownStat() const
    {
        if (fixedStatsSVec_[stats::stat::Strength]->IsHeldDown()) return stats::stat::Strength;
        if (fixedStatsSVec_[stats::stat::Accuracy]->IsHeldDown()) return stats::stat::Accuracy;
        if (fixedStatsSVec_[stats::stat::Charm]->IsHeldDown()) return stats::stat::Charm;
        if (fixedStatsSVec_[stats::stat::Luck]->IsHeldDown()) return stats::stat::Luck;
        if (fixedStatsSVec_[stats::stat::Speed]->IsHeldDown()) return stats::stat::Speed;
        if (fixedStatsSVec_[stats::stat::Intelligence]->IsHeldDown()) return stats::stat::Intelligence;

        return stats::stat::Count;
    }


    stats::stat::Enum CharacterStage::GetStatAbove(const stats::stat::Enum STAT) const
    {
        if (STAT == stats::stat::Accuracy)     return stats::stat::Strength;
        if (STAT == stats::stat::Charm)        return stats::stat::Accuracy;
        if (STAT == stats::stat::Luck)         return stats::stat::Charm;
        if (STAT == stats::stat::Speed)        return stats::stat::Luck;
        if (STAT == stats::stat::Intelligence) return stats::stat::Speed;

        return stats::stat::Count;
    }


    stats::stat::Enum CharacterStage::GetStatBelow(const stats::stat::Enum STAT) const
    {
        if (STAT == stats::stat::Strength)  return stats::stat::Accuracy;
        if (STAT == stats::stat::Accuracy)  return stats::stat::Charm;
        if (STAT == stats::stat::Charm)     return stats::stat::Luck;
        if (STAT == stats::stat::Luck)      return stats::stat::Speed;
        if (STAT == stats::stat::Speed)     return stats::stat::Intelligence;

        return stats::stat::Count;
    }


    float CharacterStage::GetStatPosTop(const stats::stat::Enum STAT) const
    {
        switch (STAT)
        {
            case stats::stat::Strength:     { return statsStrPosTop_; }
            case stats::stat::Accuracy:     { return statsAccPosTop_; }
            case stats::stat::Charm:        { return statsChaPosTop_; }
            case stats::stat::Luck:         { return statsLckPosTop_; }
            case stats::stat::Speed:        { return statsSpdPosTop_; }
            case stats::stat::Intelligence: { return statsIntPosTop_; }
            case stats::stat::Count:
            default:                        { return -1.0f; }
        }
    }


    sfml_util::gui::IGuiEntityPtr_t CharacterStage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (AreAnyAnimNumStillMoving())
        {
            return nullptr;
        }

        //process MouseUp() on all other entitys
        auto entityWithFocusPtr{ Stage::UpdateMouseUp(MOUSE_POS_V) };

        //remove animations that are finished from the vector
        animStatsSVec_.clear();

        //snap to closest position when drag stops
        const stats::stat::Enum HELD_DOWN_STAT( GetHeldDownStat() );
        if (HELD_DOWN_STAT != stats::stat::Count)
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            fixedStatsSVec_[HELD_DOWN_STAT]->SetPosY(GetStatPosTop(HELD_DOWN_STAT));
        }

        dragStartY_ = -1.0f;//anything less than zero here will work

        //clear isHeldDown status from all stat anim objects, and see if any were
        bool wereAnyHeldDown(false);
        for (std::size_t i(0); i < stats::stat::Count; ++i)
        {
            if (fixedStatsSVec_[i]->IsHeldDown())
            {
                wereAnyHeldDown = true;
            }

            fixedStatsSVec_[i]->MouseUp();
        }

        if (wereAnyHeldDown)
        {
            UndoAndClearStatModifierChanges();
            HandleChangedStatModifiers("after mouse up");
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Thock).PlayRandom();
        }

        return entityWithFocusPtr;
    }


    void CharacterStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (false == AreAnyAnimNumStillMoving())
            Stage::UpdateMouseDown(MOUSE_POS_V);

        if ((initialRollCounter_ >= 6) && (false == AreAnyAnimNumStillMoving()))
        {
            bool isNumberHeldDown(false);
            for (std::size_t i(0); i < stats::stat::Count; ++i)
                if (fixedStatsSVec_[i]->MouseDown(MOUSE_POS_V.x, MOUSE_POS_V.y))
                    isNumberHeldDown = true;

            //you can hold down the mouse button on a stat to see all stats at their base values
            if (isNumberHeldDown)
            {
                UndoAndClearStatModifierChanges();
                SetVisibleStatsToStatSetBase();
            }
        }
    }


    void CharacterStage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        Stage::UpdateMousePos(MOUSE_POS_V);
        HandleAttributeDragging(MOUSE_POS_V);
    }


    bool CharacterStage::AreAnyAnimNumStillMoving() const
    {
        const std::size_t NUM_ANIM_NUMS( animStatsSVec_.size() );
        for (std::size_t i(0); i < NUM_ANIM_NUMS; ++i)
            if ((false == animStatsSVec_[i]->IgnoreMe()) && (false == animStatsSVec_[i]->IsDoneMoving()))
                return true;

        const std::size_t NUM_FIXED_NUMS( fixedStatsSVec_.size() );
        for(std::size_t i(0); i<NUM_FIXED_NUMS; ++i)
            if ((false == fixedStatsSVec_[i]->IgnoreMe()) && (false == fixedStatsSVec_[i]->IsDoneMoving()))
                return true;

        return false;
    }


    bool CharacterStage::AreAnyStatsIgnored() const
    {
        for (std::size_t i(0); i < stats::stat::Count; ++i)
            if (fixedStatsSVec_[i]->IgnoreMe())
                return true;

        return false;
    }


    void CharacterStage::HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V)
    {
        //check there is a number being held down at all
        const stats::stat::Enum STAT_HELD(GetHeldDownStat());
        if (STAT_HELD == stats::stat::Count)
            return;

        //can't drag if numbers are still animating
        if (isAnimStats_)
            return;

        if (AreAnyAnimNumStillMoving())
            return;

        //can't drag without a full set of attributes
        if (AreAnyStatsIgnored())
            return;

        //can't drag past the edges of the numbers box
        const float NEW_POS_Y(MOUSE_POS_V.y - 10.0f);
        if ((NEW_POS_Y > statsIntPosTop_) || (NEW_POS_Y < statsStrPosTop_))
            return;

        if (dragStartY_ < 0.0f)
            dragStartY_ = fixedStatsSVec_[STAT_HELD]->GetPos().y;

        const float DIFF(dragStartY_ - NEW_POS_Y);
        const float DIFF_MAX(statsLineVertPosDiff_ - (statsLineVertPosDiff_ / 3.0f));
        const bool  IS_MOVING_UP(DIFF > 0.0f);

        fixedStatsSVec_[STAT_HELD]->SetPosY(NEW_POS_Y);

        if (IS_MOVING_UP)
        {
            const stats::stat::Enum STAT_ABOVE(GetStatAbove(STAT_HELD));
            if ((STAT_ABOVE != stats::stat::Count) && (DIFF > DIFF_MAX))
            {
                dragStartY_ -= statsLineVertPosDiff_;
                fixedStatsSVec_[STAT_ABOVE]->SetPosY(fixedStatsSVec_[STAT_ABOVE]->GetPos().y + statsLineVertPosDiff_);

                //track the closest stat enum
                closestDragStat_ = STAT_ABOVE;

                SwapAttributes(STAT_HELD, STAT_ABOVE);
            }
        }
        else
        {
            const stats::stat::Enum STAT_BELOW(GetStatBelow(STAT_HELD));
            if ((STAT_BELOW != stats::stat::Count) && (fabs(DIFF) > DIFF_MAX))
            {
                dragStartY_ += statsLineVertPosDiff_;
                fixedStatsSVec_[STAT_BELOW]->SetPosY(fixedStatsSVec_[STAT_BELOW]->GetPos().y - statsLineVertPosDiff_);

                //track the closest stat enum
                closestDragStat_ = STAT_BELOW;

                SwapAttributes(STAT_HELD, STAT_BELOW);
            }
        }
    }


    void CharacterStage::ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS)
    {
        if ((false == isAnimStats_) && (false == isWaitingForStats_))
            return;

        if (isAnimStats_)
            animStatsTimeCounterSec_ += ELAPSED_TIME_SECONDS;

        if ((isAnimStats_) && (animStatsTimeCounterSec_ > animStatsDelayPerSec_))
        {
            //reset timer to random value
            animStatsTimeCounterSec_ = 0.0f;
            animStatsDelayPerSec_ = misc::random::Float(0.05f, 0.25f);

            sfml_util::gui::TextInfo textInfo(" ", sfml_util::FontManager::Instance()->Font_NumbersDefault1(), 40, sf::Color::White, sfml_util::Justified::Left);
            const stats::Stat_t NEXT_VAL(misc::random::Int(stats::Stat::VAL_MIN_ + 1, stats::Stat::VAL_MAX_INITIAL_));

            int numToUse(misc::random::Int(1, 6));
            if (initialRollCounter_ <= 6)
                numToUse = initialRollCounter_++;

            const float SMOKE_ANIM_MID_X(smokeAnimUPtr_->GetEntityPos().x + (smokeAnimUPtr_->GetEntityRegion().width * 0.5f) - 10.0f);
            const float SMOKE_ANIM_MID_Y(smokeAnimUPtr_->GetEntityPos().y + (smokeAnimUPtr_->GetEntityRegion().height * 0.5f) - 10.0f);
            const float ANIM_NUM_TARGET_X(statsFirstNumPosLeft_ + ((NEXT_VAL < 10) ? 10.0f : 0.0f) - 10.0f);

            switch (numToUse)
            {
            case 1:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Strength,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsStrPosTop_,
                                                      textInfo);
                animStatsSVec_.push_back( ansp );

                AnimNumSPtr_t toFadeSPtr(fixedStatsSVec_[stats::stat::Strength]);
                toFadeSPtr->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFadeSPtr(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFadeSPtr->CreateNewTextRegion();
                holdFadeSPtr->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Strength] = holdFadeSPtr;
                statSetBase_.Get(stats::stat::Strength).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFadeSPtr);
                break;
            }

            case 2:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Accuracy,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsAccPosTop_, textInfo);

                animStatsSVec_.push_back(ansp);

                AnimNumSPtr_t toFade(fixedStatsSVec_[stats::stat::Accuracy]);
                toFade->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFade->CreateNewTextRegion();
                holdFade->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Accuracy] = holdFade;
                statSetBase_.Get(stats::stat::Accuracy).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFade);
                break;
            }

            case 3:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Charm,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsChaPosTop_,
                                                      textInfo);

                animStatsSVec_.push_back(ansp);

                AnimNumSPtr_t toFade(fixedStatsSVec_[stats::stat::Charm]);
                toFade->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFade->CreateNewTextRegion();
                holdFade->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Charm] = holdFade;
                statSetBase_.Get(stats::stat::Charm).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFade);
                break;
            }

            case 4:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Luck,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsLckPosTop_,
                                                      textInfo);

                animStatsSVec_.push_back(ansp);

                AnimNumSPtr_t toFade(fixedStatsSVec_[stats::stat::Luck]);
                toFade->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFade->CreateNewTextRegion();
                holdFade->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Luck] = holdFade;
                statSetBase_.Get(stats::stat::Luck).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFade);
                break;
            }

            case 5:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Speed,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsSpdPosTop_,
                                                      textInfo);

                animStatsSVec_.push_back(ansp);

                AnimNumSPtr_t toFade(fixedStatsSVec_[stats::stat::Speed]);
                toFade->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFade->CreateNewTextRegion();
                holdFade->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Speed] = holdFade;
                statSetBase_.Get(stats::stat::Speed).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFade);
                break;
            }

            case 6:
            default:
            {
                auto ansp = std::make_shared<AnimNum>(NEXT_VAL,
                                                      stats::stat::Intelligence,
                                                      SMOKE_ANIM_MID_X,
                                                      SMOKE_ANIM_MID_Y,
                                                      ANIM_NUM_TARGET_X,
                                                      statsIntPosTop_,
                                                      textInfo);

                animStatsSVec_.push_back(ansp);

                AnimNumSPtr_t toFade(fixedStatsSVec_[stats::stat::Intelligence]);
                toFade->WillFadeSet(true);
                //
                AnimNumSPtr_t holdFade(animStatsSVec_[animStatsSVec_.size() - 1]);
                holdFade->CreateNewTextRegion();
                holdFade->WillFadeSet(false);
                //
                fixedStatsSVec_[stats::stat::Intelligence] = holdFade;
                statSetBase_.Get(stats::stat::Intelligence).ResetAll(NEXT_VAL);
                animStatsSVec_.push_back(toFade);
                break;
            }
            }//end switch()
        }

        //update each active animating fixed number, and check for any still moving
        bool areAnyFixedStillMoving(false);
        for (std::size_t f(0); f < stats::stat::Count; ++f)
        {
            fixedStatsSVec_[f]->UpdateTime(ELAPSED_TIME_SECONDS);

            if (false == fixedStatsSVec_[f]->IsDoneMoving())
                areAnyFixedStillMoving = true; //don't break here because UpdateTime() needs to be called on each
        }

        //update each active animating number
        const std::size_t NUM_DIGITS(animStatsSVec_.size());
        for (std::size_t i(0); i < NUM_DIGITS; ++i)
        {
            if (false == animStatsSVec_[i]->IsDoneFading())
            {
                animStatsSVec_[i]->UpdateTime(ELAPSED_TIME_SECONDS);
            }
        }

        if (areAnyFixedStillMoving)
            isWaitingForStats_ = true;
    }


    void CharacterStage::SwapAttributes(const stats::stat::Enum A, const stats::stat::Enum B)
    {
        //swap the anim objects
        AnimNumSPtr_t tempSPtr(fixedStatsSVec_[static_cast<std::size_t>(B)]);
        fixedStatsSVec_[static_cast<std::size_t>(B)] = fixedStatsSVec_[static_cast<std::size_t>(A)];
        fixedStatsSVec_[static_cast<std::size_t>(A)] = tempSPtr;

        //swap the underlying stat values
        const stats::Stat_t TEMP_STAT(statSetBase_.Get(B).Normal());
        statSetBase_.Get(B).ResetAll(statSetBase_.Get(A).Normal());
        statSetBase_.Get(A).ResetAll(TEMP_STAT);

        sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOn).PlayRandom();
    }


    void CharacterStage::HandleStuckAnims(const float ELAPSED_TIME_SEC)
    {
        std::vector<std::size_t> stuckAnimIndexVec;
        const std::size_t NUM_ANIMS(animStatsSVec_.size());
        for (std::size_t i(0); i < NUM_ANIMS; ++i)
            if (animStatsSVec_[i]->UpdateTimer(ELAPSED_TIME_SEC))
                stuckAnimIndexVec.push_back(i);

        const std::size_t NUM_STUCK_ANIMS(stuckAnimIndexVec.size());
        for (std::size_t i(0); i < NUM_STUCK_ANIMS; ++i)
        {
            const typename AnimNumSVec_t::iterator ITR_TO_ERASE(animStatsSVec_.begin() + AnimNumSVec_t::iterator::difference_type(i));
            //M_HP_LOG("CharacterStage::HandleStuckAnims() destroying a stuck anim at index=" << i << ", value=" << (*ITR_TO_ERASE)->Value());
            animStatsSVec_.erase(ITR_TO_ERASE);
        }
    }


    bool CharacterStage::HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            HandleCallback_BackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            saveButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            HandleCallback_SaveButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::N)
        {
            nextButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            HandleCallback_NextButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::H)
        {
            helpButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Switch).PlayRandom();
            HandleCallback_HelpButton();
            return true;
        }
        else
            return false;
    }


    void CharacterStage::ResetForNewCharacterCreation()
    {
        initialRollCounter_ = 0;
        raceRadioButtonSPtr_->SetSelectNumber(0);
        roleRadioButtonSPtr_->SetSelectNumber(0);
        nameTextEntryBoxSPtr_->SetText("");
        statModifierTextVec_.clear();

        for (std::size_t i(0); i < stats::stat::Count; ++i)
            fixedStatsSVec_[i]->SetIgnoreMe();

        SetupAttrDescriptionBox();
    }

}
}
