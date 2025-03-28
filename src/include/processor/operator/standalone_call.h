#pragma once

#include "common/types/value/value.h"
#include "main/db_config.h"
#include "processor/operator/physical_operator.h"

namespace kuzu {
namespace processor {

struct StandaloneCallPrintInfo final : OPPrintInfo {
    std::string functionName;

    explicit StandaloneCallPrintInfo(std::string functionName)
        : functionName(std::move(functionName)) {}

    std::string toString() const override;

    std::unique_ptr<OPPrintInfo> copy() const override {
        return std::unique_ptr<StandaloneCallPrintInfo>(new StandaloneCallPrintInfo(*this));
    }

private:
    StandaloneCallPrintInfo(const StandaloneCallPrintInfo& other)
        : OPPrintInfo(other), functionName(other.functionName) {}
};

struct StandaloneCallInfo {
    const main::Option* option;
    common::Value optionValue;
    bool hasExecuted = false;

    StandaloneCallInfo(const main::Option* option, common::Value optionValue)
        : option{option}, optionValue{std::move(optionValue)} {}

    std::unique_ptr<StandaloneCallInfo> copy() {
        return std::make_unique<StandaloneCallInfo>(option, optionValue);
    }
};

class StandaloneCall final : public PhysicalOperator {
    static constexpr PhysicalOperatorType type_ = PhysicalOperatorType::STANDALONE_CALL;

public:
    StandaloneCall(std::unique_ptr<StandaloneCallInfo> localState, uint32_t id,
        std::unique_ptr<OPPrintInfo> printInfo)
        : PhysicalOperator{type_, id, std::move(printInfo)},
          standaloneCallInfo{std::move(localState)} {}

    bool isSource() const override { return true; }
    bool isParallel() const override { return false; }

    bool getNextTuplesInternal(ExecutionContext* context) override;

    std::unique_ptr<PhysicalOperator> copy() override {
        return std::make_unique<StandaloneCall>(standaloneCallInfo->copy(), id, printInfo->copy());
    }

private:
    std::unique_ptr<StandaloneCallInfo> standaloneCallInfo;
};

} // namespace processor
} // namespace kuzu
