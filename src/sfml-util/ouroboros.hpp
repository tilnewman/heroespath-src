// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_OUROBOROS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_OUROBOROS_HPP_INCLUDED
//
// ouroboros.hpp
//  An animated snake eating its own tail for the background of main menu screens.
//
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/entity.hpp"
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{

    // manages an animated snake image that spins, grows/shrinks, and fades in/out
    class Ouroboros : public gui::Entity
    {
    public:
        Ouroboros(const Ouroboros &) = delete;
        Ouroboros(Ouroboros &&) = delete;
        Ouroboros & operator=(const Ouroboros &) = delete;
        Ouroboros & operator=(Ouroboros &&) = delete;

        explicit Ouroboros(const std::string & NAME, const bool WILL_INVERT = false);
        virtual ~Ouroboros();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual bool UpdateTime(const float ELAPSED_TIME_SEC);

    private:
        const float IMAGE_INITIAL_WIDTH_;
        const float IMAGE_DRIFT_WIDTH_;
        const float IMAGE_MIN_DRIFT_WIDTH_;
        const float IMAGE_MAX_DRIFT_WIDTH_;
        float rotation_;
        sfml_util::CachedTexture cachedTexture_;
        sf::Sprite sprite_;
        sliders::Drifter<float> sizeDrifter_;
        sliders::Drifter<float> shadeDrifter_;
        sliders::Drifter<float> rotSpeedDrifter_;
    };

    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_OUROBOROS_HPP_INCLUDED
