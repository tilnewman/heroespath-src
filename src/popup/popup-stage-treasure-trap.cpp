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
// popup-stage-treasure-trap.cpp
//
#include "popup-stage-treasure-trap.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"


namespace popup
{

    PopupStageTreasureTrap::PopupStageTreasureTrap(const PopupInfo & POPUP_INFO)
    :
        PopupStageBase(POPUP_INFO),
        trapTexture_(),
        trapSprite_(),
        descTextRegionUPtr_()
    {}


    PopupStageTreasureTrap::~PopupStageTreasureTrap()
    {}


    void PopupStageTreasureTrap::Setup()
    {
        PopupStageBase::Setup();
        SetupTitleText();
        SetupDescriptionText();
        SetupAccentImage();
    }


    void PopupStageTreasureTrap::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);
        Stage::Draw(target, STATES);
    }


    void PopupStageTreasureTrap::SetupTitleText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;

        const sfml_util::gui::TextInfo TITLE_TEXTINFO(
            "You sprung the trap!",
            sfml_util::FontManager::Instance()->Font_BigFlavor1(),
            sfml_util::FontManager::Instance()->Size_Larger(),
            sf::Color(127, 32, 32),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        textRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStageTreasureTrap's_Title",
            TITLE_TEXTINFO,
            tempRect,
            this);
    }


    void PopupStageTreasureTrap::SetupDescriptionText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;
        tempRect.top += sfml_util::MapByRes(75.0f, 225.0f);

        const sfml_util::gui::TextInfo DESC_TEXTINFO(
            popupInfo_.TextInfo().text,
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::Black,
            sfml_util::Justified::Center);

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStageTreasureTrap's_Desc",
            DESC_TEXTINFO,
            tempRect,
            this);

        Stage::EntityAdd(descTextRegionUPtr_.get());
    }


    void PopupStageTreasureTrap::SetupAccentImage()
    {
        sfml_util::LoadTexture(
            accentTexture1_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-trap"));

        accentSprite1_.setTexture(accentTexture1_, true);

        sfml_util::CenterAndScaleSpriteToFit(
            accentSprite1_,
            textRegion_,
            0.65f);

        accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }

}
