// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-system-error.cpp
//
#include "popup-stage-system-error.hpp"

#include "game/game-data-file.hpp"

#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace popup
{

    PopupStageSystemError::PopupStageSystemError(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , bgTexture_()
        , bgSprite_()
    {}

    PopupStageSystemError::~PopupStageSystemError() = default;

    void PopupStageSystemError::Setup()
    {
        PopupStageBase::Setup();

        sfml_util::Loaders::Texture(
            bgTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-misc-error"));

        sfml_util::Mask(bgTexture_, sf::Color::White);
        bgSprite_.setTexture(bgTexture_, true);
        bgSprite_.setColor(sf::Color(255, 255, 255, 32));

        auto const HORIZ_RESCALE{ textRegion_.width / static_cast<float>(bgTexture_.getSize().x) };

        bgSprite_.setScale(HORIZ_RESCALE, HORIZ_RESCALE);

        if (bgSprite_.getGlobalBounds().height > textRegion_.height)
        {
            auto VERT_RESCALE{ textRegion_.height / static_cast<float>(bgTexture_.getSize().y) };

            bgSprite_.setScale(VERT_RESCALE, VERT_RESCALE);
        }

        auto const BG_POS_LEFT{ (textRegion_.left + (textRegion_.width * 0.5f))
                                - (bgSprite_.getGlobalBounds().width * 0.5f) };

        auto const BG_POS_TOP{ (textRegion_.top + (textRegion_.height * 0.5f))
                               - (bgSprite_.getGlobalBounds().height * 0.5f) };

        bgSprite_.setPosition(BG_POS_LEFT, BG_POS_TOP);
    }

    void PopupStageSystemError::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);
        target.draw(bgSprite_, STATES);
        Stage::Draw(target, STATES);
    }
} // namespace popup
} // namespace heroespath
