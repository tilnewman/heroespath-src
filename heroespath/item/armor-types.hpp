#ifndef APPBASE_HEROESPATH_ITEM_ARMORTYPES_INCLUDED
#define APPBASE_HEROESPATH_ITEM_ARMORTYPES_INCLUDED
//
// armor-types.hpp
//  A set of enumerations that define a piece of armor.
//
#include <string>


namespace heroespath
{
namespace item
{
namespace armor
{

    struct shield_type
    {
        enum Enum
        {
            Buckler = 0,
            Kite,
            Heater,
            Pavis,
            Count
        };

        static const std::string ToString(const shield_type::Enum);
        inline static const std::string ToStringFull(const shield_type::Enum E) { return ToString(E) + " Shield"; }
        static shield_type::Enum FromString(const std::string &);
    };


    struct helm_type
    {
        enum Enum
        {
            Leather = 0,
            MailCoif,
            Kettle,
            Archers,
            Bascinet,
            Great,
            Count
        };

        static const std::string ToString(const helm_type::Enum);
        inline static const std::string ToStringFull(const helm_type::Enum E) { return ToString(E) + " Helm"; }
        static helm_type::Enum FromString(const std::string &);
    };


    struct base_type
    {
        enum Enum
        {
            Plain = 0,
            Mail,
            Scale,
            Plate,
            Count
        };

        static const std::string ToString(const base_type::Enum);
        static base_type::Enum FromString(const std::string &);
    };


    struct cover_type
    {
        enum Enum
        {
            Cape = 0,
            Vest,
            Robe,
            Cloak,
            Count
        };

        static const std::string ToString(const cover_type::Enum);
        static cover_type::Enum FromString(const std::string &);
    };


}//namespace armor
}//namespace item
}//namespace heroespath

#endif //APPBASE_HEROESPATH_ITEM_ARMORTYPES_INCLUDED
