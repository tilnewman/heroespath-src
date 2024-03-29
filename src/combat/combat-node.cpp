// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-node.cpp
//
#include "combat-node.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/creature.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace combat
{

    const sf::Uint8 CombatNode::HIGHLIGHT_ADJ_VALUE_(55);

    const sf::Color CombatNode::HIGHLIGHT_ADJ_COLOR_(
        sf::Color(HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, 0));

    const sf::Color CombatNode::PLAYER_NAME_COLOR_(sf::Color(255, 255, 240) - HIGHLIGHT_ADJ_COLOR_);

    const sf::Color CombatNode::CONDITION_COLOR_(sfutil::color::Light - HIGHLIGHT_ADJ_COLOR_);

    const sf::Color CombatNode::NONPLAYER_NAME_COLOR_(
        sf::Color(
            255,
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)),
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)))
        - HIGHLIGHT_ADJ_COLOR_);

    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_(32);

    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_ALPHA_(32);

    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_PLAYER_(
        sf::Color(255, 243, 201, CREATURE_IMAGE_COLOR_ALPHA_)); // slightly orange

    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_NONPLAYER_(
        sf::Color(255, 235, 235, CREATURE_IMAGE_COLOR_ALPHA_)); // slightly red

    const float CombatNode::TONED_DOWN_MULT_(0.64f);
    const sf::Uint8 CombatNode::DECAL_IMAGE_ALPHA_(46);
    const float CombatNode::WING_IMAGE_SCALE_(0.65f);
    const float CombatNode::WING_IMAGE_HORIZ_OFFSET_(0.333f);
    const double CombatNode::WING_IMAGE_ANIM_SPEED_(8.0);
    const float CombatNode::WING_IMAGE_ROTATION_MAX_(90.0f);
    const std::size_t CombatNode::HEALTH_LINE_COUNT_ { 10 };

    CombatNode::CombatNode(const creature::CreaturePtr_t CREATURE_PTR)
        : Entity(
            std::string("CombatNode_of_\"").append(CREATURE_PTR->Name()).append("\""),
            sf::FloatRect())
        , creatureNameInfo_()
        , isPlayer_(CREATURE_PTR->IsPlayerCharacter())
        , nameText_(
              CREATURE_PTR->Name(),
              creatureNameInfo_.DefaultFont(),
              gui::FontManager::Instance()->Size_CombatCreatureLabels())
        , condText_(
              "",
              creatureNameInfo_.DefaultFont(),
              gui::FontManager::Instance()->Size_CombatCreatureLabels())
        , blockingPos_(0)
        , cachedTexture_(gui::LoadAndCacheImage(CREATURE_PTR))
        , sprite_(cachedTexture_.Get())
        , creatureImageColor_()
        , isSummaryView_(false)
        , isMoving_(false)
        , creaturePtr_(CREATURE_PTR)
        , willDraw_(true)
        , crossBonesCachedTexture_(
              gui::CombatImageType::RandomConfigFileKey(gui::CombatImageType::CrossBones))
        , crossBonesSprite_(crossBonesCachedTexture_.Get())
        , willShowCrossBones_(false)
        , healthRatioDisplayed_(0.0f)

        // these sf::Color initializers don't matter, see HealthChangeTasks() below
        , healthLineColor_()
        , healthLineColorRed_()
        , healthLineColorTick_()

        , healthLines_(sf::Lines, HEALTH_LINE_COUNT_)
        , isDead_(false)
        , wingCachedTexture_(gui::LoadAndCacheImage(
              gui::CombatImageType::Wing,
              gui::ImageOptions(
                  ((!isPlayer_) ? gui::ImageOpt::FlipHoriz | gui::ImageOpt::Default
                                : gui::ImageOpt::Default))))
        , wingSprite_(wingCachedTexture_.Get())
        , isFlying_(false)
        , wingFlapSlider_(0.0f, WING_IMAGE_ROTATION_MAX_, WING_IMAGE_ANIM_SPEED_)
        , imagePosV_(0.0f, 0.0f)
        , imagePosOffsetV_(0.0f, 0.0f)
        , willShowSelectAnim_(false)
        , selectAnimSprite_()
    {
        nameText_.setFillColor(NameColor());

        UpdateConditionText();
        condText_.setFillColor(sf::Color(200, 200, 200, 200));

        HealthChangeTasks();

        if (isPlayer_)
        {
            creatureImageColor_ = CREATURE_IMAGE_COLOR_PLAYER_;
        }
        else
        {
            creatureImageColor_ = CREATURE_IMAGE_COLOR_NONPLAYER_;
        }

        sprite_.setColor(creatureImageColor_);

        // um...This was the only way I could get it to work...zTn 2017-4-6
        IsFlying(true);
        IsFlying(false);

        SetupHealthLines();
    }

    CombatNode::~CombatNode() = default;

    const std::string CombatNode::ToString() const
    {
        std::ostringstream ss;
        ss << "CombatNode " << ((isPlayer_) ? "Player " : "NonPlayer ") << creaturePtr_->RaceName()
           << " " << creaturePtr_->RoleName();

        if (isPlayer_)
        {
            ss << " \"" << creaturePtr_->Name() << "\"";
        }

        ss << " blocking_pos=" << blockingPos_;

        return ss.str();
    }

    void CombatNode::SetRegion(const sf::FloatRect & NEW_REGION)
    {
        Entity::SetEntityRegion(NEW_REGION);
        SetEntityPos(NEW_REGION.left, NEW_REGION.top); // see local override
        SetImageScale();
        SetCrossBonesImageScale();
        SetWingImageScaleAndOrigin();
    }

    void CombatNode::SetRegion(const float NEW_RATIO)
    {
        const auto ORIG_RECT { GetEntityRegion() };
        auto newRect { ORIG_RECT };
        newRect.width *= NEW_RATIO;
        newRect.left += (ORIG_RECT.width - newRect.width) * 0.5f;
        newRect.height *= NEW_RATIO;
        newRect.top += (ORIG_RECT.height - newRect.height) * 0.5f;
        SetRegion(newRect);
    }

    void CombatNode::SetCharacterSize(const unsigned int NEW_CHARACTER_SIZE)
    {
        nameText_.setCharacterSize(NEW_CHARACTER_SIZE);
        condText_.setCharacterSize(NEW_CHARACTER_SIZE);
        SetupHealthLines();
    }

    void CombatNode::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // don't draw if outside the battlefield rect
        if (false == WillDraw())
        {
            return;
        }

        // creature outline image
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(sprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }

        // select anim image
        if (willShowSelectAnim_)
        {
            const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
            states.blendMode = sf::BlendAdd;
            target.draw(selectAnimSprite_, states);
            states.blendMode = ORIG_BLEND_MODE;
        }

        if (isPlayer_)
        {
            target.draw(nameText_, states);
            target.draw(condText_, states);
        }

        if (false == isSummaryView_)
        {
            if (willShowCrossBones_)
            {
                const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
                states.blendMode = sf::BlendAdd;
                target.draw(crossBonesSprite_, states);
                states.blendMode = ORIG_BLEND_MODE;
            }

            if ((false == isDead_) && (false == isMoving_))
            {
                target.draw(healthLines_, states);

                if (isFlying_)
                {
                    target.draw(wingSprite_, states);
                }

                if (false == isPlayer_)
                {
                    target.draw(nameText_, states);
                    target.draw(condText_, states);
                }
            }
        }
    }

    void CombatNode::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        Entity::SetEntityPos(POS_LEFT, POS_TOP);

        SetTextPositions();

        imagePosV_.x = (entityRegion_.left + (entityRegion_.width * 0.5f))
            - (sprite_.getGlobalBounds().width * 0.5f);

        imagePosV_.y = (entityRegion_.top + (entityRegion_.height * 0.5f))
            - (sprite_.getGlobalBounds().height * 0.5f);

        UpdateImagePosition();
        SetWingImagePosition();

        const auto CROSSBONES_IMAGE_POS_LEFT { (entityRegion_.left + (entityRegion_.width * 0.5f))
                                               - (crossBonesSprite_.getGlobalBounds().width
                                                  * 0.5f) };

        const auto CROSSBONES_IMAGE_POS_TOP { (entityRegion_.top + (entityRegion_.height * 0.5f))
                                              - (crossBonesSprite_.getGlobalBounds().height
                                                 * 0.5f) };

        crossBonesSprite_.setPosition(CROSSBONES_IMAGE_POS_LEFT, CROSSBONES_IMAGE_POS_TOP);
    }

    void CombatNode::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        nameText_.move(HORIZ, VERT);
        condText_.move(HORIZ, VERT);

        imagePosV_.x += HORIZ;
        imagePosV_.y += VERT;
        UpdateImagePosition();

        wingSprite_.move(HORIZ, VERT);
        crossBonesSprite_.move(HORIZ, VERT);

        SetupHealthLines();
    }

    bool CombatNode::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_MOUSE_STATE_CHANGE(Entity::UpdateMousePos(MOUSE_POS_V));
        SetHighlight((gui::MouseState::Over == entityMouseState_), DID_MOUSE_STATE_CHANGE);
        return DID_MOUSE_STATE_CHANGE;
    }

    void CombatNode::SetToneDown(const float TONE_DOWN_VAL)
    {
        nameText_.setFillColor(AdjustColorForToneDown(NameColor(), TONE_DOWN_VAL));
        condText_.setFillColor(AdjustColorForToneDown(CONDITION_COLOR_, TONE_DOWN_VAL));

        healthLineColor_ = AdjustColorForToneDown(HealthColor(), TONE_DOWN_VAL);
        healthLineColorRed_ = AdjustColorForToneDown(HealthColorRed(), TONE_DOWN_VAL);
        healthLineColorTick_ = AdjustColorForToneDown(HealthColorTick(), TONE_DOWN_VAL);
    }

    void CombatNode::UpdateConditionText()
    {
        condText_.setString(
            creaturePtr_->ConditionNames(3, creature::condition::Severity::LEAST_BENEFITIAL));

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
            wingSprite_.setColor(sf::Color(255, 255, 255, DECAL_IMAGE_ALPHA_));
            wingSprite_.setRotation(0.0f);
            SetWingImageScaleAndOrigin();
            SetWingImagePosition();

            wingFlapSlider_ = gui::SliderOscillator<float>(0.0f, WING_IMAGE_ROTATION_MAX_, 1.0f);
        }

        isFlying_ = IS_FLYING;
    }

    bool CombatNode::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isFlying_)
        {
            if (isPlayer_)
            {
                wingSprite_.setRotation(wingFlapSlider_.Update(ELAPSED_TIME_SECONDS) * -1.0f);
            }
            else
            {
                wingSprite_.setRotation(wingFlapSlider_.Update(ELAPSED_TIME_SECONDS));
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
        const auto WILL_SETUP_SKULL_IMAGE { (
            isPlayer_ && creaturePtr_->IsDead() && (false == willShowCrossBones_)) };

        if (WILL_SETUP_SKULL_IMAGE)
        {
            willShowCrossBones_ = true;
            isDead_ = true;
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
        sf::Color c(
            255,
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)),
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)));

        c.g = static_cast<sf::Uint8>(static_cast<float>(c.g) * healthRatioDisplayed_);
        c.b = static_cast<sf::Uint8>(static_cast<float>(c.b) * healthRatioDisplayed_);
        return c;
    }

    const sf::Color CombatNode::HealthColorRed() const
    {
        return sf::Color(
            255,
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)),
            static_cast<sf::Uint8>(255 - static_cast<int>(HIGHLIGHT_ADJ_VALUE_)));
    }

    const sf::Color CombatNode::HealthColorTick() const { return HealthColorRed(); }

    const sf::Color CombatNode::AdjustColorForToneDown(
        const sf::Color & OLD_COLOR, const float TONE_DOWN_VAL) const
    {
        sf::Color newColor(OLD_COLOR);
        newColor.r = static_cast<sf::Uint8>(
            static_cast<float>(OLD_COLOR.r) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));

        newColor.g = static_cast<sf::Uint8>(
            static_cast<float>(OLD_COLOR.g) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));

        newColor.b = static_cast<sf::Uint8>(
            static_cast<float>(OLD_COLOR.b) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));

        return newColor;
    }

    void CombatNode::SetTextPositions(const bool DID_TRANSITION_TO_SUMMARY_VIEW)
    {
        const float NAME_TEXT_POS_TOP(
            (entityRegion_.top + (entityRegion_.height * 0.7f))
            - (nameText_.getGlobalBounds().height * 0.5f));

        const float COND_TEXT_POS_TOP(
            (entityRegion_.top + entityRegion_.height)
            - (condText_.getGlobalBounds().height * 0.5f));

        if (DID_TRANSITION_TO_SUMMARY_VIEW)
        {
            nameText_.setPosition(
                entityRegion_.left + entityRegion_.width + 15.0f, NAME_TEXT_POS_TOP);

            condText_.setPosition(
                gui::Display::Instance()->GetWinWidth() + 1.0f,
                gui::Display::Instance()->GetWinHeight() + 1.0f);
        }
        else
        {
            nameText_.setPosition(
                (entityRegion_.left + (entityRegion_.width * 0.5f))
                    - (nameText_.getGlobalBounds().width * 0.5f),
                NAME_TEXT_POS_TOP);

            condText_.setPosition(
                (entityRegion_.left + (entityRegion_.width * 0.5f))
                    - (condText_.getGlobalBounds().width * 0.5f),
                COND_TEXT_POS_TOP);
        }

        SetupHealthLines();
    }

    void CombatNode::SetWingImagePosition()
    {
        const auto ORIG_ORIGIN { wingSprite_.getOrigin() };
        wingSprite_.setOrigin(0.0f, 0.0f);

        wingSprite_.setPosition(
            (sprite_.getGlobalBounds().left
             - (wingSprite_.getGlobalBounds().width * WING_IMAGE_HORIZ_OFFSET_))
                + wingSprite_.getGlobalBounds().width,
            (sprite_.getGlobalBounds().top + sprite_.getGlobalBounds().height)
                - (wingSprite_.getGlobalBounds().height * 0.5f));

        wingSprite_.setOrigin(ORIG_ORIGIN);
    }

    void CombatNode::SetWingImageScaleAndOrigin()
    {
        const auto WING_SCALE { (entityRegion_.height / wingSprite_.getLocalBounds().height)
                                * WING_IMAGE_SCALE_ };

        wingSprite_.setScale(WING_SCALE, WING_SCALE);

        if (isPlayer_)
        {
            wingSprite_.setOrigin(
                wingSprite_.getLocalBounds().width, wingSprite_.getLocalBounds().height);
        }
        else
        {
            wingSprite_.setOrigin(0.0f, wingSprite_.getLocalBounds().height);
        }
    }

    void CombatNode::UpdateImagePosition() { sprite_.setPosition(imagePosV_ + imagePosOffsetV_); }

    void CombatNode::SetHighlight(const bool WILL_HIGHLIGHT, const bool WILL_PLAY_SOUND_EFFECT)
    {
        if (WILL_HIGHLIGHT)
        {
            if (WILL_PLAY_SOUND_EFFECT)
            {
                gui::SoundManager::Instance()->PlaySfx_TickOn();
            }

            nameText_.setFillColor(NameColor() + HIGHLIGHT_ADJ_COLOR_);
            condText_.setFillColor(CONDITION_COLOR_ + HIGHLIGHT_ADJ_COLOR_);
            healthLineColor_ = HealthColor() + HIGHLIGHT_ADJ_COLOR_;
            healthLineColorTick_ = HealthColorTick() + HIGHLIGHT_ADJ_COLOR_;

            sprite_.setColor(
                creatureImageColor_ + sf::Color(0, 0, 0, CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_));
        }
        else
        {
            if (WILL_PLAY_SOUND_EFFECT)
            {
                gui::SoundManager::Instance()->PlaySfx_TickOff();
            }

            nameText_.setFillColor(NameColor());
            condText_.setFillColor(CONDITION_COLOR_);
            healthLineColor_ = HealthColor();
            healthLineColorTick_ = HealthColorTick();
            sprite_.setColor(creatureImageColor_);
        }
    }

    void CombatNode::SetRotationDegrees(const float DEGREES)
    {
        sprite_.setOrigin(
            sprite_.getGlobalBounds().width * 0.5f, sprite_.getGlobalBounds().height * 0.5f);

        sprite_.setRotation(DEGREES);
        sprite_.setOrigin(0.0f, 0.0f);
    }

    void CombatNode::SetupSkullAndCrossBones()
    {
        crossBonesSprite_.setScale(1.0f, 1.0f);
        crossBonesSprite_.setPosition(0.0f, 0.0f);
        crossBonesSprite_.setColor(sf::Color(255, 255, 255, DECAL_IMAGE_ALPHA_));
        SetCrossBonesImageScale();
        SetEntityPos(GetEntityPos().x, GetEntityPos().y);
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

        MoveEntityPos(
            (GetEntityRegion().width * SLIDER_POS) * SLIDER_POS,
            (GetEntityRegion().height * SLIDER_POS) * SLIDER_POS);
    }

    void CombatNode::SelectAnimStart()
    {
        willShowSelectAnim_ = true;
        selectAnimSprite_ = sprite_;
    }

    void CombatNode::SelectAnimUpdate(const float SLIDER_RATIO)
    {
        // grow
        const auto SCALE { 1.0f + (sfutil::MapByRes(0.5f, 2.5f) * SLIDER_RATIO) };
        selectAnimSprite_.setScale(SCALE, SCALE);

        // re-center
        const auto HORIZ_ADJ {
            (selectAnimSprite_.getGlobalBounds().width - sprite_.getGlobalBounds().width) * 0.5f
        };

        const auto VERT_ADJ {
            (selectAnimSprite_.getGlobalBounds().height - sprite_.getGlobalBounds().height) * 0.5f
        };

        selectAnimSprite_.setPosition(
            sprite_.getPosition().x - HORIZ_ADJ, sprite_.getPosition().y - VERT_ADJ);

        // fade-out
        auto color { selectAnimSprite_.getColor() };
        color.a = static_cast<sf::Uint8>(sprite_.getColor().a * (1.0f - SLIDER_RATIO));
        selectAnimSprite_.setColor(color);
    }

    void CombatNode::SelectAnimStop() { willShowSelectAnim_ = false; }

    void CombatNode::SetupHealthLines()
    {
        const auto POS_TOP { nameText_.getPosition().y
                             + (nameText_.getGlobalBounds().height * 2.0f) };

        const auto LINE_LEN_MARGIN { 0.25f * entityRegion_.width };

        const auto LINE_POS_LEFT { entityRegion_.left + LINE_LEN_MARGIN };

        const auto LINE_LEN { entityRegion_.width - (LINE_LEN_MARGIN * 2.0f) };

        const auto LINE_TICK_HEIGHT { 4.0f };

        const auto LINE_RATIO_RIGHT { LINE_POS_LEFT + (LINE_LEN * healthRatioDisplayed_) };

        const auto LINE_TOTAL_RIGHT { LINE_POS_LEFT + LINE_LEN };

        healthLines_[0].position = sf::Vector2f(LINE_POS_LEFT, POS_TOP);
        healthLines_[1].position = sf::Vector2f(LINE_RATIO_RIGHT, POS_TOP);
        healthLines_[2].position = sf::Vector2f(LINE_POS_LEFT, POS_TOP + 1);
        healthLines_[3].position = sf::Vector2f(LINE_RATIO_RIGHT, POS_TOP + 1);
        healthLines_[4].position = sf::Vector2f(LINE_POS_LEFT, POS_TOP + 2);
        healthLines_[5].position = sf::Vector2f(LINE_TOTAL_RIGHT, POS_TOP + 2);
        healthLines_[6].position = sf::Vector2f(LINE_POS_LEFT, POS_TOP - LINE_TICK_HEIGHT);
        healthLines_[7].position = sf::Vector2f(LINE_POS_LEFT, POS_TOP + LINE_TICK_HEIGHT + 2);
        healthLines_[8].position = sf::Vector2f(LINE_TOTAL_RIGHT, POS_TOP - LINE_TICK_HEIGHT);
        healthLines_[9].position = sf::Vector2f(LINE_TOTAL_RIGHT, POS_TOP + LINE_TICK_HEIGHT + 2);

        healthLines_[0].color = healthLineColor_;
        healthLines_[1].color = healthLineColor_;
        healthLines_[2].color = healthLineColor_;
        healthLines_[3].color = healthLineColor_;
        healthLines_[4].color = healthLineColor_;
        healthLines_[5].color = healthLineColorRed_;
        healthLines_[6].color = healthLineColorTick_;
        healthLines_[7].color = healthLineColorTick_;
        healthLines_[8].color = healthLineColorTick_;
        healthLines_[9].color = healthLineColorTick_;
    }

} // namespace combat
} // namespace heroespath
