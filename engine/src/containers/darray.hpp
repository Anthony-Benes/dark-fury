#pragma once

#include <defines.hpp>
#include <core/df_memory.hpp>
#include <core/logger.hpp>

namespace Engine {

template<class T>
class DF_API DArray {
  public:
    static const u64 DefaultCapacity = 1;
    static const u64 ResizeFactor    = 2;

    DArray(u64 capacity = DefaultCapacity) : mSize(0), mCapacity(capacity) {
        mData = (T*)Memory::df_allocate(mCapacity * sizeof(T), Memory::Tag::DARRAY);
    }

    ~DArray() { clear(); }

    void push(T value) {
        if ( mSize >= mCapacity ) { resize(); }
        mData[mSize++] = value;
    }

    T pop() { return mData[--mSize]; }

    void insert_at(u64 index, T value) {
        if ( index > mSize ) {
            Log::Error("Index outside the bounds of this array! Size: %i, index: %i", mSize, index);
            return;
        }
        if ( mSize >= mCapacity ) { resize(); }
        u64 addr = (u64)mData;
        if ( index != mSize - 1 ) {
            Memory::df_copy_memory((void*)(addr + ((index + 1) * sizeof(T))),
                                   (void*)(addr + (index * sizeof(T))),
                                   sizeof(T) * (mSize - index));
        }
        Memory::df_copy_memory((void*)(addr + (index * sizeof(T))), &value, sizeof(T));
        ++mSize;
    }

    T pop_at(u64 index) {
        if ( index >= mSize ) {
            Log::Error("Index outside the bounds of this array! Size: %i, index: %i", mSize, index);
            index = mSize - 1;
        }
        T dest = mData[index];
        if ( index != mSize - 1 ) {
            u64 addr = (u64)mData;
            Memory::df_copy_memory((void*)(addr + (index * sizeof(T))),
                                   (void*)(addr + ((index + 1) * sizeof(T))),
                                   sizeof(T) * (mSize - index));
        }
        --mSize;
        return dest;
    }

    void clear() {
        if ( mData ) {
            Memory::df_free(mData, mSize * sizeof(T), Memory::Tag::DARRAY);
            mData = nullptr;
        }
        mSize     = 0;
        mCapacity = 0;
    }

    b8 isEmpty() { return mSize == 0; }

    u64 size() { return mSize; }

    u64 capacity() { return mCapacity; }

    T* data() { return mData; }

  private:
    u64 mSize;
    u64 mCapacity;
    T* mData;

    void resize() {
        u64 newCapacity = mCapacity * ResizeFactor;
        T* newData      = (T*)Memory::df_allocate(newCapacity * sizeof(T), Memory::Tag::DARRAY);
        Memory::df_copy_memory(newData, mData, mSize * sizeof(T));
        Memory::df_free(mData, mCapacity * sizeof(T), Memory::Tag::DARRAY);
        mData     = newData;
        mCapacity = newCapacity;
    }

  public:
    T& operator[](u64 index) { return mData[index]; }

    const T& operator[](u64 index) const { return mData[index]; }
};

}  // namespace Engine
