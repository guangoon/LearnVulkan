cmake_minimum_required(VERSION 3.22.1)
project(LearnVulkan VERSION 0.1.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
message(STATUS "Found GLFW3 in ${GLFW_INCLUDE_DIRS}")

set(LIBS ${GLFW_LIBRARIES})

set(CHAPTERS
    1.getting_started
)

set(1.getting_started
    1.1.hello_window
)


function(create_project_from_sources chapter demo)
	file(GLOB SOURCE
            "src/${chapter}/${demo}/*.h"
            "src/${chapter}/${demo}/*.cpp"
            "src/${chapter}/${demo}/*.vs"
            "src/${chapter}/${demo}/*.fs"
            "src/${chapter}/${demo}/*.tcs"
            "src/${chapter}/${demo}/*.tes"
            "src/${chapter}/${demo}/*.gs"
            "src/${chapter}/${demo}/*.cs"
    )
	if (demo STREQUAL "")
		SET(replaced "")
		string(REPLACE "/" "_" replaced ${chapter})
		set(NAME ${replaced})
	else()
		set(NAME "${chapter}__${demo}")
	endif()
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    # copy shader files to build directory
    file(GLOB SHADERS
             "src/${chapter}/${demo}/*.vs"
             "src/${chapter}/${demo}/*.fs"
             "src/${chapter}/${demo}/*.tcs"
             "src/${chapter}/${demo}/*.tes"
             "src/${chapter}/${demo}/*.gs"
             "src/${chapter}/${demo}/*.cs"
    )
    foreach(SHADER ${SHADERS})
        file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${chapter})
    endforeach(SHADER)
endfunction()

# then create a project file per tutorial
foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
		create_project_from_sources(${CHAPTER} ${DEMO})
    endforeach(DEMO)
endforeach(CHAPTER)
foreach(GUEST_ARTICLE ${GUEST_ARTICLES})
	create_project_from_sources(${GUEST_ARTICLE} "")
endforeach(GUEST_ARTICLE)

include_directories(
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)


