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
#include "sfml-util/sfml-util-fitting.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        BoxEntity::BoxEntity(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const BoxEntityInfo & BACKGROUND_INFO)
            : Entity(NAME + "_Background_Constructor1", 0.0f, 0.0f)
            , backgroundInfo_()
            , borderOpt_()
            , coloredRectOpt_()
            , spriteOpt_()
        {
            Setup(REGION, BACKGROUND_INFO);
        }

        void BoxEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (backgroundInfo_.will_draw_color_over_sprite)
            {
                if (spriteOpt_)
                {
                    target.draw(spriteOpt_.value(), states);
                }
            }
            else
            {
                if (coloredRectOpt_)
                {
                    target.draw(coloredRectOpt_.value(), states);
                }
            }

            if (borderOpt_)
            {
                target.draw(borderOpt_.value(), states);
            }

            if (backgroundInfo_.will_draw_color_over_sprite)
            {
                if (coloredRectOpt_)
                {
                    target.draw(coloredRectOpt_.value(), states);
                }
            }
            else
            {
                if (spriteOpt_)
                {
                    target.draw(spriteOpt_.value(), states);
                }
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
            if (coloredRectOpt_)
            {
                coloredRectOpt_->MovePos(HORIZ, VERT);
            }

            if (borderOpt_)
            {
                borderOpt_->MovePos(HORIZ, VERT);
            }

            if (spriteOpt_)
            {
                spriteOpt_->move(HORIZ, VERT);
            }
        }

        void BoxEntity::SetEntityRegion(const sf::FloatRect & REGION_ORIG)
        {
            if (!(REGION_ORIG.width > 0.0f) || !(REGION_ORIG.height > 0.0f))
            {
                borderOpt_ = boost::none;
                coloredRectOpt_ = boost::none;
                spriteOpt_ = boost::none;
                Entity::SetEntityRegion(REGION_ORIG);
                return;
            }

            if (backgroundInfo_.HasBorder())
            {
                if (backgroundInfo_.line_thickness > 0.0f)
                {
                    borderOpt_ = Border(
                        REGION_ORIG,
                        backgroundInfo_.line_thickness,
                        backgroundInfo_.line_color,
                        backgroundInfo_.pad_line_color_adj,
                        backgroundInfo_.will_grow_border);
                }
                else
                {
                    borderOpt_ = Border(REGION_ORIG, backgroundInfo_.will_grow_border);
                }
            }
            else
            {
                borderOpt_ = boost::none;
            }

            const auto OUTER_REGION { ((borderOpt_) ? borderOpt_->OuterRegion() : REGION_ORIG) };

            // See Border.InnerRegion() -this might be the same as outer region...
            const auto INNER_REGION { ((borderOpt_) ? borderOpt_->InnerRegion() : REGION_ORIG) };

            if (backgroundInfo_.HasColor())
            {
                if (backgroundInfo_.HasColorSolid())
                {
                    coloredRectOpt_ = ColoredRect(INNER_REGION, backgroundInfo_.color_from);
                }
                else
                {
                    coloredRectOpt_ = ColoredRect(
                        INNER_REGION,
                        backgroundInfo_.color_from,
                        backgroundInfo_.color_to,
                        backgroundInfo_.color_from_sides,
                        backgroundInfo_.color_from_corners);
                }
            }
            else
            {
                coloredRectOpt_ = boost::none;
            }

            if (backgroundInfo_.HasImage())
            {
                spriteOpt_ = sf::Sprite(backgroundInfo_.cached_texture_opt->Get());

                if (backgroundInfo_.cached_texture_opt->Options().option_enum & ImageOpt::Repeated)
                {
                    spriteOpt_->setPosition(Position(INNER_REGION));
                    SetSize(spriteOpt_.value(), backgroundInfo_.image_size);

                    spriteOpt_->setTextureRect(
                        sf::IntRect(sf::FloatRect(sf::Vector2f(0.0f, 0.0f), Size(INNER_REGION))));
                }
                else
                {
                    if (backgroundInfo_.texture_rect_opt)
                    {
                        spriteOpt_->setTextureRect(backgroundInfo_.texture_rect_opt.value());
                    }

                    if (backgroundInfo_.will_size_instead_of_fit)
                    {
                        SetSizeAndPos(spriteOpt_.value(), INNER_REGION);
                        SetSize(spriteOpt_.value(), backgroundInfo_.image_size);
                    }
                    else
                    {
                        FitAndCenterTo(spriteOpt_.value(), INNER_REGION);
                        FitAndReCenter(spriteOpt_.value(), backgroundInfo_.image_size);
                    }
                }
            }
            else
            {
                spriteOpt_ = boost::none;
            }

            Entity::SetEntityRegion(OUTER_REGION);
        }

        void BoxEntity::OnColorChange()
        {
            if (coloredRectOpt_)
            {
                coloredRectOpt_->Color(GetEntityColorBackground());
            }

            // the border has always been set to the foreground color for some reason...
            if (borderOpt_)
            {
                borderOpt_->Color(GetEntityColorForeground());
            }

            if (spriteOpt_)
            {
                spriteOpt_->setColor(GetEntityColorForeground());
            }
        }

        const sf::FloatRect BoxEntity::InnerRegion() const
        {
            if (borderOpt_)
            {
                return borderOpt_->InnerRegion();
            }
            else if (coloredRectOpt_)
            {
                return coloredRectOpt_->Rect();
            }
            else
            {
                return GetEntityRegion();
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
