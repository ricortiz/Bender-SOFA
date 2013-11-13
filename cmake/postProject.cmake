include(${CMAKE_CURRENT_LIST_DIR}/postBuildConfig.cmake)


message(STATUS "  > Defines = ${GLOBAL_COMPILER_DEFINES} ${COMPILER_DEFINES}")
# message(STATUS "  > Linker Dependencies = ${ADDITIONAL_LINKER_DEPENDENCIES} ${LINKER_DEPENDENCIES}")
# message(STATUS "  > Compiler Flags = ${COMPILER_FLAGS_OS_SPECIFIC} ${COMPILER_FLAGS}")
# message(STATUS "  > Linker Flags = ${LINKER_FLAGS_OS_SPECIFIC} ${LINKER_FLAGS}")
# message(STATUS "  > Includes = ${${PROJECT_NAME}_INCLUDE_DIR}")
# 
# get_target_property(buildType ${PROJECT_NAME} TYPE)
# if(buildType AND (NOT ${buildType} MATCHES "EXECUTABLE"))
#   get_target_property(targetName ${PROJECT_NAME} LOCATION)
# endif()
# message(STATUS "  > Target Type = ${buildType}")
# message(STATUS "  > Target Name = ${targetName}")
message(STATUS "  > ${PROJECT_NAME} : Done\n")