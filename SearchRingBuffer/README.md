# SearchRingBuffer Overview

The SearchRingBuffer is a ring buffer (also known as a circular buffer) which can be accessed by multiple producers and consumers from multiple threads. Each data item has an associated time stamp.  
Producers push new data which has to have a later time stamp than the previously pushed data. If the buffer is already full then the oldest data will be overwritten.  
Consumers request data from a given time. The data with the closest time stamp to the requested time stamp is returned.

# Usage

## Construction

```C++
Concurrent::SearchRingBuffer<std::string, 10> circBuff;
```

## Insertion

Inserted data has to have an associated time stamp.

```C++
circBuff.push(std::chrono::system_clock::now(),"Hello World")
```

The buffer expects subsequent insertions to have an equal or later time stamp.
An attempt to insert data with an older timestamp than the latest element will result in the exception `Concurrent::SearchRingBuffer<...>::ItemTooOld` being thrown.

## Retrieval

A binary search is used to find the closest timestamp. Therefore this function has the complexity O(log(SIZE)) where SIZE is a constant defined at compile time.

The data with the closest time stamp will be retrieved.

```C++
std::string output = circBuff.read(RequestedTime);
```

An attempt to read from an empty queue will result in the exception `Concurrent::SearchRingBuffer<...>::BufferEmpty` being thrown.

# Limitations

- The size of the buffer must be known at compile time
- It is assumed the circular buffer will always be sorted by time. Time_point must be used as the key.
- The stored types must have a default constructor

# Notes

## Duplicate time stamps

When two elements with the same time are in the buffer and that time is requested the earliest data will be retrieved.

## Very large buffer

A very large buffer may result in a stack overflow. The buffer can be created on the heap to avoid this problem
