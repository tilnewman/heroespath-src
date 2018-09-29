// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// box-entity-info.cpp
//
#include "box-entity.hpp"

#include "log/log-macros.hpp"
#include "sfutil/fitting.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    BoxEntity::BoxEntity(
        const std::string & NAME,
        const sf::FloatRect & REGION,
        const BoxEntityInfo & BACKGROUND_INFO)
        : Entity(NAME + "_Background_Constructor1", 0.0f, 0.0f)
        , backgroundInfo_()
        , border_()
        , coloredRect_()
        , sprite_()
        , willDrawImage_(false)
        , innerRegion_()
    {
        Setup(REGION, BACKGROUND_INFO);
    }

    void BoxEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (backgroundInfo_.will_draw_color_over_sprite)
        {
            if (willDrawImage_)
            {
                target.draw(sprite_, states);
            }

            target.draw(coloredRect_, states);
            target.draw(border_, states);
        }
        else
        {
            target.draw(coloredRect_, states);

            if (willDrawImage_)
            {
                target.draw(sprite_, states);
            }

            target.draw(border_, states);
        }
    }

    void BoxEntity::Setup(const sf::FloatRect & REGION, const BoxEntityInfo & BACKGROUND_INFO)
    {
        backgroundInfo_ = BACKGROUND_INFO;
        SetEntityColors(BACKGROUND_INFO.focus_colors);
        SetEntityRegion(REGION);
    }

    void BoxEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - GetEntityPos() };
        MoveEntityPos(DIFF_V.x, DIFF_V.y);
    }

    void BoxEntity::MoveEntityPos(const float HORIZ, const float VERT)
    {
        coloredRect_.MovePos(HORIZ, VERT);
        border_.MovePos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
        Entity::MoveEntityPos(HORIZ, VERT);
    }

    void BoxEntity::SetEntityRegion(const sf::FloatRect & REGION_ORIG)
    {
        if (sfutil::IsSizeZeroOrLessEither(REGION_ORIG))
        {
            border_.WillDraw(false);
            coloredRect_.WillDraw(false);
            willDrawImage_ = false;

            Entity::SetEntityRegion(REGION_ORIG);
            innerRegion_ = REGION_ORIG;
            return;
        }

        if (backgroundInfo_.HasBorder())
        {
            border_.WillDraw(true);
            if (backgroundInfo_.line_thickness > 0.0f)
            {
                border_.Setup(
                    REGION_ORIG,
                    backgroundInfo_.line_thickness,
                    backgroundInfo_.line_color,
                    backgroundInfo_.pad_line_color_adj,
                    backgroundInfo_.will_grow_border);
            }
            else
            {
                border_.Setup(REGION_ORIG, backgroundInfo_.will_grow_border);
            }

            Entity::SetEntityRegion(border_.OuterRegion());
            innerRegion_ = border_.InnerRegion();
        }
        else
        {
            border_.WillDraw(false);
            Entity::SetEntityRegion(REGION_ORIG);
            innerRegion_ = REGION_ORIG;
        }

        if (backgroundInfo_.HasColor())
        {
            coloredRect_.WillDraw(true);
            if (backgroundInfo_.HasColorSolid())
            {
                coloredRect_.Setup(innerRegion_, backgroundInfo_.color_from);
            }
            else
            {
                coloredRect_.Setup(
                    innerRegion_,
                    backgroundInfo_.color_from,
                    backgroundInfo_.color_to,
                    backgroundInfo_.color_from_sides,
                    backgroundInfo_.color_from_corners);
            }
        }
        else
        {
            coloredRect_.WillDraw(false);
        }

        if (backgroundInfo_.HasImage())
        {
            willDrawImage_ = true;
            sprite_ = sf::Sprite(backgroundInfo_.cached_texture_opt->Get());

            if (backgroundInfo_.cached_texture_opt->Options().option_enum & ImageOpt::Repeated)
            {
                sprite_.setPosition(sfutil::Position(innerRegion_));

                const auto SPRITE_SIZE_BEFORE_V { sfutil::Size(sprite_) };
                sfutil::SetSize(sprite_, backgroundInfo_.image_size);
                const auto SPRITE_SIZE_AFTER_V { sfutil::Size(sprite_) };

                sf::Vector2f textureRectSizeV { sfutil::Size(innerRegion_) };

                if (misc::IsRealZeroOrLess(SPRITE_SIZE_AFTER_V.x) == false)
                {
                    textureRectSizeV.x *= SPRITE_SIZE_BEFORE_V.x / SPRITE_SIZE_AFTER_V.x;
                }

                if (misc::IsRealZeroOrLess(SPRITE_SIZE_AFTER_V.y) == false)
                {
                    textureRectSizeV.y *= SPRITE_SIZE_BEFORE_V.y / SPRITE_SIZE_AFTER_V.y;
                }

                // All these extra steps are required because once the sprite scale is changed
                // (no longer 1/1) calling sprite_.setTextureRect(x,y,w,h) actually sets it to
                // (x,y,w*scale_x, h*scale_y).  So instead we call setTextureRect() with an
                // inverse scaled width and height.

                sprite_.setTextureRect(
                    sf::IntRect(sf::FloatRect(sf::Vector2f(0.0f, 0.0f), textureRectSizeV)));
            }
            else
            {
                if (backgroundInfo_.texture_rect_opt)
                {
                    sprite_.setTextureRect(backgroundInfo_.texture_rect_opt.value());
                }

                if (backgroundInfo_.will_size_instead_of_fit)
                {
                    sfutil::SetSizeAndPos(sprite_, innerRegion_);
                    sfutil::SetSize(sprite_, backgroundInfo_.image_size);
                }
                else
                {
                    sfutil::FitAndCenterTo(sprite_, innerRegion_);
                    sfutil::FitAndReCenter(sprite_, backgroundInfo_.image_size);
                }
            }
        }
        else
        {
            willDrawImage_ = false;
        }
    }

    void BoxEntity::OnColorChange()
    {
        coloredRect_.Color(GetEntityColorBackground());

        // the border has always been set to the foreground color for some reason...
        border_.Color(GetEntityColorForeground());

        sprite_.setColor(GetEntityColorForeground());
    }

} // namespace sfml_util
} // namespace heroespath
