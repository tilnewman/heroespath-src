//
// combat-anim.cpp
//
#include "combat-anim.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"

#include "heroespath/combat/combat-display.hpp"
#include "heroespath/combat/combat-text.hpp"
#include "heroespath/combat/combat-node.hpp"
#include "heroespath/item/item.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/game-data-file.hpp"


namespace heroespath
{
namespace combat
{

    CombatAnim *        CombatAnim::instance_{ nullptr };
    CombatDisplayPtr_t  CombatAnim::combatDisplayStagePtr_{ nullptr };


    CombatAnim::CombatAnim()
    :
        SCREEN_WIDTH_               (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_              (sfml_util::Display::Instance()->GetWinHeight()),
        BATTLEFIELD_CENTERING_SPEED_(sfml_util::MapByRes(25.0f, 900.0f)),//found by experiment to be good speeds
        projAnimTextureSPtr_        (),
        projAnimSprite_             (),
        projAnimBeginPosV_          (0.0f, 0.0f),
        projAnimEndPosV_            (0.0f, 0.0f),
        projAnimWillSpin_           (false),
        deadAnimNodesPVec_          (),
        centeringAnimCombatNodePtr_ (nullptr),
        centeringAnimCreaturesPVec_ (),
        centeringAnimWillZoomOut_   (false),
        repositionAnimCreaturePtr_  (nullptr)
    {}


    CombatAnim::~CombatAnim()
    {}


    void CombatAnim::GiveCombatDisplay(CombatDisplayPtr_t combatDisplayPtr)
    {
        combatDisplayStagePtr_ = combatDisplayPtr;
    }


    CombatAnim * const CombatAnim::Instance()
    {
        if (nullptr == instance_)
        {
            instance_ = new CombatAnim();
        }

        return instance_;
    }


    void CombatAnim::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        if (projAnimTextureSPtr_.get() != nullptr)
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(projAnimSprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }
    }


    void CombatAnim::ProjectileShootAnimStart(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
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
            ss << PATH_KEY_BASE_STR << "arrow" << sfml_util::rand::Int(1, 3);
            pathKey = ss.str();
        }
        else if (WEAPON_SPTR->WeaponType() & item::weapon_type::Sling)
        {
            std::ostringstream ss;
            ss << PATH_KEY_BASE_STR << "stone" << sfml_util::rand::Int(1, 4);
            pathKey = ss.str();
        }
        else if (WEAPON_SPTR->WeaponType() & item::weapon_type::Crossbow)
        {
            pathKey = PATH_KEY_BASE_STR + "arrow4";
        }

        //load the projectile image
        sfml_util::LoadImageOrTextureSPtr(projAnimTextureSPtr_, heroespath::GameDataFile::Instance()->GetMediaPath(pathKey));

        //establish the creature positions
        sf::Vector2f creatureAttackingCenterPosV{0.0f, 0.0f};
        sf::Vector2f creatureDefendingCenterPosV{0.0f, 0.0f};
        CombatNodePVec_t combatNodePVec;
        combatDisplayStagePtr_->CombatTree().GetCombatNodes(combatNodePVec);
        for (auto const & NEXT_COMBANODE_SPTR : combatNodePVec)
        {
            if (NEXT_COMBANODE_SPTR->Creature() == CREATURE_ATTACKING_CPTRC)
            {
                creatureAttackingCenterPosV.x = NEXT_COMBANODE_SPTR->GetEntityRegion().left + NEXT_COMBANODE_SPTR->GetEntityRegion().width * 0.5f;
                creatureAttackingCenterPosV.y = NEXT_COMBANODE_SPTR->GetEntityRegion().top + NEXT_COMBANODE_SPTR->GetEntityRegion().height * 0.5f;
            }
            else if (NEXT_COMBANODE_SPTR->Creature() == CREATURE_DEFENDING_CPTRC)
            {
                creatureDefendingCenterPosV.x = NEXT_COMBANODE_SPTR->GetEntityRegion().left + NEXT_COMBANODE_SPTR->GetEntityRegion().width * 0.5f;
                creatureDefendingCenterPosV.y = NEXT_COMBANODE_SPTR->GetEntityRegion().top + NEXT_COMBANODE_SPTR->GetEntityRegion().height * 0.5f;
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


    void CombatAnim::ProjectileShootAnimUpdate(const float SLIDER_POS)
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

        if ((combatDisplayStagePtr_->BattlefieldRect().contains(projAnimSprite_.getGlobalBounds().left, projAnimSprite_.getGlobalBounds().top) == false) ||
            (combatDisplayStagePtr_->BattlefieldRect().contains(projAnimSprite_.getGlobalBounds().left + projAnimSprite_.getGlobalBounds().width, projAnimSprite_.getGlobalBounds().top + projAnimSprite_.getGlobalBounds().height) == false))
        {
            ProjectileShootAnimStop();
        }
    }


    void CombatAnim::ProjectileShootAnimStop()
    {
        projAnimTextureSPtr_.reset();
    }


    void CombatAnim::DeathAnimStart(const creature::CreaturePVec_t & KILLED_CREATURES_PVEC)
    {
        auto combatNodesPVec{ combatDisplayStagePtr_->GetCombatNodesForCreatures(KILLED_CREATURES_PVEC) };
        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            nextCombatNodePtrC->SetDead(true);
            deadAnimNodesPVec_.push_back(nextCombatNodePtrC);
        }
    }


    void CombatAnim::DeathAnimUpdate(const float SLIDER_POS)
    {
        for (auto const nextCombatNodePtrC : deadAnimNodesPVec_)
        {
            nextCombatNodePtrC->SetRotationDegrees((4.0f * 360.0f) * SLIDER_POS);
            nextCombatNodePtrC->SetRegion(1.0f - SLIDER_POS);
        }
    }


    void CombatAnim::DeathAnimStop()
    {
        //remove non-player nodes from combat tree and prepare for sliding animation
        for (auto const nextCombatNodeCPtr : deadAnimNodesPVec_)
        {
            auto const NEXT_NODE_ID{ combatDisplayStagePtr_->CombatTree().GetNodeId(nextCombatNodeCPtr) };
            auto const NEXT_NODE_SPTR{ combatDisplayStagePtr_->CombatTree().GetNodeSPtr(NEXT_NODE_ID) };
            combatDisplayStagePtr_->CombatTree().RemoveVertex(NEXT_NODE_ID, true);
            combatDisplayStagePtr_->RemoveCombatNode(NEXT_NODE_SPTR);
        }
        deadAnimNodesPVec_.clear();

        //re-position CombatNodes/Creatures on the battlefield in the slow animated way
        combatDisplayStagePtr_->PositionCombatTreeCells(true);
    }


    void CombatAnim::CenteringStart(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        centeringAnimCombatNodePtr_ = combatDisplayStagePtr_->CombatTree().GetNode(CREATURE_CPTRC);
        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }


    void CombatAnim::CenteringStart(const float TARGET_POS_X, const float TARGET_POS_Y)
    {
        centeringAnimCombatNodePtr_ = nullptr;
        combatDisplayStagePtr_->CenteringPosV( sf::Vector2f(TARGET_POS_X, TARGET_POS_Y) );
    }


    void CombatAnim::CenteringStartTargetCenterOfBatllefield()
    {
        auto const BATTLEFIELD_CENTER_V{ combatDisplayStagePtr_->GetCenterOfAllNodes() };
        CenteringStart(BATTLEFIELD_CENTER_V.x, BATTLEFIELD_CENTER_V.y);
    }


    void CombatAnim::CenteringStart(const creature::CreaturePVec_t & CREATURES_TO_CENTER_ON_PVEC)
    {
        centeringAnimCreaturesPVec_ = CREATURES_TO_CENTER_ON_PVEC;
        auto const CENTER_POS_V{ combatDisplayStagePtr_->FindCenterOfCreatures(CREATURES_TO_CENTER_ON_PVEC) };
        CenteringStart(CENTER_POS_V.x, CENTER_POS_V.y);
        centeringAnimWillZoomOut_ = combatDisplayStagePtr_->IsZoomOutRequired(centeringAnimCreaturesPVec_);
    }


    bool CombatAnim::CenteringUpdate(const float SLIDER_POS, const bool WILL_MOVE_BACKGROUND)
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

        auto const DIFF_X((combatDisplayStagePtr_->BattlefieldRect().left + (combatDisplayStagePtr_->BattlefieldRect().width * 0.5f)) - targetPosV.x);
        auto const DIFF_DIVISOR_X(SCREEN_WIDTH_ / BATTLEFIELD_CENTERING_SPEED_);
        combatDisplayStagePtr_->MoveBattlefieldHoriz((DIFF_X / DIFF_DIVISOR_X) * -1.0f * SLIDER_POS, WILL_MOVE_BACKGROUND);

        auto const DIFF_Y((combatDisplayStagePtr_->BattlefieldRect().top + (combatDisplayStagePtr_->BattlefieldRect().height * 0.5f)) - targetPosV.y);
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


    void CombatAnim::CenteringStop()
    {
        centeringAnimCombatNodePtr_ = nullptr;
        centeringAnimCreaturesPVec_.clear();
        centeringAnimWillZoomOut_ = false;
    }


    void CombatAnim::RepositionAnimStart(creature::CreaturePtr_t creaturePtr)
    {
        repositionAnimCreaturePtr_ = creaturePtr;
        CenteringStartTargetCenterOfBatllefield();
    }


    void CombatAnim::RepositionAnimUpdate(const float SLIDER_POS)
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


    void CombatAnim::RepositionAnimStop()
    {
        CenteringStop();
        repositionAnimCreaturePtr_ = nullptr;
    }

}
}
