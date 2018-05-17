// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc"

#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/real.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

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

BOOST_AUTO_TEST_CASE(Real_IsRealZero)
{
    BOOST_CHECK(IsRealZero(0.0));
    BOOST_CHECK(IsRealZero(0.0f));

    BOOST_CHECK(IsRealZero(1.0) == false);
    BOOST_CHECK(IsRealZero(1.0f) == false);

    BOOST_CHECK(IsRealZero(0.00001) == false);
    BOOST_CHECK(IsRealZero(-0.00001f) == false);
}

BOOST_AUTO_TEST_CASE(Real_IsRealOne)
{
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
}

BOOST_AUTO_TEST_CASE(NotNull_Tests)
{
    // heroespath::misc::NotNull<int *> notNull; //should not compile
    // heroespath::misc::NotNull<int *> notNull{ nullptr }; //should not compile
    // heroespath::misc::NotNull<int *> notNull{ 0 }; //should not compile

    int * p0{ nullptr };
    BOOST_CHECK_THROW(heroespath::misc::NotNull<int *> notNull0{ p0 }, std::exception);

    int one{ 1 };
    heroespath::misc::NotNull<int *> notNull1A{ &one };
    BOOST_CHECK(notNull1A.Obj() == one);
    BOOST_CHECK(notNull1A.Obj() == 1);
    // BOOST_CHECK(notNull1A != nullptr); // should not compile
    BOOST_CHECK(notNull1A.Ptr() == &one);
    BOOST_CHECK(&one == notNull1A.Ptr());
    BOOST_CHECK(notNull1A == &one);
    BOOST_CHECK(&one == notNull1A);

    const int TEMP_ONE{ *notNull1A };
    BOOST_CHECK(TEMP_ONE == 1);

    BOOST_CHECK((notNull1A.Ptr() != &one) == false);
    BOOST_CHECK((&one != notNull1A.Ptr()) == false);
    BOOST_CHECK((notNull1A != &one) == false);
    BOOST_CHECK((&one != notNull1A) == false);

    int * p1{ &one };
    heroespath::misc::NotNull<int *> notNull1B{ p1 };
    BOOST_CHECK(notNull1B.Obj() == one);
    BOOST_CHECK(notNull1B.Obj() == 1);

    BOOST_CHECK(notNull1A == notNull1B);
    BOOST_CHECK(notNull1A.Ptr() == notNull1B.Ptr());
    BOOST_CHECK(notNull1A.Obj() == notNull1B.Obj());

    heroespath::misc::NotNull<int *> notNull1C{ notNull1A };

    BOOST_CHECK(notNull1A == notNull1C);
    BOOST_CHECK(notNull1A.Ptr() == notNull1C.Ptr());
    BOOST_CHECK(notNull1A.Obj() == notNull1C.Obj());

    char c{ 'x' };
    heroespath::misc::NotNull<char *> notNullC{ &c };

    // BOOST_CHECK(notNull1A != notNullC); //should not compile
    // BOOST_CHECK(notNull1A.Ptr() != notNullC.Ptr()); // should not compile

    int * p2{ new int(2) };
    heroespath::misc::NotNull<int *> notNull2{ p2 };

    BOOST_CHECK(notNull2.Ptr() == p2);
    BOOST_CHECK(p2 == notNull2.Ptr());
    BOOST_CHECK(notNull2 == p2);
    BOOST_CHECK(p2 == notNull2);

    // heroespath::misc::NotNull<int *> notNullX{ *p2 }; // should not compile

    BOOST_CHECK(notNull1A != notNull2);
    BOOST_CHECK(notNull1A.Ptr() != notNull2.Ptr());
    BOOST_CHECK(notNull1A.Obj() != notNull2.Obj());

    notNull1A = p2;

    BOOST_CHECK(notNull1A == notNull2);
    BOOST_CHECK(notNull1A.Ptr() == notNull2.Ptr());
    BOOST_CHECK(notNull1A.Obj() == notNull2.Obj());

    notNull1B = notNull2;

    BOOST_CHECK(notNull1B == notNull2);
    BOOST_CHECK(notNull1B.Ptr() == notNull2.Ptr());
    BOOST_CHECK(notNull1B.Obj() == notNull2.Obj());

    heroespath::misc::NotNull<int *> notNull3{ new int(3) };

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2.Ptr() != notNull3.Ptr());
    BOOST_CHECK(notNull2.Obj() != notNull3.Obj());

    *notNull3 = 2;

    BOOST_CHECK(notNull2 != notNull3);
    BOOST_CHECK(notNull2.Ptr() != notNull3.Ptr());
    BOOST_CHECK(notNull2.Obj() == notNull3.Obj());

    notNull3.Obj() = 1;
    BOOST_CHECK(notNull3.Obj() == 1);

    const heroespath::misc::NotNull<int *> NOTNULL4{ new int(4) };
    BOOST_CHECK(NOTNULL4 != notNull3);
    BOOST_CHECK(NOTNULL4.Ptr() != notNull3.Ptr());
    BOOST_CHECK(NOTNULL4.Obj() != notNull3.Obj());

    // NOTNULL4 = 69; // should not compile
    // NOTNULL4.Obj() = 69; // should not compile

    // delete notNull2; should not compile
    delete p2;
    delete notNull3.Ptr();
    delete NOTNULL4.Ptr();

    boost::optional<heroespath::misc::NotNull<Thing *>> notNullThing{ new Thing(69) };
    BOOST_CHECK(notNullThing->Obj().get() == 69);
    BOOST_CHECK((*notNullThing).Obj().get() == 69);
    delete notNullThing->Ptr();
}

BOOST_AUTO_TEST_CASE(BoostOptionalThrowOnUninitTests)
{
    // In this codebase <boost/optional.hpp> is not included directly, instead a wrapper include is
    // used "misc/boost-optional-that-throws.hpp". This forces any attempted access of an
    // uninitialized boost::optional to throw a std::runtime_error. So these tests make sure that
    // boost::optional behaves in the way required by the codebase.

    boost::optional<Thing> optional{ boost::none };

    BOOST_CHECK(optional.get_ptr() == nullptr);
    BOOST_CHECK(optional.get_value_or(Thing(69)).get() == 69);
    BOOST_CHECK(optional.is_initialized() == false);

    BOOST_CHECK_THROW(optional->get(), std::exception);
    BOOST_CHECK_THROW((*optional).get(), std::exception);
    BOOST_CHECK_THROW(optional.value().get(), std::exception);
    BOOST_CHECK_THROW(optional.get().get(), std::exception);
}

BOOST_AUTO_TEST_CASE(BoostOptionalComparisonTests)
{
    using ThingPtr_t = heroespath::misc::NotNull<Thing *>;
    using ThingPtrOpt_t = boost::optional<ThingPtr_t>;

    Thing myThing1(1);
    ThingPtr_t thingPtr1{ &myThing1 };

    Thing myThing2(2);
    ThingPtr_t thingPtr2{ &myThing2 };

    ThingPtrOpt_t optInvalid{ boost::none };
    ThingPtrOpt_t optValid1{ thingPtr1 };
    ThingPtrOpt_t optValid2{ thingPtr2 };

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

    ThingPtrOpt_t optExtra{ thingPtr1 };
    BOOST_CHECK(optExtra == thingPtr1);
}
