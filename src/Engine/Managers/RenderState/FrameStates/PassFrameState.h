#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include "FrameStateBase.h"
#include <Util/DateTime.h>
#include <Defines/TimeDefinition.h>
#include <Config/PassConfigBase.h>
#include <Managers/RenderManager/Pipeline/PipelineState.h>

class PassFrameState : public FrameStateBase {
public:
    PassFrameState(): passes{}  {};
	virtual ~PassFrameState() = default;
	const PassConfigBase& GetConfig() const { return config; }
	void SetConfig(const PassConfigBase& cfg) { config = cfg; }

    std::shared_ptr<PipelineState> GetNextPass() {
        m_idxPass++;
        return GetCurrentPass();
    }
    std::shared_ptr<PipelineState> GetCurrentPass() {
        if (m_idxPass > static_cast<int>(passes.size()) - 1) {
            m_idxPass = 0;
            return nullptr;
        }
        return passes[m_idxPass];
    }

    void AddPass(std::shared_ptr<PipelineState> pass) {
        passes.push_back(pass);
    }
    void ReplacePasses(std::vector<std::shared_ptr<PipelineState>>& newPasses) {        
		passes = newPasses;        
	}
    void ClearPasses() {
        passes.clear();
        m_idxPass = 0;
    }
private:
    PassConfigBase config;
    int m_idxPass = 0;
    std::vector< std::shared_ptr<PipelineState>> passes;
};

