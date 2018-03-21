///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
