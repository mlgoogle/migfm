#ifndef _BUFFER_H__
#define _BUFFER_H__
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>

#define BUFFER_MAX_REUSE_SIZE (4*1024)
#define BUFFER_PIECE_SIZE 64


#define CONST_STR_LEN(x) x,x ? sizeof(x)-1 : 0
struct buffer{

    char   *ptr;
   
    size_t  used;
  
    size_t  size;
};

struct buffer_array{

   struct buffer   **ptr;

   size_t   used;

   size_t   size;
};


struct read_buffer{

    char    *ptr;
    
    size_t  offset;
  
    size_t  used;
  
    size_t  size;
};

struct buffer *buffer_init(void);
struct buffer *buffer_init_buffer(struct buffer *buf);
struct buffer *buffer_init_string(const char *str);

void buffer_free(struct buffer *buf);
void buffer_reset(struct buffer *buf);

int buffer_prepare_copy(struct buffer *b,size_t size);
int buffer_prepare_append(struct buffer *b,size_t size);

int buffer_copy_string(struct buffer* b,const char* s);
int buffer_copy_string_len(struct buffer *b,const char *s,size_t s_len);
int buffer_copy_string_buffer(struct buffer *b,const struct buffer *src);
int buffer_copy_long(struct buffer *b,long val);
int buffer_copy_memory(struct buffer *b,const char *s,size_t s_len);

int buffer_append_string(struct buffer *b,const char *s);
int buffer_append_string_len(struct buffer *b,const char *s,size_t s_len);
int buffer_append_string_buffer(struct buffer *b,const struct buffer *src);


int buffer_append_long_hex(struct buffer *b,unsigned long len);
int buffer_append_long(struct buffer *b,long val);

int buffer_append_memory(struct buffer *b,const char *s,size_t s_len);
int LI_ltostr(char *buf,long val);

int buffer_is_equal(struct buffer *a,struct buffer *b);
#endif
