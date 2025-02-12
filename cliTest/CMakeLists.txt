# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(cliTest cliTest)

set(cliTest_HDRS
    argumentholder.hpp
    selfcleaningfile.hpp
)

set(cliTest_SRCS
    main.cpp
    ../depthmapXcli/dxinterface/shapemapdx.cpp
    ../depthmapXcli/dxinterface/pointmapdx.cpp
    ../depthmapXcli/dxinterface/shapegraphdx.cpp
    ../depthmapXcli/dxinterface/metagraphdx.cpp
    ../depthmapXcli/printcommunicator.cpp
    ../depthmapXcli/commandlineparser.cpp
    testcommandlineparser.cpp
    testradiusconverter.cpp
    ../depthmapXcli/radiusconverter.cpp
    testsimpletimer.cpp
    testvgaparser.cpp
    ../depthmapXcli/vgaparser.cpp
    testlinkparser.cpp
    ../depthmapXcli/linkparser.cpp
    testagentparser.cpp
    ../depthmapXcli/agentparser.cpp
    testargumentholder.cpp
    ../depthmapXcli/performancewriter.cpp
    testperformancewriter.cpp
    testselfcleaningfile.cpp
    ../depthmapXcli/runmethods.cpp
    ../depthmapXcli/modeparserregistry.cpp
    testvisprepparser.cpp
    ../depthmapXcli/visprepparser.cpp
    testaxialparser.cpp
    ../depthmapXcli/axialparser.cpp
    testparsingutils.cpp
    ../depthmapXcli/parsingutils.cpp
    testisovistparser.cpp
    ../depthmapXcli/isovistparser.cpp
    testexportparser.cpp
    ../depthmapXcli/exportparser.cpp
    ../depthmapXcli/importparser.cpp
    testimportparser.cpp
    ../depthmapXcli/stepdepthparser.cpp
    teststepdepthparser.cpp
    ../depthmapXcli/segmentparser.cpp
    testsegmentparser.cpp
    ../depthmapXcli/mapconvertparser.cpp
    testmapconvertparser.cpp
    ../depthmapXcli/segmentshortestpathparser.cpp
    testsegmentshortestpathparser.cpp)

set(external_SRCS
    ../ThirdParty/Catch/catch_amalgamated.cpp
) # external_SRCS

include_directories(SYSTEM "../ThirdParty/Catch" "../ThirdParty/FakeIt")

set(LINK_LIBS salalib)

set(modules_cliTest "" CACHE INTERNAL "modules_cliTest" FORCE)
set(MODULES_GUI FALSE)
set(MODULES_CLI FALSE)
set(MODULES_CLI_TEST TRUE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST FALSE)
add_subdirectory(../modules modules)

# The headers are added to the target primarily so that they appear as part
# of this particular project to IDEs that open it
add_executable(${cliTest} ${cliTest_HDRS} ${cliTest_SRCS})

target_sources(${cliTest} PRIVATE ${external_SRCS})

set_source_files_properties(
  ${external_SRCS}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

target_compile_options(${cliTest} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${cliTest} ${LINK_LIBS} ${modules_cli} ${modules_cliTest} ${modules_core})

