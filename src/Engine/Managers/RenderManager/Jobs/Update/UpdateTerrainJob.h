#pragma once

#include <Defines/Types/ThreadTypes.h>
#include <IJob.h>

//struct JobContext;

class UpdateTerrainJob : public IJob
{
private:
    bool m_isGenerating = false;
public:
    UpdateTerrainJob() = default;
    ~UpdateTerrainJob() override = default;
    bool Execute(JobContext* context) override;
};
