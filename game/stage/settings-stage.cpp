//
// settings-stage.cpp
//
#include "settings-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "utilz/real.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/radio-button-display.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"

#include <sstream>


namespace game
{
namespace stage
{

    const float SettingsStage::BOX_WIDTH_ (910.0f);
    const float SettingsStage::BOX_HEIGHT_(530.0f);
    const float SettingsStage::BOX_INNER_PAD_(20.0f);
    const float SettingsStage::MUSIC_VOLUME_HORIZ_POS_(200.0f);
    const float SettingsStage::SOUNDEFFECTS_VOLUME_HORIZ_POS_(325.0f);
    const float SettingsStage::ANTIALIAS_HORIZ_POS_(70.0f);


    SettingsStage::SettingsStage()
    :
        Stage                        ("Settings"),
        boxTop_                      (0.0f),
        backgroundImage_             ("media-images-backgrounds-tile-darkknot"),
        mainMenuTitle_               ("settings_button_normal.png"),
        box_                         ("SettingsStage"),
        textRegionSVec_              (),
        resRadioButtonSetSPtr_       (),
        resolutionTextRegionSPtr_    (),
        musicVolumeTextRegionSPtr_   (),
        musicVolumeSliderBarSPtr_    (),
        effectsVolumeTextRegionSPtr_ (),
        effectsVolumeSliderBarSPtr_  (),
        backButtonSPtr_              (new main_menu_buttons::BackButton(0.0f, 0.0f, 1.0f, false)),
        revisionTextRegionSPtr_      (),
        aaRadioButtonSetSPtr_        (),
        aaTextRegionSPtr_            (),
        prevAALevel_                 (sfml_util::Display::Instance()->AntialiasLevel()),
        bottomSymbol_                (),
        musicInfoLabelTextRegionSPtr_(),
        musicInfoTextRegionSPtr_     ()
    {}


    SettingsStage::~SettingsStage()
    {}


    bool SettingsStage::HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == aaRadioButtonSetSPtr_.get())
        {
            prevAALevel_ = sfml_util::Display::Instance()->AntialiasLevel();
            const unsigned NEW_ANTIALIAS_LEVEL( (PACKAGE.PTR_->GetSelectedNumber() == 0) ? 0 : static_cast<unsigned>(std::pow(static_cast<double>(2), static_cast<double>(PACKAGE.PTR_->GetSelectedNumber() - 1))) );
            LoopManager::Instance()->ChangeResolution(this, this, sfml_util::Display::GetCurrentResolution(), NEW_ANTIALIAS_LEVEL);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool SettingsStage::HandleCallback(const game::callback::PopupResponse & POPUP)
    {
        M_HP_LOG(GetStageName() << " HandlePopupCallback(response=\"" << sfml_util::Response::ToString(POPUP.Response()) << "\")");

        if (POPUP.Response() == sfml_util::Response::No)
        {
            M_HP_LOG(GetStageName() << " Stage.  User rejected the new antialias level.  Changing back to the previous.");
            game::LoopManager::Instance()->ChangeResolution(this, this, sfml_util::Display::Instance()->GetCurrentResolution(), prevAALevel_);
            HandleResolutionChange();
            return false;
        }
        else if (POPUP.Response() == sfml_util::Response::Okay)
        {
            //case where the antialiasing mode was not supported and need to revert back to original value
            switch (prevAALevel_)
            {
                case 1:  { aaRadioButtonSetSPtr_->SetSelectNumber(1); break; }
                case 2:  { aaRadioButtonSetSPtr_->SetSelectNumber(2); break; }
                case 4:  { aaRadioButtonSetSPtr_->SetSelectNumber(3); break; }
                case 8:  { aaRadioButtonSetSPtr_->SetSelectNumber(4); break; }
                case 16: { aaRadioButtonSetSPtr_->SetSelectNumber(5); break; }
                default: { aaRadioButtonSetSPtr_->SetSelectNumber(0); break; }
            }
            return true;
        }
        else
            return true;
    }


    void SettingsStage::Setup()
    {
        //establish screen positions
        const sf::FloatRect NEW_STAGE_REGION(0.0f, 0.0f, sfml_util::Display::Instance()->GetWinWidth(), sfml_util::Display::Instance()->GetWinHeight());
        StageRegionSet(NEW_STAGE_REGION);
        const float BOX_POS_LEFT((StageRegionWidth() - BOX_WIDTH_) - ((StageRegionWidth() - BOX_WIDTH_) * 0.5f));
        const float BOX_POS_TOP(mainMenuTitle_.LowerPosition());
        const sf::FloatRect BOX_RECT(BOX_POS_LEFT, BOX_POS_TOP, BOX_WIDTH_, BOX_HEIGHT_);
        const sf::FloatRect BOX_RECT_INNER(BOX_POS_LEFT + BOX_INNER_PAD_, BOX_POS_TOP + BOX_INNER_PAD_, BOX_WIDTH_ - (BOX_INNER_PAD_ * 2.0f), BOX_HEIGHT_ - (BOX_INNER_PAD_ * 2.0f));
        boxTop_ = BOX_POS_TOP;

        //background box
        box_.SetupBox( sfml_util::gui::box::Info(true,
                                                 BOX_RECT,
                                                 sfml_util::gui::ColorSet(sf::Color::White),
                                                 sfml_util::gui::BackgroundInfo(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), BOX_RECT)) );

        //establish baseline TextInfo objects
        sfml_util::gui::TextInfo settingsTextInfoTitle("", sfml_util::FontManager::Instance()->Font_Typical(), 26, sf::Color(255,255,255,200));

        //change resolution label
        sf::FloatRect resRect(BOX_RECT_INNER);
        resRect.left += 90.0f;
        resRect.top -= 10.0f;
        resRect.width = 0.0f;
        resRect.height = 0.0f;
        sfml_util::gui::TextInfo resolutionTitleTextInfo(settingsTextInfoTitle);
        resolutionTitleTextInfo.text = "Change Resolution";
        resolutionTitleTextInfo.justified = sfml_util::Justified::Center;
        resolutionTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("ResolutionChangeSetting", resolutionTitleTextInfo, resRect, sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_, sfml_util::gui::box::Info()) );
        textRegionSVec_.push_back(resolutionTextRegionSPtr_);

        //resolution radiobuttonset
        const float RES_RADIOBUTTONSET_POS_LEFT(BOX_RECT_INNER.left);
        const float RES_RADIOBUTTONSET_POS_TOP (BOX_RECT.top + 50.0f);
        resRadioButtonSetSPtr_.reset( new gui_demo::RadioButtonSet_DisplayChange(RES_RADIOBUTTONSET_POS_LEFT, RES_RADIOBUTTONSET_POS_TOP, this) );
        resRadioButtonSetSPtr_->SetEntityColors(sfml_util::gui::ColorSet(sf::Color(180,180,180), sf::Color(0,0,0,60), sf::Color(180, 180, 180), sf::Color(0, 0, 0, 60)));
        EntityAdd(resRadioButtonSetSPtr_);

        resolutionTextRegionSPtr_->SetEntityPos((resRadioButtonSetSPtr_->GetEntityPos().x + (resRadioButtonSetSPtr_->GetEntityRegion().width * 0.5f)) - (resolutionTextRegionSPtr_->GetEntityRegion().width * 0.5f), resolutionTextRegionSPtr_->GetEntityPos().y);

        const float SLIDER_LABEL_OFFSET_X(0.0f);
        const float SLIDER_LABEL_OFFSET_Y(165.0f);

        //Music Volume label and sliderbar
        {
            sf::FloatRect r(BOX_RECT_INNER);
            r.left = resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + MUSIC_VOLUME_HORIZ_POS_;
            r.top -= 5.0f;
            r.width = 0.0f;
            r.height = 0.0f;
            sfml_util::gui::TextInfo musicVolumeTitleTextInfo(settingsTextInfoTitle);
            musicVolumeTitleTextInfo.text = "Music\nVolume";
            musicVolumeTitleTextInfo.justified = sfml_util::Justified::Center;
            musicVolumeTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("MusicVolumeSetting", musicVolumeTitleTextInfo, r) );
            textRegionSVec_.push_back(musicVolumeTextRegionSPtr_);
            //
            sfml_util::gui::MouseTextInfo threeTextInfos("", sfml_util::FontManager::Instance()->Font_NumbersDefault1(), 16, sf::Color::White, sf::Color(255,200,200));
            const float SLIDER_POS_LEFT(musicVolumeTextRegionSPtr_->GetEntityPos().x + (musicVolumeTextRegionSPtr_->GetEntityRegion().width  * 0.5f) - 13.0f);
            const float SLIDER_POS_TOP (musicVolumeTextRegionSPtr_->GetEntityPos().y +  musicVolumeTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            musicVolumeSliderBarSPtr_.reset( new sfml_util::SliderBarLabeled_Music("MusicVolume",
                                                                                   SLIDER_POS_LEFT,
                                                                                   SLIDER_POS_TOP,
                                                                                   160.0f,
                                                                                   sfml_util::gui::SliderStyle(),
                                                                                   threeTextInfos,
                                                                                   0.5f,
                                                                                   SLIDER_LABEL_OFFSET_X,
                                                                                   SLIDER_LABEL_OFFSET_Y) );

            EntityAdd(musicVolumeSliderBarSPtr_);
        }

        //Sound Effects Volume label and sliderbar
        {
            sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);

            sf::FloatRect r(BOX_RECT_INNER);
            r.left = resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + SOUNDEFFECTS_VOLUME_HORIZ_POS_;
            r.top -= 5.0f;
            r.width = 0.0f;
            r.height = 0.0f;
            sfml_util::gui::TextInfo effectsVolumeTitleTextInfo(settingsTextInfoTitle);
            effectsVolumeTitleTextInfo.text = "Sound Effects\nVolume";
            effectsVolumeTitleTextInfo.justified = sfml_util::Justified::Center;
            effectsVolumeTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("EffectsVolumeSetting", effectsVolumeTitleTextInfo, r, sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_, sfml_util::gui::box::Info()) );
            textRegionSVec_.push_back(effectsVolumeTextRegionSPtr_);
            //
            //
            sfml_util::gui::MouseTextInfo threeTextInfos("", sfml_util::FontManager::Instance()->Font_NumbersDefault1(), 16, sf::Color::White, sf::Color(255, 200, 200));
            const float SLIDER_POS_LEFT(effectsVolumeTextRegionSPtr_->GetEntityPos().x + (effectsVolumeTextRegionSPtr_->GetEntityRegion().width  * 0.5f) - 13.0f);
            const float SLIDER_POS_TOP (effectsVolumeTextRegionSPtr_->GetEntityPos().y +  effectsVolumeTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            effectsVolumeSliderBarSPtr_.reset( new sfml_util::SliderBarLabeled_Effects("EffectsVolume",
                                                                                       SLIDER_POS_LEFT,
                                                                                       SLIDER_POS_TOP,
                                                                                       160.0f,
                                                                                       sfml_util::gui::SliderStyle(),
                                                                                       threeTextInfos,
                                                                                       0.5f,
                                                                                       SLIDER_LABEL_OFFSET_X,
                                                                                       SLIDER_LABEL_OFFSET_Y) );
            EntityAdd(effectsVolumeSliderBarSPtr_);
            sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
        }

        //antialias label and RadioButtonSet
        {
            const float AA_LABEL_POS_LEFT(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + 65.0f);
            const float AA_LABEL_POS_TOP(BOX_RECT_INNER.top - 5.0f);
            const sf::FloatRect AA_TEXT_REGION_RECT(AA_LABEL_POS_LEFT, AA_LABEL_POS_TOP, 0.0f, 0.0f);
            sfml_util::gui::TextInfo aaLabelTextInfo(settingsTextInfoTitle);
            aaLabelTextInfo.text = "Anti-\nAliasing";
            aaLabelTextInfo.justified = sfml_util::Justified::Center;
            aaTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("AntiAliasSettingLabel", aaLabelTextInfo, AA_TEXT_REGION_RECT) );
            textRegionSVec_.push_back(aaTextRegionSPtr_);
            //
            const float AA_POS_LEFT(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + ANTIALIAS_HORIZ_POS_);
            const float AA_POS_TOP(aaTextRegionSPtr_->GetEntityPos().y + aaTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            const sfml_util::gui::TextInfo AA_TEXT_INFO(" ", sfml_util::FontManager::Instance()->Font_Default1(), 20);
            const std::vector<std::string> AA_LABEL_VEC = { "0   ", "1   ", "2   ", "4   ", "8   ", "16  " };
            const sfml_util::GradientInfo AA_GRADIENT_INFO(sf::Color(0, 0, 0, 150), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);
            const sfml_util::gui::BackgroundInfo AA_BG_INFO(sf::Color(0, 0, 0, 60), AA_GRADIENT_INFO);
            const sfml_util::gui::box::Info AA_BOX_INFO(1.0f,
                                                        true,
                                                        sf::FloatRect(),
                                                        sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
                                                        AA_BG_INFO);
            //establish which antialiasing level is current
            std::size_t aaInitialSelection(0);
            const unsigned ANTIALIAS_LEVEL(sfml_util::Display::Instance()->AntialiasLevel());
            switch (ANTIALIAS_LEVEL)
            {
                case 0:  { aaInitialSelection = 0; break; }
                case 1:  { aaInitialSelection = 1; break; }
                case 2:  { aaInitialSelection = 2; break; }
                case 4:  { aaInitialSelection = 3; break; }
                case 8:  { aaInitialSelection = 4; break; }
                case 16: { aaInitialSelection = 5; break; }
                default:
                {
                    std::ostringstream ss;
                    ss << "Unsupported Antialiasing level " << ANTIALIAS_LEVEL << " detected.  Will report and use 0 until another valid setting is made.";
                    M_HP_LOG("SettingsStage::Setup()  " << ss.str());
                    aaInitialSelection = 0;
                    break;
                }
            }
            aaRadioButtonSetSPtr_.reset( new sfml_util::gui::RadioButtonSet("Antialias",
                                                                            AA_POS_LEFT,
                                                                            AA_POS_TOP,
                                                                            AA_TEXT_INFO,
                                                                            AA_LABEL_VEC,
                                                                            aaInitialSelection,
                                                                            sfml_util::Brightness::Bright,
                                                                            sfml_util::IndexVec_t(),
                                                                            AA_BOX_INFO) );

            aaRadioButtonSetSPtr_->SetEntityColors(sfml_util::gui::ColorSet(sf::Color(180, 180, 180),
                                                                            sf::Color(0,   0,   0,  60),
                                                                            sf::Color(180, 180, 180),
                                                                            sf::Color(0,   0,   0,  60)));
            aaRadioButtonSetSPtr_->CallbackHandlerAdd(this);
            EntityAdd(aaRadioButtonSetSPtr_);

            //music info text region
            {
                const float MUSICINFO_RIGHT_PAD(30.0f);
                const float MUSICINFO_LABEL_POS_LEFT((aaRadioButtonSetSPtr_->GetEntityRegion().left - MUSICINFO_RIGHT_PAD) - BOX_INNER_PAD_);
                const float MUSICINFO_LABEL_POS_TOP(aaRadioButtonSetSPtr_->GetEntityRegion().top + aaRadioButtonSetSPtr_->GetEntityRegion().height + 30.0f);
                const float MUSICINFO_LABEL_WIDTH((BOX_RECT_INNER.left + BOX_RECT_INNER.width) - MUSICINFO_LABEL_POS_LEFT);
                const float MUSICINFO_LABEL_HEIGHT(0.0f);
                const sf::FloatRect MUSICINFO_TEXT_REGION(MUSICINFO_LABEL_POS_LEFT, MUSICINFO_LABEL_POS_TOP, MUSICINFO_LABEL_WIDTH, MUSICINFO_LABEL_HEIGHT);

                const sfml_util::gui::TextInfo MUSICINFO_LABEL_TEXTINFO("Music Currently Playing",
                                                                        sfml_util::FontManager::Instance()->Font_Typical(),
                                                                        29,
                                                                        sf::Color::White,
                                                                        sfml_util::Justified::Center);

                musicInfoLabelTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("MusicInfoLabel", MUSICINFO_LABEL_TEXTINFO, MUSICINFO_TEXT_REGION) );
                textRegionSVec_.push_back(musicInfoLabelTextRegionSPtr_);

                const sfml_util::MusicOperatorSLst_t MUSIC_OPERAOTR_SLIST(sfml_util::SoundManager::Instance()->MusicOperators());
                std::ostringstream ss;
                for (auto const & NEXT_MUSIC_OPERATOR_SPTR : MUSIC_OPERAOTR_SLIST)
                    ss  << "\"" << NEXT_MUSIC_OPERATOR_SPTR->Info().SongName()
                        << "\"\nby " << NEXT_MUSIC_OPERATOR_SPTR->Info().ArtistName()
                        << "\nLicense: " << NEXT_MUSIC_OPERATOR_SPTR->Info().LicenseTitle()
                        << "\nDuration: " << NEXT_MUSIC_OPERATOR_SPTR->Info().Duration().ToString()
                        << "\n\n";

                const sfml_util::gui::TextInfo MUSICINFO_TEXTINFO(ss.str(),
                                                                  sfml_util::FontManager::Instance()->Font_Default1(),
                                                                  27,
                                                                  sfml_util::FontManager::Color_Light(),
                                                                  sfml_util::Justified::Center);

                const float MUSICINFO_POS_LEFT(aaRadioButtonSetSPtr_->GetEntityRegion().left);
                const float MUSICINFO_POS_TOP(musicInfoLabelTextRegionSPtr_ ->GetEntityRegion().top + musicInfoLabelTextRegionSPtr_->GetEntityRegion().height + 30.0f);
                const float MUSICINFO_WIDTH(((BOX_RECT_INNER.left + BOX_RECT_INNER.width) - MUSICINFO_POS_LEFT) - MUSICINFO_RIGHT_PAD);
                const float MUSICINFO_HEIGHT((BOX_RECT_INNER.top + BOX_RECT_INNER.height) - MUSICINFO_POS_TOP);
                const sf::FloatRect MUSICINFO_REGION(MUSICINFO_POS_LEFT, MUSICINFO_POS_TOP, MUSICINFO_WIDTH, MUSICINFO_HEIGHT);

                const sfml_util::GradientInfo MUSICINFO_GRADIENTINFO(sf::Color(0, 0, 0, 150), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);
                const sfml_util::gui::BackgroundInfo MUSICINFO_BACKGROUNDINFO(sf::Color(0, 0, 0, 60), MUSICINFO_GRADIENTINFO);
                const sfml_util::gui::box::Info MUSICINFO_BOXINFO(1.0f, true, MUSICINFO_REGION, sfml_util::gui::ColorSet(sf::Color(180, 180, 180)), MUSICINFO_BACKGROUNDINFO);

                musicInfoTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SettingsStageMusicInfo",
                                                                               MUSICINFO_TEXTINFO,
                                                                               MUSICINFO_REGION,
                                                                               this,
                                                                               sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                                                                               MUSICINFO_BOXINFO) );

                textRegionSVec_.push_back(musicInfoTextRegionSPtr_);
            }
        }

        //back button
        backButtonSPtr_->SetScaleToRes();
        backButtonSPtr_->SetVertPositionToBottomOfScreenByRes(GetBackButtonPosLeft());
        EntityAdd(backButtonSPtr_);

        //revision number
        std::ostringstream ss;
        ss << "Revision: " << GameDataFile::Instance()->GetCopyStr("system-revision");
        const sfml_util::gui::TextInfo REV_NUM_TEXTINFO(ss.str(),
                                                        sfml_util::FontManager::Instance()->Font_Default1(),
                                                        26,
                                                        sfml_util::FontManager::Instance()->Color_Light(),
                                                        sfml_util::Justified::Left);

        revisionTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("Version", REV_NUM_TEXTINFO, sf::FloatRect()) );
        revisionTextRegionSPtr_->SetEntityPos(BOX_RECT_INNER.left, ((BOX_RECT_INNER.top + BOX_RECT_INNER.height) - revisionTextRegionSPtr_->GetEntityRegion().height));
        EntityAdd(revisionTextRegionSPtr_);
    }


    void SettingsStage::ResetPositions()
    {
        //establish screen positions
        const sf::FloatRect NEW_STAGE_REGION(0.0f, 0.0f, sfml_util::Display::Instance()->GetWinWidth(), sfml_util::Display::Instance()->GetWinHeight());
        StageRegionSet(NEW_STAGE_REGION);
        const float BOX_POS_LEFT((StageRegionWidth() - BOX_WIDTH_) - ((StageRegionWidth() - BOX_WIDTH_) * 0.5f));
        const float BOX_VERT_POS_PAD(sfml_util::MapByRes(35.0f, 135.0f));
        const float BOX_POS_TOP(mainMenuTitle_.Region().top + mainMenuTitle_.Region().height + BOX_VERT_POS_PAD);
        const sf::FloatRect BOX_RECT(BOX_POS_LEFT, BOX_POS_TOP, BOX_WIDTH_, BOX_HEIGHT_);
        const sf::FloatRect BOX_RECT_INNER(BOX_POS_LEFT + BOX_INNER_PAD_, BOX_POS_TOP + BOX_INNER_PAD_, BOX_WIDTH_ - (BOX_INNER_PAD_ * 2.0f), BOX_HEIGHT_ - (BOX_INNER_PAD_ * 2.0f));

        mainMenuTitle_.SetPositionAndSize();

        backgroundImage_.Reset();

        //box
        box_.SetupBox(sfml_util::gui::box::Info(true, BOX_RECT, sfml_util::gui::ColorSet(sf::Color::White), sfml_util::gui::BackgroundInfo(sfml_util::gui::GuiElements::Instance()->GetTextureWood(), BOX_RECT)));

        //establish baseline TextInfo objects
        sfml_util::gui::TextInfo settingsTextInfoTitle("", sfml_util::FontManager::Instance()->Font_Typical(), 30, sf::Color(255, 255, 255, 200));

        //change resolution label
        resolutionTextRegionSPtr_->SetEntityPos(BOX_RECT_INNER.left + 50.0f, BOX_RECT_INNER.top - 10.0f);

        //resolution radio button
        const float RES_RADIOBUTTONSET_POS_LEFT(BOX_RECT_INNER.left);
        const float RES_RADIOBUTTONSET_POS_TOP(BOX_RECT.top + 50.0f);
        resRadioButtonSetSPtr_->SetEntityPos(RES_RADIOBUTTONSET_POS_LEFT, RES_RADIOBUTTONSET_POS_TOP);

        //Music Volume label and sliderbar
        {
            musicVolumeTextRegionSPtr_->SetEntityPos(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + MUSIC_VOLUME_HORIZ_POS_, BOX_RECT_INNER.top - 5.0f);
            //
            const float SLIDER_POS_LEFT(musicVolumeTextRegionSPtr_->GetEntityPos().x + (musicVolumeTextRegionSPtr_->GetEntityRegion().width * 0.5f) - 13.0f);
            const float SLIDER_POS_TOP(musicVolumeTextRegionSPtr_->GetEntityPos().y + musicVolumeTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            musicVolumeSliderBarSPtr_->SetEntityPos(SLIDER_POS_LEFT, SLIDER_POS_TOP);
        }

        //Sound Effects Volume label and sliderbar
        {
            sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);
            effectsVolumeTextRegionSPtr_->SetEntityPos(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + SOUNDEFFECTS_VOLUME_HORIZ_POS_, BOX_RECT_INNER.top - 5.0f);
            //
            const float SLIDER_POS_LEFT(effectsVolumeTextRegionSPtr_->GetEntityPos().x + (effectsVolumeTextRegionSPtr_->GetEntityRegion().width * 0.5f) - 13.0f);
            const float SLIDER_POS_TOP(effectsVolumeTextRegionSPtr_->GetEntityPos().y + effectsVolumeTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            effectsVolumeSliderBarSPtr_->SetEntityPos(SLIDER_POS_LEFT, SLIDER_POS_TOP);
            sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
        }

        //antialias label and RadioButtonSet
        {
            const float AA_LABEL_POS_LEFT(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + 65.0f);
            const float AA_LABEL_POS_TOP(BOX_RECT_INNER.top - 5.0f);
            aaTextRegionSPtr_->SetEntityPos(AA_LABEL_POS_LEFT, AA_LABEL_POS_TOP);
            //
            const float AA_POS_LEFT(resRadioButtonSetSPtr_->GetEntityRegion().left + resRadioButtonSetSPtr_->GetEntityRegion().width + ANTIALIAS_HORIZ_POS_ - 15.0f);
            const float AA_POS_TOP(aaTextRegionSPtr_->GetEntityPos().y + aaTextRegionSPtr_->GetEntityRegion().height + 5.0f);
            aaRadioButtonSetSPtr_->SetEntityPos(AA_POS_LEFT, AA_POS_TOP);
        }

        //music info
        {
            const float MUSICINFO_RIGHT_PAD(30.0f);
            const float MUSICINFO_LABEL_POS_LEFT((aaRadioButtonSetSPtr_->GetEntityRegion().left - MUSICINFO_RIGHT_PAD) - BOX_INNER_PAD_);
            const float MUSICINFO_LABEL_POS_TOP(aaRadioButtonSetSPtr_->GetEntityRegion().top + aaRadioButtonSetSPtr_->GetEntityRegion().height + 30.0f);
            musicInfoLabelTextRegionSPtr_->SetEntityPos(MUSICINFO_LABEL_POS_LEFT, MUSICINFO_LABEL_POS_TOP);

            const float MUSICINFO_POS_LEFT(aaRadioButtonSetSPtr_->GetEntityRegion().left);
            const float MUSICINFO_POS_TOP(musicInfoLabelTextRegionSPtr_->GetEntityRegion().top + musicInfoLabelTextRegionSPtr_->GetEntityRegion().height + 30.0f);
            musicInfoTextRegionSPtr_->SetEntityPos(MUSICINFO_POS_LEFT, MUSICINFO_POS_TOP);
        }

        //back button
        backButtonSPtr_->SetScaleToRes();
        backButtonSPtr_->SetVertPositionToBottomOfScreenByRes(GetBackButtonPosLeft());

        //revision number
        revisionTextRegionSPtr_->SetEntityPos(BOX_RECT_INNER.left, ((BOX_RECT_INNER.top + BOX_RECT_INNER.height) - revisionTextRegionSPtr_->GetEntityRegion().height));

        //symbols at the bottom of the screen
        bottomSymbol_.Setup(1.0f, false, StageRegionWidth(), StageRegionHeight());
    }


    void SettingsStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        target.draw(backgroundImage_, states);
        target.draw(mainMenuTitle_, states);
        bottomSymbol_.Draw(target, states);
        target.draw(box_, states);
        Stage::Draw(target, states);

        for (auto const & NEXT_TEXTREGION_SPTR : textRegionSVec_)
            NEXT_TEXTREGION_SPTR->draw(target, states);
    }


    void SettingsStage::HandleResolutionChange()
    {
        ResetPositions();
    }


    bool SettingsStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->StaticSounds_Switch()->PlayRandom();
            LoopManager::Instance()->Goto_Previous();
            return true;
        }
        else
            return false;
    }


    float SettingsStage::GetBackButtonPosLeft() const
    {
        return sfml_util::MapByRes(50.0f, 300.0f);
    }

}
}
