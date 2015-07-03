#include "StdAfx.h"
#include "DAOExtensions.h"

#include "AfxDao.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace dao_extensions
{
	namespace internal
	{
		static void CopyTableData(CDaoDatabase *pDbSrc, CDaoDatabase *pDbDst, LPCTSTR pszTableNameSrc, LPCTSTR pszTableNameDst, LPCTSTR pszColumnName)
		{
            CString strFormat;
			strFormat.Format(_T("INSERT INTO %s IN '%s' SELECT %s FROM %s;"), pszTableNameDst, pDbDst->GetName(), pszColumnName, pszTableNameSrc);
			pDbSrc->Execute(strFormat);
		}
	}

    void CopyTableData(CDaoDatabase *pDbSrc, CDaoDatabase *pDbDst, LPCTSTR sTableNameSrc, LPCTSTR sTableNameDst)
    {
        ASSERT(sTableNameSrc);
		ASSERT(sTableNameDst);

        ASSERT(pDbSrc);
        ASSERT(pDbSrc->IsOpen());

        ASSERT(pDbDst);
        ASSERT(pDbDst->IsOpen());

        int nControlValue(1);

        CMap<CString, LPCTSTR, int, int> mapColumns;

        // collect all column names from dst
        {
            CDaoTableDef oSrcDaoTableInfo(pDbDst);
            CDaoFieldInfo fieldInfo;

            oSrcDaoTableInfo.Open(sTableNameSrc);
            if (oSrcDaoTableInfo.IsOpen())
            {
                short nFields = oSrcDaoTableInfo.GetFieldCount();
                for(short i = 0; i < nFields; ++i) {
                    oSrcDaoTableInfo.GetFieldInfo(i, fieldInfo);
                    mapColumns.SetAt(fieldInfo.m_strName, nControlValue);
                }
                oSrcDaoTableInfo.Close();
            }
        }
    
        // collect all column names from src
        {
            int nValue;
            CDaoTableDef oDstDaoTableInfo(pDbSrc);
            CDaoFieldInfo fieldInfo;

            oDstDaoTableInfo.Open(sTableNameDst);
            if ( oDstDaoTableInfo.IsOpen() )
            {
                short nFields = oDstDaoTableInfo.GetFieldCount();
                for (short i = 0; i < nFields; ++i)
                {
                    oDstDaoTableInfo.GetFieldInfo(i, fieldInfo);

                    if (mapColumns.Lookup(fieldInfo.m_strName, nValue)) {
                        mapColumns.SetAt(fieldInfo.m_strName, 2);
                    }
                }
                oDstDaoTableInfo.Close();
            }
        }
       
        POSITION pos = mapColumns.GetStartPosition();
        while ( pos ) // remove key which not union
        {
            int nValue;
            CString str;

            mapColumns.GetNextAssoc(pos, str, nValue);
            if (nValue == 1) {
                mapColumns.RemoveKey(str);
            }
        }

        if (mapColumns.GetCount())
        {
            CString strColumns;
            pos = mapColumns.GetStartPosition();
            while ( pos )
            {
                int nValue;
                CString str;

                mapColumns.GetNextAssoc(pos, str, nValue);
                if (!strColumns.IsEmpty())
                {
                    strColumns += _T(",");
                }

                strColumns += CString(sTableNameSrc) + _T(".[") + str + _T("]");
            }

            internal::CopyTableData(pDbSrc, pDbDst, sTableNameSrc, sTableNameDst, strColumns);
        }
    }
};
