// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-entity.cpp
//
#include "image-entity.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-position.hpp"

namespace heroespath
{
namespace sfml_util
{

    ImageEntity::ImageEntity(const std::string & NAME, const MouseImageInfo & MOUSE_IMAGE_INFO)
        : Entity(
              std::string(NAME).append("_ImageEntity(MouseImageInfo-constructor)_"),
              ((MOUSE_IMAGE_INFO.up.HasTexture()) ? MOUSE_IMAGE_INFO.up.sprite.getGlobalBounds()
                                                  : sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f)))
        , mouseImageInfo_(MOUSE_IMAGE_INFO)
        , sprite_()
        , willDraw_(false)
    {
        Sync();
    }

    ImageEntity::ImageEntity(
        const std::string & NAME,
        const FloatRectOpt_t & REGION_OPT,
        const sfml_util::CachedTextureOpt_t & TEXTURE_UP_OPT,
        const sfml_util::CachedTextureOpt_t & TEXTURE_DOWN_OPT,
        const sfml_util::CachedTextureOpt_t & TEXTURE_OVER_OPT,
        const sfml_util::CachedTextureOpt_t & TEXTURE_DISABLED_OPT,
        const bool WILL_DRAW_UP_IF_MISSING,
        const ColorOpt_t & COLOR_OPT,
        const bool WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION)
        : Entity(
              std::string(NAME).append("_ImageEntity(cached-texture-constructor)_"),
              ((REGION_OPT) ? REGION_OPT.value() : sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f)))
        , mouseImageInfo_(
              WILL_DRAW_UP_IF_MISSING,
              EntityImageInfo(
                  TEXTURE_UP_OPT,
                  REGION_OPT,
                  boost::none,
                  COLOR_OPT,
                  WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION),
              EntityImageInfo(
                  TEXTURE_DOWN_OPT,
                  REGION_OPT,
                  boost::none,
                  COLOR_OPT,
                  WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION),
              EntityImageInfo(
                  TEXTURE_OVER_OPT,
                  REGION_OPT,
                  boost::none,
                  COLOR_OPT,
                  WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION),
              EntityImageInfo(
                  TEXTURE_DISABLED_OPT,
                  REGION_OPT,
                  boost::none,
                  COLOR_OPT,
                  WILL_RESIZE_INSTEAD_OF_FIT_TO_REGION))
        , sprite_()
        , willDraw_(false)
    {
        Sync();
    }

    ImageEntity::~ImageEntity() = default;

    void ImageEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (willDraw_)
        {
            target.draw(sprite_, states);
        }
    }

    void ImageEntity::Color(const sf::Color & COLOR)
    {
        mouseImageInfo_.up.sprite.setColor(COLOR);
        mouseImageInfo_.down.sprite.setColor(COLOR);
        mouseImageInfo_.over.sprite.setColor(COLOR);
        mouseImageInfo_.disabled.sprite.setColor(COLOR);
        Sync();
    }

    void ImageEntity::Color(
        const ColorOpt_t & COLOR_UP_OPT,
        const ColorOpt_t & COLOR_DOWN_OPT,
        const ColorOpt_t & COLOR_OVER_OPT,
        const ColorOpt_t & COLOR_DISBALED_OPT)
    {
        if (COLOR_UP_OPT)
        {
            mouseImageInfo_.up.sprite.setColor(COLOR_UP_OPT.value());
        }

        if (COLOR_DOWN_OPT)
        {
            mouseImageInfo_.down.sprite.setColor(COLOR_UP_OPT.value());
        }

        if (COLOR_OVER_OPT)
        {
            mouseImageInfo_.over.sprite.setColor(COLOR_UP_OPT.value());
        }

        if (COLOR_DISBALED_OPT)
        {
            mouseImageInfo_.disabled.sprite.setColor(COLOR_DISBALED_OPT.value());
        }

        Sync();
    }

    void ImageEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
    }

    void ImageEntity::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);

        mouseImageInfo_.up.sprite.move(HORIZ, VERT);
        mouseImageInfo_.down.sprite.move(HORIZ, VERT);
        mouseImageInfo_.over.sprite.move(HORIZ, VERT);
        mouseImageInfo_.disabled.sprite.move(HORIZ, VERT);

        Sync();
    }

    void ImageEntity::SetEntityRegion(const sf::FloatRect & R)
    {
        mouseImageInfo_.SetRegion(R);
        Sync(); // Sync() will set entityRegion_
    }

    bool ImageEntity::SetMouseState(const MouseState::Enum NEW_MOUSE_STATE)
    {
        if (Entity::SetMouseState(NEW_MOUSE_STATE))
        {
            Sync();
            return true;
        }

        return false;
    }

    void ImageEntity::Sync()
    {
        const auto MOUSE_STATE_TO_DRAW(mouseImageInfo_.ToDraw(GetMouseState()));

        if (MOUSE_STATE_TO_DRAW == MouseState::Count)
        {
            willDraw_ = false;

            entityRegion_
                = sf::FloatRect(sfml_util::Position(GetEntityRegion()), sf::Vector2f(0.0f, 0.0f));
        }
        else
        {
            willDraw_ = true;
            sprite_ = mouseImageInfo_.FromMouseState(MOUSE_STATE_TO_DRAW).sprite;
            entityRegion_ = sprite_.getGlobalBounds();
        }
    }

} // namespace sfml_util
} // namespace heroespath
