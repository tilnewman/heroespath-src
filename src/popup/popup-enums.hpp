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

#include <SFML/Graphics/Rect.hpp>

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
    };

    struct PopupButtons : public EnumBaseBitField<SeparatorAlwaysSlash>
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

        static constexpr Enum YesNo = (Yes | No);
        static constexpr Enum YesNoCancel = (Yes | No | Cancel);
        static constexpr Enum SelectCancel = (Select | Cancel);

        static constexpr bool IsAffirmative(const PopupButtons::Enum BUTTON) noexcept
        {
            return (BUTTON & (Okay | Continue | Yes | Select));
        }
    };

    struct PopupButtonColor : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Light = 0,
            Dark,
            Count
        };
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

        static inline const sf::FloatRect
            ContentRegionRatios(const Enum IMAGE, const bool IS_LEFT = true) noexcept
        {
            switch (IMAGE)
            {
                case Banner: return sf::FloatRect(0.16f, 0.125f, 0.66f, 0.7f);
                case Regular: return sf::FloatRect(0.066f, 0.125f, 0.848f, 0.765f);
                case RegularSidebar: return sf::FloatRect(0.2f, 0.081f, 0.714f, 0.81f);
                case Large: return sf::FloatRect(0.094f, 0.086f, 0.786f, 0.794f);
                case LargeSidebar: return sf::FloatRect(0.168f, 0.098f, 0.712f, 0.782f);
                case Spellbook:
                {
                    if (IS_LEFT)
                    {
                        return sf::FloatRect(0.088f, 0.12f, 0.312f, 0.74f);
                    }
                    else
                    {
                        return sf::FloatRect(0.515f, 0.12f, 0.31f, 0.55f);
                    }
                }
                case MusicSheet:
                {
                    if (IS_LEFT)
                    {
                        return sf::FloatRect(0.078f, 0.12f, 0.372f, 0.61f);
                    }
                    else
                    {
                        return sf::FloatRect(0.512f, 0.12f, 0.368f, 0.61f);
                    }
                }
                case Count:
                default: return {};
            }
        }

        static constexpr float ScreenSizeRatio(const Enum IMAGE) noexcept
        {
            switch (IMAGE)
            {
                case Banner: return 0.25f;

                case Regular:
                case RegularSidebar: return 0.333f;

                case Large:
                case LargeSidebar:
                    return 0.5f;

                    // Spellbook and MusicSheet scale themselves, so return full size
                case Spellbook:
                case MusicSheet: return 1.0f;

                case Count:
                default: return 0.0f;
            }
        }

        static constexpr std::string_view ImageConfigKey(const PopupImage::Enum IMAGE) noexcept
        {
            switch (IMAGE)
            {
                case PopupImage::Banner: return "media-image-background-paper-popup-banner";
                case PopupImage::Regular: return "media-image-background-paper-popup-medium";

                case PopupImage::RegularSidebar:
                    return "media-image-background-paper-popup-medium-bar";

                case PopupImage::Large: return "media-image-background-paper-popup-large";

                case PopupImage::LargeSidebar:
                    return "media-image-background-paper-popup-large-bar";

                case PopupImage::Spellbook: return "media-image-background-paper-popup-spell-book";

                case PopupImage::MusicSheet:
                    return "media-image-background-paper-popup-music-sheet";

                case PopupImage::Count:
                default: return "popup::PopupImage::ImageConfigKey(ENUM=out_of_range)";
            }
        }

    }; // namespace heroespath

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
