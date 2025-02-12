// SPDX-FileCopyrightText: 2020 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "printcommunicator.hpp"

#include <iostream>

void PrintCommunicator::CommPostMessage(size_t m, size_t x) const {
    switch (m) {
    case Communicator::NUM_STEPS:
        m_numSteps = x;
        break;
    case Communicator::CURRENT_STEP:
        m_step = x;
        break;
    case Communicator::NUM_RECORDS:
        m_numRecords = x;
        break;
    case Communicator::CURRENT_RECORD:
        m_record = x;
        if (m_record > m_numRecords)
            m_record = m_numRecords;
        std::cout << "step: " << m_step << "/" << m_numSteps << " "
                  << "record: " << m_record << "/" << m_numRecords << std::endl;
        break;
    default:
        break;
    }
}
