add_library (ConcurrentQueue INTERFACE)

target_include_directories (ConcurrentQueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(ConcurrentQueue INTERFACE Threads::Threads)

add_library(Concurrent::Queue ALIAS ConcurrentQueue)

add_subdirectory (Tests)