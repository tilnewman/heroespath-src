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
#ifndef HEROESPATH_NPC_MODEL_HPP_INCLUDED
#define HEROESPATH_NPC_MODEL_HPP_INCLUDED
//
// model.hpp
//
#include "sfml-util/direction-enum.hpp"
#include <memory>
#include <vector>


namespace heroespath
{
namespace npc
{
    struct IView;
    using IViewUPtr_t = std::unique_ptr<IView>;


    //Responsible for all state and non-drawing behavior of an NPC.
    class Model
    {
        Model(const Model &) = delete;
        Model(const Model &&) = delete;
        Model & operator=(const Model &) = delete;

    public:
        Model(IViewUPtr_t viewUPtr);

        void Update(const float TIME_ELAPSED);

        inline const IView & GetView() const { return * viewUPtr_; }

        void WalkAnim(const sfml_util::Direction::Enum, const bool WILL_START);

    private:
        float RandomBlinkDelay() const;

        void ExtendTimeBeforeNextBlinkIfNeeded(const float PREV_BLINK_DELAY);

    private:
        static const float NUM_BLINKS_TIME_WINDOW_SEC_;
        static const std::size_t NUM_BLINKS_WITHIN_TIME_WINDOW_;
        static const float TIME_BETWEEN_BLINK_MIN_SEC_;
        static const float TIME_BETWEEN_BLINK_MAX_SEC_;

        IViewUPtr_t viewUPtr_;
        float blinkTimerSec_;
        float timeBeforeNextBlinkSec_;
        std::vector<float> blinkTimes_;
    };

}
}

#endif //HEROESPATH_NPC_MODEL_HPP_INCLUDED
