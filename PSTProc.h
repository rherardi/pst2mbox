// NotesProc.h: interface for the CPSTProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSTPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_)
#define AFX_PSTPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DATEMASK_DWK_ALL			_T("DWK, DD MMM YYYY HH:MI:SS TZHH")
#define DATEMASK_DWK2				_T("DWK MMM DD HH:MI:SS YYYY")
#define DATEMASK_YYYYMMDDHHMI		_T("YYYYMMDDHHMI")
#define DATEMASK_YYYYMMDDHHMISS		_T("YYYYMMDDHHMISS")

#define MODIFIER_BASE64				_T("base64")
#define MODIFIER_SMTPLOOKUP			_T("smtpLookup")

#define PSTMASK _T("*.pst")

#define BUFFER_SIZE 8192
#define ENTRY_LEN 256

class CPST2MBOXApp;
class CMailbox;
class CObjectArray;
class CMessageItem;
class CList;

typedef struct {
	unsigned long totalBytesProcessed;
	FILE *outputFile;
} CallBackData;

class CPSTProc  
{
public:
	CPSTProc();
	virtual ~CPSTProc();

	int Start(_TCHAR* exec, CPST2MBOXApp *theApp);
	int PerformMigration(char *dbFullPath);
	int PerformMigrationLocal(_TCHAR *inputDir);
	int PerformOneMailboxMigration(LPMAPISESSION pses, LPMDB pmdb, const _TCHAR *pstFile, const _TCHAR *outputdir, 
		UINT *totalFiles = NULL, UINT *totalFilesError = NULL);
//	char* ResolveEntry(DBHANDLE hDB, char* token, const _TCHAR* resolveDocField, const _TCHAR* resolveDocForm,
//							   const _TCHAR* resolveDocLookupField, int* retCode);
//	STATUS near pascal GetUniqueFileName(char *Drive, char *Ext,
//                                        char *FileName);
	HRESULT ResolveFolders(LPMAPISESSION pSes, LPMDB pmdb, LPMAPIFOLDER lpfRootF, _TCHAR *rootFolderName,
									_TCHAR *output_dir,
								  CObjectArray& resolvedFolders, 
								  UINT *totalMsgs, UINT *totalMsgsDone, 
								  UINT *totalMsgsSkipped, UINT *totalMsgsError);
	void FilterFolders(CObjectArray& resolvedFolders, _TCHAR const *requestedFolders);
	HRESULT ProcessOneSkippedFolder(LPMDB pmdb, _TCHAR *fullFolderName,
									UINT *totalFolderMsgsSkipped);
	HRESULT ProcessOneFolder(LPMAPISESSION pSes, LPMDB pmdb, 
		_TCHAR *folderName, _TCHAR *output_dir,
		UINT *totalFolderMsgs, UINT *totalFolderMsgsDone, 
		UINT *totalFolderMsgsSkipped, UINT *totalFolderMsgsError);
	HRESULT ProcessOneMessage(LPMESSAGE lpMsg, FILE *f);
	HRESULT ApplyTransformers(LPMESSAGE lpMsg,  const _TCHAR *name, 
		const _TCHAR *value, FILE *outputFile);

	HRESULT ProcessBody(LPMESSAGE lpMsg, FILE* outputFile);
	HRESULT ProcessAttachments(LPMESSAGE lpMsg, FILE* outputFile);
	HRESULT PutTextHeader(LPMESSAGE lpMsg, FILE *outputFile);
	HRESULT PutFileHeader(LPMESSAGE lpMsg, FILE *outputFile);
	HRESULT PutFilesFooter(LPMESSAGE lpMsg, FILE *outputFile);

/*
	void PutMessageSender(NOTEHANDLE hNote, LPMESSAGE lpMessage, LPSPropValue pMessagePropValues, UINT *c);
	void PutMessageRecipients(NOTEHANDLE hNote, LPMESSAGE lpMessage, LPSPropValue pMessagePropValues, UINT *c);
	STATUS ProcessBody(NOTEHANDLE hNote, LPMESSAGE lpMessage, UINT *NumNotesEncrypt);
	STATUS ProcessAttachments(NOTEHANDLE hNote, LPMESSAGE lpMessage, UINT *NumNotesEncrypt);
	void PutFileHeader(NOTEHANDLE hNote, BLOCKID value_blockid, FILE *outputFile);
	void PutFilesFooter(NOTEHANDLE hNote, FILE *outputFile);
	void PutTextHeader(NOTEHANDLE hNote, FILE *outputFile);
	STATUS ApplyTransformers(NOTEHANDLE hNote, CMessageItem *item, 
		LPMESSAGE lpMessage, LPSPropValue pMessagePropValues, UINT *c,
		BLOCKID *value_blockid = NULL);
	bool IsDateInRange(DBHANDLE hDB, NOTEHANDLE hNote, _TCHAR const *dateFilterField, 
								_TCHAR const *dateFilterAfter, _TCHAR const *dateFilterBefore, 
								_TCHAR const *dateMask);
*/
	void parseSMTPFile(const _TCHAR *smtpAddrFile, const _TCHAR *smtpPartsSep);
	void DoSmtpLookup(_TCHAR *field_text, WORD *field_len);


protected:
	bool m_verbose;
	_TCHAR m_language[3];
	_TCHAR m_server[256];
	_TCHAR m_basedn[512];
	_TCHAR m_filter[256];
	_TCHAR m_container[512];
	_TCHAR m_groupcontainer[512];
	_TCHAR m_pstfilePassword[128];
	_TCHAR m_inputdir[512];
	_TCHAR m_logFile[512];
	unsigned int m_logSizeWrap;
	_TCHAR m_outputdir[512];
	_TCHAR m_onlyPSTFile[512];
	_TCHAR m_smtpAddrFile[256];
	_TCHAR m_smtpPartsSep[32];
	bool m_appendLogFile;

	CPST2MBOXApp *theParentApp;

	CMailbox *mailbox;
	CList *listSuppl;
	CList *smtpAdr;

public:
	// Get/Set m_language
	_TCHAR const * GetLanguage();
	void SetLanguage(const _TCHAR *language);

	// Get/Set m_server
	_TCHAR const * GetServer();
	void SetServer(const _TCHAR *server);

	// Get/Set m_filter
	_TCHAR const * GetFilter();
	void SetFilter(const _TCHAR *filter);

	// Get/Set m_container
	_TCHAR const * GetContainer();
	void SetContainer(const _TCHAR *container);

	// Get/Set m_groupcontainer
	_TCHAR const * GetGroupContainer();
	void SetGroupContainer(const _TCHAR *groupcontainer);

	// Get/Set m_idfilePassword
	_TCHAR const * GetPSTFilePassword();
	void SetPSTFilePassword(const _TCHAR *idFilePassword);

	// Get/Set m_inputdir
	_TCHAR const * GetInputDir();
	void SetInputDir(const _TCHAR *inputDir);

	// Get/Set m_logFile
	_TCHAR const * GetLogFile();
	void SetLogFile(const _TCHAR *logFile);

	// Get/Set m_logSizeWrap
	unsigned int GetLogSizeWrap();
	void SetLogSizeWrap(unsigned int logSizeWrap);

	// Get/Set m_outputdir
	_TCHAR const * GetOutputDir();
	void SetOutputDir(const _TCHAR *outputDir);

	// Get/Set m_onlyNSFFile
	_TCHAR const * GetOnlyPSTFile();
	void SetOnlyPSTFile(const _TCHAR *onlyNSFFile);

	// Get/Set m_smtpAddrFile
	_TCHAR const * GetSmtpAddrFile();
	void SetSmtpAddrFile(const _TCHAR *smtpAddrFile);

	// Get/Set m_smtpPartsSep
	_TCHAR const * GetSmtpPartsSep();
	void SetSmtpPartsSep(const _TCHAR *smtpPartsSep);

	// Get/Set m_verbose
	bool const GetVerbose();
	void SetVerbose(const bool verbose);

	// Get/Set m_appendLogFile
	bool const GetAppendLogFile();
	void SetAppendLogFile(const bool appendLogFile);

	int CreateOutlookFile(_TCHAR *targetdir, const _TCHAR *name, int how, bool splitName = TRUE, _TCHAR *nsfFile = NULL);

private:
	void GetPSTMask(_TCHAR *pstmask);
	ULONG GetXMLPropTagValue(const _TCHAR *propTag);
	bool IsIgnoredFolder(_TCHAR *ignoredFolders, _TCHAR *folderName);
	bool IsIgnoredMessageForm(_TCHAR *messageForms, char *messageFormCh);
//	STATUS RetrieveFirstLastDomainFromSent(HANDLE hMessageFile, _TCHAR *firstLast);
	void SetRecipient(CMessageItem *item, LPMESSAGE lpMessage, char *field_text, char *email, LPSPropValue pMessagePropValues, UINT *c);

};

#endif // !defined(AFX_PSTPROC_H__8CD32256_505C_4973_9A04_DBC9BF1C4FA2__INCLUDED_)
