#ifndef BASE_RING_H
#define BASE_RING_H

#define BASE_RING_ENTRY(elem)						\
    struct {								        \
	struct elem * volatile next;					\
	struct elem * volatile prev;					\
    }

#define BASE_RING_HEAD(head, elem)					\
    struct head {							        \
	struct elem * volatile next;					\
	struct elem * volatile prev;					\
    }

#define BASE_OFFSET(p_type,field) \
    ((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#define BASE_OFFSETOF(s_type,field)     BASE_OFFSET(s_type*,field)

#define BASE_RING_SENTINEL(hp, elem, link)				\
    (struct elem *)((char *)(&(hp)->next) - BASE_OFFSETOF(struct elem, link))

#define BASE_RING_FIRST(hp)	(hp)->next

#define BASE_RING_LAST(hp)	(hp)->prev

#define BASE_RING_NEXT(ep, link)	(ep)->link.next

#define BASE_RING_PREV(ep, link)	(ep)->link.prev

#define BASE_RING_INIT(hp, elem, link) do {				\
	BASE_RING_FIRST((hp)) = BASE_RING_SENTINEL((hp), elem, link);	\
	BASE_RING_LAST((hp))  = BASE_RING_SENTINEL((hp), elem, link);	\
    } while (0)

#define BASE_RING_EMPTY(hp, elem, link)					\
    (BASE_RING_FIRST((hp)) == BASE_RING_SENTINEL((hp), elem, link))

#define BASE_RING_ELEM_INIT(ep, link) do {				\
	BASE_RING_NEXT((ep), link) = (ep);				\
	BASE_RING_PREV((ep), link) = (ep);				\
    } while (0)

#define BASE_RING_SPLICE_BEFORE(lep, ep1, epN, link) do {		\
	BASE_RING_NEXT((epN), link) = (lep);				\
	BASE_RING_PREV((ep1), link) = BASE_RING_PREV((lep), link);	\
	BASE_RING_NEXT(BASE_RING_PREV((lep), link), link) = (ep1);	\
	BASE_RING_PREV((lep), link) = (epN);				\
    } while (0)

#define BASE_RING_SPLICE_AFTER(lep, ep1, epN, link) do {			\
	BASE_RING_PREV((ep1), link) = (lep);				\
	BASE_RING_NEXT((epN), link) = BASE_RING_NEXT((lep), link);	\
	BASE_RING_PREV(BASE_RING_NEXT((lep), link), link) = (epN);	\
	BASE_RING_NEXT((lep), link) = (ep1);				\
    } while (0)

#define BASE_RING_INSERT_BEFORE(lep, nep, link)				\
	BASE_RING_SPLICE_BEFORE((lep), (nep), (nep), link)

#define BASE_RING_INSERT_AFTER(lep, nep, link)				\
	BASE_RING_SPLICE_AFTER((lep), (nep), (nep), link)

#define BASE_RING_SPLICE_HEAD(hp, ep1, epN, elem, link)			\
	BASE_RING_SPLICE_AFTER(BASE_RING_SENTINEL((hp), elem, link),	\
			     (ep1), (epN), link)

#define BASE_RING_SPLICE_TAIL(hp, ep1, epN, elem, link)			\
	BASE_RING_SPLICE_BEFORE(BASE_RING_SENTINEL((hp), elem, link),	\
			     (ep1), (epN), link)

#define BASE_RING_INSERT_HEAD(hp, nep, elem, link)			\
	BASE_RING_SPLICE_HEAD((hp), (nep), (nep), elem, link)

#define BASE_RING_INSERT_TAIL(hp, nep, elem, link)			\
	BASE_RING_SPLICE_TAIL((hp), (nep), (nep), elem, link)

#define BASE_RING_CONCAT(h1, h2, elem, link) do {			\
	    if (!BASE_RING_EMPTY((h2), elem, link)) {			\
	        BASE_RING_SPLICE_BEFORE(BASE_RING_SENTINEL((h1), elem, link),	\
				      BASE_RING_FIRST((h2)),			\
				      BASE_RING_LAST((h2)), link);		\
	        BASE_RING_INIT((h2), elem, link);				\
	    }								\
    } while (0)

#define BASE_RING_PREPEND(h1, h2, elem, link) do {			\
	    if (!BASE_RING_EMPTY((h2), elem, link)) {			\
	        BASE_RING_SPLICE_AFTER(BASE_RING_SENTINEL((h1), elem, link),	\
				      BASE_RING_FIRST((h2)),			\
				      BASE_RING_LAST((h2)), link);		\
	        BASE_RING_INIT((h2), elem, link);				\
	    }								\
    } while (0)

#define BASE_RING_UNSPLICE(ep1, epN, link) do {				\
	BASE_RING_NEXT(BASE_RING_PREV((ep1), link), link) =		\
		     BASE_RING_NEXT((epN), link);			\
	BASE_RING_PREV(BASE_RING_NEXT((epN), link), link) =		\
		     BASE_RING_PREV((ep1), link);			\
    } while (0)

#define BASE_RING_REMOVE(ep, link)					\
    BASE_RING_UNSPLICE((ep), (ep), link)

#define BASE_RING_FOREACH(ep, head, elem, link)                          \
    for (ep = BASE_RING_FIRST(head);                                     \
         ep != BASE_RING_SENTINEL(head, elem, link);                     \
         ep = BASE_RING_NEXT(ep, link))

#define BASE_RING_FOREACH_SAFE(ep1, ep2, head, elem, link)               \
    for (ep1 = BASE_RING_FIRST(head), ep2 = BASE_RING_NEXT(ep1, link);    \
         ep1 != BASE_RING_SENTINEL(head, elem, link);                    \
         ep1 = ep2, ep2 = BASE_RING_NEXT(ep1, link))

#endif /* !BASE_RING_H */
