#ifndef HEROESPATH_CREATURE_NAME_INFO_INCLUDED
#define HEROESPATH_CREATURE_NAME_INFO_INCLUDED
//
// name-info.hpp
//  Information about creature names and thier displayed characteristics.
//  Often in the code base the max length of a character name is needed for GUI bounds/drawing,
//  and this class answers that question.
//
#include "sfml-util/gui/text-info.hpp"

#include <map>
#include <memory>


namespace heroespath
{
namespace creature
{

    //types required by the singleton implementation
    class NameInfo;
    using NameInfoSPtr_t = std::shared_ptr<NameInfo>;


    //responsibel for data regarding creature names and their display characteristics
    class NameInfo
    {
        //types used by NameInfo
        using FontSizePair_t = std::pair<sfml_util::FontSPtr_t, unsigned int>;
        using FontSizeToWidthMap_t = std::map<FontSizePair_t, float>;

        //prevent copy construction
        NameInfo(const NameInfo &);

        //prevent copy assignment
        NameInfo & operator=(const NameInfo &);

        //prevent non-singleton construction
        NameInfo();

    public:
        virtual ~NameInfo();

        static NameInfoSPtr_t Instance();

        inline std::size_t MaxCharacterCount() const            { return 13; }

        inline float TextEntryBoxWidth(const sfml_util::FontSPtr_t & FONT_SPTR = NameInfo::Instance()->DefaultFont(),
                                       const unsigned int            CHAR_SIZE = NameInfo::Instance()->DefaultSize()) const { return Length(std::string(MaxCharacterCount() + 1, LargestLetter()), FONT_SPTR, CHAR_SIZE); }//The +1 is to accomodate the TextEntryBox's padding and margins

        sfml_util::FontSPtr_t DefaultFont() const;

        unsigned int DefaultSize() const;

        inline char LargestLetter() const                       { return 'M'; }

        inline const std::string LargestName() const            { return std::string(MaxCharacterCount(), LargestLetter()); }

        inline const std::string LargestLetterString() const    { return std::string(1, LargestLetter()); }

        inline const sfml_util::gui::TextInfo MakeTextInfo(const std::string &              TEXT = " ",
                                                           const sf::Color &                COLOR = sf::Color::White,
                                                           const sfml_util::Justified::Enum JUSTIFICATION = sfml_util::Justified::Left) const { return sfml_util::gui::TextInfo(TEXT, NameInfo::Instance()->DefaultFont(), NameInfo::Instance()->DefaultSize(), COLOR, JUSTIFICATION); }

        float LengthMax(const sfml_util::FontSPtr_t & FONT_SPTR = NameInfo::Instance()->DefaultFont(),
                        const unsigned int            CHAR_SIZE = NameInfo::Instance()->DefaultSize());

        float Length(const std::string & TEXT, const sfml_util::FontSPtr_t & FONT_SPTR, const unsigned int CHAR_SIZE) const;

        float Length(const sfml_util::gui::TextInfo & TEXT_INFO) const;

        inline float Length() { return Length(MakeTextInfo(LargestName())); }

    private:
        static NameInfoSPtr_t instance_;
        FontSizeToWidthMap_t  sizeMap_;
    };

}
}

#endif //HEROESPATH_CREATURE_NAME_INFO_INCLUDED
