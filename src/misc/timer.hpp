// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TIMER_HPP_INCLUDED
#define HEROESPATH_MISC_TIMER_HPP_INCLUDED
//
// timer.hpp
//

namespace heroespath
{
namespace misc
{

    // Responsible for keep track of a timer's current and max value
    // through recurring calls to Update().
    class Timer
    {
    public:
        explicit Timer(const float DURATION)
            : timeSec_(0.0f)
            , durationSec_(DURATION)
        {}

        float Time() const { return timeSec_; }
        float Duration() const { return durationSec_; }
        void Reset() { timeSec_ = 0.0f; }

        bool Update(const float ELAPSED_TIME_SEC)
        {
            timeSec_ += ELAPSED_TIME_SEC;

            if (timeSec_ > durationSec_)
            {
                timeSec_ = 0.0f;
                return true;
            }
            else
            {
                return false;
            }
        }

    private:
        float timeSec_;
        float durationSec_;
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_TIMER_HPP_INCLUDED
