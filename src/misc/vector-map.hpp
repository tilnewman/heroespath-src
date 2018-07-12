// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
#define HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
//
// vector-map.hpp
//
#include "misc/boost-serialize-includes.hpp"
#include "misc/vectors.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace heroespath
{
namespace misc
{

    template <typename Key_t, typename Value_t>
    class VectorMap;

    template <typename Key_t, typename Value_t>
    bool operator==(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    template <typename Key_t, typename Value_t>
    bool operator!=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    template <typename Key_t, typename Value_t>
    bool operator<(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    // Responsible for implementing std::map with a vector in a way that performs
    // no sorting.  (all operations are in linear time)
    template <typename Key_t, typename Value_t>
    class VectorMap
    {
    public:
        using Pair_t = std::pair<Key_t, Value_t>;
        using PairVec_t = std::vector<Pair_t>;
        using iterator = typename PairVec_t::iterator;
        using const_iterator = typename PairVec_t::const_iterator;
        using reverse_iterator = typename PairVec_t::reverse_iterator;
        using const_reverse_iterator = typename PairVec_t::const_reverse_iterator;

        VectorMap()
            : pairs_()
        {}

        // it is up to the caller to ensure that no duplicates are added, if you care...
        void Append(const Key_t & KEY, const Value_t & VALUE)
        {
            pairs_.push_back(std::make_pair(KEY, VALUE));
        }

        bool AppendIfKeyNotFound(const Key_t & KEY, const Value_t & VALUE)
        {
            if (Exists(KEY) == false)
            {
                pairs_.push_back(std::make_pair(KEY, VALUE));
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Exists(const Key_t & KEY) const
        {
            for (auto const & PAIR : pairs_)
            {
                if (KEY == PAIR.first)
                {
                    return true;
                }
            }

            return false;
        }

        Value_t & operator[](const Key_t & KEY)
        {
            for (auto & pair : pairs_)
            {
                if (KEY == pair.first)
                {
                    return pair.second;
                }
            }

            pairs_.emplace_back(std::make_pair(KEY, Value_t()));
            return pairs_.back().second;
        }

        bool Find(const Key_t & KEY, Value_t & thing) const
        {
            for (auto const & PAIR : pairs_)
            {
                if (KEY == PAIR.first)
                {
                    thing = PAIR.second;
                    return true;
                }
            }

            return false;
        }

        iterator Find(const Key_t & KEY)
        {
            for (iterator iter(pairs_.begin()); iter != std::end(pairs_); ++iter)
            {
                if (KEY == iter->first)
                {
                    return iter;
                }
            }

            return std::end(pairs_);
        }

        const_iterator Find(const Key_t & KEY) const
        {
            for (const_iterator iter(pairs_.begin()); iter != std::end(pairs_); ++iter)
            {
                if (KEY == iter->first)
                {
                    return iter;
                }
            }

            return std::end(pairs_);
        }

        void Erase(const Key_t & KEY)
        {
            pairs_.erase(
                std::remove_if(
                    std::begin(pairs_),
                    std::end(pairs_),
                    [&KEY](auto const & PAIR) { return (PAIR.first == KEY); }),
                std::end(pairs_));
        }

        std::size_t Size() const { return pairs_.size(); }

        bool Empty() const { return pairs_.empty(); }

        void Clear() { return pairs_.clear(); }

        void Reserve(const std::size_t NEW_CAPACITY) { pairs_.reserve(NEW_CAPACITY); }

        iterator begin() noexcept { return std::begin(pairs_); }
        iterator end() noexcept { return std::end(pairs_); }

        reverse_iterator rbegin() noexcept { return std::rbegin(pairs_); }
        reverse_iterator rend() noexcept { return std::rend(pairs_); }

        Pair_t & Front() { return pairs_.front(); }
        Pair_t & Back() { return pairs_.back(); }

        const const_iterator begin() const noexcept { return std::begin(pairs_); }
        const const_iterator end() const noexcept { return std::end(pairs_); }

        const const_reverse_iterator rbegin() const noexcept { return std::rbegin(pairs_); }
        const const_reverse_iterator rend() const noexcept { return std::rend(pairs_); }

        const Pair_t & Front() const { return pairs_.front(); }
        const Pair_t & Back() const { return pairs_.back(); }

        // clang-format off
        friend bool
            operator== <> (const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

        friend bool
            operator< <> (const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);
        // clang-format on

    private:
        PairVec_t pairs_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & pairs_;
        }
    };

    template <typename Key_t, typename Value_t>
    typename VectorMap<Key_t, Value_t>::iterator begin(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.begin();
    }

    template <typename Key_t, typename Value_t>
    typename VectorMap<Key_t, Value_t>::reverse_iterator
        rbegin(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.rbegin();
    }

    template <typename Key_t, typename Value_t>
    typename VectorMap<Key_t, Value_t>::iterator end(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.end();
    }

    template <typename Key_t, typename Value_t>
    typename VectorMap<Key_t, Value_t>::reverse_iterator
        rend(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.rend();
    }

    template <typename Key_t, typename Value_t>
    const typename VectorMap<Key_t, Value_t>::const_iterator
        begin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.begin();
    }

    template <typename Key_t, typename Value_t>
    const typename VectorMap<Key_t, Value_t>::const_reverse_iterator
        rbegin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.rbegin();
    }

    template <typename Key_t, typename Value_t>
    const typename VectorMap<Key_t, Value_t>::const_iterator
        end(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.end();
    }

    template <typename Key_t, typename Value_t>
    const typename VectorMap<Key_t, Value_t>::const_reverse_iterator
        rend(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.rend();
    }

    template <typename Key_t, typename Value_t>
    bool operator==(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return misc::Vector::OrderlessCompareEqual(L.pairs_, R.pairs_);
    }

    template <typename Key_t, typename Value_t>
    bool operator!=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return !(L == R);
    }

    template <typename Key_t, typename Value_t>
    bool operator<(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return misc::Vector::OrderlessCompareLess(L.pairs_, R.pairs_);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
