#ifndef GAME_COMBAT_COMBATANIM_HPP_INCLUDED
#define GAME_COMBAT_COMBATANIM_HPP_INCLUDED
//
// combat-anim.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/shaker.hpp"

#include <vector>
#include <memory>


namespace game
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
    using CreatureCPtr_t  = const Creature *;
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


    //All the info required to shake a creature image on the battlefield.
    struct ShakeAnimInfo
    {
        ShakeAnimInfo();

        static const float PAUSE_DURATION_SEC;
        static const float SHAKE_DURATION_SEC;

        sfml_util::Shaker<float> slider;
        float                    pause_duration_timer_sec;
        float                    shake_duration_timer_sec;

        void Reset(const float SLIDER_SPEED, const bool WILL_DOUBLE_SHAKE_DISTANCE);
    };

    using SakeInfoMap_t = std::map<combat::CombatNodePtr_t, ShakeAnimInfo>;


    //Responsible for displaying combat related animations.
    class CombatAnimation
    {
        //prevent copy construction
        CombatAnimation(const CombatAnimation &) =delete;

        //prevent copy assignment
        CombatAnimation & operator=(const CombatAnimation &) =delete;

        //prevent non-singleton construction
        CombatAnimation();

    public:
        virtual ~CombatAnimation();

        static void GiveCombatDisplay(CombatDisplayPtr_t);
        static CombatAnimation * Instance();

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

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

        //The Melee Move Toward Animation moves a creature toward their target during
        //melee attacks.
        void MeleeMoveTowardAnimStart(creature::CreatureCPtrC_t CREATURE_MOVING_CPTRC,
                                      creature::CreatureCPtrC_t CREATURE_TARGET_CPTRC);

        void MeleeMoveTowardAnimUpdate(const float SLIDER_POS);
        void MeleeMoveTowardAnimStop();

        //The Melee Move Back Animation moves a creature back to original position
        //before the Move Toward Animation.
        void MeleeMoveBackAnimStart();
        void MeleeMoveBackAnimUpdate(const float SLIDER_POS);
        void MeleeMoveBackAnimStop();

        void ImpactAnimStart(const float CREATURE_SHAKE_SLIDER_SPEED);
        void ImpactAnimUpdate(const float SLIDER_POS);
        void ImpactAnimStop();

        //The Shake Animation wiggles a creature on the battlefield back and forth.
        static float ShakeAnimDistance(const bool WILL_DOUBLE);

        void ShakeAnimStart(creature::CreatureCPtrC_t CREATURE_CPTRC,
                            const float               SLIDER_SPEED,
                            const bool                WILL_DOUBLE_SHAKE_DISTANCE);

        //if a nullptr is given then all creatures will stop shaking
        void ShakeAnimStop(creature::CreatureCPtrC_t);

        inline creature::CreatureCPtr_t ShakeAnimCreatureCPtr() { return shakeAnimCreatureWasCPtr_; }

        void ShakeAnimTemporaryStop(creature::CreatureCPtrC_t CREATURE_CPTRC);
        void ShakeAnimRestart();

    private:
        static CombatAnimation * instance_;
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

        //members supporting the Melee Move Animations
        sf::Vector2f meleeMoveAnimOrigPosV_;
        sf::Vector2f meleeMoveAnimTargetPosV_;
        CombatNodePtr_t meleeMoveAnimMovingCombatNodePtr_;
        CombatNodePtr_t meleeMoveAnimTargetCombatNodePtr_;

        //members to shake a creature image on the battlefield
        creature::CreatureCPtr_t shakeAnimCreatureWasCPtr_;
        float                    shakeAnimCreatureWasSpeed_;
        SakeInfoMap_t            shakeAnimInfoMap_;
    };

    using CombatAnimationPtr_t = CombatAnimation *;

}
}

#endif //GAME_COMBAT_COMBATANIM_HPP_INCLUDED
