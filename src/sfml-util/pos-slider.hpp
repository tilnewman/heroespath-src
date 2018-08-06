// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
//
//  pos-slider.hpp
//
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/sliders.hpp"

#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace sliders
    {

        // Presents a simple interface for controlling a position that starts at a given FROM and
        // then moves toward a TO position.  Calling Update() periodically with the time elapsed
        // advances the position.  ChangeDirection() does not change the current position, it sets
        // FROM to the current position and sets TO to either the original FROM or TO.
        class PosSlider
        {
        public:
            PosSlider();

            explicit PosSlider(
                const sf::Vector2f & FROM_POS_V,
                const sf::Vector2f & TO_POS_V,
                const float SLIDER_SPEED);

            virtual ~PosSlider() = default;

            void Setup(
                const sf::Vector2f & FROM_POS_V,
                const sf::Vector2f & TO_POS_V,
                const float SLIDER_SPEED);

            void ChangeDirection();

            // returns true if the position was changed
            virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

            const sf::Vector2f Position() const;

            float ProgressRatio() const { return slider_.Current(); }
            sfml_util::Moving::Enum Direction() const { return direction_; }
            const sf::Vector2f To() const { return toPosV_; }
            const sf::Vector2f From() const { return fromPosV_; }
            float Speed() const { return slider_.Speed(); }
            bool IsMoving() const { return isMoving_; }
            void Start() { isMoving_ = true; }
            void Stop() { isMoving_ = false; }

            void Speed(const float S) { slider_.Reset(S, slider_.Current()); }

        protected:
            sf::Vector2f origFromPosV_;
            sf::Vector2f origToPosV_;
            sf::Vector2f fromPosV_;
            sf::Vector2f toPosV_;
            sfml_util::Moving::Enum direction_;
            bool isMoving_;
            sfml_util::sliders::ZeroSliderOnce<float> slider_;
        };

    } // namespace sliders
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_POS_SLIDER_HPP_INCLUDED
