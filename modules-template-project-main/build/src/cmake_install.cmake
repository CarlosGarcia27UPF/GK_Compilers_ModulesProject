# Install script for directory: /home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/src

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

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/module_args/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/module_2/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/cli/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/io/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/comments/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/directives/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/macros/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/errors/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/tokens/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/buffer/cmake_install.cmake")
  include("/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/pp_core/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/GK_Compilers_ModulesProject/GK_Compilers_ModulesProject/modules-template-project-main/build/src/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
