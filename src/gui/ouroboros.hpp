// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_OUROBOROS_HPP_INCLUDED
#define HEROESPATH_GUI_OUROBOROS_HPP_INCLUDED
//
// ouroboros.hpp
//  An animated snake eating its own tail for the background of main menu screens.
//
#include "gui/cached-texture.hpp"
#include "gui/entity.hpp"
#include "gui/sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace gui
{

    // manages an animated snake image that spins, grows/shrinks, and fades in/out
    class Ouroboros : public Entity
    {
    public:
        explicit Ouroboros(const std::string & NAME, const bool WILL_MAKE_BLACK = false);
        virtual ~Ouroboros();

        Ouroboros(const Ouroboros &) = delete;
        Ouroboros(Ouroboros &&) = delete;
        Ouroboros & operator=(const Ouroboros &) = delete;
        Ouroboros & operator=(Ouroboros &&) = delete;

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
        bool UpdateTime(const float ELAPSED_TIME_SEC) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

    private:
        const float IMAGE_DIMMENSION_INITIAL_;
        const float IMAGE_DIMMENSION_MIN_;
        const float IMAGE_DIMMENSION_MAX_;
        gui::CachedTexture cachedTexture_;
        sf::Sprite sprite_;
        SliderDrift<float> sizeDrifter_;
        SliderDrift<float> alphaDrifter_;
        SliderDrift<float> rotationSpeedDrifter_;
    };

    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_OUROBOROS_HPP_INCLUDED
