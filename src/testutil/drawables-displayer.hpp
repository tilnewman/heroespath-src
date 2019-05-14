// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
//
// drawables-displayer.hpp
//
#include "gui/cached-texture.hpp"
#include "gui/text.hpp"
#include "gui/texture-cache.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "testutil/i-displayer.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace heroespath
{
namespace test
{

    struct DrawablesDisplayer : public IDisplayer
    {
        virtual ~DrawablesDisplayer() { teardown(); }

        const std::string name() const override { return "DrawablesDisplayer"; }

        const sf::FloatRect contentRegion() const override { return m_contentRegion; }

        virtual void incrememntProgress() override
        {
            ++m_progressCounter;
            setupProgressBar();
        }

        virtual void setProgress(const std::size_t NEW_PROGRESS_COUNT) override
        {
            m_progressCounter = NEW_PROGRESS_COUNT;
            setupProgressBar();
        }

        void setup(const sf::FloatRect & FULL_SCREEN_RECT) override
        {
            m_windowRegion = FULL_SCREEN_RECT;

            m_titleRegion
                = sf::FloatRect(0.0f, 0.0f, m_windowRegion.width, (m_windowRegion.height * 0.1f));

            m_contentRegion = sf::FloatRect(
                0.0f,
                m_titleRegion.height,
                m_windowRegion.width,
                (m_windowRegion.height - m_titleRegion.height));

            m_titleText = heroespath::gui::Text("", heroespath::gui::GuiFont::Default, 100);
        }

        void teardown() override { releaseAndFreeAll(); }

        void releaseAndFreeAll() override { m_drawables.clear(); }

        void beginDrawablesSet(const std::string & TITLE_STR) override
        {
            using namespace heroespath;

            M_HP_LOG(name() + " \"" << TITLE_STR << "\" Unit Test starting prep before display.");

            m_progressCounter = 0;
            m_progressCountMax = 100;

            m_progressRectangle.setSize(
                sf::Vector2f(0.0f, std::max(1.0f, (m_windowRegion.height / 600.0f))));

            m_progressRectangle.setPosition(0.0f, m_titleRegion.height);
            setupProgressBar();

            if (TITLE_STR.empty())
            {
                m_titleText.setString("");
                m_titleText.setFillColor(sf::Color::Transparent);
            }
            else
            {
                m_titleText.setString(TITLE_STR);
                m_titleText.setFillColor(sf::Color::White);
                sfutil::FitAndCenterTo(m_titleText, m_titleRegion);
            }
        }

        void endDrawablesSet() override { releaseAndFreeAll(); }

        void appendDrawable(std::unique_ptr<sf::Drawable> && drawableUPtr) override
        {
            m_drawables.emplace_back(std::move(drawableUPtr));
        }

        void draw(sf::RenderTarget & target, sf::RenderStates = sf::RenderStates()) const override
        {
            for (const auto & DRAWABLE_UPTR : m_drawables)
            {
                target.draw(*DRAWABLE_UPTR);
            }

            target.draw(m_titleText);
            target.draw(m_progressRectangle);
        }

    private:
        void setupProgressBar()
        {
            if (m_progressCountMax > 0)
            {
                m_progressRectangle.setFillColor(sf::Color::White);

                const float ratio(
                    static_cast<float>(m_progressCounter) / static_cast<float>(m_progressCountMax));

                m_progressRectangle.setSize(
                    sf::Vector2f((m_windowRegion.width * ratio), m_progressRectangle.getSize().y));
            }
            else
            {
                m_progressRectangle.setFillColor(sf::Color::Transparent);
            }
        }

    private:
        std::vector<std::unique_ptr<sf::Drawable>> m_drawables;
        std::size_t m_progressCounter = 0;
        std::size_t m_progressCountMax = 0;
        sf::RectangleShape m_progressRectangle;
        sf::FloatRect m_windowRegion;
        sf::FloatRect m_titleRegion;
        sf::FloatRect m_contentRegion;
        heroespath::gui::Text m_titleText;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
