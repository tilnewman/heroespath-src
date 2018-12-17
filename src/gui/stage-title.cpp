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

#include "misc/config-file.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace gui
{

    const float StageTitle::DEFAULT_SYMBOL_HEIGHT_SCREEN_RATIO_ { 0.175f };
    const float StageTitle::DEFAULT_SYMBOL_TO_TITLE_HEIGHT_RATIO_ { 0.55f };
    const float StageTitle::DEFAULT_SYMBOL_VERT_POS_SCREEN_RATIO_ { 0.011f };
    const float StageTitle::DEFAULT_BOTTOM_PAD_AS_SYMBOL_HEIGHT_RATIO_ { 0.2f };
    const float StageTitle::VISIBLE_VERT_POS_RATIO_ { 0.535f };

    StageTitle::StageTitle(
        const MenuImage::Enum MENU_IMAGE,
        const bool WILL_INVERT_SYMBOL_TO_WHITE,
        const float SYMBOL_HEIGHT_SCREEN_RATIO,
        const float SYMBOL_TO_TITLE_HEIGHT_RATIO,
        const float SYMBOL_VERT_POS_SCREEN_RATIO)
        : symbolCachedTexture_(
            MenuImage::ConfigFileKey(MenuImage::SymbolTop),
            ImageOptions(
                (WILL_INVERT_SYMBOL_TO_WHITE) ? (ImageOpt::Default | ImageOpt::Invert)
                                              : ImageOpt::Default))
        , symbolSprite_(
              symbolCachedTexture_.Get(),
              misc::ConfigFile::Instance()->GetTextureRect(
                  MenuImage::ConfigFileKey(MenuImage::SymbolTop)))
        , titleCachedTextureOpt_()
        , titleSprite_()
    {
        symbolSprite_.setColor(sf::Color(255, 255, 255, gui::MenuImage::SymbolAlpha()));

        if (MENU_IMAGE != MenuImage::Count)
        {
            const auto CONFIG_FILE_KEY { MenuImage::ConfigFileKey(MENU_IMAGE) };

            titleCachedTextureOpt_ = gui::CachedTexture(CONFIG_FILE_KEY);
            titleSprite_.setTexture(titleCachedTextureOpt_->Get());

            titleSprite_.setTextureRect(
                misc::ConfigFile::Instance()->GetTextureRect(CONFIG_FILE_KEY));
        }

        SetSizeAndReCenter(
            SYMBOL_HEIGHT_SCREEN_RATIO, SYMBOL_TO_TITLE_HEIGHT_RATIO, SYMBOL_VERT_POS_SCREEN_RATIO);
    }

    void StageTitle::SetSizeAndReCenter(
        const float SYMBOL_HEIGHT_SCREEN_RATIO,
        const float SYMBOL_TO_TITLE_HEIGHT_RATIO,
        const float SYMBOL_VERT_POS_SCREEN_RATIO)
    {
        const auto SYMBOL_HEIGHT { sfutil::ScreenRatioToPixelsVert(SYMBOL_HEIGHT_SCREEN_RATIO) };
        const auto SYMBOL_SCALE { SYMBOL_HEIGHT / symbolSprite_.getLocalBounds().height };

        symbolSprite_.setScale(SYMBOL_SCALE, SYMBOL_SCALE);

        const auto SYMBOL_VERT_POS { sfutil::ScreenRatioToPixelsVert(
            SYMBOL_VERT_POS_SCREEN_RATIO) };

        symbolSprite_.setPosition(
            sfutil::DisplayCenterHoriz(symbolSprite_.getGlobalBounds().width), SYMBOL_VERT_POS);

        symbolSprite_.setColor(sf::Color(255, 255, 255, MenuImage::SymbolAlpha()));

        if (titleCachedTextureOpt_)
        {
            const auto TITLE_HEIGHT { (
                symbolSprite_.getGlobalBounds().height * SYMBOL_TO_TITLE_HEIGHT_RATIO) };

            const auto TITLE_SCALE { TITLE_HEIGHT / titleSprite_.getLocalBounds().height };

            titleSprite_.setScale(TITLE_SCALE, TITLE_SCALE);

            auto symbolRegionWithVisibleVertAdj { symbolSprite_.getGlobalBounds() };

            const auto VISIBLE_VERT_SHIFT { (
                (symbolRegionWithVisibleVertAdj.height * VISIBLE_VERT_POS_RATIO_)
                - (symbolRegionWithVisibleVertAdj.height * 0.5f)) };

            symbolRegionWithVisibleVertAdj.top += VISIBLE_VERT_SHIFT;
            // symbolRegionWithVisibleVertAdj.height -= (VISIBLE_VERT_SHIFT * 2.0f);

            sfutil::CenterTo(titleSprite_, symbolRegionWithVisibleVertAdj);
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

    float StageTitle::DefaultBottomPad() const
    {
        return (
            symbolSprite_.getGlobalBounds().height * DEFAULT_BOTTOM_PAD_AS_SYMBOL_HEIGHT_RATIO_);
    }

} // namespace gui
} // namespace heroespath
