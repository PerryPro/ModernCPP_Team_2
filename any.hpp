#pragma once

#include <typeindex>
#include <string>
#include <typeinfo>
#include <cstdio>

class any final {  
private:
    class stor_base {
    public:
        stor_base() = default;
        stor_base(const stor_base &) = default;
        virtual ~stor_base() = default;
        virtual std::type_index type() const = 0;
        virtual stor_base * copy() const = 0;
    };

    template <typename T>
    class stor_impl : public stor_base {
    public:
        T data;
        stor_impl() = default;
        stor_impl(const T& a) : data(a){}
        stor_impl(const stor_impl &) = delete;

        std::type_index type() const  override
        {
            return typeid(T);
        }
    
        stor_base * copy() const override {
            return new stor_impl(data);
        }
    };
private:
    stor_base *value;
public:
    any() {
        value = nullptr;
    }
    ~any() {
        delete value;
    }
    friend any make_any(any&);

    template <typename T>
    any(const T& a) : value(new stor_impl<T>(a)){}
    any(const any& a) {
        value = a.value -> copy();
    }

    any& operator = (any const& a) {
        if(this != &a){
            delete value;
            value = a.value -> copy();
        }
        return *this;
    }

    std::type_index type() {
        return value ? value -> type() : typeid(void);
    }

    any reset() {
        delete value;
        value = nullptr;
        return *this;
    }

    any swap(any& a) {
        std::swap(value, a.value);
        return *this;
    }
};
