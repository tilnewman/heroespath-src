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
// popup-stage-image-fade.cpp
//
#include "popup-stage-image-fade.hpp"

#include <algorithm>

namespace heroespath
{
namespace popup
{

    const float PopupStageImageFade::BEFORE_FADE_STARTS_DELAY_SEC_{ 2.0f };

    PopupStageImageFade::PopupStageImageFade(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , textureCurr_()
        , texturePrev_()
        , spriteCurr_()
        , spritePrev_()
        , beforeFadeTimerSec_(0.0f)
        , fadeAlpha_(0.0f)
        , titleTextRegionUPtr_()
        , descTextRegionUPtr_()
    {}

    PopupStageImageFade::~PopupStageImageFade() {}

    void PopupStageImageFade::Setup()
    {
        PopupStageBase::Setup();

        const sfml_util::gui::TextInfo TITLE_TEXTINFO(
            popupInfo_.TitleText(),
            sfml_util::FontManager::Instance()->Font_BigFlavor1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        auto titleRegion{ textRegion_ };
        titleRegion.height = 0.0f;

        titleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "ImageFadePopupTitle", TITLE_TEXTINFO, titleRegion);

        const sfml_util::gui::TextInfo DESC_TEXTINFO(
            popupInfo_.DescText(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto descRegion{ textRegion_ };
        descRegion.height = 0.0f;

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "ImageFadePopupDesc", DESC_TEXTINFO, descRegion);

        descRegion = descTextRegionUPtr_->GetEntityRegion();

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x,
            (textRegion_.top + textRegion_.height) - descRegion.height);

        auto const IMAGE_PAD{ sfml_util::MapByRes(15.0f, 35.0f) };

        auto const IMAGE_TOP{ textRegionUPtr_->GetEntityRegion().top
                              + textRegionUPtr_->GetEntityRegion().height + IMAGE_PAD };

        auto const FREE_SPACE_VERT{ descTextRegionUPtr_->GetEntityPos().y
                                    - (IMAGE_TOP + IMAGE_PAD) };
        auto const FREE_SPACE_HORIZ{ textRegion_.width };
        auto const IMAGE_MAX_DIMM{ sfml_util::MapByRes(110.0f, 750.0f) };
        auto const IMAGE_WIDTH{ std::min(IMAGE_MAX_DIMM, FREE_SPACE_HORIZ) };
        auto const IMAGE_HEIGHT{ std::min(IMAGE_MAX_DIMM, FREE_SPACE_VERT) };

        descTextRegionUPtr_->SetEntityPos(
            descTextRegionUPtr_->GetEntityPos().x, IMAGE_TOP + IMAGE_HEIGHT + IMAGE_PAD);

        if (popupInfo_.ImagesCount() == 1)
        {
            spriteCurr_.setTexture(popupInfo_.ImagesAt(0), true);
        }
        else
        {
            spritePrev_.setTexture(popupInfo_.ImagesAt(0), true);
            spriteCurr_.setTexture(popupInfo_.ImagesAt(1), true);
        }

        // scale the image to fit in the free space
        // assume popupInfo_.ImagesAt(0) and popupInfo_.ImagesAt(1) are the same size
        spritePrev_.setScale(1.0f, 1.0f);
        spriteCurr_.setScale(1.0f, 1.0f);
        auto SCALE_VERT{ IMAGE_HEIGHT / spriteCurr_.getGlobalBounds().height };
        spritePrev_.setScale(SCALE_VERT, SCALE_VERT);
        spriteCurr_.setScale(SCALE_VERT, SCALE_VERT);

        if (spriteCurr_.getGlobalBounds().width > FREE_SPACE_HORIZ)
        {
            auto const SCALE_HORIZ{ IMAGE_WIDTH / spriteCurr_.getGlobalBounds().width };
            spritePrev_.setScale(SCALE_HORIZ, SCALE_HORIZ);
            spriteCurr_.setScale(SCALE_HORIZ, SCALE_HORIZ);
        }

        spritePrev_.setPosition(
            (textRegion_.left + (textRegion_.width * 0.5f))
                - (spritePrev_.getGlobalBounds().width * 0.5f),
            (IMAGE_TOP + (IMAGE_HEIGHT * 0.5f)) - (spritePrev_.getGlobalBounds().height * 0.5f));

        spriteCurr_.setPosition(
            (textRegion_.left + (textRegion_.width * 0.5f))
                - (spriteCurr_.getGlobalBounds().width * 0.5f),
            (IMAGE_TOP + (IMAGE_HEIGHT * 0.5f)) - (spriteCurr_.getGlobalBounds().height * 0.5f));

        spriteCurr_.setColor(sf::Color(255, 255, 255, 0));
    }

    void PopupStageImageFade::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        if (popupInfo_.Images().size() > 1)
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
}
}
