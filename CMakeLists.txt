cmake_minimum_required(VERSION 2.8)

enable_testing()

set(CMAKE_C_FLAGS_DEBUG "-g -Og")
set(CMAKE_C_FLAGS_RELEASE "-O3")

file(GLOB LIB_SOURCES littlerisc/*.c littlerisc/*.h littlerisc/include/*.h)
add_library(littlerisc ${LIB_SOURCES})

add_executable(register_test test/register_test.c)
target_link_libraries(register_test littlerisc)
add_test(register register_test)

add_executable(mmu_test test/mmu_test.c)
target_link_libraries(mmu_test littlerisc)
add_test(mmu mmu_test)

add_executable(rv32i_test test/rv32i_test.c)
target_link_libraries(rv32i_test littlerisc)
add_test(rv32i rv32i_test)

add_executable(csr_test test/csr_test.c)
target_link_libraries(csr_test littlerisc)
add_test(csr csr_test)

project(riscv_emu)
include_directories(libriscv/include)
add_executable(${PROJECT_NAME} "main.c")
target_link_libraries(riscv_emu littlerisc)
