set(HEADER_FILES
    ../include/salloc/stack_allocator.h
    ../include/salloc/linear_allocator.h
    ../include/salloc/fixed_block_allocator.h
    ../include/salloc/predefined_block_allocator.h
    ../include/salloc/block_allocator.h
    ../include/salloc/allocator.h
)
set(SOURCE_FILES
    linear_allocator.cpp
    predefined_block_allocator.cpp
    block_allocator.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "src" FILES ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include" PREFIX "include" FILES ${HEADER_FILES})

add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CMAKE_COMPILE_WARNING_AS_ERROR ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
