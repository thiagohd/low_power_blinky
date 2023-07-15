# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

FILE(GLOB app_sources   src/*) 

#add_executable(app ${app_sources})
#FILE(GLOB_RECURSE app_sources CONFIGURE_DEPENDS "*.c")

FILE(GLOB app_includes  src)

#FILE(GLOB_RECURSE app_includes CONFIGURE_DEPENDS "*.h")

target_include_directories(app PRIVATE ${app_includes})
target_sources(app PRIVATE ${app_sources})


project(blinky)

# Os comandos abaixo servem para setar otimizacao O0 para os arquivos app_sources/app_includes.
add_library(set-app-flags INTERFACE)
target_compile_options(set-app-flags INTERFACE -O0)
target_link_libraries(app PRIVATE set-app-flags)

