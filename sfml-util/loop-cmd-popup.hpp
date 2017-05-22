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

    //forward declaration
    class ILoop;
    using ILoopSPtr_t = std::shared_ptr<ILoop>;


    //A class that adds a popup to a Loop object
    class LoopCmd_AddStage_Popup : public LoopCmd
    {
    public:
        LoopCmd_AddStage_Popup(sfml_util::ILoopSPtr_t &      iLoopSPtr,
                               const game::PopupInfo & POPUP_INFO);

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
        explicit LoopCmd_RemoveStage_Popup(sfml_util::ILoopSPtr_t & iLoopSPtr);
        virtual ~LoopCmd_RemoveStage_Popup();
        virtual bool Execute();
    };

    using LoopCmd_RemoveStage_PopupSPtr_t = std::shared_ptr<LoopCmd_RemoveStage_Popup>;

}
#endif //SFMLUTIL_LOOPCOMMANDPOPUP_INCLUDED
