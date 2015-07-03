#include "StdAfx.h"
#include "AdoDotNetDatabaseImpl.h"

#include "AdoDotNetImpl.h"
#include "AdoDotNetRecordsetImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// String sample:
// Data Source=SQL-SERVER;Initial Catalog=catalog_name;Integrated Security=False;User ID=sa;Password=passsword
bool CAdoDotNetDatabaseImpl::IsMSSQLServerAdoDotNet(LPCTSTR sPath)
{
	if (_tcsstr(sPath, _T("Data Source")) ||
	    _tcsstr(sPath, _T("Server")))
	{
		return true;
	}

	return false;
}

CAdoDotNetDatabaseImpl::CAdoDotNetDatabaseImpl() 
: m_bReadOnly(false)
{
	m_pDatabase = CAdoDotNetUtils::CreateDatabase();
}

CAdoDotNetDatabaseImpl::~CAdoDotNetDatabaseImpl()
{
	CAdoDotNetUtils::DeleteDatabase(m_pDatabase);
}

bool CAdoDotNetDatabaseImpl::BeginTrans() 
{
	return true;
}

bool CAdoDotNetDatabaseImpl::CommitTrans() 
{
	return true;
}

bool CAdoDotNetDatabaseImpl::Rollback()   
{
	return true;
}

bool CAdoDotNetDatabaseImpl::Execute(LPCTSTR lpszSQL) 
{
	return m_pDatabase->Execute(lpszSQL);
}

void CAdoDotNetDatabaseImpl::Close() 
{
	m_pDatabase->Close();
    m_sConnString = _T("");
}

bool CAdoDotNetDatabaseImpl::OpenDB(LPCTSTR sPath, bool bReadOnly, LPCTSTR szPsw) 
{
    m_sConnString = sPath;

	m_bReadOnly = bReadOnly;

	const bool bRet = m_pDatabase->Open(sPath);

	return bRet;
}

dsDBType CAdoDotNetDatabaseImpl::GetType()
{
	return dsType_MsSQL;
}

bool CAdoDotNetDatabaseImpl::IsReadOnly() const
{
	return m_bReadOnly; 
}

bool CAdoDotNetDatabaseImpl::IsOpen() const
{
	return m_pDatabase->IsOpen();
}

CStdString CAdoDotNetDatabaseImpl::GetName()
{
	return m_sConnString;
}

bool CAdoDotNetDatabaseImpl::DoesTableExist(LPCTSTR sTable)
{
	ASSERT(FALSE);
	return false;
}

CAbsRecordset *CAdoDotNetDatabaseImpl::CreateRecordset()
{
	return new CAdoDotNetRecordsetImpl(m_pDatabase);
}

CAdoDotNetDatabaseImpl::dbErrorHandler CAdoDotNetDatabaseImpl::SetErrorHandler(CAdoDotNetDatabaseImpl::dbErrorHandler newHandler)
{
    return m_pDatabase->SetErrorHandler(newHandler);
}

void CAdoDotNetDatabaseImpl::DeleteRelation(LPCTSTR sRelation)
{
	ASSERT(FALSE);
}

bool CAdoDotNetDatabaseImpl::CreateRelation(LPCTSTR sName, LPCTSTR sTable, LPCTSTR sForeignTable, long lAttr,
											LPCTSTR sField, LPCTSTR sForeignField)
{
	ASSERT(FALSE);
	//Impl must be simillar to
	//CStdString sCreateRelSQL;
	//sCreateRelSQL.Format(_T("ALTER TABLE %s ADD CONSTRAINT %s FOREIGN KEY (%s) REFERENCES %s(%s) ON DELETE CASCADE"), sTable1, sRelName, sField1, sTable2, sField2);
	//pDB->ExecuteSQL(sCreateRelSQL.c_str());
	return true;
}

bool CAdoDotNetDatabaseImpl::GetTableFieldInfo(LPCTSTR sTable, dsTableFieldInfo &info)
{
	return m_pDatabase->GetTableFieldInfo(sTable, info);
}