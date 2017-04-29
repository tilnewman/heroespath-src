#ifndef HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
#define HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
//
// combat-anim.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <vector>
#include <memory>


namespace heroespath
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace creature
{
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
}
namespace combat
{

    class CombatDisplay;
    using CombatDisplayPtr_t = CombatDisplay *;

    class CombatNode;
    using CombatNodePtr_t  = CombatNode *;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;


    //Responsible for displaying combat related animations.
    class CombatAnim
    {
        //prevent copy construction
        CombatAnim(const CombatAnim &) =delete;

        //prevent copy assignment
        CombatAnim operator=(const CombatAnim &) =delete;

        //prevent non-singleton construction
        CombatAnim();

    public:
        virtual ~CombatAnim();

        static void GiveCombatDisplay(CombatDisplayPtr_t);
        static CombatAnim * const Instance();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void ProjectileShootAnimStart(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                      creature::CreatureCPtrC_t CREATURE_DEFENDING_CPTRC,
                                      const item::ItemSPtr_t &  WEAPON_SPTR,
                                      const bool                WILL_HIT);

        void ProjectileShootAnimUpdate(const float SLIDER_POS);
        void ProjectileShootAnimStop();

        //The Death Animation is only for non-player characters
        void DeathAnimStart(const creature::CreaturePVec_t &);
        void DeathAnimUpdate(const float SLIDER_POS);
        void DeathAnimStop();

        //The Centering Animation slides all creatures into the center of the
        //battlefield.
        void CenteringStart(creature::CreatureCPtrC_t);
        void CenteringStart(const float TARGET_POS_X, const float TARGET_POS_Y);
        void CenteringStartTargetCenterOfBatllefield();
        void CenteringStart(const creature::CreaturePVec_t &);

        //returns true if CombatStage should zoom out, or if (centeringAnimWillZoomOut_ && ! AreAllCreaturesVisible())
        bool CenteringUpdate(const float SLIDER_POS, const bool WILL_MOVE_BACKGROUND = true);
        
        void CenteringStop();

        //The Reposition Animation slides all creatures around after a blocking
        //position change, which has the effecting of looking like the battlefield
        //camera is moving.
        void RepositionAnimStart(creature::CreaturePtr_t);
        void RepositionAnimUpdate(const float SLIDER_POS);
        void RepositionAnimStop();

    private:
        static CombatAnim * instance_;
        static CombatDisplayPtr_t combatDisplayStagePtr_;

        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float BATTLEFIELD_CENTERING_SPEED_;

        //members supporting the Projectile Shoot Animation
        sfml_util::TextureSPtr_t projAnimTextureSPtr_;
        sf::Sprite projAnimSprite_;
        sf::Vector2f projAnimBeginPosV_;
        sf::Vector2f projAnimEndPosV_;
        bool projAnimWillSpin_;

        //members supporting the Death Animation
        CombatNodePVec_t deadAnimNodesPVec_;

        //members supporting the Centering Animation
        combat::CombatNodePtr_t centeringAnimCombatNodePtr_;
        creature::CreaturePVec_t centeringAnimCreaturesPVec_;
        bool centeringAnimWillZoomOut_;

        //member supporting the Reposition Animation
        creature::CreaturePtr_t repositionAnimCreaturePtr_;
    };

    using CombatAnimPtr_t = CombatAnim *;

}
}

#endif //HEROESPATH_COMBAT_COMBATANIM_HPP_INCLUDED
