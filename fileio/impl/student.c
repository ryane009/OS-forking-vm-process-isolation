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
#define CACHE_SIZE 4096
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
    
    ret->valid_bytes = pread(ret->fd, ret->cache + ret->offset, CACHE_SIZE - (ret->offset % CACHE_SIZE), ret->offset);
    ret->stats.read_calls++;
    
    check_invariants(ret);
    dbg(ret, "Just finished initializing file from path: %s\n", path);
    return ret;
}

int io300_seek(struct io300_file *const f, off_t const pos) {
    //return lseek(f->fd, pos, SEEK_SET);
    //my implementation
    //return lseek(f->fd, pos, SEEK_SET);
    check_invariants(f);
    if(pos > f->size || pos < 0){
        return -1;
    }
    f->stats.seeks++;
    f->offset = pos;
    return pos;
}

int io300_close(struct io300_file *const f) {
    check_invariants(f);

#if(DEBUG_STATISTICS == 1)
    printf("stats: {desc: %s, read_calls: %d, write_calls: %d, seeks: %d}\n",
            f->description, f->stats.read_calls, f->stats.write_calls, f->stats.seeks);
#endif
    // TODO: Implement this
    if(f->changed == 1){
        io300_flush(f);
    }
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
    unsigned char c;

    check_invariants(f);
    
    if(f->offset == f->size){
        return -1;
    }
    if(f->offset >= f->min + CACHE_SIZE || f->offset < f->min){
        f->min = f->offset - (f->offset % CACHE_SIZE);
        if(f->size - f->min < CACHE_SIZE){
            pread(f->fd, f->cache, f->size - f->min, f->min);
            f->min += CACHE_SIZE;
            f->valid_bytes = f->size - f->min;
            c = f->cache[0];
        }
        else{
            pread(f->fd, f->cache, CACHE_SIZE, f->min);
            c = f->cache[f->offset % CACHE_SIZE];
        }
    }
    else{  
        c = f->cache[f->offset % CACHE_SIZE];
        // CHARACTER IS ONLY LENGTH 1, SO YOU ONLY WANT TO ASSIGN TO INDEX 0 
    }
    // else{
    //     pread(f->fd, f->cache, CACHE_SIZE, f->offset);
    //     f->valid_bytes = CACHE_SIZE;
    //     c = f->cache[0];
    //     f->min += CACHE_SIZE;
    // }
    f->stats.read_calls++;
    f->offset++;
    f->changed = 0;
    return (unsigned char) c;
}

int io300_writec(struct io300_file *f, int ch) {
    char const c = (char)ch;

    if(f->min + CACHE_SIZE > f->offset){
        f->cache[f->offset % CACHE_SIZE] = c;
        f->valid_bytes = f->offset - f->min;
        f->changed = 1;
    }
    else{
        if(f->changed == 1){
            f->valid_bytes = CACHE_SIZE;
            io300_flush(f);
            f->changed = 0;
        }
        f->cache[f->offset % CACHE_SIZE] = c;
        f->min = f->offset;
        f->changed = 1;
    }
    f->stats.write_calls++;
    //f->valid_bytes++;
    f->offset++;
    return (unsigned char) c;
}

    

ssize_t io300_read(struct io300_file *const f, char *const buff, size_t const sz) {

    //end of file exception
    if(f->offset == f->size){
        return 0;
    }

    if(f->offset > f->size){
        return -1;
    }
    int size = (int) sz;
    if (f->offset + (int)sz > f->size)
    {
        size = f->size - f->offset;
    }

    //when offset is changed by seek, load new cache
    if(f->offset + size > f->min + CACHE_SIZE || f->offset < f->min){
        lseek(f->fd, f->offset, SEEK_SET);
        read(f->fd, buff, size);
        f->offset += size;
        f->min = f->offset - (f->offset % CACHE_SIZE);
        lseek(f->fd, f->min, SEEK_SET);
        f->valid_bytes = read(f->fd, f->cache, CACHE_SIZE);
    }
    else {
        memcpy(buff, f->cache+ f->offset % CACHE_SIZE, size);
        f->offset+=size;
    }
    f->stats.read_calls++;
    return size;
}

ssize_t io300_write(struct io300_file *const f, const char *buff, size_t const sz) {
    //return write(f->fd, buff, sz);
    int size = (int) sz;

    if(f->min + CACHE_SIZE > f->offset + size){
        memcpy(f->cache + (f->offset - f->min), buff, size);
        f->valid_bytes += size;
        f->offset += size;
    }
    else{
        if(f->changed == 1){
            io300_flush(f);
            f->changed = 0;
        }
        write(f->fd, buff, size);
        f->offset += size;
        f->min = f->offset;
    }
    f->changed = 1;
    f->stats.write_calls++;
    return size;
}

int io300_flush(struct io300_file *const f) {
    check_invariants(f);
    lseek(f->fd, f->min, SEEK_SET);
    write(f->fd, f->cache, f->valid_bytes);
    f->valid_bytes = 0;
    return 0;
}