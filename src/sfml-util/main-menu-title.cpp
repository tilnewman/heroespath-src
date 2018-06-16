// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main-menu-title.cpp
//
#include "main-menu-title.hpp"

#include "game/game-data-file.hpp"
#include "misc/filesystem-helpers.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace sfml_util
{

    MainMenuTitle::MainMenuTitle(
        const std::string & TITLE_KEY_STR,
        const bool WILL_INVERT_SYMBOL,
        const float SYMBOL_SCALE_HORIZ,
        const float SYMBOL_SCALE_VERT)
        : symbolTexture_()
        , symbolSprite_()
        , titleTexture_()
        , titleSprite_()
        , willDrawTitle_(false)
    {
        Setup(TITLE_KEY_STR, WILL_INVERT_SYMBOL, SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }

    MainMenuTitle::~MainMenuTitle() = default;

    void MainMenuTitle::Setup(
        const std::string & TITLE_IMAGE_FILENAME,
        const bool WILL_INVERT_SYMBOL,
        const float SYMBOL_SCALE_HORIZ,
        const float SYMBOL_SCALE_VERT)
    {
        Loaders::Texture(
            symbolTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol2"));

        symbolSprite_.setTexture(symbolTexture_, true);

        if (WILL_INVERT_SYMBOL)
        {
            sfml_util::Invert(symbolTexture_);
        }

        if (TITLE_IMAGE_FILENAME.empty() == false)
        {
            willDrawTitle_ = true;

            auto const TITLE_IMAGE_PATH{ misc::filesystem::CompletePath(
                game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"),
                TITLE_IMAGE_FILENAME) };

            sfml_util::Loaders::Texture(titleTexture_, TITLE_IMAGE_PATH);
            titleSprite_.setTexture(titleTexture_, true);
        }

        SetPositionAndSize(SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }

    void MainMenuTitle::SetPositionAndSize(
        const float SYMBOL_SCALE_HORIZ, const float SYMBOL_SCALE_VERT)
    {
        auto const SYMBOL_IMAGE_SCALE{ sfml_util::MapByRes(0.6f, 2.5f) };

        symbolSprite_.setScale(
            SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_HORIZ, SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_VERT);

        auto const HALF_SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() * 0.5f };
        auto const HALF_SYMBOL_WIDTH{ symbolSprite_.getGlobalBounds().width * 0.5f };

        symbolSprite_.setPosition(
            HALF_SCREEN_WIDTH - HALF_SYMBOL_WIDTH, sfml_util::MapByRes(10.0f, 30.0f));

        if (willDrawTitle_)
        {
            auto const TITLE_IMAGE_SCALE{ sfml_util::MapByRes(1.0f, 3.25f) };
            titleSprite_.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);

            auto const HALF_TITLE_WIDTH{ titleSprite_.getGlobalBounds().width * 0.5f };
            auto const HALF_TITLE_HEIGHT{ titleSprite_.getGlobalBounds().height * 0.5f };
            auto const HALF_SYMBOL_HEIGHT{ symbolSprite_.getGlobalBounds().height * 0.5f };

            titleSprite_.setPosition(
                HALF_SCREEN_WIDTH - HALF_TITLE_WIDTH,
                (symbolSprite_.getPosition().y + HALF_SYMBOL_HEIGHT) - HALF_TITLE_HEIGHT);
        }
    }

    void MainMenuTitle::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(symbolSprite_, states);

        if (willDrawTitle_)
        {
            target.draw(titleSprite_, states);
        }
    }

} // namespace sfml_util
} // namespace heroespath
