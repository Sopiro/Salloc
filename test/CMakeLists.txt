add_executable(unit_test
    doctest.h
    test.cpp
)

set_target_properties(unit_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(unit_test PUBLIC ../include/salloc)
target_link_libraries(unit_test PUBLIC salloc)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    doctest.h
    test.cpp
)