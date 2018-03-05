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
#ifndef HEROESPATH_CREDITSTAGE_HPP_INCLUDED
#define HEROESPATH_CREDITSTAGE_HPP_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"

#include "stage/credit.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the credits
    class CreditsStage : public sfml_util::Stage
    {
        CreditsStage(const CreditsStage &) = delete;
        CreditsStage(const CreditsStage &&) = delete;
        CreditsStage & operator=(const CreditsStage &) = delete;
        CreditsStage & operator=(const CreditsStage &&) = delete;

    public:
        CreditsStage();
        virtual ~CreditsStage();

        virtual void Setup();
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

    private:
        static const float DEFAULT_SCROLL_SPEED_;
        static const float SCROLL_SPEED_MULT_;
        static const float SCROLL_SPEED_MAX_;
        static const float CREDIT_BOX_INNER_PAD_;
        //
        const float CREDIT_BOX_WIDTH_;
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float CREDITBOX_POS_LEFT_;
        //
        float creditBoxPosTop_;
        float creditBoxHeight_;
        float totalHeight_;
        float heightTracker_;
        sf::Texture hpTitleTexture_;
        sf::Sprite bpTitleSprite_;
        sfml_util::gui::box::Box box_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        CreditSVec_t creditSVec_;
        float scrollSpeed_;
        bool isKeyHeldDown_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CREDITSTAGE_HPP_INCLUDED
