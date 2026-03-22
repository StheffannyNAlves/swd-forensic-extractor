# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/_deps/picotool-src"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/_deps/picotool-build"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/_deps"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/tmp"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/src/picotoolBuild-stamp"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/src"
  "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/stheffanny/Documentos/Github/P1-Engenheiro_Reverso/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
