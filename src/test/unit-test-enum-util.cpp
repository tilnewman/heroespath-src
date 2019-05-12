// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_EnumUtil"

#include <boost/test/unit_test.hpp>

#include "misc/assertlogandthrow.hpp"
#include "misc/enum-common.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

// all of the enum includes
#include "avatar/avatar-enum.hpp"
#include "avatar/pose-enum.hpp"
#include "combat/blocking-pos-type-enum.hpp"
#include "combat/combat-over-enum.hpp"
#include "combat/effect-type-enum.hpp"
#include "combat/hit-info.hpp"
#include "combat/name-position-enum.hpp"
#include "combat/strategy-enums.hpp"
#include "combat/target-enum.hpp"
#include "combat/turn-action-enum.hpp"
#include "creature/achievement-enum.hpp"
#include "creature/condition-enum.hpp"
#include "creature/dragon-class-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/nonplayer-inventory-types.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/title-enum.hpp"
#include "creature/trait-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "game/phase-enum.hpp"
#include "gui/animation-enum.hpp"
#include "gui/brightness-enum.hpp"
#include "gui/combat-image-enum.hpp"
#include "gui/corner-enum.hpp"
#include "gui/direction-enum.hpp"
#include "gui/display-change-result.hpp"
#include "gui/font-enum.hpp"
#include "gui/gui-event-enum.hpp"
#include "gui/image-option-enum.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/justified-enum.hpp"
#include "gui/mouse-state-enum.hpp"
#include "gui/moving-enum.hpp"
#include "gui/music-enum.hpp"
#include "gui/music-operator.hpp"
#include "gui/orientation-enum.hpp"
#include "gui/sfx-set-enum.hpp"
#include "gui/side-enum.hpp"
#include "gui/size-enum.hpp"
#include "gui/sound-effects-enum.hpp"
#include "interact/interact-enum.hpp"
#include "interact/interaction-button-enum.hpp"
#include "interact/interaction-button.hpp"
#include "interact/interaction-text-enum.hpp"
#include "interact/statement.hpp"
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "item/treasure-available-enum.hpp"
#include "item/treasure-image-enum.hpp"
#include "item/weapon-types.hpp"
#include "map/layer-type-enum.hpp"
#include "map/level-enum.hpp"
#include "misc/log-pri-enum.hpp"
#include "popup/popup-enums.hpp"
#include "song/song-enum.hpp"
#include "song/song-type-enum.hpp"
#include "spell/spell-enum.hpp"
#include "stage/stage-enum.hpp"

#include <iostream>
#include <type_traits>

using namespace heroespath;
using namespace heroespath::misc;

template <typename EnumWrapper_t>
void EnumTest(const EnumUnderlying_t LAST_VALID_VALUE, const bool MUST_FIRST_STRING_TO_BE_EMPTY)
{
    std::ostringstream msgSS;
    msgSS << "EnumTest<" << NAMEOF_TYPE(EnumWrapper_t) << "::Enum, "
          << NAMEOF_TYPE(EnumUnderlying_t) << ">(last_valid_value=" << LAST_VALID_VALUE
          << ", must_first_be_empty=" << std::boolalpha << MUST_FIRST_STRING_TO_BE_EMPTY << ") ";

    M_HP_LOG(msgSS.str() + "Starting...");

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (LAST_VALID_VALUE > 0), msgSS.str() << "was given a last_valid_value that was not > zero.");

    static EnumUnderlying_t flagValue { 0 };

    // this is used to verify ToString() returns a unique string for each unique enum
    static std::vector<std::string> alreadyGeneratedStrings;
    if (alreadyGeneratedStrings.capacity() < static_cast<std::size_t>(LAST_VALID_VALUE))
    {
        alreadyGeneratedStrings.reserve(static_cast<std::size_t>(LAST_VALID_VALUE));
    }

    while (flagValue <= LAST_VALID_VALUE)
    {
        const auto FLAG_VALUE_TO_TEST_AND_REPORT { flagValue++ };

        const auto ENUM { static_cast<typename EnumWrapper_t::Enum>(
            FLAG_VALUE_TO_TEST_AND_REPORT) };

        if (EnumUtil<EnumWrapper_t>::IsValid(ENUM) == false)
        {
            M_HP_LOG(msgSS.str() << " skipping invalid value=" << FLAG_VALUE_TO_TEST_AND_REPORT);
            continue;
        }

        const auto STRING { EnumUtil<EnumWrapper_t>::ToString(ENUM) };

        if ((FLAG_VALUE_TO_TEST_AND_REPORT == 0) && MUST_FIRST_STRING_TO_BE_EMPTY)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (STRING.empty()),
                msgSS.str() << "ToString() returned non-empty string (\"" << STRING
                            << "\") on first value.");
        }
        else
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (STRING.empty() == false),
                msgSS.str() << "ToString() returned an empty string on value "
                            << FLAG_VALUE_TO_TEST_AND_REPORT << ".");
        }

        const auto IS_DUPLICATE {
            std::find(
                std::begin(alreadyGeneratedStrings), std::end(alreadyGeneratedStrings), STRING)
            != std::end(alreadyGeneratedStrings)
        };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (IS_DUPLICATE == false),
            msgSS.str() << "value=" << FLAG_VALUE_TO_TEST_AND_REPORT << "=\"" << STRING
                        << "\" is a duplicate of a previous generated string.");

        const auto FROM_STRING_RESULT { EnumUtil<EnumWrapper_t>::FromString(STRING) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FROM_STRING_RESULT == FLAG_VALUE_TO_TEST_AND_REPORT),
            msgSS.str() << "FromString(\"" << STRING << "\")=" << FROM_STRING_RESULT
                        << "!=(expected)=" << FLAG_VALUE_TO_TEST_AND_REPORT << ".");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumUtil<EnumWrapper_t>::IsValid(
                static_cast<typename EnumWrapper_t::Enum>(FROM_STRING_RESULT))),
            msgSS.str() << "FromString(\"" << STRING << "\")=" << FROM_STRING_RESULT
                        << "=(expected)=" << FLAG_VALUE_TO_TEST_AND_REPORT
                        << " -but that value was not valid.  (IsValid() returned false)");
    }

    M_HP_LOG(msgSS.str() + "  Done testing all " << flagValue << " values.");
}

template <typename EnumWrapper_t>
void TestBitFieldEnum()
{
    using UnderlyingTypeActual_t =
        typename std::underlying_type<typename EnumWrapper_t::Enum>::type;

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (misc::are_same_v<EnumUnderlying_t, UnderlyingTypeActual_t>),
        NAMEOF_TYPE(EnumWrapper_t)
            << "Underlying type was: " << NAMEOF_TYPE(UnderlyingTypeActual_t)
            << " instead of what it should be: " << NAMEOF_TYPE(EnumUnderlying_t) << ".");

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (EnumWrapper_t::None == 0), NAMEOF_TYPE(EnumWrapper_t) << "::None was not zero.");

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (EnumWrapper_t::Last > 0),
        NAMEOF_TYPE(EnumWrapper_t)
            << "::Last=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Last)
            << " is not > zero.");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (EnumWrapper_t::Last <= EnumUtil<EnumWrapper_t>::LargestValidValue()),
        NAMEOF_TYPE(EnumWrapper_t)
            << "::Last=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Last)
            << " is not <= the largest valid value=" << EnumUtil<EnumWrapper_t>::LargestValidValue()
            << ".");

    EnumTest<EnumWrapper_t>(EnumUtil<EnumWrapper_t>::LargestValidValue(), true);
}

template <typename EnumWrapper_t>
void TestCountingEnum()
{

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

    using UnderlyingTypeActual_t =
        typename std::underlying_type<typename EnumWrapper_t::Enum>::type;

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (misc::are_same_v<EnumUnderlying_t, UnderlyingTypeActual_t>),
        NAMEOF_TYPE(EnumWrapper_t)
            << "Underlying type was: " << NAMEOF_TYPE(UnderlyingTypeActual_t)
            << " instead of what it should be: " << NAMEOF_TYPE(EnumUnderlying_t) << ".");

    if constexpr (EnumWrapper_t::first_value_t == EnumFirstValue::Not)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumWrapper_t::Not == 0),
            NAMEOF_TYPE(EnumWrapper_t) << "::Not=" << EnumWrapper_t::Not << " instead of zero.");
    }

    if constexpr (EnumWrapper_t::first_value_t == EnumFirstValue::Nothing)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumWrapper_t::Nothing == 0),
            NAMEOF_TYPE(EnumWrapper_t)
                << "::Nothing=" << EnumWrapper_t::Nothing << " instead of zero.");
    }

    if constexpr (EnumWrapper_t::first_value_t == EnumFirstValue::Never)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumWrapper_t::Never == 0),
            NAMEOF_TYPE(EnumWrapper_t)
                << "::Never=" << EnumWrapper_t::Never << " instead of zero.");
    }

    if constexpr (EnumWrapper_t::first_value_t == EnumFirstValue::None)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (EnumWrapper_t::None == 0),
            NAMEOF_TYPE(EnumWrapper_t) << "::None=" << EnumWrapper_t::None << " instead of zero.");
    }

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (EnumWrapper_t::Count > 0),
        NAMEOF_TYPE(EnumWrapper_t)
            << "::Count=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Count)
            << " is not > zero.");

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (static_cast<EnumUnderlying_t>(EnumWrapper_t::Count)
         == (EnumUtil<EnumWrapper_t>::LargestValidValue() + 1)),
        NAMEOF_TYPE(EnumWrapper_t)
            << "::Count(" << EnumUtil<EnumWrapper_t>::ToString(EnumWrapper_t::Count)
            << ")=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Count)
            << " is not one less than the largest valid value="
            << EnumUtil<EnumWrapper_t>::LargestValidValue() << ".");

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    EnumTest<EnumWrapper_t>(EnumUtil<EnumWrapper_t>::LargestValidValue(), false);

    M_HP_ASSERT_OR_LOG_AND_THROW(
        (EnumUtil<EnumWrapper_t>::ToString(EnumWrapper_t::Count) == "Count"),
        NAMEOF_TYPE(EnumWrapper_t) << "::ToString(Count) != \"(Count)\"");
}

// a counting enum to test with
struct Counting : public EnumBaseCounting<EnumFirstValue::None>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        One,
        Two,
        Count
    };
};

// a bitfield enum to test with
struct Bitfield : public EnumBaseBitField<>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        A = 1 << 0,
        B = 1 << 1,
        Last = B
    };
};

// a bitfield enum to test with
struct Bitfield2 : public EnumBaseBitField<>
{
    enum Enum : EnumUnderlying_t
    {
        None = 0,
        A = 1 << 0,
        B = 1 << 1,
        Last = B
    };
};

BOOST_AUTO_TEST_CASE(Case_1_MiscEnumUtil_Counting_Tests)
{
    TestCountingEnum<Counting>();

    BOOST_CHECK((misc::are_same_v<std::underlying_type<Counting::Enum>::type, EnumUnderlying_t>));

    BOOST_CHECK(Counting::None == static_cast<Counting::Enum>(0));
    BOOST_CHECK(Counting::One == static_cast<Counting::Enum>(1));
    BOOST_CHECK(Counting::Two == static_cast<Counting::Enum>(2));
    BOOST_CHECK(Counting::Count == static_cast<Counting::Enum>(3));

    BOOST_CHECK(NAMEOF_ENUM_STR(Counting::None) == "None");
    BOOST_CHECK(NAMEOF_ENUM_STR(Counting::One) == "One");
    BOOST_CHECK(NAMEOF_ENUM_STR(Counting::Two) == "Two");
    BOOST_CHECK(NAMEOF_ENUM_STR(Counting::Count) == "Count");
}

BOOST_AUTO_TEST_CASE(Case_2_MiscEnumUtil_BitField_Tests)
{
    // handy quick initial sanity check if all goes to <insert explative>
    // std::cout << "\nBitfield::A\tdefault_as_number=" << Bitfield::A << ", to_string=\""
    //          << Bitfield::ToString(Bitfield::A)
    //          << "\", from_string(\"A\")=" << EnumUtil<Bitfield2>::FromString("A")
    //          << ", from_string(\"a\")=" << EnumUtil<Bitfield2>::FromString("a");
    //
    // std::cout << "\nBitfield::B\tdefault_as_number=" << Bitfield::B << ", to_string=\""
    //          << Bitfield::ToString(Bitfield::B)
    //          << "\", from_string(\"B\")=" << EnumUtil<Bitfield2>::FromString("B")
    //          << ", from_string(\"b\")=" << EnumUtil<Bitfield2>::FromString("b");
    //
    // std::cout << "\nBitfield:: A|B\tdefault_as_number=" << (Bitfield::A | Bitfield::B)
    //          << ", to_string=\"" << Bitfield::ToString(Bitfield::A | Bitfield::B)
    //          << "\", (\"A, B\")=" << EnumUtil<Bitfield2>::FromString("A/B")
    //          << ", (\"a, b\")=" << EnumUtil<Bitfield2>::FromString("a/b")
    //          << "\", (\"A, b\")=" << EnumUtil<Bitfield2>::FromString("A/B")
    //          << ", (\"a, B\")=" << EnumUtil<Bitfield2>::FromString("a/b") << std::endl;

    TestBitFieldEnum<Bitfield>();

    //

    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(0));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(Bitfield::None));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(Bitfield::A));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(Bitfield::Last));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValid(Bitfield::Last << 1) == false);

    //

    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(0) == false);
    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(Bitfield::None) == false);
    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(Bitfield::A));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(Bitfield::Last));
    BOOST_CHECK(EnumUtil<Bitfield>::IsValidAndNonZero(Bitfield::Last << 1) == false);

    //

    BOOST_CHECK(EnumUtil<Bitfield>::ToString(Bitfield::None) == "");
    BOOST_CHECK(EnumUtil<Bitfield>::ToString(Bitfield::A) == "A");
    BOOST_CHECK(EnumUtil<Bitfield>::ToString(Bitfield::B) == "B");
    BOOST_CHECK(EnumUtil<Bitfield>::ToString((Bitfield::A | Bitfield::B)) == "A, B");

    BOOST_CHECK(
        EnumUtil<Bitfield>::ToString(Bitfield::None, EnumStringHow(", ", Wrap::No, NoneEmpty::No))
        == "None");

    BOOST_CHECK(
        EnumUtil<Bitfield>::ToString(Bitfield::None, EnumStringHow(", ", Wrap::Yes, NoneEmpty::No))
        == "(None)");

    // don't wrap empty
    BOOST_CHECK(
        EnumUtil<Bitfield>::ToString(Bitfield::None, EnumStringHow(", ", Wrap::Yes, NoneEmpty::Yes))
        == "");

    BOOST_CHECK(
        EnumUtil<Bitfield>::ToString(Bitfield::None, EnumStringHow(", ", Wrap::No, NoneEmpty::Yes))
        == "");

    //

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString(" ") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("()") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(),(),(,),,, , ") == Bitfield::None);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("None") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("B") == Bitfield::B);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("Last") != Bitfield::B);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("Last") == Bitfield::None);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("none") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("a") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("b") == Bitfield::B);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("last") == Bitfield::None);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(),(),(,),,, , A") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A(),(),(,),,, ,") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(),(A),(,),,, ,") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(),()A,(,),,, ,") == Bitfield::A);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("AA") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A,A") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A/A") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)(A)") == Bitfield::A);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("AB") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A,B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A/B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)(B)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("ab") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("a,B") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("a/b") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(a)(b)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("AC") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A,C") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A/C") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)(C)") == Bitfield::A);

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("BA") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("B,A") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("B/A") == (Bitfield::A | Bitfield::B));
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(B)(A)") == (Bitfield::A | Bitfield::B));

    BOOST_CHECK(EnumUtil<Bitfield>::FromString("ANone") == Bitfield::None);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A,None") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("A/None") == Bitfield::A);
    BOOST_CHECK(EnumUtil<Bitfield>::FromString("(A)(None)") == Bitfield::A);

    //

    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::None) == 0);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::A) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::B) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::Last) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::None | Bitfield::A) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(Bitfield::A | Bitfield::B) == 2);

    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(0) == 0);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(1) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(2) == 1);
    BOOST_CHECK(EnumUtil<Bitfield>::CountBitsSet(3) == 2);

    //

    BOOST_CHECK(EnumUtil<Bitfield>::LargestValidValue() == (Bitfield::A | Bitfield::B));

    //

    TestBitFieldEnum<Bitfield2>();

    BOOST_CHECK(EnumUtil<Bitfield2>::ToString(Bitfield2::None) == "");
    BOOST_CHECK(EnumUtil<Bitfield2>::ToString(Bitfield2::A) == "A");
    BOOST_CHECK(EnumUtil<Bitfield2>::ToString(Bitfield2::B) == "B");
    BOOST_CHECK(EnumUtil<Bitfield2>::ToString((Bitfield2::A | Bitfield2::B)) == "A, B");

    using Under_t = std::underlying_type<Bitfield::Enum>::type;

    BOOST_CHECK((are_same_v<Under_t, EnumUnderlying_t>));

    {
        const EnumUnderlying_t ZERO(0);
        const EnumUnderlying_t FULL(~ZERO);
        const EnumUnderlying_t ONE(1);
        const EnumUnderlying_t TWO(2);
        const EnumUnderlying_t THREE(3);

        // compliment
        {
            BOOST_CHECK_EQUAL(~ZERO, FULL);
            BOOST_CHECK_EQUAL(ZERO, ~FULL);

            BOOST_CHECK_EQUAL(~Bitfield::None, FULL);
            BOOST_CHECK_EQUAL(~Bitfield::A, ~ONE);
            BOOST_CHECK_EQUAL(~Bitfield::B, ~TWO);
        }

        // or
        {
            BOOST_CHECK_EQUAL((ZERO | Bitfield::A), ONE);
            BOOST_CHECK_EQUAL((ZERO | Bitfield::B), TWO);
            BOOST_CHECK_EQUAL((ZERO | Bitfield::A | Bitfield::B), THREE);
            BOOST_CHECK_EQUAL((ZERO | Bitfield2::A | Bitfield::B), THREE);

            BOOST_CHECK_EQUAL((ZERO | Bitfield2::A | Bitfield::B | ZERO | Bitfield::A), THREE);

            auto temp = Bitfield2::A;
            temp |= Bitfield2::B;
            BOOST_CHECK_EQUAL(temp, THREE);
        }

        // and
        {
            BOOST_CHECK_EQUAL((ZERO & Bitfield::A), ZERO);
            BOOST_CHECK_EQUAL((ZERO & Bitfield::B), ZERO);
            BOOST_CHECK_EQUAL((ZERO & Bitfield::A & Bitfield::B), ZERO);
            BOOST_CHECK_EQUAL((ZERO & Bitfield2::A & Bitfield::B), ZERO);

            BOOST_CHECK_EQUAL((ONE & Bitfield::A), ONE);
            BOOST_CHECK_EQUAL((ONE & Bitfield::B), ZERO);
            BOOST_CHECK_EQUAL((THREE & Bitfield::B), TWO);
            BOOST_CHECK_EQUAL((ONE & Bitfield::A & Bitfield::B), ZERO);
            BOOST_CHECK_EQUAL((ONE & Bitfield2::A & Bitfield::B), ZERO);

            {
                auto temp = Bitfield::A;
                temp &= Bitfield::B;
                BOOST_CHECK_EQUAL(temp, Bitfield::None);
            }

            {
                auto temp = Bitfield::B;
                temp &= Bitfield::Enum(EnumUnderlying_t(3));
                BOOST_CHECK_EQUAL(temp, TWO);
            }
        }
    }

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

BOOST_AUTO_TEST_CASE(Case_3_MiscEnumUtil_ActualEnums_Counting_Tests)
{
    TestCountingEnum<avatar::Avatar>();
    TestCountingEnum<avatar::Pose>();
    TestCountingEnum<combat::BlockingPosType>();
    TestCountingEnum<combat::CombatEnd>();
    TestCountingEnum<combat::EffectType>();
    TestCountingEnum<combat::HitType>();
    TestCountingEnum<combat::NamePosition>();
    TestCountingEnum<combat::strategy::AdvanceType>();
    TestCountingEnum<combat::strategy::RetreatType>();
    TestCountingEnum<combat::strategy::FrequencyType>();
    TestCountingEnum<combat::TargetType>();
    TestCountingEnum<combat::TurnAction>();
    TestCountingEnum<creature::AchievementType>();
    TestCountingEnum<creature::Conditions>();
    TestCountingEnum<creature::dragon_class>();
    TestCountingEnum<creature::nonplayer::wealth_type>();
    TestCountingEnum<creature::nonplayer::owns_magic_type>();
    TestCountingEnum<creature::nonplayer::complexity_type>();
    TestCountingEnum<creature::origin_type>();
    TestCountingEnum<creature::race>();
    TestCountingEnum<creature::rank_class>();
    TestCountingEnum<creature::role>();
    TestCountingEnum<creature::sex>();
    TestCountingEnum<creature::Titles>();
    TestCountingEnum<creature::Traits>();
    TestCountingEnum<creature::wolfen_class>();
    TestCountingEnum<gui::Animations>();
    TestCountingEnum<gui::Brightness>();
    TestCountingEnum<gui::CombatImageType>();
    TestCountingEnum<gui::Direction>();
    TestCountingEnum<gui::GuiFont>();
    TestCountingEnum<gui::ImageTextEntity::MouseStateSync>();
    TestCountingEnum<gui::Justified>();
    TestCountingEnum<gui::MouseState>();
    TestCountingEnum<gui::Moving>();
    TestCountingEnum<gui::music>();
    TestCountingEnum<gui::music_update_status>();
    TestCountingEnum<gui::Orientation>();
    TestCountingEnum<gui::sound_effect_set>();
    TestCountingEnum<gui::Size>();
    TestCountingEnum<gui::Footstep>();
    TestCountingEnum<gui::sound_effect>();
    TestCountingEnum<interact::Interact>();
    TestCountingEnum<interact::Buttons>();
    TestCountingEnum<interact::Text>();
    TestCountingEnum<item::armor::shield_type>();
    TestCountingEnum<item::armor::helm_type>();
    TestCountingEnum<item::armor::base_type>();
    TestCountingEnum<item::armor::cover_type>();
    TestCountingEnum<item::material>();
    TestCountingEnum<item::misc_type>();
    TestCountingEnum<item::set_type>();
    TestCountingEnum<item::named_type>();
    TestCountingEnum<item::armor_type>();
    TestCountingEnum<item::body_part>();
    TestCountingEnum<item::name_material_type>();
    TestCountingEnum<item::TreasureAvailable>();
    TestCountingEnum<item::TreasureImage>();
    TestCountingEnum<item::weapon::sword_type>();
    TestCountingEnum<item::weapon::axe_type>();
    TestCountingEnum<item::weapon::club_type>();
    TestCountingEnum<item::weapon::whip_type>();
    TestCountingEnum<item::weapon::projectile_type>();
    TestCountingEnum<item::weapon::bladedstaff_type>();
    TestCountingEnum<map::LayerType>();
    TestCountingEnum<map::LevelType>();
    TestCountingEnum<map::Level>();
    TestCountingEnum<popup::PopupStage>();
    TestCountingEnum<popup::PopupButtonColor>();
    TestCountingEnum<popup::PopupImage>();
    TestCountingEnum<song::Songs>();
    TestCountingEnum<song::SongType>();
    TestCountingEnum<spell::Spells>();
    TestCountingEnum<stage::Stage>();
}

BOOST_AUTO_TEST_CASE(Case_4_MiscEnumUtil_ActualEnums_Bitfield_Tests)
{
    TestBitFieldEnum<combat::strategy::RefineType>();
    TestBitFieldEnum<creature::EnchantmentType>();
    TestBitFieldEnum<creature::nonplayer::collector_type>();
    TestBitFieldEnum<game::Phase>();
    TestBitFieldEnum<gui::Corner>();
    TestBitFieldEnum<gui::GuiEvent>();
    TestBitFieldEnum<gui::ImageOpt>();
    TestBitFieldEnum<gui::Side>();
    TestBitFieldEnum<item::category>();
    TestBitFieldEnum<item::element_type>();
    TestBitFieldEnum<item::weapon_type>();
    TestBitFieldEnum<popup::PopupButtons>();

    M_HP_LOG_WRN("The big long bitfield enum test: combat::strategy::SelectType:  START");
    TestBitFieldEnum<combat::strategy::SelectType>();
    M_HP_LOG_WRN("The big long bitfield enum test: combat::strategy::SelectType:  FINISHED");
}
