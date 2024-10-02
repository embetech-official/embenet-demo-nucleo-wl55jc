cmake_minimum_required(VERSION 3.21)

add_library(
  embenet_node_port_stm32wle5 STATIC
  embenet_aes128.c
  embenet_brt.c
  embenet_capabilities.c
  embenet_critical_section.c
  embenet_eui64.c
  embenet_radio.c
  embenet_random.c
  embenet_timer.c
  radio_driver.c
)
add_library(embetech::embenet_node_port_stm32wle5 ALIAS embenet_node_port_stm32wle5)

target_include_directories(
  embenet_node_port_stm32wle5
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private_include>
)

get_source_file_property(OPTS radio_driver.c TARGET_DIRECTORY embenet_node_port_stm32wle5 COMPILE_OPTIONS)
if (NOT OPTS)
  set(OPTS)
endif ()
list(APPEND OPTS -Wno-unused-parameter -Wno-sign-conversion -Wno-conversion -Wno-enum-conversion)
set_source_files_properties(radio_driver.c TARGET_DIRECTORY embenet_node_port_stm32wle5 PROPERTIES COMPILE_OPTIONS "${OPTS}") # This is awful. I agree, however lets not suffer because someone else is
                                                                                                                              # ancient C programmer

find_package(embenet_node_port_interface REQUIRED)
find_package(ring_buffer REQUIRED)

set(STM32_HAL_TYPE "BOTH")
set(STM32_HAL_DEVICE STM32WLE5xx)
find_package(stm32wlxx_hal REQUIRED)

target_link_libraries(embenet_node_port_stm32wle5 PUBLIC embenet_node_port_interface PRIVATE stm32wlxx_hal ring_buffer)
