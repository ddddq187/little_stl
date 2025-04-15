#pragma once
#include <cstdlib>
namespace littlestl{
    // allocate momery
    class Alloc
    {
    private:
    // enum is implicitly constexpr
        enum Align{k_align_=8};
        enum MaxBytes{k_max_bytes_=128};
        enum NumFreeLists{k_num_free_list_=(MaxBytes::k_max_bytes_/Align::k_align_)};
        enum NumObjs{k_num_objs_=20};
    private:
        union Obj
        {
            union Obj* next;
            char client[1];
        };
        static Obj* free_list[NumFreeLists::k_num_free_list_];
    private:
        static char* start_free_;
        static char* end_free_;
        static size_t heap_size_;
    private:
    //bytes up round 8n
        static size_t RoundUp(size_t bytes){
            return ((bytes+Align::k_align_-1) & ~(Align::k_align_-1));//low 3bit is 000
        }
        static size_t FreeListIndex(size_t bytes){
            return ((bytes+Align::k_align_-1)/Align::k_align_-1);//1~8：0  9~16：1
        }
        static void* Refill(size_t bytes);
        static char *ChunkAlloc(size_t bytes, size_t& num_objs);
    public:
        static void* Allocate(size_t bytes);
        static void Deallocate(void* ptr,size_t bytes);
        static void* ReAllocate(void* ptr,size_t old_size,size_t now_size );
        
    };
    
   
    
}//namespace littlestl
   
