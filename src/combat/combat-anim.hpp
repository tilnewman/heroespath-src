// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATANIM_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATANIM_HPP_INCLUDED
//
// combat-anim.hpp
//
#include "creature/trait.hpp"
#include "gui/animation-enum.hpp"
#include "gui/cached-texture.hpp"
#include "gui/sliders.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
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
    } // namespace animation
} // namespace gui

namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    class CombatDisplay;
    using CombatDisplayPtr_t = misc::NotNull<CombatDisplay *>;

    class CombatNode;
    using CombatNodePtr_t = misc::NotNull<CombatNode *>;
    using CombatNodePtrOpt_t = boost::optional<CombatNodePtr_t>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;

    // All the info required to shake a creature image on the battlefield.
    struct ShakeAnimInfo
    {
        ShakeAnimInfo();

        static const float PAUSE_DURATION_SEC;
        static const float SHAKE_DURATION_SEC;

        gui::SliderOscillator<float> slider;
        float pause_duration_timer_sec;
        float shake_duration_timer_sec;

        void Reset(const float SLIDER_SPEED, const bool WILL_DOUBLE_SHAKE_DISTANCE);
    };

    using ShakeInfoMap_t = misc::VectorMap<combat::CombatNodePtr_t, ShakeAnimInfo>;

    // Responsible for displaying combat related animations.
    class CombatAnimation : public sf::Drawable
    {
    public:
        CombatAnimation(const CombatAnimation &) = delete;
        CombatAnimation(CombatAnimation &&) = delete;
        CombatAnimation & operator=(const CombatAnimation &) = delete;
        CombatAnimation & operator=(CombatAnimation &&) = delete;

        CombatAnimation();

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void ProjectileShootAnimStart(
            const sf::Vector2f & CREATURE_ATTACKING_CENTER_POSV,
            const sf::Vector2f & CREATURE_DEFENDING_CENTER_POSV,
            const item::ItemPtr_t WEAPON_PTR,
            const bool WILL_HIT);

        void ProjectileShootAnimUpdate(
            const float SLIDER_POS, const sf::FloatRect & BATTLEFIELD_RECT);

        void ProjectileShootAnimStop();

        // The Death Animation is only for non-player characters
        void DeathAnimStart(const CombatNodePVec_t &);
        void DeathAnimUpdate(const float SLIDER_POS);
        void DeathAnimStop(const CombatDisplayPtr_t);

        // The Centering Animation slides all creatures into the center of the
        // battlefield.
        void CenteringStart(const CombatNodePtr_t);

        void CenteringStart(
            const sf::Vector2f & TARGET_POSV, const CombatDisplayPtr_t COMBAT_DISPLAY_PTR);

        void CenteringStart(
            const creature::CreaturePVec_t &,
            const sf::Vector2f & CENTER_POSV,
            const bool IS_ZOOMOUT_REQUIRED,
            const CombatDisplayPtr_t);

        // returns true if CombatStage should zoom out,
        // or if (centeringAnimWillZoomOut_ && ! AreAllCreaturesVisible())
        bool CenteringUpdate(
            const float SLIDER_POS,
            const CombatDisplayPtr_t COMBAT_DISPLAY_PTR,
            const bool WILL_MOVE_BACKGROUND = true);

        void CenteringStop();

        // The Reposition Animation slides all creatures around after a blocking
        // position change, which has the effecting of looking like the battlefield
        // camera is moving.
        void RepositionAnimStart(const CombatNodePtr_t);
        void RepositionAnimStart(const sf::Vector2f &, const CombatDisplayPtr_t);
        void RepositionAnimUpdate(const float SLIDER_POS, const CombatDisplayPtr_t);
        void RepositionAnimStop();

        // The Melee Move Toward Animation moves a creature toward their target during
        // melee attacks.
        void MeleeMoveTowardAnimStart(
            const CombatNodePtr_t CREATURE_MOVING_COMBAT_NODE_PTR,
            const CombatNodePtr_t CREATURE_TARGET_COMBAT_NODE_PTR);

        void MeleeMoveTowardAnimUpdate(const float SLIDER_POS);
        void MeleeMoveTowardAnimStop();

        // The Melee Move Back Animation moves a creature back to original position
        // before the Move Toward Animation.
        void MeleeMoveBackAnimStart();
        void MeleeMoveBackAnimUpdate(const float SLIDER_POS);
        void MeleeMoveBackAnimStop();

        void ImpactAnimStart(const float CREATURE_SHAKE_SLIDER_SPEED);
        void ImpactAnimUpdate(const float SLIDER_POS);
        void ImpactAnimStop();

        // The Shake Animation wiggles a creature on the battlefield back and forth.
        static float ShakeAnimDistance(const bool WILL_DOUBLE);

        void ShakeAnimStart(
            const CombatNodePVec_t & COMBAT_NODES_PVEC,
            const float SLIDER_SPEED,
            const bool WILL_DOUBLE_SHAKE_DISTANCE);

        void ShakeAnimStart(
            const CombatNodePtr_t COMBAT_NODE_PTR,
            const float SLIDER_SPEED,
            const bool WILL_DOUBLE_SHAKE_DISTANCE);

        // if boost::none is given then all creatures will stop shaking
        void ShakeAnimStop(const CombatNodePtrOpt_t);

        bool IsThereAShakeAnimWasComabtNode() { return !!shakeAnimWasCombatNodePtrOpt_; }

        void ShakeAnimTemporaryStop(const creature::CreaturePtr_t);
        void ShakeAnimRestart();

        void SelectAnimStart(const CombatNodePtr_t);
        void SelectAnimStop();

        void SpellAnimStart(
            const spell::SpellPtr_t SPELL_PTR,
            const creature::CreaturePtr_t CASTING_CREATURE_PTR,
            const combat::CombatNodePVec_t & TARGETS_PVEC);

        bool SpellAnimUpdate(const spell::SpellPtr_t SPELL_PTR, const float ELAPSED_TIME_SEC);

        void SpellAnimStop(const spell::SpellPtr_t SPELL_PTR);

        void SparksAnimStart(
            const creature::CreaturePtr_t CASTING_CREATURE_PTR,
            const combat::CombatNodePVec_t & TARGETS_PVEC);

        bool SparksAnimUpdate(const float ELAPSED_TIME_SEC);

        void SparksAnimStop();

        void PoisonCloudAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool PoisonCloudAnimUpdate(const float ELAPSED_TIME_SEC);
        void PoisonCloudAnimStop();

        void SetupAnimations(
            const combat::CombatNodePVec_t & TARGETS_PVEC,
            const gui::Animations::Enum ENUM,
            const float FRAME_DELAY_SEC_ADJ,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO);

        void SongAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool SongAnimUpdate(const float ELAPSED_TIME_SEC);
        void SongAnimStop();

        void SparkleAnimStart(const combat::CombatNodePVec_t & TARGETS_PVEC);
        bool SparkleAnimUpdate(const float ELAPSED_TIME_SEC);
        bool SparkleAnimIsDone();
        void SparkleAnimStop();

        void TextAnimStart(const Health_t & DAMAGE, const combat::CombatNodePtr_t TARGET_PTR);

        void TextAnimStart(
            const std::vector<Health_t> & DAMAGE_VEC,
            const combat::CombatNodePVec_t & TARGETS_PVEC);

        void TextAnimStop();

        void RunAnimStart(const CombatNodePtr_t COMBAT_NODE_PTR);
        void RunAnimUpdate(const float SLIDER_POS);
        const creature::CreaturePtr_t RunAnimStop();

        void EndOfCombatCleanup();

    private:
        static const float SELECT_ANIM_SLIDER_SPEED_;
        static const sf::Uint8 ANIM_COLOR_ALT_VAL_;

        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float BATTLEFIELD_CENTERING_SPEED_;

        gui::SliderZeroToOne slider_;

        // members supporting the Projectile Shoot Animation
        gui::CachedTextureOpt_t projAnimCachedTextureOpt_;
        sf::Sprite projAnimSprite_;
        sf::Vector2f projAnimBeginPosV_;
        sf::Vector2f projAnimEndPosV_;
        bool projAnimWillSpin_;
        bool projAnimWillDraw_;

        // members supporting the Death Animation
        CombatNodePVec_t deadAnimNodesPVec_;

        // members supporting the Centering Animation
        combat::CombatNodePtrOpt_t centeringAnimCombatNodePtrOpt_;
        creature::CreaturePVec_t centeringAnimCreaturesPVec_;
        bool centeringAnimWillZoomOut_;

        // member supporting the Reposition Animation
        CombatNodePtrOpt_t repositionAnimCombatNodePtrOpt_;
        sf::Vector2f repositionAnimPosV_;

        // members supporting the Melee Move Animations
        sf::Vector2f meleeMoveAnimOrigPosV_;
        sf::Vector2f meleeMoveAnimTargetPosV_;
        CombatNodePtrOpt_t meleeMoveAnimMovingCombatNodePtrOpt_;
        CombatNodePtrOpt_t meleeMoveAnimTargetCombatNodePtrOpt_;

        // members to shake a creature image on the battlefield
        CombatNodePtrOpt_t shakeAnimWasCombatNodePtrOpt_;
        float shakeAnimCreatureWasSpeed_;
        ShakeInfoMap_t shakeAnimInfoMap_;

        // members to perform the selection animation
        CombatNodePtrOpt_t selectAnimCombatNodePtrOpt_;

        gui::animation::SparksAnimationUVec_t sparksAnimUVec_;
        gui::animation::CloudAnimationUVec_t cloudAnimUVec_;

        // members that control animations in general
        gui::AnimationUVec_t animUVec_;

        gui::animation::SongAnimationUVec_t songAnimUVec_;
        gui::animation::SparkleAnimationUVec_t sparkleAnimUVec_;
        gui::animation::TextAnimationUVec_t textAnimUVec_;

        // members that control the run animation
        CombatNodePtrOpt_t runAnimCombatNodePtrOpt_;
        sf::Vector2f runAnimPosVTarget_;
        sf::Vector2f runAnimPosVOrig_;
    };

    using CombatAnimationUPtr_t = std::unique_ptr<CombatAnimation>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATANIM_HPP_INCLUDED
