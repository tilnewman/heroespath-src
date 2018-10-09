// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#define BOOST_TEST_MODULE "HeroesPathTestModule_Misc_VectorMap"

#include "misc/vector-map.hpp"

#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4266)
#endif

#include <boost/test/unit_test.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
#pragma warning(pop)
#endif

#include <boost/type_traits.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using namespace heroespath;
using namespace heroespath::misc;

using Test_t = int;
using VectorMap_t = VectorMap<Test_t, Test_t>;
using Vector_t = std::vector<Test_t>;
using Pair_t = VectorMap_t::value_type;
using PairVec_t = VectorMap_t::container_type;

template <typename T>
inline const T copyAndReverse(const T & C)
{
    T r(C);
    std::reverse(std::begin(r), std::end(r));
    return r;
}

inline const VectorMap_t makeVectorMap(const PairVec_t & PV)
{
    VectorMap_t vm;
    for (auto const & PAIR : PV)
    {
        vm.Append(PAIR.first, PAIR.second);
    }
    return vm;
}

inline const VectorMap_t makeVectorMap(const Vector_t & V)
{
    VectorMap_t vm;
    for (auto const VALUE : V)
    {
        vm.Append(VALUE, VALUE);
    }
    return vm;
}

inline const PairVec_t makePairVec(const VectorMap_t & VM)
{
    PairVec_t pv;
    for (auto const & PAIR : VM)
    {
        pv.emplace_back(PAIR);
    }
    return pv;
}

inline const PairVec_t makePairVec(const Vector_t & V) { return makePairVec(makeVectorMap(V)); }

inline const Vector_t makeVectorCount(const std::size_t SIZE)
{
    Vector_t v;
    for (std::size_t i(0); i < SIZE; ++i)
    {
        v.push_back(static_cast<Test_t>(i));
    }
    return v;
};

inline bool areEqual(const VectorMap_t & A, const VectorMap_t & B)
{
    return (A.Size() == B.Size()) && std::equal(std::begin(A), std::end(A), std::begin(B));
}

inline bool areEqual(const VectorMap_t & VM, const PairVec_t & PV)
{
    return areEqual(VM, makeVectorMap(PV));
}

inline bool areEqual(const VectorMap_t & VM, const Vector_t & V)
{
    return areEqual(VM, makeVectorMap(V));
}

template <typename T, typename U>
void printIteratorCompareInfo(const std::string & S1, const std::string & S2)
{
    std::cout << S1 << "\n"
              << S2 << "\n\t" << boost::typeindex::type_id<T>().pretty_name() << "\n\t"
              << boost::typeindex::type_id<U>().pretty_name() << "\n\t\titer_category="
              << boost::typeindex::type_id<typename T::iterator_category>().pretty_name()
              << "\n\t\t           \"\"="
              << boost::typeindex::type_id<typename U::iterator_category>().pretty_name()
              << "\n\t\t   value_type="
              << boost::typeindex::type_id<typename T::value_type>().pretty_name()
              << "\n\t\t           \"\"="
              << boost::typeindex::type_id<typename U::value_type>().pretty_name()
              << "\n\t\t,  pointer_type="
              << boost::typeindex::type_id<typename T::pointer_type>().pretty_name()
              << "\n\t\t,          \"\"="
              << boost::typeindex::type_id<typename U::pointer_type>().pretty_name() << std::endl;
};

template <typename T>
void printIteratorInfo(const std::string & S, T)
{
    std::cout << S << ", " << boost::typeindex::type_id<T>().pretty_name() << ", \titer_category="
              << boost::typeindex::type_id<typename T::iterator_category>().pretty_name()
              << ", \t   value_type="
              << boost::typeindex::type_id<typename T::value_type>().pretty_name()
              << ", \t,  pointer_type="
              << boost::typeindex::type_id<typename T::pointer_type>().pretty_name() << std::endl;
};

inline void testSet1(VectorMap_t & vm, const std::string & MESSAGE, const std::size_t TEST_INDEX)
{
    const std::size_t TEST_SIZE(123);

    const Test_t TEST_VAL(static_cast<Test_t>(TEST_SIZE));

    Test_t ignore(0);

    const Test_t COUNT3 { 3 };

    const auto FAIL_MESSAGE { MESSAGE + ", test# " + std::to_string(TEST_INDEX) };
    const auto VM_CONST_COPY { vm };

    for (Test_t i(0); i < COUNT3; ++i)
    {
        const auto PAIR { Pair_t(i, i) };

        if (static_cast<std::size_t>(i) < vm.Size())
        {
            BOOST_CHECK_MESSAGE((vm.Exists(i)), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((VM_CONST_COPY.Exists(i)), FAIL_MESSAGE);

            ignore = TEST_VAL;
            BOOST_CHECK_MESSAGE((vm.Find(i, ignore)), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((ignore == i), FAIL_MESSAGE);

            ignore = TEST_VAL;
            BOOST_CHECK_MESSAGE((VM_CONST_COPY.Find(i, ignore)), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((ignore == i), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((vm.Find(i) == std::begin(vm) + i), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE(
                (VM_CONST_COPY.Find(i) == std::begin(VM_CONST_COPY) + i), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((*vm.Find(i) == PAIR), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((*VM_CONST_COPY.Find(i) == PAIR), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((vm.Find(PAIR) == std::begin(vm) + i), FAIL_MESSAGE);
        }
        else
        {
            BOOST_CHECK_MESSAGE((vm.Exists(i) == false), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((VM_CONST_COPY.Exists(i) == false), FAIL_MESSAGE);

            ignore = TEST_VAL;
            BOOST_CHECK_MESSAGE((vm.Find(i, ignore) == false), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((ignore == TEST_VAL), FAIL_MESSAGE);

            ignore = TEST_VAL;
            BOOST_CHECK_MESSAGE((VM_CONST_COPY.Find(i, ignore) == false), FAIL_MESSAGE);
            BOOST_CHECK_MESSAGE((ignore == TEST_VAL), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((vm.Find(i) == std::end(vm)), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((VM_CONST_COPY.Find(i) == std::end(VM_CONST_COPY)), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE((vm.Find(PAIR) == std::end(vm)), FAIL_MESSAGE);

            BOOST_CHECK_MESSAGE(
                (VM_CONST_COPY.Find(PAIR) == std::end(VM_CONST_COPY)), FAIL_MESSAGE);
        }
    }
}

inline void relationalTests(
    const VectorMap_t & VMA,
    const VectorMap_t & VMB,
    const PairVec_t & PVA,
    const PairVec_t & PVB,
    const std::string & FAIL_MESSAGE)
{
    BOOST_CHECK(areEqual(VMA, PVA));
    BOOST_CHECK(areEqual(VMB, PVB));

    PairVec_t pvAToUse(PVA);
    PairVec_t pvBToUse(PVB);

    std::sort(std::begin(pvAToUse), std::end(pvAToUse));
    std::sort(std::begin(pvBToUse), std::end(pvBToUse));

    BOOST_CHECK_MESSAGE(((VMA == VMB) == (pvAToUse == pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB == VMA) == (pvBToUse == pvAToUse)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((VMA != VMB) == (pvAToUse != pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB != VMA) == (pvBToUse != pvAToUse)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((VMA < VMB) == (pvAToUse < pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB < VMA) == (pvBToUse < pvAToUse)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((VMA > VMB) == (pvAToUse > pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB > VMA) == (pvBToUse > pvAToUse)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((VMA <= VMB) == (pvAToUse <= pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB <= VMA) == (pvBToUse <= pvAToUse)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((VMA >= VMB) == (pvAToUse >= pvBToUse)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((VMB >= VMA) == (pvBToUse >= pvAToUse)), FAIL_MESSAGE);

    auto vmA(VMA);
    auto vmB(VMB);
    auto pvAToUseNonConst(pvAToUse);
    auto pvBToUseNonConst(pvBToUse);

    BOOST_CHECK_MESSAGE(((vmA == vmB) == (pvAToUseNonConst == pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB == vmA) == (pvBToUseNonConst == pvAToUseNonConst)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((vmA != vmB) == (pvAToUseNonConst != pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB != vmA) == (pvBToUseNonConst != pvAToUseNonConst)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((vmA < vmB) == (pvAToUseNonConst < pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB < vmA) == (pvBToUseNonConst < pvAToUseNonConst)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((vmA > vmB) == (pvAToUseNonConst > pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB > vmA) == (pvBToUseNonConst > pvAToUseNonConst)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((vmA <= vmB) == (pvAToUseNonConst <= pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB <= vmA) == (pvBToUseNonConst <= pvAToUseNonConst)), FAIL_MESSAGE);

    BOOST_CHECK_MESSAGE(((vmA >= vmB) == (pvAToUseNonConst >= pvBToUseNonConst)), FAIL_MESSAGE);
    BOOST_CHECK_MESSAGE(((vmB >= vmA) == (pvBToUseNonConst >= pvAToUseNonConst)), FAIL_MESSAGE);
}

inline void relationalTestsAll(const Vector_t & A, const Vector_t & B)
{
    const auto IS_EQUAL_ORDERLESS { misc::Vector::OrderlessCompareEqual(A, B) };

    std::ostringstream ss;
    ss << "relationalTests[";
    for (std::size_t i(0); i < std::min(A.size(), std::size_t(4)); ++i)
    {
        if (0 != i)
            ss << ",";
        ss << std::to_string(A[i]);
    }
    ss << "][";
    for (std::size_t i(0); i < std::min(B.size(), std::size_t(4)); ++i)
    {
        if (0 != i)
            ss << ",";
        ss << std::to_string(B[i]);
    }
    ss << "]" << ((IS_EQUAL_ORDERLESS) ? "(ORDERLESS_EQUAL)" : "");
    const auto FAIL_MESSAGE { ss.str() };

    const VectorMap_t VMA = makeVectorMap(A);
    const VectorMap_t VMB = makeVectorMap(B);

    const PairVec_t PVA = makePairVec(A);
    const PairVec_t PVB = makePairVec(B);

    relationalTests(VMA, VMB, PVA, PVB, FAIL_MESSAGE + " A-B");

    const VectorMap_t VMA_REV = copyAndReverse(VMA);
    const VectorMap_t VMB_REV = copyAndReverse(VMB);

    const PairVec_t PVA_REV = copyAndReverse(PVA);
    const PairVec_t PVB_REV = copyAndReverse(PVB);

    relationalTests(VMA, VMB_REV, PVA, PVB_REV, FAIL_MESSAGE + " A-BR");
    relationalTests(VMA_REV, VMB, PVA_REV, PVB, FAIL_MESSAGE + " AR-B");
    relationalTests(VMA_REV, VMB_REV, PVA_REV, PVB_REV, FAIL_MESSAGE + " AR-BR");
}

BOOST_AUTO_TEST_CASE(Misc_VectorMap_Tests)
{
    const std::size_t TEST_SIZE(123);

    const Test_t TEST_VAL(static_cast<Test_t>(TEST_SIZE));

    Test_t ignore(0);

    const Test_t COUNT3 { 3 };
    const Test_t COUNT32K { 32769 };

    const Pair_t P_0(0, 0);
    const Pair_t P_1(1, 1);
    const Pair_t P_2(2, 2);

    const Vector_t V_EMPTY;
    const Vector_t V_0(1, 0);
    const Vector_t V_1(1, 1);
    const Vector_t V_2(1, 2);
    const Vector_t V_COUNT3(makeVectorCount(COUNT3));
    const Vector_t V_COUNT3_REV(copyAndReverse(V_COUNT3));
    const Vector_t V_COUNT32K(makeVectorCount(COUNT32K));
    const Vector_t V_COUNT32K_REV(copyAndReverse(V_COUNT32K));

    const PairVec_t PV_EMPTY;
    const PairVec_t PV_0(makePairVec(V_0));
    const PairVec_t PV_1(makePairVec(V_1));
    const PairVec_t PV_2(makePairVec(V_2));
    const PairVec_t PV_COUNT3(makePairVec(V_COUNT3));
    const PairVec_t PV_COUNT3_REV(copyAndReverse(PV_COUNT3));
    const PairVec_t PV_COUNT32K(makePairVec(V_COUNT32K));
    const PairVec_t PV_COUNT32K_REV(copyAndReverse(PV_COUNT32K));

    const VectorMap_t VM_EMPTY;
    const VectorMap_t VM_0(makeVectorMap(V_0));
    const VectorMap_t VM_1(makeVectorMap(V_1));
    const VectorMap_t VM_2(makeVectorMap(V_2));
    const VectorMap_t VM_COUNT3(makeVectorMap(V_COUNT3));
    const VectorMap_t VM_COUNT3_REV(copyAndReverse(VM_COUNT3));
    const VectorMap_t VM_COUNT32K(makeVectorMap(V_COUNT32K));
    const VectorMap_t VM_COUNT32K_REV(copyAndReverse(VM_COUNT32K));

    // test helper variables and helper functions
    {
        BOOST_CHECK(V_EMPTY.size() == 0);
        BOOST_CHECK(V_0.size() == 1);
        BOOST_CHECK(V_1.size() == 1);
        BOOST_CHECK(V_2.size() == 1);
        BOOST_CHECK(V_COUNT3.size() == COUNT3);
        BOOST_CHECK(V_COUNT32K.size() == COUNT32K);
        //
        BOOST_CHECK(V_0[0] == 0);
        BOOST_CHECK(V_1[0] == 1);
        BOOST_CHECK(V_2[0] == 2);
        BOOST_CHECK((V_COUNT3[0] == 0) && (V_COUNT3[1] == 1) && (V_COUNT3[2] == 2));
        //
        BOOST_CHECK(V_0 == V_0);
        BOOST_CHECK(V_0 != V_1);

        BOOST_CHECK(PV_EMPTY.size() == 0);
        BOOST_CHECK(PV_0.size() == 1);
        BOOST_CHECK(PV_1.size() == 1);
        BOOST_CHECK(PV_2.size() == 1);
        BOOST_CHECK(PV_COUNT3.size() == COUNT3);
        BOOST_CHECK(PV_COUNT32K.size() == COUNT32K);
        //
        BOOST_CHECK((PV_0[0].first == 0) && (PV_0[0].second == 0));
        BOOST_CHECK((PV_1[0].first == 1) && (PV_1[0].second == 1));
        BOOST_CHECK((PV_2[0].first == 2) && (PV_2[0].second == 2));

        BOOST_CHECK(
            (PV_COUNT3[0].first == 0) && (PV_COUNT3[1].first == 1) && (PV_COUNT3[2].first == 2));

        BOOST_CHECK(
            (PV_COUNT3[0].second == 0) && (PV_COUNT3[1].second == 1) && (PV_COUNT3[2].second == 2));
        //
        BOOST_CHECK(PV_0 == PV_0);
        BOOST_CHECK(PV_0 != PV_1);

        BOOST_CHECK(VM_EMPTY.Size() == 0);
        BOOST_CHECK(VM_0.Size() == 1);
        BOOST_CHECK(VM_1.Size() == 1);
        BOOST_CHECK(VM_2.Size() == 1);
        BOOST_CHECK(VM_COUNT3.Size() == COUNT3);
        BOOST_CHECK(VM_COUNT32K.Size() == COUNT32K);

        BOOST_CHECK(VM_0.Front() == VM_0.Back());
        BOOST_CHECK(VM_COUNT3.Front() == P_0);
        BOOST_CHECK(VM_COUNT3.Back() == P_2);

        BOOST_CHECK(areEqual(VM_0, V_0));
        BOOST_CHECK(areEqual(VM_0, PV_0));
        BOOST_CHECK(areEqual(VM_0, VM_0));

        BOOST_CHECK(areEqual(VM_0, V_1) == false);
        BOOST_CHECK(areEqual(VM_0, PV_1) == false);
        BOOST_CHECK(areEqual(VM_0, VM_1) == false);

        BOOST_CHECK(areEqual(VM_COUNT32K, V_COUNT32K));
        BOOST_CHECK(areEqual(VM_COUNT32K, PV_COUNT32K));
        BOOST_CHECK(areEqual(VM_COUNT32K, VM_COUNT32K));

        BOOST_CHECK(areEqual(VM_COUNT32K, V_COUNT3) == false);
        BOOST_CHECK(areEqual(VM_COUNT32K, PV_COUNT3) == false);
        BOOST_CHECK(areEqual(VM_COUNT32K, VM_COUNT3) == false);

        BOOST_CHECK(areEqual(VM_COUNT3, makeVectorMap(V_COUNT3)));
        BOOST_CHECK(areEqual(VM_COUNT3, makeVectorMap(PV_COUNT3)));

        BOOST_CHECK(areEqual(VM_COUNT3, makeVectorMap(V_COUNT32K)) == false);
        BOOST_CHECK(areEqual(VM_COUNT3, makeVectorMap(PV_COUNT32K)) == false);

        BOOST_CHECK(areEqual(VM_COUNT3, VM_COUNT3_REV) == false);
        BOOST_CHECK(areEqual(VM_COUNT3, copyAndReverse(VM_COUNT3_REV)));
    }

    // test when empty
    {
        const VectorMap_t vmEmpty1;
        BOOST_CHECK(areEqual(vmEmpty1, VM_EMPTY));
        BOOST_CHECK(vmEmpty1 == VM_EMPTY);
        BOOST_CHECK((vmEmpty1 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty1 < VM_EMPTY) == false);

        VectorMap_t vmEmpty2;
        BOOST_CHECK(areEqual(vmEmpty2, VM_EMPTY));
        BOOST_CHECK(vmEmpty2 == VM_EMPTY);
        BOOST_CHECK((vmEmpty2 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty2 < VM_EMPTY) == false);

        VectorMap_t vmEmpty4 = makeVectorMap(V_COUNT3);
        vmEmpty4.Clear();
        BOOST_CHECK(areEqual(vmEmpty4, VM_EMPTY));
        BOOST_CHECK(vmEmpty4 == VM_EMPTY);
        BOOST_CHECK((vmEmpty4 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty4 < VM_EMPTY) == false);

        VectorMap_t vmEmpty5 = makeVectorMap(V_COUNT3);
        for (int i(0); i < COUNT3; ++i)
        {
            vmEmpty5.Erase(i);
        }
        BOOST_CHECK(areEqual(vmEmpty5, VM_EMPTY));
        BOOST_CHECK(vmEmpty5 == VM_EMPTY);
        BOOST_CHECK((vmEmpty5 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty5 < VM_EMPTY) == false);

        VectorMap_t vmEmpty6 = vmEmpty1;
        BOOST_CHECK(areEqual(vmEmpty6, VM_EMPTY));
        BOOST_CHECK(vmEmpty6 == VM_EMPTY);
        BOOST_CHECK((vmEmpty6 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty6 < VM_EMPTY) == false);

        const VectorMap_t vmEmpty7(vmEmpty1);
        BOOST_CHECK(areEqual(vmEmpty7, VM_EMPTY));
        BOOST_CHECK(vmEmpty7 == VM_EMPTY);
        BOOST_CHECK((vmEmpty7 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty7 < VM_EMPTY) == false);

        const VectorMap_t vmEmpty8 { VectorMap_t() };
        BOOST_CHECK(areEqual(vmEmpty8, VM_EMPTY));
        BOOST_CHECK(vmEmpty8 == VM_EMPTY);
        BOOST_CHECK((vmEmpty8 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty8 < VM_EMPTY) == false);

        const VectorMap_t vmEmptyToMove1;
        const VectorMap_t vmEmpty9(std::move(vmEmptyToMove1));
        BOOST_CHECK(areEqual(vmEmpty9, VM_EMPTY));
        BOOST_CHECK(vmEmpty9 == VM_EMPTY);
        BOOST_CHECK((vmEmpty9 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty9 < VM_EMPTY) == false);

        const VectorMap_t vmEmptyToMove2;
        const VectorMap_t vmEmpty10 = std::move(vmEmptyToMove2);
        BOOST_CHECK(areEqual(vmEmpty10, VM_EMPTY));
        BOOST_CHECK(vmEmpty10 == VM_EMPTY);
        BOOST_CHECK((vmEmpty10 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty10 < VM_EMPTY) == false);

        const VectorMap_t vmEmptyToMove3;
        VectorMap_t vmEmpty11 = makeVectorMap(V_COUNT3);
        vmEmpty11 = std::move(vmEmptyToMove3);
        BOOST_CHECK(areEqual(vmEmpty11, VM_EMPTY));
        BOOST_CHECK(vmEmpty11 == VM_EMPTY);
        BOOST_CHECK((vmEmpty11 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty11 < VM_EMPTY) == false);

        VectorMap_t vmEmpty12;
        vmEmpty12.Reserve(COUNT3);
        BOOST_CHECK(areEqual(vmEmpty12, VM_EMPTY));
        BOOST_CHECK(vmEmpty12 == VM_EMPTY);
        BOOST_CHECK((vmEmpty12 != VM_EMPTY) == false);
        BOOST_CHECK((vmEmpty12 < VM_EMPTY) == false);

        //

        for (int i(0); i < COUNT3; ++i)
        {
            BOOST_CHECK_NO_THROW(vmEmpty2.Clear());
            BOOST_CHECK_NO_THROW(vmEmpty2.Erase(i));
            BOOST_CHECK(areEqual(vmEmpty2, vmEmpty1));
            BOOST_CHECK(vmEmpty2 == vmEmpty1);
            BOOST_CHECK((vmEmpty2 != vmEmpty1) == false);
            BOOST_CHECK(vmEmpty2.Exists(i) == false);
            BOOST_CHECK((vmEmpty2.Exists(i) != false) == false);
            BOOST_CHECK(vmEmpty2.Find(i) == std::end(vmEmpty2));
            BOOST_CHECK((vmEmpty2.Find(i) != std::end(vmEmpty2)) == false);
            BOOST_CHECK(vmEmpty2.Find(i) == vmEmpty2.end());
            BOOST_CHECK((vmEmpty2.Find(i) != vmEmpty2.end()) == false);
            BOOST_CHECK(vmEmpty2.Find(i, ignore) == false);
            BOOST_CHECK((vmEmpty2.Find(i, ignore) != false) == false);
            BOOST_CHECK_NO_THROW(vmEmpty2.Size());
            BOOST_CHECK(vmEmpty2.Empty());
            // BOOST_CHECK_THROW(vmEmpty2.Front(), std::exception);
            // BOOST_CHECK_THROW(vmEmpty2.Back(), std::exception);

            // BOOST_CHECK_NO_THROW(VM_EMPTY.Clear());
            // BOOST_CHECK_NO_THROW(VM_EMPTY.Erase(i));
            BOOST_CHECK(areEqual(vmEmpty1, VM_EMPTY));
            BOOST_CHECK(vmEmpty1 == VM_EMPTY);
            BOOST_CHECK((vmEmpty1 != VM_EMPTY) == false);
            BOOST_CHECK(VM_EMPTY.Exists(i) == false);
            BOOST_CHECK((VM_EMPTY.Exists(i) != false) == false);
            BOOST_CHECK(VM_EMPTY.Find(i) == std::end(VM_EMPTY));
            BOOST_CHECK((VM_EMPTY.Find(i) != std::end(VM_EMPTY)) == false);
            BOOST_CHECK(VM_EMPTY.Find(i) == VM_EMPTY.end());
            BOOST_CHECK((VM_EMPTY.Find(i) != VM_EMPTY.end()) == false);
            BOOST_CHECK(VM_EMPTY.Find(i, ignore) == false);
            BOOST_CHECK((VM_EMPTY.Find(i, ignore) != false) == false);
            BOOST_CHECK_NO_THROW(VM_EMPTY.Size());
            BOOST_CHECK(VM_EMPTY.Empty());
            // BOOST_CHECK_THROW(VM_EMPTY.Front(), std::exception);
            // BOOST_CHECK_THROW(VM_EMPTY.Back(), std::exception);
        }
    }

    // test when size 1
    {
        VectorMap_t vmS1Test1(VM_0);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));
        BOOST_CHECK(vmS1Test1 == VM_0);
        BOOST_CHECK((vmS1Test1 != VM_0) == false);
        BOOST_CHECK((vmS1Test1 < VM_0) == false);

        BOOST_CHECK(vmS1Test1.Empty() == false);
        BOOST_CHECK(vmS1Test1.Size() == 1);
        BOOST_CHECK(vmS1Test1.Front() == vmS1Test1.Back());
        BOOST_CHECK(vmS1Test1.AppendIfKeyNotFound(P_0) == false);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));
        BOOST_CHECK(vmS1Test1.Exists(0));
        BOOST_CHECK(vmS1Test1.Exists(P_0));
        BOOST_CHECK(vmS1Test1.Exists(1) == false);
        BOOST_CHECK(vmS1Test1.Exists(P_1) == false);
        BOOST_CHECK(vmS1Test1.Exists(Pair_t(0, 1)) == false);
        BOOST_CHECK(vmS1Test1.Exists(Pair_t(1, 0)) == false);
        BOOST_CHECK(vmS1Test1[0] == 0);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));
        ignore = TEST_VAL;
        BOOST_CHECK(vmS1Test1.Find(0, ignore));
        BOOST_CHECK(ignore == 0);
        ignore = TEST_VAL;
        BOOST_CHECK(vmS1Test1.Find(1, ignore) == false);
        BOOST_CHECK(ignore == TEST_VAL);
        BOOST_CHECK(vmS1Test1.Find(0) == std::begin(vmS1Test1));
        BOOST_CHECK(vmS1Test1.Find(P_0) == std::begin(vmS1Test1));
        BOOST_CHECK(vmS1Test1.Erase(P_1) == 0);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));
        BOOST_CHECK(vmS1Test1.Erase(1) == 0);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));
        vmS1Test1.Reserve(TEST_VAL);
        BOOST_CHECK(areEqual(vmS1Test1, VM_0));

        const VectorMap_t vmS1Test2(VM_1);
        BOOST_CHECK(areEqual(vmS1Test2, VM_1));
        BOOST_CHECK(vmS1Test2 == VM_1);
        BOOST_CHECK((vmS1Test2 != VM_1) == false);
        BOOST_CHECK((vmS1Test2 < VM_1) == false);

        VectorMap_t vmS1Test5;
        vmS1Test5.Append(P_0);
        BOOST_CHECK(areEqual(vmS1Test5, VM_0));
        BOOST_CHECK(vmS1Test5 == VM_0);
        BOOST_CHECK((vmS1Test5 != VM_0) == false);
        BOOST_CHECK((vmS1Test5 < VM_0) == false);

        VectorMap_t vmS1Test6;
        vmS1Test6.Append(P_1);
        BOOST_CHECK(areEqual(vmS1Test6, VM_1));
        BOOST_CHECK(vmS1Test6 == VM_1);
        BOOST_CHECK((vmS1Test6 != VM_1) == false);
        BOOST_CHECK((vmS1Test6 < VM_1) == false);

        const VectorMap_t vmS1TestToMove1(VM_2);
        const VectorMap_t vmS1Test9(std::move(vmS1TestToMove1));
        BOOST_CHECK(areEqual(vmS1Test9, VM_2));
        BOOST_CHECK(vmS1Test9 == VM_2);
        BOOST_CHECK((vmS1Test9 != VM_2) == false);
        BOOST_CHECK((vmS1Test9 < VM_2) == false);

        const VectorMap_t vmS1TestToMove2(VM_1);
        const VectorMap_t vmS1Test10 = std::move(vmS1TestToMove2);
        BOOST_CHECK(areEqual(vmS1Test10, VM_1));
        BOOST_CHECK(vmS1Test10 == VM_1);
        BOOST_CHECK((vmS1Test10 != VM_1) == false);
        BOOST_CHECK((vmS1Test10 < VM_1) == false);

        const VectorMap_t vmS1TestToMove3(VM_2);
        VectorMap_t vmS1Test11 = makeVectorMap(V_COUNT3);
        vmS1Test11 = std::move(vmS1TestToMove3);
        BOOST_CHECK(areEqual(vmS1Test11, VM_2));
        BOOST_CHECK(vmS1Test11 == VM_2);
        BOOST_CHECK((vmS1Test11 != VM_2) == false);
        BOOST_CHECK((vmS1Test11 < VM_2) == false);

        VectorMap_t vmS1Test12(VM_1);
        vmS1Test12.Reserve(COUNT3);
        BOOST_CHECK(areEqual(vmS1Test12, VM_1));
        BOOST_CHECK(vmS1Test12 == VM_1);
        BOOST_CHECK((vmS1Test12 != VM_1) == false);
        BOOST_CHECK((vmS1Test12 < VM_1) == false);
    }

    // iterator tests
    {
        BOOST_CHECK(VM_COUNT3.begin() == std::begin(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.rbegin() == std::rbegin(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.cbegin() == std::cbegin(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.crbegin() == std::crbegin(VM_COUNT3));
        //
        BOOST_CHECK(VM_COUNT3.end() == std::end(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.rend() == std::rend(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.cend() == std::cend(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.crend() == std::crend(VM_COUNT3));
        //
        BOOST_CHECK(VM_COUNT3.begin() == std::cbegin(VM_COUNT3));
        BOOST_CHECK(VM_COUNT3.rbegin() == std::crbegin(VM_COUNT3));

        // these will compile but not actually work even for something like std::vector<int>
        // BOOST_CHECK(VM_COUNT3.end() == const VectorMap_t::const_iterator());
        // BOOST_CHECK(VM_COUNT3.rend() == const VectorMap_t::const_reverse_iterator());
        // BOOST_CHECK(VM_COUNT3.cend() == const VectorMap_t::const_iterator());
        // BOOST_CHECK(VM_COUNT3.crend() == const VectorMap_t::const_reverse_iterator());

        VectorMap_t vmCount3Copy(VM_COUNT3);

        BOOST_CHECK(vmCount3Copy.begin() == std::begin(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.rbegin() == std::rbegin(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.cbegin() == std::cbegin(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.crbegin() == std::crbegin(vmCount3Copy));
        //
        BOOST_CHECK(vmCount3Copy.end() == std::end(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.rend() == std::rend(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.cend() == std::cend(vmCount3Copy));
        BOOST_CHECK(vmCount3Copy.crend() == std::crend(vmCount3Copy));

        // these will compile but not actually work even for something like std::vector<int>
        // BOOST_CHECK(vmCount3Copy.end() == VectorMap_t::iterator());
        // BOOST_CHECK(vmCount3Copy.rend() == VectorMap_t::reverse_iterator());
        // BOOST_CHECK(vmCount3Copy.crend() == VectorMap_t::const_reverse_iterator());
    }

    // more iterator tests
    {
        VectorMap_t vmCount3Copy(VM_COUNT3);

        auto origIter = std::cbegin(VM_COUNT3);
        auto origIterEnd = std::cend(VM_COUNT3);

        auto origConstIter = std::cbegin(VM_COUNT3);
        auto origConstIterEnd = std::cend(VM_COUNT3);

        auto origRevIter = std::crbegin(VM_COUNT3);
        auto origRevIterEnd = std::crend(VM_COUNT3);

        auto origConstRevIter = std::crbegin(VM_COUNT3);
        auto origConstRevIterEnd = std::crend(VM_COUNT3);

        //

        auto copyIter = std::cbegin(vmCount3Copy);
        auto copyIterEnd = std::cend(vmCount3Copy);

        auto copyConstIter = std::cbegin(vmCount3Copy);
        auto copyConstIterEnd = std::cend(vmCount3Copy);

        auto copyRevIter = std::crbegin(vmCount3Copy);
        auto copyRevIterEnd = std::crend(vmCount3Copy);

        auto copyConstRevIter = std::crbegin(vmCount3Copy);
        auto copyConstRevIterEnd = std::crend(vmCount3Copy);

        // this will compile but not actually work even for something like std::vector<int>
        // BOOST_CHECK(origIterEnd == copyIterEnd);

        BOOST_CHECK(origIter != origIterEnd);
        BOOST_CHECK(origConstIter != origConstIterEnd);
        BOOST_CHECK(origRevIter != origRevIterEnd);
        BOOST_CHECK(origConstRevIter != origConstRevIterEnd);

        BOOST_CHECK(copyIter != copyIterEnd);
        BOOST_CHECK(copyConstIter != copyConstIterEnd);
        BOOST_CHECK(copyRevIter != copyRevIterEnd);
        BOOST_CHECK(copyConstRevIter != copyConstRevIterEnd);

        BOOST_CHECK(origIterEnd != copyIterEnd);
        BOOST_CHECK(origConstIterEnd != copyConstIterEnd);
        BOOST_CHECK(origRevIterEnd != copyRevIterEnd);
        BOOST_CHECK(origConstRevIterEnd != copyConstRevIterEnd);

        int counter(0);

        do
        {
            BOOST_CHECK(*origIter == Pair_t(counter, counter));
            BOOST_CHECK(*origIter == *origConstIter);
            BOOST_CHECK(*origIter == *copyIter);
            BOOST_CHECK(*origIter == *copyConstIter);

            BOOST_CHECK(
                *origRevIter == Pair_t(Test_t(COUNT3 - 1) - counter, Test_t(COUNT3 - 1) - counter));

            BOOST_CHECK(*origRevIter == *copyConstRevIter);
            BOOST_CHECK(*origRevIter == *copyRevIter);
            BOOST_CHECK(*origRevIter == *copyConstRevIter);

            ++origIter;
            ++origConstIter;
            ++origRevIter;
            ++origConstRevIter;

            ++copyIter;
            ++copyConstIter;
            ++copyRevIter;
            ++copyConstRevIter;

        } while (vmCount3Copy.Size() != static_cast<std::size_t>(++counter));

        BOOST_CHECK(vmCount3Copy.Size() == static_cast<std::size_t>(counter));
        BOOST_CHECK(origIter == origIterEnd);
        BOOST_CHECK(origConstIter == origConstIterEnd);
        BOOST_CHECK(origRevIter == origRevIterEnd);
        BOOST_CHECK(origConstRevIter == origConstRevIterEnd);

        BOOST_CHECK(copyIter == copyIterEnd);
        BOOST_CHECK(copyConstIter == copyConstIterEnd);
        BOOST_CHECK(copyRevIter == copyRevIterEnd);
        BOOST_CHECK(copyConstRevIter == copyConstRevIterEnd);
    }

    // Append() / Exists() / Size() / Empty() / Find() / Clear() / Front() /Back() tests
    {
        {
            VectorMap_t vm(VM_EMPTY);
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "Append()", 0);

            vm.Append(P_0);
            BOOST_CHECK(areEqual(vm, VM_0));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 1);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (0 == i));
            BOOST_CHECK((vm.Front() == P_0) && (vm.Back() == P_0));
            testSet1(vm, "Append()", 1);

            vm.Append(1, 1);
            BOOST_CHECK(areEqual(vm, makeVectorMap({ 0, 1 })));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 2);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (2 != i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_1);
            testSet1(vm, "Append()", 2);

            vm.Append(P_2);
            BOOST_CHECK(areEqual(vm, VM_COUNT3));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 3);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_2);
            testSet1(vm, "Append()", 3);

            vm.Clear();
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == false);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "Append()", 4);

            vm.Clear();
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == false);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "Append()", 5);
        }

        {
            VectorMap_t vm(VM_EMPTY);
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == false);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "AppendIfKeyNotFound()", 0);

            BOOST_CHECK(vm.AppendIfKeyNotFound(P_0));
            BOOST_CHECK(areEqual(vm, VM_0));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 1);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (0 == i));
            BOOST_CHECK((vm.Front() == P_0) && (vm.Back() == P_0));
            testSet1(vm, "AppendIfKeyNotFound()", 1);

            BOOST_CHECK(vm.AppendIfKeyNotFound(P_0) == false);
            BOOST_CHECK(areEqual(vm, VM_0));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 1);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (0 == i));
            BOOST_CHECK((vm.Front() == P_0) && (vm.Back() == P_0));
            testSet1(vm, "AppendIfKeyNotFound()", 2);

            BOOST_CHECK(vm.AppendIfKeyNotFound(0, 0) == false);
            BOOST_CHECK(areEqual(vm, VM_0));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 1);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (0 == i));
            BOOST_CHECK((vm.Front() == P_0) && (vm.Back() == P_0));
            testSet1(vm, "AppendIfKeyNotFound()", 3);

            BOOST_CHECK(vm.AppendIfKeyNotFound(1, 1));
            BOOST_CHECK(areEqual(vm, makeVectorMap({ 0, 1 })));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 2);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (2 != i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_1);
            testSet1(vm, "AppendIfKeyNotFound()", 4);

            BOOST_CHECK(vm.AppendIfKeyNotFound(1, 1) == false);
            BOOST_CHECK(areEqual(vm, makeVectorMap({ 0, 1 })));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 2);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (2 != i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_1);
            testSet1(vm, "AppendIfKeyNotFound()", 5);

            BOOST_CHECK(vm.AppendIfKeyNotFound(P_1) == false);
            BOOST_CHECK(areEqual(vm, makeVectorMap({ 0, 1 })));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 2);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == (2 != i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_1);
            testSet1(vm, "AppendIfKeyNotFound()", 6);

            BOOST_CHECK(vm.AppendIfKeyNotFound(P_2));
            BOOST_CHECK(areEqual(vm, VM_COUNT3));
            BOOST_CHECK(vm.Empty() == false);
            BOOST_CHECK(vm.Size() == 3);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i));
            BOOST_CHECK(vm.Front() == P_0);
            BOOST_CHECK(vm.Back() == P_2);
            testSet1(vm, "AppendIfKeyNotFound()", 7);

            vm.Clear();
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == false);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "AppendIfKeyNotFound()", 8);

            vm.Clear();
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
            BOOST_CHECK(vm.Empty());
            BOOST_CHECK(vm.Size() == 0);
            for (Test_t i(0); i < COUNT3; ++i)
                BOOST_CHECK(vm.Exists(i) == false);
            // BOOST_CHECK_THROW(vm.Front(), std::exception);
            // BOOST_CHECK_THROW(vm.Back(), std::exception);
            testSet1(vm, "AppendIfKeyNotFound()", 9);
        }

        // Erase() tests
        {
            VectorMap_t vm(VM_EMPTY);
            BOOST_CHECK(vm.Erase(0) == 0);
            BOOST_CHECK(vm.Erase(P_0) == 0);

            vm.Append(P_0);
            vm.Append(P_1);
            vm.Append(P_2);
            BOOST_CHECK(areEqual(vm, { 0, 1, 2 }));

            BOOST_CHECK(vm.Erase(3) == 0);
            BOOST_CHECK(vm.Erase(Pair_t(3, 3)) == 0);

            BOOST_CHECK(areEqual(vm, { 0, 1, 2 }));

            BOOST_CHECK(vm.Erase(0) == 1);
            BOOST_CHECK(areEqual(vm, { 1, 2 }));

            BOOST_CHECK(vm.Erase(2) == 1);
            BOOST_CHECK(areEqual(vm, std::vector<int> { 1 }));

            BOOST_CHECK(vm.Erase(1) == 1);
            BOOST_CHECK(areEqual(vm, VM_EMPTY));

            vm.Append(P_1);
            vm.Append(P_1);
            vm.Append(P_1);

            BOOST_CHECK(vm.Erase(1) == 3);
            BOOST_CHECK(areEqual(vm, VM_EMPTY));
        }

        // index operator tests
        {
            VectorMap_t vm(VM_COUNT3);

            BOOST_CHECK(vm[0] == 0);
            BOOST_CHECK(vm[1] == 1);
            BOOST_CHECK(vm[2] == 2);
            BOOST_CHECK(areEqual(vm, VM_COUNT3));

            vm[0] = 12;
            vm[1] = 34;
            vm[2] = 56;
            BOOST_CHECK(vm.Size() == 3);

            BOOST_CHECK(vm[0] == 12);
            BOOST_CHECK(vm[1] == 34);
            BOOST_CHECK(vm[2] == 56);
            BOOST_CHECK(vm.Size() == 3);

            vm[3] = 78;
            BOOST_CHECK(vm[0] == 12);
            BOOST_CHECK(vm[1] == 34);
            BOOST_CHECK(vm[2] == 56);
            BOOST_CHECK(vm[3] == 78);
            BOOST_CHECK(vm.Size() == 4);
        }

        // relational tests
        {
            BOOST_CHECK(VM_EMPTY == VM_EMPTY);
            BOOST_CHECK((VM_EMPTY != VM_EMPTY) == false);
            BOOST_CHECK((VM_EMPTY < VM_EMPTY) == false);
            BOOST_CHECK((VM_EMPTY > VM_EMPTY) == false);
            BOOST_CHECK(VM_EMPTY <= VM_EMPTY);
            BOOST_CHECK(VM_EMPTY >= VM_EMPTY);

            BOOST_CHECK(VM_0 == VM_0);
            BOOST_CHECK((VM_0 != VM_0) == false);
            BOOST_CHECK((VM_0 < VM_0) == false);
            BOOST_CHECK((VM_0 > VM_0) == false);
            BOOST_CHECK(VM_0 <= VM_0);
            BOOST_CHECK(VM_0 >= VM_0);

            BOOST_CHECK(VM_COUNT3 == VM_COUNT3);
            BOOST_CHECK((VM_COUNT3 != VM_COUNT3) == false);
            BOOST_CHECK((VM_COUNT3 < VM_COUNT3) == false);
            BOOST_CHECK((VM_COUNT3 > VM_COUNT3) == false);
            BOOST_CHECK(VM_COUNT3 <= VM_COUNT3);
            BOOST_CHECK(VM_COUNT3 >= VM_COUNT3);

            BOOST_CHECK(VM_COUNT32K == VM_COUNT32K);
            BOOST_CHECK((VM_COUNT32K != VM_COUNT32K) == false);
            BOOST_CHECK((VM_COUNT32K < VM_COUNT32K) == false);
            BOOST_CHECK((VM_COUNT32K > VM_COUNT32K) == false);
            BOOST_CHECK(VM_COUNT32K <= VM_COUNT32K);
            BOOST_CHECK(VM_COUNT32K >= VM_COUNT32K);

            BOOST_CHECK(VM_COUNT3 == VM_COUNT3_REV);
            BOOST_CHECK((VM_COUNT3 != VM_COUNT3_REV) == false);
            BOOST_CHECK(VM_COUNT3 <= VM_COUNT3_REV);
            BOOST_CHECK(VM_COUNT3 >= VM_COUNT3_REV);
            BOOST_CHECK(
                misc::Vector::OrderlessCompareLess(PV_COUNT3, PV_COUNT3_REV)
                == (VM_COUNT3 < VM_COUNT3_REV));
            BOOST_CHECK((PV_COUNT3 > PV_COUNT3_REV) == (VM_COUNT3 > VM_COUNT3_REV));

            BOOST_CHECK(VM_COUNT3 != VM_EMPTY);
            BOOST_CHECK((VM_COUNT3 != VM_EMPTY));
            BOOST_CHECK((VM_COUNT3 < VM_EMPTY) == (PV_COUNT3 < PV_EMPTY));
            BOOST_CHECK((VM_COUNT3 > VM_EMPTY) == (PV_COUNT3 > PV_EMPTY));
            BOOST_CHECK((VM_COUNT3 <= VM_EMPTY) == (PV_COUNT3 <= PV_EMPTY));
            BOOST_CHECK((VM_COUNT3 >= VM_EMPTY) == (PV_COUNT3 >= PV_EMPTY));
            //
            BOOST_CHECK(VM_EMPTY != VM_COUNT3);
            BOOST_CHECK((VM_EMPTY != VM_COUNT3));
            BOOST_CHECK((VM_EMPTY < VM_COUNT3) == (PV_EMPTY < PV_COUNT3));
            BOOST_CHECK((VM_EMPTY > VM_COUNT3) == (PV_EMPTY > PV_COUNT3));
            BOOST_CHECK((VM_EMPTY <= VM_COUNT3) == (PV_EMPTY <= PV_COUNT3));
            BOOST_CHECK((VM_EMPTY >= VM_COUNT3) == (PV_EMPTY >= PV_COUNT3));

            const std::vector<Vector_t> TEST_VECS { {},
                                                    { 0 },
                                                    { 298 },
                                                    { -9164 },
                                                    { 0, 0 },
                                                    { 0, 1905 },
                                                    { 50983, 0 },
                                                    { 1, 1 },
                                                    { -1, -1 },
                                                    { 1, -1 },
                                                    V_COUNT3,
                                                    V_COUNT3_REV,
                                                    { 521243, -31245, 521234, -3453546 } };

            for (const auto & VEC_OUTER : TEST_VECS)
            {
                for (const auto & VEC_INNER : TEST_VECS)
                {
                    relationalTestsAll(VEC_OUTER, VEC_INNER);
                }
            }
        }
    }
}
