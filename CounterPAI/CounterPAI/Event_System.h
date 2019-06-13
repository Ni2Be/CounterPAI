#pragma once


class Event_System
{
public:
	static Event_System& instance()
	{
		static Event_System _instance;
		return _instance;
	}
	~Event_System() {}

	void xyz();
private:
	Event_System() {}
	Event_System(const Event_System&);
	Event_System& operator=(const Event_System&);
};

