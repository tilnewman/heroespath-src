//
// main-menu-buttons.cpp
//
#include "main-menu-buttons.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"


namespace game
{
namespace main_menu_buttons
{

    CreateCharactersButton::CreateCharactersButton( const float POS_LEFT,
                                                    const float POS_TOP,
                                                    const float SCALE,
                                                    const bool WILL_BOX )
    :
        FourStateButton("CreateCharacters")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup( POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("create_char_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("create_char_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void CreateCharactersButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_CharacterCreation();
    }


    ResumeButton::ResumeButton(const float POS_LEFT,
                               const float POS_TOP,
                               const float SCALE,
                               const bool  WILL_BOX)
    :
        FourStateButton("Resume")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                               POS_TOP,
                               std::string(BUTTONS_PATH).append("resume_button_normal.png"),
                               "",
                               std::string(BUTTONS_PATH).append("resume_button_lit.png"),
                               std::string(BUTTONS_PATH).append("resume_button_faded.png"),
                               sfml_util::gui::MouseTextInfo(),
                               sfml_util::gui::TextInfo(),
                               WILL_BOX,
                               SCALE);
    }

    void ResumeButton::OnClick(const sf::Vector2f & MOUSE_POS_V)
    {
        FourStateButton::OnClick(MOUSE_POS_V);
    }


    StartGameButton::StartGameButton(const float POS_LEFT,
                                     const float POS_TOP,
                                     const float SCALE,
                                     const bool  WILL_BOX)
    :
        FourStateButton("StartGame")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("startgame_button_faded.png"),
                                "",
                                "",
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void StartGameButton::OnClick(const sf::Vector2f &)
    {}


    SettingsButton::SettingsButton(const float POS_LEFT,
                                   const float POS_TOP,
                                   const float SCALE,
                                   const bool  WILL_BOX)
    :
        FourStateButton("Settings")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("settings_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("settings_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void SettingsButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Settings();
    }


    CreditsButton::CreditsButton(const float POS_LEFT,
                                 const float POS_TOP,
                                 const float SCALE,
                                 const bool  WILL_BOX)
    :
        FourStateButton("Credits")
    {
        const std::string BUTTONS_PATH(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"));

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("credits_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("credits_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void CreditsButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Credits();
    }


    ExitButton::ExitButton(const float POS_LEFT,
                           const float POS_TOP,
                           const float SCALE,
                           const bool  WILL_BOX)
    :
        FourStateButton("Exit")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("exit_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("exit_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void ExitButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Exit();
    }



    BackButton::BackButton(const float POS_LEFT,
                           const float POS_TOP,
                           const float SCALE,
                           const bool  WILL_BOX)
    :
        FourStateButton("Back")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("back_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("back_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void BackButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Previous();
    }


    DeleteButton::DeleteButton(const float POS_LEFT,
                               const float POS_TOP,
                               const float SCALE,
                               const bool  WILL_BOX)
    :
        FourStateButton("Delete")
    {
        const std::string BUTTONS_PATH( GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir") );

        FourStateButton::Setup(POS_LEFT,
                                POS_TOP,
                                std::string(BUTTONS_PATH).append("delete_button_normal.png"),
                                "",
                                std::string(BUTTONS_PATH).append("delete_button_lit.png"),
                                "",
                                sfml_util::gui::MouseTextInfo(),
                                sfml_util::gui::TextInfo(),
                                WILL_BOX,
                                SCALE);
    }

    void DeleteButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Previous();
    }

}
}
