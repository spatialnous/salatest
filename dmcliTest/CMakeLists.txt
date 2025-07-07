# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(target_NAME dmcliTest)
set(testable_target_NAME dmcli)

set(target_HDRS
    argumentholder.hpp
    selfcleaningfile.hpp
)

set(target_SRCS
    main.cpp
    ../${testable_target_NAME}/dminterface/shapemapdm.cpp
    ../${testable_target_NAME}/dminterface/pointmapdm.cpp
    ../${testable_target_NAME}/dminterface/shapegraphdm.cpp
    ../${testable_target_NAME}/dminterface/metagraphdm.cpp
    ../${testable_target_NAME}/printcommunicator.cpp
    ../${testable_target_NAME}/commandlineparser.cpp
    testcommandlineparser.cpp
    testradiusconverter.cpp
    ../${testable_target_NAME}/radiusconverter.cpp
    testsimpletimer.cpp
    testvgaparser.cpp
    ../${testable_target_NAME}/vgaparser.cpp
    testlinkparser.cpp
    ../${testable_target_NAME}/linkparser.cpp
    testagentparser.cpp
    ../${testable_target_NAME}/agentparser.cpp
    testargumentholder.cpp
    ../${testable_target_NAME}/performancewriter.cpp
    testperformancewriter.cpp
    testselfcleaningfile.cpp
    ../${testable_target_NAME}/runmethods.cpp
    ../${testable_target_NAME}/modeparserregistry.cpp
    testvisprepparser.cpp
    ../${testable_target_NAME}/visprepparser.cpp
    testaxialparser.cpp
    ../${testable_target_NAME}/axialparser.cpp
    testparsingutils.cpp
    ../${testable_target_NAME}/parsingutils.cpp
    testisovistparser.cpp
    ../${testable_target_NAME}/isovistparser.cpp
    testexportparser.cpp
    ../${testable_target_NAME}/exportparser.cpp
    ../${testable_target_NAME}/importparser.cpp
    testimportparser.cpp
    ../${testable_target_NAME}/stepdepthparser.cpp
    teststepdepthparser.cpp
    ../${testable_target_NAME}/segmentparser.cpp
    testsegmentparser.cpp
    ../${testable_target_NAME}/mapconvertparser.cpp
    testmapconvertparser.cpp
    ../${testable_target_NAME}/segmentshortestpathparser.cpp
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
add_executable(${target_NAME} ${target_HDRS} ${target_SRCS})

target_sources(${target_NAME} PRIVATE ${external_SRCS})

set_source_files_properties(
  ${external_SRCS}
  PROPERTIES
  COMPILE_FLAGS "-w"
)

target_compile_options(${target_NAME} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${target_NAME} ${LINK_LIBS} ${modules_cli} ${modules_cliTest} ${modules_core})

