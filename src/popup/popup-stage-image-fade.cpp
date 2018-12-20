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

#include "gui/font-manager.hpp"
#include "gui/text-region.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

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

        const gui::TextInfo TITLE_TEXTINFO(
            popupInfo_.TitleText(),
            gui::GuiFont::DefaultBoldFlavor,
            gui::FontManager::Instance()->Size_Large(),
            sfutil::color::GrayDarker,
            gui::Justified::Center,
            sf::Text::Bold);

        auto titleRegion { ContentRegion() };
        titleRegion.height = 0.0f;

        titleTextRegionUPtr_
            = std::make_unique<gui::TextRegion>("ImageFadePopupTitle", TITLE_TEXTINFO, titleRegion);

        const gui::TextInfo DESC_TEXTINFO(
            popupInfo_.DescText(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Smallish(),
            sfutil::color::GrayDarker,
            gui::Justified::Center);

        auto descRegion { ContentRegion() };
        descRegion.height = 0.0f;

        descTextRegionUPtr_
            = std::make_unique<gui::TextRegion>("ImageFadePopupDesc", DESC_TEXTINFO, descRegion);

        descRegion = descTextRegionUPtr_->GetEntityRegion();

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x,
            sfutil::Bottom(ContentRegion()) - descRegion.height);

        const auto IMAGE_PAD_VERT { sfutil::ScreenRatioToPixelsVert(0.0167f) };

        const sf::FloatRect IMAGE_REGION = [&]() {
            const auto IMAGE_TOP { sfutil::Bottom(ContentRegion()) + IMAGE_PAD_VERT };

            const auto MAX_HEIGHT { descTextRegionUPtr_->GetEntityPos().y
                                    - (IMAGE_TOP + IMAGE_PAD_VERT) };

            const auto IMAGE_MAX_DIMM { sfutil::ScreenRatioToPixelsHoriz(0.086f) };

            const auto IMAGE_WIDTH { std::min(IMAGE_MAX_DIMM, ContentRegion().width) };
            const auto IMAGE_HEIGHT { std::min(IMAGE_MAX_DIMM, MAX_HEIGHT) };

            return sf::FloatRect(ContentRegion().left, IMAGE_TOP, IMAGE_WIDTH, IMAGE_HEIGHT);
        }();

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x, sfutil::Bottom(IMAGE_REGION) + IMAGE_PAD_VERT);

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
        sfutil::FitAndCenterTo(spritePrev_, IMAGE_REGION);
        sfutil::FitAndCenterTo(spriteCurr_, IMAGE_REGION);

        spriteCurr_.setColor(sf::Color(255, 255, 255, 0));
    }

    void PopupStageImageFade::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);

        if (popupInfo_.ImagesCount() > 1)
        {
            target.draw(spritePrev_, states);
        }

        target.draw(spriteCurr_, states);
        target.draw(*titleTextRegionUPtr_, states);
        target.draw(*descTextRegionUPtr_, states);

        StageBase::draw(target, states);
    }

    void PopupStageImageFade::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

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
