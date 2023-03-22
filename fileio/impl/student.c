#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "../io300.h"

/*
    student.c
    Fill in the following stencils
*/

/*
    When starting, you might want to change this for testing on small files.
*/
#ifndef CACHE_SIZE
#define CACHE_SIZE 8
#endif

#if(CACHE_SIZE < 4)
#error "internal cache size should not be below 4."
#error "if you changed this during testing, that is fine."
#error "when handing in, make sure it is reset to the provided value"
#error "if this is not done, the autograder will not run"
#endif

/*
   This macro enables/disables the dbg() function. Use it to silence your
   debugging info.
   Use the dbg() function instead of printf debugging if you don't want to
   hunt down 30 printfs when you want to hand in
*/
#define DEBUG_PRINT 0
#define DEBUG_STATISTICS 1

struct io300_file {
    /* read,write,seek all take a file descriptor as a parameter */
    int fd;
    /* this will serve as our cache */
    char *cache;
    int size;


    // TODO: Your properties go here
    int offset;
    int min;
    int cache_alloc;
    int end_file;
    int changed;
    int valid_bytes;

    /* Used for debugging, keep track of which io300_file is which */
    char *description;
    /* To tell if we are getting the performance we are expecting */
    struct io300_statistics {
        int read_calls;
        int write_calls;
        int seeks;
    } stats;
};


/*
    Assert the properties that you would like your file to have at all times.
    Call this function frequently (like at the beginning of each function) to
    catch logical errors early on in development.
*/
static void check_invariants(struct io300_file *f) {
    assert(f != NULL);
    assert(f->cache != NULL);
    assert(f->fd >= 0);
    assert(f->min >= 0);
    assert(f->offset >= 0);
    // TODO: Add more invariants
}

/*
    Wrapper around printf that provides information about the
    given file. You can silence this function with the DEBUG_PRINT macro.
*/
static void dbg(struct io300_file *f, char *fmt, ...) {
    (void)f; (void)fmt;
#if(DEBUG_PRINT == 1)
    static char buff[300];
    size_t const size = sizeof(buff);
    int n = snprintf(
        buff,
        size,
        // TODO: Add the fields you want to print when debugging
        f->cache,
        "{desc:%s, } -- ",
        f->description
    );
    int const bytes_left = size - n;
    va_list args;
    va_start(args, fmt);
    vsnprintf(&buff[n], bytes_left, fmt, args);
    va_end(args);
    printf("%s", buff);
#endif
}

struct io300_file *io300_open(const char *const path, char *description) {
    if (path == NULL) {
        fprintf(stderr, "error: null file path\n");
        return NULL;
    }
    int const fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "error: could not open file: `%s`: %s\n", path, strerror(errno));
        return NULL;
    }
    struct io300_file *const ret = malloc(sizeof(*ret));
    if (ret == NULL) {
        fprintf(stderr, "error: could not allocate io300_file\n");
        return NULL;
    }
    ret->fd = fd;
    ret->cache = malloc(CACHE_SIZE);
    if (ret->cache == NULL) {
        fprintf(stderr, "error: could not allocate file cache\n");
        close(ret->fd);
        free(ret);
        return NULL;
    }
    ret->description = description;
    // TODO: Initialize your file
    ret->min = 0;
    ret->offset = 0;
    ret->end_file = 0;
    ret->changed = 0;
    ret->stats.read_calls = 0;
    ret->stats.write_calls = 0;
    ret->stats.seeks = 0;

    //loading size into metadat so onyl have to use filesize once
    ret->size = (int) io300_filesize(ret);
    if(ret->size < CACHE_SIZE){
        read(ret->fd, ret->cache + ret->offset, ret->size);
        ret->valid_bytes = ret->size;
        ret->stats.read_calls++;
    }
    else{
        read(ret->fd, ret->cache + ret->offset, CACHE_SIZE);
        ret->valid_bytes = CACHE_SIZE;
        ret->stats.read_calls++;
    }
    check_invariants(ret);
    dbg(ret, "Just finished initializing file from path: %s\n", path);
    return ret;
}

int io300_seek(struct io300_file *const f, off_t const pos) {
    return lseek(f->fd, pos, SEEK_SET);
    //my implementation
    // check_invariants(f);
    f->stats.seeks++;
    f->offset = pos;
    return pos;
    // // TODO: Implement this
    // return lseek(f->fd, pos, SEEK_SET);
}

int io300_close(struct io300_file *const f) {
    check_invariants(f);

#if(DEBUG_STATISTICS == 1)
    printf("stats: {desc: %s, read_calls: %d, write_calls: %d, seeks: %d}\n",
            f->description, f->stats.read_calls, f->stats.write_calls, f->stats.seeks);
#endif
    // TODO: Implement this
    io300_flush(f);
    close(f->fd);
    free(f->cache);
    free(f);
    return 0;
}

off_t io300_filesize(struct io300_file *const f) {
    check_invariants(f);
    struct stat s;
    int const r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}

int io300_readc(struct io300_file *const f) {
    check_invariants(f);
    unsigned char c;
    if(f->offset + 1 > f->size){
        return -1;
    }
    //(f->offset % CACHE_SIZE) + 1 <= CACHE_SIZE && f->offset!= CACHE_SIZE
    //CACHE_SIZE - (CACHE_SIZE - (f->offset % 8)) > 0
    if(f->min + CACHE_SIZE > f->offset){
        c = f->cache[f->offset % CACHE_SIZE];
        // CHARACTER IS ONLY LENGTH 1, SO YOU ONLY WANT TO ASSIGN TO INDEX 0 
        f->offset++;
    }
    else if(f->size - f->offset < CACHE_SIZE){
        read(f->fd, f->cache, f->size - f->offset);
        f->valid_bytes = f->size - f->offset;
        f->stats.read_calls++;
        c = f->cache[0];
        f->offset++;
        f->min += CACHE_SIZE;
    }
    else{
        read(f->fd, f->cache, CACHE_SIZE);
        f->stats.read_calls++;
        f->valid_bytes = CACHE_SIZE;
        c = f->cache[0];
        f->offset++;
        f->min += CACHE_SIZE;
    }
    return (unsigned char) c;

}
int io300_writec(struct io300_file *f, int ch) {
    char const c = (char)ch;
    // return write(f->fd, &c, 1) == 1 ? ch : -1;

    if(f->min + CACHE_SIZE > f->offset){
        f->cache[f->offset % CACHE_SIZE] = c;
        // CHARACTER IS ONLY LENGTH 1, SO YOU ONLY WANT TO ASSIGN TO INDEX 0 
        f->offset++;
        f->valid_bytes = f->offset % CACHE_SIZE;
    }
    else{
        if(f->changed == 1){
            f->valid_bytes = CACHE_SIZE;
            io300_flush(f);
            f->changed = 0;
        }
        f->cache[f->offset % CACHE_SIZE] = c;
        f->offset++;
        f->min += CACHE_SIZE;
        // if(f->size - f->offset < CACHE_SIZE){
        //     read(f->fd, f->cache, f->size - f->offset);
        //     f->valid_bytes = f->size - f->offset;
        //     f->cache[0] = c;
        //     f->offset++;
        //     f->min += CACHE_SIZE;
            
        // }
        // else{
        //     read(f->fd, f->cache, CACHE_SIZE);
        //     f->cache[0] = c;
        //     f->valid_bytes = CACHE_SIZE;
        //     f->offset++;
        //     f->min += CACHE_SIZE;
        // }
    }
    f->changed = 1;
    f->stats.write_calls++;
    return (unsigned char) c;
}

ssize_t io300_read(struct io300_file *const f, char *const buff, size_t const sz) {
    return read(f->fd, buff, sz);
    //new IMPLEMENTATION
    // if(f->offset + (int) sz > f->size){
    //     return -1;
    // }

    // //if the number of bytes we want to read fits within the remaining pre-read cache slots
    // //f->min + CACHE_SIZE > f->offset
    // if(f->min + CACHE_SIZE > f->offset){
    //     memcpy(buff, f->cache + (f->offset % CACHE_SIZE), sz);
    //     // CHARACTER IS ONLY LENGTH 1, SO YOU ONLY WANT TO ASSIGN TO INDEX 0 
    //     // *buff = f->cache[f->offset];
    //     f->offset += sz;
    // }
    // //if we'll need to rewrite our cache at least once to read data
    // else{
    //     char* str[sz];
    //     memcpy(buff, f->fd, sz);
    // }
}
        
    //     int num_bytes = sz;
    //     while(num_bytes > 0){
    //                     //TODO: If we're at end of reading bytes
    //         if(f->min + CACHE_SIZE > f->offset + num_bytes){
    //             //if we have enough bytes left in the file to fill up the cache completely
    //             if(f->size > f->offset + num_bytes && f->size - f->min >= CACHE_SIZE){
    //                 //pre-reading for subsequent io300 read calls
    //                 read(f->fd, f->cache, CACHE_SIZE);
    //                 f->offset += num_bytes;
    //                 f->min += CACHE_SIZE;
    //             }
    //             //if we've reached the end of our file and there
    //             //aren't enough bytes to fill up the cache 
    //             //(don't have to worry about sz going over file size as we've accounted for that earlier)
    //             //f->size can still have more bytes than remaining bytes to read (eg there are 5 bytes left in the file
    //             //and we can read up to 8 bytes into the cache but we only need to read 3, we want to read 5)
    //             //however f->size can also be equal to the number of remaining bytes to read, but either way we want to load
    //             //into the cache the number of bytes left in the file
    //             else{
    //                 //we want to read as much as we can, but we don't have enough bytes left in the file
    //                 //to fill the cache
    //                 read(f->fd, f->cache, f->size - f->min);
    //                 f->end_file = 1;
    //             }
    //             memcpy(buff + f->offset, f->cache, (size_t)num_bytes);
    //             f->offset += num_bytes;
    //             num_bytes = 0;
    //         }

    //         //this occurs when we already have preloaded data (as the cache will always be filled to the max), and we'll want
    //         //to go to the end of it
    //         if((f->offset % CACHE_SIZE) != 0){
    //             //read(f->fd, f->cache + (f->offset % CACHE_SIZE), (size_t)CACHE_SIZE - (f->offset % CACHE_SIZE));
    //             memcpy(buff + f->offset, f->cache + (f->offset % CACHE_SIZE), CACHE_SIZE - (f->offset % CACHE_SIZE));
    //             num_bytes -= (CACHE_SIZE - (f->offset % CACHE_SIZE));
    //             f->offset += CACHE_SIZE - (f->offset % CACHE_SIZE);
    //             f->min += CACHE_SIZE;
    //         }

    //         //we want to fill up the cache completely with new values as a previous call reached the end
    //         if(num_bytes >= CACHE_SIZE && (f->offset % CACHE_SIZE) == 0){
    //             read(f->fd, f->cache, CACHE_SIZE);
    //             memcpy(buff + f->offset, f->cache, CACHE_SIZE);
    //             num_bytes -= CACHE_SIZE;
    //             f->offset += CACHE_SIZE;
    //             f->min += CACHE_SIZE;
    //         }
    //     }
    // }

ssize_t io300_write(struct io300_file *const f, const char *buff, size_t const sz) {
    return write(f->fd, buff, sz);
    //my implementation
    // check_invariants(f);
    // // TODO: Implement this
    // int size = (int) sz;
    // while(size > 0){
    //     if(f->offset == f->min + CACHE_SIZE){
    //         io300_flush(f);
    //         f->min += CACHE_SIZE;
    //     }

    //     if(f->offset + size <= CACHE_SIZE - f->offset % 8){
    //         int remaining = size;
    //         for(int i = f->offset; i < remaining; i++){
    //             f->cache[f->offset % 8] = buff[size];
    //             size--;
    //         }
    //         f->offset += remaining;
    //     }
    //     else{
    //         for(int i = f->offset; i < f->min + CACHE_SIZE; i++){
    //             f->cache[f->offset % CACHE_SIZE + i] = buff[sz - size];
    //             f->offset++;
    //             size--;
    //         }
    //     }
    // }
    // f->stats.write_calls++;
    // return sz;
}

int io300_flush(struct io300_file *const f) {
    // (void)f;
    // return 0;

    check_invariants(f);
    lseek(f->fd, f->min, SEEK_SET);
    write(f->fd, f->cache, f->valid_bytes);
    return 0;

    //my implentation
    // check_invariants(f);
    // //maybe not fd
    // write(f->fd, f->cache, CACHE_SIZE);
    // for(int i = 0; i < CACHE_SIZE; i++){
    //     f->cache[i] = '\0';
    // }
    // // TODO: Implement this
    // return 0;
}