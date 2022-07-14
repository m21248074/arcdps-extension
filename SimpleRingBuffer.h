#pragma once

#include <cstddef>
#include <malloc.h>
#include <iterator>

/**
 * Problems with this implementation:
 * - begin() always return capacityBegin and not least written element
 * - end() always returns the latest element in the list. When capacity is filled, it is always capacityEnd.
 *
 * @tparam T
 * @tparam Capacity
 */
template<typename T, size_t Capacity>
class RingBuffer {
public:
    RingBuffer() {
        mCapacityBegin = (T*)calloc(Capacity, sizeof(T));
        mCapacityEnd = mCapacityBegin + Capacity;
        mCurrent = mCapacityBegin;
        mSizeEnd = mCapacityBegin;
    }

    virtual ~RingBuffer() {
        for (auto it = begin(); it != end(); ++it) {
            (*it).~T();
        }

        free(mCapacityBegin);
    }

    void PushBack(T&& pElement);
    void PushBack(const T& pElement);

    template <typename ...Args>
    void EmplaceBack(const Args& ...args);

    template <bool Const>
    struct RingBufferIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        RingBufferIterator(pointer pPtr) : mPtr(pPtr) {}
        RingBufferIterator(const RingBufferIterator& pIterator) = default;

        RingBufferIterator& operator=(const RingBufferIterator& rawIterator) = default;
        RingBufferIterator& operator=(pointer ptr){
            mPtr = ptr;
            return *this;
        }

        reference operator*() requires(!Const) { return *mPtr; }
        const reference operator*() const requires(Const) { return *mPtr; }
        pointer operator->() requires(!Const) { return mPtr; }

        // Prefix increment
        RingBufferIterator& operator++() {
            ++mPtr;
            return *this;
        }
        RingBufferIterator& operator--() {
            --mPtr;
            return *this;
        }

        // Postfix increment
        RingBufferIterator operator++(int) {
            RingBufferIterator tmp = *this;
            ++mPtr;
            return tmp;
        }
        RingBufferIterator operator--(int) {
            RingBufferIterator tmp = *this;
            --mPtr;
            return tmp;
        }

        bool operator==(const RingBufferIterator& rhs) const {
            return mPtr == rhs.mPtr;
        }

    private:
        pointer mPtr;
    };

    using ConstIterator = RingBufferIterator<true>;
    using Iterator = RingBufferIterator<false>;

    Iterator begin() { return Iterator(mCapacityBegin); }
    Iterator end() { return Iterator(mSizeEnd); }
    ConstIterator begin() const { return ConstIterator(mCapacityBegin); }
    ConstIterator end() const { return ConstIterator(mSizeEnd); }
    ConstIterator cbegin() const { return ConstIterator(mCapacityBegin); }
    ConstIterator cend() const { return ConstIterator(mSizeEnd); }

private:
    T* mCurrent;
    T* mSizeEnd;
    T* mCapacityBegin;
    T* mCapacityEnd;

    T* pushOne();
};

template<typename T, size_t Size>
void RingBuffer<T, Size>::PushBack(T&& pElement) {
    T* elem = pushOne();
    new (elem) T(pElement);
}

template<typename T, size_t Size>
void RingBuffer<T, Size>::PushBack(const T& pElement) {
    T* elem = pushOne();
    new (elem) T(pElement);
}

template<typename T, size_t Capacity>
T* RingBuffer<T, Capacity>::pushOne() {
    // If capacity not given, push end once and get old end
    if (mSizeEnd < mCapacityEnd) {
        T* oldEnd = mSizeEnd;
        ++mSizeEnd;
        mCurrent = oldEnd;
        return oldEnd;
    } else {
        if (++mCurrent == mCapacityEnd) mCurrent = mCapacityBegin;
        // call destructor of removed element
        mCurrent->~T();
        return mCurrent;
    }
}

template<typename T, size_t Capacity>
template<typename... Args>
void RingBuffer<T, Capacity>::EmplaceBack(const Args& ... args) {
    T* elem = pushOne();
    new (elem) T(args...);
}
