# Install script for directory: C:/ČVUT/pgr/pgrsem/lib/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/pgrsem")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Program Files/JetBrains/CLion 2024.3.3/bin/mingw/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ČVUT/pgr/pgrsem/cmake-build-release-mingw/lib/assimp/lib/libassimp.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.4.3" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/ČVUT/pgr/pgrsem/cmake-build-release-mingw/lib/assimp/bin/libassimp-5.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libassimp-5.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libassimp-5.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Program Files/JetBrains/CLion 2024.3.3/bin/mingw/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libassimp-5.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/anim.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/aabb.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ai_assert.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/camera.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/color4.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/color4.inl"
    "C:/ČVUT/pgr/pgrsem/cmake-build-release-mingw/lib/assimp/code/../include/assimp/config.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ColladaMetaData.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/commonMetaData.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/defs.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/cfileio.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/light.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/material.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/material.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/matrix3x3.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/matrix3x3.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/matrix4x4.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/matrix4x4.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/mesh.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ObjMaterial.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/pbrmaterial.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/GltfMaterial.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/postprocess.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/quaternion.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/quaternion.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/scene.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/metadata.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/texture.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/types.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/vector2.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/vector2.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/vector3.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/vector3.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/version.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/cimport.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/AssertHandler.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/importerdesc.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Importer.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/DefaultLogger.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ProgressHandler.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/IOStream.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/IOSystem.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Logger.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/LogStream.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/NullLogger.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/cexport.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Exporter.hpp"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/DefaultIOStream.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/DefaultIOSystem.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SceneCombiner.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/fast_atof.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/qnan.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/BaseImporter.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Hash.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ParsingUtils.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/StreamReader.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/StreamWriter.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/StringComparison.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/StringUtils.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SGSpatialSort.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/GenericProperty.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SpatialSort.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SmallVector.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SmoothingGroups.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/SmoothingGroups.inl"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/StandardShapes.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/RemoveComments.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Subdivision.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Vertex.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/LineSplitter.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/TinyFormatter.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Profiler.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/LogAux.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Bitmap.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/XMLTools.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/IOStreamBuffer.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/CreateAnimMesh.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/XmlParser.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/BlobIOSystem.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/MathFunctions.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Exceptional.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/ByteSwapper.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Compiler/poppack1.h"
    "C:/ČVUT/pgr/pgrsem/lib/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

