// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// radio-button-display.cpp
//
#include "radio-button-display.hpp"

#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/text-info.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::size_t RadioButtonSet_DisplayChange::MAX_NUM_RES_DISPLAYABLE_(10);

    RadioButtonSet_DisplayChange::RadioButtonSet_DisplayChange(
        const float POS_LEFT,
        const float POS_TOP,
        const sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t CALLBACK_HANDLER_PTR,
        const sfml_util::IStagePtr_t OWNER_ISTAGE_PTR)
        : RadioButtonSet(CALLBACK_HANDLER_PTR, "DisplayChange")
        , ownerStagePtr_(OWNER_ISTAGE_PTR)
        , resolutionVec_()
        , ORIG_INVALID_SELECTION_(
              sfml_util::Display::ComposeSupportedFullScreenVideoModesVec(resolutionVec_))
        , prevResolution_()
    {
        // handle case where there are too many resolutions to display
        if (resolutionVec_.size() > MAX_NUM_RES_DISPLAYABLE_)
        {
            resolutionVec_.resize(MAX_NUM_RES_DISPLAYABLE_);
        }

        // compose the radiobutton text fields
        sfml_util::gui::MouseTextInfoVec_t mouseTextInfoVec;
        for (auto const & NEXT_RESOLUTION : resolutionVec_)
        {
            sfml_util::gui::MouseTextInfo nextTextInfo(
                NEXT_RESOLUTION.ToString(false),
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
                sfml_util::FontManager::Instance()->Size_Small());

            mouseTextInfoVec.emplace_back(nextTextInfo);
        }

        sfml_util::GradientInfo gradientInfo(
            sf::Color(0, 0, 0, 150), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        sfml_util::gui::BackgroundInfo backgroundInfo(sf::Color(0, 0, 0, 60), gradientInfo);

        sfml_util::gui::box::Info resRadioButtonSetBoxInfo(
            1.0f,
            true,
            sf::FloatRect(),
            sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
            backgroundInfo);

        Setup(
            POS_LEFT,
            POS_TOP,
            mouseTextInfoVec,
            FindCurrentResolutionSelection(),
            sfml_util::Brightness::Bright,
            std::vector<std::size_t>(),
            resRadioButtonSetBoxInfo);
    }

    RadioButtonSet_DisplayChange::~RadioButtonSet_DisplayChange() = default;

    bool RadioButtonSet_DisplayChange::HandleCallback(const popup::PopupResponse & POPUP)
    {
        M_HP_LOG(
            GetEntityName() << " HandlePopupCallback(response=\""
                            << popup::ResponseTypes::ToString(POPUP.Response()) << "\")");

        if (POPUP.Response() == popup::ResponseTypes::No)
        {
            M_HP_LOG(
                GetEntityName()
                << " User rejected the new resolution.  Changing back to the previous res.");

            game::LoopManager::Instance()->ChangeResolution(
                ownerStagePtr_,
                this,
                prevResolution_,
                sfml_util::Display::Instance()->AntialiasLevel());
        }

        ChangeCurrentSelection(FindCurrentResolutionSelection());

        ownerStagePtr_->HandleResolutionChange();

        return true;
    }

    void RadioButtonSet_DisplayChange::OnClick(const sf::Vector2f &)
    {
        prevResolution_ = sfml_util::Display::GetCurrentResolution();

        game::LoopManager::Instance()->ChangeResolution(
            ownerStagePtr_,
            this,
            resolutionVec_[currentSelection_],
            sfml_util::Display::Instance()->AntialiasLevel());

        ownerStagePtr_->HandleResolutionChange();
    }

    std::size_t RadioButtonSet_DisplayChange::FindCurrentResolutionSelection()
    {
        const sfml_util::Resolution CURRENT_RES { sfml_util::Display::GetCurrentVideoMode() };

        auto const NUM_SUPPORTED_MODES { resolutionVec_.size() };
        auto resRadioButtonSetInitialSelection(NUM_SUPPORTED_MODES);

        for (std::size_t i(0); i < NUM_SUPPORTED_MODES; ++i)
        {
            if ((CURRENT_RES.width == resolutionVec_[i].width)
                && (CURRENT_RES.height == resolutionVec_[i].height))
            {
                resRadioButtonSetInitialSelection = i;
                break;
            }
        }

        return resRadioButtonSetInitialSelection;
    }

} // namespace sfml_util
} // namespace heroespath
