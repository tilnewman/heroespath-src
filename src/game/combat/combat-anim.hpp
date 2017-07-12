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
#ifndef GAME_COMBAT_COMBATANIM_HPP_INCLUDED
#define GAME_COMBAT_COMBATANIM_HPP_INCLUDED
//
// combat-anim.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/shaker.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/animation-enum.hpp"

#include "game/stats/types.hpp"

#include <vector>
#include <memory>
#include <string>
#include <map>


namespace sfml_util
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;
    using AnimationUVec_t = std::vector<AnimationUPtr_t>;

namespace animation
{
    class SparksAnimation;
    using SparksAnimationUPtr_t = std::unique_ptr<SparksAnimation>;
    using SparksAnimationUVec_t = std::vector<SparksAnimationUPtr_t>;

    class CloudAnimation;
    using CloudAnimationUPtr_t = std::unique_ptr<CloudAnimation>;
    using CloudAnimationUVec_t = std::vector<CloudAnimationUPtr_t>;

    class SongAnimation;
    using SongAnimationUPtr_t = std::unique_ptr<SongAnimation>;
    using SongAnimationUVec_t = std::vector<SongAnimationUPtr_t>;

    class SparkleAnimation;
    using SparkleAnimationUPtr_t = std::unique_ptr<SparkleAnimation>;
    using SparkleAnimationUVec_t = std::vector<SparkleAnimationUPtr_t>;

    class TextAnimation;
    using TextAnimationUPtr_t = std::unique_ptr<TextAnimation>;
    using TextAnimationUVec_t = std::vector<TextAnimationUPtr_t>;
}
}

namespace game
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
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

    public:
        explicit CombatAnimation(const CombatDisplayPtr_t);

        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void ProjectileShootAnimStart(creature::CreatureCPtrC_t CREATURE_ATTACKING_CPTRC,
                                      creature::CreatureCPtrC_t CREATURE_DEFENDING_CPTRC,
                                      const item::ItemPtr_t     WEAPON_PTR,
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

        //returns true if CombatStage should zoom out,
        //or if (centeringAnimWillZoomOut_ && ! AreAllCreaturesVisible())
        bool CenteringUpdate(const float SLIDER_POS, const bool WILL_MOVE_BACKGROUND = true);

        void CenteringStop();

        //The Reposition Animation slides all creatures around after a blocking
        //position change, which has the effecting of looking like the battlefield
        //camera is moving.
        void RepositionAnimStart(creature::CreaturePtr_t);
        void RepositionAnimStart(const sf::Vector2f &);
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

        void ShakeAnimTemporaryStop(creature::CreatureCPtrC_t);
        void ShakeAnimRestart();

        void SelectAnimStart(creature::CreatureCPtrC_t);
        void SelectAnimStop();

        void SpellAnimStart(const spell::SpellPtr_t          SPELL_PTR,
                            const creature::CreatureCPtrC_t  CASTING_CREATURE_CPTRC,
                            const combat::CombatNodePVec_t & TARGETS_PVEC);

        bool SpellAnimUpdate(const spell::SpellPtr_t SPELL_PTR,
                             const float             ELAPSED_TIME_SEC);

        void SpellAnimStop(const spell::SpellPtr_t SPELL_PTR);

        void SparksAnimStart(const creature::CreatureCPtrC_t  CASTING_CREATURE_CPTRC,
                             const combat::CombatNodePVec_t & TARGETS_PVEC);

        bool SparksAnimUpdate(const float ELAPSED_TIME_SEC);

        void SparksAnimStop();

        void PoisonCloudAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool PoisonCloudAnimUpdate(const float ELAPSED_TIME_SEC);
        void PoisonCloudAnimStop();

        void SetupAnimations(const combat::CombatNodePVec_t &  TARGETS_PVEC,
                             const sfml_util::Animations::Enum ENUM,
                             const float                       FRAME_DELAY_SEC,
                             const sf::Color &                 COLOR_FROM,
                             const sf::Color &                 COLOR_TO);

        void SongAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool SongAnimUpdate(const float ELAPSED_TIME_SEC);
        void SongAnimStop();

        void SparkleAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool SparkleAnimUpdate(const float ELAPSED_TIME_SEC);
        bool SparkleAnimIsDone();
        void SparkleAnimStop();

        void TextAnimStart(const stats::Health_t &       DAMAGE,
                           const combat::CombatNodePtr_t TARGET_PTR);

        void TextAnimStart(const std::vector<stats::Health_t> & DAMAGE_VEC,
                           const combat::CombatNodePVec_t &     TARGETS_PVEC);

        void TextAnimStop();

        void RunAnimStart(const creature::CreaturePtr_t CREATURE_PTR);
        void RunAnimUpdate(const float SLIDER_POS);
        creature::CreaturePtr_t RunAnimStop();

        void EndOfCombatCleanup();

    private:
        static const float SELECT_ANIM_SLIDER_SPEED_;
        static const float ANIM_TIME_BETWEEN_FRAMES_DEFAULT_;
        static const std::string ANIM_MEDIA_PATH_KEY_STR_SPARKLE_;
        static const std::string ANIM_MEDIA_PATH_KEY_STR_SHIMMER_;
        static const sf::Uint8 ANIM_COLOR_ALT_VAL_;

        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float BATTLEFIELD_CENTERING_SPEED_;

        CombatDisplayPtr_t combatDisplayStagePtr_;

        sfml_util::sliders::ZeroSliderOnce<float> slider_;

        //members supporting the Projectile Shoot Animation
        sf::Texture projAnimTexture_;
        sf::Sprite projAnimSprite_;
        sf::Vector2f projAnimBeginPosV_;
        sf::Vector2f projAnimEndPosV_;
        bool projAnimWillSpin_;
        bool projAnimWillDraw_;

        //members supporting the Death Animation
        CombatNodePVec_t deadAnimNodesPVec_;

        //members supporting the Centering Animation
        combat::CombatNodePtr_t centeringAnimCombatNodePtr_;
        creature::CreaturePVec_t centeringAnimCreaturesPVec_;
        bool centeringAnimWillZoomOut_;

        //member supporting the Reposition Animation
        creature::CreaturePtr_t repositionAnimCreaturePtr_;
        sf::Vector2f repositionAnimPosV_;

        //members supporting the Melee Move Animations
        sf::Vector2f meleeMoveAnimOrigPosV_;
        sf::Vector2f meleeMoveAnimTargetPosV_;
        CombatNodePtr_t meleeMoveAnimMovingCombatNodePtr_;
        CombatNodePtr_t meleeMoveAnimTargetCombatNodePtr_;

        //members to shake a creature image on the battlefield
        creature::CreatureCPtr_t shakeAnimCreatureWasCPtr_;
        float shakeAnimCreatureWasSpeed_;
        SakeInfoMap_t shakeAnimInfoMap_;

        //members to perform the selection animation
        CombatNodePtr_t selectAnimCombatNodePtr_;

        sfml_util::animation::SparksAnimationUVec_t sparksAnimUVec_;
        sfml_util::animation::CloudAnimationUVec_t cloudAnimUVec_;

        //members that control animations in general
        sfml_util::AnimationUVec_t animUVec_;

        sfml_util::animation::SongAnimationUVec_t songAnimUVec_;
        sfml_util::animation::SparkleAnimationUVec_t sparkleAnimUVec_;
        sfml_util::animation::TextAnimationUVec_t textAnimUVec_;

        //members that control the run animation
        CombatNodePtr_t runAnimCombatNodePtr_;
        sf::Vector2f runAnimPosVTarget_;
        sf::Vector2f runAnimPosVOrig_;
    };

    using CombatAnimationUPtr_t = std::unique_ptr<CombatAnimation>;

}
}

#endif //GAME_COMBAT_COMBATANIM_HPP_INCLUDED
