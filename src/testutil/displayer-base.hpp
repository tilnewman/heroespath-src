// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_DISPLAYER_BASE_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_DISPLAYER_BASE_HPP_INCLUDED
//
// displayer-base.hpp
//
#include "gui/text.hpp"
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

    struct DisplayerBase : public IDisplayer
    {
        DisplayerBase(const std::string & NAME) { m_name = NAME; }

        virtual ~DisplayerBase() = default;

        const std::string name() const final { return m_name; }

        const sf::FloatRect windowRegion() const final { return m_windowRegion; }
        const sf::FloatRect titleRegion() const final { return m_titleRegion; }
        const sf::FloatRect contentRegion() const final { return m_contentRegion; }

        void incrememntProgress() final
        {
            ++m_progressCounter;
            updateProgressBar();
        }

        void setProgress(const std::size_t NEW_PROGRESS_COUNT) final
        {
            m_progressCounter = NEW_PROGRESS_COUNT;
            updateProgressBar();
        }

        void setProgressMax(const std::size_t NEW_PROGRESS_MAX) final
        {
            m_progressCountMax = NEW_PROGRESS_MAX;
            updateProgressBar();
        }

        void setup(const sf::FloatRect & FULL_SCREEN_RECT) override
        {
            setupCommon(FULL_SCREEN_RECT);
        }

        void teardown() override { teardownCommon(); }

        void beginImageSeries(
            const std::string & TITLE_STR, const std::size_t EXPECTED_IMAGE_COUNT = 0) override
        {
            beginCommon(TITLE_STR, EXPECTED_IMAGE_COUNT);
        }

        void endImageSeries() override { endCommon(); }

        void appendImageToSeries(gui::CachedTexture) override {}

        void beginDrawablesSet(const std::string & TITLE_STR) override
        {
            beginCommon(TITLE_STR, 0);
        }

        void endDrawablesSet() override { endCommon(); }

        void appendDrawable(std::unique_ptr<sf::Drawable>) override {}

    protected:
        void setupCommon(const sf::FloatRect & FULL_SCREEN_RECT)
        {
            setupRegions(FULL_SCREEN_RECT);
            setupTitle();
        }

        void teardownCommon() { releaseAndFreeAll(); }

        void beginCommon(const std::string & TITLE_STR, const std::size_t PROGRESS_COUNT_MAX)
        {
            M_HP_LOG(name() + " \"" << TITLE_STR << "\" beginning a new set.");

            m_progressRectangle.setSize(
                sf::Vector2f(0.0f, std::max(1.0f, (m_windowRegion.height / 500.0f))));

            m_progressRectangle.setPosition(
                0.0f,
                (m_titleRegion.height + ((m_contentRegion.top - m_titleRegion.height) * 0.5f)
                 + (m_progressRectangle.getSize().y * 0.5f)));

            m_progressCounter = 0;
            m_progressCountMax = PROGRESS_COUNT_MAX;
            updateProgressBar();

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

        void endCommon()
        {
            M_HP_LOG(name() + " \"" << m_titleText.getString() << "\" ending the set.");
            releaseAndFreeAll();
        }

        // draws the title and progress bar
        void drawCommon(sf::RenderTarget & target) const
        {
            target.draw(m_titleText);
            target.draw(m_progressRectangle);
        }

        void updateProgressBar()
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

        const std::string makeErrorMessagePrefix() const
        {
            std::ostringstream ss;

            ss << name() << ", title=\"" << m_titleText.getString()
               << "\", progress=" << m_progressCounter << " of " << m_progressCountMax << ":  ";

            return ss.str();
        }

    private:
        void setupTitle() { m_titleText = gui::Text("", gui::GuiFont::Default, 100); }

        void setupRegions(const sf::FloatRect & FULL_SCREEN_RECT)
        {
            m_windowRegion = FULL_SCREEN_RECT;

            const auto TITLE_HEIGHT_RATIO = 0.08f;
            const auto BETWEEN_TITLE_AND_CONTENT_RATIO = 0.03f;

            const auto CONTENT_HEIGHT_RATIO
                = ((1.0f - TITLE_HEIGHT_RATIO) - BETWEEN_TITLE_AND_CONTENT_RATIO);

            m_titleRegion = sf::FloatRect(
                0.0f, 0.0f, m_windowRegion.width, (m_windowRegion.height * TITLE_HEIGHT_RATIO));

            m_contentRegion = sf::FloatRect(
                0.0f,
                (m_windowRegion.height - (m_windowRegion.height * CONTENT_HEIGHT_RATIO)),
                m_windowRegion.width,
                (m_windowRegion.height * CONTENT_HEIGHT_RATIO));
        }

    private:
        std::size_t m_progressCounter = 0;
        std::size_t m_progressCountMax = 0;
        sf::RectangleShape m_progressRectangle;
        sf::FloatRect m_windowRegion;
        sf::FloatRect m_titleRegion;
        sf::FloatRect m_contentRegion;
        gui::Text m_titleText;
        std::string m_name;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_DISPLAYER_BASE_HPP_INCLUDED
