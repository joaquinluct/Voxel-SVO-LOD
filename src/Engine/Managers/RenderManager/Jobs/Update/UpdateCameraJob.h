#pragma once

#include <Defines/Usings/ThreadTypes.h>
#include <IJob.h>

//struct JobContext;

class UpdateCameraJob : public IJob
{
public:
    UpdateCameraJob() = default;
    ~UpdateCameraJob() override = default;
    bool Execute(JobContext* context) override;
};
