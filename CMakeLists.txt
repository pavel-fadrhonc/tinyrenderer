# CMakeList.txt : CMake project for tinyrenderer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("tinyrenderer")

# Add source to this project's executable.
add_executable ("${CMAKE_PROJECT_NAME}")

target_link_libraries("${CMAKE_PROJECT_NAME}" "user32.lib" "opengl32.lib")

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} "src/TGAColor.h")

set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES WIN32_EXECUTABLE TRUE)

target_compile_options("${CMAKE_PROJECT_NAME}"  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc /W4 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN>
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tinyrenderer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
