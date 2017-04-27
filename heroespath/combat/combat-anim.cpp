//
// combat-anim.cpp
//
#include "combat-anim.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

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
    CombatDisplayPtr_t  CombatAnim::combatDisplayPtr_{ nullptr };


    CombatAnim::CombatAnim()
    :
        projAnimTextureSPtr_(),
        projAnimSprite_     (),
        projAnimBeginPosV_  (0.0f, 0.0f),
        projAnimEndPosV_    (0.0f, 0.0f),
        projAnimWillSpin_   (false),
        deadAnimNodesPVec_  ()
    {}


    CombatAnim::~CombatAnim()
    {}


    void CombatAnim::GiveCombatDisplay(CombatDisplayPtr_t combatDisplayPtr)
    {
        combatDisplayPtr_ = combatDisplayPtr;
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
        combatDisplayPtr_->CombatTree().GetCombatNodes(combatNodePVec);
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

        if ((combatDisplayPtr_->BattlefieldRect().contains(projAnimSprite_.getGlobalBounds().left, projAnimSprite_.getGlobalBounds().top) == false) ||
            (combatDisplayPtr_->BattlefieldRect().contains(projAnimSprite_.getGlobalBounds().left + projAnimSprite_.getGlobalBounds().width, projAnimSprite_.getGlobalBounds().top + projAnimSprite_.getGlobalBounds().height) == false))
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
        auto combatNodesPVec{ combatDisplayPtr_->GetCombatNodesForCreatures(KILLED_CREATURES_PVEC) };
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
            auto const NEXT_NODE_ID{ combatDisplayPtr_->CombatTree().GetNodeId(nextCombatNodeCPtr) };
            auto const NEXT_NODE_SPTR{ combatDisplayPtr_->CombatTree().GetNodeSPtr(NEXT_NODE_ID) };
            combatDisplayPtr_->CombatTree().RemoveVertex(NEXT_NODE_ID, true);
            combatDisplayPtr_->RemoveCombatNode(NEXT_NODE_SPTR);
        }
        deadAnimNodesPVec_.clear();

        //re-position CombatNodes/Creatures on the battlefield in the slow animated way
        combatDisplayPtr_->PositionCombatTreeCells(true);
    }

}
}
