#pragma once

#include <IJob.h>

struct JobContext;

class UpdateLightJob : public IJob
{
public:
	UpdateLightJob() = default;
	~UpdateLightJob() override = default;
	bool Execute(JobContext* context) override;
};
