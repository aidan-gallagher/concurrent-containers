#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace Concurrent
{

template<typename T, class Container = std::deque<T>>
class Queue
{
public:
  Queue() = default;
  Queue(const Queue& other);
  Queue<T>& operator=(const Queue& other);
  ~Queue() = default;

  void             push(const T& object);
  void             push(T&& object);
  T                waitGet();
  std::optional<T> tryGet();
  bool             isEmpty() const;

private:
  mutable std::mutex       mMutex;
  std::queue<T, Container> mQueue;
  std::condition_variable  mConVar;
};

template<typename T, class Container>
Queue<T, Container>::Queue(const Queue& other)
{
  std::scoped_lock scopedLock(other.mMutex);
  mQueue = other.mQueue;
}

template<typename T, class Container>
Queue<T>&
Queue<T, Container>::operator=(const Queue& other)
{
  // Check for self assignment
  if (this != &other)
  {
    // Lock both mutexes at the same time
    std::unique_lock unique_lock(mMutex, std::defer_lock);
    std::unique_lock other_unique_lock(other.mMutex, std::defer_lock);
    std::lock(unique_lock, other_unique_lock);

    mQueue = other.mQueue;
  }
  return *this;
}

template<typename T, class Container>
void
Queue<T, Container>::push(const T& object)
{
  std::unique_lock uniqueLock(mMutex);
  const bool       was_empty = mQueue.empty();

  mQueue.push(object);

  // If the queue was empty there may be some consumers waiting for data.
  if (was_empty)
  {
    // Unlock mutex before notifying so waiting threads do not have to wait for the mutex after being woken.
    uniqueLock.unlock();
    mConVar.notify_one();
  }
}

template<typename T, class Container>
void
Queue<T, Container>::push(T&& object)
{
  std::unique_lock uniqueLock(mMutex);
  const bool       was_empty = mQueue.empty();

  mQueue.push(std::move(object));

  // If the queue was empty there may be some consumers waiting for data.
  if (was_empty)
  {
    // Unlock mutex before notifying so waiting threads do not have to wait for the mutex after being woken.
    uniqueLock.unlock();
    mConVar.notify_one();
  }
}

template<typename T, class Container>
T
Queue<T, Container>::waitGet()
{
  std::unique_lock uniqueLock(mMutex);
  mConVar.wait(uniqueLock, [this] { return !mQueue.empty(); });
  T object = std::move(mQueue.front());
  mQueue.pop();
  return object;
}

template<typename T, class Container>
std::optional<T>
Queue<T, Container>::tryGet()
{
  std::scoped_lock scopedLock(mMutex);
  if (!mQueue.empty())
  {
    T object = std::move(mQueue.front());
    mQueue.pop();
    return object;
  }
  else
  {
    return std::nullopt;
  }
}

template<typename T, class Container>
bool
Queue<T, Container>::isEmpty() const
{
  std::scoped_lock scopedLock(mMutex);
  return mQueue.empty();
}

} // End namespace Concurrent
#endif // End header guard