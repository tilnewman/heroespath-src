// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// entity-image-info.cpp
//
#include "entity-image-info.hpp"

#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <string>

namespace heroespath
{
namespace gui
{
    EntityImageInfo::EntityImageInfo(
        const gui::CachedTextureOpt_t & TEXTURE_OPT,
        const FloatRectOpt_t & REGION_OPT,
        const IntRectOpt_t & TEXTURE_REGION_OPT,
        const ColorOpt_t & COLOR_OPT,
        const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
        : cached_texture_opt()
        , sprite()
        , will_resize_instead_of_fit(WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
    {
        Setup(
            TEXTURE_OPT,
            REGION_OPT,
            TEXTURE_REGION_OPT,
            COLOR_OPT,
            WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION);
    }

    EntityImageInfo::EntityImageInfo(
        const gui::CachedTexture & TEXTURE,
        const Vector2fOpt_t & POS_V_OPT,
        const Vector2fOpt_t & SCALE_V_OPT,
        const IntRectOpt_t & TEXTURE_REGION_OPT,
        const ColorOpt_t & COLOR_OPT,
        const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
        : cached_texture_opt()
        , sprite()
        , will_resize_instead_of_fit(WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
    {
        Setup(
            TEXTURE,
            POS_V_OPT,
            SCALE_V_OPT,
            TEXTURE_REGION_OPT,
            COLOR_OPT,
            WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION);
    }

    void EntityImageInfo::Setup(
        const gui::CachedTexture & TEXTURE,
        const Vector2fOpt_t & POS_V_OPT,
        const Vector2fOpt_t & SCALE_V_OPT,
        const IntRectOpt_t & TEXTURE_REGION_OPT,
        const ColorOpt_t & COLOR_OPT,
        const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
    {
        will_resize_instead_of_fit = WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION;

        FloatRectOpt_t rectOpt { boost::none };

        if (POS_V_OPT || SCALE_V_OPT)
        {
            rectOpt = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);

            if (POS_V_OPT)
            {
                rectOpt->left = POS_V_OPT->x;
                rectOpt->top = POS_V_OPT->y;
            }

            if (SCALE_V_OPT)
            {
                sf::Vector2f sizeV { TEXTURE.Get().getSize() };

                if (TEXTURE_REGION_OPT)
                {
                    sizeV = sf::Vector2f { sfutil::Size(*TEXTURE_REGION_OPT) };
                }

                rectOpt->width = SCALE_V_OPT->x * sizeV.x;
                rectOpt->height = SCALE_V_OPT->y * sizeV.y;
            }
        }

        Setup(
            TEXTURE, rectOpt, TEXTURE_REGION_OPT, COLOR_OPT, WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION);
    }

    void EntityImageInfo::Setup(
        const gui::CachedTextureOpt_t & TEXTURE_OPT,
        const FloatRectOpt_t & REGION_OPT,
        const IntRectOpt_t & TEXTURE_REGION_OPT,
        const ColorOpt_t & COLOR_OPT,
        const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
    {
        will_resize_instead_of_fit = WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION;

        if (TEXTURE_OPT)
        {
            cached_texture_opt = TEXTURE_OPT;
            sprite.setTexture(cached_texture_opt->Get(), true);
        }

        if (TEXTURE_REGION_OPT)
        {
            sprite.setTextureRect(TEXTURE_REGION_OPT.value());
        }

        if (REGION_OPT)
        {
            SetRegion(REGION_OPT.value());
        }

        if (COLOR_OPT)
        {
            sprite.setColor(COLOR_OPT.value());
        }
    }

    const std::string EntityImageInfo::ToString(const bool WILL_PREFIX, const Wrap WILL_WRAP) const
    {
        std::ostringstream ss;

        if (cached_texture_opt)
        {
            ss << "path\"" << cached_texture_opt->Path() << "\"";
        }
        else
        {
            ss << "no_image";
        }

        ss << "_region" << sprite.getGlobalBounds() << "_" << sprite.getColor() << "_scale"
           << sprite.getScale() << ", will_";

        if (will_resize_instead_of_fit)
        {
            ss << "resize";
        }
        else
        {
            ss << "fit";
        }

        const auto PARTS_STR { ((WILL_WRAP == Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("EntityImageInfo").append((WILL_WRAP == Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

    void EntityImageInfo::SetRegion(const sf::FloatRect & NEW_GLOBAL_BOUNDS)
    {
        if (will_resize_instead_of_fit)
        {
            sfutil::SetSizeAndPos(sprite, NEW_GLOBAL_BOUNDS);
        }
        else
        {
            sfutil::FitAndCenterTo(sprite, NEW_GLOBAL_BOUNDS);
        }
    }

} // namespace gui
} // namespace heroespath
