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
#ifndef HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
//
// adventure-stage.hpp
//
#include "interact/interaction-manager.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"

namespace heroespath
{
namespace stage
{

    class AdventureDisplayStage;

    // Responsible for managing all AdventureStage interactions with the player.
    class AdventureStage : public sfml_util::Stage
    {
        AdventureStage(const AdventureStage &) = delete;
        AdventureStage(const AdventureStage &&) = delete;
        AdventureStage & operator=(const AdventureStage &) = delete;
        AdventureStage & operator=(const AdventureStage &&) = delete;

    public:
        AdventureStage();
        virtual ~AdventureStage();

        virtual void Setup() override;
        virtual void UpdateTime(const float ELAPSED_TIME_SEC) override;

    private:
        AdventureDisplayStage * adventureDisplayStagePtr_;
        interact::InteractionManager interactionManager_;
    };
}
}

#endif // HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
