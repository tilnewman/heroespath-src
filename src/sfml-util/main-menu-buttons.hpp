// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
//
// main-menu-buttons.hpp
//
#include "sfml-util/image-text-entity.hpp"
#include "stage/stage-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for implementing an ImageTextEntity that behaves like a one of the standard
    // menu stage buttons such as Back/Next/etc.
    class MainMenuButton : public sfml_util::ImageTextEntity
    {
    public:
        MainMenuButton(const MainMenuButton &) = delete;
        MainMenuButton(MainMenuButton &&) = delete;
        MainMenuButton & operator=(const MainMenuButton &) = delete;
        MainMenuButton & operator=(MainMenuButton &&) = delete;

        // if FORCED_IMAGE_WIDTH=0 it is ignored, if <0 then the default is used
        explicit MainMenuButton(
            const stage::Stage::Enum TRANSITION_TO = stage::Stage::Count,
            const ImageTextEntity::Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT
            = boost::none,
            const float FORCED_IMAGE_WIDTH = 0.0f,
            const sf::Vector2f & POS_V = sf::Vector2f(0.0f, 0.0f));

        virtual ~MainMenuButton() = default;

        static float DefaultWidth();

        void PretendClicked(const sf::Vector2f & FAKE_MOUSE_POS_V = sf::Vector2f(-1.0f, -1.0f))
        {
            OnClick(FAKE_MOUSE_POS_V);
        }

    protected:
        void OnClick(const sf::Vector2f &) override;

        static const MouseImageInfo MakeMouseImageInfo(
            const stage::Stage::Enum TRANSITION_TO,
            const sf::Vector2f & POS_V,
            const float FORCED_IMAGE_WIDTH);

    private:
        static const float SCREEN_SIZE_RATIO_WIDTH_DEFAULT_;

        stage::Stage::Enum transitionTo_;
    };

    using MainMenuButtonUPtr_t = std::unique_ptr<MainMenuButton>;
    using MainMenuButtonUVec_t = std::vector<MainMenuButtonUPtr_t>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
