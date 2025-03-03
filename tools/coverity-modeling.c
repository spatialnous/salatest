// SPDX-FileCopyrightText: 2025 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

// This file is to be uploaded to coverity to signify
// falso positives

// Disable checking of deliberately unsafe functions
void testUnsafeRowMatrixCopyMove() { __coverity_panic__(); }
void testUnsafeColumnMatrixCopyMove() { __coverity_panic__(); }
