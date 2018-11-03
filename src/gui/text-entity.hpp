// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_ENTITY_HPP_INCLUDED
//
// text-entity.hpp
//
#include "gui/entity.hpp"
#include "gui/mouse-text-info.hpp"
#include "gui/text-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace sf
{
class RenderTexture;
}

namespace heroespath
{
namespace gui
{

    using RenderTextureUPtr_t = std::unique_ptr<sf::RenderTexture>;

    // Responsible for being an Entity that has different text for different MouseStates.
    class TextEntity : public Entity
    {
        using CacheMap_t = misc::VectorMap<TextInfo, RenderTextureUPtr_t>;

    public:
        TextEntity(const TextEntity &) = delete;
        TextEntity(TextEntity &&) = delete;
        TextEntity & operator=(const TextEntity &) = delete;
        TextEntity & operator=(TextEntity &&) = delete;

        explicit TextEntity(
            const std::string & NAME,
            const bool WILL_CACHE = true,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX = false);

        TextEntity(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT = 0.0f,
            const bool WILL_CACHE = true,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX = false);

        virtual ~TextEntity();

        void Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT = 0.0f,
            const bool WILL_CACHE = true,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX = false);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        bool WillCache() const { return willCache_; }
        void WillCache(const bool WILL_CACHE) { willCache_ = WILL_CACHE; }

        // if returns true then the mouse state changed and Sync() was called
        bool SetMouseState(const MouseState::Enum) override;

        // does not actually set the position, instead it moves relative to GetEntityPos()
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

        void MoveEntityPos(const float HORIZ, const float VERT) override;

        void SetEntityRegion(const sf::FloatRect & R) override;

        TextInfo & Up() { return mouseTextInfo_.up; }
        const TextInfo & Up() const { return mouseTextInfo_.up; }

        TextInfo & Down() { return mouseTextInfo_.down; }
        const TextInfo & Down() const { return mouseTextInfo_.down; }

        TextInfo & Over() { return mouseTextInfo_.over; }
        const TextInfo & Over() const { return mouseTextInfo_.over; }

        TextInfo & Disabled() { return mouseTextInfo_.disabled; }
        const TextInfo & Disabled() const { return mouseTextInfo_.disabled; }

        const TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE) const
        {
            return mouseTextInfo_.FromMouseState(MOUSE_STATE);
        }

        TextInfo & FromMouseState(const MouseState::Enum MOUSE_STATE)
        {
            return mouseTextInfo_.FromMouseState(MOUSE_STATE);
        }

        TextInfo & FromMouseState() { return FromMouseState(GetMouseState()); }

        const TextInfo & FromMouseState() const { return FromMouseState(GetMouseState()); }

        const std::string GetText() const { return FromMouseState().text; }

        void SetText(const std::string &);

        virtual void Sync();

        const sf::Color OverlayColor() const { return sprite_.getColor(); }
        void OverlayColor(const sf::Color & OVERLAY_COLOR) { sprite_.setColor(OVERLAY_COLOR); }

        bool WillPlayMouseOverTickSfx() const { return willPlayMouseOverTickSfx_; }

        void WillPlayMouseOverTickSfx(const bool WILL_PLAY_SFX)
        {
            willPlayMouseOverTickSfx_ = WILL_PLAY_SFX;
        }

        bool WillDraw() const { return willDraw_; }
        void WillDraw(const bool WILL_DRAW) { willDraw_ = WILL_DRAW; }

    private:
        MouseTextInfo mouseTextInfo_;
        CacheMap_t cacheMap_;
        sf::Sprite sprite_;
        float textWidthLimit_;
        bool willCache_;
        bool willPlayMouseOverTickSfx_;
        bool willDraw_;
    };

    using TextEntityPtr_t = misc::NotNull<TextEntity *>;
    using TextEntityUPtr_t = std::unique_ptr<TextEntity>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_ENTITY_HPP_INCLUDED
