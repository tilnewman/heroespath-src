// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef TEST_TESTSTUFF_INCLUDED
#define TEST_TESTSTUFF_INCLUDED
//
// Test-stuff.hpp
//  Helpful testing data and functions.
//
#include <array>
#include <cstddef> //for std::size_t
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "game/startup-shutdown.hpp"
#include "gui/cached-texture.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/text.hpp"
#include "gui/texture-cache.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics.hpp>

namespace heroespath
{
namespace test
{
    constexpr std::size_t randomRetryCount = 1000;

    constexpr std::array<int, 24> smallValuesWorthTestingInt
        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 16, 17, 19, 20, 21, 29, 30, 31, 32, 33 };

    constexpr std::size_t smallValuesWorthTestingCount = smallValuesWorthTestingInt.size();

    template <typename T>
    constexpr std::array<T, smallValuesWorthTestingCount> smallValuesWorthTesting()
    {
        std::array<T, smallValuesWorthTestingCount> temp;

        if constexpr (std::is_same_v<T, int>)
        {
            temp = smallValuesWorthTestingInt;
        }
        else
        {
            for (std::size_t i(0); i < temp.size(); ++i)
            {
                temp[i] = static_cast<T>(smallValuesWorthTestingInt[i]);
            }
        }

        return temp;
    }

    template <typename T>
    const std::string containerToString(const T & CONTAINER)
    {
        std::ostringstream ss;

        ss << "[";

        auto iter = std::cbegin(CONTAINER);
        while (iter != std::cend(CONTAINER))
        {
            if (iter != std::cbegin(CONTAINER))
            {
                ss << ", ";
            }

            ss << *iter;

            ++iter;
        }

        ss << "]";

        return ss.str();
    }

    template <typename T>
    bool isUnique(const T & CONTAINER)
    {
        if (CONTAINER.size() <= 1)
        {
            return true;
        }

        std::set<typename T::value_type> set;

        auto iter = std::cbegin(CONTAINER);
        while (iter != std::cend(CONTAINER))
        {
            const auto & VALUE = *iter++;

            if (set.find(VALUE) == std::end(set))
            {
                set.insert(VALUE);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    template <typename T>
    bool isSortedDescending(const T & CONTAINER)
    {
        auto const SIZE { CONTAINER.size() };

        if (SIZE <= 1)
        {
            return true;
        }

        for (std::size_t i(1); i < SIZE; ++i)
        {
            if (CONTAINER[i - 1] > CONTAINER[i])
            {
                return false;
            }
        }

        return true;
    }

    struct SingleImageDisplayer : public sf::Drawable
    {
        SingleImageDisplayer()
            : m_cachedTextureUPtr()
            , m_sprite()
            , m_progressCounter(0)
            , m_progressCountMax(0)
            , m_progressRectangle()
            , m_windowRegion()
            , m_titleRegion()
            , m_imageRegion()
            , m_text()
        {}

        void setup(const sf::FloatRect & FULL_SCREEN_RECT)
        {
            m_windowRegion = FULL_SCREEN_RECT;

            m_titleRegion
                = sf::FloatRect(0.0f, 0.0f, m_windowRegion.width, (m_windowRegion.height * 0.1f));

            m_imageRegion = sf::FloatRect(
                0.0f,
                m_titleRegion.height,
                m_windowRegion.width,
                (m_windowRegion.height - m_titleRegion.height));

            m_text = gui::Text("", gui::GuiFont::Default, 100);
        }

        void teardown() { freeAndReleaseCachedTexture(); }

        void freeAndReleaseCachedTexture() { m_cachedTextureUPtr.reset(); }

        void
            startUnitTestSeries(const std::string & TITLE_STR, const std::size_t PROGRESS_COUNT_MAX)
        {
            M_HP_LOG(
                "SingleImageDisplayer Unit Test starting \""
                << TITLE_STR << "\" that expects to display " << PROGRESS_COUNT_MAX << " images.");

            m_cachedTextureUPtr.reset();
            m_sprite = sf::Sprite();

            m_progressCounter = 0;
            m_progressCountMax = PROGRESS_COUNT_MAX;

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

        void setupNextImage(gui::CachedTexture && cachedTexture)
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

        void setupTexture(gui::CachedTexture && cachedTexture)
        {
            m_cachedTextureUPtr = std::make_unique<gui::CachedTexture>(cachedTexture);

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
                   << "setupSprite() created sprite with invalid bounds="
                   << m_sprite.getGlobalBounds();

                throw std::runtime_error(ss.str());
            }

            if ((SPRITE_INITIAL_GLOBAL_BOUNDS.width > m_windowRegion.width)
                || (SPRITE_INITIAL_GLOBAL_BOUNDS.height > m_windowRegion.height))
            {
                sfutil::FitAndCenterTo(m_sprite, m_imageRegion);
            }
            else
            {
                sfutil::CenterTo(m_sprite, m_imageRegion);
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
        std::unique_ptr<gui::CachedTexture> m_cachedTextureUPtr;
        sf::Sprite m_sprite;
        std::size_t m_progressCounter;
        std::size_t m_progressCountMax;
        sf::RectangleShape m_progressRectangle;
        sf::FloatRect m_windowRegion;
        sf::FloatRect m_titleRegion;
        sf::FloatRect m_imageRegion;
        gui::Text m_text;
    };

} // namespace test
} // namespace heroespath

struct GameEngineGlobalFixture
{
    void setup()
    {
        startupShutdownUPtr = std::make_unique<heroespath::game::StartupShutdown>(
            "HP Test Image Loading", 0, nullptr, true);

        auto & window = *heroespath::gui::Display::Instance();

        window.PollEvents();

        if (!window.IsOpen())
        {
            throw std::runtime_error(
                "GameEngineGlobalFixture::setup() failed to open a display window.");
        }

        heroespath::gui::FontManager::Instance()->Load(heroespath::gui::GuiFont::Default);
        displayer.setup(window.FullScreenRect());
        window.PollEvents();
    }

    void teardown()
    {
        heroespath::gui::Display::Instance()->PollEvents();
        displayer.teardown();
        startupShutdownUPtr.reset();
    }

    static void
        startUnitTestSeries(const std::string & TITLE_STR, const std::size_t PROGRESS_COUNT_MAX)
    {
        heroespath::gui::Display::Instance()->PollEvents();
        displayer.freeAndReleaseCachedTexture();
        heroespath::gui::TextureCache::Instance()->RemoveAll();
        displayer.startUnitTestSeries(TITLE_STR, PROGRESS_COUNT_MAX);
        draw();
    }

    static void displayNextImage(heroespath::gui::CachedTexture && cachedTexture)
    {
        displayer.setupNextImage(std::move(cachedTexture));
        draw();
    }

private:
    static void draw()
    {
        auto & window = *heroespath::gui::Display::Instance();
        window.ClearToBlack();
        window.TestDraw(displayer);
        window.DisplayFrameBuffer();
        // sf::sleep(sf::seconds(0.01f));
    }

    static std::unique_ptr<heroespath::game::StartupShutdown> startupShutdownUPtr;
    static heroespath::test::SingleImageDisplayer displayer;
};

std::unique_ptr<heroespath::game::StartupShutdown> GameEngineGlobalFixture::startupShutdownUPtr;
heroespath::test::SingleImageDisplayer GameEngineGlobalFixture::displayer;

#endif // TEST_TESTSTUFF_INCLUDED
