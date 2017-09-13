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
#ifndef GAME_COMBAT_COMBATNODE_INCLUDED
#define GAME_COMBAT_COMBATNODE_INCLUDED
//
// combat-node.hpp
//  A class that handles drawing nodes on the combat tree.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sliders.hpp"

#include "game/creature/creature.hpp"

#include <string>
#include <vector>
#include <memory>


namespace game
{
namespace combat
{

    //represents a node in the CombatTree
    class CombatNode : public sfml_util::gui::GuiEntity
    {
        //prevent copy construction
        CombatNode(const CombatNode &) =delete;

        //prevent copy assignment
        CombatNode & operator=(const CombatNode &) =delete;

    public:
        explicit CombatNode(const creature::CreaturePtr_t CREATURE_PTR);

        virtual ~CombatNode();

        const std::string ToString() const;

        //NEW_REGION replaces the postion and rect of the entity, also re-scales images
        void SetRegion(const sf::FloatRect & NEW_REGION);

        //same as other SetRegion(), except this takes a float from 0.0f (%) to 1.0f (%)
        void SetRegion(const float NEW_RATIO);

        void SetCharacterSize(const unsigned int);

        inline creature::CreaturePtr_t Creature() const     { return creaturePtr_; }

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline int GetBlockingPos() const                   { return blockingPos_; }
        inline void SetBlockingPos(const int NEW_POS)       { blockingPos_ = NEW_POS; }

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        void SetToneDown(const float TONE_DOWN_VAL);

        inline bool IsSummaryView() const                   { return isSummaryView_; }
        void IsSummaryView(const bool);

        inline bool IsMoving() const                        { return isMoving_; }
        inline void IsMoving(const bool B)                  { isMoving_ = B; }

        void UpdateConditionText();

        inline bool IsFlying() const                        { return isFlying_; }
        void IsFlying(const bool);

        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        void SetImagePosOffset(const float HORIZ, const float VERT);

        inline const sf::FloatRect GetImageBounds() const   { return sprite_.getGlobalBounds(); }

        void HealthChangeTasks();

        void SetHighlight(const bool WILL_HIGHLIGHT, const bool WILL_PLAY_SOUND_EFFECT);

        void SetDead(const bool IS_DEAD)                    { isDead_ = IS_DEAD; }

        void SetRotationDegrees(const float DEGREES);

        void UpdateDeathAnim(const float SLIDER_POS);

        void SelectAnimStart();
        void SelectAnimUpdate(const float SLIDER_RATIO);
        void SelectAnimStop();

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
        const sf::Color HealthColor() const;
        const sf::Color HealthColorRed() const;
        const sf::Color HealthColorTick() const;
        const sf::Color AdjustColorForToneDown(const sf::Color & ORIG_COLOR,
                                               const float       TONE_DOWN_VAL) const;

        void SetTextPositions(const bool DID_TRANSITION_TO_SUMMARY_VIEW = false);

        void SetWingImagePosition();
        void SetWingImageScaleAndOrigin();
        void UpdateImagePosition();
        void SetupSkullAndCrossBones();
        void SetImageScale();
        void SetCrossBonesImageScale();

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
        static const float     TONED_DOWN_MULT_;
        static const sf::Uint8 DECAL_IMAGE_ALPHA_;
        static const float     WING_IMAGE_SCALE_;
        static const float     WING_IMAGE_HORIZ_OFFSET_;
        static const float     WING_IMAGE_ANIM_SPEED_;
        static const float     WING_IMAGE_ROTATION_MAX_;
        //
        sf::Text                 nameTextObj_;
        sf::Text                 condTextObj_;
        int                      blockingPos_;
        sf::Color                healthLineColor_;
        sf::Color                healthLineColorRed_;
        sf::Color                healthLineColorTick_;
        sf::Texture              texture_;
        sf::Sprite               sprite_;
        sf::Color                creatureImageColor_;
        bool                     isSummaryView_;
        bool                     isMoving_;
        creature::CreaturePtr_t  creaturePtr_;

        //members that control the display of skull and crossbones
        sfml_util::TextureUPtr_t crossBonesTextureUPtr_;
        sf::Sprite crossBonesSprite_;
        bool willShowCrossBones_;

        //The health displayed is not the actual creature health but this
        //cached value, so that changes in health appear when they are
        //reported and not when they actually occur.
        float healthRatioDisplayed_;

        //The changes in drawing when a creature dies (stop anim wing, stop
        //drawing conditions and health, etc.) does not occur when the
        //creature actually dies, but when the dead animations start,
        //so this cached value keeps track instead of the actual creature
        //status.
        bool isDead_;

        //members that control the flapping wing animation
        sfml_util::TextureUPtr_t wingTextureUPtr_;
        sf::Sprite wingSprite_;
        bool isFlying_;
        sfml_util::sliders::ZeroSliderOnce<float> wingFlapSlider_;

        //members that control the shake or image position offset
        sf::Vector2f imagePosV_;
        sf::Vector2f imagePosOffsetV_;

        //members that support the select animation
        bool willShowSelectAnim_;
        sf::Sprite selectAnimSprite_;

    };

    using CombatNodePtr_t   = CombatNode *;
    using CombatNodeCPtr_t  = const CombatNode *;
    using CombatNodePtrC_t  = CombatNode * const;
    using CombatNodeCPtrC_t = const CombatNode * const;
    using CombatNodeSPtr_t  = std::shared_ptr<CombatNode>;

    using CombatNodePVec_t  = std::vector<CombatNodePtr_t>;

}
}
#endif //GAME_COMBAT_COMBATNODE_INCLUDED
