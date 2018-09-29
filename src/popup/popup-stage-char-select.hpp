// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
//
// popup-stage-char-select.hpp
//
#include "misc/vector-map.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "sfml-util/gui/text-region.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Character Select Popup Stage.
    class PopupStageCharacterSelect : public PopupStageImageSelect
    {
        PopupStageCharacterSelect(const PopupStageCharacterSelect &);
        PopupStageCharacterSelect & operator=(const PopupStageCharacterSelect &);

    public:
        explicit PopupStageCharacterSelect(const PopupInfo &);
        virtual ~PopupStageCharacterSelect();

        using PopupStageBase::HandleCallback;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;

    protected:
        bool HandleSelect() override;

    private:
        std::size_t CountMax() const override;
        void SetupContent(const bool WILL_ERASE) override;
        void SetupCharacterSelectDetailText(const bool WILL_ERASE);
        void SetupCharacterSelectionRejectImage(const bool WILL_ERASE);

        const sfml_util::CachedTexture &
            GetCurrentCachedTexture(const std::size_t IMAGE_INDEX) override;

    private:
        sfml_util::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        misc::VectorMap<creature::CreaturePtr_t, sfml_util::CachedTexture> creatureToTextureMap_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
