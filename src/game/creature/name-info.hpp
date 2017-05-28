///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef GAME_CREATURE_NAME_INFO_INCLUDED
#define GAME_CREATURE_NAME_INFO_INCLUDED
//
// name-info.hpp
//  Information about creature names and thier displayed characteristics.
//  Often in the code base the max length of a character name is needed for GUI bounds/drawing,
//  and this class answers that question.
//
#include "sfml-util/gui/text-info.hpp"

#include <map>
#include <memory>
#include <string>
#include <utility>


namespace game
{
namespace creature
{

    //responsibel for data regarding creature names and their display characteristics
    class NameInfo
    {
        //types used by NameInfo
        using FontSizePair_t = std::pair<sfml_util::FontPtr_t, unsigned int>;
        using FontSizeToWidthMap_t = std::map<FontSizePair_t, float>;

        //prevent copy construction
        NameInfo(const NameInfo &) =delete;

        //prevent copy assignment
        NameInfo & operator=(const NameInfo &) =delete;

        //prevent non-singleton construction
        NameInfo();

    public:
        ~NameInfo();

        static NameInfo * Instance();
        static void Acquire();
        static void Release();

        inline std::size_t MaxCharacterCount() const            { return 13; }

        inline float TextEntryBoxWidth(const sfml_util::FontPtr_t FONT_PTR  = NameInfo::Instance()->DefaultFont(),
                                       const unsigned int         CHAR_SIZE = NameInfo::Instance()->DefaultSize()) const { return Length(std::string(MaxCharacterCount() + 1, LargestLetter()), FONT_PTR, CHAR_SIZE); }//The +1 is to accomodate the TextEntryBox's padding and margins

        sfml_util::FontPtr_t DefaultFont() const;

        unsigned int DefaultSize() const;

        inline char LargestLetter() const                       { return 'M'; }

        inline const std::string LargestName() const            { return std::string(MaxCharacterCount(), LargestLetter()); }

        inline const std::string LargestLetterString() const    { return std::string(1, LargestLetter()); }

        inline const sfml_util::gui::TextInfo MakeTextInfo(const std::string &              TEXT = " ",
                                                           const sf::Color &                COLOR = sf::Color::White,
                                                           const sfml_util::Justified::Enum JUSTIFICATION = sfml_util::Justified::Left) const { return sfml_util::gui::TextInfo(TEXT, NameInfo::Instance()->DefaultFont(), NameInfo::Instance()->DefaultSize(), COLOR, JUSTIFICATION); }

        float LengthMax(const sfml_util::FontPtr_t FONT_PTR  = NameInfo::Instance()->DefaultFont(),
                        const unsigned int         CHAR_SIZE = NameInfo::Instance()->DefaultSize());

        float Length(const std::string & TEXT, const sfml_util::FontPtr_t FONT_PTR, const unsigned int CHAR_SIZE) const;

        float Length(const sfml_util::gui::TextInfo & TEXT_INFO) const;

        inline float Length() { return Length(MakeTextInfo(LargestName())); }

    private:
        static std::unique_ptr<NameInfo> instanceUPtr_;
        FontSizeToWidthMap_t sizeMap_;
    };

}
}

#endif //GAME_CREATURE_NAME_INFO_INCLUDED
