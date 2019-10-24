#pragma once
#include <iostream>
template <class T>
class data_holder final
{
    T *data_ptr = nullptr;
    mutable long r_count = 0;

public:
    ~data_holder()
    {
        delete data_ptr;
    }
    long get_count() const
    {
        return r_count;
    }
    void add_count()
    {
        r_count++;
    }
    void del_count()
    {
        r_count--;
    }
    data_holder(T *ptr)
    {
        data_ptr = ptr;
        r_count = 1;
    }
    T *get_ptr() const
    {
        return data_ptr;
    }
};
template <class T>
class shared_ptr final
{
    data_holder<T> *holder_ptr = nullptr;

public:
    ~shared_ptr()
    {
        del_op();
    }
    void del_op() noexcept
    {
        if (holder_ptr)
        {
            holder_ptr->del_count();
            if (holder_ptr->get_count() == 0)
            {
                delete holder_ptr;
            }
        }
    }
    shared_ptr() = default;
    explicit shared_ptr(T *ptr)
    {
        holder_ptr = new data_holder<T>(ptr);
    }
    shared_ptr(const shared_ptr &another) noexcept
    {
        holder_ptr = another.holder_ptr;
        if (holder_ptr)
        {
            holder_ptr->add_count();
        }
    }
    T *get() const noexcept
    {
        if (holder_ptr && holder_ptr->get_ptr())
        {
            return holder_ptr->get_ptr();
        }
    }
    shared_ptr &operator=(const shared_ptr &another) noexcept
    {
        if (this != &another)
        {
            del_op();
            holder_ptr = another.holder_ptr;
            if (holder_ptr)
            {
                holder_ptr->add_count();
            }
        }
        return *this;
    }
    T &operator*() const noexcept
    {
        if (holder_ptr)
        {
            if (holder_ptr->get_count() > 0 && holder_ptr->get_ptr())
            {
                return *holder_ptr->get_ptr();
            }
        }
    }
    T *operator->() const noexcept
    {
        if (holder_ptr->get_count() > 0 && holder_ptr->get_ptr())
        {
            return holder_ptr->get_ptr();
        }
    }
    T &operator[](std::ptrdiff_t idx) const
    {
        if (holder_ptr && holder_ptr->get_ptr())
        {
            return holder_ptr->get_ptr()[idx];
        }
    }
    shared_ptr &swap(shared_ptr &another) noexcept
    {
        std::swap(holder_ptr, holder_ptr);
        return *this;
    }
    void reset(T *ptr)
    {
        if (holder_ptr && get())
        {
            holder_ptr->del_count();
            if (holder_ptr->get_count() == 0)
            {
                delete holder_ptr;
            }
        }
        holder_ptr = new data_holder<T>(ptr);
    }
    void reset() noexcept
    {
        del_op();
        holder_ptr = nullptr;
    }
    long use_count() const noexcept
    {
        if (holder_ptr)
        {
            return holder_ptr->get_count();
        }
    }
};