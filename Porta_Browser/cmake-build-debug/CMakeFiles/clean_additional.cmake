# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Porta_Browser_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Porta_Browser_autogen.dir/ParseCache.txt"
  "Porta_Browser_autogen"
  )
endif()
