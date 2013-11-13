if(PRECONFIGURE_DONE)

	# print report
	unset(FINAL_LOG)
	if(GLOBAL_ERROR_MESSAGE)
		set(FINAL_LOG "${FINAL_LOG}Error(s):\n")
		foreach(ERROR_MESSAGE ${GLOBAL_ERROR_MESSAGE})
			set(FINAL_LOG "${FINAL_LOG}${ERROR_MESSAGE}\n")
		endforeach()
	endif()
	if(GLOBAL_WARNING_MESSAGE)
		set(FINAL_LOG "${FINAL_LOG}Warning(s):\n")
		foreach(WARNING_MESSAGE ${GLOBAL_WARNING_MESSAGE})
			set(FINAL_LOG "${FINAL_LOG}${WARNING_MESSAGE}\n")
		endforeach()
	endif()
	if(GLOBAL_LOG_MESSAGE)
		set(FINAL_LOG "${FINAL_LOG}Log(s):\n")
		foreach(LOG_MESSAGE ${GLOBAL_LOG_MESSAGE})
			set(FINAL_LOG "${FINAL_LOG}${LOG_MESSAGE}\n")
		endforeach()
	endif()
	if(GLOBAL_FORCE_RECONFIGURE)
		set(FINAL_LOG "${FINAL_LOG}###########################################################\nConfiguration changed, you must configure the project again\n###########################################################")
	endif()
	if(FINAL_LOG)
		set(FINAL_LOG "\nFinal report:\n${FINAL_LOG}\n\n")
		if(GLOBAL_ERROR_MESSAGE OR GLOBAL_FORCE_RECONFIGURE)
			message(SEND_ERROR "${FINAL_LOG}")
		elseif(GLOBAL_WARNING_MESSAGE)
			message(WARNING "${FINAL_LOG}")
		else()
			message(STATUS "${FINAL_LOG}")
		endif()
	endif()
	set(SOFA_LIBRARIES)
	set(SOFA_INCLUDE_DIRS)
	set(SOFA_DEFINES)
	set(projectNames ${GLOBAL_DEPENDENCIES})
    foreach(projectName ${projectNames})
        get_target_property(buildType ${projectName} TYPE)
        if(buildType AND (NOT ${buildType} MATCHES "EXECUTABLE"))
          get_target_property(targetName ${projectName} LOCATION_${CMAKE_BUILD_TYPE})
          set(SOFA_LIBRARIES ${SOFA_LIBRARIES} ${targetName})
          set(SOFA_INCLUDE_DIRS ${SOFA_INCLUDE_DIRS} ${${projectName}_INCLUDE_DIR})
          set(SOFA_DEFINES ${SOFA_DEFINES} ${${projectName}_COMPILER_DEFINES})
#           message(STATUS "  > Defines = ${GLOBAL_COMPILER_DEFINES} ${COMPILER_DEFINES}")
#           message(STATUS "  > Target Type = ${buildType}")
#           message(STATUS "  > Target Name = ${targetName}")
#           message(STATUS "  > Includes = ${${projectName}_INCLUDE_DIR}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES SOFA_LIBRARIES)
    list(REMOVE_DUPLICATES SOFA_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES SOFA_DEFINES)
	configure_file(${SOFA_CMAKE_DIR}/SOFAConfig.cmake.in 
      ${CMAKE_CURRENT_BINARY_DIR}/SOFAConfig.cmake @ONLY)
	
	message(STATUS "--------------------------------------------")
	message(STATUS "----- DONE CONFIGURING SOFA FRAMEWORK ------")
	message(STATUS "--------------------------------------------")
	message(STATUS "")
	
endif()

set(PRECONFIGURE_DONE 1 CACHE INTERNAL "Configure does not set projects up, it just displays options")