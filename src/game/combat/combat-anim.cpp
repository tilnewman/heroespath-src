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
// combat-anim.cpp
//
#include "combat-anim.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sparks-animation.hpp"
#include "sfml-util/cloud-animation.hpp"
#include "sfml-util/sparkle-animation.hpp"
#include "sfml-util/text-animation.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/song-animation.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/combat/combat-display.hpp"
#include "game/combat/combat-text.hpp"
#include "game/combat/combat-node.hpp"
#include "game/item/item.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/algorithms.hpp"
#include "game/game-data-file.hpp"
#include "game/spell/spell-base.hpp"

#include <string>


namespace game
{
namespace combat
{

    const float ShakeAnimInfo::PAUSE_DURATION_SEC(1.0f);
    const float ShakeAnimInfo::SHAKE_DURATION_SEC(0.65f);


    ShakeAnimInfo::ShakeAnimInfo()
    :
        slider(),

        //anything larger than PAUSE_DURATION_SEC will work here
        pause_duration_timer_sec(PAUSE_DURATION_SEC + 1.0f),

        //anything larger than SHAKE_DURATION_SEC will work here
        shake_duration_timer_sec(SHAKE_DURATION_SEC + 1.0f)
    {}


    void ShakeAnimInfo::Reset(const float SLIDER_SPEED, const bool WILL_DOUBLE_SHAKE_DISTANCE)
    {
        auto const SHAKE_DISTANCE{
            CombatAnimation::ShakeAnimDistance(WILL_DOUBLE_SHAKE_DISTANCE) };

        slider.Reset(0.0f,
                     SHAKE_DISTANCE,
                     SLIDER_SPEED,
                     (SHAKE_DISTANCE * 0.5f),
                     misc::random::Bool());

        //anything larger than PAUSE_DURATION_SEC will work here
        pause_duration_timer_sec = ShakeAnimInfo::PAUSE_DURATION_SEC + 1.0f;

        shake_duration_timer_sec = 0.0f;
    }


    const float       CombatAnimation::SELECT_ANIM_SLIDER_SPEED_        { 8.0f };
    const float       CombatAnimation::ANIM_TIME_BETWEEN_FRAMES_DEFAULT_{ 0.06f };

    const std::string CombatAnimation::ANIM_MEDIA_PATH_KEY_STR_SPARKLE_
    { "media-anim-image-flash-sparkle" };

    const std::string CombatAnimation::ANIM_MEDIA_PATH_KEY_STR_SHIMMER_
    { "media-anim-images-dir-shimmer" };

    const sf::Uint8   CombatAnimation::ANIM_COLOR_ALT_VAL_              { 232 };


    CombatAnimation::CombatAnimation(const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    :
        SCREEN_WIDTH_                    (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_                   (sfml_util::Display::Instance()->GetWinHeight()),

        //found by experiment to be good speeds
        BATTLEFIELD_CENTERING_SPEED_     (sfml_util::MapByRes(25.0f, 900.0f)),

        combatDisplayStagePtr_           (COMBAT_DISPLAY_PTR),

        //any value greater than zero will work temporarily here
        slider_                          (1.0f),

        projAnimTexture_                 (),
        projAnimSprite_                  (),
        projAnimBeginPosV_               (0.0f, 0.0f),
        projAnimEndPosV_                 (0.0f, 0.0f),
        projAnimWillSpin_                (false),
        projAnimWillDraw_                (false),
        deadAnimNodesPVec_               (),
        centeringAnimCombatNodePtr_      (nullptr),
        centeringAnimCreaturesPVec_      (),
        centeringAnimWillZoomOut_        (false),
        repositionAnimCreaturePtr_       (nullptr),
        repositionAnimPosV_              (0.0f, 0.0f),
        meleeMoveAnimOrigPosV_           (0.0f, 0.0f),
        meleeMoveAnimTargetPosV_         (0.0f, 0.0f),
        meleeMoveAnimMovingCombatNodePtr_(nullptr),
        meleeMoveAnimTargetCombatNodePtr_(nullptr),
        shakeAnimCreatureWasCPtr_        (nullptr),
        shakeAnimCreatureWasSpeed_       (0.0f),
        shakeAnimInfoMap_                (),
        selectAnimCombatNodePtr_         (nullptr),
        sparksAnimUVec_                  (),
        cloudAnimUVec_                   (),
        animUVec_                        (),
        songAnimUVec_                    (),
        sparkleAnimUVec_                 (),
        textAnimUVec_                    (),
        runAnimCombatNodePtr_            (nullptr),
        runAnimPosVTarget_               (0.0f, 0.0f),
        runAnimPosVOrig_                 (0.0f, 0.0f)
    {}


    void CombatAnimation::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (projAnimWillDraw_)
        {
            sf::RenderStates tempStates(STATES);
            tempStates.blendMode = sf::BlendAdd;
            target.draw(projAnimSprite_, tempStates);
        }

        for (auto const & NEXT_SPARKSANIM_UPTR : sparksAnimUVec_)
        {
            NEXT_SPARKSANIM_UPTR->draw(target, STATES);
        }

        for (auto const & NEXT_CLOUDANIM_UPTR : cloudAnimUVec_)
        {
            NEXT_CLOUDANIM_UPTR->draw(target, STATES);
        }

        for (auto const & NEXT_ANIM_UPTR : animUVec_)
        {
            if (NEXT_ANIM_UPTR->IsFinished() == false)
            {
                NEXT_ANIM_UPTR->draw(target, STATES);
            }
        }

        for (auto & NEXT_SONGANIM_PTR : songAnimUVec_)
        {
            if (NEXT_SONGANIM_PTR->IsFinished() == false)
            {
                NEXT_SONGANIM_PTR->draw(target, STATES);
            }
        }

        for (auto & NEXT_SPARKLEANIM_PTR : sparkleAnimUVec_)
        {
            if (NEXT_SPARKLEANIM_PTR->IsFinished() == false)
            {
                NEXT_SPARKLEANIM_PTR->draw(target, STATES);
            }
        }

        for (auto const & NEXT_TEXTANIM_UPTR : textAnimUVec_)
        {
            if (NEXT_TEXTANIM_UPTR->IsFinished() == false)
            {
                NEXT_TEXTANIM_UPTR->draw(target, STATES);
            }
        }
    }


    void CombatAnimation::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        bool willRemoveNullShakeInfo(false);
        for (auto & nextShakeInfoPair : shakeAnimInfoMap_)
        {
            if (nextShakeInfoPair.first == nullptr)
            {
                willRemoveNullShakeInfo = true;
                continue;
            }

            if (nextShakeInfoPair.second.shake_duration_timer_sec <
                ShakeAnimInfo::SHAKE_DURATION_SEC)
            {
                nextShakeInfoPair.second.shake_duration_timer_sec += ELAPSED_TIME_SECONDS;
                nextShakeInfoPair.first->SetImagePosOffset((ShakeAnimDistance(false) * -0.5f) +
                    nextShakeInfoPair.second.slider.Update(ELAPSED_TIME_SECONDS), 0.0f);

                if (nextShakeInfoPair.second.shake_duration_timer_sec >
                    ShakeAnimInfo::SHAKE_DURATION_SEC)
                {
                    nextShakeInfoPair.second.pause_duration_timer_sec = 0.0f;
                    nextShakeInfoPair.first->SetImagePosOffset(0.0f, 0.0f);
                }
            }

            if (nextShakeInfoPair.second.pause_duration_timer_sec <
                ShakeAnimInfo::PAUSE_DURATION_SEC)
            {
                nextShakeInfoPair.second.pause_duration_timer_sec += ELAPSED_TIME_SECONDS;

                if (nextShakeInfoPair.second.pause_duration_timer_sec >
                    ShakeAnimInfo::PAUSE_DURATION_SEC)
                {
                    nextShakeInfoPair.second.shake_duration_timer_sec = 0.0f;
                }
            }
        }

        if (willRemoveNullShakeInfo)
        {
            shakeAnimInfoMap_.erase(nullptr);
        }

        if (selectAnimCombatNodePtr_ != nullptr)
        {
            selectAnimCombatNodePtr_->SelectAnimUpdate( slider_.Update(ELAPSED_TIME_SECONDS) );
            if (slider_.GetIsDone())
            {
                SelectAnimStop();
            }
        }

        for (auto const & NEXT_TEXTANIM_UPTR : textAnimUVec_)
        {
            if (NEXT_TEXTANIM_UPTR->IsFinished() == false)
            {
                NEXT_TEXTANIM_UPTR->Update(ELAPSED_TIME_SECONDS);
            }
        }
    }


    void CombatAnimation::ProjectileShootAnimStart(
        creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
        creature::CreatureCPtrC_t CREATURE_DEFENDING_CPTRC,
        const item::ItemPtr_t     WEAPON_PTR,
        const bool                WILL_HIT)
    {
        projAnimWillDraw_ = true;
        projAnimWillSpin_ = ! WILL_HIT;

        //establish the game data file path key to the projectile image
        const std::string PATH_KEY_BASE_STR{"media-images-combat-"};
        std::string pathKey{ PATH_KEY_BASE_STR + "dart" };
        if (WEAPON_PTR->WeaponType() & item::weapon_type::Bow)
        {
            std::ostringstream ss;
            ss << PATH_KEY_BASE_STR << "arrow" << misc::random::Int(1, 3);
            pathKey = ss.str();
        }
        else if (WEAPON_PTR->WeaponType() & item::weapon_type::Sling)
        {
            std::ostringstream ss;
            ss << PATH_KEY_BASE_STR << "stone" << misc::random::Int(1, 4);
            pathKey = ss.str();
        }
        else if (WEAPON_PTR->WeaponType() & item::weapon_type::Crossbow)
        {
            pathKey = PATH_KEY_BASE_STR + "arrow4";
        }

        //load the projectile image
        sfml_util::LoadTexture(projAnimTexture_,
            game::GameDataFile::Instance()->GetMediaPath(pathKey));

        //establish the creature positions
        sf::Vector2f creatureAttackingCenterPosV{0.0f, 0.0f};
        sf::Vector2f creatureDefendingCenterPosV{0.0f, 0.0f};
        CombatNodePVec_t combatNodePVec;
        combatDisplayStagePtr_->CombatTreeObj().GetCombatNodes(combatNodePVec);
        for (auto const & NEXT_COMBANODE_SPTR : combatNodePVec)
        {
            auto const NEXT_REGION{ NEXT_COMBANODE_SPTR->GetEntityRegion() };
            if (NEXT_COMBANODE_SPTR->Creature() == CREATURE_ATTACKING_CPTRC)
            {
                creatureAttackingCenterPosV.x = NEXT_REGION.left + NEXT_REGION.width * 0.5f;
                creatureAttackingCenterPosV.y = NEXT_REGION.top +  NEXT_REGION.height * 0.5f;
            }
            else if (NEXT_COMBANODE_SPTR->Creature() == CREATURE_DEFENDING_CPTRC)
            {
                creatureDefendingCenterPosV.x = NEXT_REGION.left + NEXT_REGION.width * 0.5f;
                creatureDefendingCenterPosV.y = NEXT_REGION.top +  NEXT_REGION.height * 0.5f;
            }
        }

        projAnimSprite_.setTexture(projAnimTexture_, true);
        projAnimSprite_.setColor(sf::Color(255, 255, 255, 127));

        //scale the sprite down to a reasonable size
        projAnimSprite_.setOrigin(0.0f, 0.0f);

        //this is the scale for Sling projectiles (stones)
        auto scale{ sfml_util::MapByRes(0.05f, 0.55f) };

        if ((WEAPON_PTR->WeaponType() & item::weapon_type::Bow) ||
            (WEAPON_PTR->WeaponType() & item::weapon_type::Crossbow))
        {
            scale = sfml_util::MapByRes(0.3f, 2.0f);
        }
        else if (WEAPON_PTR->WeaponType() & item::weapon_type::Blowpipe)
        {
            scale = sfml_util::MapByRes(0.05f, 1.0f);
        }
        projAnimSprite_.setScale(scale, scale);

        projAnimSprite_.setOrigin(projAnimSprite_.getLocalBounds().width * 0.5f,
                                  projAnimSprite_.getLocalBounds().height * 0.5f);

        projAnimBeginPosV_ = creatureAttackingCenterPosV;

        if (WILL_HIT)
        {
            projAnimEndPosV_ = creatureDefendingCenterPosV;
        }
        else
        {
            projAnimEndPosV_ = sfml_util::ProjectToScreenEdge(
                creatureAttackingCenterPosV,
                creatureDefendingCenterPosV,
                sf::Vector2f(projAnimSprite_.getGlobalBounds().width,
                             projAnimSprite_.getGlobalBounds().height));
        }

        projAnimSprite_.setOrigin(projAnimSprite_.getGlobalBounds().width * 0.5f,
                                  projAnimSprite_.getGlobalBounds().height * 0.5f);

        projAnimSprite_.setRotation( sfml_util::GetAngleInDegrees(projAnimBeginPosV_,
                                                                  projAnimEndPosV_) );

        projAnimSprite_.setOrigin(0.0f, 0.0f);
    }


    void CombatAnimation::ProjectileShootAnimUpdate(const float SLIDER_POS)
    {
        auto const SPRITE_POS_HORIZ{ projAnimBeginPosV_.x +
            ((projAnimEndPosV_.x - projAnimBeginPosV_.x) * SLIDER_POS) };

        auto const SPRITE_POS_VERT { projAnimBeginPosV_.y +
            ((projAnimEndPosV_.y - projAnimBeginPosV_.y) * SLIDER_POS) };

        projAnimSprite_.setPosition(SPRITE_POS_HORIZ, SPRITE_POS_VERT);

        if (projAnimWillSpin_)
        {
            projAnimSprite_.setOrigin(projAnimSprite_.getGlobalBounds().width * 0.5f,
                                      projAnimSprite_.getGlobalBounds().height * 0.5f);

            projAnimSprite_.rotate(3.0f);
            projAnimSprite_.setOrigin(0.0f, 0.0f);
        }

        auto const PROJ_ANIM_SPRITE_GBOUNDS{ projAnimSprite_.getGlobalBounds() };

        if ((combatDisplayStagePtr_->BattlefieldRect().contains(
            PROJ_ANIM_SPRITE_GBOUNDS.left, PROJ_ANIM_SPRITE_GBOUNDS.top) == false) ||
            (combatDisplayStagePtr_->BattlefieldRect().contains(
                PROJ_ANIM_SPRITE_GBOUNDS.left + PROJ_ANIM_SPRITE_GBOUNDS.width,
                PROJ_ANIM_SPRITE_GBOUNDS.top + PROJ_ANIM_SPRITE_GBOUNDS.height) == false))
        {
            ProjectileShootAnimStop();
        }
    }


    void CombatAnimation::ProjectileShootAnimStop()
    {
        projAnimWillDraw_ = false;
    }


    void CombatAnimation::DeathAnimStart(const creature::CreaturePVec_t & KILLED_CREATURES_PVEC)
    {
        auto combatNodesPVec{
            combatDisplayStagePtr_->GetCombatNodesForCreatures(KILLED_CREATURES_PVEC) };

        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            nextCombatNodePtrC->SetDead(true);
            deadAnimNodesPVec_.push_back(nextCombatNodePtrC);
        }
    }


    void CombatAnimation::DeathAnimUpdate(const float SLIDER_POS)
    {
        for (auto const nextCombatNodePtrC : deadAnimNodesPVec_)
        {
            nextCombatNodePtrC->UpdateDeathAnim(SLIDER_POS);
        }
    }


    void CombatAnimation::DeathAnimStop()
    {
        combatDisplayStagePtr_->HandleDeaths(deadAnimNodesPVec_);
        deadAnimNodesPVec_.clear();
    }


    void CombatAnimation::CenteringStart(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        centeringAnimCombatNodePtr_ = combatDisplayStagePtr_->
            CombatTreeObj().GetNode(CREATURE_CPTRC);

        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }


    void CombatAnimation::CenteringStart(const float TARGET_POS_X, const float TARGET_POS_Y)
    {
        centeringAnimCombatNodePtr_ = nullptr;
        combatDisplayStagePtr_->CenteringPosV( sf::Vector2f(TARGET_POS_X, TARGET_POS_Y) );
    }


    void CombatAnimation::CenteringStartTargetCenterOfBatllefield()
    {
        auto const BATTLEFIELD_CENTER_V{ combatDisplayStagePtr_->GetCenterOfAllNodes() };
        CenteringStart(BATTLEFIELD_CENTER_V.x, BATTLEFIELD_CENTER_V.y);
    }


    void CombatAnimation::CenteringStart(
        const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC)
    {
        centeringAnimCreaturesPVec_ = CREATURES_TO_CENTER_ON_PVEC;

        auto const CENTER_POS_V{ combatDisplayStagePtr_->
            FindCenterOfCreatures(CREATURES_TO_CENTER_ON_PVEC) };

        CenteringStart(CENTER_POS_V.x, CENTER_POS_V.y);

        centeringAnimWillZoomOut_ = combatDisplayStagePtr_->
            IsZoomOutRequired(centeringAnimCreaturesPVec_);
    }


    bool CombatAnimation::CenteringUpdate(const float SLIDER_POS, const bool WILL_MOVE_BACKGROUND)
    {
        sf::Vector2f targetPosV{ 0.0f, 0.0f };
        if (centeringAnimCombatNodePtr_ == nullptr)
        {
            targetPosV = combatDisplayStagePtr_->CenteringPosV();
        }
        else
        {
            auto const TARGET_POS_X(centeringAnimCombatNodePtr_->GetEntityPos().x +
                (centeringAnimCombatNodePtr_->GetEntityRegion().width  * 0.5f));

            auto const TARGET_POS_Y(centeringAnimCombatNodePtr_->GetEntityPos().y +
                (centeringAnimCombatNodePtr_->GetEntityRegion().height * 0.5f));

            targetPosV = sf::Vector2f(TARGET_POS_X, TARGET_POS_Y);
        }

        auto const BF_RECT{ combatDisplayStagePtr_->BattlefieldRect() };

        auto const DIFF_X{ (BF_RECT.left + (BF_RECT.width * 0.5f)) - targetPosV.x };
        auto const DIFF_DIVISOR_X{ SCREEN_WIDTH_ / BATTLEFIELD_CENTERING_SPEED_ };
        auto const MOVE_AMOUNT_X{ (DIFF_X / DIFF_DIVISOR_X) * -1.0f * SLIDER_POS };

        combatDisplayStagePtr_->MoveBattlefieldHoriz(MOVE_AMOUNT_X, WILL_MOVE_BACKGROUND);

        auto const DIFF_Y{ (BF_RECT.top + (BF_RECT.height * 0.5f)) - targetPosV.y };
        auto const DIFF_DIVISOR_Y{ SCREEN_HEIGHT_ / BATTLEFIELD_CENTERING_SPEED_ };
        auto const MOVE_AMOUNT_Y{ (DIFF_Y / DIFF_DIVISOR_Y) * -1.0f * SLIDER_POS };

        combatDisplayStagePtr_->MoveBattlefieldVert(MOVE_AMOUNT_Y, WILL_MOVE_BACKGROUND);

        if (centeringAnimCombatNodePtr_ == nullptr)
        {
            return (centeringAnimWillZoomOut_ && (combatDisplayStagePtr_->
                AreAllCreaturesVisible(centeringAnimCreaturesPVec_) == false));
        }
        else
        {
            return false;
        }
    }


    void CombatAnimation::CenteringStop()
    {
        centeringAnimCombatNodePtr_ = nullptr;
        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }


    void CombatAnimation::RepositionAnimStart(creature::CreaturePtr_t creaturePtr)
    {
        repositionAnimCreaturePtr_ = creaturePtr;
        repositionAnimPosV_ = sf::Vector2f(0.0f, 0.0f);
        CenteringStart(creaturePtr);
    }


    void CombatAnimation::RepositionAnimStart(const sf::Vector2f & POS_V)
    {
        repositionAnimCreaturePtr_ = nullptr;
        repositionAnimPosV_ = POS_V;
        CenteringStart(POS_V.x, POS_V.y);
    }


    void CombatAnimation::RepositionAnimUpdate(const float SLIDER_POS)
    {
        auto nodePositionTrackerMap{ combatDisplayStagePtr_->NodePositionTrackerMap() };
        for (const auto & NEXT_NODEPOSTRACK_PAIR : nodePositionTrackerMap)
        {
            const float NEW_POS_HORIZ(NEXT_NODEPOSTRACK_PAIR.second.posHorizOrig +
                (NEXT_NODEPOSTRACK_PAIR.second.horizDiff * SLIDER_POS));

            const float NEW_POS_VERT(NEXT_NODEPOSTRACK_PAIR.second.posVertOrig +
                (NEXT_NODEPOSTRACK_PAIR.second.vertDiff * SLIDER_POS));

            NEXT_NODEPOSTRACK_PAIR.first->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
        }

        if (nullptr == repositionAnimCreaturePtr_)
        {
            CenteringStart(repositionAnimPosV_.x, repositionAnimPosV_.y);
        }
        else
        {
            CenteringStart(repositionAnimCreaturePtr_);
        }

        //not sure why 1.0f does not work here and 10.0f is needed instead -zTn 2017-4-28
        CenteringUpdate(10.0f, false);

        combatDisplayStagePtr_->UpdateWhichNodesWillDraw();
    }


    void CombatAnimation::RepositionAnimStop()
    {
        CenteringStop();
        repositionAnimCreaturePtr_ = nullptr;
        repositionAnimPosV_ = sf::Vector2f(0.0f, 0.0f);
    }


    void CombatAnimation::MeleeMoveTowardAnimStart(creature::CreatureCPtrC_t CREATURE_MOVING_CPTRC,
                                                   creature::CreatureCPtrC_t CREATURE_TARGET_CPTRC)
    {
        meleeMoveAnimMovingCombatNodePtr_ = combatDisplayStagePtr_->
            GetCombatNodeForCreature(CREATURE_MOVING_CPTRC);

        meleeMoveAnimOrigPosV_ = meleeMoveAnimMovingCombatNodePtr_->GetEntityPos();

        meleeMoveAnimTargetCombatNodePtr_ = combatDisplayStagePtr_->
            GetCombatNodeForCreature(CREATURE_TARGET_CPTRC);

        meleeMoveAnimTargetPosV_ = meleeMoveAnimTargetCombatNodePtr_->GetEntityPos();

        if (meleeMoveAnimOrigPosV_.x < meleeMoveAnimTargetPosV_.x)
        {
            meleeMoveAnimTargetPosV_.x -= (
                meleeMoveAnimTargetCombatNodePtr_->GetEntityRegion().width * 0.65f);
        }
        else
        {
            meleeMoveAnimTargetPosV_.x += (
                meleeMoveAnimTargetCombatNodePtr_->GetEntityRegion().width * 0.65f);
        }
    }


    void CombatAnimation::MeleeMoveTowardAnimUpdate(const float SLIDER_POS)
    {
        auto const NEW_POS_HORIZ{ meleeMoveAnimOrigPosV_.x +
            ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x) * SLIDER_POS) };

        auto const NEW_POS_VERT { meleeMoveAnimOrigPosV_.y +
            ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y) * SLIDER_POS) };

        meleeMoveAnimMovingCombatNodePtr_->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
    }


    void CombatAnimation::MeleeMoveTowardAnimStop()
    {}


    void CombatAnimation::MeleeMoveBackAnimStart()
    {
        auto const TEMP_TARGET_POS_V{ meleeMoveAnimTargetPosV_ };
        meleeMoveAnimTargetPosV_ = meleeMoveAnimOrigPosV_;
        meleeMoveAnimOrigPosV_ = TEMP_TARGET_POS_V;
    }


    void CombatAnimation::MeleeMoveBackAnimUpdate(const float SLIDER_POS)
    {
        auto const NEW_POS_HORIZ{ meleeMoveAnimOrigPosV_.x +
            ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x) * SLIDER_POS) };

        auto const NEW_POS_VERT{ meleeMoveAnimOrigPosV_.y +
            ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y) * SLIDER_POS) };

        meleeMoveAnimMovingCombatNodePtr_->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
    }


    void CombatAnimation::MeleeMoveBackAnimStop()
    {
        meleeMoveAnimOrigPosV_ = sf::Vector2f(0.0f, 0.0f);
        meleeMoveAnimTargetPosV_ = sf::Vector2f(0.0f, 0.0f);
        meleeMoveAnimMovingCombatNodePtr_ = nullptr;
    }


    void CombatAnimation::ImpactAnimStart(const float CREATURE_SHAKE_SLIDER_SPEED)
    {
        if ((meleeMoveAnimMovingCombatNodePtr_ != nullptr) &&
            (meleeMoveAnimTargetCombatNodePtr_ != nullptr))
        {
            ShakeAnimStart(meleeMoveAnimMovingCombatNodePtr_->Creature(),
                           CREATURE_SHAKE_SLIDER_SPEED,
                           true);

            ShakeAnimStart(meleeMoveAnimTargetCombatNodePtr_->Creature(),
                           CREATURE_SHAKE_SLIDER_SPEED,
                           true);
        }
    }


    void CombatAnimation::ImpactAnimUpdate(const float)
    {}


    void CombatAnimation::ImpactAnimStop()
    {
        //stop ALL creature shaking
        ShakeAnimStop(nullptr);
    }


    float CombatAnimation::ShakeAnimDistance(const bool WILL_DOUBLE)
    {
        auto distance{ sfml_util::MapByRes(16.0f, 48.0f) };

        if (WILL_DOUBLE)
        {
            distance *= 2.0f;
        }

        return distance;
    }


    void CombatAnimation::ShakeAnimStart(creature::CreatureCPtrC_t CREATURE_CPTRC,
                                    const float               SLIDER_SPEED,
                                    const bool                WILL_DOUBLE_SHAKE_DISTANCE)
    {
        auto combatNodePtr{ combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_CPTRC) };
        if (combatNodePtr != nullptr)
        {
            shakeAnimInfoMap_[combatNodePtr].Reset(SLIDER_SPEED, WILL_DOUBLE_SHAKE_DISTANCE);
            shakeAnimCreatureWasSpeed_ = SLIDER_SPEED;
        }
    }


    void CombatAnimation::ShakeAnimStop(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        CombatNodePVec_t combatNodesToErasePVec;
        for (auto & nextShakeInfoPair : shakeAnimInfoMap_)
        {
            //if given a nullptr then stop all shaking
            if ((CREATURE_CPTRC == nullptr) ||
                (nextShakeInfoPair.first->Creature() == CREATURE_CPTRC))
            {
                nextShakeInfoPair.first->SetImagePosOffset(0.0f, 0.0f);
                combatNodesToErasePVec.push_back(nextShakeInfoPair.first);
            }
        }

        for (auto const NEXT_COMBATNODE_PTR : combatNodesToErasePVec)
        {
            shakeAnimInfoMap_.erase(NEXT_COMBATNODE_PTR);
        }
    }


    void CombatAnimation::ShakeAnimTemporaryStop(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        for (auto & nextShakeInfoPair : shakeAnimInfoMap_)
        {
            if (nextShakeInfoPair.first->Creature() == CREATURE_CPTRC)
            {
                shakeAnimCreatureWasCPtr_ = nextShakeInfoPair.first->Creature();
                ShakeAnimStop(shakeAnimCreatureWasCPtr_);
                break;
            }
        }
    }


    void CombatAnimation::ShakeAnimRestart()
    {
        ShakeAnimStart(shakeAnimCreatureWasCPtr_, shakeAnimCreatureWasSpeed_, false);
        shakeAnimCreatureWasCPtr_ = nullptr;
    }


    void CombatAnimation::SelectAnimStart(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        selectAnimCombatNodePtr_ = combatDisplayStagePtr_->
            GetCombatNodeForCreature(CREATURE_CPTRC);

        if (selectAnimCombatNodePtr_ != nullptr)
        {
            selectAnimCombatNodePtr_->SelectAnimStart();
            slider_.Reset(SELECT_ANIM_SLIDER_SPEED_);
        }
    }


    void CombatAnimation::SelectAnimStop()
    {
        if (selectAnimCombatNodePtr_ != nullptr)
        {
            selectAnimCombatNodePtr_->SelectAnimStop();
            selectAnimCombatNodePtr_ = nullptr;
        }
    }


    void CombatAnimation::SpellAnimStart(const spell::SpellPtr_t          SPELL_PTR,
                                         const creature::CreatureCPtrC_t  CASTING_CREATURE_CPTRC,
                                         const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((TARGETS_PVEC.empty() == false),
            "game::combat::CombatAnimation::SpellAnimStart(spell=" << SPELL_PTR->Name()
            << ", caster=" << CASTING_CREATURE_CPTRC->Name()
            << ") was given an empty targets vec.");

        switch (SPELL_PTR->Which())
        {
            case spell::Spells::Sparks:
            {
                SparksAnimStart(CASTING_CREATURE_CPTRC, TARGETS_PVEC);
                return;
            }

            case spell::Spells::Bandage:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::FlashSparkle,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ + 0.015f,
                                sf::Color::White,
                                sf::Color(255, ANIM_COLOR_ALT_VAL_, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Sleep:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::Shimmer,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ + 0.03f,
                                sf::Color::White,
                                sf::Color(255, ANIM_COLOR_ALT_VAL_, 255));
                return;
            }

            case spell::Spells::Awaken:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::FlashSparkle,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ + 0.015f,
                                sf::Color::White,
                                sf::Color(255, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Trip:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::Shimmer,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ - 0.03f,
                                sf::Color::White,
                                sf::Color(255, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Lift:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::FlashSparkle,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_,
                                sf::Color::White,
                                sf::Color(ANIM_COLOR_ALT_VAL_, 255, 255));
                return;
            }

            case spell::Spells::Daze:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::Shimmer,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_,
                                sf::Color::White,
                                sf::Color(255, ANIM_COLOR_ALT_VAL_, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Panic:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::Shimmer,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ + 0.015f,
                                sf::Color::White,
                                sf::Color(255, ANIM_COLOR_ALT_VAL_, 255));
                return;
            }

            case spell::Spells::ClearMind:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::FlashSparkle,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ - 0.015f,
                                sf::Color::White,
                                sf::Color::White);
                return;
            }

            case spell::Spells::Poison:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::Shimmer,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_ + 0.03f,
                                sf::Color::White,
                                sf::Color(ANIM_COLOR_ALT_VAL_, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Antidote:
            {
                SetupAnimations(TARGETS_PVEC,
                                sfml_util::Animations::FlashSparkle,
                                ANIM_TIME_BETWEEN_FRAMES_DEFAULT_,
                                sf::Color::White,
                                sf::Color(ANIM_COLOR_ALT_VAL_, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::PoisonCloud:
            {
                PoisonCloudAnimStart(TARGETS_PVEC);
                return;
            }

            case spell::Spells::Count:
            default:
            {
                break;
            }
        }

        M_HP_LOG_ERR("game::combat::CombatAnimation::SpellAnimStart(spell=" << SPELL_PTR->Name()
            << ", caster=" << CASTING_CREATURE_CPTRC->Name() << ", targets=" << TARGETS_PVEC.size()
            << ") failed to trigger the start of any animation.");
    }


    bool CombatAnimation::SpellAnimUpdate(const spell::SpellPtr_t SPELL_PTR,
                                          const float             ELAPSED_TIME_SEC)
    {
        if (SPELL_PTR->Which() == spell::Spells::Sparks)
        {
            return SparksAnimUpdate(ELAPSED_TIME_SEC);
        }
        else if (SPELL_PTR->Which() == spell::Spells::PoisonCloud)
        {
            return PoisonCloudAnimUpdate(ELAPSED_TIME_SEC);
        }
        else
        {
            auto areAllAnimsFinished{ true };
            for (auto & nextAnimUPtr : animUVec_)
            {
                if (nextAnimUPtr->IsFinished() == false)
                {
                    areAllAnimsFinished = false;
                    nextAnimUPtr->UpdateTime(ELAPSED_TIME_SEC);
                }
            }

            return areAllAnimsFinished;
        }
    }


    void CombatAnimation::SpellAnimStop(const spell::SpellPtr_t SPELL_PTR)
    {
        if (SPELL_PTR->Which() == spell::Spells::Sparks)
        {
            SparksAnimStop();
        }
        else if (SPELL_PTR->Which() == spell::Spells::PoisonCloud)
        {
            PoisonCloudAnimStop();
        }
        else
        {
            animUVec_.clear();
        }
    }


    void CombatAnimation::SparksAnimStart(const creature::CreatureCPtrC_t  CASTING_CREATURE_CPTRC,
                                          const combat::CombatNodePVec_t & COMBAT_NODE_PVEC)
    {
        sparksAnimUVec_.clear();

        for (auto const NEXT_COMBATNODE_PTR : COMBAT_NODE_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->GetEntityWillDraw())
            {
                sparksAnimUVec_.push_back( std::make_unique<sfml_util::animation::SparksAnimation>(
                    ! NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter(),
                    NEXT_COMBATNODE_PTR->GetEntityRegion(),
                    0.33f,
                    sfml_util::MapByRes(0.15f, 0.45f),
                    0.9f,
                    CASTING_CREATURE_CPTRC->RankRatio(),
                    1.25f) );
            }
        }
    }


    bool CombatAnimation::SparksAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSparksAnimFinished{ true };
        for (auto & nextSparksAnimUPtr : sparksAnimUVec_)
        {
            if (nextSparksAnimUPtr->IsFinished() == false)
            {
                areAllSparksAnimFinished = false;
                nextSparksAnimUPtr->Update(ELAPSED_TIME_SEC);
            }
        }

        return areAllSparksAnimFinished;
    }


    void CombatAnimation::SparksAnimStop()
    {
        sparksAnimUVec_.clear();
    }


    void CombatAnimation::PoisonCloudAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        cloudAnimUVec_.clear();

        for (auto const NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->GetEntityWillDraw())
            {
                cloudAnimUVec_.push_back( std::make_unique<sfml_util::animation::CloudAnimation>(
                    NEXT_COMBATNODE_PTR->GetEntityRegion(),
                    0.35f,
                    sfml_util::MapByRes(0.1f, 0.35f),
                    0.2f,
                    sfml_util::MapByRes(0.4f, 1.2f),
                    0.2f,
                    4.0f,
                    0.75f,
                    3.0f,
                    1.0f,
                    0.5f,
                    1.0f,
                    0.5f) );
            }
        }
    }


    bool CombatAnimation::PoisonCloudAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllCloudAnimFinished{ true };
        for (auto & nextCloudAnimUPtr : cloudAnimUVec_)
        {
            if (nextCloudAnimUPtr->IsFinished() == false)
            {
                areAllCloudAnimFinished = false;
                nextCloudAnimUPtr->Update(ELAPSED_TIME_SEC);
            }
        }

        return areAllCloudAnimFinished;
    }


    void CombatAnimation::PoisonCloudAnimStop()
    {
        cloudAnimUVec_.clear();
    }


    void CombatAnimation::SetupAnimations(
        const combat::CombatNodePVec_t &  TARGETS_PVEC,
        const sfml_util::Animations::Enum ENUM,
        const float                       FRAME_DELAY_SEC,
        const sf::Color &                 COLOR_FROM,
        const sf::Color &                 COLOR_TO)
    {
        animUVec_.clear();

        for (auto const NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->GetEntityWillDraw() == false)
            {
                continue;
            }

            auto region{ sfml_util::MakeMinimalSquareAndCenter(
                NEXT_COMBATNODE_PTR->GetEntityRegion()) };

            //grow the shimmer animation to better cover the creature image
            if (ENUM == sfml_util::Animations::Shimmer)
            {
                auto const ADJ{ sfml_util::MapByRes(40.0f, 120.0f) };
                region.left -= ADJ * 0.5f;
                region.top -= ADJ * 1.25f;
                region.width += ADJ * 2.0f;
                region.height += ADJ * 2.0f;
            }

            animUVec_.push_back( sfml_util::AnimationFactory::Make(
                ENUM,
                region,
                FRAME_DELAY_SEC,
                COLOR_FROM,
                COLOR_TO) );
        }
    }


    void CombatAnimation::SongAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        songAnimUVec_.clear();

        for (auto const NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->GetEntityWillDraw() == false)
            {
                continue;
            }

            songAnimUVec_.push_back(
                std::make_unique<sfml_util::animation::SongAnimation>(
                    NEXT_COMBATNODE_PTR->GetEntityRegion(),
                    0.1f,
                    sfml_util::MapByRes(0.1f, 0.25f),
                    0.25f,
                    sfml_util::MapByRes(0.35f, 0.9f),
                    0.25f,
                    6.0f,
                    0.75f,
                    5.0f,
                    1.25f,
                    0.5f,
                    1.0f,
                    0.0f) );
        }
    }


    bool CombatAnimation::SongAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSongAnimFinished{ true };
        for (auto & nextSongAnimUPtr : songAnimUVec_)
        {
            if (nextSongAnimUPtr->IsFinished() == false)
            {
                areAllSongAnimFinished = false;
                nextSongAnimUPtr->Update(ELAPSED_TIME_SEC);
            }
        }

        return areAllSongAnimFinished;
    }


    void CombatAnimation::SongAnimStop()
    {
        songAnimUVec_.clear();
    }


    void CombatAnimation::SparkleAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        sparkleAnimUVec_.clear();

        for (auto const NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->GetEntityWillDraw() == false)
            {
                continue;
            }

            sparkleAnimUVec_.push_back( std::make_unique<sfml_util::animation::SparkleAnimation>(
                NEXT_COMBATNODE_PTR->GetEntityRegion(),
                sfml_util::MapByRes(0.333f, 1.33f),
                0.5f,
                10.0f,
                0.5f,
                3.0f,
                4.0f,
                0.5f) );
        }
    }


    bool CombatAnimation::SparkleAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSparkleAnimFinished{ true };
        for (auto & nextSparkleAnimUPtr : sparkleAnimUVec_)
        {
            if (nextSparkleAnimUPtr->IsFinished() == false)
            {
                areAllSparkleAnimFinished = false;
                nextSparkleAnimUPtr->Update(ELAPSED_TIME_SEC);
            }
        }

        return areAllSparkleAnimFinished;
    }


    bool CombatAnimation::SparkleAnimIsDone()
    {
        auto areAllAnimsDone{ true };
        for (auto const & NEXT_SPARKLEANIM_UPTR : sparkleAnimUVec_)
        {
            if ((NEXT_SPARKLEANIM_UPTR.get() != nullptr) &&
                (NEXT_SPARKLEANIM_UPTR->IsFinished() == false))
            {
                areAllAnimsDone = false;
            }
        }

        return areAllAnimsDone;
    }


    void CombatAnimation::SparkleAnimStop()
    {
        sparkleAnimUVec_.clear();
    }


    void CombatAnimation::TextAnimStart(const stats::Health_t &       DAMAGE,
                                        const combat::CombatNodePtr_t TARGET_PTR)
    {
        TextAnimStart(std::vector<stats::Health_t>(1, DAMAGE),
            combat::CombatNodePVec_t(1, TARGET_PTR));
    }


    void CombatAnimation::TextAnimStart(const std::vector<stats::Health_t> & DAMAGE_VEC,
                                        const combat::CombatNodePVec_t &     TARGETS_PVEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((DAMAGE_VEC.size() == TARGETS_PVEC.size()),
            "game::combat::CombatAnimation::TextAnimStart(damage_vec_size="
            << DAMAGE_VEC.size() << ", combat_node_vec_size=" << TARGETS_PVEC.size()
            << ") sizes do not match.");

        textAnimUVec_.clear();

        std::size_t damageIndex{ 0 };
        for (auto const NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            auto const NEXT_DAMAGE_VALUE{ DAMAGE_VEC[damageIndex++] };

            if ((NEXT_DAMAGE_VALUE == 0) ||
                (NEXT_COMBATNODE_PTR->GetEntityWillDraw() == false))
            {
                continue;
            }

            std::ostringstream ss;
            sf::Color startColor(255, 0, 0);
            sf::Color endColor(255, 0, 0, 0);
            if (NEXT_DAMAGE_VALUE > 0)
            {
                ss << "+";
                startColor.g = 232;
                startColor.b = 232;
                endColor.g = 232;
                endColor.b = 232;
            }

            ss << NEXT_DAMAGE_VALUE;

            textAnimUVec_.push_back( std::make_unique<sfml_util::animation::TextAnimation>(
                ss.str(),
                NEXT_COMBATNODE_PTR->GetEntityRegion(),
                1.0f,
                1,
                sfml_util::FontManager::Instance()->Size_Larger() + 64,
                startColor,
                endColor) );
        }
    }


    void CombatAnimation::TextAnimStop()
    {
        textAnimUVec_.clear();
    }


    void CombatAnimation::RunAnimStart(const creature::CreaturePtr_t CREATURE_PTR)
    {
        runAnimCombatNodePtr_ = combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_PTR);
        runAnimPosVOrig_ = runAnimCombatNodePtr_->GetEntityPos();

        auto const OFFSCREEN_PAD{ 1000.0f };
        if (CREATURE_PTR->IsPlayerCharacter())
        {
            runAnimPosVTarget_ = sf::Vector2f(0.0f - OFFSCREEN_PAD, 0.0f);
        }
        else
        {
            runAnimPosVTarget_ = sf::Vector2f(sfml_util::Display::Instance()->GetWinWidth() +
                OFFSCREEN_PAD, 0.0f);
        }
    }


    void CombatAnimation::RunAnimUpdate(const float SLIDER_POS)
    {
        auto const POS_LEFT{ runAnimPosVOrig_.x +
            ((runAnimPosVTarget_.x - runAnimPosVOrig_.x) * SLIDER_POS) };

        auto const POS_TOP{ runAnimPosVOrig_.y +
            ((runAnimPosVTarget_.y - runAnimPosVOrig_.y) * SLIDER_POS) };

        runAnimCombatNodePtr_->SetEntityPos(POS_LEFT, POS_TOP);
    }


    creature::CreaturePtr_t CombatAnimation::RunAnimStop()
    {
        auto const CREATURE_PTR{ runAnimCombatNodePtr_->Creature() };
        runAnimCombatNodePtr_ = nullptr;
        runAnimPosVOrig_ = sf::Vector2f(0.0f, 0.0f);
        runAnimPosVTarget_ = sf::Vector2f(0.0f, 0.0f);
        return CREATURE_PTR;
    }


    void CombatAnimation::EndOfCombatCleanup()
    {
        deadAnimNodesPVec_.clear();
        centeringAnimCombatNodePtr_ = nullptr;
        centeringAnimCreaturesPVec_.clear();
        repositionAnimCreaturePtr_ = nullptr;
        meleeMoveAnimMovingCombatNodePtr_ = nullptr;
        meleeMoveAnimTargetCombatNodePtr_ = nullptr;
        shakeAnimCreatureWasCPtr_ = nullptr;
        selectAnimCombatNodePtr_ = nullptr;
        runAnimCombatNodePtr_ = nullptr;
    }

}
}
