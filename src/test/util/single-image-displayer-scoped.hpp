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
#include "test/util/i-displayer.hpp"

namespace heroespath
{
namespace test
{

    struct SingleImageDisplayerScoped
    {
        // negative DELAY_AFTER_EACH_DRAW means "no change"
        SingleImageDisplayerScoped(
            const std::string & NAME_OF_IMAGE_SET,
            const std::size_t EXPECTED_IMAGE_COUNT,
            const bool WILL_ENSURE_ALL_IMAGES_HAVE_SAME_SIZE = false,
            const float DELAY_AFTER_EACH_DRAW = -1.0f);

        ~SingleImageDisplayerScoped();

        IDisplayer & get();

        EventFlags::Enum draw(const bool WILL_CHECK_EVENTS = true);

        bool isPaused();
        void isPaused(const bool IS_PAUSED);

    private:
        float m_delayAfterDrawOrigToRestore;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_SCOPED_HPP_INCLUDED
