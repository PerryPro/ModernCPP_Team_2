#pragma once

#include <iostream>
#include <typeinfo>
#include <string>

class bad_any_cast final : public std::bad_cast{

public:
    virtual const char *what() const noexcept override
    {
        return "bad any_cast";
    }

};

class any final{

    private:

    template<typename T>
    friend const T &any_cast(const any&);

    template<typename T>
    friend T &any_cast(any &);

    template<typename T>
    friend T *const any_cast(const any*);

    template<typename T>
    friend T *any_cast(any *);

    class value_base{
    public:

        value_base() = default;

        value_base(const value_base&) = delete;

        virtual ~value_base() = default;

        virtual value_base *copy() = 0;

        virtual const std::type_info &type() const noexcept= 0;
    };

    template<typename T>
    class value_holder: public value_base{
        
        friend class any;

        template<typename X>
        friend const X &any_cast(const any &);

        template<typename X>
        friend X &any_cast(any &);

        template<typename X>
        friend X *const any_cast(const any *);

        template<typename X>
        friend X *any_cast(any *);

        T data;
        
    public:

        value_holder() = delete;

        value_holder(T t):data(t){};

        virtual ~value_holder() = default;

        virtual value_base *copy() override   
        {//有问题
            return new value_holder(data);
        }

        virtual const std::type_info &type() const noexcept override
        {
            return typeid(T);
        }

    };

    value_base *value = nullptr;

    public:

    constexpr any() = default;

    any(const any& v):value(v.value == nullptr? nullptr : value->copy()){};

    template<typename T>
    any(const T& val):value(new value_holder<T>(val)){};

    ~any()
    {
        delete value;
    }

    any &operator= (const any& v)
    {
        if(&v != this)
        {
            delete value;
            value = v.value == nullptr? nullptr : v.value->copy();
        }
        return *this;
    }

    void reset()
    {
        delete value;
        value = nullptr;
    }

    bool has_value()
    {
        return value != nullptr;
    }

    const std::type_info &type() const noexcept
    {
        return value == nullptr ? typeid(void) : value->type();
    }

    void swap(any &v)
    {
        std::swap(value,v.value);
    }
};

void swap(any &a, any &b)
{
    a.swap(b);
}

template<typename T>
const T &any_cast(const any &v)
{
    if(typeid(T) != v.type())
        throw bad_any_cast();
    return dynamic_cast<any::value_holder<T> *>(v.value)->data;
}

template<typename T>
T &any_cast(any &v)
{
    if(typeid(T) != v.type())
        throw bad_any_cast();
    return dynamic_cast<any::value_holder<T> *>(v.value)->data;
}

template<typename T>
T *const any_cast(const any *v)
{
    if(v == nullptr)
        return nullptr;
    if(typeid(T) != v->type())
        throw bad_any_cast();
    return &dynamic_cast<any::value_holder<T> *>(v->value)->data;
}

template<typename T>
T *any_cast(any *v)
{
    if(v == nullptr)
        return nullptr;
    if(typeid(T) != v->type())
        throw bad_any_cast();
    return &dynamic_cast<any::value_holder<T> *>(v->value)->data;
}
