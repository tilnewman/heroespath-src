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
#ifndef GAME_OUROBOROS_INCLUDED
#define GAME_OUROBOROS_INCLUDED
//
// ouroboros.hpp
//  An animated snake eating its own tail for the background of main menu screens.
//
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sliders.hpp"

#include <memory>
#include <string>


namespace game
{

    //manages an animated snake image that spins, grows/shrinks, and fades in/out
    class Ouroboros : public sfml_util::gui::GuiEntity
    {
        //prevent copy construction
        Ouroboros(const Ouroboros &) =delete;

        //prevent copy assignment
        Ouroboros & operator=(const Ouroboros &) =delete;

    public:
        explicit Ouroboros(const std::string & NAME, const bool WILL_INVERT = false);
        virtual ~Ouroboros();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        virtual bool UpdateTime(const float ELAPSED_TIME_SEC);
        virtual void OnClick(const sf::Vector2f &) {}
    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        float rotation_;
        sf::Sprite  sprite_;
        sf::Texture texture_;
        sfml_util::sliders::Drifter<float> sizeDrifter_;
        sfml_util::sliders::Drifter<float> shadeDrifter_;
        sfml_util::sliders::Drifter<float> rotSpeedDrifter_;
    };

    using OuroborosSPtr_t = std::shared_ptr<Ouroboros>;

}
#endif //GAME_OUROBOROS_INCLUDED
