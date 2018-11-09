// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage-title.cpp
//
#include "stage-title.hpp"

#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace gui
{

    const float StageTitle::DEFAULT_IMAGE_WIDTH_AS_SCREEN_RATIO_ { 0.6f };
    const float StageTitle::TITLE_IMAGE_HEIGHT_RATIO_OF_SYMBOL_HEIGHT_ { 0.65f };

    StageTitle::StageTitle(
        const std::string & TITLE_IMAGE_PATH_KEY,
        const bool WILL_INVERT_SYMBOL,
        const float SIZE_HORIZ,
        const float SIZE_VERT)
        : symbolCachedTexture_(
            "media-images-gui-accents-symbol2",
            ImageOptions(
                (WILL_INVERT_SYMBOL) ? (ImageOpt::Default | ImageOpt::Invert) : ImageOpt::Default))
        , symbolSprite_(symbolCachedTexture_.Get())
        , titleCachedTextureOpt_()
        , titleSprite_()
    {
        if (TITLE_IMAGE_PATH_KEY.empty() == false)
        {
            titleCachedTextureOpt_ = gui::CachedTexture(TITLE_IMAGE_PATH_KEY);
            titleSprite_.setTexture(titleCachedTextureOpt_->Get(), true);
        }

        SetSizeAndReCenter(SIZE_HORIZ, SIZE_VERT);
    }

    void StageTitle::SetSizeAndReCenter(const float SIZE_HORIZ, const float SIZE_VERT)
    {
        const auto SYMBOL_WIDTH_DEFAULT { sfutil::ScreenRatioToPixelsHoriz(
            DEFAULT_IMAGE_WIDTH_AS_SCREEN_RATIO_) };

        const auto SYMBOL_SCALE_DEFAULT { SYMBOL_WIDTH_DEFAULT
                                          / symbolSprite_.getLocalBounds().width };

        auto symbolWidth { SYMBOL_WIDTH_DEFAULT };

        if (SIZE_HORIZ > 0.0f)
        {
            symbolWidth = SIZE_HORIZ;
        }

        const auto SYMBOL_SCALE_HORIZ { symbolWidth / symbolSprite_.getLocalBounds().width };

        auto symbolHeight { symbolSprite_.getLocalBounds().height * SYMBOL_SCALE_DEFAULT };

        if (SIZE_VERT > 0.0f)
        {
            symbolHeight = SIZE_VERT;
        }

        const auto SYMBOL_SCALE_VERT { symbolHeight / symbolSprite_.getLocalBounds().height };

        symbolSprite_.setScale(SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);

        symbolSprite_.setPosition(
            sfutil::DisplayCenterHoriz(symbolSprite_.getGlobalBounds().width),
            sfutil::ScreenRatioToPixelsVert(0.011f));

        if (titleCachedTextureOpt_)
        {
            sfutil::FitAndCenterTo(
                titleSprite_,
                sfutil::ScaleAndReCenterCopy(
                    symbolSprite_.getGlobalBounds(), TITLE_IMAGE_HEIGHT_RATIO_OF_SYMBOL_HEIGHT_));
        }
    }

    void StageTitle::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(symbolSprite_, states);

        if (titleCachedTextureOpt_)
        {
            target.draw(titleSprite_, states);
        }
    }

    float StageTitle::DefaultBottomPad() const { return sfutil::ScreenRatioToPixelsVert(0.039f); }

} // namespace gui
} // namespace heroespath