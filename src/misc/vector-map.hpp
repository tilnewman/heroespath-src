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
#include "misc/assertlogandthrow.hpp"
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

    template <typename Key_t, typename Value_t>
    bool operator>(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    template <typename Key_t, typename Value_t>
    bool operator<=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    template <typename Key_t, typename Value_t>
    bool operator>=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

    // Responsible for implementing std::map with a vector in a way that performs
    // no sorting.  (all operations are in linear time)
    template <typename Key_t, typename Value_t>
    class VectorMap
    {
    public:
        using value_type = std::pair<Key_t, Value_t>;
        using container_type = std::vector<value_type>;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        VectorMap()
            : pairs_()
        {}

        // it is up to the caller to ensure that no duplicates are added, if you care...
        void Append(const value_type & PAIR) { pairs_.push_back(PAIR); }

        // it is up to the caller to ensure that no duplicates are added, if you care...
        void Append(const Key_t & KEY, const Value_t & VALUE)
        {
            Append(std::make_pair(KEY, VALUE));
        }

        bool AppendIfKeyNotFound(const value_type & PAIR)
        {
            if (Exists(PAIR.first))
            {
                return false;
            }
            else
            {
                Append(PAIR);
                return true;
            }
        }

        bool AppendIfKeyNotFound(const Key_t & KEY, const Value_t & VALUE)
        {
            return AppendIfKeyNotFound(std::make_pair(KEY, VALUE));
        }

        bool Exists(const Key_t & KEY) const { return (Find(KEY) != std::end(pairs_)); }

        bool Exists(const value_type & PAIR) const { return (Find(PAIR) != std::end(pairs_)); }

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
            const auto FOUND_ITER { Find(KEY) };

            if (FOUND_ITER == std::end(pairs_))
            {
                return false;
            }
            else
            {
                thing = FOUND_ITER->second;
                return true;
            }
        }

        iterator Find(const Key_t & KEY)
        {
            return std::find_if(std::begin(pairs_), std::end(pairs_), [&KEY](const auto & PAIR) {
                return (PAIR.first == KEY);
            });
        }

        iterator Find(const value_type & PAIR)
        {
            return std::find(std::begin(pairs_), std::end(pairs_), PAIR);
        }

        const_iterator Find(const Key_t & KEY) const
        {
            return std::find_if(std::begin(pairs_), std::end(pairs_), [&KEY](const auto & PAIR) {
                return (PAIR.first == KEY);
            });
        }

        const_iterator Find(const value_type & PAIR) const
        {
            return std::find(std::begin(pairs_), std::end(pairs_), PAIR);
        }

        // returns the number of elements erased
        std::size_t Erase(const Key_t & KEY)
        {
            const auto ORIG_SIZE { Size() };

            pairs_.erase(
                std::remove_if(
                    std::begin(pairs_),
                    std::end(pairs_),
                    [&KEY](const auto & PAIR) { return (PAIR.first == KEY); }),
                std::end(pairs_));

            return ORIG_SIZE - Size();
        }

        // returns the number of elements erased
        std::size_t Erase(const value_type & PAIR)
        {
            const auto ORIG_SIZE { Size() };
            pairs_.erase(std::remove(std::begin(pairs_), std::end(pairs_), PAIR), std::end(pairs_));
            return ORIG_SIZE - Size();
        }

        std::size_t Size() const noexcept { return pairs_.size(); }

        bool Empty() const noexcept { return pairs_.empty(); }

        void Clear() noexcept { return pairs_.clear(); }

        void Reserve(const std::size_t NEW_CAPACITY) { pairs_.reserve(NEW_CAPACITY); }

        void Capacity() const noexcept { return pairs_.capacity(); }

        void MaxSize() const noexcept { return pairs_.max_size(); }

        constexpr iterator begin() noexcept { return std::begin(pairs_); }
        constexpr iterator end() noexcept { return std::end(pairs_); }

        constexpr const_iterator begin() const noexcept { return std::begin(pairs_); }
        constexpr const_iterator end() const noexcept { return std::end(pairs_); }

        constexpr const_iterator cbegin() const noexcept { return begin(); }
        constexpr const_iterator cend() const noexcept { return end(); }

        constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

        constexpr const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        constexpr const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        constexpr const_reverse_iterator crend() const noexcept { return rend(); }

        value_type & Front()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), "misc::VectorMap::Front() non-const, called when empty.");

            return pairs_.front();
        }

        value_type & Back()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), "misc::VectorMap::Back() non-const, called when empty.");

            return pairs_.back();
        }

        const value_type & Front() const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), "misc::VectorMap::Front() const, called when empty.");

            return pairs_.front();
        }

        const value_type & Back() const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), "misc::VectorMap::Back() const, called when empty.");

            return pairs_.back();
        }

        // clang-format off
        friend bool
            operator== <>(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);

        friend bool
            operator< <>(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R);
        // clang-format on

    private:
        container_type pairs_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & pairs_;
        }
    };

    template <typename Key_t, typename Value_t>
    constexpr auto begin(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.begin();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto begin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.begin();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto cbegin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return begin(CPM);
    }

    template <typename Key_t, typename Value_t>
    constexpr auto rbegin(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.rbegin();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto rbegin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.rbegin();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto crbegin(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return rbegin(CPM);
    }

    template <typename Key_t, typename Value_t>
    constexpr auto end(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.end();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto end(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.end();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto cend(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return end(CPM);
    }

    template <typename Key_t, typename Value_t>
    constexpr auto rend(VectorMap<Key_t, Value_t> & cpm) noexcept
    {
        return cpm.rend();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto rend(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return CPM.rend();
    }

    template <typename Key_t, typename Value_t>
    constexpr auto crend(const VectorMap<Key_t, Value_t> & CPM) noexcept
    {
        return rend(CPM);
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

    template <typename Key_t, typename Value_t>
    bool operator>(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return (R < L);
    }

    template <typename Key_t, typename Value_t>
    bool operator<=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return !(L > R);
    }

    template <typename Key_t, typename Value_t>
    bool operator>=(const VectorMap<Key_t, Value_t> & L, const VectorMap<Key_t, Value_t> & R)
    {
        return !(L < R);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_VECTORMAP_HPP_INCLUDED
