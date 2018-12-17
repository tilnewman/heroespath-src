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
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    PopupStageSystemError::PopupStageSystemError(const PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , bgCachedTexture_(
              "media-image-misc-system-error",
              gui::ImageOptions(gui::ImageOpt::Default, sf::Color::White))
        , bgSprite_(bgCachedTexture_.Get())
    {}

    PopupStageSystemError::~PopupStageSystemError() = default;

    void PopupStageSystemError::Setup()
    {
        PopupStageBase::Setup();
        bgSprite_.setColor(sf::Color(255, 255, 255, 32));
        sfutil::FitAndCenterTo(bgSprite_, textRegion_);
    }

    void PopupStageSystemError::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageBase::draw(target, states);
        target.draw(bgSprite_, states);
        StageBase::draw(target, states);
    }

} // namespace popup
} // namespace heroespath
