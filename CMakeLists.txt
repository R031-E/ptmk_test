cmake_minimum_required(VERSION 3.12)
project(ptmk_test)

set(CMAKE_CXX_STANDARD 20)
set(PostgreSQL_ADDITIONAL_VERSIONS "17")

add_executable(ptmk_test main.cpp
        employee.cpp
        employee.h
)

# Find and link libpqxx
set(LIBPQXX_INCLUDE_DIR "C:/Program Files/libpqxx/include")
set(LIBPQXX_LIBRARY_DIR "C:/Program Files/libpqxx/lib")

# Find and link PostgreSQL C library (libpq)
set(PostgreSQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/17/include")
set(PostgreSQL_LIBRARY_DIR "C:/Program Files/PostgreSQL/17/lib")

# Find and link Windows SDK
set(WIN_SDK_LIB_DIR "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/um/x64")

if(NOT EXISTS "${LIBPQXX_INCLUDE_DIR}")
    message(FATAL_ERROR "libpqxx include directory not found: ${LIBPQXX_INCLUDE_DIR}")
endif()

if(NOT EXISTS "${LIBPQXX_LIBRARY_DIR}")
    message(FATAL_ERROR "libpqxx library directory not found: ${LIBPQXX_LIBRARY_DIR}")
endif()

if(NOT EXISTS "${PostgreSQL_INCLUDE_DIR}")
    message(FATAL_ERROR "PostgreSQL include directory not found: ${PostgreSQL_INCLUDE_DIR}")
endif()

if(NOT EXISTS "${PostgreSQL_LIBRARY_DIR}")
    message(FATAL_ERROR "PostgreSQL library directory not found: ${PostgreSQL_LIBRARY_DIR}")
endif()

if(NOT EXISTS "${WIN_SDK_LIB_DIR}")
    message(FATAL_ERROR "Windows SDK library directory not found: ${WIN_SDK_LIB_DIR}")
endif()

target_include_directories(ptmk_test PRIVATE "${LIBPQXX_INCLUDE_DIR}" "${PostgreSQL_INCLUDE_DIR}")
target_link_directories(ptmk_test PRIVATE "${LIBPQXX_LIBRARY_DIR}" "${PostgreSQL_LIBRARY_DIR}" "${WIN_SDK_LIB_DIR}")
target_link_libraries(ptmk_test PRIVATE pqxx libpq ws2_32 secur32 wldap32)