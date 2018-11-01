// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_LOCK_INTERACTIONS_HPP_INCLUDED
#define HEROESPATH_INTERACT_LOCK_INTERACTIONS_HPP_INCLUDED
//
// lock-interactions.hpp
//
#include "gui/callback.hpp"
#include "gui/sound-effects-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <string>

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
} // namespace stage

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature

namespace interact
{

    // A collection of functions that are useful when unlocking items.
    class LockPicking
    {
    public:
        LockPicking();

        bool Attempt() const;

        void PopupCharacterSelection(
            const gui::PopupCallback_t::IHandlerPtr_t, stage::IStagePtr_t iStagePtr) const;

        // returns true if a character was selected and characterPtr_ is not null,
        // if returns false then characterPtr_ is null.
        bool HandleCharacterSelectionPopupResponse(
            const gui::PopupCallback_t::IHandlerPtr_t,
            const gui::PopupCallback_t::PacketPtr_t &,
            stage::IStagePtr_t iStagePtr);

        void PopupAttempting(
            const gui::PopupCallback_t::IHandlerPtr_t,
            const std::string &,
            stage::IStagePtr_t iStagePtr) const;

        void PopupSuccess(
            const gui::PopupCallback_t::IHandlerPtr_t,
            const std::string & NAME_OF_WHAT_OPENED,
            stage::IStagePtr_t iStagePtr) const;

        // returns true if a new title is achieved and the popup is displayed
        bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
            const gui::PopupCallback_t::IHandlerPtr_t, stage::IStagePtr_t iStagePtr);

        const creature::CreaturePtrOpt_t CharacterPtrOpt() { return characterPtrOpt_; }

    private:
        const std::vector<std::string> MakeInvalidLockPickCharacterMessages() const;

        std::size_t CharacterIndexWhoPrevAttempted() const;

        gui::sound_effect::Enum RandomPickingSfx() const;

        void PlaySuccessSfx() const;
        void PlayFailureSfx() const;

    public:
        static const std::string POPUP_NAME_CHARACTER_SELECTION_;
        static const std::string POPUP_NAME_NO_CHARACTER_CAN_PICK_;
        static const std::string POPUP_NAME_ATTEMPTING_;
        static const std::string POPUP_NAME_SUCCESS_;
        static const std::string POPUP_NAME_TITLE_ARCHIEVED_;

    private:
        creature::CreaturePtrOpt_t characterPtrOpt_;
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_LOCK_INTERACTIONS_HPP_INCLUDED
