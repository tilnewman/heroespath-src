//
// combat-node.cpp
//
#include "combat-node.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"
#include "sfml-util/random.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/creature/name-info.hpp"
#include "heroespath/game-data-file.hpp"


namespace heroespath
{
namespace combat
{

    const sf::Uint8 CombatNode::HIGHLIGHT_ADJ_VALUE_                 (sfml_util::FontManager::ColorValue_Highlight());
    const sf::Color CombatNode::HIGHLIGHT_ADJ_COLOR_                 (sf::Color(HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, HIGHLIGHT_ADJ_VALUE_, 0));
    const sf::Color CombatNode::PLAYER_NAME_COLOR_                   (sf::Color(255, 255, 240) - HIGHLIGHT_ADJ_COLOR_);
    const sf::Color CombatNode::CONDITION_COLOR_                     (sfml_util::FontManager::Color_Light() - HIGHLIGHT_ADJ_COLOR_);
    const sf::Color CombatNode::NONPLAYER_NAME_COLOR_                (sf::Color(255, 255 - HIGHLIGHT_ADJ_VALUE_, 255 - HIGHLIGHT_ADJ_VALUE_) - HIGHLIGHT_ADJ_COLOR_);
    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_HIGHLIGHT_VALUE_(32);
    const sf::Uint8 CombatNode::CREATURE_IMAGE_COLOR_ALPHA_          (32);
    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_PLAYER_         (sf::Color(255, 243, 201, CREATURE_IMAGE_COLOR_ALPHA_));//slightly orange
    const sf::Color CombatNode::CREATURE_IMAGE_COLOR_NONPLAYER_      (sf::Color(255, 235, 235, CREATURE_IMAGE_COLOR_ALPHA_));//slightly red
    const float     CombatNode::TONED_DOWN_MULT_                     (0.64f);
    const sf::Uint8 CombatNode::WING_IMAGE_ALPHA_                    (46);
    const float     CombatNode::WING_IMAGE_SCALE_                    (0.65f);
    const float     CombatNode::WING_IMAGE_HORIZ_OFFSET_             (0.333f);
    const float     CombatNode::WING_IMAGE_ANIM_SPEED_               (4.0f);
    const float     CombatNode::WING_IMAGE_ROTATION_MAX_             (-90.0f);
    //
    sfml_util::TextureSPtr_t CombatNode::wingTextureSPtr_(nullptr);


    CombatNode::CombatNode(const creature::CreatureSPtr_t & CREATURE_SPTR,
                           const sfml_util::FontSPtr_t &    FONT_SPTR,
                           const unsigned int               FONT_CHAR_SIZE)
    :
        GuiEntity           (std::string("CombatNode_of_\"").append(CREATURE_SPTR->Name()).append("\""), sf::FloatRect()),
        nameTextObj_        (CREATURE_SPTR->Name(), * FONT_SPTR, FONT_CHAR_SIZE),
        condTextObj_        ("", * FONT_SPTR, FONT_CHAR_SIZE),
        blockingPos_        (0),
        type_               (NodeType::Position),
        healthLineColor_    (),//this initializer doesn't matter, see constructor body below
        healthLineColorRed_ (),// "
        healthLineColorTick_(),// "
        textureSPtr_        (sfml_util::gui::CreatureImageManager::Instance()->GetImage(CREATURE_SPTR->ImageFilename(), CREATURE_SPTR->IsPlayerCharacter())),
        sprite_             (),
        creatureImageColor_ (),
        isSummaryView_      (false),
        isMoving_           (false),
        creatureSPtr_       (CREATURE_SPTR),
        wingSprite_         (),
        isFlying_           (false),
        wingFlapSlider_     (WING_IMAGE_ANIM_SPEED_),
        imagePosV_          (0.0f, 0.0f),
        imagePosOffsetV_    (0.0f, 0.0f),
        soundsTickOnSPtr_   (sfml_util::SoundManager::Instance()->StaticSounds_TickOn()),
        soundsTickOffSPtr_  (sfml_util::SoundManager::Instance()->StaticSounds_TickOff())
    {
        const sf::Color NAME_COLOR((CREATURE_SPTR->IsPlayerCharacter()) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);
        sfml_util::SetTextColor(nameTextObj_, NAME_COLOR);

        UpdateConditionText();
        sfml_util::SetTextColor(condTextObj_, sf::Color(200, 200, 200, 200));

        HealthChangeTasks();

        sprite_.setTexture( * textureSPtr_);

        //sprite color
        if (CREATURE_SPTR->IsPlayerCharacter())
            creatureImageColor_ = CREATURE_IMAGE_COLOR_PLAYER_;
        else
            creatureImageColor_ = CREATURE_IMAGE_COLOR_NONPLAYER_;
        sprite_.setColor(creatureImageColor_);

        //wing image
        if (wingTextureSPtr_.get() == nullptr)
            wingTextureSPtr_ = sfml_util::gui::CombatImageManager::Instance()->Get(sfml_util::gui::CombatImageType::Wing, ! creatureSPtr_->IsPlayerCharacter());

        //um...This was the only way I could get it to work...zTn 2017-4-6
        SetIsFlying(true);
        SetIsFlying(false);
    }


    CombatNode::~CombatNode()
    {}


    const std::string CombatNode::ToString() const
    {
        std::ostringstream ss;
        ss << "CombatNode " << ((creatureSPtr_->IsPlayerCharacter()) ? "Player " : "NonPlayer ") << creatureSPtr_->Race().Name() << " " << creatureSPtr_->Role().Name();

        if (creatureSPtr_->IsPlayerCharacter())
            ss << " \"" << creatureSPtr_->Name() << "\"";

        ss << " node_type=" << NodeType::ToString(type_) << " blocking_pos=" << blockingPos_;

        return ss.str();
    }


    void CombatNode::ResetRegionAndTextSize(const sf::FloatRect & REGION, const unsigned int NAME_CHAR_SIZE)
    {
        entityRegion_ = REGION;
        nameTextObj_.setCharacterSize(NAME_CHAR_SIZE);
        condTextObj_.setCharacterSize(NAME_CHAR_SIZE);

        SetEntityPos(REGION.left, REGION.top);//see local override

        //scale
        //start by setting the scale to whatever will match the desired vertical size, which is stored in entityRegion_
        const float SCALE_VERT(entityRegion_.height / sprite_.getLocalBounds().height);
        sprite_.setScale(SCALE_VERT, SCALE_VERT);
        //...then reduce the scale if the width of the resulting sprite is too large, which will be unlikely given that the majority of creature images are tall not wide
        if (sprite_.getGlobalBounds().width > entityRegion_.width)
        {
            const float SCALE_HORIZ(entityRegion_.width / sprite_.getLocalBounds().width);

            if (SCALE_HORIZ < SCALE_VERT)
            {
                sprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);
            }
        }

        //scale wing image
        SetWingImageScaleAndOrigin();
    }


    void CombatNode::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //don't draw if outside the battlefield rect
        if (false == GetEntityWillDraw())
            return;

        //sfml_util::DrawRectangle(target, states, sf::FloatRect(entityRegion_));

        //creature outline
        const sf::BlendMode ORIG_BLEND_MODE(states.blendMode);
        states.blendMode = sf::BlendAdd;
        target.draw(sprite_, states);
        states.blendMode = ORIG_BLEND_MODE;

        if (isFlying_)
            target.draw(wingSprite_, states);

        if ((false == isMoving_) && (false == isSummaryView_))
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
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + (HEALTH_LINE_LEN * Creature()->HealthRatio()), HEALTH_POS_TOP)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT, HEALTH_POS_TOP + 1)),
                sf::Vertex(sf::Vector2f(HEALTH_LINE_POS_LEFT + (HEALTH_LINE_LEN * Creature()->HealthRatio()), HEALTH_POS_TOP + 1)),
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
        const sf::Color NAME_COLOR((creatureSPtr_->IsPlayerCharacter()) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);
        sfml_util::SetTextColor(nameTextObj_, AdjustColorForToneDown(NAME_COLOR, TONE_DOWN_VAL));
        sfml_util::SetTextColor(condTextObj_, AdjustColorForToneDown(CONDITION_COLOR_, TONE_DOWN_VAL));

        healthLineColor_ = AdjustColorForToneDown(HealthColor(), TONE_DOWN_VAL);
        healthLineColorRed_ = AdjustColorForToneDown(HealthColorRed(), TONE_DOWN_VAL);
        healthLineColorTick_ = AdjustColorForToneDown(HealthColorTick(), TONE_DOWN_VAL);
    }


    void CombatNode::UpdateConditionText()
    {
        condTextObj_.setString(creatureSPtr_->ConditionList(3, creature::condition::Severity::BENEFITIAL));
        SetTextPositions();
    }


    void CombatNode::IsSummaryView(const bool IS_IN_SUMMARY_VIEW)
    {
        if ((false == isSummaryView_) && IS_IN_SUMMARY_VIEW)
            SetTextPositions(true);

        isSummaryView_ = IS_IN_SUMMARY_VIEW;
    }


    void CombatNode::SetIsFlying(const bool IS_FLYING)
    {
        if (IS_FLYING)
        {
            wingSprite_.setTexture( * wingTextureSPtr_);
            wingSprite_.setTextureRect( sf::IntRect(0, 0, wingTextureSPtr_->getSize().x, wingTextureSPtr_->getSize().y) );
            wingSprite_.setColor(sf::Color(255, 255, 255, WING_IMAGE_ALPHA_));
            wingSprite_.setRotation(0.0f);
            SetWingImageScaleAndOrigin();
            SetWingImagePosition();

            wingFlapSlider_.Reset(WING_IMAGE_ANIM_SPEED_);
        }

        isFlying_ = IS_FLYING;
    }


    bool CombatNode::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        //Skip check of isFlying_ since calculation is faster than branching,
        //and because there is already a isFlying_ check on the draw function.
        if (creatureSPtr_->IsPlayerCharacter())
        {
            wingSprite_.setRotation(WING_IMAGE_ROTATION_MAX_ * wingFlapSlider_.Update(ELAPSED_TIME_SECONDS));
        }
        else
        {
            wingSprite_.setRotation(WING_IMAGE_ROTATION_MAX_ * wingFlapSlider_.Update(ELAPSED_TIME_SECONDS) * -1.0f);
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
        UpdateConditionText();
        healthLineColor_ = HealthColor();
        healthLineColorRed_ = HealthColorRed();
        healthLineColorTick_ = HealthColorTick();
    }


    const sf::Color CombatNode::HealthColor() const
    {
        sf::Color c(255, 255 - HIGHLIGHT_ADJ_VALUE_, 255 - HIGHLIGHT_ADJ_VALUE_);
        c.g = static_cast<sf::Uint8>(static_cast<float>(c.g) * Creature()->HealthRatio());
        c.b = static_cast<sf::Uint8>(static_cast<float>(c.b) * Creature()->HealthRatio());
        return c;
    }


    const sf::Color CombatNode::HealthColorRed() const
    {
        return sf::Color(255, 255 - HIGHLIGHT_ADJ_VALUE_, 255 - HIGHLIGHT_ADJ_VALUE_);
    }


    const sf::Color CombatNode::HealthColorTick() const
    {
        return sf::Color(255, 255 - HIGHLIGHT_ADJ_VALUE_, 255 - HIGHLIGHT_ADJ_VALUE_);
    }


    const sf::Color CombatNode::AdjustColorForToneDown(const sf::Color & OLD_COLOR, const float TONE_DOWN_VAL) const
    {
        sf::Color newColor(OLD_COLOR);
        newColor.r = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.r) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        newColor.g = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.g) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        newColor.b = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.b) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
        //newColor.a = static_cast<sf::Uint8>(static_cast<float>(OLD_COLOR.a) * (1.0f - (TONE_DOWN_VAL * TONED_DOWN_MULT_)));
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

        if (creatureSPtr_->IsPlayerCharacter())
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
        const sf::Color NAME_COLOR((creatureSPtr_->IsPlayerCharacter()) ? PLAYER_NAME_COLOR_ : NONPLAYER_NAME_COLOR_);

        if (WILL_HIGHLIGHT)
        {
            if (WILL_PLAY_SOUND_EFFECT)
            {
                soundsTickOnSPtr_->PlayRandom();
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
                soundsTickOffSPtr_->PlayRandom();
            }

            sfml_util::SetTextColor(nameTextObj_, NAME_COLOR);
            sfml_util::SetTextColor(condTextObj_, CONDITION_COLOR_);
            healthLineColor_ = HealthColor();
            healthLineColorTick_ = HealthColorTick();
            sprite_.setColor(creatureImageColor_);
        }
    }

}
}
