cmake_minimum_required(VERSION 2.8)

enable_testing()

file(GLOB LIB_SOURCES littlerisc/*.c littlerisc/*.h littlerisc/include/*.h)
add_library(liblittlerisc ${LIB_SOURCES})

add_executable(register_test test/register_test.c)
target_link_libraries(register_test liblittlerisc)
add_test(register register_test)

add_executable(mmu_test test/mmu_test.c)
target_link_libraries(mmu_test liblittlerisc)
add_test(mmu mmu_test)

add_executable(rv32i_test test/rv32i_test.c)
target_link_libraries(rv32i_test liblittlerisc)
add_test(rv32i rv32i_test)

add_executable(csr_test test/csr_test.c)
target_link_libraries(csr_test liblittlerisc)
add_test(csr csr_test)

project(riscv_emu)
include_directories(libriscv/include)
add_executable(${PROJECT_NAME} "main.c")
target_link_libraries(riscv_emu liblittlerisc)
