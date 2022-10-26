cmake_minimum_required(VERSION 3.10)
project(celonis)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++14")

include_directories(./include)
link_directories(./lib)
include_directories(/user/include)
link_directories(/user/lib/x86_64-linux-gnu)

find_library(PTHREAD pthread)
find_package(Boost)

if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS:\"${Boost_INCLUDE_DIRS}\", ENABLE_BOOST is opened")
    message(STATUS "Boost_LIBRARIES:\"${Boost_LIBRARIES}\"")
    message(STATUS "Boost_VERSION:\"${Boost_VERSION}\"")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

set(LIB_SRC
    src/kv.cpp
    src/kv_service.cpp
    src/kv_primary.cpp
    )

add_library(celonis SHARED ${LIB_SRC})
target_link_libraries(celonis mysqlcppconn)

set(LIBS 
    pthread
    mysqlcppconn
    celonis
    rpc
    eva01
)

add_executable(main main.cpp)
add_dependencies(main celonis)
target_link_libraries(main ${LIBS})

add_executable(secondary secondary.cpp)
add_dependencies(secondary celonis)
target_link_libraries(secondary ${LIBS})

add_executable(primary primary.cpp)
add_dependencies(primary celonis)
target_link_libraries(primary ${LIBS})

add_executable(celonis_kv celonis_kv.cpp)
add_dependencies(celonis_kv celonis_kv)
target_link_libraries(celonis_kv ${LIBS})

add_custom_target(run
    COMMAND main)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

