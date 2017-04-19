//
// radio-button-display.cpp
//
#include "radio-button-display.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/text-info.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/loop-manager.hpp"


namespace gui_demo
{

    const std::size_t RadioButtonSet_DisplayChange::MAX_NUM_RES_DISPLAYABLE_(14);


    RadioButtonSet_DisplayChange::RadioButtonSet_DisplayChange(const float               POS_LEFT,
                                                               const float               POS_TOP,
                                                               sfml_util::IStage * const OWNER_STAGE_PTR)
    :
        RadioButtonSet         ("DisplayChange"),
        ownerStagePtr_         (OWNER_STAGE_PTR),
        resolutionVec_         (),
        ORIG_INVALID_SELECTION_(sfml_util::Display::ComposeSupportedFullScreenVideoModesVec(resolutionVec_)),
        prevResolution_        ()
    {
        //handle case where there are too many resolutions to display
        if (resolutionVec_.size() > MAX_NUM_RES_DISPLAYABLE_)
            resolutionVec_.resize(MAX_NUM_RES_DISPLAYABLE_);

        //compose the radiobutton text fields
        sfml_util::gui::MouseTextInfoVec_t mouseTextInfoVec;
        for (auto const & NEXT_RESOLUTION : resolutionVec_)
        {
            sfml_util::gui::MouseTextInfo nextTextInfo(NEXT_RESOLUTION.ToString(false), sfml_util::FontManager::Instance()->Font_Default1(), 20);
            mouseTextInfoVec.push_back(nextTextInfo);
        }

        sfml_util::GradientInfo gradientInfo(sf::Color(0, 0, 0, 150), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        sfml_util::gui::BackgroundInfo backgroundInfo(sf::Color(0, 0, 0, 60), gradientInfo);

        sfml_util::gui::box::Info resRadioButtonSetBoxInfo(1.0f,
                                                           true,
                                                           sf::FloatRect(),
                                                           sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
                                                           backgroundInfo);

        Setup(POS_LEFT,
              POS_TOP,
              mouseTextInfoVec,
              FindCurrentResolutionSelection(),
              sfml_util::Brightness::Bright,
              sfml_util::IndexVec_t(),
              resRadioButtonSetBoxInfo);
    }


    RadioButtonSet_DisplayChange::~RadioButtonSet_DisplayChange()
    {}


    bool RadioButtonSet_DisplayChange::HandleCallback(const heroespath::callback::PopupResponse & POPUP)
    {
        M_HP_LOG(GetEntityName() << " HandlePopupCallback(response=\"" << sfml_util::Response::ToString(POPUP.Response()) << "\")");

        if (POPUP.Response() == sfml_util::Response::No)
        {
            M_HP_LOG(GetEntityName() << " User rejected the new resolution.  Changing back to the previous res.");
            heroespath::LoopManager::Instance()->ChangeResolution(ownerStagePtr_, this, prevResolution_, sfml_util::Display::Instance()->AntialiasLevel());
        }

        ChangeCurrentSelection(FindCurrentResolutionSelection());

        M_ASSERT_OR_LOGANDTHROW_SS((nullptr != ownerStagePtr_), GetEntityName() << "'s RadioButtonSet_DisplayChange::HandlePopupCallback(" << sfml_util::Response::ToString(POPUP.Response()) << ") was called when the ownerStagePtr_ was null.");
        ownerStagePtr_->HandleResolutionChange();

        return true;
    }


    void RadioButtonSet_DisplayChange::OnClick(const sf::Vector2f &)
    {
        prevResolution_ = sfml_util::Display::GetCurrentResolution();
        heroespath::LoopManager::Instance()->ChangeResolution(ownerStagePtr_, this, resolutionVec_[currentSelection_], sfml_util::Display::Instance()->AntialiasLevel());

        M_ASSERT_OR_LOGANDTHROW_SS((nullptr != ownerStagePtr_), GetEntityName() << "'s RadioButtonSet_DisplayChange::OnClick() was called when the ownerStagePtr_ was null.");
        ownerStagePtr_->HandleResolutionChange();
    }


    std::size_t RadioButtonSet_DisplayChange::FindCurrentResolutionSelection()
    {
        const sfml_util::Resolution CURRENT_RES(sfml_util::Display::GetCurrentVideoMode());

        const std::size_t NUM_SUPPORTED_MODES(resolutionVec_.size());
        std::size_t resRadioButtonSetInitialSelection(NUM_SUPPORTED_MODES);

        for (std::size_t i(0); i < NUM_SUPPORTED_MODES; ++i)
        {
            if ((CURRENT_RES.width == resolutionVec_[i].width) && (CURRENT_RES.height == resolutionVec_[i].height))
            {
                resRadioButtonSetInitialSelection = i;
                break;
            }
        }

        return resRadioButtonSetInitialSelection;
    }

}
