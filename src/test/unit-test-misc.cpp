// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include "gui/list-no-element.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/real.hpp"

#include <iostream>
#include <limits>
#include <type_traits>

using namespace heroespath::gui;
using namespace heroespath::misc;

// a simple class used to test working with non-POD
class Thing
{
public:
    explicit Thing(const int NUMBER = 0)
        : m_number(NUMBER)
    {}

    int get() const { return m_number; }
    void set(const int NUM) { m_number = NUM; }

private:
    int m_number;
};

inline bool operator==(const Thing & L, const Thing & R) { return (L.get() == R.get()); }
inline bool operator!=(const Thing & L, const Thing & R) { return !(L == R); }

template <typename T>
bool testTypeIsZero()
{

    bool didPassTests { IsRealZero(T(0)) && !IsRealZero(T(1)) && IsRealZero(T(1) - T(1))
                        && IsRealZero(
                            std::numeric_limits<T>::max() - std::numeric_limits<T>::max()) };

    if constexpr (std::is_signed<T>::value)
    {
        if (IsRealZero(T(-1)) || !IsRealZero(T(-1) + T(1))
            || IsRealZero(std::numeric_limits<T>::max())
            || !IsRealZero(std::numeric_limits<T>::min() + std::numeric_limits<T>::max()))
            ;
        {
            didPassTests = false;
        }
    }

    return didPassTests;
}

template <typename T>
bool testTypeIsRealZero()
{
    bool didPassTests { IsRealZero(T(0)) && !IsRealZero(T(1)) && IsRealZero(T(1) - T(1))
                        && IsRealZero(
                            std::numeric_limits<T>::max() - std::numeric_limits<T>::max()) };

    if constexpr (std::is_signed<T>::value)
    {
        if (IsRealZero(T(-1)) || !IsRealZero(T(-1) + T(1))
            || IsRealZero(std::numeric_limits<T>::max())
            || IsRealZero(std::numeric_limits<T>::min())
            || !IsRealZero(T(1) + std::numeric_limits<T>::min() + std::numeric_limits<T>::max()))
        {
            didPassTests = false;
        }
    }

    return didPassTests;
}

template <typename T>
bool testTypeIsRealOne()
{
    bool didPassTests {
        IsRealOne(T(1)) && !IsRealOne(T(0)) && IsRealOne(T(0) + T(1))
        && IsRealOne(T(1) + (std::numeric_limits<T>::max() - std::numeric_limits<T>::max()))
    };

    if constexpr (std::is_signed<T>::value)
    {
        if (IsRealOne(T(-1)) || !IsRealOne(T(-1) + T(2)))
        {
            didPassTests = false;
        }
    }

    return didPassTests;
}

BOOST_AUTO_TEST_CASE(Real_IsRealZero)
{
    BOOST_CHECK(testTypeIsRealZero<char>());
    BOOST_CHECK(testTypeIsRealZero<short>());
    BOOST_CHECK(testTypeIsRealZero<int>());
    BOOST_CHECK(testTypeIsRealZero<long>());
    BOOST_CHECK(testTypeIsRealZero<long long>());
    BOOST_CHECK(testTypeIsRealZero<unsigned char>());
    BOOST_CHECK(testTypeIsRealZero<unsigned short>());
    BOOST_CHECK(testTypeIsRealZero<unsigned int>());
    BOOST_CHECK(testTypeIsRealZero<unsigned long>());
    BOOST_CHECK(testTypeIsRealZero<unsigned long long>());

    BOOST_CHECK(IsRealZero(0.0));
    BOOST_CHECK(IsRealZero(0.0f));

    BOOST_CHECK(IsRealZero(1.0) == false);
    BOOST_CHECK(IsRealZero(1.0f) == false);

    BOOST_CHECK(IsRealZero(0.00001) == false);
    BOOST_CHECK(IsRealZero(-0.00001f) == false);
}

BOOST_AUTO_TEST_CASE(Real_IsRealOne)
{
    BOOST_CHECK(testTypeIsRealOne<char>());
    BOOST_CHECK(testTypeIsRealOne<short>());
    BOOST_CHECK(testTypeIsRealOne<int>());
    BOOST_CHECK(testTypeIsRealOne<long>());
    BOOST_CHECK(testTypeIsRealOne<long long>());
    BOOST_CHECK(testTypeIsRealOne<unsigned char>());
    BOOST_CHECK(testTypeIsRealOne<unsigned short>());
    BOOST_CHECK(testTypeIsRealOne<unsigned int>());
    BOOST_CHECK(testTypeIsRealOne<unsigned long>());
    BOOST_CHECK(testTypeIsRealOne<unsigned long long>());

    BOOST_CHECK(IsRealOne(1.0));
    BOOST_CHECK(IsRealOne(1.0f));

    BOOST_CHECK(IsRealOne(0.0) == false);
    BOOST_CHECK(IsRealOne(0.0f) == false);

    BOOST_CHECK(IsRealOne(1.00001) == false);
    BOOST_CHECK(IsRealOne(0.9999f) == false);
}

BOOST_AUTO_TEST_CASE(Real_IsRealClose)
{
    BOOST_CHECK(IsRealClose(0.0, 0.0));
    BOOST_CHECK(IsRealClose(1.0, 1.0));
    BOOST_CHECK(IsRealClose(123.456, 123.456));

    BOOST_CHECK(IsRealClose(0.0f, 0.0f));
    BOOST_CHECK(IsRealClose(1.0f, 1.0f));
    BOOST_CHECK(IsRealClose(123.456f, 123.456f));

    BOOST_CHECK(IsRealClose(0.0, 1.0) == false);
    BOOST_CHECK(IsRealClose(0.0, 0.00001) == false);

    BOOST_CHECK(IsRealClose(0.0, 1.0) == false);

    BOOST_CHECK(IsRealClose(999999999999.0f, 999999999999.0001f));
    BOOST_CHECK(IsRealClose(999999999999.0, 999999999999.0001));
    BOOST_CHECK(IsRealClose(999999999999999.0L, 999999999999999.0000001L));
}

BOOST_AUTO_TEST_CASE(NotNull_Tests)
{
    // heroespath::misc::NotNull<int *> notNull; // should not compile
    // heroespath::misc::NotNull<int *> notNull { nullptr }; // should not compile
    // heroespath::misc::NotNull<int *> notNull { 0 }; // should not compile

    // std::cout << "(the next line output to the console will be an exception about a NotNull "
    //             "constructor given nullptr that is expected and can be ignored)"
    //          << std::endl;
    //
    // int * p0 { nullptr };
    // BOOST_CHECK_THROW(heroespath::misc::NotNull<int *> notNull0 { p0 }, std::exception);

    int one { 1 };
    heroespath::misc::NotNull<int *> notNull1A { &one };
    BOOST_CHECK(*notNull1A == one);
    BOOST_CHECK(*notNull1A == 1);
    BOOST_CHECK(notNull1A == &one);
    BOOST_CHECK(&one == notNull1A);
    BOOST_CHECK(notNull1A == &one);
    BOOST_CHECK(&one == notNull1A);

    const int TEMP_ONE { *notNull1A };
    BOOST_CHECK(TEMP_ONE == 1);

    BOOST_CHECK((notNull1A != &one) == false);
    BOOST_CHECK((&one != notNull1A) == false);
    BOOST_CHECK((notNull1A != &one) == false);
    BOOST_CHECK((&one != notNull1A) == false);

    int * p1 { &one };
    heroespath::misc::NotNull<int *> notNull1B { p1 };
    BOOST_CHECK(*notNull1B == one);
    BOOST_CHECK(*notNull1B == 1);

    BOOST_CHECK(notNull1A == notNull1B);
    BOOST_CHECK(notNull1A == notNull1B);
    BOOST_CHECK(*notNull1A == *notNull1B);

    heroespath::misc::NotNull<int *> notNull1C { notNull1A };

    BOOST_CHECK(notNull1A == notNull1C);
    BOOST_CHECK(notNull1A == notNull1C);
    BOOST_CHECK(*notNull1A == *notNull1C);

    char c { 'x' };
    heroespath::misc::NotNull<char *> notNullC { &c };

    // BOOST_CHECK(notNull1A != notNullC); //should not compile
    // BOOST_CHECK(notNull1A != notNullC); // should not compile

    int * p2 { new int(2) };
    heroespath::misc::NotNull<int *> notNull2 { p2 };

    BOOST_CHECK(notNull2 == p2);
    BOOST_CHECK(p2 == notNull2);
    BOOST_CHECK(notNull2 == p2);
    BOOST_CHECK(p2 == notNull2);

    // heroespath::misc::NotNull<int *> notNullX{ *p2 }; // should not compile

    BOOST_CHECK(notNull1A != notNull2);
    BOOST_CHECK(notNull1A != notNull2);
    BOOST_CHECK(*notNull1A != *notNull2);

    notNull1A = MakeNotNull(p2);

    BOOST_CHECK(notNull1A == notNull2);
    BOOST_CHECK(notNull1A == notNull2);
    BOOST_CHECK(*notNull1A == *notNull2);

    notNull1B = notNull2;

    BOOST_CHECK(notNull1B == notNull2);
    BOOST_CHECK(notNull1B == notNull2);
    BOOST_CHECK(*notNull1B == *notNull2);

    heroespath::misc::NotNull<int *> notNull3 { new int(3) };

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(*notNull2 != *notNull3);

    *notNull3 = 2;

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(*notNull2 == *notNull3);

    *notNull3 = 1;
    BOOST_CHECK(*notNull3 == 1);

    const heroespath::misc::NotNull<const int *> NOTNULL4 { new int(4) };
    BOOST_CHECK(NOTNULL4 != notNull3);
    BOOST_CHECK(NOTNULL4 != notNull3);
    BOOST_CHECK(*NOTNULL4 != *notNull3);

    // NOTNULL4 = 69; // should not compile
    //*NOTNULL4 = 0; // should NOT compile since the NotNull<T> type is a const
    // NOTNULL4 = p2; //should NOT compile because the pointer type is const

    // delete notNull2; should not compile
    delete p2;
    delete notNull3;
    delete NOTNULL4;

    boost::optional<heroespath::misc::NotNull<Thing *>> notNullThingPtrOpt { new Thing(69) };
    BOOST_CHECK(notNullThingPtrOpt.value()->get() == 69);
    BOOST_CHECK((*notNullThingPtrOpt)->get() == 69);
    delete notNullThingPtrOpt.value();
}

BOOST_AUTO_TEST_CASE(BoostOptionalThrowOnUninitTests)
{
    // In this codebase <boost/optional.hpp> is not included directly, instead a wrapper include is
    // used "misc/boost-optional-that-throws.hpp". This forces any attempted access of an
    // uninitialized boost::optional to throw a std::runtime_error. So these tests make sure that
    // boost::optional behaves in the way required by the codebase.

    boost::optional<Thing> optional { boost::none };

    BOOST_CHECK(optional.get_ptr() == nullptr);
    BOOST_CHECK(optional.get_value_or(Thing(69)).get() == 69);
    BOOST_CHECK(optional.is_initialized() == false);

    BOOST_CHECK_THROW(optional->get(), std::exception);
    BOOST_CHECK_THROW((*optional).get(), std::exception);
    BOOST_CHECK_THROW(optional.value().get(), std::exception);
    BOOST_CHECK_NO_THROW(optional.value_or(Thing(0)));
    BOOST_CHECK(optional.value_or(Thing(0)) == Thing(0));
}

BOOST_AUTO_TEST_CASE(BoostOptionalNoElementTests)
{
    // first test that NoElement works as expected
    BOOST_CHECK(NoElement_t() == NoElement_t());

    NoElement_t noElement1;
    NoElement_t noElement2;

    BOOST_CHECK(noElement1 == noElement1);
    BOOST_CHECK(noElement2 == noElement2);
    BOOST_CHECK(noElement1 == noElement2);
    BOOST_CHECK(noElement1 == NoElement_t());
    BOOST_CHECK(noElement2 == NoElement_t());

    // test that an optional of type gui::NoElement_t works in every way
    boost::optional<NoElement_t> noOpt;
    BOOST_CHECK(!noOpt);
    BOOST_CHECK(!noOpt.is_initialized());
    BOOST_CHECK(noOpt.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOpt.get_value_or(noElement1) == noElement2);
    BOOST_CHECK_THROW(noOpt.get(), std::exception);
    BOOST_CHECK(noOpt.get_ptr() == nullptr);
    BOOST_CHECK_THROW(noOpt.value(), std::exception);
    BOOST_CHECK_NO_THROW(noOpt.reset());

    boost::optional<NoElement_t> noOptInit(noElement1);
    BOOST_CHECK(noOptInit);
    BOOST_CHECK(!!noOptInit);
    BOOST_CHECK(noOptInit.is_initialized());
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement2);
    BOOST_CHECK(noOptInit.get() == noElement1);
    BOOST_CHECK(noOptInit.get() == noElement2);
    BOOST_CHECK(noOptInit.get_ptr() != nullptr);
    BOOST_CHECK(noOptInit.value() == noElement1);

    BOOST_CHECK_NO_THROW(noOptInit.reset());

    BOOST_CHECK(!noOptInit);

    noOptInit = noElement1;

    BOOST_CHECK(noOptInit);
    BOOST_CHECK(!!noOptInit);
    BOOST_CHECK(noOptInit.is_initialized());
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement1);
    BOOST_CHECK(noOptInit.get_value_or(noElement1) == noElement2);
    BOOST_CHECK(noOptInit.get() == noElement1);
    BOOST_CHECK(noOptInit.get() == noElement2);
    BOOST_CHECK(noOptInit.get_ptr() != nullptr);
    BOOST_CHECK(noOptInit.value() == noElement1);

    BOOST_CHECK(noOptInit == noElement1);
    BOOST_CHECK(noOptInit == noElement2);

    boost::optional<NoElement_t> noOptInitB(noElement1);
    BOOST_CHECK(noOptInit == noOptInitB);
    BOOST_CHECK(noOptInit.get() == noOptInitB.get());
    BOOST_CHECK(noOptInit.value() == noOptInitB.value());

    boost::optional<NoElement_t> noOptInit2(noElement2);
    BOOST_CHECK(noOptInit == noOptInit2);
    BOOST_CHECK(noOptInit.get() == noOptInit2.get());
    BOOST_CHECK(noOptInit.value() == noOptInit2.value());
}

BOOST_AUTO_TEST_CASE(BoostOptionalComparisonTests)
{
    using ThingPtr_t = heroespath::misc::NotNull<Thing *>;
    using ThingPtrOpt_t = boost::optional<ThingPtr_t>;

    Thing myThing1(1);
    ThingPtr_t thingPtr1 { &myThing1 };

    Thing myThing2(2);
    ThingPtr_t thingPtr2 { &myThing2 };

    ThingPtrOpt_t optInvalid { boost::none };
    ThingPtrOpt_t optValid1 { thingPtr1 };
    ThingPtrOpt_t optValid2 { thingPtr2 };

    BOOST_CHECK(optValid1 != optInvalid);
    BOOST_CHECK(optValid2 != optInvalid);
    BOOST_CHECK(optValid1 != optValid2);
    BOOST_CHECK(optValid1.value() != optValid2.value());
    BOOST_CHECK(optInvalid == ThingPtrOpt_t());

    // these test are the real meat of this test set
    BOOST_CHECK(optInvalid == boost::none);
    BOOST_CHECK(optValid1 != boost::none);
    BOOST_CHECK(optValid1 == thingPtr1);
    BOOST_CHECK(optValid1 != thingPtr2);

    ThingPtrOpt_t optExtra { thingPtr1 };
    BOOST_CHECK(optExtra == thingPtr1);
}
