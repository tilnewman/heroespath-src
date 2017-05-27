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
#ifndef SFMLUTIL_GUIELEMENTS_INCLUDED
#define SFMLUTIL_GUIELEMENTS_INCLUDED
//
// gui-elements.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Manages a set of sprites relating to gui drawing elements.
    class GuiElements
    {
        //prevent copy construction
        GuiElements(const GuiElements &) =delete;

        //prevent copy assignment
        GuiElements & operator=(const GuiElements &) =delete;

        //prevent non-singleton construction
        GuiElements();

    public:
        static GuiElements * Instance();
        static void Acquire();
        static void Release();

        //Managed textures
        //these textures are used so often that it saves resources to keep a single instance of them in memory
        inline const sf::Texture & GetTexture() const       { return elementsTexture_; }
        inline const sf::Texture & GetTextureWood() const   { return woodBgTexture_; }
        inline const sf::Texture & GetTextureKnot() const   { return darkKnotBgTexture_; }

        //If the small line box is going to grow to fit inner contents, then these values specity how to grow or shrink each side.
        inline float GetLineSmallBoxPadLeft() const         { return 8.0f; } //subtract this amount from left
        inline float GetLineSmallBoxPadRight() const        { return 9.0f; } //add this amount to right
        inline float GetLineSmallBoxPadTop() const          { return 8.0f; } //subtract this amount from top
        inline float GetLineSmallBoxPadBot() const          { return 9.0f; } //add this amount to bottom

        //Get'ers for rects that define each gui element
        inline const sf::IntRect GetRect_RadioButtonBrightOff() const	 { return radioButtonBrightOffSpriteRect_; }
        inline const sf::IntRect GetRect_RadioButtonBrightOn() const	 { return radioButtonBrightOnSpriteRect_; }
        inline const sf::IntRect GetRect_RadioButtonMedOff() const		 { return radioButtonMedOffSpriteRect_; }
        inline const sf::IntRect GetRect_RadioButtonMedOn() const		 { return radioButtonMedOnSpriteRect_; }
        inline const sf::IntRect GetRect_RadioButtonDarkOff() const		 { return radioButtonDarkOffSpriteRect_; }
        inline const sf::IntRect GetRect_RadioButtonDarkOn() const		 { return radioButtonDarkOnSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxMedOff() const		     { return checkBoxMedOffSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxMedOn() const		     { return checkBoxMedOnSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxBrightOff() const		 { return checkBoxBrightOffSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxBrightOn() const		 { return checkBoxBrightOnSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxDarkOff() const		 { return checkBoxDarkOffSpriteRect_; }
        inline const sf::IntRect GetRect_CheckBoxDarkOn() const		     { return checkBoxDarkOnSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusLeftMed() const		 { return arrowMinusLeftMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusRightMed() const		 { return arrowPlusRightMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusLeftBright() const	 { return arrowMinusLeftBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusRightBright() const	 { return arrowPlusRightBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusLeftDark() const		 { return arrowMinusLeftDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusRightDark() const		 { return arrowPlusRightDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusDownBright() const	 { return arrowMinusDownBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusUpBright() const	     { return arrowPlusUpBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusDownMed() const	     { return arrowMinusDownMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusUpMed() const		     { return arrowPlusUpMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowMinusDownDark() const	     { return arrowMinusDownDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowPlusUpDark() const	     { return arrowPlusUpDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLeftMed() const		     { return arrowLeftMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowRightMed() const		     { return arrowRightMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowDownMed() const		     { return arrowDownMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowUpMed() const		         { return arrowUpMedSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLeftBright() const	     { return arrowLeftBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowRightBright() const	     { return arrowRightBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowDownBright() const	     { return arrowDownBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowUpBright() const		     { return arrowUpBrightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLeftDark() const		     { return arrowLeftDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowRightDark() const		     { return arrowRightDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowDownDark() const		     { return arrowDownDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowUpDark() const		     { return arrowUpDarkSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLargeLeft() const		     { return arrowLargeLeftSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLargeUp() const		     { return arrowLargeUpSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLargeRight() const	     { return arrowLargeRightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowLargeDown() const		     { return arrowLargeDownSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowSmallLeft() const		     { return arrowSmallLeftSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowSmallUp() const		     { return arrowSmallUpSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowSmallRight() const		 { return arrowSmallRightSpriteRect_; }
        inline const sf::IntRect GetRect_ArrowSmallDown() const		     { return arrowSmallDownSpriteRect_; }
        inline const sf::IntRect GetRect_BoxSmall() const		         { return boxSmallSpriteRect_; }
        inline const sf::IntRect GetRect_PadVerticalSmall() const		 { return padVerticalSmallSpriteRect_; }
        inline const sf::IntRect GetRect_PadVerticalLarge() const		 { return padVerticalLargeSpriteRect_; }
        inline const sf::IntRect GetRect_PadHorizontalLarge() const		 { return padHorizontalLargeSpriteRect_; }
        inline const sf::IntRect GetRect_HorizontalLineLarge() const	 { return horizontalLineLargeSpriteRect_; }
        inline const sf::IntRect GetRect_HorizontalLineSmall() const	 { return horizontalLineSmallSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallVerticalLeft() const	 { return lineSmallVerticalLeftSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallVerticalRight() const	 { return lineSmallVerticalRightSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallEndLeft() const		 { return lineSmallEndLeftSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallEndRight() const		 { return lineSmallEndRightSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallEndTop() const		 { return lineSmallEndTopSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallEndBot() const		 { return lineSmallEndBotSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallCornerTopLeft() const	 { return lineSmallCornerTopLeftSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallCornerTopRight() const { return lineSmallCornerTopRightSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallCornerBotLeft() const	 { return lineSmallCornerBotLeftSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallCornerBotRight() const { return lineSmallCornerBotRightSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallHorizontalTop() const	 { return lineSmallHorizontalTopSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallHorizontalBot() const	 { return lineSmallHorizontalBotSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallCross() const		     { return lineSmallCrossSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallTTop() const		     { return lineSmallTTopSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallTLeft() const		     { return lineSmallTLeftSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallTBot() const		     { return lineSmallTBotSpriteRect_; }
        inline const sf::IntRect GetRect_LineSmallTRight() const		 { return lineSmallTRightSpriteRect_; }

    private:
        static std::unique_ptr<GuiElements> instanceUPtr_;
        //
        sf::Texture elementsTexture_;
        sf::Texture woodBgTexture_;
        sf::Texture darkKnotBgTexture_;
        //
        const sf::IntRect radioButtonBrightOffSpriteRect_;
        const sf::IntRect radioButtonBrightOnSpriteRect_;
        const sf::IntRect radioButtonMedOffSpriteRect_;
        const sf::IntRect radioButtonMedOnSpriteRect_;
        const sf::IntRect radioButtonDarkOffSpriteRect_;
        const sf::IntRect radioButtonDarkOnSpriteRect_;
        const sf::IntRect checkBoxMedOffSpriteRect_;
        const sf::IntRect checkBoxMedOnSpriteRect_;
        const sf::IntRect checkBoxBrightOffSpriteRect_;
        const sf::IntRect checkBoxBrightOnSpriteRect_;
        const sf::IntRect checkBoxDarkOffSpriteRect_;
        const sf::IntRect checkBoxDarkOnSpriteRect_;
        const sf::IntRect arrowMinusLeftMedSpriteRect_;
        const sf::IntRect arrowPlusRightMedSpriteRect_;
        const sf::IntRect arrowMinusLeftBrightSpriteRect_;
        const sf::IntRect arrowPlusRightBrightSpriteRect_;
        const sf::IntRect arrowMinusLeftDarkSpriteRect_;
        const sf::IntRect arrowPlusRightDarkSpriteRect_;
        const sf::IntRect arrowMinusDownBrightSpriteRect_;
        const sf::IntRect arrowPlusUpBrightSpriteRect_;
        const sf::IntRect arrowMinusDownMedSpriteRect_;
        const sf::IntRect arrowPlusUpMedSpriteRect_;
        const sf::IntRect arrowMinusDownDarkSpriteRect_;
        const sf::IntRect arrowPlusUpDarkSpriteRect_;
        const sf::IntRect arrowLeftMedSpriteRect_;
        const sf::IntRect arrowRightMedSpriteRect_;
        const sf::IntRect arrowDownMedSpriteRect_;
        const sf::IntRect arrowUpMedSpriteRect_;
        const sf::IntRect arrowLeftBrightSpriteRect_;
        const sf::IntRect arrowRightBrightSpriteRect_;
        const sf::IntRect arrowDownBrightSpriteRect_;
        const sf::IntRect arrowUpBrightSpriteRect_;
        const sf::IntRect arrowLeftDarkSpriteRect_;
        const sf::IntRect arrowRightDarkSpriteRect_;
        const sf::IntRect arrowDownDarkSpriteRect_;
        const sf::IntRect arrowUpDarkSpriteRect_;
        const sf::IntRect arrowLargeLeftSpriteRect_;
        const sf::IntRect arrowLargeUpSpriteRect_;
        const sf::IntRect arrowLargeRightSpriteRect_;
        const sf::IntRect arrowLargeDownSpriteRect_;
        const sf::IntRect arrowSmallLeftSpriteRect_;
        const sf::IntRect arrowSmallUpSpriteRect_;
        const sf::IntRect arrowSmallRightSpriteRect_;
        const sf::IntRect arrowSmallDownSpriteRect_;
        const sf::IntRect boxSmallSpriteRect_;
        const sf::IntRect padVerticalSmallSpriteRect_;
        const sf::IntRect padVerticalLargeSpriteRect_;
        const sf::IntRect padHorizontalLargeSpriteRect_;
        const sf::IntRect horizontalLineLargeSpriteRect_;
        const sf::IntRect horizontalLineSmallSpriteRect_;
        const sf::IntRect lineSmallVerticalLeftSpriteRect_;
        const sf::IntRect lineSmallVerticalRightSpriteRect_;
        const sf::IntRect lineSmallEndLeftSpriteRect_;
        const sf::IntRect lineSmallEndRightSpriteRect_;
        const sf::IntRect lineSmallEndTopSpriteRect_;
        const sf::IntRect lineSmallEndBotSpriteRect_;
        const sf::IntRect lineSmallCornerTopLeftSpriteRect_;
        const sf::IntRect lineSmallCornerTopRightSpriteRect_;
        const sf::IntRect lineSmallCornerBotLeftSpriteRect_;
        const sf::IntRect lineSmallCornerBotRightSpriteRect_;
        const sf::IntRect lineSmallHorizontalTopSpriteRect_;
        const sf::IntRect lineSmallHorizontalBotSpriteRect_;
        const sf::IntRect lineSmallCrossSpriteRect_;
        const sf::IntRect lineSmallTTopSpriteRect_;
        const sf::IntRect lineSmallTLeftSpriteRect_;
        const sf::IntRect lineSmallTBotSpriteRect_;
        const sf::IntRect lineSmallTRightSpriteRect_;
    };

}
}

#endif //SFMLUTIL_GUIELEMENTS_INCLUDED
