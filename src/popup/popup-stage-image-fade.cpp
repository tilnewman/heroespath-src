// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-image-fade.cpp
//
#include "popup-stage-image-fade.hpp"

#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-position.hpp"

#include <algorithm>

namespace heroespath
{
namespace popup
{

    const float PopupStageImageFade::BEFORE_FADE_STARTS_DELAY_SEC_ { 2.0f };

    PopupStageImageFade::PopupStageImageFade(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , spriteCurr_()
        , spritePrev_()
        , beforeFadeTimerSec_(0.0f)
        , fadeAlpha_(0.0f)
        , titleTextRegionUPtr_()
        , descTextRegionUPtr_()
    {}

    PopupStageImageFade::~PopupStageImageFade() = default;

    void PopupStageImageFade::Setup()
    {
        PopupStageBase::Setup();

        const sfml_util::TextInfo TITLE_TEXTINFO(
            popupInfo_.TitleText(),
            sfml_util::GuiFont::DefaultBoldFlavor,
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::color::GrayDarker,
            sfml_util::Justified::Center,
            sf::Text::Bold);

        auto titleRegion { textRegion_ };
        titleRegion.height = 0.0f;

        titleTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "ImageFadePopupTitle", TITLE_TEXTINFO, titleRegion);

        const sfml_util::TextInfo DESC_TEXTINFO(
            popupInfo_.DescText(),
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::color::GrayDarker,
            sfml_util::Justified::Center);

        auto descRegion { textRegion_ };
        descRegion.height = 0.0f;

        descTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "ImageFadePopupDesc", DESC_TEXTINFO, descRegion);

        descRegion = descTextRegionUPtr_->GetEntityRegion();

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x,
            (textRegion_.top + textRegion_.height) - descRegion.height);

        auto const IMAGE_PAD_VERT { sfml_util::ScreenRatioToPixelsVert(0.0167f) };

        const sf::FloatRect IMAGE_REGION = [&]() {
            auto const IMAGE_TOP { sfml_util::Bottom(textRegionUPtr_->GetEntityRegion())
                                   + IMAGE_PAD_VERT };

            auto const MAX_HEIGHT { descTextRegionUPtr_->GetEntityPos().y
                                    - (IMAGE_TOP + IMAGE_PAD_VERT) };

            auto const IMAGE_MAX_DIMM { sfml_util::ScreenRatioToPixelsHoriz(0.086f) };

            auto const IMAGE_WIDTH { std::min(IMAGE_MAX_DIMM, textRegion_.width) };
            auto const IMAGE_HEIGHT { std::min(IMAGE_MAX_DIMM, MAX_HEIGHT) };

            return sf::FloatRect(textRegion_.left, IMAGE_TOP, IMAGE_WIDTH, IMAGE_HEIGHT);
        }();

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x,
            sfml_util::Bottom(IMAGE_REGION) + IMAGE_PAD_VERT);

        if (popupInfo_.ImagesCount() == 1)
        {
            spriteCurr_.setTexture(popupInfo_.ImagesAt(0).Get(), true);
        }
        else
        {
            spritePrev_.setTexture(popupInfo_.ImagesAt(0).Get(), true);
            spriteCurr_.setTexture(popupInfo_.ImagesAt(1).Get(), true);
        }

        spritePrev_.setScale(1.0f, 1.0f);
        spriteCurr_.setScale(1.0f, 1.0f);
        sfml_util::FitAndCenterTo(spritePrev_, IMAGE_REGION);
        sfml_util::FitAndCenterTo(spriteCurr_, IMAGE_REGION);

        spriteCurr_.setColor(sf::Color(255, 255, 255, 0));
    }

    void PopupStageImageFade::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        if (popupInfo_.ImagesCount() > 1)
        {
            target.draw(spritePrev_, STATES);
        }

        target.draw(spriteCurr_, STATES);
        target.draw(*titleTextRegionUPtr_, STATES);
        target.draw(*descTextRegionUPtr_, STATES);

        Stage::Draw(target, STATES);
    }

    void PopupStageImageFade::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (beforeFadeTimerSec_ < BEFORE_FADE_STARTS_DELAY_SEC_)
        {
            beforeFadeTimerSec_ += ELAPSED_TIME_SECONDS;
        }
        else
        {
            if (fadeAlpha_ < 255.0f)
            {
                fadeAlpha_ += ELAPSED_TIME_SECONDS * popupInfo_.ImageFadeSpeed();

                if (fadeAlpha_ > 255.0f)
                {
                    fadeAlpha_ = 255.0f;
                }

                spriteCurr_.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(fadeAlpha_)));
            }
            else
            {
                spriteCurr_.setColor(sf::Color::White);
            }
        }
    }

    bool PopupStageImageFade::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        beforeFadeTimerSec_ = BEFORE_FADE_STARTS_DELAY_SEC_;
        fadeAlpha_ = 256.0f; // anything greater than 255.0f will work here
        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

} // namespace popup
} // namespace heroespath
