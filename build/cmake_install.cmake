# Install script for directory: /home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/x86_64-linux-gnu-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ThirdParty/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiConsole/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiLua/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiMath/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiPhysics/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiGraph/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiTimer/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiMesh/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiMPI/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiLog/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiDataTypes/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/ChiMiscUtils/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiTech/LuaTest/cmake_install.cmake")
  include("/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/ChiModules/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/jason/Documents/Work/Chi-Tech/chi-tech-adjoint/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
