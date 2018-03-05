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
#ifndef HEROESPATH_CONFIG_CONFIGBASE_HPP_INCLUDED
#define HEROESPATH_CONFIG_CONFIGBASE_HPP_INCLUDED
//
// configbase.hpp
//
//  TODO for a derived version
//      integrate the logger?
//      cache return values in a map?
//      comments in the file?
//
#include "iconfig.hpp"

// prevent warnings that can be ignored
#if defined(WIN32) || defined(_WIN32) || defined(__WINDOWS__)
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4512)
#elif defined(__APPLE__) || defined(__MACH__)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/type_index.hpp> //for boost::typeindex::type_id<T>().pretty_name()

#if defined(WIN32) || defined(_WIN32) || defined(__WINDOWS__)
#pragma warning(pop)
#elif defined(__APPLE__) || defined(__MACH__)
#pragma GCC diagnostic warning "-Wswitch-enum"
#pragma GCC diagnostic warning "-Wnested-anon-types"
#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wundef"
#endif

#include <exception>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

namespace heroespath
{
namespace config
{

    //
    // ConfigBase Class
    //  Intended to be a bare-bones base implementation that is fully functional,
    //  and easy to derive from and customize.
    //
    //  KEY strings must not contain the SEP_STR, but VALUE strings can.
    //
    class ConfigBase : public IConfigBase
    {
        // constructors / destructor
    public:
        explicit ConfigBase(
            const std::string & FILE_PATH_STR = FILE_PATH_DEFAULT_,
            const std::string & SEP_STR = SEP_STR_DEFAULT_,
            const std::string & COMMENT_STR = COMMENT_STR_DEFAULT_);

        virtual ~ConfigBase();

        // custom types
    private:
        using ConfigMap = std::map<std::string, std::string>;
        using ConfigMapIter = ConfigMap::iterator;
        using ConfigMapCIter = ConfigMap::const_iterator;

        // functions
    public:
        virtual bool Load();

        // Pass a string to override the default which uses the member fileNameStr.
        virtual bool Save(const std::string & FILENAME = "") const;

        // Returns the value stored at KEY.
        // Retuns DEFAULT if key does not exist, or cannot be cast to the desired type.
        template <typename T>
        T GetWithDefault(const std::string & KEY, const T DEFAULT = T()) const
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
        inline virtual const std::string
            GetWithDefaultStr(const std::string & KEY, const std::string & DEFAULT = "") const
        {
            return GetWithDefault<std::string>(KEY, DEFAULT);
        }
        inline virtual int GetWithDefaultInt(const std::string & KEY, const int DEFAULT = 0) const
        {
            return GetWithDefault<int>(KEY, DEFAULT);
        }
        inline virtual std::size_t
            GetWithDefaultSizet(const std::string & KEY, const std::size_t DEFAULT = 0) const
        {
            return GetWithDefault<std::size_t>(KEY, DEFAULT);
        }
        inline virtual float
            GetWithDefaultFloat(const std::string & KEY, const float DEFAULT = 0.0f) const
        {
            return GetWithDefault<float>(KEY, DEFAULT);
        }
        inline virtual double
            GetWithDefaultDouble(const std::string & KEY, const double DEFAULT = 0.0) const
        {
            return GetWithDefault<double>(KEY, DEFAULT);
        }
        virtual bool GetWithDefaultBool(const std::string & KEY, const bool DEFAULT = false) const;
        //
        inline virtual const std::string GetCopyStr(const std::string & KEY) const
        {
            std::string s;
            Get<std::string>(s, KEY);
            return s;
        }
        inline virtual int GetCopyInt(const std::string & KEY) const
        {
            int i;
            Get<int>(i, KEY);
            return i;
        }
        inline virtual std::size_t GetCopySizet(const std::string & KEY) const
        {
            std::size_t s;
            Get<std::size_t>(s, KEY);
            return s;
        }
        inline virtual float GetCopyFloat(const std::string & KEY) const
        {
            float f;
            Get<float>(f, KEY);
            return f;
        }
        inline virtual double GetCopyDouble(const std::string & KEY) const
        {
            double d;
            Get<double>(d, KEY);
            return d;
        }
        inline virtual bool GetCopyBool(const std::string & KEY) const
        {
            bool b;
            GetBool(b, KEY);
            return b;
        }

        // Sets val to value stored at KEY and returns true.
        // If KEY does not exist or cannot be cast to the desired type, then val is not changed and
        // false is returned.
        //
        virtual void GetStr(std::string & val, const std::string & KEY) const;
        inline virtual void GetInt(int & val, const std::string & KEY) const
        {
            return Get<int>(val, KEY);
        }
        inline virtual void GetSizet(std::size_t & val, const std::string & KEY) const
        {
            return Get<std::size_t>(val, KEY);
        }
        inline virtual void GetFloat(float & val, const std::string & KEY) const
        {
            return Get<float>(val, KEY);
        }
        inline virtual void GetDouble(double & val, const std::string & KEY) const
        {
            return Get<double>(val, KEY);
        }
        virtual void GetBool(bool & val, const std::string & KEY) const;

        // Sets the value at KEY to VALUE, creating the key if needed.
        virtual void SetStr(const std::string & KEY, const std::string & VALUE);
        inline virtual void SetInt(const std::string & KEY, const int VALUE)
        {
            Set<int>(KEY, VALUE);
        }
        inline virtual void SetSizet(const std::string & KEY, const std::size_t VALUE)
        {
            Set<std::size_t>(KEY, VALUE);
        }
        inline virtual void SetBool(const std::string & KEY, const bool VALUE)
        {
            Set<bool>(KEY, VALUE);
        }
        inline virtual void SetFloat(const std::string & KEY, const float VALUE)
        {
            Set<float>(KEY, VALUE);
        }
        inline virtual void SetDouble(const std::string & KEY, const double VALUE)
        {
            Set<double>(KEY, VALUE);
        }

        virtual void Dump(std::ostream &);

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
                ss << "config::ConfigBase::Get<" << boost::typeindex::type_id<T>().pretty_name()
                   << ">(key=\"" << KEY << "\") Failed to convert value \"" << value
                   << "\" into a \"" << boost::typeindex::type_id<T>().pretty_name()
                   << "\".  boost::lexical_cast threw: [" << EX.what() << "]";

                throw std::runtime_error(ss.str());
            }
        }

    private:
        virtual void HandleLoadSaveError(const std::string & ERR_MSG) const;
        virtual void HandleLoadInvalidLineError(const std::string & ERR_MSG) const;

        // when saving and loading, this function is used to establish a full path to the file
        virtual const boost::filesystem::path
            GetFullPath(const std::string & USER_SPEC_PATH_STR) const;

        // when loading, this function process each line, determines if comment/etc, and modifies
        // data_ returns true if any changes were made to data_
        virtual bool LoadNextLine(const std::size_t LINE_NUM, const std::string & NEXT_LINE);

        // when saving, this function transforms an entry in data_ into a string to be saved to the
        // file returns true if anything was written to stream
        virtual bool SaveNextLine(
            const ConfigMap::const_iterator & NEXT_ENTRY_ITR, std::ostream & stream) const;

        // Returns true if the line is a comment and should be ignored.
        virtual bool IsCommentLine(const std::string &) const;

        bool StringToBool(const std::string & S, bool & result) const;

        // getters for immutable internal data
    public:
        virtual const std::string GetFileNameStr() const;
        virtual const std::string GetFileNameFullPathStr() const;
        virtual const std::string GetSeparatorStr() const;
        virtual const std::string GetCommentStr() const;

        // data
    public:
        static const std::string FILE_PATH_DEFAULT_;
        static const std::string SEP_STR_DEFAULT_;
        static const std::string COMMENT_STR_DEFAULT_;

    private:
        mutable boost::recursive_mutex dataAccessMutex_;
        std::string filePathStr_;
        std::string sepStr_;
        std::string commentStr_;
        ConfigMap data_;
    };
}
}

#endif // HEROESPATH_CONFIG_CONFIGBASE_HPP_INCLUDED
