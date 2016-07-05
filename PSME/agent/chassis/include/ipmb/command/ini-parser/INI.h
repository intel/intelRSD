/*
   Feather INI Parser - 1.40
   You are free to use this however you wish.

   If you find a bug, please attept to debug the cause.
   Post your environment details and the cause or fix in the issues section of GitHub.

   Written by Turbine.

   Website:
   https://github.com/Turbine1991/feather-ini-parser
   http://code.google.com/p/feather-ini-parser/downloads

   Help:
   Bundled example & readme.
   http://code.google.com/p/feather-ini-parser/wiki/Tutorials
*/

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

#define FINI_SAFE
#define FINI_BUFFER_SIZE 128

#if __cplusplus >= 201103L
  #include <unordered_map>
  #define FINI_CPP11
  #define ALLOCATE_SECTIONS 100
  #define ALLOCATE_KEYS 5
#else
  #include <map>
#endif

#ifdef FINI_WIDE_SUPPORT
  #include <wchar.h>

  typedef std::wstringstream fini_sstream_t;
  typedef std::wstring fini_string_t;
  typedef wchar_t fini_char_t;
  typedef std::wifstream fini_ifstream_t;
  typedef std::wofstream fini_ofstream_t;

  #define fini_strlen(a) wcslen(a)
  #define fini_strncpy(a, b) wcscpy(a, b)
  #define fini_strncpy(a, b, c) wcsncpy(a, b, c)
  #define fini_strtok(a, b) wcstok(a, b)

  #define T_(x) L ##x
#else
  #include <cstring>

  typedef std::stringstream fini_sstream_t;
  typedef std::string fini_string_t;
  typedef char fini_char_t;
  typedef std::ifstream fini_ifstream_t;
  typedef std::ofstream fini_ofstream_t;

  #define fini_strlen(a) static_cast<long>(strlen(a))
  #define fini_strcpy(a, b) strcpy(a, b)
  #define fini_strncpy(a, b, c) strncpy(a, b, c)
  #define fini_strtok(a, b) strtok(a, b)

  #define T_(x) x
#endif

#define CHAR_SIZE sizeof(fini_char_t)

///Simple converter using templates and streams to effectively required for the flexibility of handling native types
class Converters
{
public:
   template <typename T, typename U>
     static T Convert(U value);
   template <typename T>
      static void GetLine(fini_sstream_t& out, T& value);
   static void GetLine(fini_sstream_t& out, fini_string_t& value);
   template <typename T>
      static size_t GetDataSize(T& value);
   static size_t GetDataSize(fini_string_t value);
};

///
template <typename T = fini_string_t, typename U = fini_string_t, typename V = fini_string_t>
   class INI
{
public:
   typedef T section_t;
   typedef U key_t;
   typedef V value_t;
   typedef INI<section_t, key_t, value_t> ini_t;

///Type definition declarations
#ifdef FINI_CPP11
   typedef typename std::unordered_map<key_t, value_t> keys_t;
   typedef typename std::unordered_map<section_t, keys_t*> sections_t;
#else
   typedef typename std::map<key_t, value_t> keys_t;
   typedef typename std::map<section_t, keys_t*> sections_t;
#endif

   typedef typename keys_t::iterator keysit_t;
   typedef typename sections_t::iterator sectionsit_t;

   typedef typename std::pair<key_t, value_t> keyspair_t;
   typedef typename std::pair<section_t, keys_t*> sectionspair_t;

   typedef char data_t;

   enum source_e {SOURCE_FILE, SOURCE_MEMORY};

///Data members
   std::string filename;
   data_t* data;
   size_t dataSize;
   keys_t* current;
   sections_t sections;
   source_e source;

///Constuctor/Destructor
   //Specify the filename to associate and whether to parse immediately
   INI(const std::string filename_, bool doParse): filename(filename_)
   {
      init(SOURCE_FILE, doParse);
   }

   //Used for loading INI from memory
   INI(void* data_, size_t dataSize_, bool doParse): data(reinterpret_cast<data_t*>(data_)), dataSize(dataSize_)
   {
      init(SOURCE_MEMORY, doParse);
   }

   ~INI()
   {
      clear();
   }

///Access Content
   //Provide bracket access to section contents
   keys_t& operator[](section_t section)
   {
   #ifdef FINI_SAFE
     if (!sections[section])
       sections[section] = new keys_t;
   #endif

      return *sections[section];
   }

   //Create a new section and select it
   bool create(const section_t section)
   {
      if (select(section))
         return false;

      current = new keys_t;
      sections[section] = current;

      reserveKeys(current);

      return true;
   }

   //Select a section for performing operations
   bool select(const section_t section)
   {
      sectionsit_t sectionsit = sections.find(section);
      if (sectionsit == sections.end())
         return false;

      current = sectionsit->second;

      return true;
   }

///Set
   //Assign a value for key under the selected section
   bool set(const key_t key, const value_t value)
   {
      if (current == nullptr)
         return false;

      (*current)[key] = value;

      return true;
   }

   template <typename W, typename X>
      bool set(const W key, const X value)
         { return set(Converters::Convert<key_t>(key), Converters::Convert<value_t>(value)); }

///Get
   value_t get(const key_t key, value_t def = value_t())
   {
      keysit_t it = current->find(key);
      if (current == nullptr || it == current->end())
         return def;

      return it->second;
   }

   bool remove(const key_t& key) {
      return current->erase(key);
   }

   value_t get(const section_t section, const key_t key, value_t def)
   {
      if (!select(section))
         return def;

      return get(key, def);
   }

   template <typename W, typename X>
      X get(const W key, const X def = value_t())
         { return Converters::Convert<X>(get(Converters::Convert<key_t>(key), Converters::Convert<value_t>(def))); }

   template <typename W>
      fini_string_t get(const W key, const fini_char_t* def = T_(""))  //Handle C string default value without casting
         { return Converters::Convert<fini_string_t>(get(Converters::Convert<key_t>(key), Converters::Convert<value_t>(def))); }

   template <typename W, typename X, typename Y>
      Y get(const W section, const X key, const Y def)
         { return Converters::Convert<Y>(get(Converters::Convert<section_t>(section), Converters::Convert<key_t>(key), Converters::Convert<value_t>(def))); }

   template <typename W, typename X>
      fini_string_t get(const W section, const X key, const fini_char_t* def)  //Handle C string default value without casting
         { return Converters::Convert<fini_string_t>(Converters::Convert<section_t>(section), get(Converters::Convert<key_t>(key), Converters::Convert<value_t>(def))); }

///Functions
   void parse(std::istream& file)
   {
      fini_char_t line[FINI_BUFFER_SIZE];
      bool first = true;
      fini_sstream_t out;

      while(!file.eof())
      {
         file.getline(line, FINI_BUFFER_SIZE);

         if (first)
         {
            first = false;
         }

         nake(line);

         if (line[0])
         {
            size_t len = static_cast<size_t>(fini_strlen(line));
            if (len > 0 && !((len >= 2 && (line[0] == '/' && line[1] == '/')) || (len >= 1 && line[0] == '#')))  //Ignore comment and empty lines
            {
               if (line[0] == '[')  //Section
               {
                  section_t section;
                  size_t length = static_cast<size_t>(fini_strlen(line)) - 2;  //Without section brackets
                  while(isspace(line[length + 1]))  //Leave out any additional new line characters, not "spaces" as the name suggests
                     --length;

                  fini_char_t* ssection = reinterpret_cast<fini_char_t*>(calloc(CHAR_SIZE, length + 1));
                  fini_strncpy(ssection, line + 1, length);  //Count after first bracket

                  current = new keys_t;

                  out << ssection;
                  free(ssection);
                  Converters::GetLine(out, section);

                  sections[section] = current;
               }
               else  //Key
               {
                  key_t key;
                  value_t value;

                  fini_char_t* skey;
                  fini_char_t* svalue;

                  skey = fini_strtok(line, T_("="));
                  svalue = fini_strtok(nullptr, T_("\n"));

                  if (skey && svalue)
                  {
                     size_t index = 0;  //Without section brackets
                     while(isspace(skey[index]))  //Leave out any additional new line characters, not "spaces" as the name suggests
                        index++;

                     if (index != 0)  //Has preceeding white space
                        fini_strcpy(skey, skey + index);

                     out << skey;

                     Converters::GetLine(out, key);

                     out.clear();
                     out.str(fini_string_t());

                     out << svalue;
                     Converters::GetLine(out, value);

                     if (value != value_t())
                       (*current)[key] = value;
                  }
               }

               out.clear();
               out.str(fini_string_t()); //Clear existing stream;
            }
         }
      }
   }

   //Parse an INI's contents into memory from the filename given during construction
   bool parse()
   {
      switch(source)
      {
      case SOURCE_FILE: {
            fini_ifstream_t file(filename.c_str());

            if (!file.is_open())
                throw std::runtime_error{"Cannot open file " + filename};

            parse(file);

            file.close();
         }
         break;

      case SOURCE_MEMORY: {
            std::stringstream sstream;
            sstream.rdbuf()->pubsetbuf(data, static_cast<long>(dataSize));

            parse(sstream);
         }
         break;
        default : { return true; }
      }


      return true;
   }

   bool parseBinary()
   {
      fini_ifstream_t file(filename.c_str(), std::ios::binary);
      if (!file.is_open())
         return false;

      size_t sectionCount;
      size_t keyCount;
      key_t key;
      value_t value;
      section_t section;

      //file.read((fini_char_t*)&sectionCount, sizeof(sectionCount));
      file >> sectionCount;

      for(size_t i = 0; i < sectionCount; i++)
      {
         if (i > 0)
            file.seekg(1 + file.tellg());

         file.read(reinterpret_cast<fini_char_t*>(&keyCount), sizeof(keyCount));
         file >> section;

         create(section);

         for(size_t j = 0; j < keyCount; j++)
         {
            file >> key;
            file >> value;
            set(key, value);
         }
      }

      file.close();

      return true;
   }

   //Clear the contents from memory
   void clear()
   {
      clean();
      sections.clear();
   }

///Output
   //Save from memory into file
   bool save(const std::string filenamee = "")
   {
      if (!hasFileAssociation(filenamee))
         return false;

      fini_ofstream_t file(((filenamee == "")? this->filename: filenamee).c_str(), std::ios::trunc);
      if (!file.is_open())
         return false;

      //Loop through sections
      for(typename INI::sectionsit_t i = sections.begin(); i != sections.end(); i++)
      {
         if (i->second->size() == 0)  //No keys/values in section, skip to next
            continue;

         //Write section
         const fini_string_t temp = makeSection(i->first);
         const fini_char_t* line = temp.c_str();
         file.write(line, fini_strlen(line));

         for(typename INI::keysit_t j = i->second->begin(); j != i->second->end(); j++)
         {
            //Write key and value
            const fini_string_t tempp = makeKeyValue(j->first, j->second);
            const fini_char_t* liine = tempp.c_str();
            file.write(liine, fini_strlen(liine));
         }
      }

      file.close();

      return true;
   }

   //Saves it without any conventional INI formatting characters, however it only uses string streams
   bool saveBinary(const std::string filenamee = "")
   {
      if (!hasFileAssociation(filenamee))
         return false;

      fini_ofstream_t file(((filenamee == "")? this->filename: filenamee).c_str(), std::ios::trunc | std::ios::binary);
      if (!file.is_open())
         return false;

      size_t sectionCount = sections.size();
      size_t keyCount;

      file.write(reinterpret_cast<fini_char_t*>(&sectionCount), sizeof(sectionCount));

      //Loop through sections
      for(typename INI::sectionsit_t i = sections.begin(); i != sections.end(); i++)
      {
         keyCount = i->second->size();
         file.write(reinterpret_cast<fini_char_t*>(&keyCount), sizeof(keyCount));

         file << i->first << std::endl;

         for(typename INI::keysit_t j = i->second->begin(); j != i->second->end(); j++)
         {
            file << j->first << std::endl;
            file << j->second << std::endl;
         }
      }

      file.close();

      return true;
   }

   //Saves it as a true binary file, intended to replace the existing one. Don't bother using it with all strings.
   bool saveBinaryExperimental(std::string filenamee = "")
   {
      if (!hasFileAssociation(filenamee))
         return false;

      fini_ofstream_t file(((filenamee == "")? this->filename: filenamee).c_str(), std::ios::trunc | std::ios::binary);
      if (!file.is_open())
         return false;

      size_t sectionCount = sections.size();
      size_t keyCount;

      file.write(reinterpret_cast<fini_char_t*>(&sectionCount), sizeof(sectionCount));

      //Loop through sections
      for(typename INI::sectionsit_t i = sections.begin(); i != sections.end(); i++)
      {
         keyCount = i->second->size();
         file.write(reinterpret_cast<fini_char_t*>(&keyCount), sizeof(keyCount));

         file.write(reinterpret_cast<fini_char_t*>(&i->first), Converters::GetDataSize(i->first));

         for(typename INI::keysit_t j = i->second->begin(); j != i->second->end(); j++)
         {
            file.write(reinterpret_cast<fini_char_t*>(&j->first), Converters::GetDataSize(j->first));
            file.write(reinterpret_cast<fini_char_t*>(&j->second), Converters::GetDataSize(j->second));
         }
      }

      file.close();

      return true;
   }

   //Alows another INI's contents to be insert into another, with the ability to retain the original values
   void merge(ini_t& other, bool retainValues = true)
   {
      for(typename INI::sectionsit_t i = other.sections.begin(); i != other.sections.end(); i++)
      {
         if (!select(i->first)) //Create and insert all key values into a missing section
         {
            keys_t* keys = new keys_t(*i->second);
            sections.insert(std::make_pair(i->first, keys));
         }
         else
         {
            for(typename INI::keysit_t j = i->second->begin(); j != i->second->end(); j++)
            {
               keysit_t it = current->find(j->first);
               if (it == current->end())
                  current->insert(std::make_pair(j->first, j->second));
               else if (!retainValues)
                  it->second = j->second;
            }
         }
      }
   }

private:
///Functions
   //Init the INI in with values set by constructor
   void init(source_e sourcee, bool doParse)
   {
      this->source = sourcee;

      reserveSections();
      if (doParse)
         parse();
   }

   //Clean the contents for descruction
   void clean()
   {
      for(sectionsit_t i = sections.begin(); i != sections.end(); i++)
         delete i->second;

      current = nullptr;
   }

   //Make any alterations to the raw line
   void nake(const fini_char_t*)  //Strip the line of any non-interpretable characters
   {

   }

   void reserveSections()
   {
   #ifdef FINI_CPP11
      sections.reserve(ALLOCATE_SECTIONS);
   #endif
   }

   void reserveKeys(keys_t* currente)
   {
   #ifdef FINI_CPP11
      currente->reserve(ALLOCATE_KEYS);
   #endif
   }

   bool hasFileAssociation(std::string filenamee)
   {
      if (source == SOURCE_MEMORY && filenamee == "") //No association to a file
         return false;

      return true;
   }

///Output
   //Creates a section as a string
   fini_string_t makeSection(const section_t& section)
   {
      fini_sstream_t line;
      line << '[' << section << ']' << std::endl;

      return line.str();
   }

   //Creates a key and a value as a string
   fini_string_t makeKeyValue(const key_t& key, const value_t& value)
   {
      fini_sstream_t line;
      line << key << '=' << value << std::endl;

      return line.str();
   }
};

///Definitions
template <typename T, typename U>
  inline T Converters::Convert(U value)
{
  fini_sstream_t sout;
  T result;

  sout << value;
  sout >> result;

  sout.str(fini_string_t());

  return result;
}

template <>
  inline fini_string_t Converters::Convert<fini_string_t, fini_string_t>(fini_string_t value)
{
  return value;
}

template <>
  inline fini_string_t Converters::Convert<fini_string_t>(const fini_char_t* value)
{
  return value;
}

template <typename T>
  inline void Converters::GetLine(fini_sstream_t& out, T& value)
{
  out >> value;
}

inline void Converters::GetLine(fini_sstream_t& out, fini_string_t& value)
{
  std::getline(out, value);
}

template <typename T>
   inline size_t Converters::GetDataSize(T& value)
{
   return sizeof(value);
}

inline size_t Converters::GetDataSize(fini_string_t value)
{
   return value.size() + 1;
}

using INIConfig = INI<>;
