// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-anim.cpp
//
#include "combat-anim.hpp"

#include "combat/combat-display.hpp"
#include "combat/combat-node.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature.hpp"
#include "gui/animation-factory.hpp"
#include "gui/cloud-animation.hpp"
#include "gui/combat-image-enum.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/song-animation.hpp"
#include "gui/sparkle-animation.hpp"
#include "gui/sparks-animation.hpp"
#include "gui/text-animation.hpp"
#include "gui/text-region.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "sfutil/angles.hpp"
#include "sfutil/display.hpp"
#include "sfutil/size-and-scale.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace combat
{

    const float ShakeAnimInfo::PAUSE_DURATION_SEC { 1.0f };
    const float ShakeAnimInfo::SHAKE_DURATION_SEC { 0.65f };

    ShakeAnimInfo::ShakeAnimInfo()
        : slider(0.0f, 1.0f, 50.0f)

        // anything larger than PAUSE_DURATION_SEC will work here
        , pause_duration_timer_sec(PAUSE_DURATION_SEC + 1.0f)

        // anything larger than SHAKE_DURATION_SEC will work here
        , shake_duration_timer_sec(SHAKE_DURATION_SEC + 1.0f)
    {}

    void ShakeAnimInfo::Reset(const float SLIDER_SPEED, const bool WILL_DOUBLE_SHAKE_DISTANCE)
    {
        const auto SHAKE_DISTANCE { CombatAnimation::ShakeAnimDistance(
            WILL_DOUBLE_SHAKE_DISTANCE) };

        slider = gui::SliderOscillator<float>(
            0.0f, SHAKE_DISTANCE, SLIDER_SPEED, (SHAKE_DISTANCE * 0.5f));

        // anything larger than PAUSE_DURATION_SEC will work here
        pause_duration_timer_sec = ShakeAnimInfo::PAUSE_DURATION_SEC + 1.0f;

        shake_duration_timer_sec = 0.0f;
    }

    const float CombatAnimation::SELECT_ANIM_SLIDER_SPEED_ { 8.0f };
    const sf::Uint8 CombatAnimation::ANIM_COLOR_ALT_VAL_ { 232 };

    CombatAnimation::CombatAnimation()
        : SCREEN_WIDTH_(gui::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(gui::Display::Instance()->GetWinHeight())
        ,

        // found by experiment to be good speeds
        BATTLEFIELD_CENTERING_SPEED_(sfutil::MapByRes(25.0f, 900.0f))
        ,

        // any value greater than zero will work temporarily here
        slider_(1.0f)
        , projAnimCachedTextureOpt_()
        , projAnimSprite_()
        , projAnimBeginPosV_(0.0f, 0.0f)
        , projAnimEndPosV_(0.0f, 0.0f)
        , projAnimWillSpin_(false)
        , projAnimWillDraw_(false)
        , deadAnimNodesPVec_()
        , centeringAnimCombatNodePtrOpt_()
        , centeringAnimCreaturesPVec_()
        , centeringAnimWillZoomOut_(false)
        , repositionAnimCombatNodePtrOpt_()
        , repositionAnimPosV_(0.0f, 0.0f)
        , meleeMoveAnimOrigPosV_(0.0f, 0.0f)
        , meleeMoveAnimTargetPosV_(0.0f, 0.0f)
        , meleeMoveAnimMovingCombatNodePtrOpt_()
        , meleeMoveAnimTargetCombatNodePtrOpt_()
        , shakeAnimWasCombatNodePtrOpt_()
        , shakeAnimCreatureWasSpeed_(0.0f)
        , shakeAnimInfoMap_()
        , selectAnimCombatNodePtrOpt_()
        , sparksAnimUVec_()
        , cloudAnimUVec_()
        , animUVec_()
        , songAnimUVec_()
        , sparkleAnimUVec_()
        , textAnimUVec_()
        , runAnimCombatNodePtrOpt_()
        , runAnimPosVTarget_(0.0f, 0.0f)
        , runAnimPosVOrig_(0.0f, 0.0f)
    {
        shakeAnimInfoMap_.Reserve(32);
    }

    void CombatAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (projAnimWillDraw_)
        {
            sf::RenderStates tempStates(states);
            tempStates.blendMode = sf::BlendAdd;
            target.draw(projAnimSprite_, tempStates);
        }

        for (const auto & NEXT_SPARKSANIM_UPTR : sparksAnimUVec_)
        {
            NEXT_SPARKSANIM_UPTR->draw(target, states);
        }

        for (const auto & NEXT_CLOUDANIM_UPTR : cloudAnimUVec_)
        {
            NEXT_CLOUDANIM_UPTR->draw(target, states);
        }

        for (const auto & NEXT_ANIM_UPTR : animUVec_)
        {
            if (NEXT_ANIM_UPTR->IsFinished() == false)
            {
                NEXT_ANIM_UPTR->draw(target, states);
            }
        }

        for (auto & NEXT_SONGANIM_PTR : songAnimUVec_)
        {
            if (NEXT_SONGANIM_PTR->IsFinished() == false)
            {
                NEXT_SONGANIM_PTR->draw(target, states);
            }
        }

        for (auto & NEXT_SPARKLEANIM_PTR : sparkleAnimUVec_)
        {
            if (NEXT_SPARKLEANIM_PTR->IsFinished() == false)
            {
                NEXT_SPARKLEANIM_PTR->draw(target, states);
            }
        }

        for (const auto & NEXT_TEXTANIM_UPTR : textAnimUVec_)
        {
            if (NEXT_TEXTANIM_UPTR->IsFinished() == false)
            {
                NEXT_TEXTANIM_UPTR->draw(target, states);
            }
        }
    }

    void CombatAnimation::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & nextShakeInfoPair : shakeAnimInfoMap_)
        {
            if (nextShakeInfoPair.second.shake_duration_timer_sec
                < ShakeAnimInfo::SHAKE_DURATION_SEC)
            {
                nextShakeInfoPair.second.shake_duration_timer_sec += ELAPSED_TIME_SECONDS;
                nextShakeInfoPair.first->SetImagePosOffset(
                    (ShakeAnimDistance(false) * -0.5f)
                        + nextShakeInfoPair.second.slider.Update(ELAPSED_TIME_SECONDS),
                    0.0f);

                if (nextShakeInfoPair.second.shake_duration_timer_sec
                    > ShakeAnimInfo::SHAKE_DURATION_SEC)
                {
                    nextShakeInfoPair.second.pause_duration_timer_sec = 0.0f;
                    nextShakeInfoPair.first->SetImagePosOffset(0.0f, 0.0f);
                }
            }

            if (nextShakeInfoPair.second.pause_duration_timer_sec
                < ShakeAnimInfo::PAUSE_DURATION_SEC)
            {
                nextShakeInfoPair.second.pause_duration_timer_sec += ELAPSED_TIME_SECONDS;

                if (nextShakeInfoPair.second.pause_duration_timer_sec
                    > ShakeAnimInfo::PAUSE_DURATION_SEC)
                {
                    nextShakeInfoPair.second.shake_duration_timer_sec = 0.0f;
                }
            }
        }

        if (selectAnimCombatNodePtrOpt_)
        {
            selectAnimCombatNodePtrOpt_.value()->SelectAnimUpdate(
                slider_.Update(ELAPSED_TIME_SECONDS));

            if (slider_.IsStopped())
            {
                SelectAnimStop();
            }
        }

        for (const auto & NEXT_TEXTANIM_UPTR : textAnimUVec_)
        {
            if (NEXT_TEXTANIM_UPTR->IsFinished() == false)
            {
                NEXT_TEXTANIM_UPTR->Update(ELAPSED_TIME_SECONDS);
            }
        }
    }

    void CombatAnimation::ProjectileShootAnimStart(
        const sf::Vector2f & CREATURE_ATTACKING_CENTER_POSV,
        const sf::Vector2f & CREATURE_DEFENDING_CENTER_POSV,
        const item::ItemPtr_t WEAPON_PTR,
        const bool WILL_HIT)
    {
        projAnimWillDraw_ = true;
        projAnimWillSpin_ = !WILL_HIT;

        const auto COMBAT_IMAGE_TYPE = [&]() {
            if (WEAPON_PTR->WeaponInfo().IsBow())
            {
                return gui::CombatImageType::Arrow;
            }
            else if (
                WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Sling)
            {
                return gui::CombatImageType::Stone;
            }
            else if (
                WEAPON_PTR->WeaponInfo().ProjectileType()
                == item::weapon::projectile_type::Crossbow)
            {
                return gui::CombatImageType::Bolt;
            }
            else
            {
                return gui::CombatImageType::Dart;
            }
        }();

        const auto IMAGE_PATH_CONFIG_FILE_KEY { gui::CombatImageType::RandomConfigFileKey(
            COMBAT_IMAGE_TYPE) };

        projAnimCachedTextureOpt_ = gui::CachedTexture(IMAGE_PATH_CONFIG_FILE_KEY);

        projAnimSprite_.setTexture(projAnimCachedTextureOpt_->Get(), true);

        projAnimSprite_.setTextureRect(
            misc::ConfigFile::Instance()->GetTextureRect(IMAGE_PATH_CONFIG_FILE_KEY));

        projAnimSprite_.setColor(sf::Color(255, 255, 255, 127));

        // scale the sprite down to a reasonable size
        projAnimSprite_.setOrigin(0.0f, 0.0f);

        auto imageScale { 1.0f };
        if ((COMBAT_IMAGE_TYPE == gui::CombatImageType::Arrow)
            || (COMBAT_IMAGE_TYPE == gui::CombatImageType::Bolt))
        {
            imageScale = sfutil::MapByRes(0.15f, 1.0f);
        }
        else if (COMBAT_IMAGE_TYPE == gui::CombatImageType::Stone)
        {
            imageScale = sfutil::MapByRes(0.05f, 0.55f);
        }
        else if (COMBAT_IMAGE_TYPE == gui::CombatImageType::Dart)
        {
            imageScale = sfutil::MapByRes(0.025f, 0.5f);
        }

        projAnimSprite_.setScale(imageScale, imageScale);

        projAnimSprite_.setOrigin(
            projAnimSprite_.getLocalBounds().width * 0.5f,
            projAnimSprite_.getLocalBounds().height * 0.5f);

        projAnimBeginPosV_ = CREATURE_ATTACKING_CENTER_POSV;

        if (WILL_HIT)
        {
            projAnimEndPosV_ = CREATURE_DEFENDING_CENTER_POSV;
        }
        else
        {
            projAnimEndPosV_ = sfutil::ProjectJustPastScreenEdge(
                CREATURE_ATTACKING_CENTER_POSV,
                CREATURE_DEFENDING_CENTER_POSV,
                sf::Vector2f(
                    projAnimSprite_.getGlobalBounds().width,
                    projAnimSprite_.getGlobalBounds().height));
        }

        projAnimSprite_.setOrigin(
            projAnimSprite_.getGlobalBounds().width * 0.5f,
            projAnimSprite_.getGlobalBounds().height * 0.5f);

        projAnimSprite_.setRotation(
            sfutil::GetAngleInDegrees(projAnimBeginPosV_, projAnimEndPosV_));

        projAnimSprite_.setOrigin(0.0f, 0.0f);
    }

    void CombatAnimation::ProjectileShootAnimUpdate(
        const float SLIDER_POS, const sf::FloatRect & BATTLEFIELD_RECT)
    {
        const auto SPRITE_POS_HORIZ {
            projAnimBeginPosV_.x + ((projAnimEndPosV_.x - projAnimBeginPosV_.x) * SLIDER_POS)
        };

        const auto SPRITE_POS_VERT { projAnimBeginPosV_.y
                                     + ((projAnimEndPosV_.y - projAnimBeginPosV_.y) * SLIDER_POS) };

        projAnimSprite_.setPosition(SPRITE_POS_HORIZ, SPRITE_POS_VERT);

        if (projAnimWillSpin_)
        {
            projAnimSprite_.setOrigin(
                projAnimSprite_.getGlobalBounds().width * 0.5f,
                projAnimSprite_.getGlobalBounds().height * 0.5f);

            projAnimSprite_.rotate(3.0f);
            projAnimSprite_.setOrigin(0.0f, 0.0f);
        }

        const auto PROJ_ANIM_SPRITE_GBOUNDS { projAnimSprite_.getGlobalBounds() };

        if ((BATTLEFIELD_RECT.contains(PROJ_ANIM_SPRITE_GBOUNDS.left, PROJ_ANIM_SPRITE_GBOUNDS.top)
             == false)
            || (BATTLEFIELD_RECT.contains(
                    PROJ_ANIM_SPRITE_GBOUNDS.left + PROJ_ANIM_SPRITE_GBOUNDS.width,
                    PROJ_ANIM_SPRITE_GBOUNDS.top + PROJ_ANIM_SPRITE_GBOUNDS.height)
                == false))
        {
            ProjectileShootAnimStop();
        }
    }

    void CombatAnimation::ProjectileShootAnimStop() { projAnimWillDraw_ = false; }

    void CombatAnimation::DeathAnimStart(const CombatNodePVec_t & KILLED_CREATURES_COMBATNODES_PVEC)
    {
        for (const auto & COMBAT_NODE_PTR : KILLED_CREATURES_COMBATNODES_PVEC)
        {
            COMBAT_NODE_PTR->SetDead(true);
            deadAnimNodesPVec_.emplace_back(COMBAT_NODE_PTR);
        }
    }

    void CombatAnimation::DeathAnimUpdate(const float SLIDER_POS)
    {
        for (const auto & COMBAT_NODE_PTR : deadAnimNodesPVec_)
        {
            COMBAT_NODE_PTR->UpdateDeathAnim(SLIDER_POS);
        }
    }

    void CombatAnimation::DeathAnimStop(const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        COMBAT_DISPLAY_PTR->HandleCombatNodeElimination(deadAnimNodesPVec_);
        deadAnimNodesPVec_.clear();
    }

    void CombatAnimation::CenteringStart(const CombatNodePtr_t COMBAT_NODE_PTR)
    {
        centeringAnimCombatNodePtrOpt_ = COMBAT_NODE_PTR;
        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }

    void CombatAnimation::CenteringStart(
        const sf::Vector2f & TARGET_POSV, const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        centeringAnimCombatNodePtrOpt_ = boost::none;
        COMBAT_DISPLAY_PTR->CenteringPosV(TARGET_POSV);
    }

    void CombatAnimation::CenteringStart(
        const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC,
        const sf::Vector2f & CENTER_POSV,
        const bool IS_ZOOMOUT_REQUIRED,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        centeringAnimCreaturesPVec_ = CREATURES_TO_CENTER_ON_PVEC;
        CenteringStart(CENTER_POSV, COMBAT_DISPLAY_PTR);
        centeringAnimWillZoomOut_ = IS_ZOOMOUT_REQUIRED;
    }

    bool CombatAnimation::CenteringUpdate(
        const float SLIDER_POS,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
        const bool WILL_MOVE_BACKGROUND)
    {
        sf::Vector2f targetPosV { 0.0f, 0.0f };
        if (centeringAnimCombatNodePtrOpt_)
        {
            const auto CENTERING_ANIM_COMBAT_NODE_PTR { centeringAnimCombatNodePtrOpt_.value() };

            const auto TARGET_POS_X(
                CENTERING_ANIM_COMBAT_NODE_PTR->GetEntityPos().x
                + (CENTERING_ANIM_COMBAT_NODE_PTR->GetEntityRegion().width * 0.5f));

            const auto TARGET_POS_Y(
                CENTERING_ANIM_COMBAT_NODE_PTR->GetEntityPos().y
                + (CENTERING_ANIM_COMBAT_NODE_PTR->GetEntityRegion().height * 0.5f));

            targetPosV = sf::Vector2f(TARGET_POS_X, TARGET_POS_Y);
        }
        else
        {
            targetPosV = COMBAT_DISPLAY_PTR->CenteringPosV();
        }

        const auto BF_RECT { COMBAT_DISPLAY_PTR->BattlefieldRect() };

        const auto DIFF_X { (BF_RECT.left + (BF_RECT.width * 0.5f)) - targetPosV.x };
        const auto DIFF_DIVISOR_X { SCREEN_WIDTH_ / BATTLEFIELD_CENTERING_SPEED_ };
        const auto MOVE_AMOUNT_X { (DIFF_X / DIFF_DIVISOR_X) * -1.0f * SLIDER_POS };

        COMBAT_DISPLAY_PTR->MoveBattlefieldHoriz(MOVE_AMOUNT_X, WILL_MOVE_BACKGROUND);

        const auto DIFF_Y { (BF_RECT.top + (BF_RECT.height * 0.5f)) - targetPosV.y };
        const auto DIFF_DIVISOR_Y { SCREEN_HEIGHT_ / BATTLEFIELD_CENTERING_SPEED_ };
        const auto MOVE_AMOUNT_Y { (DIFF_Y / DIFF_DIVISOR_Y) * -1.0f * SLIDER_POS };

        COMBAT_DISPLAY_PTR->MoveBattlefieldVert(MOVE_AMOUNT_Y, WILL_MOVE_BACKGROUND);

        if (!centeringAnimCombatNodePtrOpt_)
        {
            return (
                centeringAnimWillZoomOut_
                && (COMBAT_DISPLAY_PTR->AreAllCreaturesVisible(centeringAnimCreaturesPVec_)
                    == false));
        }
        else
        {
            return false;
        }
    }

    void CombatAnimation::CenteringStop()
    {
        centeringAnimCombatNodePtrOpt_ = boost::none;
        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }

    void CombatAnimation::RepositionAnimStart(const CombatNodePtr_t COMBAT_NODE_PTR)
    {
        repositionAnimCombatNodePtrOpt_ = COMBAT_NODE_PTR;
        repositionAnimPosV_ = sf::Vector2f(0.0f, 0.0f);
        CenteringStart(COMBAT_NODE_PTR);
    }

    void CombatAnimation::RepositionAnimStart(
        const sf::Vector2f & POS_V, const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        repositionAnimCombatNodePtrOpt_ = boost::none;
        repositionAnimPosV_ = POS_V;
        CenteringStart(POS_V, COMBAT_DISPLAY_PTR);
    }

    void CombatAnimation::RepositionAnimUpdate(
        const float SLIDER_POS, const CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        COMBAT_DISPLAY_PTR->RepositionCombatNodesBasedOnSliderPosition(SLIDER_POS);

        if (repositionAnimCombatNodePtrOpt_)
        {
            CenteringStart(repositionAnimCombatNodePtrOpt_.value());
        }
        else
        {
            CenteringStart(repositionAnimPosV_, COMBAT_DISPLAY_PTR);
        }

        // not sure why 1.0f does not work here and 10.0f is needed instead -zTn 2017-4-28
        CenteringUpdate(10.0f, COMBAT_DISPLAY_PTR, false);

        COMBAT_DISPLAY_PTR->UpdateWhichNodesWillDraw();
    }

    void CombatAnimation::RepositionAnimStop()
    {
        CenteringStop();
        repositionAnimCombatNodePtrOpt_ = boost::none;
        repositionAnimPosV_ = sf::Vector2f(0.0f, 0.0f);
    }

    void CombatAnimation::MeleeMoveTowardAnimStart(
        const CombatNodePtr_t CREATURE_MOVING_COMBAT_NODE_PTR,
        const CombatNodePtr_t CREATURE_TARGET_COMBAT_NODE_PTR)
    {
        meleeMoveAnimMovingCombatNodePtrOpt_ = CREATURE_MOVING_COMBAT_NODE_PTR;
        meleeMoveAnimOrigPosV_ = meleeMoveAnimMovingCombatNodePtrOpt_.value()->GetEntityPos();
        meleeMoveAnimTargetCombatNodePtrOpt_ = CREATURE_TARGET_COMBAT_NODE_PTR;

        const auto MELEE_MOVE_ANIM_TARGET_COMBAT_NODE_PTR {
            meleeMoveAnimTargetCombatNodePtrOpt_.value()
        };

        meleeMoveAnimTargetPosV_ = MELEE_MOVE_ANIM_TARGET_COMBAT_NODE_PTR->GetEntityPos();

        if (meleeMoveAnimOrigPosV_.x < meleeMoveAnimTargetPosV_.x)
        {
            meleeMoveAnimTargetPosV_.x
                -= (MELEE_MOVE_ANIM_TARGET_COMBAT_NODE_PTR->GetEntityRegion().width * 0.65f);
        }
        else
        {
            meleeMoveAnimTargetPosV_.x
                += (MELEE_MOVE_ANIM_TARGET_COMBAT_NODE_PTR->GetEntityRegion().width * 0.65f);
        }
    }

    void CombatAnimation::MeleeMoveTowardAnimUpdate(const float SLIDER_POS)
    {
        const auto NEW_POS_HORIZ { meleeMoveAnimOrigPosV_.x
                                   + ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x)
                                      * SLIDER_POS) };

        const auto NEW_POS_VERT { meleeMoveAnimOrigPosV_.y
                                  + ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y)
                                     * SLIDER_POS) };

        meleeMoveAnimMovingCombatNodePtrOpt_.value()->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
    }

    void CombatAnimation::MeleeMoveTowardAnimStop() {}

    void CombatAnimation::MeleeMoveBackAnimStart()
    {
        const auto TEMP_TARGET_POS_V { meleeMoveAnimTargetPosV_ };
        meleeMoveAnimTargetPosV_ = meleeMoveAnimOrigPosV_;
        meleeMoveAnimOrigPosV_ = TEMP_TARGET_POS_V;
    }

    void CombatAnimation::MeleeMoveBackAnimUpdate(const float SLIDER_POS)
    {
        const auto NEW_POS_HORIZ { meleeMoveAnimOrigPosV_.x
                                   + ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x)
                                      * SLIDER_POS) };

        const auto NEW_POS_VERT { meleeMoveAnimOrigPosV_.y
                                  + ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y)
                                     * SLIDER_POS) };

        meleeMoveAnimMovingCombatNodePtrOpt_.value()->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
    }

    void CombatAnimation::MeleeMoveBackAnimStop()
    {
        meleeMoveAnimOrigPosV_ = sf::Vector2f(0.0f, 0.0f);
        meleeMoveAnimTargetPosV_ = sf::Vector2f(0.0f, 0.0f);
        meleeMoveAnimMovingCombatNodePtrOpt_ = boost::none;
    }

    void CombatAnimation::ImpactAnimStart(const float CREATURE_SHAKE_SLIDER_SPEED)
    {
        if (meleeMoveAnimMovingCombatNodePtrOpt_ && meleeMoveAnimTargetCombatNodePtrOpt_)
        {
            ShakeAnimStart(
                meleeMoveAnimMovingCombatNodePtrOpt_.value(), CREATURE_SHAKE_SLIDER_SPEED, true);

            ShakeAnimStart(
                meleeMoveAnimTargetCombatNodePtrOpt_.value(), CREATURE_SHAKE_SLIDER_SPEED, true);
        }
    }

    void CombatAnimation::ImpactAnimUpdate(const float) {}

    void CombatAnimation::ImpactAnimStop()
    {
        // nullptr means stop ALL creature shaking
        ShakeAnimStop(boost::none);
    }

    float CombatAnimation::ShakeAnimDistance(const bool WILL_DOUBLE)
    {
        auto distance { sfutil::MapByRes(16.0f, 48.0f) };

        if (WILL_DOUBLE)
        {
            distance *= 2.0f;
        }

        return distance;
    }

    void CombatAnimation::ShakeAnimStart(
        const CombatNodePVec_t & COMBAT_NODES_PVEC,
        const float SLIDER_SPEED,
        const bool WILL_DOUBLE_SHAKE_DISTANCE)
    {
        for (const auto & COMBAT_NODE_PTR : COMBAT_NODES_PVEC)
        {
            ShakeAnimStart(COMBAT_NODE_PTR, SLIDER_SPEED, WILL_DOUBLE_SHAKE_DISTANCE);
        }
    }

    void CombatAnimation::ShakeAnimStart(
        const CombatNodePtr_t COMBAT_NODE_PTR,
        const float SLIDER_SPEED,
        const bool WILL_DOUBLE_SHAKE_DISTANCE)
    {
        shakeAnimInfoMap_[COMBAT_NODE_PTR].Reset(SLIDER_SPEED, WILL_DOUBLE_SHAKE_DISTANCE);
        shakeAnimCreatureWasSpeed_ = SLIDER_SPEED;
    }

    void CombatAnimation::ShakeAnimStop(const CombatNodePtrOpt_t COMBAT_NODE_PTR_OPT)
    {
        CombatNodePVec_t combatNodesToErasePVec;
        for (auto & nextShakeInfoPair : shakeAnimInfoMap_)
        {
            // if given a nullptr then stop all shaking
            if ((!COMBAT_NODE_PTR_OPT) || (nextShakeInfoPair.first == COMBAT_NODE_PTR_OPT.value()))
            {
                nextShakeInfoPair.first->SetImagePosOffset(0.0f, 0.0f);
                combatNodesToErasePVec.emplace_back(nextShakeInfoPair.first);
            }
        }

        for (const auto & NEXT_COMBATNODE_PTR : combatNodesToErasePVec)
        {
            shakeAnimInfoMap_.Erase(NEXT_COMBATNODE_PTR);
        }
    }

    void CombatAnimation::ShakeAnimTemporaryStop(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto FOUND_ITER { std::find_if(
            std::begin(shakeAnimInfoMap_),
            std::end(shakeAnimInfoMap_),
            [CREATURE_PTR](const auto & SHAKE_INFO_PAIR) {
                return (SHAKE_INFO_PAIR.first->Creature() == CREATURE_PTR);
            }) };

        if (FOUND_ITER != std::end(shakeAnimInfoMap_))
        {
            shakeAnimWasCombatNodePtrOpt_ = FOUND_ITER->first;
            ShakeAnimStop(shakeAnimWasCombatNodePtrOpt_);
        }
    }

    void CombatAnimation::ShakeAnimRestart()
    {
        ShakeAnimStart(shakeAnimWasCombatNodePtrOpt_.value(), shakeAnimCreatureWasSpeed_, false);
        shakeAnimWasCombatNodePtrOpt_ = boost::none;
    }

    void CombatAnimation::SelectAnimStart(const CombatNodePtr_t COMBAT_NODE_PTR)
    {
        selectAnimCombatNodePtrOpt_ = COMBAT_NODE_PTR;
        selectAnimCombatNodePtrOpt_.value()->SelectAnimStart();
        slider_ = gui::SliderZeroToOne(SELECT_ANIM_SLIDER_SPEED_);
    }

    void CombatAnimation::SelectAnimStop()
    {
        if (selectAnimCombatNodePtrOpt_)
        {
            selectAnimCombatNodePtrOpt_.value()->SelectAnimStop();
            selectAnimCombatNodePtrOpt_ = boost::none;
        }
    }

    void CombatAnimation::SpellAnimStart(
        const spell::SpellPtr_t SPELL_PTR,
        const creature::CreaturePtr_t CASTING_CREATURE_PTR,
        const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TARGETS_PVEC.empty() == false),
            "combat::CombatAnimation::SpellAnimStart(spell="
                << SPELL_PTR->Name() << ", caster=" << CASTING_CREATURE_PTR->Name()
                << ") was given an empty targets vec.");

        switch (SPELL_PTR->Which())
        {
            case spell::Spells::Sparks:
            {
                SparksAnimStart(CASTING_CREATURE_PTR, TARGETS_PVEC);
                return;
            }

            case spell::Spells::Bandage:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::FlashSparkle,
                    0.015f,
                    sf::Color::White,
                    sf::Color(255, ANIM_COLOR_ALT_VAL_, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Sleep:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::Shimmer,
                    0.03f,
                    sf::Color::White,
                    sf::Color(255, ANIM_COLOR_ALT_VAL_, 255));
                return;
            }

            case spell::Spells::Awaken:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::FlashSparkle,
                    0.015f,
                    sf::Color::White,
                    sf::Color(255, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Trip:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::Shimmer,
                    -0.03f,
                    sf::Color::White,
                    sf::Color(255, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Lift:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::FlashSparkle,
                    0.0f,
                    sf::Color::White,
                    sf::Color(ANIM_COLOR_ALT_VAL_, 255, 255));
                return;
            }

            case spell::Spells::Daze:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::Shimmer,
                    0.0f,
                    sf::Color::White,
                    sf::Color(255, ANIM_COLOR_ALT_VAL_, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Panic:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::Shimmer,
                    0.015f,
                    sf::Color::White,
                    sf::Color(255, ANIM_COLOR_ALT_VAL_, 255));
                return;
            }

            case spell::Spells::ClearMind:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::FlashSparkle,
                    -0.015f,
                    sf::Color::White,
                    sf::Color::White);
                return;
            }

            case spell::Spells::Poison:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::Shimmer,
                    0.03f,
                    sf::Color::White,
                    sf::Color(ANIM_COLOR_ALT_VAL_, 255, ANIM_COLOR_ALT_VAL_));
                return;
            }

            case spell::Spells::Antidote:
            {
                SetupAnimations(
                    TARGETS_PVEC,
                    gui::Animations::FlashSparkle,
                    0.0f,
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
            default: { break;
            }
        }

        M_HP_LOG_ERR(
            "combat::CombatAnimation::SpellAnimStart(spell="
            << SPELL_PTR->Name() << ", caster=" << CASTING_CREATURE_PTR->Name() << ", targets="
            << TARGETS_PVEC.size() << ") failed to trigger the start of any animation.");
    }

    bool CombatAnimation::SpellAnimUpdate(
        const spell::SpellPtr_t SPELL_PTR, const float ELAPSED_TIME_SEC)
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
            auto areAllAnimsFinished { true };
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

    void CombatAnimation::SparksAnimStart(
        const creature::CreaturePtr_t CASTING_CREATURE_PTR,
        const combat::CombatNodePVec_t & COMBAT_NODE_PVEC)
    {
        sparksAnimUVec_.clear();

        for (const auto & NEXT_COMBATNODE_PTR : COMBAT_NODE_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->WillDraw())
            {
                sparksAnimUVec_.emplace_back(std::make_unique<gui::animation::SparksAnimation>(
                    !NEXT_COMBATNODE_PTR->Creature()->IsPlayerCharacter(),
                    NEXT_COMBATNODE_PTR->GetEntityRegion(),
                    0.33f,
                    sfutil::MapByRes(0.15f, 0.45f),
                    0.9f,
                    CASTING_CREATURE_PTR->RankRatio(),
                    1.25f));
            }
        }
    }

    bool CombatAnimation::SparksAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSparksAnimFinished { true };
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

    void CombatAnimation::SparksAnimStop() { sparksAnimUVec_.clear(); }

    void CombatAnimation::PoisonCloudAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        cloudAnimUVec_.clear();

        for (const auto & NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->WillDraw())
            {
                cloudAnimUVec_.emplace_back(std::make_unique<gui::animation::CloudAnimation>(
                    NEXT_COMBATNODE_PTR->GetEntityRegion(),
                    0.35f,
                    sfutil::MapByRes(0.1f, 0.35f),
                    0.2f,
                    sfutil::MapByRes(0.4f, 1.2f),
                    0.2f,
                    4.0f,
                    0.75f,
                    3.0f,
                    1.0f,
                    0.5f,
                    1.0f,
                    0.5f));
            }
        }
    }

    bool CombatAnimation::PoisonCloudAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllCloudAnimFinished { true };
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

    void CombatAnimation::PoisonCloudAnimStop() { cloudAnimUVec_.clear(); }

    void CombatAnimation::SetupAnimations(
        const combat::CombatNodePVec_t & TARGETS_PVEC,
        const gui::Animations::Enum ENUM,
        const float FRAME_DELAY_SEC_ADJ,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
    {
        animUVec_.clear();

        for (const auto & NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->WillDraw() == false)
            {
                continue;
            }

            auto region { sfutil::ShrinkToSquareAndReCenterCopy(
                NEXT_COMBATNODE_PTR->GetEntityRegion()) };

            // grow the shimmer animation to better cover the creature image
            if (ENUM == gui::Animations::Shimmer)
            {
                const auto ADJ { sfutil::MapByRes(40.0f, 120.0f) };
                region.left -= ADJ * 0.5f;
                region.top -= ADJ * 1.25f;
                region.width += ADJ * 2.0f;
                region.height += ADJ * 2.0f;
            }

            animUVec_.emplace_back(gui::AnimationFactory::Make(
                ENUM, region, FRAME_DELAY_SEC_ADJ, COLOR_FROM, COLOR_TO));
        }
    }

    void CombatAnimation::SongAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        songAnimUVec_.clear();

        for (const auto & NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->WillDraw() == false)
            {
                continue;
            }

            songAnimUVec_.emplace_back(std::make_unique<gui::animation::SongAnimation>(
                NEXT_COMBATNODE_PTR->GetEntityRegion(),
                0.1f,
                sfutil::MapByRes(0.1f, 0.25f),
                0.25f,
                sfutil::MapByRes(0.35f, 0.9f),
                0.25f,
                6.0f,
                0.75f,
                5.0f,
                1.25f,
                0.5f,
                1.0f,
                0.0f));
        }
    }

    bool CombatAnimation::SongAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSongAnimFinished { true };
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

    void CombatAnimation::SongAnimStop() { songAnimUVec_.clear(); }

    void CombatAnimation::SparkleAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        sparkleAnimUVec_.clear();

        for (const auto & NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            if (NEXT_COMBATNODE_PTR->WillDraw() == false)
            {
                continue;
            }

            sparkleAnimUVec_.emplace_back(std::make_unique<gui::animation::SparkleAnimation>(
                NEXT_COMBATNODE_PTR->GetEntityRegion(),
                sfutil::MapByRes(0.333f, 1.0f),
                0.5f,
                10.0f,
                0.5f,
                3.0f,
                4.0f,
                0.5f));
        }
    }

    bool CombatAnimation::SparkleAnimUpdate(const float ELAPSED_TIME_SEC)
    {
        auto areAllSparkleAnimFinished { true };
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
        return std::any_of(
            std::begin(sparkleAnimUVec_),
            std::end(sparkleAnimUVec_),
            [](const auto & SPARKLE_ANIM_UPTR) {
                return (SPARKLE_ANIM_UPTR && (SPARKLE_ANIM_UPTR->IsFinished() == false));
            });
    }

    void CombatAnimation::SparkleAnimStop() { sparkleAnimUVec_.clear(); }

    void CombatAnimation::TextAnimStart(
        const Health_t & DAMAGE, const combat::CombatNodePtr_t TARGET_PTR)
    {
        TextAnimStart(std::vector<Health_t>(1, DAMAGE), combat::CombatNodePVec_t(1, TARGET_PTR));
    }

    void CombatAnimation::TextAnimStart(
        const std::vector<Health_t> & DAMAGE_VEC, const combat::CombatNodePVec_t & TARGETS_PVEC)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (DAMAGE_VEC.size() == TARGETS_PVEC.size()),
            "combat::CombatAnimation::TextAnimStart(damage_vec_size="
                << DAMAGE_VEC.size() << ", combat_node_vec_size=" << TARGETS_PVEC.size()
                << ") sizes do not match.");

        textAnimUVec_.clear();

        std::size_t damageIndex { 0 };
        for (const auto & NEXT_COMBATNODE_PTR : TARGETS_PVEC)
        {
            const auto NEXT_DAMAGE_VALUE { DAMAGE_VEC[damageIndex++] };

            if ((NEXT_DAMAGE_VALUE.IsZero()) || (NEXT_COMBATNODE_PTR->WillDraw() == false))
            {
                continue;
            }

            std::ostringstream ss;
            sf::Color startColor(255, 0, 0);
            sf::Color endColor(255, 0, 0, 0);
            if (NEXT_DAMAGE_VALUE > 0_health)
            {
                ss << "+";
                startColor.g = 232;
                startColor.b = 232;
                endColor.g = 232;
                endColor.b = 232;
            }

            ss << NEXT_DAMAGE_VALUE;

            textAnimUVec_.emplace_back(std::make_unique<gui::animation::TextAnimation>(
                ss.str(),
                NEXT_COMBATNODE_PTR->GetEntityRegion(),
                1.0f,
                1,
                gui::FontManager::Instance()->Size_Larger() + 64,
                startColor,
                endColor));
        }
    }

    void CombatAnimation::TextAnimStop() { textAnimUVec_.clear(); }

    void CombatAnimation::RunAnimStart(const CombatNodePtr_t COMBAT_NODE_PTR)
    {
        runAnimCombatNodePtrOpt_ = COMBAT_NODE_PTR;
        runAnimPosVOrig_ = runAnimCombatNodePtrOpt_.value()->GetEntityPos();

        const auto OFFSCREEN_PAD { 1000.0f };
        if (COMBAT_NODE_PTR->Creature()->IsPlayerCharacter())
        {
            runAnimPosVTarget_ = sf::Vector2f(0.0f - OFFSCREEN_PAD, 0.0f);
        }
        else
        {
            runAnimPosVTarget_
                = sf::Vector2f(gui::Display::Instance()->GetWinWidth() + OFFSCREEN_PAD, 0.0f);
        }
    }

    void CombatAnimation::RunAnimUpdate(const float SLIDER_POS)
    {
        const auto POS_LEFT { runAnimPosVOrig_.x
                              + ((runAnimPosVTarget_.x - runAnimPosVOrig_.x) * SLIDER_POS) };

        const auto POS_TOP { runAnimPosVOrig_.y
                             + ((runAnimPosVTarget_.y - runAnimPosVOrig_.y) * SLIDER_POS) };

        runAnimCombatNodePtrOpt_.value()->SetEntityPos(POS_LEFT, POS_TOP);
    }

    const creature::CreaturePtr_t CombatAnimation::RunAnimStop()
    {
        const auto CREATURE_PTR { runAnimCombatNodePtrOpt_.value()->Creature() };
        runAnimCombatNodePtrOpt_ = boost::none;
        runAnimPosVOrig_ = sf::Vector2f(0.0f, 0.0f);
        runAnimPosVTarget_ = sf::Vector2f(0.0f, 0.0f);
        return CREATURE_PTR;
    }

    void CombatAnimation::EndOfCombatCleanup()
    {
        deadAnimNodesPVec_.clear();
        centeringAnimCombatNodePtrOpt_ = boost::none;
        centeringAnimCreaturesPVec_.clear();
        repositionAnimCombatNodePtrOpt_ = boost::none;
        meleeMoveAnimMovingCombatNodePtrOpt_ = boost::none;
        meleeMoveAnimTargetCombatNodePtrOpt_ = boost::none;
        shakeAnimWasCombatNodePtrOpt_ = boost::none;
        selectAnimCombatNodePtrOpt_ = boost::none;
        runAnimCombatNodePtrOpt_ = boost::none;
    }

} // namespace combat
} // namespace heroespath
