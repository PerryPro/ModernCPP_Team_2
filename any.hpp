#pragma once

#include <iostream>
#include <string>
#include <typeinfo>

class bad_any_cast final: public std::bad_cast{

    public:
    virtual const char *what() const noexcept override
    {
        return "bad any_cast";
    }
};

class any final{
    private:

    template<typename T>
    friend const T &any_cast(const any &);

    template<typename T>
    friend T &any_cast(any &);

    template<typename T>
    friend T *const any_cast(const any *);

    template<typename T>
    friend T *any_cast(any *);

    class value_base{
        public:

        value_base() = default;

        value_base(const value_base &) = delete;

        virtual ~value_base() = default;

        virtual value_base *copy() = 0;

        virtual const std::type_info &type() const noexcept = 0;
    };

    template<typename T>
    class value_holder : public value_base{

        friend class any;

        template<typename X>
        friend const X &any_cast(const any&);

        template<typename X>
        friend X &any_cast(any &);

        template<typename X>
        friend X *const any_cast(const any *);

        template<typename X>
        friend X *any_cast(any *);

        T data;

        public:

        value_holder() = delete;

        value_holder(T t): data(t){};

        virtual ~value_holder() = default;

        virtual value_base *copy() override
        {
            return new value_holder(data);
        }

        virtual const std::type_info &type() const noexcept override
        {
            return typeid(T);
        }
    };

    value_base *data = nullptr;

    public:

    constexpr any() = default;

    template<typename T>
    any(const T &val): data(new value_holder<T>(val)){}; //

    ~any()
    {
        delete data;
    }

    any(const any &v): data(v.data == nullptr? nullptr: v.data->copy()){};

    any &operator= (const any &v)
    {
        if(&v != this)
        {
            delete data;
            data = nullptr;
            this->data = (v.data == nullptr ? nullptr : v.data->copy());
        }
        return *this;
    }

    const std::type_info &type() const noexcept
    {
        return (data == nullptr ? typeid(void) : data->type());
    }

    void swap(any &v)
    {
        std::swap(data, v.data);
    }

    void reset()
    {
        delete data;
        data = nullptr;
    }

    bool has_value()
    {
        return data != nullptr;
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
    return dynamic_cast<any::value_holder<T> *>(v.data)->data;
}

template<typename T>
T &any_cast(any &v)
{
    if(typeid(T) != v.type())
        throw bad_any_cast();
    return dynamic_cast<any::value_holder<T> *>(v.data)->data;
}

template<typename T>
T *const any_cast(const any *v)
{
    if(v == nullptr)
        return nullptr;
    if(typeid(T) != v->type())
        throw bad_any_cast();
    return &dynamic_cast<any::value_holder<T> *>(v->data)->data;
}

template<typename T>
T *any_cast(any *v)
{
    if(v == nullptr)
        return nullptr;
    if(typeid(T) != v->type())
        throw bad_any_cast();
    return &dynamic_cast<any::value_holder<T> *>(v->data)->data;    
}


