#!/bin/bash
# SPDX-FileCopyrightText: 2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later


podman run -it -v ../../:/code gensalatest:test
