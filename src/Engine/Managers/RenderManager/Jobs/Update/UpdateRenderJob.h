#pragma once

#include <IJob.h>

struct JobContext;

class UpdateRenderJob : public IJob
{
public:
	UpdateRenderJob() = default;
	~UpdateRenderJob() override = default;
	bool Execute(JobContext* context) override;
};