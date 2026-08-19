#include "models/Report.hpp"

#include "Types.hpp"

using namespace std;

Report::Report(int id, string reporter, string reported, string reason)
    : id_(id),
      reporter_(std::move(reporter)),
      reported_(std::move(reported)),
      reason_(std::move(reason)) {}

auto Report::getReportInfo() const -> ReportInfo { return {id_, reporter_, reported_, reason_}; }