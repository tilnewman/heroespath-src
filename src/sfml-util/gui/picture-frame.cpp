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
// picture-frame.hpp
//
#include "picture-frame.hpp"
#include "game/game-data-file.hpp"
#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        PictureFrame::PictureFrame(const sf::FloatRect & RECT, const sf::Color & COLOR)
            : color_(COLOR)
            , outerRect_(RECT)
            , innerRect_()
            , texture_()
            , topLeftSprite_()
            , topRightSprite_()
            , botLeftSprite_()
            , botRightSprite_()
            , sideSprites_()
        {
            sfml_util::LoadTexture(
                texture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-misc-picture-frame"));

            topLeftSprite_.setTexture(texture_);
            topRightSprite_.setTexture(texture_);
            botLeftSprite_.setTexture(texture_);
            botRightSprite_.setTexture(texture_);

            topLeftSprite_.setTextureRect(sf::IntRect(0, 0, 94, 124));
            topRightSprite_.setTextureRect(sf::IntRect(96, 0, 94, 108));
            botLeftSprite_.setTextureRect(sf::IntRect(0, 126, 100, 115));
            botRightSprite_.setTextureRect(sf::IntRect(102, 132, 94, 109));

            if (RECT.width > 0.0f)
            {
                Setup(RECT, COLOR);
            }
        }

        void PictureFrame::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            for (auto const & SPRITE : sideSprites_)
                target.draw(SPRITE, states);

            target.draw(topLeftSprite_, states);
            target.draw(topRightSprite_, states);
            target.draw(botLeftSprite_, states);
            target.draw(botRightSprite_, states);
        }

        const sf::FloatRect PictureFrame::Setup(const sf::FloatRect & RECT, const sf::Color & COLOR)
        {
            color_ = COLOR;

            topLeftSprite_.setColor(color_);
            topRightSprite_.setColor(color_);
            botLeftSprite_.setColor(color_);
            botRightSprite_.setColor(color_);

            outerRect_ = RECT;

            auto const LEFT_INNER_EDGE_WIDTH{ 27.0f };
            innerRect_.left = outerRect_.left + LEFT_INNER_EDGE_WIDTH;

            auto const TOP_INNER_EDGE_HEIGHT{ 31.0f };
            innerRect_.top = outerRect_.top + TOP_INNER_EDGE_HEIGHT;

            auto const RIGHT_OUTER_EDGE_WIDTH{ 28.0f };
            innerRect_.width = (outerRect_.width - LEFT_INNER_EDGE_WIDTH) - RIGHT_OUTER_EDGE_WIDTH;

            auto const BOT_OUTER_EDGE_HEIGHT{ 31.0f };
            innerRect_.height = (outerRect_.height - TOP_INNER_EDGE_HEIGHT) - BOT_OUTER_EDGE_HEIGHT;

            sideSprites_.clear();

            topLeftSprite_.setPosition(outerRect_.left, outerRect_.top);

            auto const TOP_RIGHT_POS_LEFT{ ((outerRect_.left + outerRect_.width)
                                            - topRightSprite_.getGlobalBounds().width)
                                           + 3.0f };

            topRightSprite_.setPosition(TOP_RIGHT_POS_LEFT, outerRect_.top);

            auto const BOT_LEFT_POS_TOP{ (outerRect_.top + outerRect_.height)
                                         - botLeftSprite_.getGlobalBounds().height };

            botLeftSprite_.setPosition(outerRect_.left, BOT_LEFT_POS_TOP);

            auto const BOT_RIGHT_POS_LEFT{ (outerRect_.left + outerRect_.width)
                                           - botRightSprite_.getGlobalBounds().width };

            auto const BOT_RIGHT_POS_TOP{ (outerRect_.top + outerRect_.height)
                                          - botRightSprite_.getGlobalBounds().height };

            botRightSprite_.setPosition(BOT_RIGHT_POS_LEFT, BOT_RIGHT_POS_TOP);

            {
                const sf::Sprite TOP_SPRITE(texture_, sf::IntRect(228, 133, 28, 23));

                auto const TOP_GAP_HORIZ{ (outerRect_.width
                                           - topLeftSprite_.getGlobalBounds().width)
                                          - topRightSprite_.getGlobalBounds().width };

                auto const TOP_HORIZ_FILL_COUNT{
                    static_cast<int>(TOP_GAP_HORIZ / TOP_SPRITE.getGlobalBounds().width) + 1
                };

                auto const OUTER_EDGE{ 8.0f };

                auto const POS_LEFT{ topLeftSprite_.getGlobalBounds().left
                                     + topLeftSprite_.getGlobalBounds().width };

                auto const POS_TOP{ outerRect_.top + OUTER_EDGE };

                for (int i(0); i < TOP_HORIZ_FILL_COUNT; ++i)
                {
                    sf::Sprite sprite{ TOP_SPRITE };
                    auto const OFFSET{ static_cast<float>(i) * TOP_SPRITE.getGlobalBounds().width };
                    sprite.setPosition(POS_LEFT + OFFSET, POS_TOP);
                    sprite.setColor(color_);
                    sideSprites_.push_back(sprite);
                }
            }

            {
                const sf::Sprite BOT_SPRITE(texture_, sf::IntRect(230, 233, 26, 23));

                auto const BOT_GAP_HORIZ{ (outerRect_.width
                                           - botLeftSprite_.getGlobalBounds().width)
                                          - botRightSprite_.getGlobalBounds().width };

                auto const BOT_HORIZ_FILL_COUNT{
                    static_cast<int>(BOT_GAP_HORIZ / BOT_SPRITE.getGlobalBounds().width) + 1
                };

                auto const OUTER_EDGE{ 33.0f };

                auto const POS_LEFT{ botLeftSprite_.getGlobalBounds().left
                                     + botLeftSprite_.getGlobalBounds().width };

                auto const POS_TOP{ (outerRect_.top + outerRect_.height) - OUTER_EDGE };

                for (int i(0); i < BOT_HORIZ_FILL_COUNT; ++i)
                {
                    sf::Sprite sprite{ BOT_SPRITE };
                    auto const OFFSET{ static_cast<float>(i) * BOT_SPRITE.getGlobalBounds().width };
                    sprite.setPosition(POS_LEFT + OFFSET, POS_TOP);
                    sprite.setColor(color_);
                    sideSprites_.push_back(sprite);
                }
            }

            {
                const sf::Sprite LEFT_SPRITE(texture_, sf::IntRect(236, 158, 20, 40));

                auto const LEFT_GAP_HORIZ{ (outerRect_.height
                                            - topLeftSprite_.getGlobalBounds().height)
                                           - botLeftSprite_.getGlobalBounds().height };

                auto const LEFT_HORIZ_FILL_COUNT{
                    static_cast<int>(LEFT_GAP_HORIZ / LEFT_SPRITE.getGlobalBounds().height) + 1
                };

                auto const OUTER_EDGE{ 7.0f };
                auto const POS_LEFT{ outerRect_.left + OUTER_EDGE };

                auto const POS_TOP{ topLeftSprite_.getGlobalBounds().top
                                    + topLeftSprite_.getGlobalBounds().height };

                for (int i(0); i < LEFT_HORIZ_FILL_COUNT; ++i)
                {
                    sf::Sprite sprite{ LEFT_SPRITE };
                    auto const OFFSET{ static_cast<float>(i)
                                       * LEFT_SPRITE.getGlobalBounds().height };
                    sprite.setPosition(POS_LEFT, POS_TOP + OFFSET);
                    sprite.setColor(color_);
                    sideSprites_.push_back(sprite);
                }
            }

            {
                const sf::Sprite RIGHT_SPRITE(texture_, sf::IntRect(234, 200, 22, 31));

                auto const RIGHT_GAP_HORIZ{ (outerRect_.height
                                             - topRightSprite_.getGlobalBounds().height)
                                            - botRightSprite_.getGlobalBounds().height };

                auto const RIGHT_HORIZ_FILL_COUNT{
                    static_cast<int>(RIGHT_GAP_HORIZ / RIGHT_SPRITE.getGlobalBounds().height) + 1
                };

                auto const OUTER_EDGE{ 29.0f };
                auto const POS_LEFT{ (outerRect_.left + outerRect_.width) - OUTER_EDGE };

                auto const POS_TOP{ topRightSprite_.getGlobalBounds().top
                                    + topRightSprite_.getGlobalBounds().height };

                for (int i(0); i < RIGHT_HORIZ_FILL_COUNT; ++i)
                {
                    sf::Sprite sprite{ RIGHT_SPRITE };

                    auto const OFFSET{ static_cast<float>(i)
                                       * (RIGHT_SPRITE.getGlobalBounds().height) };

                    sprite.setPosition(POS_LEFT, POS_TOP + OFFSET);
                    sprite.setColor(color_);
                    sideSprites_.push_back(sprite);
                }
            }

            return innerRect_;
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
