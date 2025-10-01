#pragma once

#include <Defines/Types/ThreadTypes.h>
#include <IJob.h>

//struct JobContext;

class UpdateMeshJob : public IJob
{
private:
    bool m_isGenerating = false;
public:
    UpdateMeshJob() = default;
    ~UpdateMeshJob() override = default;
    bool Execute(JobContext* context) override;
};
