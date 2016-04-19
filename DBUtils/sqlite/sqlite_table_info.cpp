#include "StdAfx.h"
#include "sqlite_table_info.h"

#include "SqLiteRecordsetImpl.h"
#include "sqlite_bind_util.h"
#include "sqlite_include.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace sqlite_util
{
    eFieldType CFieldInfo::GetFieldType() const
    {
        CStdStringA sType = m_sType;
        sType.ToUpper();
        if ( sType == "INTEGER" ) {
            return eFieldType_Long;
        }

        if ( sType == "TEXT" ) {
            return eFieldType_Text;
        }

        if ( sType == "REAL" ) {
            return eFieldType_Double;
        }

        if ( sType == "BLOB" ) {
            return eFieldType_Binary;
        }

        if ( sType == "DATE" ) {
            return eFieldType_Long;
        }
        if ( sType == "DATETIME" ) {
            return eFieldType_Long;
        }
        // If the declared type contains the string "INT" then it is assigned INTEGER affinity.
        if ( sType == "INT" ) {
            return eFieldType_Long;
        }
        if ( sType == "INTEGER" ) {
            return eFieldType_Long;
        }
        if ( sType == "TINYINT" ) {
            return eFieldType_Long;
        }
        if ( sType == "SMALLINT" ) {
            return eFieldType_Long;
        }
        if ( sType == "MEDIUMINT" ) {
            return eFieldType_Long;
        }
        if ( sType == "BIGINT" ) {
            return eFieldType_Long;
        }
        if ( sType == "UNSIGNED BIG INT" ) {
            return eFieldType_Long;
        }
        if ( sType == "INT2" ) {
            return eFieldType_Long;
        }
        if ( sType == "INT8" ) {
            return eFieldType_Long;
        }

        // If the declared type of the column contains any of the strings "CHAR", "CLOB", or "TEXT" 
        // then that column has TEXT affinity. Notice that the type VARCHAR contains the string "CHAR" and is thus assigned TEXT affinity.
        if ( sType == "CHARACTER" ) {
            return eFieldType_Text;
        }
        if ( sType == "VARCHAR" ) {
            return eFieldType_Text;
        }
        if ( sType == "VARYING CHARACTER" ) {
            return eFieldType_Text;
        }
        if ( sType == "NCHAR" ) {
            return eFieldType_Text;
        }
        if ( sType == "NATIVE CHARACTER" ) {
            return eFieldType_Text;
        }
        if ( sType == "NVARCHAR" ) {
            return eFieldType_Text;
        }
        if ( sType == "CHAR" ) {
            return eFieldType_Text;
        }
        if ( sType == "CLOB" ) {
            return eFieldType_Text;
        }
        //SQLite does not have a separate Boolean storage class. Instead, Boolean values are stored as integers 0 (false) and 1 (true).
        if ( sType == "BOOLEAN" ) {
            return eFieldType_Long;
        }
        // If the declared type for a column contains any of the strings "REAL", "FLOA", or "DOUB" then the column has REAL affinity
        if ( sType == "NUMERIC" ) {
            return eFieldType_Double;
        }
        if ( sType == "DECIMAL" ) {
            return eFieldType_Double;
        }
        if ( sType == "FLOAT" ) {
            return eFieldType_Double;
        }
        if ( sType == "DOUBLE" ) {
            return eFieldType_Double;
        }
        if ( sType == "DOUBLE PRECISION" ) {
            return eFieldType_Double;
        }

        ASSERT(FALSE);
        return eFieldType_Undefined;
    }

    bool sqlite_get_table_fields_info(CSqLiteDatabaseImpl *pDB, const char *sTableName, CSqLiteErrorHandler *pErrorHandler, CFieldInfoMap &map)
    {
        ASSERT(strlen(sTableName) > 0);

        // http://stackoverflow.com/questions/20979239/how-to-tell-if-a-sqlite-column-is-autoincrement
        // Interpretation:
            // If the count came out as non-zero, the table has an autoincrement primary key column.
            // If the count came out as zero, the table is either empty and has never contained data, or does not have an autoincrement primary key column.

        // The autoincrement keyword can only be applied to an
        // integer primary key column, and when it is, sqlite creates a entry in
        // its internal sqlite_sequence table with the name set to the name of the
        // table. 
        bool bAutoIncrement = false;
        {
            CSqLiteRecordsetImpl auto_increment_loader(pDB, pErrorHandler);
            std::string sSQL;
            sSQL = "SELECT COUNT(*) FROM sqlite_sequence WHERE name='";
            sSQL += sTableName;
            sSQL += "'";
            if ( auto_increment_loader.OpenSQLUTF8(sSQL.c_str()) ) {
                if ( auto_increment_loader.MoveFirstImpl() ) {
                    bAutoIncrement = auto_increment_loader.GetFieldLong(_T("COUNT(*)")) > 0;
                }
            }
        }
     
        std::string sPragma; 
        sPragma = "PRAGMA table_info ("; 
        sPragma += sTableName;
        sPragma += ")";

        CSqLiteRecordsetImpl loader(pDB, pErrorHandler);

        if ( !loader.OpenSQLUTF8(sPragma.c_str()) ) {
            return false;
        }

        if ( !loader.MoveFirstImpl() ) {
            return false;
        }

        while ( !loader.IsEOF() ) 
        {
            CFieldInfo field_info;
            const std::string sColumn = loader.GetFieldStringUTF8("name");
            field_info.m_sType    = loader.GetFieldStringUTF8("type");
            field_info.m_bNotNull = loader.GetFieldLong(_T("notnull")) == 1;
            field_info.m_sDefault = loader.GetFieldStringUTF8("dflt_value");
            field_info.m_bPrimary = loader.GetFieldLong(_T("pk")) == 1;
            if ( field_info.m_bPrimary ) {
                field_info.m_bAutoIncrement = bAutoIncrement;
            } else {
                field_info.m_bAutoIncrement = false;
            }
            map[sColumn] = field_info;

            loader.MoveNext();
        }

        return true;
    }
};