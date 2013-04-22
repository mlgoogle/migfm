#ifndef _BUFFERED_QUEUE_H_
#define _BUFFERED_QUEUE_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <queue>
#include "base\logging.h"

////////////////////////////////////////////////////////////////////////////////
namespace util {

template < class _Ty, class _Container=std::deque<_Ty> >
class BufferedQueue
{
public:
	typedef _Ty ValueType;
	typedef _Container ContainerType;

	class Queue : public std::queue<_Ty, _Container>
	{
	public:
		void Swap(Queue &right) {
			c.swap(right.c);
		}
	};

	BufferedQueue(void) {
	}

	~BufferedQueue(void) {
	}

	bool IsWorkQueueEmpty() const {
		return work_queue_.empty();
	}

	void Push(const ValueType& val) {
		AutoLock lock(buffered_queue_lock_);
		buffered_queue_.push(val);
	}

	ValueType Pop() {
		ValueType val = work_queue_.front();
		work_queue_.pop();
		return val;
	}

	bool Reload() {
		if (!work_queue_.empty())
			return true;

		{
			AutoLock lock(buffered_queue_lock_);
			if (buffered_queue_.empty())
				return false;
			buffered_queue_.Swap(work_queue_);
			DCHECK(buffered_queue_.empty());
		}
		return !work_queue_.empty();
	}

private:
	Queue work_queue_;
	Queue buffered_queue_;
	Lock buffered_queue_lock_;
};

} // namespace util

#endif // _BUFFERED_QUEUE_H_
