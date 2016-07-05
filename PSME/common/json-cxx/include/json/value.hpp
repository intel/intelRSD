/*!
 * @copyright
 * Copyright (c) 2015, Tymoteusz Blazejczyk
 *
 * @copyright
 * All rights reserved.
 *
 * @copyright
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * @copyright
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * @copyright
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * @copyright
 * * Neither the name of json-cxx nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * @copyright
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file json/value.hpp
 *
 * @brief JSON value interface
 * */

#ifndef JSON_CXX_VALUE_HPP
#define JSON_CXX_VALUE_HPP

#include "json/number.hpp"

#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

namespace json {

template<bool is_const>
class base_iterator;

class Value;

/*! JSON string */
using String = std::string;

/*! JSON member pair key:value */
using Pair = std::pair<String, Value>;

/*! JSON object that contain JSON members */
using Object = std::vector<Pair>;

/*! JSON array that contains JSON values */
using Array = std::vector<Value>;

/*! JSON boolean */
using Bool = bool;

/*! JSON null */
using Null = std::nullptr_t;

/*!
 * @brief JSON value
 *
 * JSON value class that can contain JSON object, array, number, boolen or null
 * */
class Value {
public:
    friend class Deserializer;

    /*! Non-const iterator that can iterate through JSON values */
    using iterator = base_iterator<false>;

    /*! Const iterator that can iterate through JSON values */
    using const_iterator = base_iterator<true>;

    /*! JSON type */
    enum class Type {
        NIL,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN
    };

    /*!
     * @brief JSON value exception
     * */
    class Exception : public std::runtime_error {
    public:
        /*!
         * @brief JSON value exception constructor
         *
         * @param[in]   str JSON value exception message
         * */
        Exception(const char* str);

        /*!
         * @brief JSON value exception constructor
         *
         * @param[in]   str JSON value exception message
         * */
        Exception(const std::string& str);

        /*! Exception copy constructor */
        Exception(const Exception&) = default;

        /*! Exception destructor */
        ~Exception();
    };

    /*!
     * @brief Default constructor
     *
     * Without any arguments creates JSON null. Passed Type to constructor will
     * create given JSON type with zero initialization. For object/array
     * creates empty container, for boolean type set to false, for
     * number set to zero and for string set to ""
     *
     * @param[in]   type    JSON type
     * */
    Value(Type type = Type::NIL);

    /*!
     * @brief Create JSON null
     *
     * This constructor is equivalent to default constructor without any given
     * arguments or with Type::NIL
     *
     * @code
     * Value value(nullptr);
     * @endcode
     *
     * @param[in]   null    Valid argument is only nullptr
     * */
    Value(Null null);

    /*!
     * @brief Create JSON boolean
     *
     * Initialization with given boolean argument
     *
     * @param[in]   boolean  May be true or false
     * */
    Value(Bool boolean);

    /*!
     * @brief Create JSON string
     *
     * Create JSON string with given null-terminated characters array.
     * String will be copied
     *
     * param[in]    str     Null-terminated characters array
     * */
    Value(const char* str);

    /*!
     * @brief Create JSON string
     *
     * Create JSON string with given String object.
     * String will be copied
     *
     * param[in]    str     String object
     * */
    Value(const String& str);

    /*!
     * @brief Create JSON object with member
     *
     * Initialize object as JSON object with one given JSON member as pair
     * that contains key:value
     *
     * @param[in]   pair    JSON pair key:value
     * */
    Value(const Pair& pair);

    /*!
     * @brief Create JSON object with member
     *
     * Initialize object as JSON object with one given JSON member as
     * separate key string and JSON value. Key string is null-terminated
     * characters array
     *
     * @param[in]   key     Unique key string for value
     * @param[in]   value   JSON value
     * */
    Value(const char* key, const Value& value);

    /*!
     * @brief Create JSON object with member
     *
     * Initialize object as JSON object with one given JSON member as
     * separate key string and JSON value
     *
     * @param[in]   key     Unique key string for value
     * @param[in]   value   JSON value
     * */
    Value(const String& key, const Value& value);

    /*!
     * @brief Create JSON number as unsigned int
     *
     * Initialize number with given value
     *
     * @param[in]   value   Unsigned integer
     * */
    Value(Uint value);

    /*!
     * @brief Create JSON number as unsigned int
     *
     * Initialize number with given value
     *
     * @param[in]   value   Unsigned integer
     * */
    Value(Int value);

    /*!
     * @brief Create JSON number as double
     *
     * Initialize number with given value
     *
     * @param[in]   value   Double
     * */
    Value(Double value);

    /*!
     * @brief Create JSON number
     *
     * Initialize object with given JSON number
     *
     * @param[in]   number   JSON Number
     * */
    Value(const Number& number);

    /*!
     * @brief Create JSON array with copies of JSON values
     *
     * @param[in]   count   Array size
     * @param[in]   value   JSON value used to fill JSON array
     * */
    Value(size_t count, const Value& value);

    /*!
     * @brief Create JSON object with given JSON members
     *
     * @param[in]   init_list   JSON members list used to fill JSON object
     * */
    Value(std::initializer_list<Pair> init_list);

    /*!
     * @brief Create JSON array with given JSON values
     *
     * @param[in]   init_list   JSON values list used to fill JSON array
     * */
    Value(std::initializer_list<Value> init_list);

    /*!
     * @brief Copy constructor
     * */
    Value(const Value& value);

    /*!
     * @brief Move constructor
     *
     * After moving JSON value to new object, given JSON value is changed to
     * JSON null
     * */
    Value(Value&& value);

    /*!
     * @brief Destructor
     *
     * Call suitable JSON type destructor based on his Type
     * */
    ~Value();

    /*!
     * @brief Copy assignment
     *
     * If JSON values type are not equal, destroy old contents, create new JSON
     * type based on given JSON value argument and copy all data
     *
     * @param[in]   value   JSON value to copy
     * */
    Value& operator=(const Value& value);

    /*!
     * @brief Move assignment
     *
     * If JSON values type are not equal, destroy old contents, create new JSON
     * type based on given JSON value argument and move all data. JSON value
     * that data have been moved to object, will be changed to JSON null
     *
     * @param[in]   value   JSON value to move
     * */
    Value& operator=(Value&& value);

    /*!
     * @brief Assignment JSON object with JSON members
     *
     * Destroy old contents and assignment new value. JSON type will be changed
     * to JSON object
     *
     * @param[in]   init_list   JSON object with JSON members
     * */
    Value& operator=(std::initializer_list<Pair> init_list);

    /*!
     * @brief Assignment JSON array with JSON values
     *
     * Destroy old contents and assignment new value. JSON type will be changed
     * to JSON array
     *
     * @param[in]   init_list   JSON array with JSON values
     * */
    Value& operator=(std::initializer_list<Value> init_list);

    /*!
     * @brief Add new JSON value to JSON array
     *
     * If object is a JSON array, add new JSON value to the end
     * If object is a JSON object and given JSON value is a member or an object,
     * add contents to the end
     * If object is a JSON string and given JSON value is also a string, append
     * existing string with new content at the end
     *
     * param[in]    value   JSON value
     * */
    Value& operator+=(const Value& value);

    /*!
     * @brief Assign JSON array with JSON value copies
     *
     * Replace actual content with new JSON array that holds JSON value copies.
     * JSON Type will be changed to JSON array
     *
     * @param[in]   count   Array size
     * @param[in]   value   JSON value copies
     * */
    void assign(size_t count, const Value& value);

    /*!
     * @brief Assign JSON object with JSON members
     *
     * Replace actual content with new JSON objects that holds JSON members.
     * JSON Type will be changed to JSON object
     *
     * @param[in]   init_list   JSON object with JSON members
     * */
    void assign(std::initializer_list<Pair> init_list);

    /*!
     * @brief Assign JSON array with JSON values
     *
     * Replace actual content with new JSON array that holds JSON values.
     * JSON Type will be changed to JSON array
     *
     * @param[in]   init_list   JSON array with JSON values
     * */
    void assign(std::initializer_list<Value> init_list);

    /*!
     * @brief Push at the end new JSON member
     *
     * This work only for JSON object type. When JSON is a null, JSON type
     * will be changed to JSON object and then perform push_back() method
     *
     * @param[in]   pair    JSON member key:value
     * */
    void push_back(const Pair& pair);

    /*!
     * @brief Push at the end new JSON value
     *
     * This work only for JSON array type. When JSON is a null, JSON type
     * will be changed to JSON array and then perform push_back() method
     *
     * @param[in]   value    JSON value
     * */
    void push_back(const Value& value);

    /*!
     * @brief Pop JSON value or member from JSON array or object
     *
     * This work only for JSON array or object type
     * */
    void pop_back();

    /*!
     * @brief Get number of elements in JSON array or object type
     *
     * For any others JSON types it returns always zero
     *
     * @return  Number of JSON array or object elements
     * */
    size_t size() const;

    /*!
     * @brief Clear
     *
     * For JSON array or object clear all elements, the size will be zero
     * For JSON string clear to empty "" string
     * For JSON number clear to zero
     * For JSON boolean change value to false
     * */
    void clear();

    /*!
     * @brief Check if JSON array or object is empty
     *
     * Equivalent to !size() method
     *
     * @return   true when container is empty otherwise false
     * */
    bool empty() const { return !size(); }

    /*!
     * @brief Erase member from JSON object with given key
     *
     * Available only for JSON object
     *
     * @param[in]   key     String object
     * */
    size_t erase(const String& key);

    /*!
     * @brief Erase member from JSON object with given key
     *
     * Available only for JSON object
     *
     * @param[in]   key     Null-terminated characters array
     * */
    size_t erase(const char* key);

    /*!
     * @brief Erase JSON value from JSON object or array
     *
     * @param[in]   pos Iterator to the JSON value to remove
     *
     * @return Iterator following the last removed element. If the iterator
     * pos refers to the last element, the end() iterator is returned
     * */
    iterator erase(const_iterator pos);

    /*!
     * @brief Erase JSON values from JSON object or array
     *
     * @param[in]   first   First iterator for range removing
     * @param[in]   last    Last iterator for range removing
     *
     * @return Iterator following the last removed element. If the iterator
     * pos refers to the last element, the end() iterator is returned
     * */
    iterator erase(const_iterator first, const_iterator last);

    /*!
     * @brief Insert new JSON value for JSON array
     *
     * @param[in]   pos     Iterator before which the content will be inserted
     * @param[in]   value   JSON value to insert
     *
     * @return Iterator pointing to the inserted JSON value
     * */
    iterator insert(const_iterator pos, const Value& value);

    /*!
     * @brief Insert new JSON value for JSON array
     *
     * @param[in]   pos     Iterator before which the content will be inserted
     * @param[in]   value   JSON value to insert using move semantic
     *
     * @return Iterator pointing to the inserted JSON value
     * */
    iterator insert(const_iterator pos, Value&& value);

    /*!
     * @brief Insert new JSON values for JSON array
     *
     * @param[in]   pos     Iterator before which the content will be inserted
     * @param[in]   count   Repeat insertion
     * @param[in]   value   JSON value to insert using
     *
     * @return Iterator pointing to the inserted JSON value
     * */
    iterator insert(const_iterator pos, size_t count, const Value& value);

    /*!
     * @brief Insert new JSON values for JSON array
     *
     * @param[in]   pos     Iterator before which the content will be inserted
     * @param[in]   first   First iterator for range insertion
     * @param[in]   last    Last iterator for range insertion
     *
     * @return Iterator pointing to the first inserted JSON value or pos if
     * first == last
     * */
    iterator insert(const_iterator pos,
            const_iterator first, const_iterator last);

    /*!
     * @brief Insert new JSON values for JSON array
     *
     * @param[in]   pos         Iterator before which the content will be
     *                          inserted
     * @param[in]   init_list   Initializer list to insert the JSON values from
     *
     * @return Iterator pointing to the first inserted JSON value or pos if
     * initialize list is empty
     * */
    iterator insert(const_iterator pos, std::initializer_list<Value> init_list);

    /*!
     * @brief Swap JSON values
     *
     * @param[in]   value JSON value to swap
     * */
    void swap(Value& value);

    /*!
     * @brief Array access to JSON value in JSON array or JSON member in JSON
     * object
     *
     * When object is null type, after calling operator[] change type to JSON
     * array. When index is equal to size(), append only JSON array with new
     * JSON null element at the end
     *
     * @code
     *  Value value;    // Create JSON null
     *  value[0] = 1;   // Change type to JSON array and assign number
     *  value[1];       // Append array with new JSON null element
     * @endcode
     *
     * @param[in]   index   Element index access
     *
     * @return  JSON value indexed in array or object, otherwise return myself
     * */
    Value& operator[](size_t index);

    /*!
     * @brief Array access to JSON value in JSON array or JSON member in JSON
     * object
     *
     * When object is not neither array or object, return myself
     *
     * @param[in]   index   Element index access
     *
     * @return  JSON value indexed in array or object, otherwise return myself
     * */
    const Value& operator[](size_t index) const;

    /*!
     * @brief Array access to JSON value in JSON array or object
     *
     * When object is null type, after calling operator[] change type to JSON
     * array. When index is equal to size(), append JSON array with new
     * JSON null element at the end
     *
     * @code
     *  Value value;    // Create JSON null
     *  value[0] = 1;   // Change type to JSON array and assign number
     *  value[1];       // Append array with new JSON null element
     * @endcode
     *
     * @param[in]   index   Element index access
     *
     * @return  JSON value indexed in array or object, otherwise return myself
     * */
    Value& operator[](int index);

    /*!
     * @brief Array access to JSON value in JSON array or object
     *
     * When object is neither array or object, return myself
     *
     * @param[in]   index   Element index access
     *
     * @return  JSON value indexed in array or object, otherwise return myself
     * */
    const Value& operator[](int index) const;

    /*!
     * @brief Array access to JSON value in JSON object
     *
     * Only valid for JSON object or null type. When object is a JSON null, it
     * will be converted to JSON object with one JSON member that contains
     * given key and value as JSON null. When operator[] cannot find member
     * with given key, create new JSON member at the end with given key and
     * value as JSON null. For other JSON types return myself
     *
     * @param[in]   key     JSON key member that identify JSON value in object
     * @return      JSON value from JSON object
     * */
    Value& operator[](const char* key);

    /*!
     * @brief Array access to JSON value in JSON object
     *
     * Only valid for JSON object. When operator[] cannot find member
     * with given key, it returns global const JSON null. For other JSON types
     * return myself
     *
     * @param[in]   key     JSON key member that identify JSON value in object
     * @return      JSON value from JSON object
     * */
    const Value& operator[](const char* key) const;

    /*!
     * @brief Array access to JSON value in JSON object
     *
     * Only valid for JSON object or null type. When object is a JSON null, it
     * will be converted to JSON object with one JSON member that contains
     * given key and value as JSON null. When operator[] cannot find member
     * with given key, create new JSON member at the end with given key and
     * value as JSON null. For other JSON types return myself
     *
     * @param[in]   key     JSON key member that identify JSON value in object
     * @return      JSON value from JSON object
     * */
    Value& operator[](const String& key);

    /*!
     * @brief Array access to JSON value in JSON object
     *
     * Only valid for JSON object. When operator[] cannot find member
     * with given key, it returns global const JSON null. For other JSON types
     * return myself
     *
     * @param[in]   key     JSON key member that identify JSON value in object
     * @return      JSON value from JSON object
     * */
    const Value& operator[](const String& key) const;

    /*!
     * @brief Get JSON type
     * */
    Type get_type() const { return m_type; }

    /*!
     * @brief Check if JSON value is a string
     * @return true when is otherwise false
     * */
    bool is_string() const {
        return Type::STRING == m_type;
    }

    /*!
     * @brief Check if JSON value is a object
     * @return true when is otherwise false
     * */
    bool is_object() const {
        return Type::OBJECT == m_type;
    }

    /*!
     * @brief Check if JSON value is a array
     * @return true when is otherwise false
     * */
    bool is_array() const {
        return Type::ARRAY == m_type;
    }

    /*!
     * @brief Check if JSON value is a number
     * @return true when is otherwise false
     * */
    bool is_number() const {
        return Type::NUMBER == m_type;
    }

    /*!
     * @brief Check if JSON value is a boolean
     * @return true when is otherwise false
     * */
    bool is_boolean() const {
        return Type::BOOLEAN == m_type;
    }

    /*!
     * @brief Check if JSON value is a null
     * @return true when is otherwise false
     * */
    bool is_null() const {
        return Type::NIL == m_type;
    }

    /*!
     * @brief Check if JSON value is a signed integer
     * @return true when is otherwise false
     * */
    bool is_int() const {
        return is_number() ? Number(m_number).is_int() : false;
    }

    /*!
     * @brief Check if JSON value is a unsigned integer
     * @return true when is otherwise false
     * */
    bool is_uint() const {
        return is_number() ? Number(m_number).is_uint() : false;
    }

    /*!
     * @brief Check if JSON value is a double
     * @return true when is otherwise false
     * */
    bool is_double() const {
        return is_number() ? Number(m_number).is_double() : false;
    }

    /*!
     * @brief Check if JSON member with given key exist in JSON object
     *
     * @param[in]   key     key string to identify JSON member in JSON object
     *
     * @return true when member exist otherwise false
     * */
    bool is_member(const std::string& key) const;

    /*!
     * @brief Check if JSON member with given key exist in JSON object
     *
     * @param[in]   key     key string to identify JSON member in JSON object
     *
     * @return true when member exist otherwise false
     * */
    bool is_member(const char* key) const;

    /*! Convert JSON value to string */
    explicit operator String&() { return m_string; }

    /*! Convert JSON value to string */
    explicit operator const String&() const { return m_string; }

    /*! Convert JSON value to string */
    explicit operator const char*() const { return m_string.c_str(); }

    /*! Convert JSON value to boolean */
    explicit operator Bool() const { return m_boolean; }

    /*! Convert JSON value to null */
    explicit operator Null() const { return nullptr; }

    /*! Convert JSON value to signed integer */
    explicit operator Int() const { return Int(m_number); }

    /*! Convert JSON value to unsigned integer */
    explicit operator Uint() const { return Uint(m_number); }

    /*! Convert JSON value to double */
    explicit operator Double() const { return Double(m_number); }

    /*! Convert JSON value to array */
    explicit operator Array&() { return m_array; }

    /*! Convert JSON value to number */
    explicit operator Number&() { return m_number; }

    /*! Convert JSON value to array */
    explicit operator const Array&() const { return m_array; }

    /*! Convert JSON value to object */
    explicit operator const Object&() const { return m_object; }

    /*! Convert JSON value to number */
    explicit operator const Number&() const { return m_number; }

    /*! Convert JSON value to string */
    String& as_string();

    /*! Convert JSON value to string */
    const String& as_string() const;

    /*! Convert JSON value to string */
    const char* as_char() const;

    /*! Convert JSON value to boolean */
    Bool as_bool() const;

    /*! Convert JSON value to null */
    Null as_null() const;

    /*! Convert JSON value to signed integer */
    Int as_int() const;

    /*! Convert JSON value to unsigned integer */
    Uint as_uint() const;

    /*! Convert JSON value to double */
    Double as_double() const;

    /*! Convert JSON value to array */
    Array& as_array();

    /*! Convert JSON value to number */
    Number& as_number();

    /*! Convert JSON value to array */
    const Array& as_array() const;

    /*! Convert JSON value to object */
    const Object& as_object() const;

    /*! Convert JSON value to number */
    const Number& as_number() const;

    /*! Equivalent to is_null() */
    bool operator!() const { return is_null(); }

    /*! JSON values comparison */
    friend bool operator==(const Value&, const Value&);

    /*! JSON values comparison */
    friend bool operator!=(const Value&, const Value&);

    /*! JSON values comparison */
    friend bool operator<(const Value&, const Value&);

    /*! JSON values comparison */
    friend bool operator>(const Value&, const Value&);

    /*! JSON values comparison */
    friend bool operator<=(const Value&, const Value&);

    /*! JSON values comparison */
    friend bool operator>=(const Value&, const Value&);

    /*! Begin iterator */
    iterator begin();

    /*! End iterator */
    iterator end();

    /*! Begin const iterator */
    const_iterator begin() const;

    /*! End const iterator */
    const_iterator end() const;

    /*! Begin const iterator */
    const_iterator cbegin() const;

    /*! End const iterator */
    const_iterator cend() const;
private:
    enum Type m_type;

    union {
        Object m_object;
        Array m_array;
        String m_string;
        Number m_number;
        Bool m_boolean;
    };

    void create_container(Type type);
};

/*! JSON values comparison */
bool operator==(const Value&, const Value&);

/*! JSON values comparison */
inline bool operator!=(const Value& val1, const Value& val2) {
    return !(val1 == val2);
}

/*! JSON values comparison */
bool operator< (const Value&, const Value&);

/*! JSON values comparison */
inline bool operator> (const Value& val1, const Value& val2) {
    return val2 < val1;
}

/*! JSON values comparison */
inline bool operator<=(const Value& val1, const Value& val2) {
    return !(val2 < val1);
}

/*! JSON values comparison */
inline bool operator>=(const Value& val1, const Value& val2) {
    return !(val1 < val2);
}

} /* namespace json */

#endif /* JSON_CXX_VALUE_HPP */
