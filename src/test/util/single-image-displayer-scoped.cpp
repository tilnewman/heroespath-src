// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// single-image-displayer-scoped.cpp
//
#include "single-image-displayer-scoped.hpp"

#include "gui/cached-texture.hpp"
#include "test/util/game-engine-global-fixture.hpp"
#include "test/util/single-image-displayer.hpp"

#include <boost/test/unit_test.hpp>

namespace heroespath
{
namespace test
{

    SingleImageDisplayerScoped::SingleImageDisplayerScoped(
        const std::string & NAME_OF_IMAGE_SET,
        const std::size_t EXPECTED_IMAGE_COUNT,
        const bool WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE,
        const float DELAY_AFTER_EACH_DRAW)
        : m_delayAfterDrawOrigToRestore(GameEngineGlobalFixture::delayAfterEachDraw())
    {
        const auto CURRENT_TEST_MODULE_NAME
            = boost::unit_test::framework::current_auto_test_suite().full_name();

        const auto CURRENT_TEST_NAME = boost::unit_test::framework::current_test_case().full_name();

        if (!(DELAY_AFTER_EACH_DRAW < 0.0f))
        {
            GameEngineGlobalFixture::delayAfterEachDrawSet(DELAY_AFTER_EACH_DRAW);
        }

        GameEngineGlobalFixture::displayer().beginImageSeries(
            CURRENT_TEST_MODULE_NAME + "__" + CURRENT_TEST_NAME + "__" + NAME_OF_IMAGE_SET + "__",
            EXPECTED_IMAGE_COUNT,
            WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE);
    }

    SingleImageDisplayerScoped::~SingleImageDisplayerScoped()
    {
        GameEngineGlobalFixture::displayer().endImageSeries();
        GameEngineGlobalFixture::delayAfterEachDrawSet(m_delayAfterDrawOrigToRestore);
    }

    IDisplayer & SingleImageDisplayerScoped::get() { return GameEngineGlobalFixture::displayer(); }

    EventFlags::Enum SingleImageDisplayerScoped::draw(const bool WILL_CHECK_EVENTS)
    {
        return GameEngineGlobalFixture::draw(WILL_CHECK_EVENTS);
    }

    bool SingleImageDisplayerScoped::isPaused() { return GameEngineGlobalFixture::isPaused(); }

    void SingleImageDisplayerScoped::isPaused(const bool IS_PAUSED)
    {
        GameEngineGlobalFixture::isPaused(IS_PAUSED);
    }

} // namespace test
} // namespace heroespath
