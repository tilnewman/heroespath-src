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
#define BOOST_TEST_MODULE "HeroesPathTestModule_Game"

#include <boost/test/unit_test.hpp>

#include "Test-stuff.hpp"

#include "misc/warehouse.hpp"

#include <exception>
#include <string>

using namespace heroespath;
namespace ts = test_stuff;

// responsible for keeping a track of a static reference count
struct Counted
{
    Counted() { ++m_refCount; }
    ~Counted() { --m_refCount; }
    static int refCount() { return m_refCount; }
    const std::string ToString() const { return "Counted"; }

private:
    static int m_refCount;
};

int Counted::m_refCount{ 0 };

// responsible for implementing a warehouse of Counted objects
class CountedWarehouse
{
public:
    CountedWarehouse()
        : m_warehouse()
    {}
    Counted * store(Counted * ptr) { return m_warehouse.Store(ptr); }
    void free(Counted *& ptr) { m_warehouse.Free(ptr); }
    std::size_t size() const { return m_warehouse.Size(); }

private:
    misc::Warehouse<Counted> m_warehouse;
};

BOOST_AUTO_TEST_CASE(Warehouse_Construction)
{
    BOOST_CHECK(Counted::refCount() == 0);
    CountedWarehouse warehouse;
    BOOST_CHECK(warehouse.size() == 0);
    BOOST_CHECK_THROW(warehouse.store(nullptr), std::runtime_error);

    Counted * nPtr{ nullptr };
    BOOST_CHECK_THROW(warehouse.free(nPtr), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Warehouse_StoreAndFree)
{
    CountedWarehouse warehouse;

    auto ptr1{ new Counted() };
    BOOST_CHECK(Counted::refCount() == 1);

    BOOST_CHECK(warehouse.store(ptr1) == ptr1);
    BOOST_CHECK(Counted::refCount() == 1);
    BOOST_CHECK(warehouse.size() == 1);

    BOOST_CHECK_THROW(warehouse.store(ptr1), std::runtime_error);

    auto ptr2{ new Counted() };
    BOOST_CHECK(Counted::refCount() == 2);

    warehouse.free(ptr2);
    BOOST_CHECK(ptr2 != nullptr);

    BOOST_CHECK(warehouse.store(ptr2) == ptr2);
    BOOST_CHECK(Counted::refCount() == 2);
    BOOST_CHECK(warehouse.size() == 2);

    BOOST_CHECK_THROW(warehouse.store(ptr2), std::runtime_error);
    BOOST_CHECK(warehouse.size() == 2);
    BOOST_CHECK(Counted::refCount() == 2);

    warehouse.free(ptr1);
    BOOST_CHECK(Counted::refCount() == 1);

    BOOST_CHECK_MESSAGE(
        warehouse.size() == 2, "The warehouse slot should be set to nullptr but it should remain.");

    auto ptr3{ new Counted() };
    BOOST_CHECK(Counted::refCount() == 2);

    warehouse.store(ptr3);
    BOOST_CHECK_MESSAGE(
        warehouse.size() == 2,
        "The warehouse slot that was previously used for pt2 should now be used by ptr3.");

    warehouse.free(ptr2);
    warehouse.free(ptr3);

    BOOST_CHECK(Counted::refCount() == 0);

    BOOST_CHECK_MESSAGE(
        warehouse.size() == 2,
        "All warehouse slots should remain but be set to nullptr (be empty).");
}
