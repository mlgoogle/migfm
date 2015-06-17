#define DEBUG_SUBSYSTEM S_TEST

//#ifdef __cplusplus

#include "debugnew.h"
#include "basic/basictypes.h"
#include "log/mig_log.h"
#include "rbtree.h"

struct mem_chunk
{
	struct rb_node node;
	char *ptr;
	size_t size;
	const char *file;
	int line;
};


class DebugMemCheck
{
public:
	static int Startup(void) {return __startup;}
	
	DebugMemCheck(void);
	~DebugMemCheck(void);
	
	void Malloc(struct mem_chunk *);
	struct mem_chunk * Free(void *ptr);
private:
	struct mem_chunk *FindChunkNolocked(void *ptr);
	pthread_mutex_t _lock;
	struct rb_root _root;
static int __startup;
};

DebugMemCheck::DebugMemCheck(void)
{
	pthread_mutex_init(&_lock, 0);
	_root.rb_node = NULL;
	__startup = 1;
}

DebugMemCheck::~DebugMemCheck(void)
{
	struct rb_node *node;
	struct mem_chunk *chunk;
	__startup = 0;
	for(node = rb_first(&_root);node;)
	{
		chunk = rb_entry(node, struct mem_chunk, node);
		//printf("memory leaks checked:size[%d] %s:%d line: %p\n", chunk->size,
			//chunk->file, chunk->line, chunk->ptr);

		MIG_DEBUG(SYSTEM_LEVEL,"memory leaks checked:size[%d] %s:%d line: %p\n", chunk->size,
				chunk->file, chunk->line, chunk->ptr);
		node = rb_next(node);
		rb_erase(&chunk->node, &_root);
		
		free((void *)chunk);//, chunk->size+sizeof(struct mem_chunk));
	}

	pthread_mutex_destroy(&_lock);

}

void
DebugMemCheck::Malloc(struct mem_chunk *c)
{
	struct rb_node **link, *parent = NULL;
	struct mem_chunk *pos;

	pthread_mutex_lock(&_lock);
	link = &this->_root.rb_node;
	while(*link)
	{
		parent = *link;
		pos = rb_entry(parent, struct mem_chunk, node);
		assert(pos->ptr!=c->ptr);
		if(((uint64)pos->ptr)>((uint64)c->ptr))
		{
			link = &((*link)->rb_left);
		}
		else
		{
			link = &((*link)->rb_right);
		}
	}

	rb_link_node(&c->node, parent, link);
	rb_insert_color(&c->node, &_root);
	pthread_mutex_unlock(&_lock);
}


struct mem_chunk *
DebugMemCheck::FindChunkNolocked(void *ptr)
{
	struct rb_node *node = this->_root.rb_node;
	struct mem_chunk *pos;

	while(node)
	{
		pos = rb_entry(node, struct mem_chunk, node);
		if(((uint64)pos->ptr)>(uint64)ptr) //编译的系统为linux64位，其指针类型和long型大小相等（8B）而与int型4B，故会出现：loses precision。故：先将char*-->long,然后从long到int自动隐式转换。

			node = node->rb_left;
		else if(((uint64)pos->ptr)<(uint64)ptr)
			node = node->rb_right;
		else
		{
			return pos;
		}
	}
	return NULL;
}

struct mem_chunk *
DebugMemCheck::Free(void *ptr)
{
	pthread_mutex_lock(&_lock);
	struct mem_chunk *chunk = FindChunkNolocked(ptr);
	if(chunk)
	{
		rb_erase(&chunk->node, &_root);
	}
	pthread_mutex_unlock(&_lock);
	return chunk;
}

int DebugMemCheck::__startup = 0;
static DebugMemCheck ___DebugMemCheck;


void *operator new (size_t size)
{
	if(!DebugMemCheck::Startup())
		return malloc(size);
	char *ptr;
//	PE_ALLOC(ptr, size+sizeof(struct mem_chunk), char *);
	ptr = (char *)malloc(size+sizeof(struct mem_chunk));

	MIG_DEBUG(SYSTEM_LEVEL, "malloced : %d at %p.\n",size+sizeof(struct mem_chunk), ptr);
	if(ptr)
	{
		struct mem_chunk *c = (struct mem_chunk *)ptr;
		c->ptr = ptr+sizeof(struct mem_chunk);
		c->size = size;
		c->file = NULL;
		c->line = 0;
		___DebugMemCheck.Malloc(c);
		return c->ptr;
	}
	else
	{
		return NULL;
	}
}

void * operator new (size_t size, const char *file, int line)
{
	if(!DebugMemCheck::Startup())
		return malloc(size);
	char *ptr;
//	PINFO("%s %d\n", file, line);
//	PE_ALLOC(ptr, size+sizeof(struct mem_chunk), char *);
	ptr = (char *)malloc(size+sizeof(struct mem_chunk));
	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:malloced : %d at %p.\n",
		file, line, size+sizeof(struct mem_chunk), ptr);
	if(ptr)
	{
		struct mem_chunk *c = (struct mem_chunk *)ptr;
//		INIT_HLIST_NODE(&c->hash);
		c->ptr = ptr+sizeof(struct mem_chunk);
		c->size = size;
		c->file = file;
		c->line = line;
		___DebugMemCheck.Malloc(c);
		return c->ptr;
	}
	else
	{
		return NULL;
	}
}


void operator delete (void *p, const char *file, int line)
{

	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:delete : at %p.\n",
			file, line,p);

	if(p==NULL)
		return;
	if(!DebugMemCheck::Startup())
	{
		free(p);
		return;
	}
	struct mem_chunk *c = ___DebugMemCheck.Free(p);
	if(c)
	{
//		PE_FREE((void *)c, c->size+sizeof(struct mem_chunk));
		free(c);
	}
	else
	{
		//PDEBUG(D_MALLOC, "%s:%d line: delete(%p) failed.\n", file, line, p);
		MIG_DEBUG(SYSTEM_LEVEL, "%s:%d line: delete(%p) failed.\n", file, line, p);
	}
}

void *operator new [] (size_t size, const char *file, int line)
{
	if(!DebugMemCheck::Startup())
		return malloc(size);
	char *ptr;
//	PE_ALLOC(ptr, size+sizeof(struct mem_chunk), char *);
	ptr = (char *)malloc(size+sizeof(struct mem_chunk));
	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:malloced : %d at %p.\n",
			file, line, size+sizeof(struct mem_chunk), ptr);
	if(ptr)
	{
		struct mem_chunk *c = (struct mem_chunk *)ptr;
		c->ptr = ptr+sizeof(struct mem_chunk);
		c->size = size;
		c->file = file;
		c->line = line;
		___DebugMemCheck.Malloc(c);
		return c->ptr;
	}
	else
		return NULL;
}

void operator delete [] (void *p, const char *file, int line)
{
	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:delete : at %p.\n",
				file, line,p);
	if(!DebugMemCheck::Startup())
	{
		free(p);
		return;
	}
	struct mem_chunk *c = ___DebugMemCheck.Free(p);
	if(c)
	{
//		PE_FREE((void *)c, c->size+sizeof(struct mem_chunk));
		free(c);
	}
	else
	{
		//PDEBUG(D_MALLOC, "%s:%d line: delete(%p) failed.\n", file, line, p);
		MIG_DEBUG(SYSTEM_LEVEL,"%s:%d line: delete(%p) failed.\n", file, line, p);
	}
}

void operator delete (void *p)
{
	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:delete : at %p.\n",
				__FILE__, __LINE__,p);
	if(p==NULL)
		return;
	if(!DebugMemCheck::Startup())
	{
		free(p);
		return;
	}
	struct mem_chunk *c = ___DebugMemCheck.Free(p);
	if(c)
	{
//		PE_FREE((void *)c, c->size+sizeof(struct mem_chunk));
		free(c);
	}
	else
	{
		//PDEBUG(D_MALLOC, "delete(%p) failed.\n", p);
		MIG_DEBUG(SYSTEM_LEVEL,"delete(%p) failed.\n", p);
	}
}

void operator delete [] (void *p)
{
	MIG_DEBUG(SYSTEM_LEVEL, "<%s>:%d line:delete : at %p.\n",
					__FILE__, __LINE__,p);
	if(p==NULL)
		return;
	if(!DebugMemCheck::Startup())
	{
		free(p);
		return;
	}
	struct mem_chunk *c = ___DebugMemCheck.Free(p);
	if(c)
	{
//		PE_FREE((void *)c, c->size+sizeof(struct mem_chunk));
		free(c);
	}
	else
	{
		//PERROR("delete(%p) failed.\n", p);
		MIG_ERROR(SYSTEM_LEVEL,"delete(%p) failed.\n", p);

	}
}

//#endif
