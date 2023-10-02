#include "utils.h"

#include "Jobs.h"

#include <Windows.h>
#include <sstream>

std::string reflection::GetNewId()
{
    GUID guid;
    HRESULT hCreateGuid = CoCreateGuid(&guid);

    std::stringstream ss;

    ss << std::uppercase;
    ss.width(8);
    ss << std::hex << guid.Data1 << '-';

    ss.width(4);
    ss << std::hex << guid.Data2 << '-';

    ss.width(4);
    ss << std::hex << guid.Data3 << '-';

    ss.width(2);
    ss << std::hex
        << static_cast<short>(guid.Data4[0])
        << static_cast<short>(guid.Data4[1])
        << '-'
        << static_cast<short>(guid.Data4[2])
        << static_cast<short>(guid.Data4[3])
        << static_cast<short>(guid.Data4[4])
        << static_cast<short>(guid.Data4[5])
        << static_cast<short>(guid.Data4[6])
        << static_cast<short>(guid.Data4[7]);
    ss << std::nouppercase;

    std::string res = ss.str();
    return res;
}

void reflection::RunMain(jobs::Job* job)
{
    jobs::GetMainJobSystem()->ScheduleJob(job);
}
