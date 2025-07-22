#pragma once

struct IDefine
{
private:
	bool m_isRunning = false;
public:
	virtual ~IDefine() = default;
	void Start() {
		m_isRunning = true;
	};
	void Stop() {
		m_isRunning = false;
	};
	bool IsRunning() const {
		return m_isRunning;
	};
};
#pragma once
