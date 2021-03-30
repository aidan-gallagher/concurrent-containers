# Queue Overview

The queue can be accessed by multiple producers and consumers from multiple threads. When consumers get data from the queue it is removed the queue.

# Usage

# Construction

```C++
Concurrent::Queue<std::string> queue;
```

# Insertion

```C++
queue.push("Hello");
queue.push("World");
```

# Retrieval

Data returned from the queue will be removed from the queue.

If the queue is empty, `tryGet()` will return (a `std::optional`) immediately.

```C++
auto val = queue.tryGet();
if (val){
    std::cout << "My data is " << *val;
}
else{
    std::cout << "The queue was empty";
}
```

If the queue is empty, `waitGet()` will wait until a producer puts something in the queue

```C++
auto val = queue.waitGet(); // Execution will wait here if the queue is empty
std::cout << "My data is " << val;
```

# Rationale

## tryGet() return type

This function needs to return 2 bits of information

- Whether it was successful
- Potentially the popped data

### Iterator: `Container::const_iterator`

**Option**: The underlying container could be changed to `std::deque` to allow iterator access.  
**Reason**: The concurrentQueue should not return an iterator to the underlying container because then a user could break the encapsulation and bypass the thread safety to access queue elements.

### Unique Pointer `std::unique_ptr<T>`

**Option**: The function could return a `nullptr` if the queue was empty and return a pointer to the data if it was possible.  
**Reason**: This would result in memory allocation on the heap every time data is accessed.

### Pair `std::pair<T,bool>`

**Option**: If data was available the function could return the data, and `true` to signify the data is good.
Otherwise the return type would be default constructed and returned with a `false` to signify the data is bad.
The bool should be the 2nd param to be consistent with the [STL](https://en.cppreference.com/w/cpp/container/map/insert) and for [performance reasons](https://stackoverflow.com/questions/56761591/how-do-i-organize-members-in-a-struct-to-waste-the-least-space-on-alignment).  
**Reason**: This means only default constructable objects can be stored in the queue.

### Out Parameter

**Option**: Similar to the pair method except the data is returned via an out parameter. This puts the responsibility on the calling code to construct the object.  
**Reason**: Results in extra code on the client side

### Throw an exception

**Option**: An exception could be thrown when trying to access an empty queue.  
**Reason**: An empty queue isn't exceptional. Exceptions shouldn't be used for control flow.

### Optional `std::optional<T>`

**Option**: Return a `std::optional`.  
**Reason**: Calling code doesn't have to construct any out parameter objects.
Clean syntax to check if data is valid.

## Mutex & Conditional Variable type

`std::shared_mutex` allows for multiple readers (non-modifying) access to the queue at the same time. However that means the less efficient `std::condition_variable_any` has to be used (rather than `std::condition_variable`).  
Under most circumstances there is no value in querying the state of the queue since there is no guarantee that the state will not be changed by another thread before you can make an action based off of the state. Therefore it is not worth making performance sacrifices for shared reading of the queue.
