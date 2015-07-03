#include "StdAfx.h"
#include "LogImpl.h"

#include "../dsConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace internal
{
    // http://www.cplusplus.com/reference/ctime/asctime/ 
    // do not return the \n at the end (diff. from asctime)
    static inline char* asctime_impl(const struct tm *timeptr)
    {
        static const char wday_name[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };
        static const char mon_name[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
        static char result[26];
        sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
        wday_name[timeptr->tm_wday],
        mon_name[timeptr->tm_mon],
        timeptr->tm_mday, timeptr->tm_hour,
        timeptr->tm_min, timeptr->tm_sec,
        1900 + timeptr->tm_year);
        return result;
    }
};

#ifdef DS_LOG_MUTEX
static dsMutex g_log_mutex; // just in case do allow only one thread to log data
                            // dao single threaded
#endif

void CLogImpl::Log(LPCTSTR sMsg)
{
    #ifdef DS_LOG_MUTEX
    dsMutexLock lock(g_log_mutex);
    #endif

    const CStdString sFilePath = ds_log_file_path::GetDefaultPath();

    time_t rawtime;
    struct tm *timeinfo;
            
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    const char *sTime = internal::asctime_impl(timeinfo);
    CStdString sTimeUnicode = sTime;

    CStdString sMsgImpl;
    sMsgImpl.Format(_T("[%s] %s\n"), sTimeUnicode.c_str(), sMsg);
    TRACE(sMsgImpl.c_str());
    FILE *pFile = _tfopen(sFilePath.c_str(), _T("a"));    
        _ftprintf(pFile, _T("%s"), sMsgImpl.c_str());
    fclose(pFile);
}