#pragma once

#include <IJob.h>

struct JobContext;

class UpdateConstantBuffersJob : public IJob
{
public:
    UpdateConstantBuffersJob() = default;
    ~UpdateConstantBuffersJob() override = default;
    bool Execute(JobContext* context) override;
private:
    bool isProcessing = false;
};
