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


namespace heroespath
{
namespace stage
{

    Credit::Credit(sf::FloatRect &     trackingRect,
                   const std::string & TITLE_TEXT,
                   const std::string & CONTENT_TEXT)
    :
        titleTextUPtr_  (),
        contentTextUPtr_(),
        mediaType_      (MediaType::Text),
        mediaPathKey_   (""),
        texture_        (),
        sprite_         (),
        animUPtr_       ()
    {
        Setup(trackingRect,
              TITLE_TEXT,
              sfml_util::FontManager::Instance()->Font_Typical(),
              sfml_util::FontManager::Instance()->Size_Smallish(),
              CONTENT_TEXT,
              mediaType_,
              mediaPathKey_,
              1.0f,
              sfml_util::Animations::Count,
              1.0f,
              0.0f);
    }


    Credit::Credit(sf::FloatRect &                   trackingRect,
                   const std::string &               TITLE_TEXT,
                   const std::string &               CONTENT_TEXT,
                   const sfml_util::Animations::Enum ANIM_ENUM,
                   const float                       ANIM_SCALE,
                   const float                       ANIM_FRAME_TIME_SEC)
    :
        titleTextUPtr_  (),
        contentTextUPtr_(),
        mediaType_      (MediaType::Anim),
        mediaPathKey_   (sfml_util::Animations::MediaPathKey(ANIM_ENUM)),
        texture_        (),
        sprite_         (),
        animUPtr_       ()
    {
        Setup(trackingRect,
              TITLE_TEXT,
              sfml_util::FontManager::Instance()->Font_Typical(),
              sfml_util::FontManager::Instance()->Size_Smallish(),
              CONTENT_TEXT,
              mediaType_,
              mediaPathKey_,
              1.0f,
              ANIM_ENUM,
              ANIM_SCALE,
              ANIM_FRAME_TIME_SEC);
    }


    Credit::Credit(sf::FloatRect &     trackingRect,
                   const std::string & TITLE_TEXT,
                   const std::string & CONTENT_TEXT,
                   const std::string & IMAGE_PATH_KEY,
                   const float         IMAGE_SCALE)
    :
        titleTextUPtr_  (),
        contentTextUPtr_(),
        mediaType_      (MediaType::Image),
        mediaPathKey_   (IMAGE_PATH_KEY),
        texture_        (),
        sprite_         (),
        animUPtr_       ()
    {
        Setup(trackingRect,
              TITLE_TEXT,
              sfml_util::FontManager::Instance()->Font_Typical(),
              sfml_util::FontManager::Instance()->Size_Smallish(),
              CONTENT_TEXT,
              mediaType_,
              mediaPathKey_,
              IMAGE_SCALE,
              sfml_util::Animations::Count,
              1.0f,
              0.0f);
    }


    Credit::Credit(sf::FloatRect &                   trackingRect,
                   const std::string &               TITLE_TEXT,
                   const sfml_util::FontPtr_t        FONT_PTR,
                   const std::string &               CONTENT_TEXT)
    :
        titleTextUPtr_  (),
        contentTextUPtr_(),
        mediaType_      (MediaType::Image),
        mediaPathKey_   ("media-images-logos-openfontlicense"),
        texture_        (),
        sprite_         (),
        animUPtr_       ()
    {
        Setup(trackingRect,
              TITLE_TEXT,
              FONT_PTR,
              sfml_util::FontManager::Instance()->Size_Smallish(),
              CONTENT_TEXT,
              mediaType_,
              mediaPathKey_,
              sfml_util::MapByRes(1.5f, 5.75f),
              sfml_util::Animations::Count,
              1.0f,
              0.0f);
    }


    void Credit::Setup(sf::FloatRect &                   trackingRect,
                       const std::string &               TITLE_TEXT,
                       const sfml_util::FontPtr_t        TITLE_FONT_PTR,
                       const unsigned int                TITLE_FONT_SIZE,
                       const std::string &               CONTENT_TEXT,
                       const MediaType::Enum             MEDIA_TYPE,
                       const std::string &               MEDIA_PATH,
                       const float                       MEDIA_SCALE,
                       const sfml_util::Animations::Enum ANIM_ENUM,
                       const float                       ANIM_SCALE,
                       const float                       ANIM_FRAME_TIME_SEC)
    {
        if (MEDIA_TYPE == MediaType::Image)
        {
            sfml_util::LoadTexture(texture_,
                game::GameDataFile::Instance()->GetMediaPath(MEDIA_PATH));

            sprite_.setTexture(texture_);
            sprite_.setScale(MEDIA_SCALE, MEDIA_SCALE);

            auto const POS_LEFT{ (trackingRect.left + (trackingRect.width * 0.5f)) -
                (sprite_.getGlobalBounds().width * 0.5f) };

            auto const POS_TOP{ (trackingRect.top + (trackingRect.height * 0.5f)) -
                (sprite_.getGlobalBounds().height * 0.5f) };

            sprite_.setPosition(POS_LEFT, POS_TOP);

            trackingRect.top += sprite_.getGlobalBounds().height;
        }
        else if (MEDIA_TYPE == MediaType::Anim)
        {
            //inital size and pos are default (zeros)
            animUPtr_ = sfml_util::AnimationFactory::Make(ANIM_ENUM,
                                                          sf::FloatRect(),
                                                          ANIM_FRAME_TIME_SEC);

            //correct size and position
            auto const WIDTH{ animUPtr_->OrigSize().x * ANIM_SCALE };
            auto const HEIGHT{ animUPtr_->OrigSize().y * ANIM_SCALE };

            auto const POS_LEFT{ (trackingRect.left + (trackingRect.width * 0.5f)) -
                (WIDTH * 0.5f) };

            auto const POS_TOP{ (trackingRect.top + (trackingRect.height * 0.5f)) -
                (HEIGHT * 0.5f) };

            animUPtr_->SetEntityRegion( sf::FloatRect(POS_LEFT, POS_TOP, WIDTH, HEIGHT) );

            //account for height of animation
            trackingRect.top += animUPtr_->GetEntityRegion().height;
        }

        const sfml_util::gui::TextInfo TEXT_INFO_TITLE(TITLE_TEXT,
                                                       TITLE_FONT_PTR,
                                                       TITLE_FONT_SIZE,
                                                       sf::Color(255, 255, 255, 200),
                                                       sfml_util::Justified::Center);

        titleTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CreditTitle_" + TITLE_TEXT, TEXT_INFO_TITLE, trackingRect);

        if (TITLE_TEXT != " ")
        {
            trackingRect.top += titleTextUPtr_->GetEntityRegion().height;
        }

        sfml_util::gui::TextInfo textInfoContent(
            CONTENT_TEXT,
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sfml_util::Justified::Center);

        //if there is a lot of text (multi-lined), reduce the size to look better
        auto const NUM_NEWLINES{ static_cast<int>(std::count(CONTENT_TEXT.begin(),
                                                             CONTENT_TEXT.end(),
                                                             '\n')) };

        if (NUM_NEWLINES > 1)
        {
            auto const FONT_SIZE_REDUCTION{ static_cast<unsigned int>(2 * NUM_NEWLINES) };

            if (textInfoContent.charSize > FONT_SIZE_REDUCTION)
            {
                textInfoContent.charSize -= FONT_SIZE_REDUCTION;
            }
            else
            {
                textInfoContent.charSize = 1;
            }
        }

        contentTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>("CreditContent",
                                                                        textInfoContent,
                                                                        trackingRect);

        trackingRect.top += contentTextUPtr_->GetEntityRegion().height;

        //add space between credits
        trackingRect.top += sfml_util::MapByRes(100.0f, 200.0f);
    }


    void Credit::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        if (titleTextUPtr_.get() != nullptr)
        {
            target.draw( * titleTextUPtr_, states);
        }

        if (contentTextUPtr_.get() != nullptr)
        {
            target.draw( * contentTextUPtr_, states);
        }

        if (animUPtr_.get() != nullptr)
        {
            animUPtr_->draw(target, states);
        }

        if (MediaType::Image == mediaType_)
        {
            target.draw(sprite_, states);
        }
    }


    void Credit::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (animUPtr_.get() != nullptr)
        {
            animUPtr_->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }


    void Credit::Move(const float ADJ_HORIZ, const float ADJ_VERT)
    {
        if (titleTextUPtr_.get() != nullptr)
        {
            titleTextUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        if (contentTextUPtr_.get() != nullptr)
        {
            contentTextUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }

        sprite_.move(ADJ_HORIZ, ADJ_VERT);

        if (animUPtr_.get() != nullptr)
        {
            animUPtr_->MoveEntityPos(ADJ_HORIZ, ADJ_VERT);
        }
    }

}
}