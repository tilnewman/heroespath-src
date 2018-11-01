// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// adventure-display-stage.cpp
//
#include "adventure-display-stage.hpp"

#include "gui/direction-enum.hpp"
#include "map/level-enum.hpp"
#include "map/map-display.hpp"
#include "map/map.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"
#include "stage/adventure-stage-interact-stage.hpp"
#include "stage/adventure-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace heroespath
{
namespace stage
{

    const float AdventureDisplayStage::TIME_BETWEEN_MAP_MOVES_SEC_ { 0.0333f };

    AdventureDisplayStage::AdventureDisplayStage()
        : StageBase("AdventureDisplay", {}, false)
        , interactionManager_()
        , stageTitle_("", true, 0.0f, sfutil::ScreenRatioToPixelsVert(0.12f))
        , bottomImage_(0.75f, true, sf::Color::White)
        , MAP_OUTER_REGION_(
              sfutil::ScreenRatioToPixelsHoriz(0.0586f),
              sfutil::Bottom(stageTitle_.Region()) - sfutil::ScreenRatioToPixelsVert(0.0278f),
              sfutil::ScreenRatioToPixelsHoriz(0.4f),
              sfutil::ScreenRatioToPixelsVert(0.3f))
        , MAP_INNER_REGION_(MAP_OUTER_REGION_)
        , mapUPtr_(std::make_unique<map::Map>(MAP_INNER_REGION_, interactionManager_))
        , characterListUPtr_(std::make_unique<AdventureCharacterList>(this))
        , bgCachedTexture_(
              "media-images-backgrounds-paper-2", gui::ImageOpt::Default | gui::ImageOpt::FlipVert)
        , bgSprite_(bgCachedTexture_.Get())
        , moveTimerSec_(0.0f)
        , wasPressedLeft_(false)
        , wasPressedRight_(false)
        , wasPressedUp_(false)
        , wasPressedDown_(false)
    {}

    AdventureDisplayStage::~AdventureDisplayStage() = default;

    void AdventureDisplayStage::Setup()
    {
        Setup_CharacterList();
        Setup_BackgroundImage();
        Setup_Map();
    }

    void AdventureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(bgSprite_, STATES);
        target.draw(bottomImage_, STATES);
        target.draw(stageTitle_, STATES);
        target.draw(*characterListUPtr_, STATES);
        target.draw(*mapUPtr_, STATES);
        StageBase::Draw(target, STATES);
    }

    void AdventureDisplayStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        interactionManager_.Update();

        mapUPtr_->Update(ELAPSED_TIME_SECONDS);

        // don't process map moves every frame to save resources
        moveTimerSec_ += ELAPSED_TIME_SECONDS;
        if (moveTimerSec_ > TIME_BETWEEN_MAP_MOVES_SEC_)
        {
            moveTimerSec_ = 0.0f;

            HandleMovementKeypresses(
                gui::Direction::Left,
                wasPressedLeft_,
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left));

            HandleMovementKeypresses(
                gui::Direction::Right,
                wasPressedRight_,
                sf::Keyboard::isKeyPressed(sf::Keyboard::Right));

            HandleMovementKeypresses(
                gui::Direction::Up, wasPressedUp_, sf::Keyboard::isKeyPressed(sf::Keyboard::Up));

            HandleMovementKeypresses(
                gui::Direction::Down,
                wasPressedDown_,
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down));

            mapUPtr_->MoveNonPlayers();
        }
    }

    void AdventureDisplayStage::Setup_CharacterList()
    {
        characterListUPtr_->Setup();

        const auto CHARACTER_LIST_LEFT { (StageRegion().width * 0.5f)
                                         - (characterListUPtr_->GetEntityRegion().width * 0.5f) };

        const auto CHARACTER_LIST_TOP { StageRegion().height
                                        - characterListUPtr_->GetEntityRegion().height
                                        - bottomImage_.Region().height
                                        - sfutil::ScreenRatioToPixelsVert(0.0333f) };

        characterListUPtr_->SetEntityPos(CHARACTER_LIST_LEFT, CHARACTER_LIST_TOP);
    }

    void AdventureDisplayStage::Setup_BackgroundImage()
    {
        sfutil::SetSizeAndPos(bgSprite_, StageRegion());
    }

    void AdventureDisplayStage::Setup_Map()
    {
        mapUPtr_->Load(map::Level::Thornberry_GuardPostWest, map::Level::Thornberry);
    }

    void AdventureDisplayStage::HandleMovementKeypresses(
        const gui::Direction::Enum DIRECTION, bool & wasPressed, const bool IS_PRESSED)
    {
        if ((false == wasPressed) && IS_PRESSED)
        {
            mapUPtr_->SetPlayerWalkAnim(DIRECTION, true);
        }
        else if (wasPressed && (IS_PRESSED == false))
        {
            mapUPtr_->SetPlayerWalkAnim(DIRECTION, false);

            if ((DIRECTION == gui::Direction::Up) || (DIRECTION == gui::Direction::Down))
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Left, true);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Right, true);
                }
                else if (
                    (DIRECTION == gui::Direction::Up)
                    && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Down, true);
                }
                else if (
                    (DIRECTION == gui::Direction::Down)
                    && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Up, true);
                }
            }
            else
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Up, true);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Down, true);
                }
                else if (
                    (DIRECTION == gui::Direction::Left)
                    && (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Right, true);
                }
                else if (
                    (DIRECTION == gui::Direction::Right)
                    && (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    mapUPtr_->SetPlayerWalkAnim(gui::Direction::Left, true);
                }
            }
        }

        if (IS_PRESSED)
        {
            mapUPtr_->MovePlayer(DIRECTION);
        }

        wasPressed = IS_PRESSED;
    }

    const sf::FloatRect AdventureDisplayStage::CalcInteractRegionForStageFactory() const
    {
        sf::FloatRect interactRegion;

        const auto VERT_SPACER { sfutil::ScreenRatioToPixelsVert(0.0333f) };

        const auto BETWEEN_MAP_AND_INTERACT_REGION_WIDTH { VERT_SPACER };

        interactRegion.left = MAP_OUTER_REGION_.left + MAP_OUTER_REGION_.width
            + BETWEEN_MAP_AND_INTERACT_REGION_WIDTH;

        const auto RIGHT_MARGIN { sfutil::ScreenRatioToPixelsHoriz(0.04f) };

        interactRegion.width = (StageRegion().width - interactRegion.left) - RIGHT_MARGIN;

        const auto TOP_MARGIN { VERT_SPACER };

        interactRegion.top = MAP_OUTER_REGION_.top + TOP_MARGIN;

        const auto BOTTOM_MARGIN { TOP_MARGIN / 2.0f };

        interactRegion.height = MAP_OUTER_REGION_.height - (TOP_MARGIN + BOTTOM_MARGIN);

        return interactRegion;
    }

} // namespace stage
} // namespace heroespath
