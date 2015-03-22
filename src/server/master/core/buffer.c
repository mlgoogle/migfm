#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define NDEBUG
#include<assert.h>
#include"buffer.h"

static const char hex_chars[] = "0123456789abcdef";
struct buffer* buffer_init(void)
{

    struct buffer  *buf;
    
    buf = (struct buffer*)malloc(sizeof(*buf));
    assert(buf);
   
    buf->ptr = NULL;
    buf->size = 0;
    buf->used = 0;

    return buf;
}

int buffer_is_equal(struct buffer *a,struct buffer *b)
{

    if(a->used!=b->used) return 0;
    if(a->used==0) return 1;

    return  (0==strcmp(a->ptr,b->ptr));
}

void  buffer_reset(struct buffer* b){

    if(!b) return;
   
    if(b->size>BUFFER_MAX_REUSE_SIZE){
        
        free(b->ptr);
        b->ptr = NULL;
        b->size = 0;
    } else if(b->size){
       
        b->ptr[0]= '\0';
    }
    b->used = 0;
}

struct buffer *buffer_init_buffer(struct buffer *src)
{

    struct buffer *b = buffer_init();
    buffer_copy_string_buffer(b,src);
    return b;
}

struct buffer *buffer_init_string(const char *str)
{

    struct buffer *b = buffer_init();
    buffer_copy_string(b,str);
    return b;
}


int buffer_copy_string_buffer(struct buffer *b,const struct buffer *src)
{

    if(!src) return -1;
  
    if(src->used == 0){
        buffer_reset(b);
        return 0;
    }
   
    return buffer_copy_string_len(b,src->ptr,src->used-1);
}

int buffer_copy_string(struct buffer *b,const char* s)
{

    size_t s_len;
    if(!s||!b) return -1;

    s_len = strlen(s)+1;
    buffer_prepare_copy(b,s_len);
    memcpy(b->ptr,s,s_len);
    b->used = s_len;
    return 1;
}

int buffer_copy_string_len(struct buffer *b,const char *src, size_t s_len){

    if(!src||!b) return -1;

    buffer_prepare_copy(b,s_len);
   
    memcpy(b->ptr,src,s_len);
    b->ptr[s_len] = '\0';
    b->used = s_len+1;

    return 0;
}


int buffer_prepare_copy(struct buffer *b,size_t size){

    if(!b) return -1;
    if((0==b->size)||
        (size>b->size)){
    
        if(b->size) {free(b->ptr);b->ptr=NULL;}
   
        b->size = size;
    
        b->size += BUFFER_PIECE_SIZE -(b->size%BUFFER_PIECE_SIZE);

        b->ptr = (char*)malloc(b->size);
        assert(b->ptr);
    }
   
    b->used = 0;

    return 0;
}

void buffer_free(struct buffer *b)
{

    if(!b) return;
    if(b->ptr) {free(b->ptr);b->ptr=NULL;}
    if(b) {free(b);b=NULL;}
}

int  LI_ltostr(char *buf,long val)
{
    char swap;
    char *end;
    int len = 1;

    if (val < 0) {
        len++;
        *(buf++) = '-';
        val = -val;
    }

    end = buf;
    while (val > 9) {
    *(end++) = '0' + (val % 10);
    val = val / 10;
    }
    
    *(end) = '0' + val;
    *(end + 1) = '\0';
    len += end - buf;

    while (buf < end) {
        swap = *end;
        *end = *buf;
        *buf = swap;
   
        buf++;
        end--;
    }

    return len;
}

int buffer_append_long(struct buffer *b,long val)
{
    if (!b) return -1;
    
    buffer_prepare_append(b,32);
    
    if(b->used == 0)
        b->used++;
   
    b->used += LI_ltostr(b->ptr+(b->used - 1),val);
    return 0;
}

int buffer_copy_long(struct buffer *b, long val)
{

    if(!b) return -1;

    b->used = 0;
    return buffer_append_long(b,val);
}


int buffer_append_memory(struct buffer *b,const char *s, size_t s_len)
{

    if(!s||!b) return -1;
    if(s_len == 0) return 0;

    buffer_prepare_append(b,s_len);
    memcpy(b->ptr+b->used,s,s_len);
    b->used += s_len;
    return 0;
}

int buffer_copy_memory(struct buffer *b,const char *s,size_t s_len)
{
    if(!s||!b) return -1;
    b->used = 0;
    return buffer_append_memory(b,s,s_len);
}

int buffer_append_string(struct buffer *b,const char *s)
{

    size_t s_len;
   
    if(!s||!b) return -1;

    s_len = strlen(s);
    buffer_prepare_append(b,s_len+1);

    if(b->used==0)
        b->used++;
  
    memcpy(b->ptr+b->used-1,s,s_len+1);
    b->used += s_len;
    
    return 0;
}


int buffer_append_string_len(struct buffer *b,const char *s,size_t s_len)
{

    if(!s||!b) return -1;
    if(s_len == 0) return 0;
   
    buffer_prepare_append(b,s_len+1);
   
    if(b->used == 0)
        b->used++;
  
    //判断s_len
    size_t c_len = s_len>(b->size - b->used)?(b->size - b->used):s_len;
    memcpy(b->ptr+b->used-1,s,c_len);
    b->used+=c_len;
    b->ptr[b->used -1] = '\0';
    
    return 0;
}

int buffer_append_string_buffer(struct buffer *b,const struct buffer *src)
{

    if(!src) return -1;
    if(src->used == 0) return 0;
   
    return buffer_append_string_len(b,src->ptr,src->used - 1);
}


int buffer_append_long_hex(struct buffer *b,unsigned long value)
{
    char *buf;
    int shift = 0;
    unsigned long copy = value;

    while (copy) {
        copy >>= 4;
        shift++;
    }
    if (shift == 0)
        shift++;
    if (shift & 0x01)
        shift++;

    buffer_prepare_append(b, shift + 1);
    if (b->used == 0)
        b->used++;
    buf = b->ptr + (b->used - 1);
    b->used += shift;

    shift <<= 2;
    
    while (shift > 0) {
        shift -= 4;
        *(buf++) = hex_chars[(value >> shift) & 0x0F];
    }
    *buf = '\0';

    return 0;
}

int buffer_prepare_append(struct buffer *b, size_t size) {
    if (!b) return -1;

    if (0 == b->size) {
        b->size = size;

        /* always allocate a multiply of BUFFER_PIECE_SIZE */
        b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);

        b->ptr = (char*)malloc(b->size);
        b->used = 0;
        assert(b->ptr);
    } else if (b->used + size > b->size) {
        b->size += size;

	/* always allocate a multiply of BUFFER_PIECE_SIZE */
        b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);
        b->ptr = (char*)realloc(b->ptr, b->size);
        assert(b->ptr);
    }
    return 0;
}

