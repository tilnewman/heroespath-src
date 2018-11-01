// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTANIMATION_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTANIMATION_HPP_INCLUDED
//
// text-animation.hpp
//
#include "gui/sliders.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    namespace animation
    {

        // Responsible for drawing emitted sparks
        class TextAnimation : public sf::Drawable
        {
            TextAnimation(const TextAnimation &);
            TextAnimation & operator=(const TextAnimation &);

        public:
            explicit TextAnimation(
                const std::string & TEXT,
                const sf::FloatRect & REGION,
                const float SPEED,
                const unsigned int START_FONT_SIZE,
                const unsigned int END_FONT_SIZE,
                const sf::Color & START_COLOR,
                const sf::Color & END_COLOR);

            bool IsFinished() const { return isFinished_; }
            void Update(const float ELAPSED_TIME_SEC);
            void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        private:
            const std::string TEXT_;
            const unsigned int START_FONT_SIZE_;
            const unsigned int END_FONT_SIZE_;
            const sf::Color START_COLOR_;
            const sf::Color END_COLOR_;
            sf::Vector2f startPosV_;
            sf::Vector2f endPosV_;
            bool isFinished_;
            TextRegionUPtr_t textRegionUPtr_;
            SliderZeroToOne slider_;
        };

        using TextAnimationUPtr_t = std::unique_ptr<TextAnimation>;
        using TextAnimationUVec_t = std::vector<TextAnimationUPtr_t>;

    } // namespace animation
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTANIMATION_HPP_INCLUDED
