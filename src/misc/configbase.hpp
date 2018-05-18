// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_CONFIG_BASE_HPP_INCLUDED
#define HEROESPATH_MISC_CONFIG_BASE_HPP_INCLUDED
//
// configbase.hpp
//
#include "misc/vector-map.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

#include <exception>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

namespace heroespath
{
namespace misc
{

    //
    // ConfigBase Class
    //  Intended to be a bare-bones base implementation that is fully functional,
    //  and easy to derive from and customize.
    //
    //  KEY strings must not contain the SEP_STR, but VALUE strings can.
    //
    class ConfigBase
    {
        using KeyValueMap_t = misc::VectorMap<std::string, std::string>;

    public:
        explicit ConfigBase(
            const std::string & FILE_PATH_STR = FILE_PATH_DEFAULT_,
            const std::string & SEP_STR = SEP_STR_DEFAULT_,
            const std::string & COMMENT_STR = COMMENT_STR_DEFAULT_);

        // MUST CALL SAVE - not saved automatically upon destruction
        virtual ~ConfigBase() = default;

        // functions
    public:
        bool Load();

        // Pass a string to override the default which uses the member fileNameStr.
        bool Save(const std::string & FILENAME = "") const;

        // Returns the value stored at KEY.
        // Retuns DEFAULT if key does not exist, or cannot be cast to the desired type.
        template <typename T>
        T GetWithDefault(const std::string & KEY, const T & DEFAULT = T()) const
        {
            T x;

            try
            {
                Get<T>(x, KEY);
            }
            catch (...)
            {
                x = DEFAULT;
            }

            return x;
        }
        //
        const std::string
            GetWithDefaultStr(const std::string & KEY, const std::string & DEFAULT = "") const
        {
            return GetWithDefault<std::string>(KEY, DEFAULT);
        }
        int GetWithDefaultInt(const std::string & KEY, const int DEFAULT = 0) const
        {
            return GetWithDefault<int>(KEY, DEFAULT);
        }
        std::size_t
            GetWithDefaultSizet(const std::string & KEY, const std::size_t DEFAULT = 0) const
        {
            return GetWithDefault<std::size_t>(KEY, DEFAULT);
        }
        float GetWithDefaultFloat(const std::string & KEY, const float DEFAULT = 0.0f) const
        {
            return GetWithDefault<float>(KEY, DEFAULT);
        }
        double GetWithDefaultDouble(const std::string & KEY, const double DEFAULT = 0.0) const
        {
            return GetWithDefault<double>(KEY, DEFAULT);
        }
        bool GetWithDefaultBool(const std::string & KEY, const bool DEFAULT = false) const;
        //
        const std::string GetCopyStr(const std::string & KEY) const
        {
            std::string s;
            Get<std::string>(s, KEY);
            return s;
        }
        int GetCopyInt(const std::string & KEY) const
        {
            int i;
            Get<int>(i, KEY);
            return i;
        }
        std::size_t GetCopySizet(const std::string & KEY) const
        {
            std::size_t s;
            Get<std::size_t>(s, KEY);
            return s;
        }
        float GetCopyFloat(const std::string & KEY) const
        {
            float f;
            Get<float>(f, KEY);
            return f;
        }
        double GetCopyDouble(const std::string & KEY) const
        {
            double d;
            Get<double>(d, KEY);
            return d;
        }
        bool GetCopyBool(const std::string & KEY) const
        {
            bool b;
            GetBool(b, KEY);
            return b;
        }

        // Sets val to value stored at KEY and returns true.
        // If KEY does not exist or cannot be cast to the desired type, then val is not changed and
        // false is returned.
        //
        void GetStr(std::string & val, const std::string & KEY) const;
        void GetInt(int & val, const std::string & KEY) const { return Get<int>(val, KEY); }
        void GetSizet(std::size_t & val, const std::string & KEY) const
        {
            return Get<std::size_t>(val, KEY);
        }
        void GetFloat(float & val, const std::string & KEY) const { return Get<float>(val, KEY); }
        void GetDouble(double & val, const std::string & KEY) const
        {
            return Get<double>(val, KEY);
        }
        void GetBool(bool & val, const std::string & KEY) const;

        // Sets the value at KEY to VALUE, creating the key if needed.
        void SetStr(const std::string & KEY, const std::string & VALUE);
        void SetInt(const std::string & KEY, const int VALUE) { Set<int>(KEY, VALUE); }
        void SetSizet(const std::string & KEY, const std::size_t VALUE)
        {
            Set<std::size_t>(KEY, VALUE);
        }
        void SetBool(const std::string & KEY, const bool VALUE) { Set<bool>(KEY, VALUE); }
        void SetFloat(const std::string & KEY, const float VALUE) { Set<float>(KEY, VALUE); }
        void SetDouble(const std::string & KEY, const double VALUE) { Set<double>(KEY, VALUE); }

        void Dump(std::ostream &);

        template <typename T>
        void Set(const std::string & KEY, const T & THING)
        {
            std::ostringstream ss;
            ss << THING;
            SetStr(KEY, ss.str());
        }

        template <typename T>
        void Get(T & val, const std::string & KEY) const
        {
            std::string value{ "" };
            GetStr(value, KEY); // throws if key cannot be found

            try
            {
                val = boost::lexical_cast<T>(value);
            }
            catch (const std::exception & EX)
            {
                std::ostringstream ss;
                ss << "misc::ConfigBase::Get<" << boost::typeindex::type_id<T>().pretty_name()
                   << ">(key=\"" << KEY << "\") Failed to convert value \"" << value
                   << "\" into a \"" << boost::typeindex::type_id<T>().pretty_name()
                   << "\".  boost::lexical_cast threw: [" << EX.what() << "]";

                throw std::runtime_error(ss.str());
            }
        }

        const std::string GetFileNameStr() const;
        const std::string GetFileNameFullPathStr() const;
        const std::string GetSeparatorStr() const;
        const std::string GetCommentStr() const;

    private:
        void HandleLoadSaveError(const std::string & ERR_MSG) const;
        void HandleLoadInvalidLineError(const std::string & ERR_MSG) const;

        // when saving and loading, this function is used to establish a full path to the file
        const boost::filesystem::path GetFullPath(const std::string & USER_SPEC_PATH_STR) const;

        // when loading, this function process each line, determines if comment/etc, and modifies
        // data_ returns true if any changes were made to data_
        bool LoadNextLine(const std::size_t LINE_NUM, const std::string & NEXT_LINE);

        // when saving, this function transforms an entry in data_ into a string to be saved to the
        // file returns true if anything was written to stream
        bool SaveNextLine(const std::pair<std::string, std::string> &, std::ostream & stream) const;

        // Returns true if the line is a comment and should be ignored.
        bool IsCommentLine(const std::string &) const;

        bool StringToBool(const std::string & S, bool & result) const;

    private:
        static const std::string FILE_PATH_DEFAULT_;
        static const std::string SEP_STR_DEFAULT_;
        static const std::string COMMENT_STR_DEFAULT_;

        std::string filePathStr_;
        std::string sepStr_;
        std::string commentStr_;
        KeyValueMap_t map_;
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_CONFIG_BASE_HPP_INCLUDED
