// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_SCOPED_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_SCOPED_HPP_INCLUDED
//
// single-image-displayer-scoped.hpp
//
#include "testutil/game-engine-global-fixture.hpp"
#include "testutil/single-image-displayer.hpp"

#include <boost/test/unit_test.hpp>

namespace heroespath
{
namespace test
{

    void GameEngineGlobalFixture::setupBeforeAllTests()
    {
        m_iDisplayerUPtr = std::make_unique<SingleImageDisplayer>();
        m_delayAfterEachDrawSec = 0.01f;
    }

    struct SingleImageDisplayerScoped
    {
        SingleImageDisplayerScoped(
            const std::size_t EXPECTED_IMAGE_COUNT,
            const std::string & EXTRA_NAME_OR_INFO = "",
            const float DELAY_AFTER_EACH_DRAW = -1.0f)
            : m_delayAfterDrawOrigToRestore(GameEngineGlobalFixture::delayAfterEachDraw())
        {
            const auto CURRENT_TEST_MODULE_NAME
                = boost::unit_test::framework::current_auto_test_suite().full_name();

            const auto CURRENT_TEST_NAME
                = boost::unit_test::framework::current_test_case().full_name();

            if (!(DELAY_AFTER_EACH_DRAW < 0.0f))
            {
                GameEngineGlobalFixture::delayAfterEachDrawSet(DELAY_AFTER_EACH_DRAW);
            }

            GameEngineGlobalFixture::displayer().beginImageSeries(
                CURRENT_TEST_MODULE_NAME + "__" + CURRENT_TEST_NAME + "__" + EXTRA_NAME_OR_INFO,
                EXPECTED_IMAGE_COUNT);
        }

        ~SingleImageDisplayerScoped()
        {
            GameEngineGlobalFixture::displayer().endImageSeries();
            GameEngineGlobalFixture::delayAfterEachDrawSet(m_delayAfterDrawOrigToRestore);
        }

        void appendImage(gui::CachedTexture cachedTexture)
        {
            GameEngineGlobalFixture::displayer().appendImageToSeries(std::move(cachedTexture));
            GameEngineGlobalFixture::draw();
        }

        const std::string name() const { return GameEngineGlobalFixture::displayer().name(); }

    private:
        float m_delayAfterDrawOrigToRestore;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_SCOPED_HPP_INCLUDED
