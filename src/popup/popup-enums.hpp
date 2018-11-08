// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
#define HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
//
// popup-enums.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    struct PopupStage : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Generic = 0,
            CharacterSelect,
            CombatOver,
            ImageFade,
            ImageSelect,
            InventoryPrompt,
            Musicsheet,
            NumberSelect,
            ResolutionChange,
            Spellbook,
            SystemError,
            TreasureTrap,
            Count
        };

        static const std::string ToString(const Enum);
    };

    struct PopupButtons : public EnumBaseBitField
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            Okay = 1 << 0,
            Continue = 1 << 1,
            Yes = 1 << 2,
            No = 1 << 3,
            Cancel = 1 << 4,
            Select = 1 << 5,
            Last = Select
        };

        static const Enum YesNo = (Yes | No);
        static const Enum YesNoCancel = (Yes | No | Cancel);
        static const Enum SelectCancel = (Select | Cancel);

        static const std::string ToString(const Enum, const EnumStringHow & HOW = EnumStringHow());

        static bool IsAffirmative(const Enum);

        static const std::string
            ToStringPopulate(const EnumUnderlying_t BUTTONS, const std::string & SEPARATOR = "/");
    };

    struct PopupButtonColor : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Light = 0,
            Dark,
            Count
        };

        static const std::string ToString(const PopupButtonColor::Enum);
    };

    struct PopupImage : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Banner = 0,
            Regular,
            RegularSidebar,
            Large,
            LargeSidebar,
            Spellbook,
            MusicSheet,
            Count
        };

        static const std::string ToString(const PopupImage::Enum);
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
