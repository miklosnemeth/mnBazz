# Display version: PROJECT_VERSION on the release tag, else "<version>-dev+<short commit>".

set(MNBAZZ_VERSION_STRING "${PROJECT_VERSION}")

find_package(Git QUIET)
set(_git_dir "${CMAKE_CURRENT_SOURCE_DIR}/.git")

if(GIT_FOUND AND IS_DIRECTORY "${_git_dir}")
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match --match "v${PROJECT_VERSION}"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                    RESULT_VARIABLE _not_release OUTPUT_QUIET ERROR_QUIET)

    if(_not_release)
        execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                        OUTPUT_VARIABLE _commit OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
        if(_commit)
            set(MNBAZZ_VERSION_STRING "${PROJECT_VERSION}-dev+${_commit}")
        else()
            set(MNBAZZ_VERSION_STRING "${PROJECT_VERSION}-dev")
        endif()
    endif()

    set(_watch "${_git_dir}/HEAD" "${_git_dir}/packed-refs")
    file(READ "${_git_dir}/HEAD" _head)
    if(_head MATCHES "^ref: ([^\n]+)")
        list(APPEND _watch "${_git_dir}/${CMAKE_MATCH_1}")
    endif()
    foreach(_file IN LISTS _watch)
        if(EXISTS "${_file}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_file}")
        endif()
    endforeach()
    file(GLOB_RECURSE _tag_files "${_git_dir}/refs/tags/*")
    foreach(_file IN LISTS _tag_files)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_file}")
    endforeach()
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/MnBazzVersion.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/generated/MnBazzVersion.h" @ONLY)
message(STATUS "mnBazz version: ${MNBAZZ_VERSION_STRING}")
