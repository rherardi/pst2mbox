// Notes2PSTApp.hpp: interface for the CPST2MBOXApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PST2MBOXAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_)
#define AFX_PST2MBOXAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPSTProc;
class CXMLMailProc;
class CMailbox;
class CMessageItem;
class CEntryTransformer;
class CConversionRuleEntry;
class CXMLNameValue;

#define DEFAULT_CONFIG_FILE	_T("pst2mbox.xml")
#define DEFAULT_SCHEMA_FILE	_T("pst2mbox.xsd")

#define DEFAULT_SCHEMA_PREFIX		_T("http://www.gssnet.com/pst2mbox")
#define DEFAULT_PST_EXT _T(".pst")

class CPST2MBOXApp  
{
public:
	CPST2MBOXApp();
	virtual ~CPST2MBOXApp();

	void ShowUsage();
	_TCHAR *PrefixWithCurDir(_TCHAR *fileName, _TCHAR *fileNameFull);
	int ProcessCommandLine(int argc, _TCHAR *argv[]);
	int StartMigration(_TCHAR* exec);
	CMailbox* getMailboxInfo();
	CMessageItem * GetNextMessageItem(_TCHAR *partCondition = NULL);
	CEntryTransformer* getNextEntryTransformer(_TCHAR *nodeSelector = NULL);
	CConversionRuleEntry* getNextConversionRuleEntry(_TCHAR *nodeSelector = NULL);
	void resetConversionRuleIndex();
	void resetEntryListIndex();
	void resetTransformerListIndex();
	const _TCHAR* getConfigSettingValue(const _TCHAR* settingName);
	bool matchesRegExp(char *regExpPattern, char* matchString);
	CXMLMailProc *getXMLProc();
	void InitializeMessageItemList();
	CXMLNameValue* getNextNameValue(_TCHAR* selector, const _TCHAR *name_attr = _T("name"), const _TCHAR *value_attr = _T("value"));
	bool IsPrintFormsOnly();
	bool IsSMTPLookupRequested();

protected:
	CPSTProc *m_pstProc;
	CXMLMailProc *m_xmlProc;
	bool m_verbose;
	bool m_printFormsOnly;

public:

	int IsVerbose() {
		return m_verbose == true;
	}

private:
	int ExtractSchemaFile(_TCHAR* tempDir);
};

#endif // !defined(AFX_PST2MBOXAPP_HPP__DF6D88D6_63BA_4734_8960_CA9DC7327262__INCLUDED_)
