#pragma once
#include <iostream>
#include <typeindex>
#include <typeinfo>
using std::type_index;
class value_base
{
public:
    virtual ~value_base() = default;
    virtual value_base *copy() const noexcept = 0;
    virtual const type_index get_type() const noexcept = 0;
};
template <class T>
class value : public value_base
{
private:
    T data;

public:
    ~value() = default;
    value(const T &tmp)
    {
        data = tmp;
    }
    virtual value_base *copy() const noexcept override
    {
        return new value(data);
    }
    virtual const type_index get_type() const noexcept override
    {
        return typeid(data);
    }

    T &get_value()
    {
        return data;
    }
};
class any final
{
private:
    value_base *base_ptr = nullptr;

public:
    ~any()
    {
        delete base_ptr;
    }
    template <class T>
    any(const T &tmp)
    {
        base_ptr = new value<T>(tmp);
    }
    any(const any &tmp)
    {
        base_ptr = tmp.get_base_ptr()->copy();
    }
    value_base *get_base_ptr() const
    {
        return base_ptr;
    }
    void set_base_ptr(value_base *tmp_ptr)
    {
        base_ptr = tmp_ptr;
    }
    any &operator=(const any &tmp)
    {
        if (this != &tmp)
        {
            delete base_ptr;
            base_ptr = tmp.get_base_ptr()->copy();
        }
        return *this;
    }
    const type_index type() const
    {
        return base_ptr ? base_ptr->get_type() : typeid(void);
    }
    void reset()
    {
        delete base_ptr;
        base_ptr = nullptr;
    }
    any &swap(any &tmp)
    {
        std::swap(base_ptr, tmp.base_ptr);
        return *this;
    }
    bool has_value()
    {
        if (base_ptr == nullptr)
            return false;
        return true;
    }
    template <class T>
    friend T &any_cast(const any &a);
    template <class T>
    friend T make_any(T &d);
};
class bad_any_cast final : public std::bad_cast
{
public:
    virtual const char *what() const noexcept
    {

        return "bad any_cast";
    }
};
template <class T>
T &any_cast(const any &a)
{
    if (a.type() != typeid(T))

        throw bad_any_cast();

    return dynamic_cast<value<T> *>(a.base_ptr)->get_value();
}
template <class T>
any make_any(T &d)
{
    return any(d);
}