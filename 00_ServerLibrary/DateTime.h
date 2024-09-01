#pragma once
namespace netlib
{
	class DateTime
	{
	public:
		DateTime();

		int32 Year() const { return _time.tm_year + 1900; }
		int32 Month() const { return _time.tm_mon + 1; }
		int32 Day() const { return _time.tm_mday; }
		int32 Hour() const { return _time.tm_hour; }
		int32 Min() const { return _time.tm_min; }
		int32 Sec() const { return _time.tm_sec; }

		wstring to_string(wstring_view fmt = L"%F %T");
	private:
		tm _time;
	};
}
