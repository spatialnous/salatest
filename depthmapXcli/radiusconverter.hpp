// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>

class IRadiusConverter {
  public:
    virtual double ConvertForVisibility(const std::string &radius) const = 0;
    virtual double ConvertForMetric(const std::string &radius) const = 0;
    virtual ~IRadiusConverter() {}
};

class RadiusConverter : public IRadiusConverter {
  public:
    double ConvertForVisibility(const std::string &radius) const override;
    double ConvertForMetric(const std::string &radius) const override;
};
