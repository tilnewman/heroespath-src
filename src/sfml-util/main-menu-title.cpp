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
// main-menu-title.cpp
//
#include "main-menu-title.hpp"

#include "game/game-data-file.hpp"

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
        LoadTexture(
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

            auto const TITLE_IMAGE_PATH{ game::GameDataFile::Instance()->GetMediaPath(
                                             "media-images-buttons-mainmenu-dir")
                                         + TITLE_IMAGE_FILENAME };

            sfml_util::LoadTexture(titleTexture_, TITLE_IMAGE_PATH);
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
