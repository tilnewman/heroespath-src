// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
//
// popup-stage-combat-over.hpp
//
#include "popup/popup-stage-base.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/text-region.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Image Fade Popup Stage.
    class PopupStageCombatOver : public PopupStageBase
    {
    public:
        PopupStageCombatOver(const PopupStageCombatOver &) = delete;
        PopupStageCombatOver(PopupStageCombatOver &&) = delete;
        PopupStageCombatOver & operator=(const PopupStageCombatOver &) = delete;
        PopupStageCombatOver & operator=(PopupStageCombatOver &&) = delete;

    public:
        explicit PopupStageCombatOver(const PopupInfo &);
        virtual ~PopupStageCombatOver();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

    private:
        const std::string PickImagePathKey(const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const;
        void PlaySfxForHowCombatEnded(const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const;

        sfml_util::CachedTexture bgCachedTexture_;
        sf::Sprite bgSprite_;
        sfml_util::TextRegionUPtr_t titleTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t descTextRegionUPtr_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGECOMBATOVER_HPP_INCLUDED
