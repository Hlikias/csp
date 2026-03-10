# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CSPro_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CSPro_autogen.dir\\ParseCache.txt"
  "CSPro_autogen"
  "Common\\CMakeFiles\\Common_autogen.dir\\AutogenUsed.txt"
  "Common\\CMakeFiles\\Common_autogen.dir\\ParseCache.txt"
  "Common\\Common_autogen"
  "FlotingWin\\CMakeFiles\\FlotingWin_autogen.dir\\AutogenUsed.txt"
  "FlotingWin\\CMakeFiles\\FlotingWin_autogen.dir\\ParseCache.txt"
  "FlotingWin\\CMakeFiles\\FlotingWinplugin_autogen.dir\\AutogenUsed.txt"
  "FlotingWin\\CMakeFiles\\FlotingWinplugin_autogen.dir\\ParseCache.txt"
  "FlotingWin\\FlotingWin_autogen"
  "FlotingWin\\FlotingWinplugin_autogen"
  "NetManager\\CMakeFiles\\NetManager_autogen.dir\\AutogenUsed.txt"
  "NetManager\\CMakeFiles\\NetManager_autogen.dir\\ParseCache.txt"
  "NetManager\\NetManager_autogen"
  "XmlManager\\CMakeFiles\\XmlManager_autogen.dir\\AutogenUsed.txt"
  "XmlManager\\CMakeFiles\\XmlManager_autogen.dir\\ParseCache.txt"
  "XmlManager\\XmlManager_autogen"
  )
endif()
