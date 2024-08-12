# CMakeList.txt : CMake project for LearnOpenGL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Event Horizon")

set(SOURCES "ai.h"
    "asteroid.cpp"
    "asteroid.h"
    "controller.cpp"
    "controller.h"
    "coord.cpp"
    "coord.h"
    "ehbutton.cpp"
    "ehbutton.h"
    "ehmenuitem.cpp"
    "ehmenuitem.h"
    "main.cpp"
    "obj.h"
    "object3D.cpp"
    "object3D.h"
    "planet.h"
    "player.cpp"
    "player.h"
    "shader.cpp"
    "shader.h"
    "shieldspot.cpp"
    "shieldspot.h"
    "solarsystem.cpp"
    "solarsystem.h"
    "spaceobject.cpp"
    "spaceobject.h"
    "spaceship.cpp"
    "spaceship.h"
    "texture.cpp"
    "texture.h"
    
)

list(TRANSFORM SOURCES PREPEND "cpp/")

# Add source to this project's executable.
add_executable (EventHorizon ${SOURCES})

find_package(glad CONFIG REQUIRED)
target_link_libraries(EventHorizon PRIVATE glad::glad)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(EventHorizon
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

find_package(SDL2_image CONFIG REQUIRED)
    target_link_libraries(EventHorizon PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

find_package(glm CONFIG REQUIRED)
target_link_libraries(EventHorizon PRIVATE glm::glm-header-only)

find_package(freetype-gl CONFIG REQUIRED)
target_link_libraries(EventHorizon PRIVATE freetype-gl)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EventHorizon PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
