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

#include "map/map.hpp"
#include "map/map-display.hpp"
#include "map/level-enum.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "stage/adventure-stage.hpp"
#include "avatar/i-view.hpp"
#include "stage/adventure-stage-interact-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/direction-enum.hpp"


namespace heroespath
{
namespace stage
{

    const float AdventureDisplayStage::TIME_BETWEEN_MAP_MOVES_SEC_{ 0.0333f };


    AdventureDisplayStage::AdventureDisplayStage(AdventureStage * const)
    :
        Stage(
            "AdventureDisplay",
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()),
        interactStagePtr_(nullptr),
        characterListUPtr_(std::make_unique<AdventureCharacterList>(this)),
        backgroundTexture_(),
        bottomImage_(0.75f, true, sf::Color::White),
        topImage_("", true, 1.0f, 0.75f),
        mapUPtr_(),
        mapFrame_(),
        moveTimerSec_(0.0f),
        wasPressedLeft_(false),
        wasPressedRight_(false),
        wasPressedUp_(false),
        wasPressedDown_(false)
    {}


    AdventureDisplayStage::~AdventureDisplayStage()
    {}


    void AdventureDisplayStage::Setup()
    {
        Setup_CharacterList();
        Setup_BackgroundImage();
        auto const MAP_REGION{ Setup_Map() };

        interactStagePtr_ = new stage::InteractStage( * mapUPtr_, CalcInteractRegion(MAP_REGION));
        interactStagePtr_->Setup();

        //give control of Stages object lifetime to the Loop class
        game::LoopManager::Instance()->AddStage(this);
        game::LoopManager::Instance()->AddStage(interactStagePtr_);
    }


    void AdventureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);
        target.draw(bottomImage_, STATES);
        target.draw(topImage_, STATES);
        target.draw( * characterListUPtr_, STATES);
        target.draw( * mapUPtr_, STATES);
        target.draw(mapFrame_, STATES);
        Stage::Draw(target, STATES);
    }


    void AdventureDisplayStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        mapUPtr_->Update(ELAPSED_TIME_SECONDS);

        //don't process map moves every frame to save resources
        moveTimerSec_ += ELAPSED_TIME_SECONDS;
        if (moveTimerSec_ > TIME_BETWEEN_MAP_MOVES_SEC_)
        {
            moveTimerSec_ = 0.0f;

            HandleMovementKeypresses(
                sfml_util::Direction::Left,
                wasPressedLeft_,
                game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Left));

            HandleMovementKeypresses(
                sfml_util::Direction::Right,
                wasPressedRight_,
                game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Right));

            HandleMovementKeypresses(
                sfml_util::Direction::Up,
                wasPressedUp_,
                game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Up));

            HandleMovementKeypresses(
                sfml_util::Direction::Down,
                wasPressedDown_,
                game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Down));

            mapUPtr_->MoveNonPlayers();
        }
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
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2"));

        sfml_util::FlipVert(backgroundTexture_);

        backgroundSprite_.setTexture(backgroundTexture_);
        backgroundSprite_.setPosition(0.0f, 0.0f);

        backgroundSprite_.setScale(
            sfml_util::Display::Instance()->GetWinWidth() /
                backgroundSprite_.getLocalBounds().width,
            sfml_util::Display::Instance()->GetWinHeight() /
                backgroundSprite_.getLocalBounds().height);
    }


    const sf::FloatRect AdventureDisplayStage::Setup_Map()
    {
        const sf::FloatRect MAP_OUTER_REGION(
            sfml_util::MapByRes(75.0f, 100.0f),
            topImage_.Bottom() - sfml_util::MapByRes(25.0f, 75.0f),
            sfml_util::MapByRes(500.0f, 3000.0f),
            sfml_util::MapByRes(250.0f, 2500.0f));

        auto const MAP_INNER_REGION{ mapFrame_.Setup(
            MAP_OUTER_REGION,
            sf::Color(239, 220, 234)) };

        mapUPtr_ = std::make_unique<map::Map>(MAP_INNER_REGION);
        mapUPtr_->Load(map::Level::Thornberry_GuardPostWest, map::Level::Thornberry);

        return MAP_OUTER_REGION;
    }


    void AdventureDisplayStage::HandleMovementKeypresses(
        const sfml_util::Direction::Enum DIRECTION,
        bool & wasPressed,
        const bool IS_PRESSED)
    {
        if ((false == wasPressed) && IS_PRESSED)
        {
            mapUPtr_->SetPlayerWalkAnim(DIRECTION, true);
        }
        else if (wasPressed && (IS_PRESSED == false))
        {
            mapUPtr_->SetPlayerWalkAnim(DIRECTION, false);

            if ((DIRECTION == sfml_util::Direction::Up) ||
                (DIRECTION == sfml_util::Direction::Down))
            {
                if (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Left))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Left, true);
                }
                else if (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Right))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Right, true);
                }
                else if ((DIRECTION == sfml_util::Direction::Up) &&
                    (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Down)))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Down, true);
                }
                else if ((DIRECTION == sfml_util::Direction::Down) &&
                    (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Up)))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Up, true);
                }
            }
            else
            {
                if (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Up))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Up, true);
                }
                else if (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Down))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Down, true);
                }
                else if ((DIRECTION == sfml_util::Direction::Left) &&
                    (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Right)))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Right, true);
                }
                else if ((DIRECTION == sfml_util::Direction::Right) &&
                    (game::LoopManager::Instance()->IsKeyPressed(sf::Keyboard::Left)))
                {
                    mapUPtr_->SetPlayerWalkAnim(sfml_util::Direction::Left, true);
                }
            }
        }

        if (IS_PRESSED)
        {
            mapUPtr_->MovePlayer(DIRECTION);
        }

        wasPressed = IS_PRESSED;
    }


    const sf::FloatRect AdventureDisplayStage::CalcInteractRegion(
        const sf::FloatRect & MAP_REGION) const
    {
        sf::FloatRect interactRegion{ MAP_REGION };

        auto const BETWEEN_MAP_AND_INTERACT_REGION_WIDTH{
            sfml_util::MapByRes(30.0f, 150.0f) };

        interactRegion.left =
            MAP_REGION.left +
            MAP_REGION.width +
            BETWEEN_MAP_AND_INTERACT_REGION_WIDTH;

        auto const RIGHT_MARGIN{ sfml_util::MapByRes(50.0f, 300.0f) };

        interactRegion.width =
            (StageRegion().width - interactRegion.left) - RIGHT_MARGIN;

        return interactRegion;
    }

}
}
