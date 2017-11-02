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
// adventure-display-stage.cpp
//
#include "adventure-display-stage.hpp"

#include "game/game-data-file.hpp"
#include "game/stage/adventure-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"


namespace game
{
namespace stage
{

    AdventureDisplayStage::AdventureDisplayStage(AdventureStage * const)
    :
        Stage(
            "AdventureDisplay",
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()),
        //adventureStagePtr_(adventureStagePtr),
        characterListUPtr_(std::make_unique<AdventureCharacterList>(this)),
        backgroundTexture_(),
        bottomImage_(0.75f, true, sf::Color::White),
        topImage_("", true, 1.0f, 0.75f)
    {}


    AdventureDisplayStage::~AdventureDisplayStage()
    {}


    void AdventureDisplayStage::Setup()
    {
        Setup_CharacterList();
        Setup_BackgroundImage();
        Setup_Map();
    }


    void AdventureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);
        target.draw(bottomImage_, STATES);
        target.draw(topImage_, STATES);
        target.draw(*characterListUPtr_, STATES);
        Stage::Draw(target, STATES);
        mapUPtr_->Draw(target, STATES);
    }


    bool AdventureDisplayStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        auto const MOVE_AMOUNT{ 1.0f };

        if (KEY_EVENT.code == sf::Keyboard::Up)
        {
            mapUPtr_->MoveUp(MOVE_AMOUNT);
            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Down)
        {
            mapUPtr_->MoveDown(MOVE_AMOUNT);
            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Left)
        {
            mapUPtr_->MoveLeft(MOVE_AMOUNT);
            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Right)
        {
            mapUPtr_->MoveRight(MOVE_AMOUNT);
            return true;
        }

        return false;
    }


    void AdventureDisplayStage::Setup_CharacterList()
    {
        characterListUPtr_->Setup();

        auto const CHARACTER_LIST_LEFT{
            (sfml_util::Display::Instance()->GetWinWidth() * 0.5f) -
            (characterListUPtr_->GetEntityRegion().width * 0.5f) };

        auto const CHARACTER_LIST_TOP{
            sfml_util::Display::Instance()->GetWinHeight() -
            characterListUPtr_->GetEntityRegion().height -
            bottomImage_.Height() -
            sfml_util::MapByRes(30.0f, 90.0f)};

        characterListUPtr_->SetEntityPos(CHARACTER_LIST_LEFT, CHARACTER_LIST_TOP);
    }


    void AdventureDisplayStage::Setup_BackgroundImage()
    {
        sfml_util::LoadTexture(
            backgroundTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2"));

        sfml_util::FlipVert(backgroundTexture_);

        backgroundSprite_.setTexture(backgroundTexture_);
        backgroundSprite_.setPosition(0.0f, 0.0f);

        backgroundSprite_.setScale(
            sfml_util::Display::Instance()->GetWinWidth() /
                backgroundSprite_.getLocalBounds().width,
            sfml_util::Display::Instance()->GetWinHeight() /
                backgroundSprite_.getLocalBounds().height);
    }


    void AdventureDisplayStage::Setup_Map()
    {
        mapUPtr_ = std::make_unique<sfml_util::TileMap>(
            GameDataFile::Instance()->GetMediaPath("media-maps-thornberry-highlands"),
            sf::Vector2f(100.0f + sfml_util::MapByRes(30.0f, 90.0f), topImage_.Bottom()),
            sf::Vector2u(512, 256),
            sf::Vector2f(150.0f, 150.0f));
    }

}
}
