// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_UNIT_TEST_TEST_STUFF_SINGLE_IMAGE_DISPLAYER_INCLUDED
#define HEROESPATH_UNIT_TEST_TEST_STUFF_SINGLE_IMAGE_DISPLAYER_INCLUDED
//
// unit-test-test-stuff-single-image-displayer.hpp
//
#include "gui/cached-texture.hpp"
#include "gui/text.hpp"
#include "gui/texture-cache.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "test/unit-test-test-stuff-i-displayer.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

// this has to be in the global scope or else boost_unit_test complains...whatever.
//
// pretty boost_unit_test wants this to be a struct for some damn reason...whatever
//
struct SingleImageDisplayer : public IDisplayer
{
    virtual ~SingleImageDisplayer() { teardown(); }

    const std::string name() const override { return "SingleImageDisplayer"; }

    void setup(const sf::FloatRect & FULL_SCREEN_RECT) override
    {
        m_windowRegion = FULL_SCREEN_RECT;

        m_titleRegion
            = sf::FloatRect(0.0f, 0.0f, m_windowRegion.width, (m_windowRegion.height * 0.1f));

        m_imageRegion = sf::FloatRect(
            0.0f,
            m_titleRegion.height,
            m_windowRegion.width,
            (m_windowRegion.height - m_titleRegion.height));

        m_text = heroespath::gui::Text("", heroespath::gui::GuiFont::Default, 100);
    }

    void teardown() override { releaseAndFreeAllTextures(); }

    void releaseAndFreeAllTextures() override { m_cachedTextureUPtr.reset(); }

    void beginImageSeries(
        const std::string & TITLE_STR, const std::size_t EXPECTED_IMAGE_COUNT = 0) override
    {
        using namespace heroespath;

        M_HP_LOG(
            name() + " \"" << TITLE_STR << "\" Unit Test starting that expects to display "
                           << EXPECTED_IMAGE_COUNT << " images.");

        m_cachedTextureUPtr.reset();
        m_sprite = sf::Sprite();

        m_progressCounter = 0;
        m_progressCountMax = EXPECTED_IMAGE_COUNT;

        m_progressRectangle.setSize(
            sf::Vector2f(0.0f, std::max(1.0f, (m_windowRegion.height / 600.0f))));

        m_progressRectangle.setPosition(0.0f, m_titleRegion.height);
        setupProgressBar();

        if (TITLE_STR.empty())
        {
            m_text.setString("");
            m_text.setFillColor(sf::Color::Transparent);
        }
        else
        {
            m_text.setString(TITLE_STR);
            m_text.setFillColor(sf::Color::White);
            sfutil::FitAndCenterTo(m_text, m_titleRegion);
        }
    }

    void endImageSeries() override { releaseAndFreeAllTextures(); }

    void appendImageToSeries(heroespath::gui::CachedTexture && cachedTexture) override
    {
        setupTexture(std::move(cachedTexture));
        setupSprite();

        ++m_progressCounter;
        setupProgressBar();
    }

    void draw(sf::RenderTarget & target, sf::RenderStates = sf::RenderStates()) const override
    {
        target.draw(m_sprite);
        target.draw(m_text);
        target.draw(m_progressRectangle);
    }

private:
    const std::string makeErrorMessagePrefix() const
    {
        std::ostringstream ss;

        ss << "SingleImageDisplayer:  For test set lableed \"" << m_text.getString() << "\" on "
           << m_progressCounter << " of " << m_progressCountMax << ":  ";

        return ss.str();
    }

    void setupTexture(heroespath::gui::CachedTexture && cachedTexture)
    {
        m_cachedTextureUPtr = std::make_unique<heroespath::gui::CachedTexture>(cachedTexture);

        if ((m_cachedTextureUPtr->Get().getSize().x == 0)
            || (m_cachedTextureUPtr->Get().getSize().y == 0))
        {
            std::ostringstream ss;
            ss << makeErrorMessagePrefix()
               << "setupTexture(cachedTexture.Path=\"" + cachedTexture.Path()
                    + "\") Created a CachedTexture with an invalid size="
               << m_cachedTextureUPtr->Get().getSize();

            throw std::runtime_error(ss.str());
        }
    }

    void setupSprite()
    {
        if (m_cachedTextureUPtr.get() == nullptr)
        {
            std::ostringstream ss;

            ss << makeErrorMessagePrefix()
               << "setupSprite() called when m_cachedTextureUPtr was nullptr.";

            throw std::runtime_error(ss.str());
        }

        m_sprite.setTexture(m_cachedTextureUPtr->Get(), true);

        const auto SPRITE_INITIAL_GLOBAL_BOUNDS = m_sprite.getGlobalBounds();

        if (!(SPRITE_INITIAL_GLOBAL_BOUNDS.width > 0.0f)
            || !(SPRITE_INITIAL_GLOBAL_BOUNDS.height > 0.0f))
        {
            std::ostringstream ss;

            ss << makeErrorMessagePrefix()
               << "setupSprite() created sprite with invalid bounds=" << m_sprite.getGlobalBounds();

            throw std::runtime_error(ss.str());
        }

        if ((SPRITE_INITIAL_GLOBAL_BOUNDS.width > m_windowRegion.width)
            || (SPRITE_INITIAL_GLOBAL_BOUNDS.height > m_windowRegion.height))
        {
            heroespath::sfutil::FitAndCenterTo(m_sprite, m_imageRegion);
        }
        else
        {
            heroespath::sfutil::CenterTo(m_sprite, m_imageRegion);
        }
    }

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
    std::unique_ptr<heroespath::gui::CachedTexture> m_cachedTextureUPtr;
    sf::Sprite m_sprite;
    std::size_t m_progressCounter = 0;
    std::size_t m_progressCountMax = 0;
    sf::RectangleShape m_progressRectangle;
    sf::FloatRect m_windowRegion;
    sf::FloatRect m_titleRegion;
    sf::FloatRect m_imageRegion;
    heroespath::gui::Text m_text;
};

#endif // HEROESPATH_UNIT_TEST_TEST_STUFF_SINGLE_IMAGE_DISPLAYER_INCLUDED
