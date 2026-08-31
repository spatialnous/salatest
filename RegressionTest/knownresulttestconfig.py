# SPDX-FileCopyrightText: 2017-2019 Christian Sailer
# SPDX-FileCopyrightText: 2017-2026 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os

class KnownResultTestConfig:
    """ Encapsulate known result tests config
        This takes an optional known result tests config. All Elements
        in the known result tests config are optional. If it is None
        completely, known result testing will be disabled
        "knownresult":
        {
            enabled = 1,             <- enable known result tests
            resultsdir = "expected", <- location of expected results
            tolerance = 0.0001,      <- what tolerance to accept for result correctness.
                                        NOT CURRENTLY USED - see the note on
                                        self.tolerance below.
        }
    """
    def __init__(self, config):
        if None == config or ("enabled" in config and config["enabled"] not in ["True", "true", "1", "yes"]):
            self.enabled = False
            return
        self.enabled = True;
        self.resultsdir = config.get("resultsdir", None)
        if isinstance(self.resultsdir, list):
            self.resultsdir = os.path.join(*self.resultsdir)

        # Parsed but deliberately unused: KnownResultTestRunner compares outputs
        # byte-for-byte (diffBinaryFiles), because the project targets bit-identical
        # results on every platform rather than results that merely agree to within
        # a tolerance. Kept so the option stays available if a measure is ever added
        # whose output cannot reasonably be made bit-exact. Do NOT wire this up to
        # paper over a cross-platform divergence - such a divergence is a bug in the
        # analysis code, and the ones found so far (FMA contraction, long double,
        # libm-derived values used as container keys or bin indices) were all fixable
        # at source.
        self.tolerance = float(config.get("tolerance", 1))
