#pragma once
// #include <vector>

#include <cstddef>
using namespace std;

class LedsManager
{
public:
	static LedsManager *GetLedsManager();
	virtual ~LedsManager();

	void SetLed(const size_t led_index, const bool led_on);
	void ToggleLed(const size_t led_index);
	bool GetLed(const size_t led_index);

private:
	LedsManager();
	inline int GetMask(const size_t led_index);
	size_t m_iNumberOfLeds;
};
