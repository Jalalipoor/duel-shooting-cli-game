#ifndef REPORT_HPP_
#define REPORT_HPP_

#include <string>

#include "Types.hpp"

class Report {
public:
    Report(int id, std::string reporter, std::string reported,
           std::string reason);

    auto getId() const -> int { return id_; }
    auto getReporter() const -> std::string { return reporter_; }
    auto getReported() const -> std::string { return reported_; }
    auto getReason() const -> std::string { return reason_; }

    auto getReportInfo() const -> ReportInfo;

    void close() { is_closed_ = true; }
    bool isClosed() { return is_closed_; }

private:
    int id_;
    std::string reporter_;
    std::string reported_;
    std::string reason_;
    bool is_closed_{false};
};

#endif // REPORT_HPP_