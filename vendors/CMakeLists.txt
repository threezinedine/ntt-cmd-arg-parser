######## Variables for the googletest dependency ########
######## End of the variables for the {self.folder} dependency ########
######## Conditions for the googletest dependency ########
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT" FORCE)
endif()
######## End of the conditions for the {self.folder} dependency ########
add_subdirectory(googletest)
######## Variables for the json dependency ########
######## End of the variables for the {self.folder} dependency ########
######## Conditions for the json dependency ########
######## End of the conditions for the {self.folder} dependency ########
add_subdirectory(json)
######## Variables for the ntt-lib dependency ########
######## End of the variables for the {self.folder} dependency ########
######## Conditions for the ntt-lib dependency ########
######## End of the conditions for the {self.folder} dependency ########
add_subdirectory(ntt-lib)
