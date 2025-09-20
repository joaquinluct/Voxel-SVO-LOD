#pragma once

#include <IJob.h>

struct JobContext;

class UpdateCommonJob : public IJob
{
public:
    UpdateCommonJob() = default;
    ~UpdateCommonJob() override = default;
    bool Execute(JobContext* context) override;
};
