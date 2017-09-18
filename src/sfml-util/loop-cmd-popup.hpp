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
#ifndef SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
#define SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
//
// loop-cmd-popup.hpp
//  Code that adds a popup Stage to a Loop object.
//
#include "sfml-util/loop-cmd.hpp"

#include <memory>


namespace sfml_util
{

    //A class that adds a popup to a Loop object
    class LoopCmd_AddStage_Popup : public LoopCmd
    {
    public:
        LoopCmd_AddStage_Popup(const game::PopupInfo & POPUP_INFO);

        virtual ~LoopCmd_AddStage_Popup();

        virtual bool Execute();

    private:
        const game::PopupInfo POPUP_INFO_;
    };

    using LoopCmd_AddStage_PopupSPtr_t = std::shared_ptr<LoopCmd_AddStage_Popup>;


    //A class that removes a popup from a Loop object
    class LoopCmd_RemoveStage_Popup : public LoopCmd
    {
    public:
        explicit LoopCmd_RemoveStage_Popup();
        virtual ~LoopCmd_RemoveStage_Popup();
        virtual bool Execute();
    };

    using LoopCmd_RemoveStage_PopupSPtr_t = std::shared_ptr<LoopCmd_RemoveStage_Popup>;

}
#endif //SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
