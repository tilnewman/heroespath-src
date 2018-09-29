// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_COMBATNODE_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATNODE_HPP_INCLUDED
//
// combat-node.hpp
//  A class that handles drawing nodes on the combat tree.
//
#include "creature/name-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/entity.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // represents a node in the CombatTree
    class CombatNode : public sfml_util::Entity
    {
    public:
        CombatNode(const CombatNode &) = delete;
        CombatNode(CombatNode &&) = delete;
        CombatNode & operator=(const CombatNode &) = delete;
        CombatNode & operator=(CombatNode &&) = delete;

        explicit CombatNode(const creature::CreaturePtr_t CREATURE_PTR);

        virtual ~CombatNode();

        const std::string ToString() const;

        // NEW_REGION replaces the postion and rect of the entity, also re-scales images
        void SetRegion(const sf::FloatRect & NEW_REGION);

        // same as other SetRegion(), except this takes a float from 0.0f (%) to 1.0f (%)
        void SetRegion(const float NEW_RATIO);

        void SetCharacterSize(const unsigned int);

        const creature::CreaturePtr_t Creature() const { return creaturePtr_; }

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        int GetBlockingPos() const { return blockingPos_; }
        void SetBlockingPos(const int NEW_POS) { blockingPos_ = NEW_POS; }

        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) override;

        void SetToneDown(const float TONE_DOWN_VAL);

        bool IsSummaryView() const { return isSummaryView_; }
        void IsSummaryView(const bool);

        bool IsMoving() const { return isMoving_; }
        void IsMoving(const bool B) { isMoving_ = B; }

        void UpdateConditionText();

        bool IsFlying() const { return isFlying_; }
        void IsFlying(const bool);

        bool UpdateTime(const float ELAPSED_TIME_SECONDS) override;

        void SetImagePosOffset(const float HORIZ, const float VERT);

        const sf::FloatRect GetImageBounds() const { return sprite_.getGlobalBounds(); }

        void HealthChangeTasks();

        void SetHighlight(const bool WILL_HIGHLIGHT, const bool WILL_PLAY_SOUND_EFFECT);

        void SetDead(const bool IS_DEAD) { isDead_ = IS_DEAD; }

        void SetRotationDegrees(const float DEGREES);

        void UpdateDeathAnim(const float SLIDER_POS);

        void SelectAnimStart();
        void SelectAnimUpdate(const float SLIDER_RATIO);
        void SelectAnimStop();

        float GetNameWidth() const { return nameText_.getGlobalBounds().width; }

        bool WillDraw() const { return willDraw_; }
        void WillDraw(const bool WILL_DRAW) { willDraw_ = WILL_DRAW; }

    protected:
        const sf::Color HealthColor() const;
        const sf::Color HealthColorRed() const;
        const sf::Color HealthColorTick() const;

        const sf::Color
            AdjustColorForToneDown(const sf::Color & ORIG_COLOR, const float TONE_DOWN_VAL) const;

        void SetTextPositions(const bool DID_TRANSITION_TO_SUMMARY_VIEW = false);

        void SetWingImagePosition();
        void SetWingImageScaleAndOrigin();
        void UpdateImagePosition();
        void SetupSkullAndCrossBones();
        void SetImageScale();
        void SetCrossBonesImageScale();

        void SetupHealthLines();

        const sf::Color NameColor() const
        {
            return ((isPlayer_) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);
        }

    protected:
        static const sf::Uint8 HIGHLIGHT_ADJ_VALUE_;
        static const sf::Color HIGHLIGHT_ADJ_COLOR_;
        static const sf::Color PLAYER_NAME_COLOR_;
        static const sf::Color CONDITION_COLOR_;
        static const sf::Color NONPLAYER_NAME_COLOR_;
        static const sf::Uint8 CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_;
        static const sf::Uint8 CREATURE_IMAGE_COLOR_ALPHA_;
        static const sf::Color CREATURE_IMAGE_COLOR_PLAYER_;
        static const sf::Color CREATURE_IMAGE_COLOR_NONPLAYER_;
        static const float TONED_DOWN_MULT_;
        static const sf::Uint8 DECAL_IMAGE_ALPHA_;
        static const float WING_IMAGE_SCALE_;
        static const float WING_IMAGE_HORIZ_OFFSET_;
        static const double WING_IMAGE_ANIM_SPEED_;
        static const float WING_IMAGE_ROTATION_MAX_;
        static const std::size_t HEALTH_LINE_COUNT_;
        //
        creature::NameInfo creatureNameInfo_;
        bool isPlayer_;
        sfml_util::Text nameText_;
        sfml_util::Text condText_;
        int blockingPos_;
        sfml_util::CachedTexture cachedTexture_;
        sf::Sprite sprite_;
        sf::Color creatureImageColor_;
        bool isSummaryView_;
        bool isMoving_;
        creature::CreaturePtr_t creaturePtr_;
        bool willDraw_;

        // members that control the display of skull and crossbones
        sfml_util::CachedTexture crossBonesCachedTexture_;
        sf::Sprite crossBonesSprite_;
        bool willShowCrossBones_;

        // The health displayed is not the actual creature health but this
        // cached value, so that changes in health appear when they are
        // reported and not when they actually occur.
        float healthRatioDisplayed_;
        sf::Color healthLineColor_;
        sf::Color healthLineColorRed_;
        sf::Color healthLineColorTick_;
        sf::VertexArray healthLines_;

        // The changes in drawing when a creature dies (stop anim wing, stop
        // drawing conditions and health, etc.) does not occur when the
        // creature actually dies, but when the dead animations start,
        // so this cached value keeps track instead of the actual creature
        // status.
        bool isDead_;

        // members that control the flapping wing animation
        sfml_util::CachedTexture wingCachedTexture_;
        sf::Sprite wingSprite_;
        bool isFlying_;
        sfml_util::sliders::Slider<float> wingFlapSlider_;

        // members that control the shake or image position offset
        sf::Vector2f imagePosV_;
        sf::Vector2f imagePosOffsetV_;

        // members that support the select animation
        bool willShowSelectAnim_;
        sf::Sprite selectAnimSprite_;
    };

    using CombatNodePtr_t = misc::NotNull<CombatNode *>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATNODE_HPP_INCLUDED
