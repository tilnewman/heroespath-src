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
#include "gui/i-entity.hpp"
#include "gui/text.hpp"
#include "test/util/i-displayer.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace gui
{
    class CachedTexture;
}
namespace test
{

    struct DisplayerBase : public IDisplayer
    {
        DisplayerBase(const std::string & NAME) { m_name = NAME; }

        virtual ~DisplayerBase() = default;

        const std::string name() const final { return m_name; }
        bool willEnsureAllImagesSameSize() const final { return m_willEnsureAllImagesAreSameSize; }
        const sf::FloatRect windowRegion() const final { return m_windowRegion; }
        const sf::FloatRect titleRegion() const final { return m_titleRegion; }
        const sf::FloatRect contentRegion() const final { return m_contentRegion; }

        const sf::Color backgroundColor() const final { return m_backgroundColor; }
        void backgroundColor(const sf::Color & COLOR) final { m_backgroundColor = COLOR; }

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
            const std::string & TITLE_STR,
            const std::size_t EXPECTED_IMAGE_COUNT = 0,
            const bool WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE = false) override
        {
            m_willEnsureAllImagesAreSameSize = WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE;
            beginCommon(TITLE_STR, EXPECTED_IMAGE_COUNT);
        }

        void endImageSeries() override { endCommon(); }

        void appendImageToSeries(gui::CachedTexture) override;

        void beginDrawablesSet(const std::string & TITLE_STR) override
        {
            beginCommon(TITLE_STR, 0);
        }

        void endDrawablesSet() override { endCommon(); }

        void appendDrawable(std::unique_ptr<sf::Drawable>) override;

        void appendEntity(const gui::IEntityPtr_t ENTITY_PTR) override
        {
            m_entityPtrs.emplace_back(ENTITY_PTR);
        }

    protected:
        void setupCommon(const sf::FloatRect & FULL_SCREEN_RECT)
        {
            setupRegions(FULL_SCREEN_RECT);
            setupTitle();
        }

        void teardownCommon() { releaseAndFreeAll(); }

        void beginCommon(const std::string & TITLE_STR, const std::size_t PROGRESS_COUNT_MAX);
        void endCommon();

        // draws the title, progress bar, and all entitys
        void drawCommon(sf::RenderTarget & target) const;

        void updateProgressBar();

        const std::string makeErrorMessagePrefix() const;

    private:
        void setupTitle();
        void setupRegions(const sf::FloatRect & FULL_SCREEN_RECT);

    private:
        std::size_t m_progressCounter = 0;
        std::size_t m_progressCountMax = 0;
        sf::RectangleShape m_progressRectangle;
        sf::FloatRect m_windowRegion;
        sf::FloatRect m_titleRegion;
        sf::FloatRect m_contentRegion;
        gui::Text m_titleText;
        std::string m_name;
        bool m_willEnsureAllImagesAreSameSize = false;
        sf::Color m_backgroundColor = sf::Color(50, 50, 50);

    protected:
        std::vector<gui::IEntityPtr_t> m_entityPtrs;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_DISPLAYER_BASE_HPP_INCLUDED
