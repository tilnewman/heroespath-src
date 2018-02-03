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
#ifndef HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
#define HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
//
// vector-map.hpp
//
#include <vector>
#include <utility>


namespace heroespath
{
namespace misc
{

    //Responsible for implementing std::map with a vector in a way that performs
    //no sorting.  (all operations are in linear time)
    template<typename Key_t, typename Value_t>
    class VectorMap
    {
    public:
        using Pair_t = std::pair<Key_t, Value_t>;
        using PairVec_t = std::vector<Pair_t>;
        using iterator = typename PairVec_t::iterator;
        using const_iterator = typename PairVec_t::const_iterator;

        VectorMap()
        :
            m_pairVec()
        {}

        bool Exists(const Key_t & KEY) const
        {
            for (auto & pair : m_pairVec)
            {
                if (KEY == pair.first)
                {
                    return true;
                }
            }

            return false;
        }

        Value_t & operator[](const Key_t & KEY)
        {
            for (auto & pair : m_pairVec)
            {
                if (KEY == pair.first)
                {
                    return pair.second;
                }
            }

            m_pairVec.push_back(std::make_pair(KEY, Value_t()));
            return m_pairVec[m_pairVec.size() - 1].second;
        }

        bool Find(const Key_t & KEY, Value_t & thing) const
        {
            for (auto & pair : m_pairVec)
            {
                if (KEY == pair.first)
                {
                    thing = pair.second;
                    return true;
                }
            }

            return false;
        }

        std::size_t Size() const { return m_pairVec.size(); }

        bool Empty() const { return m_pairVec.empty(); }

        void Clear() { return m_pairVec.clear(); }

        iterator begin() { return std::begin(m_pairVec); }
        iterator end() { return std::end(m_pairVec); }

        const const_iterator begin() const { return std::begin(m_pairVec); }
        const const_iterator end() const { return std::end(m_pairVec); }

    private:
        PairVec_t m_pairVec;
    };


    template<typename Key_t, typename Value_t>
    inline typename VectorMap<Key_t, Value_t>::iterator begin(VectorMap<Key_t, Value_t> & cpm)
    {
        return cpm.begin();
    }

    template<typename Key_t, typename Value_t>
    inline typename VectorMap<Key_t, Value_t>::iterator end(VectorMap<Key_t, Value_t> & cpm)
    {
        return cpm.end();
    }


    template<typename Key_t, typename Value_t>
    inline const typename VectorMap<Key_t, Value_t>::const_iterator begin(const VectorMap<Key_t, Value_t> & CPM)
    {
        return CPM.begin();
    }

    template<typename Key_t, typename Value_t>
    inline const typename VectorMap<Key_t, Value_t>::const_iterator end(const VectorMap<Key_t, Value_t> & CPM)
    {
        return CPM.end();
    }

}
}

#endif //HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED