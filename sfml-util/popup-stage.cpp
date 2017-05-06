//
// popup-stage.cpp
//
#include "popup-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"
#include "sfml-util/text-rendering.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "game/loop-manager.hpp"
#include "game/log-macros.hpp"
#include "game/creature/name-info.hpp"
#include "game/creature/creature.hpp"

#include "utilz/random.hpp"

#include <sstream>
#include <random>


namespace sfml_util
{

    const float PopupStage::IMAGE_SLIDER_SPEED_                     { 4.0f };
    const int   PopupStage::NUMBER_SELECT_INVALID_                  { -1 };//any negative number will work here
    const float PopupStage::BEFORE_FADE_STARTS_DELAY_SEC_           { 2.0f };
    const float PopupStage::SPELLBOOK_POPUP_BACKGROUND_WIDTH_RATIO_ { 0.75f };


    PopupStage::PopupStage(const game::PopupInfo & POPUP_INFO,
                           const sf::FloatRect &         REGION,
                           const sf::FloatRect &         INNER_REGION,
                           const TextureSPtr_t &         TEXTURE_SPTR,
                           const sf::Sprite &            ACCENT_SPRITE,
                           const TextureSPtr_t &         ACCENT_TEXTURE_SPTR)
    :
        Stage                  (std::string(POPUP_INFO.Name()).append("_PopupStage"), REGION),
        POPUP_INFO_            (POPUP_INFO),
        backgroundSprite_      ( * TEXTURE_SPTR),
        backgroundTextureSPtr_ (TEXTURE_SPTR),
        innerRegion_           (INNER_REGION),
        textRegionSPtr_        (),
        textRegion_            (),
        elapsedTimeCounter_    (0.0f),
        secondCounter_         (((POPUP_INFO.Type() == game::Popup::ResolutionChange) ? 10 : 0)),//resolution change confirmation timer is six seconds
        box_                   ("PopupWindow's", gui::box::Info()),
        gradient_              (),
        accentSprite_          (ACCENT_SPRITE),
        accentTextureSPtr_     (ACCENT_TEXTURE_SPTR),
        selectPopupButtonSPtr_ (),
        sliderbarSPtr_         (),
        sliderbarPosTop_       (0.0f),
        willSliderbarUpdate_   (true),
        willTextBoxUpdate_     (true),
        infoTextRegionSPtr_    (),
        textEntryBoxSPtr_      (),
        isImageProcAllowed_    (false),
        isInitialAnimation_    (true),
        willShowImageCount_    (false),
        imageSpriteCurr_       (),
        imageSpritePrev_       (),
        areImagesMoving_       (false),
        areImagesMovingLeft_   (false),
        imagesRect_            (),
        imageWrnTextRegionSPtr_(),
        imageNumTextRegionSPtr_(),
        imageIndex_            (0),
        imageIndexLastSoundOn_ (0),
        imageIndexLastSoundOff_(0),
        imageCurrTargetScale_  (1.0f),
        imagePrevStartScale_   (1.0f),
        imagePrevStartPosX_    (0.0f),
        imageCurrTravelDist_   (0.0f),
        imagePrevTravelDist_   (0.0f),
        imageMoveQueue_        (),
        imageSlider_           (IMAGE_SLIDER_SPEED_),
        beforeFadeTimerSec_    (0.0f),
        fadeAlpha_             (0.0f),
        spellbookState_        (SpellbookState::FadeingIn),
        playerTextureSPtr_     (),
        playerSprite_          ()
    {}


    PopupStage::PopupStage(const game::PopupInfo & POPUP_INFO,
                           const sf::FloatRect &         REGION,
                           const sf::FloatRect &         INNER_REGION,
                           const sf::Sprite &            ACCENT_SPRITE,
                           const TextureSPtr_t &         ACCENT_TEXTURE_SPTR)
    :
        Stage                  (std::string(POPUP_INFO.Name()).append("_PopupStage"), REGION),
        POPUP_INFO_            (POPUP_INFO),
        backgroundSprite_      (),
        backgroundTextureSPtr_     (),
        innerRegion_           (INNER_REGION),
        textRegionSPtr_        (),
        textRegion_            (),
        elapsedTimeCounter_    (0.0f),
        secondCounter_         (((POPUP_INFO.Type() == game::Popup::ResolutionChange) ? 10 : 0)),//resolution change confirmation timer is six seconds
        box_                   ("PopupWindow's", POPUP_INFO.BoxInfo()),
        gradient_              (),
        accentSprite_          (ACCENT_SPRITE),
        accentTextureSPtr_     (ACCENT_TEXTURE_SPTR),
        selectPopupButtonSPtr_ (),
        sliderbarSPtr_         (),
        sliderbarPosTop_       (0.0f),
        willSliderbarUpdate_   (true),
        willTextBoxUpdate_     (true),
        infoTextRegionSPtr_    (),
        textEntryBoxSPtr_      (),
        isImageProcAllowed_    (false),
        isInitialAnimation_    (true),
        willShowImageCount_    (false),
        imageSpriteCurr_       (),
        imageSpritePrev_       (),
        areImagesMoving_       (false),
        areImagesMovingLeft_   (false),
        imagesRect_            (),
        imageWrnTextRegionSPtr_(),
        imageNumTextRegionSPtr_(),
        imageIndex_            (0),
        imageIndexLastSoundOn_ (0),
        imageIndexLastSoundOff_(0),
        imageCurrTargetScale_  (1.0f),
        imagePrevStartScale_   (1.0f),
        imagePrevStartPosX_    (0.0f),
        imageCurrTravelDist_   (0.0f),
        imagePrevTravelDist_   (0.0f),
        imageMoveQueue_        (),
        imageSlider_           (IMAGE_SLIDER_SPEED_),
        beforeFadeTimerSec_    (0.0f),
        fadeAlpha_             (0.0f),
        spellbookState_        (SpellbookState::FadeingIn),
        playerTextureSPtr_     (),
        playerSprite_          ()
    {}


    PopupStage::~PopupStage()
    {}


    bool PopupStage::HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if (POPUP_INFO_.Type() == game::Popup::ImageSelection)
        {
            if (isImageProcAllowed_)
            {
                const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f / static_cast<float>(POPUP_INFO_.Images().size()));
                std::size_t index(static_cast<std::size_t>(sliderbarSPtr_->GetCurrentValue() / SINGLE_IMAGE_SLIDER_WIDTH_RATIO));

                if (index >= POPUP_INFO_.Images().size())
                    index = POPUP_INFO_.Images().size() - 1;

                imageMoveQueue_.push(index);
            }

            return true;
        }
        else if ((POPUP_INFO_.Type() == game::Popup::NumberSelection))
        {
            const float CURR_RATIO(PACKAGE.PTR_->GetCurrentValue());
            const std::size_t CURR_VAL(POPUP_INFO_.NumberSelMin() + static_cast<std::size_t>(CURR_RATIO * static_cast<float>(POPUP_INFO_.NumberSelMax() - POPUP_INFO_.NumberSelMin())));

            selectPopupButtonSPtr_->SetSelection(CURR_VAL);

            std::ostringstream minNumSS;
            minNumSS << POPUP_INFO_.NumberSelMin();
            std::string numStr(minNumSS.str());

            try
            {
                numStr = boost::lexical_cast<std::string>(CURR_VAL);
            }
            catch (...)
            {
                numStr = minNumSS.str();
            }

            if ((textEntryBoxSPtr_.get() != nullptr) && willSliderbarUpdate_)
            {
                willTextBoxUpdate_ = false;
                textEntryBoxSPtr_->SetText(numStr);
                willTextBoxUpdate_ = true;
                SetupInfoText("");
            }
        }

        return false;
    }


    bool PopupStage::HandleCallback(const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &)
    {
        if (POPUP_INFO_.Type() == game::Popup::NumberSelection)
        {
            const int NUM(GetSelectNumber());
            selectPopupButtonSPtr_->SetSelection(NUM);

            if (willTextBoxUpdate_)
                ProcessSelectNumber();

            return true;
        }
        else
            return false;
    }


    void PopupStage::Setup()
    {
        //sound effect
        SoundManager::Instance()->StaticSounds_Prompt()->Play(POPUP_INFO_.SoundEffect());

        //darken the box gold bars a bit
        box_.SetEntityColors(sfml_util::gui::ColorSet(sf::Color(200,200,200)));

        if (POPUP_INFO_.Image() == sfml_util::PopupImage::Spellbook)
        {
            auto const BACKGROUND_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() * SPELLBOOK_POPUP_BACKGROUND_WIDTH_RATIO_ };
            auto const BACKGROUND_HEIGHT{ (static_cast<float>(backgroundTextureSPtr_->getSize().y) * BACKGROUND_WIDTH) / static_cast<float>(backgroundTextureSPtr_->getSize().x) };

            sf::FloatRect r;
            r.left = ((sfml_util::Display::Instance()->GetWinWidth() - BACKGROUND_WIDTH) * 0.5f);
            r.top = ((sfml_util::Display::Instance()->GetWinHeight() - BACKGROUND_HEIGHT) * 0.5f);
            r.width = BACKGROUND_WIDTH;
            r.height = BACKGROUND_HEIGHT;

            StageRegionSet( r );
        }
        else
        {
            backgroundSprite_.setScale(POPUP_INFO_.ImageScale(), POPUP_INFO_.ImageScale());

            //set the stage region to fit the newly sized background paper/whatever image
            const float BACKGROUND_WIDTH(backgroundSprite_.getGlobalBounds().width);
            const float BACKGROUND_HEIGHT(backgroundSprite_.getGlobalBounds().height);
            const float BACKGROUND_POS_LEFT((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (BACKGROUND_WIDTH * 0.5f));
            const float BACKGROUND_POS_TOP((sfml_util::Display::Instance()->GetWinHeight() * 0.5f) - (BACKGROUND_HEIGHT * 0.5f));
            StageRegionSet(sf::FloatRect(BACKGROUND_POS_LEFT, BACKGROUND_POS_TOP, BACKGROUND_WIDTH, BACKGROUND_HEIGHT));
        }

        //positions can now accurately be based on StageRegion
        backgroundSprite_.setPosition(StageRegionLeft(), StageRegionTop());

        //add and position buttons
        const sfml_util::gui::MouseTextInfo TEMP_MOUSE_TEXT_INFO(sfml_util::gui::MouseTextInfo::Make_PopupButtonSet(game::creature::NameInfo::Instance()->LargestLetterString(), POPUP_INFO_));
        const sf::Text TEMP_TEXT_OBJ(TEMP_MOUSE_TEXT_INFO.up.text, * TEMP_MOUSE_TEXT_INFO.up.fontSPtr, TEMP_MOUSE_TEXT_INFO.up.charSize);
        const float POPUPBUTTON_TEXT_HEIGHT(TEMP_TEXT_OBJ.getGlobalBounds().height);
        const float POPUPBUTTON_TEXT_BOTTOM_MARGIN(sfml_util::MapByRes(30.0f, 90.0f));
        const float POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM((POPUP_INFO_.Image() == sfml_util::PopupImage::Custom) ? 25.0f : 0.0f);
        const float BUTTON_VERT_OFFSET(POPUPBUTTON_TEXT_HEIGHT + POPUPBUTTON_TEXT_BOTTOM_MARGIN + POPUPBUTTON_TEXT_BOTTOM_MARGIN_EXTRA_FOR_CUSTOM);
        const float BUTTON_POS_TOP((StageRegionTop() + innerRegion_.top + innerRegion_.height) - BUTTON_VERT_OFFSET);

        if (POPUP_INFO_.Buttons() & Response::Yes)
        {
            sfml_util::PopupButton_YesSPtr_t buttonYesSPtr(new sfml_util::PopupButton_Yes(POPUP_INFO_, StageRegionLeft() + innerRegion_.left + (innerRegion_.width / 4.0f) - 50.0f, BUTTON_POS_TOP));
            EntityAdd(buttonYesSPtr);
        }

        if (POPUP_INFO_.Buttons() & Response::No)
        {
            sfml_util::PopupButton_NoSPtr_t buttonNoSPtr(new sfml_util::PopupButton_No(POPUP_INFO_, StageRegionLeft() + innerRegion_.left + (2.0f * (innerRegion_.width / 4.0f)) - 40.0f, BUTTON_POS_TOP));
            EntityAdd(buttonNoSPtr);
        }

        if (POPUP_INFO_.Buttons() & Response::Cancel)
        {
            sfml_util::PopupButton_CancelSPtr_t buttonCancelSPtr(new sfml_util::PopupButton_Cancel(POPUP_INFO_, (StageRegionLeft() + innerRegion_.left + innerRegion_.width) - (innerRegion_.width / 3.0f) - 0.0f, BUTTON_POS_TOP));
            EntityAdd(buttonCancelSPtr);
        }

        if (POPUP_INFO_.Buttons() & Response::Continue)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left + (innerRegion_.width * 0.5f));
            sfml_util::PopupButton_ContinueSPtr_t buttonContinueSPtr(new sfml_util::PopupButton_Continue(POPUP_INFO_, MIDDLE - 30.0f, BUTTON_POS_TOP));

            if (POPUP_INFO_.Image() == sfml_util::PopupImage::Custom)
                buttonContinueSPtr->SetEntityPos(MIDDLE - (buttonContinueSPtr->GetEntityRegion().width * 0.5f), buttonContinueSPtr->GetEntityPos().y );

            EntityAdd(buttonContinueSPtr);
        }

        if (POPUP_INFO_.Buttons() & Response::Okay)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left + (innerRegion_.width * 0.5f));
            sfml_util::PopupButton_OkaySPtr_t buttonOkaySPtr(new sfml_util::PopupButton_Okay(POPUP_INFO_, MIDDLE - 50.0f, BUTTON_POS_TOP));

            if (POPUP_INFO_.Image() == sfml_util::PopupImage::Custom)
                buttonOkaySPtr->SetEntityPos(MIDDLE - (buttonOkaySPtr->GetEntityRegion().width * 0.5f) - 10.0f, buttonOkaySPtr->GetEntityPos().y);

            EntityAdd(buttonOkaySPtr);
        }

        if (POPUP_INFO_.Buttons() & Response::Select)
        {
            const float MIDDLE(StageRegionLeft() + innerRegion_.left + (innerRegion_.width * 0.5f));
            selectPopupButtonSPtr_.reset( new sfml_util::PopupButton_Select(POPUP_INFO_, MIDDLE - 100.0f, BUTTON_POS_TOP) );

            if (POPUP_INFO_.Image() == sfml_util::PopupImage::Custom)
                selectPopupButtonSPtr_->SetEntityPos(MIDDLE - (selectPopupButtonSPtr_->GetEntityRegion().width * 0.5f) - 10.0f, selectPopupButtonSPtr_->GetEntityPos().y);

            EntityAdd(selectPopupButtonSPtr_);
        }

        //establish text region
        //Note:  Spellbook popup has two regions, one for each page, so textRegion_ is not used on the Spellbook popup stage.
        textRegion_.left   = StageRegionLeft() + innerRegion_.left;
        textRegion_.top    = StageRegionTop() + innerRegion_.top;
        textRegion_.width  = innerRegion_.width;
        textRegion_.height = (innerRegion_.height - BUTTON_VERT_OFFSET) - 12.0f;//this magic # found by exeriment to be a good looking boundary between text and buttons

        sf::FloatRect textRegionRect(textRegion_);
        textRegionRect.height = 0.0f;

        //setup and render actual text
        //Note:  Spellbook popup has no 'typical' central text, so only " " is rendered here.
        textRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage's",
                                                              POPUP_INFO_.TextInfo(),
                                                              textRegionRect,
                                                              this) );

        if ((textRegionSPtr_->GetEntityRegion().top + textRegionSPtr_->GetEntityRegion().height) > (textRegion_.top + textRegion_.height))
        {
            textRegionRect.height = textRegion_.height;

            textRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage's",
                                                              POPUP_INFO_.TextInfo(),
                                                              textRegionRect,
                                                              this) );
        }

        //setup gradient
        const sf::FloatRect R(StageRegionLeft(),
                              textRegion_.top + textRegion_.height,
                              StageRegionWidth() - 1.0f,
                              (StageRegionHeight() - textRegion_.height) + 2.0f);

        gradient_.Setup(R, GradientInfo(sf::Color::Black, 0, sfml_util::Side::Top, sf::Color(0, 0, 0, 20)));

        //establish the accent sprite position and size
        if (POPUP_INFO_.WillAddRandImage())
        {
            //scale the accent image
            auto const SIZE_RATIO{ utilz::random::Float(0.65f, 0.85f) };
            const float SCALE_VERT((textRegion_.height * SIZE_RATIO) / accentSprite_.getLocalBounds().height);
            accentSprite_.setScale(SCALE_VERT, SCALE_VERT);

            if (accentSprite_.getGlobalBounds().width > (textRegion_.width * SIZE_RATIO))
            {
                const float SCALE_HORIZ((textRegion_.width * SIZE_RATIO) / accentSprite_.getLocalBounds().width);

                if (SCALE_HORIZ < SCALE_VERT)
                {
                    accentSprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);
                }
            }

            //always center the accent sprite image
            const float ACCENT_POS_LEFT((StageRegionLeft() + (StageRegionWidth() * 0.5f)) - (accentSprite_.getGlobalBounds().width * 0.5f));
            const float ACCENT_POS_TOP((StageRegionTop() + (StageRegionHeight() * 0.5f)) - (accentSprite_.getGlobalBounds().height * 0.5f));
            accentSprite_.setPosition(ACCENT_POS_LEFT, ACCENT_POS_TOP);
        }

        //sliderbar setup
        const float SLIDERBAR_LENGTH(textRegion_.width * 0.75f);
        const float SLIDERBAR_POS_LEFT(textRegion_.left + ((textRegion_.width - SLIDERBAR_LENGTH) * 0.5f));
        sliderbarPosTop_ = (BUTTON_POS_TOP - (POPUPBUTTON_TEXT_HEIGHT * 3.0f));
        if ((POPUP_INFO_.Type() == game::Popup::ImageSelection) || (POPUP_INFO_.Type() == game::Popup::NumberSelection))
        {
            sliderbarSPtr_ = std::make_shared<sfml_util::gui::SliderBar>("PopupStage's",
                                                                         SLIDERBAR_POS_LEFT,
                                                                         sliderbarPosTop_,
                                                                         SLIDERBAR_LENGTH,
                                                                         sfml_util::gui::SliderStyle(sfml_util::Orientation::Horiz),
                                                                         this);
            EntityAdd(sliderbarSPtr_);
        }

        if (POPUP_INFO_.Type() == game::Popup::NumberSelection)
        {
            const sfml_util::gui::TextInfo INFO_TEXT_INFO(" ",
                                                          FontManager::Instance()->Font_Default1(),
                                                          FontManager::Instance()->Size_Small(),
                                                          gui::PopupManager::Color_Font(),
                                                          Justified::Center);

            infoTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect()) );
            EntityAdd(infoTextRegionSPtr_);
            SetupInfoText("(type a number or use the slider below)");

            const float TEXTENTRY_BOX_WIDTH(textRegion_.width * 0.45f);
            const float TEXTENTRY_BOX_POS_LEFT(textRegion_.left + ((textRegion_.width - TEXTENTRY_BOX_WIDTH) * 0.5f));
            const float TEXTENTRY_BOX_POS_TOP(infoTextRegionSPtr_->GetEntityPos().y - 115.0f);//this spacer value of 115 found to look good by experiment
            const float TEXTENTRY_BOX_HEIGHT(55.0f);//this is the minimum height a textbox can be, and fits pretty well here with the font size being "large"

            const sf::FloatRect TEXTENTRY_REGION(TEXTENTRY_BOX_POS_LEFT,
                                                 TEXTENTRY_BOX_POS_TOP,
                                                 TEXTENTRY_BOX_WIDTH,
                                                 TEXTENTRY_BOX_HEIGHT);

            std::ostringstream minNumSS;
            minNumSS << POPUP_INFO_.NumberSelMin();

            const sfml_util::gui::TextInfo TEXTENTRY_TEXT_INFO(minNumSS.str(),
                                                               FontManager::Instance()->Font_Default1(),
                                                               FontManager::Instance()->Size_Large(),
                                                               sf::Color::White,
                                                               Justified::Left);

            const sfml_util::gui::BackgroundInfo TEXTENTRY_BG_INFO(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), TEXTENTRY_REGION);

            const sfml_util::gui::box::Info TEXTENTRY_BOX_INFO(true,
                                                               TEXTENTRY_REGION,
                                                               sf::Color::White,
                                                               TEXTENTRY_BG_INFO);

            textEntryBoxSPtr_.reset( new sfml_util::gui::TextEntryBox("PopupStage's",
                                                                      TEXTENTRY_REGION,
                                                                      TEXTENTRY_TEXT_INFO,
                                                                      FontManager::Color_Light(),
                                                                      TEXTENTRY_BOX_INFO,
                                                                      this) );

            textEntryBoxSPtr_->SetText(minNumSS.str());
            EntityAdd(textEntryBoxSPtr_);

            RemoveFocus();
            SetFocus(textEntryBoxSPtr_);
            textEntryBoxSPtr_->SetHasFocus(true);
        }
        else if (POPUP_INFO_.Type() == game::Popup::ImageSelection)
        {
            imagesRect_ = textRegion_;
            imagesRect_.top = textRegionSPtr_->GetEntityPos().y + sfml_util::MapByRes(70.0f, 200.0f);//added is a pad so the text does not touch the images
            imagesRect_.height = (sliderbarPosTop_ - (POPUPBUTTON_TEXT_HEIGHT * 2.0f)) - imagesRect_.top;
            isImageProcAllowed_ = ! POPUP_INFO_.Images().empty();
            imageMoveQueue_.push(0);

            if (POPUP_INFO_.Images().size() == 1)
            {
                const sfml_util::gui::TextInfo TEXT_INFO("(there is only one image)",
                                                         FontManager::Instance()->Font_Default1(),
                                                         FontManager::Instance()->Size_Small(),
                                                         gui::PopupManager::Color_Font(),
                                                         Justified::Center);

                sf::FloatRect region(textRegion_);
                region.top = sliderbarPosTop_;
                region.height = 0.0f;
                imageWrnTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage'sImageSelectionion", TEXT_INFO, region) );

                EntityAdd(imageWrnTextRegionSPtr_);
                EntityRemove(sliderbarSPtr_);
            }
        }
        else if (POPUP_INFO_.Type() == game::Popup::ImageFade)
        {
            const float IMAGE_PAD(10.0f);
            const float IMAGE_REGION_TOP(textRegionSPtr_->GetEntityRegion().top + textRegionSPtr_->GetEntityRegion().height + IMAGE_PAD);
            const float IMAGE_REGION_HEIGHT((textRegion_.top + textRegion_.height) - IMAGE_REGION_TOP);

            const sfml_util::TextureSVec_t textureSVec(POPUP_INFO_.Images());

            if (textureSVec.size() == 1)
            {
                imageSpriteCurr_.setTexture( * POPUP_INFO_.Images().at(0));
            }
            else
            {
                imageSpritePrev_.setTexture( * POPUP_INFO_.Images().at(0));
                imageSpriteCurr_.setTexture( * POPUP_INFO_.Images().at(1));
            }

            float newScale(1.0f);
            if (imageSpriteCurr_.getLocalBounds().height > IMAGE_REGION_HEIGHT)
                newScale = IMAGE_REGION_HEIGHT / imageSpriteCurr_.getLocalBounds().height;

            //assume POPUP_INFO_.Images()[0] and POPUP_INFO_.Images()[1] are the same size
            imageSpritePrev_.setScale(newScale, newScale);
            imageSpriteCurr_.setScale(newScale, newScale);

            imageSpritePrev_.setPosition((textRegion_.left + (textRegion_.width * 0.5f)) - (imageSpritePrev_.getGlobalBounds().width * 0.5f),
                                         (IMAGE_REGION_TOP + (IMAGE_REGION_HEIGHT * 0.5f)) - (imageSpritePrev_.getGlobalBounds().height * 0.5f));

            imageSpriteCurr_.setPosition((textRegion_.left + (textRegion_.width * 0.5f)) - (imageSpriteCurr_.getGlobalBounds().width * 0.5f),
                                         (IMAGE_REGION_TOP + (IMAGE_REGION_HEIGHT * 0.5f)) - (imageSpriteCurr_.getGlobalBounds().height * 0.5f));

            imageSpriteCurr_.setColor(sf::Color(255, 255, 255, 0));
        }
        else if (POPUP_INFO_.Type() == game::Popup::Spellbook)
        {
            playerTextureSPtr_ = sfml_util::gui::CreatureImageManager::Instance()->GetImage(POPUP_INFO_.CreaturePtr()->ImageFilename(), true);
        }
    }


    void PopupStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        if (POPUP_INFO_.Image() == sfml_util::PopupImage::Custom)
        {
            target.draw(box_, states);
            target.draw(gradient_, states);
        }
        else
            target.draw(backgroundSprite_, states);

        if (POPUP_INFO_.WillAddRandImage())
            target.draw(accentSprite_, states);

        textRegionSPtr_->draw(target, states);

        if (POPUP_INFO_.Type() == game::Popup::ImageSelection)
        {
            target.draw(imageSpriteCurr_, states);

            if (areImagesMoving_ && (false == isInitialAnimation_))
                target.draw(imageSpritePrev_, states);

            if (willShowImageCount_ && (imageNumTextRegionSPtr_.get() != nullptr))
                imageNumTextRegionSPtr_->draw(target, states);
        }
        else if (POPUP_INFO_.Type() == game::Popup::ImageFade)
        {
            target.draw(imageSpritePrev_, states);
            target.draw(imageSpriteCurr_, states);
        }
        else if (POPUP_INFO_.Type() == game::Popup::Spellbook)
        {
            target.draw(playerSprite_, states);
        }

        Stage::Draw(target, states);
    }


    void PopupStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (secondCounter_ > 0)
            elapsedTimeCounter_ += ELAPSED_TIME_SECONDS;

        if ((secondCounter_ > 0) && (elapsedTimeCounter_ > 1.0f))
        {
            elapsedTimeCounter_ = 0.0f;
            --secondCounter_;

            if (secondCounter_ > 0)
            {
                sfml_util::gui::TextInfo textInfo(POPUP_INFO_.TextInfo());
                std::ostringstream ss;
                ss << textInfo.text << "\n" << secondCounter_;
                textInfo.text = ss.str();

                textRegionSPtr_->Setup(textInfo,
                                       textRegion_,
                                       this);
            }
            else
            {
                game::LoopManager::Instance()->PopupWaitEnd(sfml_util::Response::No);
            }
        }

        if (areImagesMoving_ || (imageMoveQueue_.empty() == false))
        {
            float currRatio(0.0f);

            if (areImagesMoving_)
            {
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);
                areImagesMoving_ = ! imageSlider_.GetIsDone();

                if (false == areImagesMoving_)
                {
                    willShowImageCount_ = true;

                    if (imageIndexLastSoundOff_ != imageIndex_)
                        sfml_util::SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();

                    imageIndexLastSoundOff_ = imageIndex_;
                }

                if (isInitialAnimation_)
                    isInitialAnimation_ = areImagesMoving_;
            }

            if ((false == areImagesMoving_) && (imageMoveQueue_.empty() == false))
            {
                areImagesMoving_ = true;
                willShowImageCount_ = false;

                if (imageIndexLastSoundOn_ != imageMoveQueue_.front())
                    sfml_util::SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();

                imageIndexLastSoundOn_ = imageMoveQueue_.front();

                SetupSelectImage(imageMoveQueue_.front(), IMAGE_SLIDER_SPEED_ + (static_cast<float>(imageMoveQueue_.size() * 0.25f)));
                imageMoveQueue_.pop();
                currRatio = imageSlider_.Update(ELAPSED_TIME_SECONDS);

            }

            const float CURR_SCALE(imageCurrTargetScale_ * currRatio);
            imageSpriteCurr_.setScale(CURR_SCALE, CURR_SCALE);

            const float PREV_SCALE(imagePrevStartScale_ * (1.0f - currRatio));
            imageSpritePrev_.setScale(PREV_SCALE, PREV_SCALE);

            const float CURR_POS_TOP((imagesRect_.top + (imagesRect_.height * 0.5f)) - (imageSpriteCurr_.getGlobalBounds().height * 0.5f));
            const float PREV_POS_TOP((imagesRect_.top + (imagesRect_.height * 0.5f)) - (imageSpritePrev_.getGlobalBounds().height * 0.5f));

            if (willShowImageCount_)
                imageNumTextRegionSPtr_->SetEntityPos(imageNumTextRegionSPtr_->GetEntityPos().x, CURR_POS_TOP - imageNumTextRegionSPtr_->GetEntityRegion().height);

            if (areImagesMovingLeft_)
            {
                const float CURR_POS_LEFT((imagesRect_.left + imagesRect_.width) - (currRatio * imageCurrTravelDist_));
                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ - (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
            else
            {
                const float CURR_POS_LEFT(imagesRect_.left + (currRatio * imageCurrTravelDist_));
                imageSpriteCurr_.setPosition(CURR_POS_LEFT, CURR_POS_TOP);

                const float PREV_POS_LEFT(imagePrevStartPosX_ + (currRatio * imagePrevTravelDist_));
                imageSpritePrev_.setPosition(PREV_POS_LEFT, PREV_POS_TOP);
            }
        }

        if (POPUP_INFO_.Type() == game::Popup::ImageFade)
        {
            if (beforeFadeTimerSec_ < BEFORE_FADE_STARTS_DELAY_SEC_)
            {
                beforeFadeTimerSec_ += ELAPSED_TIME_SECONDS;
            }
            else
            {
                if (fadeAlpha_ < 255.0f)
                {
                    fadeAlpha_ += ELAPSED_TIME_SECONDS * POPUP_INFO_.ImageFadeSpeed();

                    if (fadeAlpha_ > 255.0f)
                        fadeAlpha_ = 255.0f;

                    imageSpriteCurr_.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(fadeAlpha_)));
                }
                else
                {
                    imageSpriteCurr_.setColor(sf::Color::White);
                }
            }
        }
    }


    bool PopupStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (POPUP_INFO_.Type() == game::Popup::ImageFade)
        {
            beforeFadeTimerSec_ = BEFORE_FADE_STARTS_DELAY_SEC_;
            fadeAlpha_ = 256.0f;//anything greater than 255.0f will work here
        }

        if ((POPUP_INFO_.Type() == game::Popup::ContentSelectionWithItem) || (POPUP_INFO_.Type() == game::Popup::ContentSelectionWithoutItem))
        {
            if ((KEY_EVENT.code == sf::Keyboard::I) && (POPUP_INFO_.Type() == game::Popup::ContentSelectionWithItem))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, game::PopupInfo::ContentNum_Item());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::C)
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, game::PopupInfo::ContentNum_Coins());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::G)
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, game::PopupInfo::ContentNum_Gems());
                return true;
            }
            else if (KEY_EVENT.code == sf::Keyboard::M)
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, game::PopupInfo::ContentNum_MeteorShards());
                return true;
            }
        }

        if ((POPUP_INFO_.Type() == game::Popup::NumberSelection) &&
            ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return)))
        {
            if (ProcessSelectNumber())
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, GetSelectNumber());
                return true;
            }
            else
                return false;
        }

        if ((KEY_EVENT.code == sf::Keyboard::Left) && (POPUP_INFO_.Images().empty() == false))
        {
            if (imageIndex_ > 0)
            {
                std::size_t index(imageIndex_);
                --index;

                const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f / static_cast<float>(POPUP_INFO_.Images().size()));
                sliderbarSPtr_->SetCurrentValue(static_cast<float>(index) * SINGLE_IMAGE_SLIDER_WIDTH_RATIO);

                imageMoveQueue_.push(index);
                return true;
            }
            else
                return false;
        }

        if ((KEY_EVENT.code == sf::Keyboard::Right) && (POPUP_INFO_.Images().empty() == false))
        {
            if (imageIndex_ < (POPUP_INFO_.Images().size() - 1))
            {
                std::size_t index(imageIndex_);
                ++index;

                const float SINGLE_IMAGE_SLIDER_WIDTH_RATIO(1.0f / static_cast<float>(POPUP_INFO_.Images().size()));
                sliderbarSPtr_->SetCurrentValue(static_cast<float>(index) * SINGLE_IMAGE_SLIDER_WIDTH_RATIO);

                //if already at the end, then make sure the sliderbar is all the way to the right
                if (index >= (POPUP_INFO_.Images().size() - 1))
                {
                    //prevent processing and adding to the imageMoveQueue_ or calling SetupSelectImage() when setting the sliderbar value here.
                    isImageProcAllowed_ = false;
                    sliderbarSPtr_->SetCurrentValue(1.0f);
                    isImageProcAllowed_ = true;
                }

                imageMoveQueue_.push(index);
                return true;
            }
            else
                return false;
        }

        if (POPUP_INFO_.Buttons() & Response::Continue)
        {
            if ((KEY_EVENT.code == sf::Keyboard::C) ||
                (KEY_EVENT.code == sf::Keyboard::Space) ||
                (KEY_EVENT.code == sf::Keyboard::Return))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
                return true;
            }
        }

        if (POPUP_INFO_.Buttons() & Response::Okay)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Space) ||
                (KEY_EVENT.code == sf::Keyboard::O) ||
                (KEY_EVENT.code == sf::Keyboard::Return))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Okay);
                return true;
            }
        }

        if ((POPUP_INFO_.Buttons() & Response::Yes) && (KEY_EVENT.code == sf::Keyboard::Y))
        {
            SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
            return true;
        }

        if ((POPUP_INFO_.Buttons() & Response::No) &&
            ((KEY_EVENT.code == sf::Keyboard::N) || (KEY_EVENT.code == sf::Keyboard::Escape)))
        {
            SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::No);
            return true;
        }

        if (POPUP_INFO_.Buttons() & Response::Cancel)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
                ((KEY_EVENT.code == sf::Keyboard::C) && ((POPUP_INFO_.Type() != game::Popup::ContentSelectionWithItem) && (POPUP_INFO_.Type() != game::Popup::ContentSelectionWithoutItem))) ||
                ((KEY_EVENT.code == sf::Keyboard::Return) && (POPUP_INFO_.Buttons() == sfml_util::PopupButtons::Cancel)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
                return true;
            }
        }

        if ((POPUP_INFO_.Buttons() & Response::Select) &&
            ((KEY_EVENT.code == sf::Keyboard::S) || (KEY_EVENT.code == sf::Keyboard::Return)))
        {
            SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Select, imageIndex_);
            return true;
        }

        if (POPUP_INFO_.Type() == game::Popup::CharacterSelection)
        {
            if ((KEY_EVENT.code == sf::Keyboard::Num1) && (POPUP_INFO_.IsNumberValid(0)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 0);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num2) && (POPUP_INFO_.IsNumberValid(1)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 1);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num3) && (POPUP_INFO_.IsNumberValid(2)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 2);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num4) && (POPUP_INFO_.IsNumberValid(3)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 3);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num5) && (POPUP_INFO_.IsNumberValid(4)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 4);
                return true;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Num6) && (POPUP_INFO_.IsNumberValid(5)))
            {
                SoundManager::Instance()->StaticSounds_Thock()->PlayRandom();
                game::LoopManager::Instance()->PopupWaitEnd(Response::Select, 5);
                return true;
            }
        }

        return Stage::KeyRelease(KEY_EVENT);
    }


    void PopupStage::SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED)
    {
        if (false == isImageProcAllowed_)
            return;

        std::size_t newIndex(NEW_IMAGE_INDEX);
        if (newIndex >= POPUP_INFO_.Images().size())
            newIndex = POPUP_INFO_.Images().size() - 1;

        if ((imageIndex_ != newIndex) || isInitialAnimation_)
        {
            areImagesMovingLeft_ = (imageIndex_ < newIndex);
            imageIndex_ = newIndex;
            selectPopupButtonSPtr_->SetSelection(static_cast<int>(imageIndex_));

            std::ostringstream ss;
            ss << imageIndex_ + 1 << "/" << POPUP_INFO_.Images().size();
            const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                     FontManager::Instance()->Font_Typical(),
                                                     FontManager::Instance()->Size_Smallish(),
                                                     gui::PopupManager::Color_Font(),
                                                     Justified::Center);
            sf::FloatRect imageCountTextRect(textRegion_);
            imageCountTextRect.height = 0.0f;
            imageNumTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage'sImageSelectNumber", TEXT_INFO, imageCountTextRect) );

            imageSlider_.Reset(SLIDER_SPEED);

            imageSpritePrev_ = imageSpriteCurr_;

            POPUP_INFO_.Images()[newIndex]->setSmooth(true);
            imageSpriteCurr_.setTexture( * POPUP_INFO_.Images()[newIndex] );
            imageSpriteCurr_.setTextureRect(sf::IntRect(0, 0, POPUP_INFO_.Images()[newIndex]->getSize().x, POPUP_INFO_.Images()[newIndex]->getSize().y));
            imageSpriteCurr_.setScale(1.0f, 1.0f);

            const float POS_LEFT(textRegion_.left + ((areImagesMovingLeft_) ? textRegion_.width : 0.0f));
            const float POS_TOP(imagesRect_.top + (imagesRect_.height * 0.5f));
            imageSpriteCurr_.setPosition(POS_LEFT, POS_TOP);

            //establish target scale of new image
            const float RESIZE_RATIO(0.8f);
            const float SPRITE_TARGET_WIDTH(imagesRect_.width * RESIZE_RATIO);
            const float SPRITE_TARGET_HEIGHT(imagesRect_.height * RESIZE_RATIO);
            const float SCALE_HORIZ(SPRITE_TARGET_WIDTH / imageSpriteCurr_.getGlobalBounds().width);
            const float SCALE_VERT(SPRITE_TARGET_HEIGHT / imageSpriteCurr_.getGlobalBounds().height);
            imageCurrTargetScale_ = std::min(SCALE_HORIZ, SCALE_VERT);
            imageSpriteCurr_.setScale(0.0f, 0.0f);

            imagePrevStartScale_ = imageSpritePrev_.getScale().x;
            imagePrevStartPosX_ = imageSpritePrev_.getPosition().x;

            //establish the distance selectSpriteCurr, the new index sprite, will travel
            {
                sf::Sprite s(imageSpriteCurr_);
                s.setScale(imageCurrTargetScale_, imageCurrTargetScale_);
                const float POS_LEFT_CENTERED((textRegion_.left + (textRegion_.width * 0.5f)) - (s.getGlobalBounds().width * 0.5f));
                if (areImagesMovingLeft_)
                    imageCurrTravelDist_ = (textRegion_.left + textRegion_.width) - POS_LEFT_CENTERED;
                else
                    imageCurrTravelDist_ = POS_LEFT_CENTERED - textRegion_.left;
            }

            //establish the distance selectSpritePrev, the old index sprite, will travel
            if (areImagesMovingLeft_)
                imagePrevTravelDist_ = imageSpritePrev_.getGlobalBounds().left - imagesRect_.left;
            else
                imagePrevTravelDist_ = (imagesRect_.left + imagesRect_.width) - imageSpritePrev_.getGlobalBounds().left;
        }
    }


    void PopupStage::SetupInfoText(const std::string & TEXT)
    {
        const std::string TEXT_TO_USE((TEXT.empty()) ? " " : TEXT);

        const sfml_util::gui::TextInfo INFO_TEXT_INFO(TEXT_TO_USE,
                                                      FontManager::Instance()->Font_Default1(),
                                                      FontManager::Instance()->Size_Small(),
                                                      gui::PopupManager::Color_Font(),
                                                      sfml_util::Justified::Center);

        EntityRemove(infoTextRegionSPtr_);

        infoTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("PopupStage'sInfo", INFO_TEXT_INFO, sf::FloatRect()) );

        EntityAdd(infoTextRegionSPtr_);

        const float INFO_TEXT_POS_LEFT((textRegion_.left + (textRegion_.width * 0.5f)) - (infoTextRegionSPtr_->GetEntityRegion().width * 0.5f));
        const float INFO_TEXT_POS_TOP(sliderbarPosTop_ - (2.0f * infoTextRegionSPtr_->GetEntityRegion().height));
        infoTextRegionSPtr_->SetEntityPos(INFO_TEXT_POS_LEFT + static_cast<float>(TEXT.size() / 2), INFO_TEXT_POS_TOP);
    }


    int PopupStage::GetSelectNumber() const
    {
        if (textEntryBoxSPtr_.get() == nullptr)
            return NUMBER_SELECT_INVALID_;

        const std::string TEXT(boost::algorithm::trim_copy(textEntryBoxSPtr_->GetText()));

        int num(NUMBER_SELECT_INVALID_);
        try
        {
            num = boost::lexical_cast<int>(TEXT);
        }
        catch (...)
        {
            num = NUMBER_SELECT_INVALID_;
        }

        return num;
    }


    bool PopupStage::ProcessSelectNumber()
    {
        const int NUM(GetSelectNumber());
        selectPopupButtonSPtr_->SetSelection(NUM);

        if (NUM < 0)
        {
            selectPopupButtonSPtr_->SetSelection(NUMBER_SELECT_INVALID_);
            std::ostringstream ss;
            ss << "(invalid, type a number between " << POPUP_INFO_.NumberSelMin() << " and " << POPUP_INFO_.NumberSelMax() << ")";
            SetupInfoText(ss.str());
            return false;
        }
        if (NUM < static_cast<int>(POPUP_INFO_.NumberSelMin()))
        {
            selectPopupButtonSPtr_->SetSelection(NUMBER_SELECT_INVALID_);
            std::ostringstream ss;
            ss << "(the number is too small, the minimum is " << POPUP_INFO_.NumberSelMin() << ")";
            SetupInfoText(ss.str());
            return false;
        }
        else if (NUM > static_cast<int>(POPUP_INFO_.NumberSelMax()))
        {
            selectPopupButtonSPtr_->SetSelection(NUMBER_SELECT_INVALID_);
            std::ostringstream ss;
            ss << "(the number is too large, the maximum is " << POPUP_INFO_.NumberSelMax() << ")";
            SetupInfoText(ss.str());
            return false;
        }
        else
        {
            SetupInfoText("");

            if (sliderbarSPtr_.get() != nullptr)
            {
                willSliderbarUpdate_ = false;

                if (NUM == static_cast<int>(POPUP_INFO_.NumberSelMin()))
                    sliderbarSPtr_->SetCurrentValue(0.0f);
                else
                    sliderbarSPtr_->SetCurrentValue(static_cast<float>(NUM) / static_cast<float>(POPUP_INFO_.NumberSelMax()));

                willSliderbarUpdate_ = true;
            }

            return true;
        }
    }

}
