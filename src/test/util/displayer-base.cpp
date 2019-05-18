// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// displayer-base.cpp
//
#include "displayer-base.hpp"

#include "gui/cached-texture.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "test/util/i-displayer.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <sstream>

namespace heroespath
{
namespace test
{

    void DisplayerBase::appendImageToSeries(gui::CachedTexture) {}

    void DisplayerBase::appendDrawable(std::unique_ptr<sf::Drawable>) {}

    void DisplayerBase::beginCommon(
        const std::string & TITLE_STR, const std::size_t PROGRESS_COUNT_MAX)
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

    void DisplayerBase::endCommon()
    {
        M_HP_LOG(name() + " \"" << m_titleText.getString() << "\" ending the set.");
        m_willEnsureAllImagesAreSameSize = false;
        releaseAndFreeAll();
    }

    // draws the title and progress bar
    void DisplayerBase::drawCommon(sf::RenderTarget & target) const
    {
        target.draw(m_titleText);
        target.draw(m_progressRectangle);
    }

    void DisplayerBase::updateProgressBar()
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

    const std::string DisplayerBase::makeErrorMessagePrefix() const
    {
        std::ostringstream ss;

        ss << name() << ", title=\"" << m_titleText.getString()
           << "\", progress=" << m_progressCounter << " of " << m_progressCountMax << ":  ";

        return ss.str();
    }

    void DisplayerBase::setupTitle() { m_titleText = gui::Text("", gui::GuiFont::Default, 100); }

    void DisplayerBase::setupRegions(const sf::FloatRect & FULL_SCREEN_RECT)
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

} // namespace test
} // namespace heroespath
