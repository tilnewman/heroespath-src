// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_I_DISPLAYER_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_I_DISPLAYER_HPP_INCLUDED
//
// i-displayer.hpp
//
#include "misc/enum-common.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace heroespath
{
namespace gui
{
    class IEntity;
    using IEntityPtr_t = misc::NotNull<IEntity *>;

    class CachedTexture;
} // namespace gui

namespace test
{

    struct EventFlags
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            PauseKey = 1 << 0,
            OtherInput = 1 << 1
        };
    };

    struct IDisplayer : public sf::Drawable
    {
        virtual ~IDisplayer() = default;

        virtual const std::string name() const = 0;
        virtual bool willEnsureAllImagesSameSize() const = 0;
        virtual const sf::FloatRect windowRegion() const = 0;
        virtual const sf::FloatRect titleRegion() const = 0;
        virtual const sf::FloatRect contentRegion() const = 0;

        virtual const sf::Color backgroundColor() const = 0;
        virtual void backgroundColor(const sf::Color &) = 0;

        virtual void incrememntProgress() = 0;
        virtual void setProgress(const std::size_t) = 0;
        virtual void setProgressMax(const std::size_t) = 0;

        virtual void setup(const sf::FloatRect & FULL_SCREEN_RECT) = 0;
        virtual void teardown() = 0;

        virtual void releaseAndFreeAll() = 0;

        virtual void beginImageSeries(
            const std::string &,
            const std::size_t = 0,
            const bool WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE = false)
            = 0;

        virtual void endImageSeries() = 0;
        virtual void appendImageToSeries(gui::CachedTexture) = 0;

        virtual void beginDrawablesSet(const std::string &) = 0;
        virtual void endDrawablesSet() = 0;
        virtual void appendDrawable(std::unique_ptr<sf::Drawable>) = 0;

        virtual void appendEntity(const gui::IEntityPtr_t) = 0;

        void draw(sf::RenderTarget &, sf::RenderStates = sf::RenderStates()) const override = 0;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_I_DISPLAYER_HPP_INCLUDED
