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


namespace game
{

    MainMenuTitle::MainMenuTitle(const std::string & TITLE_KEY_STR,
                                 const bool          WILL_INVERT_SYMBOL,
                                 const float         SYMBOL_SCALE_HORIZ,
                                 const float         SYMBOL_SCALE_VERT)
    :
        symbolTextureSPtr_(),
        symbolSprite_     (),
        titleTextureSPtr_ (),
        titleSprite_      ()
    {
        Setup(TITLE_KEY_STR, WILL_INVERT_SYMBOL, SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }


    MainMenuTitle::~MainMenuTitle()
    {}


    void MainMenuTitle::Setup(const std::string & TITLE_IMAGE_FILENAME,
                              const bool          WILL_INVERT_SYMBOL,
                              const float         SYMBOL_SCALE_HORIZ,
                              const float         SYMBOL_SCALE_VERT)
    {
        const std::string IMAGE_PATH_KEY((WILL_INVERT_SYMBOL) ? "media-images-gui-accents-symbol2-inv" : "media-images-gui-accents-symbol2");
        sfml_util::LoadImageOrTextureSPtr<sfml_util::TextureSPtr_t>(symbolTextureSPtr_, GameDataFile::Instance()->GetMediaPath(IMAGE_PATH_KEY));

        symbolTextureSPtr_->setSmooth(true);
        symbolSprite_.setTexture( * symbolTextureSPtr_);

        if (TITLE_IMAGE_FILENAME.empty() == false)
        {
            const std::string TITLE_IMAGE_PATH(std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append(TITLE_IMAGE_FILENAME));
            sfml_util::LoadImageOrTextureSPtr<sfml_util::TextureSPtr_t>(titleTextureSPtr_, TITLE_IMAGE_PATH);
            titleTextureSPtr_->setSmooth(true);
            titleSprite_.setTexture( * titleTextureSPtr_ );
        }

        SetPositionAndSize(SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }


    void MainMenuTitle::SetPositionAndSize(const float SYMBOL_SCALE_HORIZ,
                                           const float SYMBOL_SCALE_VERT)
    {
        const float SYMBOL_IMAGE_SCALE(sfml_util::MapByRes(0.6f, 2.5f));
        symbolSprite_.setScale(SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_HORIZ, SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_VERT);
        symbolSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (symbolSprite_.getGlobalBounds().width * 0.5f), sfml_util::MapByRes(10.0f, 30.0f));

        if (titleTextureSPtr_.get() != nullptr)
        {
            const float TITLE_IMAGE_SCALE(sfml_util::MapByRes(1.0f, 3.25f));
            titleSprite_.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);
            titleSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f),
                                     (symbolSprite_.getPosition().y + (symbolSprite_.getGlobalBounds().height * 0.5f)) - (titleSprite_.getGlobalBounds().height * 0.5f));
        }
    }


    void MainMenuTitle::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(symbolSprite_, states);

        if (titleTextureSPtr_.get() != nullptr)
            target.draw(titleSprite_, states);
    }

}