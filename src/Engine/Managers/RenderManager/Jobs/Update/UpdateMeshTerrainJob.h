#pragma once

#include <Defines/Usings/ThreadTypes.h>
#include <IJob.h>

//struct JobContext;

class UpdateMeshTerrainJob : public IJob
{
private:
    bool m_isGenerating = false;
public:
    UpdateMeshTerrainJob() = default;
    ~UpdateMeshTerrainJob() override = default;
    bool Execute(JobContext* context) override;
};
