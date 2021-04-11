#ifndef CONCURRENT_SEARCHRINGBUFFER_H
#define CONCURRENT_SEARCHRINGBUFFER_H

#include <algorithm>
#include <array>
#include <chrono>
#include <mutex>
#include <shared_mutex>

namespace Concurrent
{

template<typename T, std::size_t SIZE>
class SearchRingBuffer
{
  using time_point = std::chrono::system_clock::time_point;

public:
  SearchRingBuffer();
  SearchRingBuffer(const SearchRingBuffer&) = delete;            // Disable copying of the class //TODO
  SearchRingBuffer& operator=(const SearchRingBuffer&) = delete; // Disable assignment of the class //TODO

  T    read(const time_point requestedTime) const;
  void push(const time_point time, const T& item);
  bool isEmpty() const;

  // Exceptions
  class BufferEmpty : public std::runtime_error
  {
  public:
    BufferEmpty() : runtime_error("Error trying to read data from empty circular buffer") {}
  };

  class ItemTooOld : public std::runtime_error
  {
  public:
    ItemTooOld() : runtime_error("Error trying to insert old data into the buffer") {}
  };

private:
  using iterator = typename std::array<std::pair<time_point, T>, SIZE>::iterator;
  using const_iterator = typename std::array<std::pair<time_point, T>, SIZE>::const_iterator;

  mutable std::shared_mutex                  mMutex;  // Mutex to control multithreaded access
  std::array<std::pair<time_point, T>, SIZE> mBuffer; // Underlying buffer
  iterator                                   mNewest; // Iterator to the last element inserted
  iterator                                   mOldest; // Iterator to the oldest element inserted
  bool                                       mFull;   // Flag set to true once size == capacity
  bool                                       mEmpty;  // Flag set to false once first element is inserted

  // Increment the iterator by one item except if iterator points to the last element
  // then wrap round back to the first element
  iterator nextIter(const iterator& iter);

  // Split the array into two arrays using mNewest as the splitting point
  // Determine which array the value is in and binary search for it.
  T findItem(const time_point requestedTime) const;
};

template<typename T, std::size_t SIZE>
SearchRingBuffer<T, SIZE>::SearchRingBuffer() :
    mFull(false),
    mEmpty(true),
    mNewest(std::prev(mBuffer.end())), // Initialised to last place in array so initial nextIter() puts its to first place
    mOldest(mBuffer.begin())
{
}

template<typename T, std::size_t SIZE>
void
SearchRingBuffer<T, SIZE>::push(const time_point time, const T& item)
{
  std::scoped_lock<std::shared_mutex> aScopedLock(mMutex);

  if (!mEmpty && time < mNewest->first) // If the inserted time is less than latest time then cancel the insertion
  {
    throw ItemTooOld{};
  }
  else
  {
    mNewest = nextIter(mNewest);
    *mNewest = std::make_pair(time, item); // Write the data
    mEmpty = false;
  }

  if (!mFull) // Check if the buffer is now full
  {
    mFull = (mNewest == std::prev(mBuffer.end()));
  }
  else
  {
    mOldest = nextIter(mOldest); // Move the mOldest iter to the next place
  }
}

template<typename T, std::size_t SIZE>
T
SearchRingBuffer<T, SIZE>::read(const time_point requestedTime) const
{
  std::shared_lock<std::shared_mutex> aSharedLock(mMutex);

  if (mEmpty) // Check the buffer is not empty
  {
    throw BufferEmpty{};
  }
  else if (requestedTime < mOldest->first) // Check if the requested data is too old
  {
    return mOldest->second;
  }
  else if (requestedTime > mNewest->first) // Check if the requested data is too new
  {
    return mNewest->second;
  }
  else // We know the requested data is within the limits of the buffer
  {
    return findItem(requestedTime);
  }
}

template<typename T, std::size_t SIZE>
bool
SearchRingBuffer<T, SIZE>::isEmpty() const
{
  std::shared_lock<std::shared_mutex> aSharedLock(mMutex);
  return mEmpty;
}

template<typename T, std::size_t SIZE>
typename SearchRingBuffer<T, SIZE>::iterator
SearchRingBuffer<T, SIZE>::nextIter(const iterator& iter)
{
  if (iter == std::prev(mBuffer.end())) // If iterator is currently pointing to last element
  {
    return mBuffer.begin(); // Then wrap around and point to the first
  }
  else
  {
    return std::next(iter); // Othewise progress to the next element
  }
}

// mBuffer is sorted but the start point is not the lowest and end point is not the highest.
// This means we have two sorted arrays. For example:
//  <----arr1---><-------arr2--->
//  | 8 | 9 | 10 | 4 | 5 | 6 | 7 |
// Use mNewest to find where to split. (mNewest is included in the first array)
//   Everything equal to and below mNewest is sorted.
//   Everything greater than mNewest is sorted.
// Find which array the value is in
//   If value is greater than or equal to arr[0] then in first half
//   If value is less than or equal to arr[END] then in second half
// Perform normal binary search on whichever array the value is in
template<typename T, std::size_t SIZE>
T
SearchRingBuffer<T, SIZE>::findItem(const time_point requestedTime) const
{
  const_iterator start_arr;
  const_iterator end_arr;

  // Find which array the value is in
  // Set the start and end point for the array which the value is in
  if (requestedTime >= mBuffer.begin()->first)
  {
    // The start to the newest (inclusive)
    start_arr = mBuffer.begin();
    end_arr = mNewest;
  }
  else if (requestedTime <= std::prev(mBuffer.end())->first)
  {
    // One past the newest to the end
    start_arr = std::next(mNewest);
    end_arr = std::prev(mBuffer.end());
  }
  else //  last element > requestedTime > first element - so no binary search needed
  {
    // find which one is closer
    auto above_diff = mBuffer.begin()->first - requestedTime;
    auto below_diff = requestedTime - std::prev(mBuffer.end())->first;
    return (below_diff < above_diff) ? std::prev(mBuffer.end())->second : mBuffer.begin()->second;
  }

  // Binary search using std::lower_bound to find the first element which does not compare less than requestedTime
  // Use std::lower_bound and use a binary predicate comparison which will return true is the first element is less than the second
  // Comparison lambda will use the requestedTime (pair.first) to compare
  T    item; // Fake object needed to construct target object.
  auto above_val =
    std::lower_bound(start_arr, end_arr, std::make_pair(requestedTime, item), [](std::pair<time_point, T> lhs, std::pair<time_point, T> rhs) -> bool {
      return lhs.first < rhs.first;
    });

  if (above_val->first == requestedTime)
  {
    return above_val->second;
  }
  else // If not an exact match then find the closest
  {
    // Execution will not get here if the requested item
    //	* is less than the first element the function
    //  * exactly the first element
    // So safe to do std::prev without it going out of bounds
    auto below_val = std::prev(above_val);
    auto above_diff = above_val->first - requestedTime;
    auto below_diff = requestedTime - below_val->first;
    return (below_diff < above_diff) ? below_val->second : above_val->second;
  }
}

} // End namespace Concurrent
#endif // End header guard