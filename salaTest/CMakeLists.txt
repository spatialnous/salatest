# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2026 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(salaTest salaTest)

set(salaTest_SRCS
    main.cpp
    testentityparsing.cpp
    testlatticemap.cpp
    testlinkutils.cpp
    testgridproperties.cpp
    testisovistdef.cpp
    testmgraph.cpp
    testagent.cpp
    testshapegraphs.cpp
    teststructsizes.cpp
    testsparksieve.cpp
    testattributetable.cpp
    testattributetableindex.cpp
    testlayermanager.cpp
    testattributetablehelpers.cpp
    testattributetableview.cpp
    testshapemaps.cpp
    testgeometrygenerators.cpp
    testmapinfodata.cpp
    testsalaprogram.cpp
    testdxfp.cpp
    testshaperemove.cpp
    testmapconversion.cpp
    testpointinpoly.cpp
    testpushvalues.cpp
    testisovist.cpp
    testsegmentpathscore.cpp
) # salaTest_SRCS

set(external_SRCS
    ../ThirdParty/Catch/catch_amalgamated.cpp
) # external_SRCS

include_directories(SYSTEM "../ThirdParty/Catch" "../ThirdParty/FakeIt")

set(LINK_LIBS salalib)

add_executable(${salaTest} ${salaTest_SRCS}
  comm.hpp)

target_sources(${salaTest} PRIVATE ${external_SRCS})

set_source_files_properties(
  ${external_SRCS}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

target_compile_options(${salaTest} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${salaTest} ${LINK_LIBS})
