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

#include <boost/test/unit_test.hpp>

#include "misc/not-null.hpp"
#include "misc/real.hpp"

using namespace heroespath::misc;

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

    // heroespath::misc::NotNull<int> notNullX{ *p2 }; // should not compile

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

    //*NOTNULL4 = 69; //should not compile
    // NOTNULL4.Obj() = 69; // should not compile

    // delete notNull2; should not compile
    delete p2;
    delete notNull3.Ptr();
    delete NOTNULL4.Ptr();
}
