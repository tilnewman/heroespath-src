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
#ifndef CONFIGBASE_HPP
#define CONFIGBASE_HPP
//
// configbase.hpp
//
//  TODO for a derived version
//      integrate the logger?
//      cache return values in a map?
//      comments in the file?
//
#include "common/iconfig.hpp"
#include "common/boostinc.hpp"

#include <boost/type_index.hpp>//for boost::typeindex::type_id<T>().pretty_name()

#include <string>
#include <map>
#include <ostream>
#include <sstream>
#include <exception>


namespace appbase
{
    //
    // ConfigBase Class
    //  Intended to be a bare-bones base implementation that is fully functional,
    //  and easy to derive from and customize.
    //
    //  KEY strings must not contain the SEP_STR, but VALUE strings can.
    //
    class ConfigBase : public appbase::IConfigBase
    {
        // constructors / destructor
    public:
        explicit ConfigBase(const std::string & FILE_PATH_STR = FILE_PATH_DEFAULT_,
                            const std::string & SEP_STR       = SEP_STR_DEFAULT_,
                            const std::string & COMMENT_STR   = COMMENT_STR_DEFAULT_);

        virtual ~ConfigBase();


        // custom types
    private:
        using ConfigMap = std::map<std::string, std::string>;
        using ConfigMapIter = ConfigMap::iterator;
        using ConfigMapCIter = ConfigMap::const_iterator;


        // functions
    public:
        virtual bool Load();

        //Pass a string to override the default which uses the member fileNameStr.
        virtual bool Save(const std::string & FILENAME = "") const;

        //Returns the value stored at KEY.
        //Retuns DEFAULT if key does not exist, or cannot be cast to the desired type.
        template<typename T>
        T GetCopy(const std::string & KEY, const T DEFAULT = T()) const
        {
            T temp(DEFAULT);
            Get<T>(temp, KEY);
            return temp;
        }
        //
        virtual const std::string GetCopyStr(const std::string & KEY, const std::string & DEFAULT = "") const;
        virtual int               GetCopyInt(const std::string & KEY, const int DEFAULT = 0) const            { return GetCopy<int>(KEY, DEFAULT); }
        virtual std::size_t       GetCopySizet(const std::string & KEY, const std::size_t DEFAULT = 0) const       { return GetCopy<std::size_t>(KEY, DEFAULT); }
        virtual bool              GetCopyBool(const std::string & KEY, const bool DEFAULT = false) const      { return GetCopy<bool>(KEY, DEFAULT); }
        virtual float             GetCopyFloat(const std::string & KEY, const float DEFAULT = 0.0f) const     { return GetCopy<float>(KEY, DEFAULT); }
        virtual double            GetCopyDouble(const std::string & KEY, const double DEFAULT = 0.0) const    { return GetCopy<double>(KEY, DEFAULT); }

        //Sets val to value stored at KEY and returns true.
        //If KEY does not exist or cannot be cast to the desired type, then val is not changed and false is returned.
        //
        virtual void GetStr(std::string & val, const std::string & KEY) const;
        virtual void GetInt(int & val, const std::string & KEY) const           { return Get<int>(val, KEY); }
        virtual void GetSizet(std::size_t & val, const std::string & KEY) const { return Get<std::size_t>(val, KEY); }
        virtual void GetBool(bool & val, const std::string & KEY) const         { return Get<bool>(val, KEY); }
        virtual void GetFloat(float & val, const std::string & KEY) const       { return Get<float>(val, KEY); }
        virtual void GetDouble(double & val, const std::string & KEY) const     { return Get<double>(val, KEY); }

        //Sets the value at KEY to VALUE, creating the key if needed.
        virtual void SetStr(const std::string & KEY, const std::string & VALUE);
        virtual void SetInt(const std::string & KEY, const int VALUE)           { Set<int>(KEY, VALUE); }
        virtual void SetSizet(const std::string & KEY, const std::size_t VALUE) { Set<std::size_t>(KEY, VALUE); }
        virtual void SetBool(const std::string & KEY, const bool VALUE)         { Set<bool>(KEY, VALUE); }
        virtual void SetFloat(const std::string & KEY, const float VALUE)       { Set<float>(KEY, VALUE); }
        virtual void SetDouble(const std::string & KEY, const double VALUE)     { Set<double>(KEY, VALUE); }

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
            GetStr(value, KEY);//throws if key cannot be found

            try
            {
                val = boost::lexical_cast<T>(value);
            }
            catch(const std::exception & EX)
            {
                std::ostringstream ss;
                ss << "appbase::ConfigBase::Get<" << boost::typeindex::type_id<T>().pretty_name() << ">(key=\"" << KEY << "\") Failed to convert value \"" << value << "\" into a \"" << boost::typeindex::type_id<T>().pretty_name() << "\".  boost::lexical_cast threw: [" << EX.what() << "]";
                throw std::runtime_error(ss.str());
            }
        }

    private:
        virtual void HandleLoadSaveError(const std::string & ERR_MSG) const;
        virtual void HandleLoadInvalidLineError(const std::string & ERR_MSG) const;

        //when saving and loading, this function is used to establish a full path to the file
        virtual const boost::filesystem::path GetFullPath(const std::string & USER_SPEC_PATH_STR) const;

        //when loading, this function process each line, determines if comment/etc, and modifies data_
        //returns true if any changes were made to data_
        virtual bool LoadNextLine(const std::size_t LINE_NUM,
                                  const std::string & NEXT_LINE);

        //when saving, this function transforms an entry in data_ into a string to be saved to the file
        //returns true if anything was written to stream
        virtual bool SaveNextLine(const ConfigMap::const_iterator & NEXT_ENTRY_ITR,
                                  std::ostream & stream) const;

        //Returns true if the line is a comment and should be ignored.
        virtual bool IsCommentLine(const std::string &) const;

        //getters for immutable internal data
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

}//end of namespace appbase


#endif //CONFIGBASE_HPP
