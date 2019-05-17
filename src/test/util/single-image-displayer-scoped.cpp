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
        const std::size_t EXPECTED_IMAGE_COUNT,
        const std::string & EXTRA_NAME_OR_INFO,
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
            CURRENT_TEST_MODULE_NAME + "__" + CURRENT_TEST_NAME + "__" + EXTRA_NAME_OR_INFO,
            EXPECTED_IMAGE_COUNT);
    }

    SingleImageDisplayerScoped::~SingleImageDisplayerScoped()
    {
        GameEngineGlobalFixture::displayer().endImageSeries();
        GameEngineGlobalFixture::delayAfterEachDrawSet(m_delayAfterDrawOrigToRestore);
    }

    const std::string SingleImageDisplayerScoped::name() const
    {
        return GameEngineGlobalFixture::displayer().name();
    }

    void SingleImageDisplayerScoped::appendImage(gui::CachedTexture cachedTexture)
    {
        GameEngineGlobalFixture::displayer().appendImageToSeries(std::move(cachedTexture));
        GameEngineGlobalFixture::draw();
    }

} // namespace test
} // namespace heroespath
