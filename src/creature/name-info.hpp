// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
#define HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
//
// name-info.hpp
//
#include "misc/vector-map.hpp"
#include "sfml-util/font-enum.hpp"
#include "sfml-util/text-info.hpp"
#include "sfutil/size-and-scale.hpp"

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
        using FontSizePair_t = std::pair<gui::GuiFont::Enum, unsigned int>;
        using FontSizeToWidthMap_t = misc::VectorMap<FontSizePair_t, float>;

    public:
        NameInfo(const NameInfo &) = delete;
        NameInfo(NameInfo &&) = delete;
        NameInfo & operator=(const NameInfo &) = delete;
        NameInfo & operator=(NameInfo &&) = delete;

        NameInfo() = default;

        std::size_t MaxCharacterCount() const { return 13; }

        float DefaultTextEntryBoxWidth() const;

        gui::GuiFont::Enum DefaultFont() const;

        unsigned int DefaultSize() const;

        char LargestLetter() const { return 'M'; }

        const std::string LargestName() const
        {
            return std::string(MaxCharacterCount(), LargestLetter());
        }

        const std::string LargestLetterString() const { return std::string(1, LargestLetter()); }

        const gui::TextInfo MakeTextInfo() const
        {
            return MakeTextInfo(DefaultFont(), DefaultSize());
        }

        const gui::TextInfo
            MakeTextInfo(const gui::GuiFont::Enum FONT, const unsigned int CHAR_SIZE) const;

        const sf::Vector2f Size() const { return Size(MakeTextInfo()); }

        const sf::Vector2f Size(const gui::GuiFont::Enum FONT, const unsigned int CHAR_SIZE) const
        {
            return Size(MakeTextInfo(FONT, CHAR_SIZE));
        }

        const sf::Vector2f Size(const gui::TextInfo & TEXT_INFO) const;

        const sf::Vector2f ScreenRatio() const { return ConvertSizeToScreenRatio(Size()); }

        const sf::Vector2f
            ScreenRatio(const gui::GuiFont::Enum FONT, const unsigned int CHAR_SIZE) const
        {
            return ConvertSizeToScreenRatio(Size(FONT, CHAR_SIZE));
        }

        const sf::Vector2f ScreenRatio(const gui::TextInfo & TEXT_INFO) const
        {
            return ConvertSizeToScreenRatio(Size(TEXT_INFO));
        }

    private:
        const sf::Vector2f ConvertSizeToScreenRatio(const sf::Vector2f &) const;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_NAME_INFO_HPP_INCLUDED
