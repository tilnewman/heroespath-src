#ifndef SFMLUTIL_POPUPBUTTONS_INCLUDED
#define SFMLUTIL_POPUPBUTTONS_INCLUDED
//
// popup-buttons.hpp
//  Popup buttons types.
//
#include "sfml-util/gui/text-button.hpp"
#include "game/popup-info.hpp"
#include <memory>


namespace sfml_util
{

    class PopupButton_Yes : public gui::TextButton
    {
    public:
        PopupButton_Yes(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_Yes() {}
    protected:
        virtual void OnClick(const sf::Vector2f &);
    };

    using PopupButton_YesSPtr_t = std::shared_ptr<PopupButton_Yes>;


    class PopupButton_No : public gui::TextButton
    {
    public:
        PopupButton_No(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_No() {}
    protected:
        virtual void OnClick(const sf::Vector2f &);
    };

    using PopupButton_NoSPtr_t = std::shared_ptr<PopupButton_No>;


    class PopupButton_Cancel : public gui::TextButton
    {
    public:
        PopupButton_Cancel(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_Cancel() {}
        virtual void OnClick(const sf::Vector2f &);
    };

    using PopupButton_CancelSPtr_t = std::shared_ptr<PopupButton_Cancel>;


    class PopupButton_Continue : public gui::TextButton
    {
    public:
        PopupButton_Continue(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_Continue() {}
        virtual void OnClick(const sf::Vector2f &);
    };

    using PopupButton_ContinueSPtr_t = std::shared_ptr<PopupButton_Continue>;


    class PopupButton_Okay : public gui::TextButton
    {
    public:
        PopupButton_Okay(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_Okay() {}
        virtual void OnClick(const sf::Vector2f &);
    };

    using PopupButton_OkaySPtr_t = std::shared_ptr<PopupButton_Okay>;


    class PopupButton_Select : public gui::TextButton
    {
    public:
        PopupButton_Select(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP);
        virtual ~PopupButton_Select() {}
        virtual void OnClick(const sf::Vector2f &);
        void SetSelection(const int SELECTION)  { selection_ = SELECTION; }
        int GetSelection() const                { return selection_; }
    private:
        int selection_;
    };

    using PopupButton_SelectSPtr_t = std::shared_ptr<PopupButton_Select>;

}
#endif //SFMLUTIL_POPUPBUTTONS_INCLUDED
