//
// sfml-util.cpp
//
#include "sfml-util.hpp"
#include "utilz/platform.hpp"
#include "sfml-util/display.hpp"

#include <tuple>

//prevent boost warnings that can be ignored
#include "utilz/platform.hpp"
#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/math/constants/constants.hpp> //for boost::math::constants::pi<double>() etc.

#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wundef"
#endif



namespace sfml_util
{

    float GetAngleInDegrees(const sf::Vector2f & BEGIN_POS_V, const sf::Vector2f & END_POS_V)
    {
        auto const ANGLE_RADIANS{ std::atan((std::max(BEGIN_POS_V.y, END_POS_V.y) - std::min(BEGIN_POS_V.y, END_POS_V.y)) / (std::max(BEGIN_POS_V.x, END_POS_V.x) - std::min(BEGIN_POS_V.x, END_POS_V.x))) };
        auto const ANGLE_DEGREES{ (ANGLE_RADIANS * 180.0f) / boost::math::constants::pi<float>() };

        if (BEGIN_POS_V.x < END_POS_V.x)
        {
            if (BEGIN_POS_V.y > END_POS_V.y)
            {
                return ANGLE_DEGREES * -1.0f; //QI
            }
            else
            {
                return ANGLE_DEGREES; //QIV
            }
        }
        else
        {
            if (BEGIN_POS_V.y > END_POS_V.y)
            {
                return (180.0f - ANGLE_DEGREES) * -1.0f; //QII
            }
            else
            {
                return (180.0f + ANGLE_DEGREES) * -1.0f; //QIII
            }
        }
    }


    const sf::Vector2f ProjectToScreenEdge(const sf::Vector2f & FIRST_POS_V,
                                           const sf::Vector2f & SECOND_POS_V,
                                           const sf::Vector2f & PROJECTED_IMAGE_SIZE)
    {
        sf::Vector2f finalPosV{0.0f, 0.0f};

        auto const SCREEN_WIDTH{ Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ Display::Instance()->GetWinHeight() };

        //use linear relations to find the edge-of-screen miss point, first assuming the vertical position is the vertical screen edge...
        auto const SPRITE_HEIGHT{ PROJECTED_IMAGE_SIZE.y };
        finalPosV.y = ((FIRST_POS_V.y < SECOND_POS_V.y) ? (SCREEN_HEIGHT + SPRITE_HEIGHT + 1.0f) : ((-1.0f * SPRITE_HEIGHT) - 1.0f));

        auto const AV_HORIZ_OVER_VERT{ (std::max(FIRST_POS_V.x, SECOND_POS_V.x) - std::min(FIRST_POS_V.x, SECOND_POS_V.x)) /
            (std::max(FIRST_POS_V.y, SECOND_POS_V.y) - std::min(FIRST_POS_V.y, SECOND_POS_V.y)) };

        auto const AV_VERTICAL_EXTENT{ ((SECOND_POS_V.y > FIRST_POS_V.y) ? (SCREEN_HEIGHT - FIRST_POS_V.y) : FIRST_POS_V.y) };

        auto const AV_HORIZ_EXTENT{ AV_HORIZ_OVER_VERT * AV_VERTICAL_EXTENT };

        if (SECOND_POS_V.x > FIRST_POS_V.x)
        {
            finalPosV.x = FIRST_POS_V.x + AV_HORIZ_EXTENT;
        }
        else
        {
            finalPosV.x = FIRST_POS_V.x - AV_HORIZ_EXTENT;
        }

        //...but if that is too far, then assume the horizontal screen edge
        if (std::abs(finalPosV.x - FIRST_POS_V.x) > SCREEN_WIDTH)
        {
            auto const SPRITE_WIDTH{ PROJECTED_IMAGE_SIZE.x };
            finalPosV.x = ((FIRST_POS_V.x < SECOND_POS_V.x) ? (SCREEN_WIDTH + SPRITE_WIDTH + 1.0f) : (0.0f - SPRITE_WIDTH - 1.0f));

            auto const AH_VERT_OVER_HORIZ{ (std::max(FIRST_POS_V.y, SECOND_POS_V.y) - std::min(FIRST_POS_V.y, SECOND_POS_V.y)) /
                (std::max(FIRST_POS_V.x, SECOND_POS_V.x) - std::min(FIRST_POS_V.x, SECOND_POS_V.x)) };

            auto const AH_HORIZ_EXTENT{ ((SECOND_POS_V.x > FIRST_POS_V.x) ? (SCREEN_WIDTH - FIRST_POS_V.x) : FIRST_POS_V.x) };

            auto const AH_VERT_EXTENT{ AH_VERT_OVER_HORIZ * AH_HORIZ_EXTENT };

            if (SECOND_POS_V.y > FIRST_POS_V.y)
            {
                finalPosV.y = FIRST_POS_V.y + AH_VERT_EXTENT;
            }
            else
            {
                finalPosV.y = FIRST_POS_V.y - AH_VERT_EXTENT;
            }
        }

        return finalPosV;
    }


    const std::string KeyCodeToString(const sf::Keyboard::Key KEY)
    {
        switch (KEY)
        {
            case sf::Keyboard::A:        { return "A"; }
            case sf::Keyboard::B:        { return "B"; }
            case sf::Keyboard::C:        { return "C"; }
            case sf::Keyboard::D:        { return "D"; }
            case sf::Keyboard::E:        { return "E"; }
            case sf::Keyboard::F:        { return "F"; }
            case sf::Keyboard::G:        { return "G"; }
            case sf::Keyboard::H:        { return "H"; }
            case sf::Keyboard::I:        { return "I"; }
            case sf::Keyboard::J:        { return "J"; }
            case sf::Keyboard::K:        { return "K"; }
            case sf::Keyboard::L:        { return "L"; }
            case sf::Keyboard::M:        { return "M"; }
            case sf::Keyboard::N:        { return "N"; }
            case sf::Keyboard::O:        { return "O"; }
            case sf::Keyboard::P:        { return "P"; }
            case sf::Keyboard::Q:        { return "Q"; }
            case sf::Keyboard::R:        { return "R"; }
            case sf::Keyboard::S:        { return "S"; }
            case sf::Keyboard::T:        { return "T"; }
            case sf::Keyboard::U:        { return "U"; }
            case sf::Keyboard::V:        { return "V"; }
            case sf::Keyboard::W:        { return "W"; }
            case sf::Keyboard::X:        { return "X"; }
            case sf::Keyboard::Y:        { return "Y"; }
            case sf::Keyboard::Z:        { return "Z"; }
            case sf::Keyboard::Num0:     { return "Num0"; }
            case sf::Keyboard::Num1:     { return "Num1"; }
            case sf::Keyboard::Num2:     { return "Num2"; }
            case sf::Keyboard::Num3:     { return "Num3"; }
            case sf::Keyboard::Num4:     { return "Num4"; }
            case sf::Keyboard::Num5:     { return "Num5"; }
            case sf::Keyboard::Num6:     { return "Num6"; }
            case sf::Keyboard::Num7:     { return "Num7"; }
            case sf::Keyboard::Num8:     { return "Num8"; }
            case sf::Keyboard::Num9:     { return "Num9"; }
            case sf::Keyboard::Escape:   { return "Escape"; }
            case sf::Keyboard::LControl: { return "LControl"; }
            case sf::Keyboard::LShift:   { return "LShift"; }
            case sf::Keyboard::LAlt:     { return "LAlt"; }
            case sf::Keyboard::LSystem:  { return "LSystem:"; }
            case sf::Keyboard::RControl: { return "RControl"; }
            case sf::Keyboard::RShift:   { return "RShift"; }
            case sf::Keyboard::RAlt:     { return "RAlt"; }
            case sf::Keyboard::RSystem:  { return "RSystem"; }
            case sf::Keyboard::Menu:     { return "Menu"; }
            case sf::Keyboard::LBracket: { return "LBracket"; }
            case sf::Keyboard::RBracket: { return "RBracket"; }
            case sf::Keyboard::SemiColon:{ return "SemiColon"; }
            case sf::Keyboard::Comma:    { return "Comma"; }
            case sf::Keyboard::Period:   { return "Period"; }
            case sf::Keyboard::Quote:    { return "Quote"; }
            case sf::Keyboard::Slash:    { return "Slash"; }
            case sf::Keyboard::BackSlash:{ return "BackSlash"; }
            case sf::Keyboard::Tilde:    { return "Tilde"; }
            case sf::Keyboard::Equal:    { return "Equal"; }
            case sf::Keyboard::Dash:     { return "Dash"; }
            case sf::Keyboard::Space:    { return "Space"; }
            case sf::Keyboard::Return:   { return "Return"; }
            case sf::Keyboard::BackSpace:{ return "BackSpace"; }
            case sf::Keyboard::Tab:      { return "Tab"; }
            case sf::Keyboard::PageUp:   { return "PageUp"; }
            case sf::Keyboard::PageDown: { return "PageDown"; }
            case sf::Keyboard::End:      { return "End"; }
            case sf::Keyboard::Home:     { return "Home"; }
            case sf::Keyboard::Insert:   { return "Insert"; }
            case sf::Keyboard::Delete:   { return "Delete"; }
            case sf::Keyboard::Add:      { return "Add"; }
            case sf::Keyboard::Subtract: { return "Subtract"; }
            case sf::Keyboard::Multiply: { return "Multiply"; }
            case sf::Keyboard::Divide:   { return "Divide"; }
            case sf::Keyboard::Left:     { return "Left"; }
            case sf::Keyboard::Right:    { return "Right"; }
            case sf::Keyboard::Up:       { return "Up"; }
            case sf::Keyboard::Down:     { return "Down"; }
            case sf::Keyboard::Numpad0:  { return "Numpad0"; }
            case sf::Keyboard::Numpad1:  { return "Numpad1"; }
            case sf::Keyboard::Numpad2:  { return "Numpad2"; }
            case sf::Keyboard::Numpad3:  { return "Numpad3"; }
            case sf::Keyboard::Numpad4:  { return "Numpad4"; }
            case sf::Keyboard::Numpad5:  { return "Numpad5"; }
            case sf::Keyboard::Numpad6:  { return "Numpad6"; }
            case sf::Keyboard::Numpad7:  { return "Numpad7"; }
            case sf::Keyboard::Numpad8:  { return "Numpad8"; }
            case sf::Keyboard::Numpad9:  { return "Numpad9"; }
            case sf::Keyboard::F1:       { return "F1"; }
            case sf::Keyboard::F2:       { return "F2"; }
            case sf::Keyboard::F3:       { return "F3"; }
            case sf::Keyboard::F4:       { return "F4"; }
            case sf::Keyboard::F5:       { return "F5"; }
            case sf::Keyboard::F6:       { return "F6"; }
            case sf::Keyboard::F7:       { return "F7"; }
            case sf::Keyboard::F8:       { return "F8"; }
            case sf::Keyboard::F9:       { return "F9"; }
            case sf::Keyboard::F10:      { return "F10"; }
            case sf::Keyboard::F11:      { return "F11"; }
            case sf::Keyboard::F12:      { return "F12"; }
            case sf::Keyboard::F13:      { return "F13"; }
            case sf::Keyboard::F14:      { return "F14"; }
            case sf::Keyboard::F15:      { return "F15"; }
            case sf::Keyboard::Pause:    { return "Pause"; }
            case sf::Keyboard::Unknown:
            case sf::Keyboard::KeyCount:
            default: { return "UnknownKeyCodeError"; }
        }
    }


    void GetResolutionAreas(float & resAreaCurrent, float & resAreaMin, float & resAreaMax)
    {
        resAreaCurrent = (sfml_util::Display::Instance()->GetWinWidth() * sfml_util::Display::Instance()->GetWinHeight());
        resAreaMin = (sfml_util::Display::Instance()->GetWinWidthMin() * sfml_util::Display::Instance()->GetWinHeightMin());
        resAreaMax = (sfml_util::Display::Instance()->GetWinWidthMax() * sfml_util::Display::Instance()->GetWinHeightMax()) * 0.5f;
    }


    const std::string VideoModeToString(const sf::VideoMode & VM, const bool WILL_WRAP)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "[" : "") << VM.width << "x" << VM.height << ":" << VM.bitsPerPixel << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }


    void SetTextColor(sf::Text & text, const sf::Color & COLOR)
    {
#ifdef SFMLUTIL_PLATFORMDETECT__LINUX
        text.setColor(COLOR);
#else
        text.setFillColor(COLOR);
#endif
    }


    void FlipHoriz(sf::Texture & texture)
    {
        sf::Image tempImage(texture.copyToImage());
        tempImage.flipHorizontally();
        texture.loadFromImage(tempImage);
    }


    TextureSPtr_t FlipHorizCopy(const sf::Texture & TEXTURE)
    {
        sf::Image tempImage(TEXTURE.copyToImage());
        tempImage.flipHorizontally();
        TextureSPtr_t newTextureSPtr( new sf::Texture );
        newTextureSPtr->loadFromImage(tempImage);
        return newTextureSPtr;
    }


    void FlipVert(sf::Texture & texture)
    {
        sf::Image tempImage(texture.copyToImage());
        tempImage.flipVertically();
        texture.loadFromImage(tempImage);
    }


    TextureSPtr_t FlipVertCopy(const sf::Texture & TEXTURE)
    {
        sf::Image tempImage(TEXTURE.copyToImage());
        tempImage.flipVertically();
        TextureSPtr_t newTextureSPtr( new sf::Texture );
        newTextureSPtr->loadFromImage(tempImage);
        return newTextureSPtr;
    }


    void Invert(sf::Texture & texture, const bool WILL_INVERT_ALPHA)
    {
        sf::Image image(texture.copyToImage());

        for (unsigned int y(0); y < image.getSize().y; ++y)
        {
            for (unsigned int x(0); x < image.getSize().x; ++x)
            {
                sf::Color color(image.getPixel(x, y));
                color.r = static_cast<sf::Uint8>(255 - static_cast<int>(color.r));
                color.g = static_cast<sf::Uint8>(255 - static_cast<int>(color.g));
                color.b = static_cast<sf::Uint8>(255 - static_cast<int>(color.b));

                if (WILL_INVERT_ALPHA)
                    color.a = 255 - color.a;

                image.setPixel(x, y, color);
            }
        }

        texture.loadFromImage(image);
    }


    void Mask(sf::Texture &     texture,
              const sf::Color & COLOR_TO_MASK,
              const sf::Uint8   NEW_ALPHA)
    {
        sf::Image image(texture.copyToImage());
        image.createMaskFromColor(COLOR_TO_MASK, NEW_ALPHA);
        texture.loadFromImage(image);
    }


    void SpriteToRenderTexture(sf::Sprite &             sprite,
                               sf::RenderTexture &      renderTexture,
                               const sf::RenderStates & RENDER_STATES,
                               const bool               WILL_DISPLAY)
    {
        renderTexture.create(static_cast<unsigned int>(sprite.getGlobalBounds().width), static_cast<unsigned int>(sprite.getGlobalBounds().height));
        const sf::Vector2f ORIG_POS(sprite.getPosition());
        sprite.setPosition(0.0f, 0.0f);
        renderTexture.draw(sprite, RENDER_STATES);

        if (WILL_DISPLAY)
            renderTexture.display();

        sprite.setPosition(ORIG_POS);
    }


    void SpriteToImage(sf::Sprite & sprite, sf::Image & image, const sf::RenderStates & RENDER_STATES)
    {
        sf::RenderTexture renderTexture;
        SpriteToRenderTexture(sprite, renderTexture, RENDER_STATES, true);
        image = renderTexture.getTexture().copyToImage();
    }


    namespace color
    {
        bool ColorLess(const sf::Color & L, const sf::Color & R)
        {
            const sf::Uint8 LR(L.r);
            const sf::Uint8 LG(L.g);
            const sf::Uint8 LB(L.b);
            const sf::Uint8 LA(L.a);

            const sf::Uint8 RR(R.r);
            const sf::Uint8 RG(R.g);
            const sf::Uint8 RB(R.b);
            const sf::Uint8 RA(R.a);

            return std::tie(LR, LG, LB, LA)
                <
                std::tie(RR, RG, RB, RA);
        }


        bool BlendModeLess(const sf::BlendMode & L, const sf::BlendMode & R)
        {
            const sf::BlendMode::Factor   L_ADF(L.alphaDstFactor);
            const sf::BlendMode::Equation L_AEQ(L.alphaEquation);
            const sf::BlendMode::Factor   L_ASF(L.alphaSrcFactor);

            const sf::BlendMode::Factor   R_ADF(R.alphaDstFactor);
            const sf::BlendMode::Equation R_AEQ(R.alphaEquation);
            const sf::BlendMode::Factor   R_ASF(R.alphaSrcFactor);

            return std::tie(L_ADF, L_AEQ, L_ASF) < std::tie(R_ADF, R_AEQ, R_ASF);
        }


        const sf::Color TransitionColor(const sf::Color & FROM, const sf::Color & TO, const float RATIO_COMPLETE)
        {
            auto const RED  { static_cast<sf::Uint8>(FROM.r + static_cast<sf::Uint8>((static_cast<float>(TO.r) - static_cast<float>(FROM.r)) * RATIO_COMPLETE)) };
            auto const GREEN{ static_cast<sf::Uint8>(FROM.g + static_cast<sf::Uint8>((static_cast<float>(TO.g) - static_cast<float>(FROM.g)) * RATIO_COMPLETE)) };
            auto const BLUE { static_cast<sf::Uint8>(FROM.b + static_cast<sf::Uint8>((static_cast<float>(TO.b) - static_cast<float>(FROM.b)) * RATIO_COMPLETE)) };
            auto const ALPHA{ static_cast<sf::Uint8>(FROM.a + static_cast<sf::Uint8>((static_cast<float>(TO.a) - static_cast<float>(FROM.a)) * RATIO_COMPLETE)) };
            return sf::Color(RED, GREEN, BLUE, ALPHA);
        }


        const std::string ColorToString(const sf::Color & C, const bool WILL_WRAP)
        {
            if (sf::Color::Black == C)
                return "Black";

            if (sf::Color::White == C)
                return "White";

            if (sf::Color::Red == C)
                return "Red";

            if (sf::Color::Green == C)
                return "Green";

            if (sf::Color::Blue == C)
                return "Blue";

            if (sf::Color::Yellow == C)
                return "Yellow";

            if (sf::Color::Magenta == C)
                return "Magenta";

            if (sf::Color::Cyan == C)
                return "Cyan";

            std::ostringstream ss;
            ss << ((WILL_WRAP) ? "(" : "")
                << static_cast<int>(C.r)
                << ", "
                << static_cast<int>(C.g)
                << ", "
                << static_cast<int>(C.b);

            if (C.a != 255)
                ss << ", " << static_cast<int>(C.a);

            ss << ((WILL_WRAP) ? ")" : "");

            return ss.str();
        }
    }

}//end of namespace sfml_util
