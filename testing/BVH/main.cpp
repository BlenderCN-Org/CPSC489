#include<iostream>
#include<functional>
#include<memory>

#include "vector.h"
#include "BVH.h"

template<class T>
using unique_ptr_SSE = typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, std::unique_ptr<T, std::function<void(typename std::remove_extent<T>::type*)>>>::type;

template<class T>
inline unique_ptr_SSE<T> make_unique_SSE(size_t n)
{
 typedef typename std::remove_extent<T>::type E;
 auto deleter = [](E* ptr) { if(ptr) _aligned_free(ptr); };
 return std::unique_ptr<E[], std::function<void(E*)>>(static_cast<E*>(_aligned_malloc(n, 64)), deleter);
}

int main()
{
 auto vb = make_unique_SSE<vector4D<float>[]>(12);

 // create index buffer
 auto ib = std::make_unique<unsigned short[]>(12);
 for(int i = 0; i < 12; i++) ib[i] = i;

 return 0;
}