#include "../include/Alloc.h"
namespace littlestl{
    Alloc::Obj* Alloc::free_list[NumFreeLists::k_num_free_list_]={
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
    char* Alloc::start_free_=0;
    char* Alloc::end_free_=0;
    size_t Alloc::heap_size_=0;

    void* Alloc::Refill(size_t bytes){
        size_t num_objs=NumObjs::k_num_objs_;
        char* chunk=ChunkAlloc(bytes,num_objs);
        Obj** my_free_list=0;
        Obj* result=0;
        Obj* current_obj=0,*next_obj=0;
        if(num_objs==1){
            return chunk;
        }
        else{
            my_free_list=free_list+FreeListIndex(bytes);
            result=(Obj*)(chunk);
            *my_free_list=next_obj=(Obj*)(chunk+bytes);
            for(int i=1;;i++){
                current_obj=next_obj;
                next_obj=(Obj*)((char*)next_obj+bytes);
                if(num_objs-1==i){
                    current_obj->next=0;
                    break;
                }
                else{
                    current_obj->next=next_obj;
                }
            }
            return result;
        }


    }
    char* Alloc::ChunkAlloc(size_t bytes, size_t& num_objs){
        char* result=0;
        size_t total_bytes=bytes*num_objs;
        size_t bytes_left=end_free_-start_free_;
        if(bytes_left>=total_bytes){
            result=start_free_;
            start_free_=start_free_+total_bytes;
            return result;
        }
        else if(bytes_left>=bytes){
            num_objs=bytes_left/bytes;
            total_bytes=bytes*num_objs;
            result=start_free_;
            start_free_=start_free_+total_bytes;
            return result;
        }
        else{
            size_t bytes_to_get=2*total_bytes+RoundUp(heap_size_>>4);
            if(bytes_left>0){
                Obj** my_free_list=0,*p=0;
                ((Obj*)start_free_)->next=*my_free_list;
                *my_free_list=(Obj*)start_free_;
                
            }
            start_free_=(char*)malloc(bytes_to_get);
            if(!start_free_){
                Obj** my_free_list=0,*p=0;
                for(int i=0;i<=MaxBytes::k_max_bytes_;i+=Align::k_align_){
                    my_free_list=free_list+FreeListIndex(i);
                    p=*my_free_list;
                    if(p!=0){
                        *my_free_list=p->next;
                        start_free_=(char*)p;
                        end_free_=start_free_+i;
                        return ChunkAlloc(bytes,num_objs);
                    }
                }
                end_free_=0;
            }
            heap_size_+=bytes_to_get;
            end_free_=start_free_+bytes_to_get;
            return ChunkAlloc(bytes,num_objs);
        }

    }
    void* Alloc::Allocate(size_t bytes){
        if(bytes>MaxBytes::k_max_bytes_){
            return malloc(bytes);
        }
        size_t index=FreeListIndex(bytes);
        Obj* list=free_list[index];
        if(list){
            free_list[index]=list->next;
            return list;
        }
        else{
            return Refill(RoundUp(bytes));
        }


    }
    void Alloc::Deallocate(void* ptr,size_t bytes){
        if(bytes>MaxBytes::k_max_bytes_){
            free(ptr);
        }
        else{
            size_t index=FreeListIndex(bytes);
            Obj* node=static_cast<Obj*>(ptr);
            node->next=free_list[index];
            free_list[index]=node;
        }

    }
    void* Alloc::ReAllocate(void* ptr,size_t old_size,size_t now_size ){
        Deallocate(ptr,old_size);
        ptr=Allocate(now_size);
        return ptr;
    }
}