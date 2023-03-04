#define DMALLOC_DISABLE 1
#include "dmalloc.hh"
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <iterator>


struct dmalloc_stats g_stats = {0,0,0,0,0,0,SIZE_MAX,0};

std::unordered_map<void*, bool> active_map;

/**
 * dmalloc(sz,file,line)
 *      malloc() wrapper. Dynamically allocate the requested amount `sz` of memory and 
 *      return a pointer to it 
 * 
 * @arg size_t sz : the amount of memory requested 
 * @arg const char *file : a string containing the filename from which dmalloc was called 
 * @arg long line : the line number from which dmalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dmalloc(size_t sz, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.

    void* ptr = base_malloc(sizeof(meta_data) + sz + 1000);
    if(long(ptr) < (long(ptr) - long(sz))){
        g_stats.nfail++;
        g_stats.fail_size += sz;
        return nullptr;
    }
    if(ptr == nullptr){
        g_stats.fail_size += sz;
        g_stats.nfail++;
        return nullptr;
    }

    
    if(((uintptr_t) ((char*) (ptr) + sz)) > g_stats.heap_max){
        g_stats.heap_max = (uintptr_t) ((char*) (ptr) + sz + sizeof(meta_data));
    }

    if((uintptr_t) ptr <= g_stats.heap_min){
        g_stats.heap_min = (uintptr_t) ptr;
    }

    meta_data* m_ptr = (meta_data*)ptr;
    char* secret = (char*) (m_ptr + 1) + sz;
    *secret = 'a';

    m_ptr->data_sz = sz;
    m_ptr->file = file;
    m_ptr->line = line;
    active_map[m_ptr + 1] = true;
    g_stats.nactive++;
    g_stats.ntotal++;
    g_stats.total_size += (int) sz;
    g_stats.active_size += (int) sz;
    return (m_ptr + 1);
}

/**
 * dfree(ptr, file, line)
 *      free() wrapper. Release the block of heap memory pointed to by `ptr`. This should 
 *      be a pointer that was previously allocated on the heap. If `ptr` is a nullptr do nothing. 
 * 
 * @arg void *ptr : a pointer to the heap 
 * @arg const char *file : a string containing the filename from which dfree was called 
 * @arg long line : the line number from which dfree was called 
 */
void dfree(void* ptr, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings

    if(ptr == nullptr){
        return;
    }    

    if((uintptr_t)((char*)ptr) < g_stats.heap_max && (uintptr_t)((char*)ptr) >= g_stats.heap_min){
        if(active_map.count(ptr) == 1){
            if(active_map[ptr] == true){
                meta_data* m_ptr = (meta_data*)((char*)ptr);
                m_ptr--;
                if(*((char*)(ptr) + m_ptr->data_sz) != 'a'){
                    fprintf(stderr, "MEMORY BUG: detected wild write during free of pointer %p\n",(char*)ptr);
                    abort();
                }
                g_stats.active_size -= m_ptr->data_sz;
                g_stats.nactive--;
                active_map[ptr] = false;
                base_free(ptr); 
            }
            else{
                fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, double free\n", (char*) file, (int)line, (char*)ptr);
                abort();
            }
        }
        else{
            fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, not allocated\n", (char*) file, (int)line, (char*)ptr);
            meta_data* m_ptr = (meta_data*)((char*)ptr);
            m_ptr--;
            // if((char*)(m_ptr + 1) + m_ptr->data_sz > ptr && (char*)(m_ptr + 1) < ptr){
                //     fprintf(stderr, "%s:%d: is %d inside a %d byte region allocated here\n",(char*)m_ptr->file, (int) line, (int)((char*)ptr - ((char*)m_ptr + sizeof(meta_data))), (int) m_ptr->data_sz);
                //     abort();
                // }
            abort();
        }
    }
    else{
        fprintf(stderr, "MEMORY BUG: %s:%d: invalid free of pointer %p, not in heap\n", (char*) file, (int)line, (char*)ptr);
        abort();
    }



    // if(m_ptr->status == EMPTY){
    //     fprintf(stderr, "MEMORY BUG: %s:%s invalid free of pointer %p, not allocated/n", (char*) file, (char*)line, ptr);
    //     abort();
    //     return;
    // }

    // else if(m_ptr == nullptr){
    //     fprintf(stderr, "MEMORY BUG: %s:%s invalid free of pointer %p, not in heap/n", (char*) file, (char*)line, ptr);
    //     abort();
    //     return;
    // }
}

/**
 * dcalloc(nmemb, sz, file, line)
 *      calloc() wrapper. Dynamically allocate enough memory to store an array of `nmemb` 
 *      number of elements with wach element being `sz` bytes. The memory should be initialized 
 *      to zero  
 * 
 * @arg size_t nmemb : the number of items that space is requested for
 * @arg size_t sz : the size in bytes of the items that space is requested for
 * @arg const char *file : a string containing the filename from which dcalloc was called 
 * @arg long line : the line number from which dcalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dcalloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test014).
    if (nmemb > SIZE_MAX / sz) {
        g_stats.nfail++;
        g_stats.fail_size += sz;
        return nullptr;
    }
    void* ptr = dmalloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}

/**
 * get_statistics(stats)
 *      fill a dmalloc_stats pointer with the current memory statistics  
 * 
 * @arg dmalloc_stats *stats : a pointer to the the dmalloc_stats struct we want to fill
 */
void get_statistics(dmalloc_stats* stats) {
    // Stub: set all statistics to enormous numbers
    memset(stats, 255, sizeof(*stats));
    // Your code here.
    stats->nactive = g_stats.nactive;
    stats->active_size = g_stats.active_size;
    stats->ntotal = g_stats.ntotal;
    stats->total_size = g_stats.total_size;    
    stats->nfail = g_stats.nfail;       
    stats->fail_size = g_stats.fail_size;
    stats->heap_min = g_stats.heap_min;
    stats->heap_max = g_stats.heap_max;       
}

/**
 * print_statistics()
 *      print the current memory statistics to stdout       
 */
void print_statistics() {
    dmalloc_stats stats;
    get_statistics(&stats);

    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}

/**  
 * print_leak_report()
 *      Print a report of all currently-active allocated blocks of dynamic
 *      memory.
 */
void print_leak_report() {
    for (const auto kv : active_map) {
        if (kv.second) {
            void* ptr = kv.first;
            meta_data* m_ptr = (meta_data*)((char*)ptr);
            m_ptr--;
            printf("LEAK CHECK: %s:%d: allocated object %p with size %d\n", m_ptr->file, m_ptr->line, ptr, (int)m_ptr->data_sz);
        }
    }   

}
