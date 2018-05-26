// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-treasure-trap.cpp
//
#include "popup-stage-treasure-trap.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageTreasureTrap::PopupStageTreasureTrap(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , trapTexture_()
        , trapSprite_()
        , descTextRegionUPtr_()
    {}

    PopupStageTreasureTrap::~PopupStageTreasureTrap() = default;

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
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::DefaultBoldFlavor),
            sfml_util::FontManager::Instance()->Size_Larger(),
            sf::Color(127, 32, 32),
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        textRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStageTreasureTrap's_Title",
            TITLE_TEXTINFO,
            tempRect,
            sfml_util::IStagePtr_t(this));
    }

    void PopupStageTreasureTrap::SetupDescriptionText()
    {
        sf::FloatRect tempRect(textRegion_);
        tempRect.height = 0.0f;
        tempRect.top += sfml_util::MapByRes(75.0f, 225.0f);

        const sfml_util::gui::TextInfo DESC_TEXTINFO(
            popupInfo_.TextInfo().text,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::Black,
            sfml_util::Justified::Center);

        descTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PopupStageTreasureTrap's_Desc", DESC_TEXTINFO, tempRect, sfml_util::IStagePtr_t(this));

        Stage::EntityAdd(descTextRegionUPtr_.get());
    }

    void PopupStageTreasureTrap::SetupAccentImage()
    {
        sfml_util::Loaders::Texture(
            accentTexture1_, game::GameDataFile::Instance()->GetMediaPath("media-images-trap"));

        accentSprite1_.setTexture(accentTexture1_, true);

        sfml_util::CenterAndScaleSpriteToFit(accentSprite1_, textRegion_, 0.65f);

        accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }
} // namespace popup
} // namespace heroespath
