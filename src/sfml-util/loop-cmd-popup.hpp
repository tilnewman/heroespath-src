// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
#define HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
//
// loop-cmd-popup.hpp
//  Code that adds a popup Stage to a Loop object.
//
#include "popup/popup-info.hpp"
#include "sfml-util/loop-cmd.hpp"

#include <boost/type_index.hpp>

#include <memory>

namespace heroespath
{
namespace sfml_util
{

    class Loop;

    // Responsible for setting a specific popup stage
    template <typename PopupType_t>
    class LoopCmd_AddStage_Popup_Specific : public LoopCmd
    {
    public:
        LoopCmd_AddStage_Popup_Specific(sfml_util::Loop & loop, const popup::PopupInfo & POPUP_INFO)
            : LoopCmd(
                  "AddStagePopupSpecific_" + boost::typeindex::type_id<PopupType_t>().pretty_name())
            , loop_(loop)
            , popupInfo_(POPUP_INFO)
        {}

        virtual ~LoopCmd_AddStage_Popup_Specific() = default;

        void Execute() override
        {
            auto popupStagePtr(new PopupType_t(popupInfo_));
            popupStagePtr->Setup();
            loop_.SetPopup(popupStagePtr);
        }

    private:
        sfml_util::Loop & loop_;
        popup::PopupInfo popupInfo_;
    };

    // Responsible for remvoing a popup stage
    class LoopCmd_RemoveStage_Popup : public LoopCmd
    {
    public:
        LoopCmd_RemoveStage_Popup();
        virtual ~LoopCmd_RemoveStage_Popup();
        void Execute() override;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
