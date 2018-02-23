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
#ifndef HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
//
// adventure-stage-interact-stage.hpp
//
#include "sfml-util/stage.hpp"
#include "sfml-util/sfml-graphics.hpp"


namespace heroespath
{
namespace map
{
    class Map;
}
namespace stage
{

    //Responsible for the interaction region of the Adventure Stage.
    class InteractStage : public sfml_util::Stage
    {
        InteractStage(const InteractStage &) = delete;
        InteractStage & operator=(const InteractStage &) = delete;

    public:
        InteractStage(map::Map &, const sf::FloatRect & STAGE_REGION);
        virtual ~InteractStage() {}

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    private:
        map::Map & map_;
        sf::Texture texture_;
        sf::Sprite sprite_;
    };

}
}


#endif //HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
