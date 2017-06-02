// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// combat-node.cpp
//
#include "combat-node.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "game/log-macros.hpp"
#include "game/creature/name-info.hpp"
#include "game/game-data-file.hpp"
#include "game/creature/condition-algorithms.hpp"

#include "misc/random.hpp"


namespace game
{
namespace combat
{

    const sf::Uint8 CombatNode::HIGHLIGHT_ADJ_VALUE_                 (sfml_util::FontManager::ColorValue_Highlight());
    const sf::Color CombatNode::HIGHLIGHT_ADJ_COLOR_                 (sf::Color(HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, 0));
    const sf::Color CombatNode::PLAYER_NAME_COLOR_                   (sf::Color(255, 255, 240) - HIGHLIGHT_ADJ_COLOR_);
    const sf::Color CombatNode::CONDITION_COLOR_                     (sfml_util::FontManager::Color_Light() - HIGHLIGHT_ADJ_COLOR_);
    const sf::Color CombatNode::NONPLAYER_NAME_COLOR_                (sf::Color(255, static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)), static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_))) - HIGHLIGHT_ADJ_COLOR_);
    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_(32);
    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_ALPHA_          (32);
    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_PLAYER_         (sf::Color(255, 243, 201, CREATURE_IMAGE_COLOR_ALPHA_));//slightly orange
    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_NONPLAYER_      (sf::Color(255, 235, 235, CREATURE_IMAGE_COLOR_ALPHA_));//slightly red
    const float     CombatNode::TONED_DOWN_MULT_                     (0.64f);
    const sf::Uint8 CombatNode::DECAL_IMAGE_ALPHA_                   (46);
    const float     CombatNode::WING_IMAGE_SCALE_                    (0.65f);
    const float     CombatNode::WING_IMAGE_HORIZ_OFFSET_             (0.333f);
    const float     CombatNode::WING_IMAGE_ANIM_SPEED_               (8.0f);
    const float     CombatNode::WING_IMAGE_ROTATION_MAX_             (-90.0f);
    

    CombatNode::CombatNode(const creature::CreaturePtr_t CREATURE_PTR,
                           const sfml_util::FontPtr_t    FONT_PTR,
                           const unsigned int            FONT_CHAR_SIZE)
    :
        GuiEntity             (std::string("CombatNode_of_\"").append(CREATURE_PTR->Name()).append("\""), sf::FloatRect()),
        nameTextObj_          (CREATURE_PTR->Name(), * FONT_PTR, FONT_CHAR_SIZE),
        condTextObj_          ("", * FONT_PTR, FONT_CHAR_SIZE),
        blockingPos_          (0),
        healthLineColor_      (),//this initializer doesn't matter, see constructor body below
        healthLineColorRed_   (),// "
        healthLineColorTick_  (),// "
        texture_              (),
        sprite_               (),
        creatureImageColor_   (),
        isSummaryView_        (false),
        isMoving_             (false),
        creaturePtr_          (CREATURE_PTR),
        crossBonesTextureUPtr_(),
        crossBonesSprite_     (),
        willShowCrossBones_   (false),
        healthRatioDisplayed_ (0.0f),
        isDead_               (false),
        wingTextureUPtr_      (),
        wingSprite_           (),
        isFlying_             (false),
        wingFlapSlider_       (WING_IMAGE_ANIM_SPEED_),
        imagePosV_            (0.0f, 0.0f),
        imagePosOffsetV_      (0.0f, 0.0f),
        willShowSelectAnim_   (false),
        selectAnimSprite_     ()
    {
        const sf::Color NAME_COLOR((CREATURE_PTR->IsPlayerCharacter()) ?
            PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);

        sfml_util::SetTextColor(nameTextObj_, NAME_COLOR);

        UpdateConditionText();
        sfml_util::SetTextColor(condTextObj_, sf::Color(200, 200, 200, 200));

        HealthChangeTasks();

        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
            texture_,
            CREATURE_PTR->ImageFilename(),
            CREATURE_PTR->IsPlayerCharacter());

        texture_.setSmooth(true);
        sprite_.setTexture(texture_);

        //sprite color
        if (CREATURE_PTR->IsPlayerCharacter())
        {
            creatureImageColor_ = CREATURE_IMAGE_COLOR_PLAYER_;
        }
        else
        {
            creatureImageColor_ = CREATURE_IMAGE_COLOR_NONPLAYER_;
        }

        sprite_.setColor(creatureImageColor_);

        //um...This was the only way I could get it to work...zTn 2017-4-6
        IsFlying(true);
        IsFlying(false);
    }


    CombatNode::~CombatNode()
    {}


    const std::string CombatNode::ToString() const
    {
        std::ostringstream ss;
        ss << "CombatNode " << ((creaturePtr_->IsPlayerCharacter()) ? "Player " : "NonPlayer ") << creaturePtr_->Race().Name() << " " << creaturePtr_->Role().Name();

        if (creaturePtr_->IsPlayerCharacter())
        {
            ss << " \"" << creaturePtr_->Name() << "\"";
        }

        ss << " blocking_pos=" << blockingPos_;

        return ss.str();
    }


    void CombatNode::SetRegion(const sf::FloatRect & NEW_REGION)
    {
        entityRegion_ = NEW_REGION;
        SetEntityPos(NEW_REGION.left, NEW_REGION.top);//see local override
        SetImageScale();
        SetCrossBonesImageScale();
        SetWingImageScaleAndOrigin();
    }


    void CombatNode::SetRegion(const float NEW_RATIO)
    {
        auto const ORIG_RECT{ GetEntityRegion() };
        auto newRect{ ORIG_RECT };
        newRect.width *= NEW_RATIO;
        newRect.left += (ORIG_RECT.width - newRect.width) * 0.5f;
        newRect.height *= NEW_RATIO;
        newRect.top += (ORIG_RECT.height - newRect.height) * 0.5f;
        SetRegion(newRect);
    }


    void CombatNode::SetCharacterSize(const unsigned int NEW_CHARACTER_SIZE)
    {
        nameTextObj_.setCharacterSize(NEW_CHARACTER_SIZE);
        condTextObj_.setCharacterSize(NEW_CHARACTER_SIZE);
    }


    void CombatNode::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //don't draw if outside the battlefield rect
        if (false == GetEntityWillDraw())
        {
            return;
        }

        //sfml_util::DrawRectangle(target, states, sf::FloatRect(entityRegion_));

        //creature outline image
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(sprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }

        //select anim image
        if (willShowSelectAnim_)
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(selectAnimSprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }

        if (isFlying_ &&
            (false == isDead_) &&
            (false == isMoving_) &&
            (false == isSummaryView_))
        {
            target.draw(wingSprite_, states);
        }

        if (willShowCrossBones_)
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(crossBonesSprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }

        if ((false == isMoving_) &&
            (false == isSummaryView_) &&
            (false == isDead_))
        {
            target.draw(nameTextObj_, states);
            target.draw(condTextObj_, states);

            //draw health lines
            auto const HEALTH_POS_TOP           { nameTextObj_.getPosition().y + (nameTextObj_.getGlobalBounds().height * 2.0f) };
            auto const HEALTH_LINE_LEN_MARGIN   { 0.25f * entityRegion_.width };
            auto const HEALTH_LINE_POS_LEFT     { entityRegion_.left + HEALTH_LINE_LEN_MARGIN };
            auto const HEALTH_LINE_LEN          { entityRegion_.width - (HEALTH_LINE_LEN_MARGIN * 2.0f) };
            auto const HEALTH_LINE_TICK_HEIGHT  { 4.0f };
            sf::Vertex lines[] =
            {
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + (HEALTH_LINE_LEN * healthRatioDisplayed_), HEALTH_POS_TOP)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP + 1)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + (HEALTH_LINE_LEN * healthRatioDisplayed_), HEALTH_POS_TOP + 1)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP + 2)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + HEALTH_LINE_LEN, HEALTH_POS_TOP + 2)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP - HEALTH_LINE_TICK_HEIGHT)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP + HEALTH_LINE_TICK_HEIGHT + 2)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + HEALTH_LINE_LEN, HEALTH_POS_TOP - HEALTH_LINE_TICK_HEIGHT)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + HEALTH_LINE_LEN, HEALTH_POS_TOP + HEALTH_LINE_TICK_HEIGHT + 2))
            };
            lines[0].color = healthLineColor_;
            lines[1].color = healthLineColor_;
            lines[2].color = healthLineColor_;
            lines[3].color = healthLineColor_;
            lines[4].color = healthLineColor_;
            lines[5].color = healthLineColorRed_;
            lines[6].color = healthLineColorTick_;
            lines[7].color = healthLineColorTick_;
            lines[8].color = healthLineColorTick_;
            lines[9].color = healthLineColorTick_;
            target.draw(lines, 10, sf::Lines, states);
        }
    }


    void CombatNode::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);

        SetTextPositions();

        imagePosV_.x = (entityRegion_.left + (entityRegion_.width  * 0.5f)) - (sprite_.getGlobalBounds().width  * 0.5f);
        imagePosV_.y = (entityRegion_.top  + (entityRegion_.height * 0.5f)) - (sprite_.getGlobalBounds().height * 0.5f);

        UpdateImagePosition();
        SetWingImagePosition();

        auto const CROSSBONES_IMAGE_POS_LEFT{ (entityRegion_.left + (entityRegion_.width  * 0.5f)) - (crossBonesSprite_.getGlobalBounds().width  * 0.5f) };
        auto const CROSSBONES_IMAGE_POS_TOP { (entityRegion_.top  + (entityRegion_.height * 0.5f)) - (crossBonesSprite_.getGlobalBounds().height * 0.5f) };
        crossBonesSprite_.setPosition(CROSSBONES_IMAGE_POS_LEFT, CROSSBONES_IMAGE_POS_TOP);
    }


    void CombatNode::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        nameTextObj_.move(HORIZ, VERT);
        condTextObj_.move(HORIZ, VERT);

        imagePosV_.x += HORIZ;
        imagePosV_.y += VERT;
        UpdateImagePosition();

        wingSprite_.move(HORIZ, VERT);
        crossBonesSprite_.move(HORIZ, VERT);
    }


    bool CombatNode::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_MOUSE_STATE_CHANGE( GuiEntity::UpdateMousePos(MOUSE_POS_V) );
        if (DID_MOUSE_STATE_CHANGE)
        {
            SetHighlight((sfml_util::MouseState::Over == entityMouseState_), true);
        }

        return DID_MOUSE_STATE_CHANGE;
    }


    void CombatNode::SetToneDown(const float TONE_DOWN_VAL)
    {
        const sf::Color NAME_COLOR((creaturePtr_->IsPlayerCharacter()) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);
        sfml_util::SetTextColor(nameTextObj_, AdjustColorForToneDown(NAME_COLOR, TONE_DOWN_VAL));
        sfml_util::SetTextColor(condTextObj_, AdjustColorForToneDown(CONDITION_COLOR_, TONE_DOWN_VAL));

        healthLineColor_ = AdjustColorForToneDown(HealthColor(), TONE_DOWN_VAL);
        healthLineColorRed_ = AdjustColorForToneDown(HealthColorRed(), TONE_DOWN_VAL);
        healthLineColorTick_ = AdjustColorForToneDown(HealthColorTick(), TONE_DOWN_VAL);
    }


    void CombatNode::UpdateConditionText()
    {
        condTextObj_.setString( creaturePtr_->ConditionNames(3, creature::condition::Severity::LEAST_BENEFITIAL) );
        SetTextPositions();
    }


    void CombatNode::IsSummaryView(const bool IS_IN_SUMMARY_VIEW)
    {
        if ((false == isSummaryView_) && IS_IN_SUMMARY_VIEW)
            SetTextPositions(true);

        isSummaryView_ = IS_IN_SUMMARY_VIEW;
    }


    void CombatNode::IsFlying(const bool IS_FLYING)
    {
        if (IS_FLYING)
        {
            if (wingTextureUPtr_.get() == nullptr)
            {
                wingTextureUPtr_ = std::make_unique<sf::Texture>();
                sfml_util::gui::CombatImageManager::Instance()->Get(
                    * wingTextureUPtr_,
                    sfml_util::gui::CombatImageType::Wing,
                    ! creaturePtr_->IsPlayerCharacter());
            }

            wingSprite_.setTexture( * wingTextureUPtr_, true);
            wingSprite_.setColor(sf::Color(255, 255, 255, DECAL_IMAGE_ALPHA_));
            wingSprite_.setRotation(0.0f);
            SetWingImageScaleAndOrigin();
            SetWingImagePosition();

            wingFlapSlider_.Reset(WING_IMAGE_ANIM_SPEED_);
        }
        else
        {
            wingTextureUPtr_.reset();
        }

        isFlying_ = IS_FLYING;
    }


    bool CombatNode::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isFlying_)
        {
            if (creaturePtr_->IsPlayerCharacter())
            {
                wingSprite_.setRotation(WING_IMAGE_ROTATION_MAX_ * wingFlapSlider_.Update(ELAPSED_TIME_SECONDS));
            }
            else
            {
                wingSprite_.setRotation(WING_IMAGE_ROTATION_MAX_ * wingFlapSlider_.Update(ELAPSED_TIME_SECONDS) * -1.0f);
            }
        }

        return true;
    }


    void CombatNode::SetImagePosOffset(const float HORIZ, const float VERT)
    {
        imagePosOffsetV_.x = HORIZ;
        imagePosOffsetV_.y = VERT;
        UpdateImagePosition();
    }


    void CombatNode::HealthChangeTasks()
    {
        willShowCrossBones_ = (creaturePtr_->IsPlayerCharacter() && creaturePtr_->IsDead());
        if (willShowCrossBones_)
        {
            SetupSkullAndCrossBones();
        }

        healthRatioDisplayed_ = creaturePtr_->HealthRatio();
        UpdateConditionText();
        healthLineColor_ = HealthColor();
        healthLineColorRed_ = HealthColorRed();
        healthLineColorTick_ = HealthColorTick();
    }


    const sf::Color CombatNode::HealthColor() const
    {
        sf::Color c(255, static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)), static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)));
        c.g = static_cast<sf::Uint8>(static_cast<float>(c.g) * healthRatioDisplayed_);
        c.b = static_cast<sf::Uint8>(static_cast<float>(c.b) * healthRatioDisplayed_);
        return c;
    }


    const sf::Color CombatNode::HealthColorRed() const
    {
        return sf::Color(255, static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)), static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)));
    }


    const sf::Color CombatNode::HealthColorTick() const
    {
        return sf::Color(255, static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)), static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)));
    }


    const sf::Color CombatNode::AdjustColorForToneDown(const sf::Color & OLD_COLOR, const float TONE_DOWN_VAL) const
    {
        sf::Color newColor(OLD_COLOR);
        newColor.r = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.r) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        newColor.g = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.g) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        newColor.b = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.b) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        return newColor;
    }


    void CombatNode::SetTextPositions(const bool DID_TRANSITION_TO_SUMMARY_VIEW)
    {
        const float NAME_TEXT_POS_TOP((entityRegion_.top + (entityRegion_.height * 0.7f)) - (nameTextObj_.getGlobalBounds().height * 0.5f));
        const float COND_TEXT_POS_TOP((entityRegion_.top + entityRegion_.height) - (condTextObj_.getGlobalBounds().height * 0.5f));

        if (DID_TRANSITION_TO_SUMMARY_VIEW)
        {
            nameTextObj_.setPosition(entityRegion_.left + entityRegion_.width + 15.0f, NAME_TEXT_POS_TOP);
            condTextObj_.setPosition(sfml_util::Display::Instance()->GetWinWidth() + 1.0f, sfml_util::Display::Instance()->GetWinHeight() + 1.0f);
        }
        else
        {
            nameTextObj_.setPosition((entityRegion_.left + (entityRegion_.width  * 0.5f)) - (nameTextObj_.getGlobalBounds().width  * 0.5f), NAME_TEXT_POS_TOP);
            condTextObj_.setPosition((entityRegion_.left + (entityRegion_.width  * 0.5f)) - (condTextObj_.getGlobalBounds().width  * 0.5f), COND_TEXT_POS_TOP);
        }
    }


    void CombatNode::SetWingImagePosition()
    {
        auto const ORIG_ORIGIN{ wingSprite_.getOrigin() };
        wingSprite_.setOrigin(0.0f, 0.0f);

        wingSprite_.setPosition((sprite_.getGlobalBounds().left - (wingSprite_.getGlobalBounds().width * WING_IMAGE_HORIZ_OFFSET_)) + wingSprite_.getGlobalBounds().width,
                                (sprite_.getGlobalBounds().top + sprite_.getGlobalBounds().height) - (wingSprite_.getGlobalBounds().height * 0.5f));

        wingSprite_.setOrigin(ORIG_ORIGIN);
    }


    void CombatNode::SetWingImageScaleAndOrigin()
    {
        auto const WING_SCALE{ (entityRegion_.height / wingSprite_.getLocalBounds().height) * WING_IMAGE_SCALE_ };
        wingSprite_.setScale(WING_SCALE, WING_SCALE);

        if (creaturePtr_->IsPlayerCharacter())
        {
            wingSprite_.setOrigin(wingSprite_.getLocalBounds().width, wingSprite_.getLocalBounds().height);
        }
        else
        {
            wingSprite_.setOrigin(0.0f, wingSprite_.getLocalBounds().height);
        }
    }


    void CombatNode::UpdateImagePosition()
    {
        sprite_.setPosition(imagePosV_ + imagePosOffsetV_);
    }


    void CombatNode::SetHighlight(const bool WILL_HIGHLIGHT, const bool WILL_PLAY_SOUND_EFFECT)
    {
        const sf::Color NAME_COLOR((creaturePtr_->IsPlayerCharacter()) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);

        if (WILL_HIGHLIGHT)
        {
            if (WILL_PLAY_SOUND_EFFECT)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOn).PlayRandom();
            }

            sfml_util::SetTextColor(nameTextObj_, NAME_COLOR + HIGHLIGHT_ADJ_COLOR_);
            sfml_util::SetTextColor(condTextObj_, CONDITION_COLOR_ + HIGHLIGHT_ADJ_COLOR_);
            healthLineColor_ = HealthColor() + HIGHLIGHT_ADJ_COLOR_;
            healthLineColorTick_ = HealthColorTick() + HIGHLIGHT_ADJ_COLOR_;
            sprite_.setColor(creatureImageColor_ + sf::Color(0, 0, 0, CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_));
        }
        else
        {
            if (WILL_PLAY_SOUND_EFFECT)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOff).PlayRandom();
            }

            sfml_util::SetTextColor(nameTextObj_, NAME_COLOR);
            sfml_util::SetTextColor(condTextObj_, CONDITION_COLOR_);
            healthLineColor_ = HealthColor();
            healthLineColorTick_ = HealthColorTick();
            sprite_.setColor(creatureImageColor_);
        }
    }


    void CombatNode::SetRotationDegrees(const float DEGREES)
    {
        sprite_.setOrigin(sprite_.getGlobalBounds().width * 0.5f, sprite_.getGlobalBounds().height * 0.5f);
        sprite_.setRotation(DEGREES);
        sprite_.setOrigin(0.0f, 0.0f);
    }


    void CombatNode::SetupSkullAndCrossBones()
    {
        if (crossBonesTextureUPtr_.get() == nullptr)
        {
            crossBonesTextureUPtr_ = std::make_unique<sf::Texture>();

            sfml_util::LoadImageOrTexture( * crossBonesTextureUPtr_,
                GameDataFile::Instance()->GetMediaPath("media-images-combat-crossbones"));
        }

        crossBonesSprite_.setTexture( * crossBonesTextureUPtr_, true);
        crossBonesSprite_.setColor( sf::Color(255, 255, 255, DECAL_IMAGE_ALPHA_) );
        SetCrossBonesImageScale();
    }


    void CombatNode::SetImageScale()
    {
        const float SCALE_VERT(entityRegion_.height / sprite_.getLocalBounds().height);
        sprite_.setScale(SCALE_VERT, SCALE_VERT);

        if (sprite_.getGlobalBounds().width > entityRegion_.width)
        {
            const float SCALE_HORIZ(entityRegion_.width / sprite_.getLocalBounds().width);

            if (SCALE_HORIZ < SCALE_VERT)
            {
                sprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);
            }
        }
    }


    void CombatNode::SetCrossBonesImageScale()
    {
        const float SCALE_VERT(entityRegion_.height / crossBonesSprite_.getLocalBounds().height);
        crossBonesSprite_.setScale(SCALE_VERT, SCALE_VERT);

        if (crossBonesSprite_.getGlobalBounds().width > entityRegion_.width)
        {
            const float SCALE_HORIZ(entityRegion_.width / crossBonesSprite_.getLocalBounds().width);

            if (SCALE_HORIZ < SCALE_VERT)
            {
                crossBonesSprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);
            }
        }
    }


    void CombatNode::UpdateDeathAnim(const float SLIDER_POS)
    {
        SetRotationDegrees((12.0f * 360.0f) * SLIDER_POS);
        SetRegion(1.0f - SLIDER_POS);
        MoveEntityPos((GetEntityRegion().width * SLIDER_POS) * SLIDER_POS, (GetEntityRegion().height * SLIDER_POS) * SLIDER_POS);
    }


    void CombatNode::SelectAnimStart()
    {
        willShowSelectAnim_ = true;
        selectAnimSprite_ = sprite_;
    }


    void CombatNode::SelectAnimUpdate(const float SLIDER_RATIO)
    {
        //grow
        auto const SCALE{ 1.0f + (sfml_util::MapByRes(0.5f, 2.5f) * SLIDER_RATIO) };
        selectAnimSprite_.setScale(SCALE, SCALE);

        //re-center
        auto const HORIZ_ADJ{ (selectAnimSprite_.getGlobalBounds().width  - sprite_.getGlobalBounds().width)  * 0.5f };
        auto const VERT_ADJ { (selectAnimSprite_.getGlobalBounds().height - sprite_.getGlobalBounds().height) * 0.5f };
        selectAnimSprite_.setPosition(sprite_.getPosition().x - HORIZ_ADJ, sprite_.getPosition().y - VERT_ADJ);

        //fade-out
        auto color{ selectAnimSprite_.getColor() };
        color.a = static_cast<sf::Uint8>(sprite_.getColor().a * (1.0f - SLIDER_RATIO));
        selectAnimSprite_.setColor(color);
    }


    void CombatNode::SelectAnimStop()
    {
        willShowSelectAnim_ = false;
    }

}
}
