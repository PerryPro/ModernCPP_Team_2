#pragma once

#include <iostream>
#include <typeinfo>
#include <string>

class bad_any_cast final : public std::bad_cast{

public:

    virtual const char *what() const noexcept{

            return "bad any_cast";

    }

};

class value_handler{

public:

    value_handler() = default;

    virtual ~value_handler(){};

    virtual const std::type_info &type() const noexcept = 0;

    virtual value_handler *copy() const = 0;//复制函数

};


template<typename T>
class handler : public value_handler{

public:

    T data;

    handler(const T &t): data(t) {};

    virtual const std::type_info &type() const noexcept override{

        return typeid(T);

    }

    //复制函数
    virtual value_handler *copy() const override{

        return new handler(data);

    }//0
    
};


class any final{

private:

    value_handler *dataPtr;

    template<typename T>
    friend T make_any(T &v);

    template<typename T>
    friend T &any_cast(const any &a);

public:

    any() = default;

    template<typename T>
    any(const T &v): dataPtr(new handler<T>(v)){};

    any &operator=(const any &a){

        this->dataPtr = a.dataPtr;

        return *this;

    }

    template<typename T>
    any &operator=(T&& t){

        value_handler* dataPtr2 = new handler<T>(t);

        std::swap(dataPtr,dataPtr2);

        return *this;

    }
    

    void reset(){

        if(dataPtr != nullptr)
        delete dataPtr;

        dataPtr = nullptr;

    }

    any &swap(any &a){

        std::swap(dataPtr,a.dataPtr);

        return *this;

    }

    bool has_value(){

        return dataPtr != nullptr;
        
    }


    const std::type_info &type() const noexcept{

        return dataPtr != nullptr ? dataPtr->type() : typeid(void);

    }

    ~any(){

        reset();

    }

};

template<typename T>
T make_any(T &v){

    return any(v);

}

template<typename T>
T &any_cast(const any&a){

    if (a.type() != typeid(T))

        throw bad_any_cast();

    return dynamic_cast<handler<T>*>(a.dataPtr)->data;

}
