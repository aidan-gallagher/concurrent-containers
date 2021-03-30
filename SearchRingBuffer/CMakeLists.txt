project (SearchRingBuffer)

add_library (SearchRingBuffer INTERFACE)

target_include_directories (SearchRingBuffer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(SearchRingBuffer INTERFACE Threads::Threads)

add_library(Concurrent::SearchRingBuffer ALIAS SearchRingBuffer)

add_subdirectory (Tests)
