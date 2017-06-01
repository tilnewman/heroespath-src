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
// credit.cpp
//
#include "credit.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/game-data-file.hpp"


namespace game
{
namespace stage
{

    Credit::Credit(sf::FloatRect &                  creditsRegion,
                   const std::string &              TITLE,
                   const std::string &              CONTENT_TEXT,
                   const credit_media_type::Enum    MEDIA_TYPE,
                   const std::string &              MEDIA_PATH,
                   const float                      MEDIA_SCALE,
                   const sf::Vector2f &             MEDIA_POS_ADJUSTMENT,
                   const float                      ANIM_FRAME_TIME_SEC,
                   const unsigned int               ANIM_FRAME_COUNT,
                   const unsigned int               ANIM_FRAME_SIZE_HORIZ,
                   const unsigned int               ANIM_FRAME_SIZE_VERT)
    :
        titleTextRegionUPtr_  (),
        contentTextRegionUPtr_(),
        contentType_          (MEDIA_TYPE),
        path_                 (MEDIA_PATH),
        posAdj_               (MEDIA_POS_ADJUSTMENT),
        texture_              (),
        sprite_               (),
        multiTextureAnimSPtr_ (),
        singleTextureAnimSPtr_()
    {
        Setup(creditsRegion,
              TITLE,
              sfml_util::FontManager::Instance()->Font_Typical(),
              sfml_util::FontManager::Instance()->Size_Smallish(),
              CONTENT_TEXT,
              MEDIA_TYPE,
              MEDIA_PATH,
              MEDIA_SCALE,
              MEDIA_POS_ADJUSTMENT,
              ANIM_FRAME_TIME_SEC,
              ANIM_FRAME_COUNT,
              ANIM_FRAME_SIZE_HORIZ,
              ANIM_FRAME_SIZE_VERT);
    }


    Credit::Credit(sf::FloatRect &                  creditsRegion,
                   const std::string &              TITLE,
                   const sfml_util::FontPtr_t       FONT_PTR,
                   const std::string &              CONTENT_TEXT,
                   const credit_media_type::Enum    MEDIA_TYPE,
                   const std::string &              MEDIA_PATH,
                   const float                      MEDIA_SCALE)
    :
        titleTextRegionUPtr_  (),
        contentTextRegionUPtr_(),
        contentType_          (MEDIA_TYPE),
        path_                 (MEDIA_PATH),
        posAdj_               (sf::Vector2f(0.0f, 0.0f)),
        texture_              (),
        sprite_               (),
        multiTextureAnimSPtr_ (),
        singleTextureAnimSPtr_()
    {
        Setup(creditsRegion,
              TITLE,
              FONT_PTR,
              sfml_util::FontManager::Instance()->Size_Larger(),
              CONTENT_TEXT,
              MEDIA_TYPE,
              MEDIA_PATH,
              MEDIA_SCALE,
              sf::Vector2f(0.0f, 0.0f),
              0.0f,
              0,
              0,
              0);
    }


    void Credit::Setup(sf::FloatRect &                  creditsRegion,
                       const std::string &              TITLE,
                       const sfml_util::FontPtr_t       TITLE_FONT_PTR,
                       const unsigned int               TITLE_FONT_SIZE,
                       const std::string &              CONTENT_TEXT,
                       const credit_media_type::Enum    MEDIA_TYPE,
                       const std::string &              MEDIA_PATH,
                       const float                      MEDIA_SCALE,
                       const sf::Vector2f &             MEDIA_POS_ADJUSTMENT,
                       const float                      ANIM_FRAME_TIME_SEC,
                       const unsigned int               ANIM_FRAME_COUNT,
                       const unsigned int               ANIM_FRAME_SIZE_HORIZ,
                       const unsigned int               ANIM_FRAME_SIZE_VERT)
    {
        const float VERT_PAD_SMALL(0.0f);

        if (MEDIA_TYPE == credit_media_type::Image)
        {
            sfml_util::LoadImageOrTexture<sf::Texture>(texture_, GameDataFile::Instance()->GetMediaPath(MEDIA_PATH));
            texture_.setSmooth(true);
            sprite_.setTexture(texture_);
            sprite_.setScale(MEDIA_SCALE, MEDIA_SCALE);

            sprite_.setPosition(PositionMedia(sf::Vector2f(sprite_.getGlobalBounds().width, sprite_.getGlobalBounds().height),
                                                           creditsRegion,
                                                           MEDIA_POS_ADJUSTMENT));

            creditsRegion.top += sprite_.getGlobalBounds().height + VERT_PAD_SMALL;
        }
        else if (MEDIA_TYPE == credit_media_type::AnimSingleTexture)
        {
            const sf::Vector2f POS_V(PositionMedia(sf::Vector2f(0.0f, 0.0f),
                                                   creditsRegion,
                                                   MEDIA_POS_ADJUSTMENT));

            singleTextureAnimSPtr_ = std::make_shared<sfml_util::SingleTextureAnimation>(
                "Credit_",
                GameDataFile::Instance()->GetMediaPath(MEDIA_PATH),
                POS_V.x,
                POS_V.y,
                ANIM_FRAME_SIZE_HORIZ,
                ANIM_FRAME_SIZE_VERT,
                ANIM_FRAME_TIME_SEC,
                ANIM_FRAME_COUNT);

            singleTextureAnimSPtr_->MovePosition((singleTextureAnimSPtr_->GetEntityRegion().width * -0.5f), 0.0f);

            creditsRegion.top += singleTextureAnimSPtr_->GetEntityRegion().height + VERT_PAD_SMALL;
        }
        else if (MEDIA_TYPE == credit_media_type::AnimMultiTexture)
        {
            const sf::Vector2f POS_V(PositionMedia(sf::Vector2f(0.0f, 0.0f),
                                                   creditsRegion,
                                                   MEDIA_POS_ADJUSTMENT));

            multiTextureAnimSPtr_ = std::make_shared<sfml_util::MultiTextureAnimation>(
                "Credit_" + TITLE,
                GameDataFile::Instance()->GetMediaPath(MEDIA_PATH),
                POS_V.x,
                POS_V.y,
                ANIM_FRAME_TIME_SEC,
                MEDIA_SCALE,
                MEDIA_SCALE);

            multiTextureAnimSPtr_->MovePosition((multiTextureAnimSPtr_->GetEntityRegion().width * -0.5f), 0.0f);

            creditsRegion.top += multiTextureAnimSPtr_->GetEntityRegion().height + VERT_PAD_SMALL;
        }

        const sfml_util::gui::TextInfo TEXT_INFO_TITLE(TITLE,
                                                       TITLE_FONT_PTR,
                                                       TITLE_FONT_SIZE,
                                                       sf::Color(255, 255, 255, 200),
                                                       sfml_util::Justified::Center);

        titleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CreditTitle_" + TITLE, TEXT_INFO_TITLE, creditsRegion);

        if (TITLE != " ")
        {
            creditsRegion.top += titleTextRegionUPtr_->
                GetEntityRegion().height + VERT_PAD_SMALL;
        }

        sfml_util::gui::TextInfo textInfoContent(
            CONTENT_TEXT,
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sfml_util::Justified::Center);

        //if there is a lot of text (multi-lined), reduce the size to look better
        if (boost::algorithm::icontains(CONTENT_TEXT, "\n"))
        {
            textInfoContent.charSize -= static_cast<unsigned int>(sfml_util::MapByRes(3, 50));
        }

        contentTextRegionUPtr_.reset(new sfml_util::gui::TextRegion(
            "CreditContent", textInfoContent, creditsRegion));

        creditsRegion.top += contentTextRegionUPtr_->GetEntityRegion().height;

        const float SPACE_BETWEEN_CREDITS(sfml_util::MapByRes(100.0f, 200.0f));
        creditsRegion.top += SPACE_BETWEEN_CREDITS;
    }


    void Credit::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        if (titleTextRegionUPtr_.get() != nullptr)
        {
            target.draw( * titleTextRegionUPtr_, states);
        }

        if (contentTextRegionUPtr_.get() != nullptr)
        {
            target.draw( * contentTextRegionUPtr_, states);
        }

        if (multiTextureAnimSPtr_.get() != nullptr)
        {
            multiTextureAnimSPtr_->draw(target, states);
        }

        if (singleTextureAnimSPtr_.get() != nullptr)
        {
            singleTextureAnimSPtr_->draw(target, states);
        }

        if (credit_media_type::Image == contentType_)
        {
            target.draw(sprite_, states);
        }
    }


    void Credit::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (multiTextureAnimSPtr_.get() != nullptr)
        {
            multiTextureAnimSPtr_->UpdateTime(ELAPSED_TIME_SECONDS);
        }

        if (singleTextureAnimSPtr_.get() != nullptr)
        {
            singleTextureAnimSPtr_->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }


    void Credit::Move(const float ADJ_HORIZ, const float ADJ_VERT)
    {
        if (titleTextRegionUPtr_.get() != nullptr)
        {
            titleTextRegionUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        if (contentTextRegionUPtr_.get() != nullptr)
        {
            contentTextRegionUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        sprite_.move(ADJ_HORIZ, ADJ_VERT);

        if (multiTextureAnimSPtr_.get() != nullptr)
        {
            multiTextureAnimSPtr_->MovePosition(ADJ_HORIZ, ADJ_VERT);
        }

        if (singleTextureAnimSPtr_.get() != nullptr)
        {
            singleTextureAnimSPtr_->MovePosition(ADJ_HORIZ, ADJ_VERT);
        }
    }


    const sf::Vector2f Credit::PositionMedia(const sf::Vector2f &  MEDIA_SIZE,
                                             const sf::FloatRect & CREDITS_REGION,
                                             const sf::Vector2f &  POS_ADJUSTMENTS)
    {
        sf::Vector2f v;
        v.y = CREDITS_REGION.top;
        v.x = (CREDITS_REGION.left + (CREDITS_REGION.width * 0.5f)) - (MEDIA_SIZE.x * 0.5f);
        v += POS_ADJUSTMENTS;
        return v;
    }

}
}
