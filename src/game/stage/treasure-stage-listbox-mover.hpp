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
#ifndef GAME_STAGE_TREASURESTAGELISTBOXMOVER_HPP_INCLUDED
#define GAME_STAGE_TREASURESTAGELISTBOXMOVER_HPP_INCLUDED
//
// treasure-stage-listbox-mover.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity-slider.hpp"


namespace sfml_util
{
namespace gui
{
    class IGuiEntity;
    using IGuiEntityPtr_t = IGuiEntity *;
}
}
namespace game
{
namespace stage
{
namespace TreasureStage
{

    enum class SourceType
    {
        Worn,
        Container
    };


    //Responsible for managing the state and position of a source and target listbox.  The source
    //listbox is either the contents of worn items on defeated enemy creatures or the contents of
    //their treasure container.  The target listbox shows the contents of one of the player
    //character's unequipped items.  A simple interface for switching between either of the
    //possible source listboxes or one of the six possible target listboxes must be presented.
    //The listboxes animate when changed, so the state of a change-in-progress must be maintained.
    class ListboxMover
    {
        ListboxMover() =delete;

    public:
        ListboxMover(
            const std::size_t NUM_PLAYERS,
            sfml_util::gui::IGuiEntityPtr_t sourceListboxGEPtr,
            const sf::Vector2f & SOURCE_ONSCREEN_POS_V,
            const sf::Vector2f & SOURCE_OFFSCREEN_POS_V,
            sfml_util::gui::IGuiEntityPtr_t targetListboxGEPtr,
            const sf::Vector2f & TARGET_ONSCREEN_POS_V,
            const sf::Vector2f & TARGET_OFFSCREEN_POS_V);

        ~ListboxMover();

        inline bool IsSourceMoving() const { return sourceSlider_.IsMoving(); }
        inline bool IsTargetMoving() const { return targetSlider_.IsMoving(); }
        inline bool IsEitherMoving() const { return IsSourceMoving() || IsTargetMoving(); }

        inline SourceType SourceType() const { return sourceType_; }
        inline std::size_t TargetNumber() const { return targetNumber_; }

        TreasureStage::SourceType SourceChange();
        std::size_t TargetChangeInc();
        std::size_t TargetChangeDec();

        //These functions return true if the listbox has just finished
        //moving offscreen and its contents need to be updated.
        virtual bool UpdateTimeSource(const float ELAPSED_TIME_SECONDS);
        virtual bool UpdateTimeTarget(const float ELAPSED_TIME_SECONDS);

    private:
        void TargetChange();

        bool UpdateTime(
            sfml_util::gui::GuiEntitySlider & slider,
            const float ELAPSED_TIME_SECONDS);

    private:
        static const float SLIDE_SPEED_;
        //
        std::size_t targetNumber_;
        std::size_t targetNumberMax_;
        TreasureStage::SourceType sourceType_;
        sfml_util::gui::GuiEntitySlider sourceSlider_;
        sfml_util::gui::GuiEntitySlider targetSlider_;
    };

}
}
}

#endif //GAME_STAGE_TREASURESTAGELISTBOXMOVER_HPP_INCLUDED
