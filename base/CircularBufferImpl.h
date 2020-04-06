
namespace std {
    namespace Detail {
        template <class T, size_t N, class Alloc>
        int cb_iter<T, N, Alloc>::prevIndex(int index) {
            --index;
            index = (index == -1 ? index + N : index);
            return index;
        }

        template <class T, size_t N, class Alloc>
        cb_iter<T, N, Alloc>& cb_iter<T, N, Alloc>::operator = (const cb_iter& cit) {
            if(this != &cit) {
                ptr_ = cit.ptr_;
                index_ = cit.index_;
                container_ = cit.container_;
            }

            return *this;
        }

        template <class T, size_t N, class Alloc>
        cb_iter<T, N, Alloc>& cb_iter<T, N, Alloc>::operator ++() {
            setIndex_(nextIndex(index_));
            setPtr_(container_->start_ + index_);
            return *this;
        }

        template <class T, size_t N, class Alloc>
        cb_iter<T, N, Alloc> cb_iter<T, N, Alloc>::operator ++(int) {
            cb_iter temp(*this);
            ++(*this);
            return temp;
        }

        template <class T, size_t N, class Alloc>
        cb_iter<T, N, Alloc>& cb_iter<T, N, Alloc>::operator --() {
            setIndex_(prevIndex(index_));
            setPtr_(container_->start_ + index_);
            return *this;
        }

        template <class T, size_t N, class Alloc>
        cb_iter<T, N, Alloc> cb_iter<T, N, Alloc>::operator --(int) {
            cb_iter temp(*this);
            --(*this);
            return temp;
        }
    }

    template <class T, size_t N, class Alloc>
    circular_buffer<T, N, Alloc>::~circular_buffer() {
        clear();
        dataAllocator.deallocate(start_, N);
    }

    template <class T, size_t N, class Alloc>
    circular_buffer<T, N, Alloc>::circular_buffer(const int& n, const value_type& val) {
        allocateAndFillN(n, val);
    }

    template <class T, size_t N, class Alloc>
    circular_buffer<T, N, Alloc>::circular_buffer(const circular_buffer& cb) {
        clone(cb);
    }

    template <class T, size_t N, class Alloc>
    circular_buffer<T, N, Alloc>::circular_buffer(circular_buffer&& cb) {
        copyAllMembers(cb);
        zeroCircular(cb);
    }

    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::push_back(const T& val){
        if (full()){
            indexOfTail = nextIndex(indexOfTail);
            dataAllocator.destroy(start_ + indexOfTail);
            dataAllocator.construct(start_ + indexOfTail, val);
            indexOfHead = nextIndex(indexOfHead);
        }
        else{
            indexOfTail = nextIndex(indexOfTail);
            dataAllocator.construct(start_ + indexOfTail, val);
            ++size_;
        }
    }

    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::clear(){
        for (; !empty(); indexOfHead = nextIndex(indexOfHead), --size_){
            dataAllocator.destroy(start_ + indexOfHead);
        }
        indexOfHead = indexOfTail = 0;
    }

    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::allocateAndFillN(const int& n, const value_type& val){
        start_ = dataAllocator.allocate(N);
        finish_ = start_ + N;
        indexOfHead = 0;
        if (N <= n){
            finish_ = std::uninitialized_fill_n(start_, N, val);
            indexOfTail = N - 1;
            size_ = N;
        }
        else{
            finish_ = std::uninitialized_fill_n(start_, n, val);
            finish_ = std::uninitialized_fill_n(finish_, N - n, value_type());
            indexOfTail = n - 1;
            size_ = n;
        }
    }

    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::copyAllMembers(const circular_buffer& cb){
        start_ = cb.start_;
        finish_ = cb.finish_;
        indexOfHead = cb.indexOfHead;
        indexOfTail = cb.indexOfTail;
        size_ = cb.size_;
    }

    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::zeroCircular(circular_buffer& cb){
        cb.start_ = cb.finish_ = 0;
        cb.indexOfHead = cb.indexOfTail = cb.size_ = 0;
    }
    template<class T, size_t N, class Alloc>
    void circular_buffer<T, N, Alloc>::clone(const circular_buffer& cb){
        start_ = dataAllocator.allocate(N);
        finish_ = start_ + N;
        size_ = N;
        indexOfHead = cb.indexOfHead;
        indexOfTail = cb.indexOfTail;
        std::uninitialized_copy(cb.start_, cb.finish_, start_);
    }
}