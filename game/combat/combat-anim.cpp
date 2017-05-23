//
// combat-anim.cpp
//
#include "combat-anim.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"

#include "game/combat/combat-display.hpp"
#include "game/combat/combat-text.hpp"
#include "game/combat/combat-node.hpp"
#include "game/item/item.hpp"
#include "game/creature/creature.hpp"
#include "game/game-data-file.hpp"

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
        pause_duration_timer_sec(PAUSE_DURATION_SEC + 1.0f), //anything larger than PAUSE_DURATION_SEC will work here
        shake_duration_timer_sec(SHAKE_DURATION_SEC + 1.0f) //anything larger than SHAKE_DURATION_SEC will work here
    {}


    void ShakeAnimInfo::Reset(const float SLIDER_SPEED, const bool WILL_DOUBLE_SHAKE_DISTANCE)
    {
        auto const SHAKE_DISTANCE{ CombatAnimation::ShakeAnimDistance(WILL_DOUBLE_SHAKE_DISTANCE) };

        slider.Reset(0.0f,
                     SHAKE_DISTANCE,
                     SLIDER_SPEED,
                     (SHAKE_DISTANCE * 0.5f),
                     utilz::random::Bool());

        pause_duration_timer_sec = ShakeAnimInfo::PAUSE_DURATION_SEC + 1.0f;//anything larger than PAUSE_DURATION_SEC will work here
        shake_duration_timer_sec = 0.0f;
    }


    CombatAnimation *   CombatAnimation::instance_                  { nullptr };
    CombatDisplayPtr_t  CombatAnimation::combatDisplayStagePtr_     { nullptr };
    const float         CombatAnimation::SELECT_ANIM_SLIDER_SPEED_  { 8.0f };


    CombatAnimation::CombatAnimation()
    :
        SCREEN_WIDTH_                    (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_                   (sfml_util::Display::Instance()->GetWinHeight()),
        BATTLEFIELD_CENTERING_SPEED_     (sfml_util::MapByRes(25.0f, 900.0f)),//found by experiment to be good speeds
        slider_                          (1.0f), //any value greater than zero will work temporarily here
        projAnimTextureSPtr_             (),
        projAnimSprite_                  (),
        projAnimBeginPosV_               (0.0f, 0.0f),
        projAnimEndPosV_                 (0.0f, 0.0f),
        projAnimWillSpin_                (false),
        deadAnimNodesPVec_               (),
        centeringAnimCombatNodePtr_      (nullptr),
        centeringAnimCreaturesPVec_      (),
        centeringAnimWillZoomOut_        (false),
        repositionAnimCreaturePtr_       (nullptr),
        meleeMoveAnimOrigPosV_           (0.0f, 0.0f),
        meleeMoveAnimTargetPosV_         (0.0f, 0.0f),
        meleeMoveAnimMovingCombatNodePtr_(nullptr),
        meleeMoveAnimTargetCombatNodePtr_(nullptr),
        shakeAnimCreatureWasCPtr_        (nullptr),
        shakeAnimCreatureWasSpeed_       (0.0f),
        shakeAnimInfoMap_                (),
        selectAnimCombatNodePtr_         (nullptr)
    {}


    CombatAnimation::~CombatAnimation()
    {}


    void CombatAnimation::GiveCombatDisplay(CombatDisplayPtr_t combatDisplayPtr)
    {
        combatDisplayStagePtr_ = combatDisplayPtr;
    }


    CombatAnimation * CombatAnimation::Instance()
    {
        if (nullptr == instance_)
        {
            instance_ = new CombatAnimation();
        }

        return instance_;
    }


    void CombatAnimation::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        if (projAnimTextureSPtr_.get() != nullptr)
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(projAnimSprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
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

            if (nextShakeInfoPair.second.shake_duration_timer_sec < ShakeAnimInfo::SHAKE_DURATION_SEC)
            {
                nextShakeInfoPair.second.shake_duration_timer_sec += ELAPSED_TIME_SECONDS;
                nextShakeInfoPair.first->SetImagePosOffset((ShakeAnimDistance(false) * -0.5f) + nextShakeInfoPair.second.slider.Update(ELAPSED_TIME_SECONDS), 0.0f);

                if (nextShakeInfoPair.second.shake_duration_timer_sec > ShakeAnimInfo::SHAKE_DURATION_SEC)
                {
                    nextShakeInfoPair.second.pause_duration_timer_sec = 0.0f;
                    nextShakeInfoPair.first->SetImagePosOffset(0.0f, 0.0f);
                }
            }

            if (nextShakeInfoPair.second.pause_duration_timer_sec < ShakeAnimInfo::PAUSE_DURATION_SEC)
            {
                nextShakeInfoPair.second.pause_duration_timer_sec += ELAPSED_TIME_SECONDS;

                if (nextShakeInfoPair.second.pause_duration_timer_sec > ShakeAnimInfo::PAUSE_DURATION_SEC)
                    nextShakeInfoPair.second.shake_duration_timer_sec = 0.0f;
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
    }


    void CombatAnimation::ProjectileShootAnimStart(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                              creature::CreatureCPtrC_t CREATURE_DEFENDING_CPTRC,
                                              const item::ItemSPtr_t &  WEAPON_SPTR,
                                              const bool                WILL_HIT)
    {
        projAnimWillSpin_ = ! WILL_HIT;

        //establish the game data file path key to the projectile image
        const std::string PATH_KEY_BASE_STR{"media-images-combat-"};
        std::string pathKey{ PATH_KEY_BASE_STR + "dart" };
        if (WEAPON_SPTR->WeaponType() & item::weapon_type::Bow)
        {
            std::ostringstream ss;
            ss << PATH_KEY_BASE_STR << "arrow" << utilz::random::Int(1, 3);
            pathKey = ss.str();
        }
        else if (WEAPON_SPTR->WeaponType() & item::weapon_type::Sling)
        {
            std::ostringstream ss;
            ss << PATH_KEY_BASE_STR << "stone" << utilz::random::Int(1, 4);
            pathKey = ss.str();
        }
        else if (WEAPON_SPTR->WeaponType() & item::weapon_type::Crossbow)
        {
            pathKey = PATH_KEY_BASE_STR + "arrow4";
        }

        //load the projectile image
        sfml_util::LoadImageOrTextureSPtr(projAnimTextureSPtr_, game::GameDataFile::Instance()->GetMediaPath(pathKey));

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

        projAnimSprite_.setTexture( * projAnimTextureSPtr_, true);
        projAnimSprite_.setColor(sf::Color(255, 255, 255, 127));

        //scale the sprite down to a reasonable size
        projAnimSprite_.setOrigin(0.0f, 0.0f);
        auto scale{ sfml_util::MapByRes(0.05f, 0.55f) };//this is the scale for Sling projectiles (stones)
        if ((WEAPON_SPTR->WeaponType() & item::weapon_type::Bow) || (WEAPON_SPTR->WeaponType() & item::weapon_type::Crossbow))
        {
            scale = sfml_util::MapByRes(0.3f, 2.0f);
        }
        else if (WEAPON_SPTR->WeaponType() & item::weapon_type::Blowpipe)
        {
            scale = sfml_util::MapByRes(0.05f, 1.0f);
        }
        projAnimSprite_.setScale(scale, scale);

        projAnimSprite_.setOrigin(projAnimSprite_.getLocalBounds().width * 0.5f, projAnimSprite_.getLocalBounds().height * 0.5f);

        projAnimBeginPosV_ = creatureAttackingCenterPosV;

        if (WILL_HIT)
        {
            projAnimEndPosV_ = creatureDefendingCenterPosV;
        }
        else
        {
            projAnimEndPosV_ = sfml_util::ProjectToScreenEdge(creatureAttackingCenterPosV,
                                                              creatureDefendingCenterPosV,
                                                              sf::Vector2f(projAnimSprite_.getGlobalBounds().width, projAnimSprite_.getGlobalBounds().height));
        }

        projAnimSprite_.setOrigin(projAnimSprite_.getGlobalBounds().width * 0.5f, projAnimSprite_.getGlobalBounds().height * 0.5f);
        projAnimSprite_.setRotation( sfml_util::GetAngleInDegrees(projAnimBeginPosV_, projAnimEndPosV_) );
        projAnimSprite_.setOrigin(0.0f, 0.0f);
    }


    void CombatAnimation::ProjectileShootAnimUpdate(const float SLIDER_POS)
    {
        auto const SPRITE_POS_HORIZ{ projAnimBeginPosV_.x + ((projAnimEndPosV_.x - projAnimBeginPosV_.x) * SLIDER_POS) };
        auto const SPRITE_POS_VERT { projAnimBeginPosV_.y + ((projAnimEndPosV_.y - projAnimBeginPosV_.y) * SLIDER_POS) };
        projAnimSprite_.setPosition(SPRITE_POS_HORIZ, SPRITE_POS_VERT);

        if (projAnimWillSpin_)
        {
            projAnimSprite_.setOrigin(projAnimSprite_.getGlobalBounds().width * 0.5f, projAnimSprite_.getGlobalBounds().height * 0.5f);
            projAnimSprite_.rotate(3.0f);
            projAnimSprite_.setOrigin(0.0f, 0.0f);
        }

        auto const PROJ_ANIM_SPRITE_GBOUNDS{ projAnimSprite_.getGlobalBounds() };

        if ((combatDisplayStagePtr_->BattlefieldRect().contains(PROJ_ANIM_SPRITE_GBOUNDS.left,  PROJ_ANIM_SPRITE_GBOUNDS.top) == false) ||
            (combatDisplayStagePtr_->BattlefieldRect().contains(PROJ_ANIM_SPRITE_GBOUNDS.left + PROJ_ANIM_SPRITE_GBOUNDS.width, PROJ_ANIM_SPRITE_GBOUNDS.top + PROJ_ANIM_SPRITE_GBOUNDS.height) == false))
        {
            ProjectileShootAnimStop();
        }
    }


    void CombatAnimation::ProjectileShootAnimStop()
    {
        projAnimTextureSPtr_.reset();
    }


    void CombatAnimation::DeathAnimStart(const creature::CreaturePVec_t & KILLED_CREATURES_PVEC)
    {
        auto combatNodesPVec{ combatDisplayStagePtr_->GetCombatNodesForCreatures(KILLED_CREATURES_PVEC) };
        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            nextCombatNodePtrC->SetDead(true);
            deadAnimNodesPVec_.push_back(nextCombatNodePtrC);
        }
    }


    void CombatAnimation::DeathAnimUpdate(const float SLIDER_POS)
    {
        for (auto const nextCombatNodePtrC : deadAnimNodesPVec_)
            nextCombatNodePtrC->UpdateDeathAnim(SLIDER_POS);
    }


    void CombatAnimation::DeathAnimStop()
    {
        //remove non-player nodes from combat tree and prepare for sliding animation
        for (auto const nextCombatNodeCPtr : deadAnimNodesPVec_)
        {
            auto const NEXT_NODE_ID{ combatDisplayStagePtr_->CombatTreeObj().GetNodeId(nextCombatNodeCPtr) };
            auto const NEXT_NODE_SPTR{ combatDisplayStagePtr_->CombatTreeObj().GetNodeSPtr(NEXT_NODE_ID) };
            
            //TODO WARN THIS INVALIDATES A CombatNodePtr that could be stored in CombatAnim::shakeAnimInfoMap_ etc.
            combatDisplayStagePtr_->CombatTreeObj().RemoveVertex(NEXT_NODE_ID, true);
            combatDisplayStagePtr_->RemoveCombatNode(NEXT_NODE_SPTR);
        }
        deadAnimNodesPVec_.clear();

        //re-position CombatNodes/Creatures on the battlefield in the slow animated way
        combatDisplayStagePtr_->PositionCombatTreeCells(true);
    }


    void CombatAnimation::CenteringStart(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        centeringAnimCombatNodePtr_ = combatDisplayStagePtr_->CombatTreeObj().GetNode(CREATURE_CPTRC);
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


    void CombatAnimation::CenteringStart(const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC)
    {
        centeringAnimCreaturesPVec_ = CREATURES_TO_CENTER_ON_PVEC;
        auto const CENTER_POS_V{ combatDisplayStagePtr_->FindCenterOfCreatures(CREATURES_TO_CENTER_ON_PVEC) };
        CenteringStart(CENTER_POS_V.x, CENTER_POS_V.y);
        centeringAnimWillZoomOut_ = combatDisplayStagePtr_->IsZoomOutRequired(centeringAnimCreaturesPVec_);
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
            auto const TARGET_POS_X(centeringAnimCombatNodePtr_->GetEntityPos().x + (centeringAnimCombatNodePtr_->GetEntityRegion().width  * 0.5f));
            auto const TARGET_POS_Y(centeringAnimCombatNodePtr_->GetEntityPos().y + (centeringAnimCombatNodePtr_->GetEntityRegion().height * 0.5f));
            targetPosV = sf::Vector2f(TARGET_POS_X, TARGET_POS_Y);
        }

        auto const BF_RECT{ combatDisplayStagePtr_->BattlefieldRect() };

        auto const DIFF_X((BF_RECT.left + (BF_RECT.width * 0.5f)) - targetPosV.x);
        auto const DIFF_DIVISOR_X(SCREEN_WIDTH_ / BATTLEFIELD_CENTERING_SPEED_);
        combatDisplayStagePtr_->MoveBattlefieldHoriz((DIFF_X / DIFF_DIVISOR_X) * -1.0f * SLIDER_POS, WILL_MOVE_BACKGROUND);

        auto const DIFF_Y((BF_RECT.top + (BF_RECT.height * 0.5f)) - targetPosV.y);
        auto const DIFF_DIVISOR_Y(SCREEN_HEIGHT_ / BATTLEFIELD_CENTERING_SPEED_);
        combatDisplayStagePtr_->MoveBattlefieldVert((DIFF_Y / DIFF_DIVISOR_Y) * -1.0f * SLIDER_POS, WILL_MOVE_BACKGROUND);

        if (centeringAnimCombatNodePtr_ == nullptr)
        {
            return (centeringAnimWillZoomOut_ && (combatDisplayStagePtr_->AreAllCreaturesVisible(centeringAnimCreaturesPVec_) == false));
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
        CenteringStartTargetCenterOfBatllefield();
    }


    void CombatAnimation::RepositionAnimUpdate(const float SLIDER_POS)
    {
        auto nodePositionTrackerMap{ combatDisplayStagePtr_->NodePositionTrackerMap() };
        for (const auto & NEXT_NODEPOSTRACK_PAIR : nodePositionTrackerMap)
        {
            const float NEW_POS_HORIZ(NEXT_NODEPOSTRACK_PAIR.second.posHorizOrig + (NEXT_NODEPOSTRACK_PAIR.second.horizDiff * SLIDER_POS));
            const float NEW_POS_VERT(NEXT_NODEPOSTRACK_PAIR.second.posVertOrig + (NEXT_NODEPOSTRACK_PAIR.second.vertDiff * SLIDER_POS));
            NEXT_NODEPOSTRACK_PAIR.first->SetEntityPos(NEW_POS_HORIZ, NEW_POS_VERT);
        }

        creature::CreaturePVec_t creaturePVec{ repositionAnimCreaturePtr_ };
        CenteringStart(creaturePVec);
        CenteringUpdate(10.0f, false); //not sure why 1.0f does not work here and 10.0f is needed instead -zTn 2017-4-28

        combatDisplayStagePtr_->UpdateWhichNodesWillDraw();
    }


    void CombatAnimation::RepositionAnimStop()
    {
        CenteringStop();
        repositionAnimCreaturePtr_ = nullptr;
    }


    void CombatAnimation::MeleeMoveTowardAnimStart(creature::CreatureCPtrC_t CREATURE_MOVING_CPTRC,
                                              creature::CreatureCPtrC_t CREATURE_TARGET_CPTRC)
    {
        meleeMoveAnimMovingCombatNodePtr_ = combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_MOVING_CPTRC);
        meleeMoveAnimOrigPosV_ = meleeMoveAnimMovingCombatNodePtr_->GetEntityPos();

        meleeMoveAnimTargetCombatNodePtr_ = combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_TARGET_CPTRC);
        meleeMoveAnimTargetPosV_ = meleeMoveAnimTargetCombatNodePtr_->GetEntityPos();

        if (meleeMoveAnimOrigPosV_.x < meleeMoveAnimTargetPosV_.x)
        {
            meleeMoveAnimTargetPosV_.x -= (meleeMoveAnimTargetCombatNodePtr_->GetEntityRegion().width * 0.65f);
        }
        else
        {
            meleeMoveAnimTargetPosV_.x += (meleeMoveAnimTargetCombatNodePtr_->GetEntityRegion().width * 0.65f);
        }
    }


    void CombatAnimation::MeleeMoveTowardAnimUpdate(const float SLIDER_POS)
    {
        auto const NEW_POS_HORIZ{ meleeMoveAnimOrigPosV_.x + ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x) * SLIDER_POS) };
        auto const NEW_POS_VERT { meleeMoveAnimOrigPosV_.y + ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y) * SLIDER_POS) };
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
        auto const NEW_POS_HORIZ{ meleeMoveAnimOrigPosV_.x + ((meleeMoveAnimTargetPosV_.x - meleeMoveAnimOrigPosV_.x) * SLIDER_POS) };
        auto const NEW_POS_VERT{ meleeMoveAnimOrigPosV_.y + ((meleeMoveAnimTargetPosV_.y - meleeMoveAnimOrigPosV_.y) * SLIDER_POS) };
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
            if ((CREATURE_CPTRC == nullptr) || (nextShakeInfoPair.first->Creature() == CREATURE_CPTRC))
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
        selectAnimCombatNodePtr_ = combatDisplayStagePtr_->GetCombatNodeForCreature(CREATURE_CPTRC);
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

}
}
