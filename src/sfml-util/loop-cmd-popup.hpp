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
#ifndef HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
//
// loop-cmd-popup.hpp
//  Code that adds a popup Stage to a Loop object.
//
#include "sfml-util/loop-cmd.hpp"

#include "popup/popup-info.hpp"

#include <boost/type_index.hpp>

#include <memory>

namespace heroespath
{
namespace sfml_util
{

    class Loop;

    template <typename PopupType_t>
    class LoopCmd_AddStage_Popup_Specific : public LoopCmd
    {
    public:
        LoopCmd_AddStage_Popup_Specific(sfml_util::Loop & loop, const popup::PopupInfo & POPUP_INFO)
            : LoopCmd(
                  "AddStage_Popup_Specific_"
                  + boost::typeindex::type_id<PopupType_t>().pretty_name())
            , loop_(loop)
            , popupInfo_(POPUP_INFO)
        {}

        virtual ~LoopCmd_AddStage_Popup_Specific() = default;

        virtual bool Execute()
        {
            auto popupStagePtr(new PopupType_t(popupInfo_));
            popupStagePtr->Setup();
            loop_.SetPopup(popupStagePtr);
            return true;
        }

    private:
        sfml_util::Loop & loop_;
        popup::PopupInfo popupInfo_;
    };

    // A class that removes a popup from a Loop object
    class LoopCmd_RemoveStage_Popup : public LoopCmd
    {
    public:
        LoopCmd_RemoveStage_Popup();
        virtual ~LoopCmd_RemoveStage_Popup();
        virtual bool Execute();
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
