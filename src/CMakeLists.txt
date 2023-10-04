set(HEADER_FILES
    ../include/allocator/stack_allocator.h
    ../include/allocator/fixed_block_allocator.h
    ../include/allocator/predefined_block_allocator.h
    ../include/allocator/block_allocator.h
    ../include/allocator/allocator.h
)
set(SOURCE_FILES
    stack_allocator.cpp
    predefined_block_allocator.cpp
    block_allocator.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "src" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include" PREFIX "include" FILES ${HEADER_FILES})

add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)