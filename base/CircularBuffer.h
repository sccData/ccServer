#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H
#include <memory>
#include <iterator>

namespace std {
    template <class T, size_t N, class Alloc>
    class circular_buffer;
    namespace Detail {
        template <class T, size_t N, class Alloc = allocator<T>>
        class cb_iter {
        public:
            typedef bidirectional_iterator_tag iterator_category;
            typedef T                          value_type;
            typedef ptrdiff_t                  difference_type;
            typedef T*                         pointer;
            typedef T&                         reference;

            typedef std::circular_buffer<T, N, Alloc> cb;
            typedef cb* cbPtr;

        public:
            cb_iter() : ptr_(0), index_(0), container_(0) {}
            cb_iter(T* ptr, cbPtr container) :
                    ptr_(ptr), index_(ptr - container->start_), container_(container) {}
            cb_iter(const cb_iter& cit) :
                    ptr_(cit.ptr_), index_(cit.index_), container_(cit.container_) {}

            cb_iter& operator = (const cb_iter& cit);

        public:
            operator T*()   { return ptr_; }
            T& operator *() { return *ptr_; }
            T* operator->() { &(operator*()); }

            cb_iter& operator ++();
            cb_iter  operator ++(int);
            cb_iter& operator --();
            cb_iter  operator --(int);

        private:
            void setIndex_(int index) { index_ = index; }
            void setPtr_(T* ptr)      { ptr_ = ptr; }
            int  nextIndex(int index) { return ((++index) % N); }
            int  prevIndex(int index);

        private:
            T* ptr_;
            int index_;
            cbPtr container_;
        };
    }

    template <class T, size_t N, class Alloc = allocator<T>>
    class circular_buffer {
    
        friend class Detail::cb_iter<T, N, Alloc>;
    public:
        typedef T                     value_type;
        typedef Detail::cb_iter<T, N> iterator;
        typedef iterator              pointer;
        typedef T&                    reference;
        typedef int                   size_type;
        typedef ptrdiff_t             difference_type;

    public:
        explicit circular_buffer(const int& n, const value_type& val = value_type());
        circular_buffer(const circular_buffer& cb);
        circular_buffer(circular_buffer&& cb);
        ~circular_buffer();

        bool full()       { return (size_ == N); }
        bool empty()      { return (size_ == 0); }
        size_type size()  { return size_; }
        void clear();

        iterator begin() { return iterator(start_ + indexOfHead, this); }
        iterator end()   { return iterator(start_ + indexOfTail, this); }

        reference operator[](size_type i) { return *(start_ + i); }
        reference front()                 { return *(start_ + indexOfHead); }
        reference back()                  { return *(start_ + indexOfTail); }
        void push_back(const T& val);

    private:
        void allocateAndFillN(const int& n, const value_type& val);
        int nextIndex(int index) { return ((index + 1) % N); }
        void copyAllMembers(const circular_buffer& cb);
        void zeroCircular(circular_buffer& cb);
        void clone(const circular_buffer& cb);
        
    private:
        T* start_;
        T* finish_;
        int indexOfHead;
        int indexOfTail;
        size_type size_;
        Alloc dataAllocator;
    };
}

#include "CircularBufferImpl.h"
#endif
