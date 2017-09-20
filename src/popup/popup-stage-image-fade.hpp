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
#ifndef POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
#define POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
//
// popup-stage-image-fade.hpp
//
#include "popup/popup-stage-base.hpp"

#include "sfml-util/gui/text-region.hpp"


namespace popup
{

    //Responsible for implementing the Image Fade Popup Stage.
    class PopupStageImageFade : public PopupStageBase
    {
        //prevent copy construction
        PopupStageImageFade(const PopupStageImageFade &) =delete;

        //prevent copy assignment
        PopupStageImageFade & operator=(const PopupStageImageFade &) =delete;

    public:
        explicit PopupStageImageFade(const PopupInfo &);
        virtual ~PopupStageImageFade();

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        static const float BEFORE_FADE_STARTS_DELAY_SEC_;

        sf::Texture textureCurr_;
        sf::Texture texturePrev_;
        sf::Sprite spriteCurr_;
        sf::Sprite spritePrev_;
        float beforeFadeTimerSec_;
        float fadeAlpha_;
        sfml_util::gui::TextRegionUPtr_t titleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
    };

}

#endif //POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
