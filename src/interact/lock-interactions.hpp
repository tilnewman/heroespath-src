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
#include "misc/boost-optional-that-throws.hpp"
#include "misc/handy-types.hpp"
#include "misc/not-null.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include <string>

namespace heroespath
{
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

        void PopupCharacterSelection(const popup::IPopupHandlerPtr_t) const;

        // returns true if a character was selected and characterPtr_ is not null,
        // if returns false then characterPtr_ is null.
        bool HandleCharacterSelectionPopupResponse(
            const popup::IPopupHandlerPtr_t, const popup::PopupResponse &);

        void PopupAttempting(const popup::IPopupHandlerPtr_t, const std::string &) const;

        void PopupSuccess(
            const popup::IPopupHandlerPtr_t, const std::string & NAME_OF_WHAT_OPENED) const;

        // returns true if a new title is achieved and the popup is displayed
        bool HandleAchievementIncrementAndReturnTrueOnNewTitleWithPopup(
            const popup::IPopupHandlerPtr_t);

        const creature::CreaturePtrOpt_t CharacterPtrOpt() { return characterPtrOpt_; }

    private:
        const misc::StrVec_t MakeInvalidLockPickCharacterMessages() const;

        std::size_t CharacterIndexWhoPrevAttempted() const;

        sfml_util::sound_effect::Enum RandomPickingSfx() const;

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
