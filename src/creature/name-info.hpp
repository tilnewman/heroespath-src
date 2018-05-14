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
#ifndef HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
#define HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
//
// name-info.hpp
//
#include "sfml-util/gui/text-info.hpp"

#include <string>
#include <utility>

namespace heroespath
{
namespace creature
{

    // Responsible for storing and calculating values regarding creature names and their display
    // characteristics.
    class NameInfo
    {
        using FontSizePair_t = std::pair<sfml_util::FontPtr_t, unsigned int>;
        using FontSizeToWidthMap_t = misc::VectorMap<FontSizePair_t, float>;

    public:
        NameInfo(const NameInfo &) = delete;
        NameInfo(NameInfo &&) = delete;
        NameInfo & operator=(const NameInfo &) = delete;
        NameInfo & operator=(NameInfo &&) = delete;

        NameInfo() = default;

        std::size_t MaxCharacterCount() const { return 13; }

        float DefaultTextEntryBoxWidth() const
        {
            // The +1 is to accomodate the TextEntryBox's padding and margins
            return Length(
                std::string(MaxCharacterCount() + 1, LargestLetter()),
                DefaultFont(),
                DefaultSize());
        }

        const sfml_util::FontPtr_t DefaultFont() const;

        unsigned int DefaultSize() const;

        char LargestLetter() const { return 'M'; }

        const std::string LargestName() const
        {
            return std::string(MaxCharacterCount(), LargestLetter());
        }

        const std::string LargestLetterString() const { return std::string(1, LargestLetter()); }

        const sfml_util::gui::TextInfo MakeTextInfo(
            const std::string & TEXT = " ",
            const sf::Color & COLOR = sf::Color::White,
            const sfml_util::Justified::Enum JUSTIFICATION = sfml_util::Justified::Left) const
        {
            return sfml_util::gui::TextInfo(
                TEXT, DefaultFont(), DefaultSize(), COLOR, JUSTIFICATION);
        }

        float Length(
            const std::string & TEXT,
            const sfml_util::FontPtr_t FONT_PTR,
            const unsigned int CHAR_SIZE) const;

        float Length(const sfml_util::gui::TextInfo & TEXT_INFO) const;

        float Length() { return Length(MakeTextInfo(LargestName())); }
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
