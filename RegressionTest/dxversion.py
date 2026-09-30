# SPDX-FileCopyrightText: 2024-2026 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

class dXversion():
    def __init__(self, versionString):
        iv = versionString.rsplit(' ', 1)
        if len(iv) == 2:
            self.interface = iv[0]
            self.version = [int(numeric_string) for numeric_string in iv[1].split('.')]
        else:
            self.interface = ""
            self.version = [int(numeric_string) for numeric_string in iv[0].split('.')]

    def __eq__(self, other):
        return (self.version[0] == other.version[0] and
            self.version[1] == other.version[1] and
            self.version[2] == other.version[2])

    def __gt__(self, other):
        return (self.version[0] > other.version[0] or
            (self.version[0] == other.version[0] and
                self.version[1] > other.version[1]) or
            (self.version[0] == other.version[0] and
                self.version[1] == other.version[1] and
                self.version[2] > other.version[2]))

    def __str__(self):
        prefix = ""
        if self.interface != "":
            prefix = self.interface + " "
        return prefix + ".".join(str(x) for x in self.version)