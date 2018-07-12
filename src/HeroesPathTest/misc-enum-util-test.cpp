// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_EnumUtil"

#include "misc/enum-util.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <type_traits>

using namespace heroespath;
using namespace heroespath::misc;

// a counting enum to test with
struct Counting : public EnumBaseCounting<Counting, EnumFirstValueNone>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        One,
        Two,
        Count
    };

    static const std::string ToString(const Enum ENUM_VALUE)
    {
        switch (ENUM_VALUE)
        {
            case None:
            {
                return "None";
            }
            case One:
            {
                return "One";
            }
            case Two:
            {
                return "Two";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(ENUM_VALUE, "ToString");
            }
        }
    }
};

// a bitfield enum to test with
struct Bitfield : public EnumBaseBitField<Bitfield>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        A = 1 << 0,
        B = 1 << 1,
        Last = B
    };

    static const std::string ToString(const EnumUnderlying_t ENUM_VALUE)
    {
        return EnumBaseBitField::ToString(ENUM_VALUE, Wrap::Yes, "/");
    }

    static const std::string ToStringNoneNotEmpty(const EnumUnderlying_t ENUM_VALUE)
    {
        return EnumBaseBitField::ToString(ENUM_VALUE, Wrap::Yes, "/", NoneEmpty::No);
    }

    static void ToStringPopulate(
        std::ostringstream & ss, const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Enum::A, "A", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Enum::B, "B", SEPARATOR);
    }
};

// a bitfield enum to test with
struct Bitfield2 : public EnumBaseBitField<Bitfield, NoneEmpty::No>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        A = 1 << 0,
        B = 1 << 1,
        Last = B
    };

    static const std::string ToString(const EnumUnderlying_t ENUM_VALUE)
    {
        return EnumBaseBitField::ToString(ENUM_VALUE, Wrap::Yes, "/");
    }

    static const std::string ToStringNoneEmpty(const EnumUnderlying_t ENUM_VALUE)
    {
        return EnumBaseBitField::ToString(ENUM_VALUE, Wrap::Yes, "/", NoneEmpty::Yes);
    }

    static void ToStringPopulate(
        std::ostringstream & ss, const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Enum::A, "A", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, Enum::B, "B", SEPARATOR);
    }
};

BOOST_AUTO_TEST_CASE(MiscEnumUtil_Counting_Tests)
{
    Counting::Test(false);

    BOOST_CHECK(
        (std::is_same<std::underlying_type<Counting::Enum>::type, EnumUnderlying_t>::value));

    BOOST_CHECK(Counting::None == static_cast<Counting::Enum>(0));
    BOOST_CHECK(Counting::One == static_cast<Counting::Enum>(1));
    BOOST_CHECK(Counting::Two == static_cast<Counting::Enum>(2));
}

BOOST_AUTO_TEST_CASE(MiscEnumUtil_BitField_Tests)
{
    Bitfield::Test(false);

    //

    BOOST_CHECK(Bitfield::IsValid(0));
    BOOST_CHECK(Bitfield::IsValid(Bitfield::None));
    BOOST_CHECK(Bitfield::IsValid(Bitfield::A));
    BOOST_CHECK(Bitfield::IsValid(Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::IsValid(Bitfield::Last));
    BOOST_CHECK(Bitfield::IsValid(Bitfield::Last << 1) == false);

    //

    BOOST_CHECK(Bitfield::IsNonZeroValid(0) == false);
    BOOST_CHECK(Bitfield::IsNonZeroValid(Bitfield::None) == false);
    BOOST_CHECK(Bitfield::IsNonZeroValid(Bitfield::A));
    BOOST_CHECK(Bitfield::IsNonZeroValid(Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::IsNonZeroValid(Bitfield::Last));
    BOOST_CHECK(Bitfield::IsNonZeroValid(Bitfield::Last << 1) == false);

    //

    BOOST_CHECK(Bitfield::ToString(Bitfield::None) == "");
    BOOST_CHECK(Bitfield::ToStringNoneNotEmpty(Bitfield::None) == "(None)");
    BOOST_CHECK(Bitfield::ToString(Bitfield::A) == "(A)");
    BOOST_CHECK(Bitfield::ToString(Bitfield::B) == "(B)");
    BOOST_CHECK(Bitfield::ToString((Bitfield::A | Bitfield::B)) == "(A/B)");

    //

    BOOST_CHECK(Bitfield::FromString("") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString(" ") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("()") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("(),(),(,),,, , ") == Bitfield::None);

    BOOST_CHECK(Bitfield::FromString("None") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("B") == Bitfield::B);
    BOOST_CHECK(Bitfield::FromString("Last") != Bitfield::B);
    BOOST_CHECK(Bitfield::FromString("Last") == Bitfield::None);

    BOOST_CHECK(Bitfield::FromString("none") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("a") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("b") == Bitfield::B);
    BOOST_CHECK(Bitfield::FromString("last") == Bitfield::None);

    BOOST_CHECK(Bitfield::FromString("(A)") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(A)") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(),(),(,),,, , A") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("A(),(),(,),,, ,") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(),(A),(,),,, ,") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(),()A,(,),,, ,") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString(" A") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("A ") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString(" A ") == Bitfield::A);

    // spaces are not valid separators because some names will have spaces
    BOOST_CHECK(Bitfield::FromString("A A") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A B") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("B A") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("a b") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A C") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A None") == Bitfield::None);

    BOOST_CHECK(Bitfield::FromString("AA") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A,A") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("A/A") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(A)(A)") == Bitfield::A);

    BOOST_CHECK(Bitfield::FromString("AB") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A,B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("A/B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("(A)(B)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(Bitfield::FromString("ab") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("a,B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("a/b") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("(a)(b)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(Bitfield::FromString("AC") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A,C") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("A/C") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(A)(C)") == Bitfield::A);

    BOOST_CHECK(Bitfield::FromString("BA") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("B,A") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("B/A") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(Bitfield::FromString("(B)(A)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(Bitfield::FromString("ANone") == Bitfield::None);
    BOOST_CHECK(Bitfield::FromString("A,None") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("A/None") == Bitfield::A);
    BOOST_CHECK(Bitfield::FromString("(A)(None)") == Bitfield::A);

    //

    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::None) == 0);
    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::A) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::B) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::Last) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::None | Bitfield::A) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(Bitfield::A | Bitfield::B) == 2);

    BOOST_CHECK(Bitfield::CountBitsSet(0) == 0);
    BOOST_CHECK(Bitfield::CountBitsSet(1) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(2) == 1);
    BOOST_CHECK(Bitfield::CountBitsSet(3) == 2);

    //

    BOOST_CHECK(Bitfield::LargestValidValue() == (Bitfield::A | Bitfield::B));

    //

    Bitfield2::Test(false);

    BOOST_CHECK(Bitfield2::ToString(Bitfield2::None) == "(None)");
    BOOST_CHECK(Bitfield2::ToStringNoneEmpty(Bitfield2::None) == "");
    BOOST_CHECK(Bitfield2::ToString(Bitfield2::A) == "(A)");
    BOOST_CHECK(Bitfield2::ToString(Bitfield2::B) == "(B)");
    BOOST_CHECK(Bitfield2::ToString((Bitfield2::A | Bitfield2::B)) == "(A/B)");

    using Under_t = std::underlying_type<Bitfield::Enum>::type;

    BOOST_CHECK((std::is_same<Under_t, EnumUnderlying_t>::value));

    {
        Under_t none { 0 };
        Under_t a { 1 };
        Under_t b { 2 };
        Under_t ab { 3 };

        auto Reset { [&]() {
            none = 0;
            a = 1;
            b = 2;
            ab = 3;
        } };

        Reset();

        BOOST_CHECK(Bitfield::Enum(none) == Bitfield::None);
        BOOST_CHECK(Bitfield::Enum(a) == Bitfield::A);
        BOOST_CHECK(Bitfield::Enum(b) == Bitfield::B);
        BOOST_CHECK(Bitfield::Enum(ab) == (Bitfield::A | Bitfield::B));

        // BitSetCopy()
        Reset();
        BOOST_CHECK(BitSetCopy(Bitfield::None, Bitfield::None) == Bitfield::None);
        BOOST_CHECK(BitSetCopy(Bitfield::None, Bitfield::None) == none);
        BOOST_CHECK(BitSetCopy(none, none) == none);
        BOOST_CHECK(BitSetCopy(none, none) == Bitfield::None);
        BOOST_CHECK(none == 0);

        BOOST_CHECK(BitSetCopy(Bitfield::None, Bitfield::A) == Bitfield::A);
        BOOST_CHECK(BitSetCopy(Bitfield::None, Bitfield::B) == b);
        BOOST_CHECK(BitSetCopy(none, a) == a);
        BOOST_CHECK(BitSetCopy(none, b) == Bitfield::B);
        BOOST_CHECK(none == 0);
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);

        BOOST_CHECK(BitSetCopy(Bitfield::A, Bitfield::A) == Bitfield::A);
        BOOST_CHECK(BitSetCopy(Bitfield::A, Bitfield::A) == a);
        BOOST_CHECK(BitSetCopy(a, a) == a);
        BOOST_CHECK(BitSetCopy(a, a) == Bitfield::A);
        BOOST_CHECK(a == 1);

        BOOST_CHECK(BitSetCopy(Bitfield::A, Bitfield::B) == (Bitfield::A | Bitfield::B));
        BOOST_CHECK(BitSetCopy(Bitfield::A, Bitfield::B) == ab);
        BOOST_CHECK(BitSetCopy(a, b) == ab);
        BOOST_CHECK(BitSetCopy(a, b) == (Bitfield::A | Bitfield::B));
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);

        // BitClearCopy()
        Reset();
        BOOST_CHECK(BitClearCopy(Bitfield::None, Bitfield::None) == Bitfield::None);
        BOOST_CHECK(BitClearCopy(Bitfield::None, Bitfield::None) == none);
        BOOST_CHECK(BitClearCopy(none, none) == none);
        BOOST_CHECK(BitClearCopy(none, none) == Bitfield::None);
        BOOST_CHECK(none == 0);

        BOOST_CHECK(BitClearCopy(Bitfield::None, Bitfield::A) == Bitfield::None);
        BOOST_CHECK(BitClearCopy(Bitfield::None, Bitfield::B) == none);
        BOOST_CHECK(BitClearCopy(none, a) == none);
        BOOST_CHECK(BitClearCopy(none, b) == Bitfield::None);
        BOOST_CHECK(none == 0);
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);

        BOOST_CHECK(BitClearCopy(Bitfield::A, Bitfield::A) == Bitfield::None);
        BOOST_CHECK(BitClearCopy(Bitfield::A, Bitfield::A) == none);
        BOOST_CHECK(BitClearCopy(a, a) == none);
        BOOST_CHECK(BitClearCopy(a, a) == Bitfield::None);
        BOOST_CHECK(a == 1);

        BOOST_CHECK(BitClearCopy((Bitfield::A | Bitfield::B), Bitfield::B) == Bitfield::A);
        BOOST_CHECK(BitClearCopy((Bitfield::A | Bitfield::B), Bitfield::A) == b);
        BOOST_CHECK(BitClearCopy(ab, a) == b);
        BOOST_CHECK(BitClearCopy(ab, b) == Bitfield::A);
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);

        // BitToggleCopy()
        Reset();
        BOOST_CHECK(BitToggleCopy(Bitfield::None, Bitfield::None) == Bitfield::None);
        BOOST_CHECK(BitToggleCopy(Bitfield::None, Bitfield::None) == none);
        BOOST_CHECK(BitToggleCopy(none, none) == none);
        BOOST_CHECK(BitToggleCopy(none, none) == Bitfield::None);
        BOOST_CHECK(none == 0);

        BOOST_CHECK(BitToggleCopy(Bitfield::None, Bitfield::A) == Bitfield::A);
        BOOST_CHECK(BitToggleCopy(Bitfield::None, Bitfield::B) == b);
        BOOST_CHECK(BitToggleCopy(none, a) == a);
        BOOST_CHECK(BitToggleCopy(none, b) == Bitfield::B);
        BOOST_CHECK(none == 0);
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);

        BOOST_CHECK(BitToggleCopy(Bitfield::A, Bitfield::A) == Bitfield::None);
        BOOST_CHECK(BitToggleCopy(Bitfield::A, Bitfield::A) == none);
        BOOST_CHECK(BitToggleCopy(a, a) == none);
        BOOST_CHECK(BitToggleCopy(a, a) == Bitfield::None);
        BOOST_CHECK(a == 1);

        BOOST_CHECK(BitToggleCopy(Bitfield::A, Bitfield::B) == (Bitfield::A | Bitfield::B));
        BOOST_CHECK(BitToggleCopy(Bitfield::A, Bitfield::B) == ab);
        BOOST_CHECK(BitToggleCopy(a, b) == ab);
        BOOST_CHECK(BitToggleCopy(a, b) == (Bitfield::A | Bitfield::B));
        BOOST_CHECK(a == 1);
        BOOST_CHECK(b == 2);
    }

    {
        Bitfield::Enum none { Bitfield::None };
        Bitfield::Enum a { Bitfield::A };
        Bitfield::Enum b { Bitfield::B };
        Bitfield::Enum ab { Bitfield::A | Bitfield::B };

        auto Reset { [&]() {
            none = Bitfield::None;
            a = Bitfield::A;
            b = Bitfield::B;
            ab = Bitfield::A | Bitfield::B;
        } };

        Reset();

        // BitSet()
        Reset();
        BitSet(none, Bitfield::None);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitSet(none, none);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitSet(a, Bitfield::A);
        BOOST_CHECK(a == Bitfield::A);

        Reset();
        BitSet(none, Bitfield::A);
        BOOST_CHECK(none == Bitfield::A);

        Reset();
        BitSet(none, Bitfield::B);
        BOOST_CHECK(none == Bitfield::B);

        Reset();
        BitSet(a, Bitfield::B);
        BOOST_CHECK(a == ab);

        Reset();
        BitSet(b, Bitfield::A);
        BOOST_CHECK(b == ab);

        // BitClear()
        Reset();
        BitClear(none, Bitfield::None);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitClear(none, none);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitClear(a, Bitfield::A);
        BOOST_CHECK(a == Bitfield::None);

        Reset();
        BitClear(none, Bitfield::A);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitClear(none, Bitfield::B);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitClear(a, Bitfield::B);
        BOOST_CHECK(a == Bitfield::A);

        Reset();
        BitClear(b, Bitfield::A);
        BOOST_CHECK(b == Bitfield::B);

        // BitToggle()
        Reset();
        BitToggle(none, Bitfield::None);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitToggle(none, none);
        BOOST_CHECK(none == Bitfield::None);

        Reset();
        BitToggle(a, Bitfield::A);
        BOOST_CHECK(a == Bitfield::None);

        Reset();
        BitToggle(none, Bitfield::A);
        BOOST_CHECK(none == Bitfield::A);

        Reset();
        BitToggle(none, Bitfield::B);
        BOOST_CHECK(none == Bitfield::B);

        Reset();
        BitToggle(a, Bitfield::B);
        BOOST_CHECK(a == ab);

        Reset();
        BitToggle(b, Bitfield::A);
        BOOST_CHECK(b == ab);
    }
}
