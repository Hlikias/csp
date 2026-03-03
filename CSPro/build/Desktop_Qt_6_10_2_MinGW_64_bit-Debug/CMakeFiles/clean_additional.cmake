# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CSPro_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CSPro_autogen.dir\\ParseCache.txt"
  "CSPro_autogen"
  )
endif()
