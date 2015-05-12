// NotesProc.cpp: implementation of the CPSTProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define DONT_USE_OUTPUT_UNICODE
#define OUTLOOK_UNICODE

#include "mapistuff.h"

#include "PSTProc.h"
#include "resource.h"
#include "PST2MBOXApp.hpp"
#include "Config.hpp"
#include "Mailbox.hpp"
#include "MessageItem.hpp"
#include "XMLNameValue.hpp"
#include "EntryTransformer.hpp"
#include "OutlineInfo.hpp"
#include "ObjectArray.hpp"
#include "DirUtils.hpp"
#include "NotesCondition.hpp"
#include "ConversionRuleEntry.hpp"

#include "JESCommon.h"
#include "inistuff.h"
#include "list.hpp"

static char smtp[] = "SMTP";

#define XML_PROP_TAG(NAME)  { _T(#NAME), NAME }

struct PropTag {
	_TCHAR *prop;
	ULONG value;
};

struct PropTag PropTags[] = {
	XML_PROP_TAG(PR_MESSAGE_CLASS),
	XML_PROP_TAG(PR_MESSAGE_FLAGS),
	XML_PROP_TAG(PR_SENDER_ENTRYID),
	XML_PROP_TAG(PR_SENDER_NAME),
	XML_PROP_TAG(PR_SENDER_EMAIL_ADDRESS),
	XML_PROP_TAG(PR_SENDER_ADDRTYPE),
	XML_PROP_TAG(PR_SENT_REPRESENTING_ENTRYID),
	XML_PROP_TAG(PR_SENT_REPRESENTING_NAME),
	XML_PROP_TAG(PR_SENT_REPRESENTING_EMAIL_ADDRESS),
	XML_PROP_TAG(PR_SENT_REPRESENTING_ADDRTYPE),
	XML_PROP_TAG(PR_IMPORTANCE),
	XML_PROP_TAG(PR_CLIENT_SUBMIT_TIME),
	XML_PROP_TAG(PR_MESSAGE_DELIVERY_TIME),
	XML_PROP_TAG(PR_DISPLAY_NAME),
	XML_PROP_TAG(PR_RECIPIENT_TYPE),
	XML_PROP_TAG(PR_SUBJECT),
	XML_PROP_TAG(PR_BODY),
	XML_PROP_TAG(PR_ATTACH_METHOD),
	XML_PROP_TAG(PR_ATTACH_FILENAME),
	XML_PROP_TAG(PR_ATTACH_LONG_FILENAME),
	XML_PROP_TAG(PR_DISPLAY_TO),
	XML_PROP_TAG(PR_DISPLAY_CC)
};

#ifndef MAX_LINE
#define MAX_LINE 256
#endif

/* constants */
#define     READMAIL_BODY_LINELEN   40
char szDriveLetter_blank[] = "";
char szExtensionName_TMP[] = "TMP";
char szNewline_Terminate[] = "\r\n";
#define CHARS_PER_LINE 80

#define SHORT_PRODUCT_TITLE	"pst2mbox"
#define SHORT_PRODUCT_TITLE_W	L"pst2mbox"

LIST *lst;
LIST *notFoundLst;

CPSTProc *pPSTProc;

#define STRING_LENGTH   256

#define FOLDER_INFO_VIEW	"($FolderInfo)"

#define FOLDER_INBOX_CFG_NAME			"Inbox"
#define FOLDER_DRAFTS_CFG_NAME			"Drafts"
#define FOLDER_SENT_CFG_NAME			"Sent"
#define FOLDER_JUNKMAIL_CFG_NAME		"Junk Mail"
#define FOLDER_TRASH_CFG_NAME			"Trash"
#define FOLDER_PRIVATEFOLDERS_CFG_NAME	"PrivateFolders"
#define FOLDER_SHAREDFOLDERS_CFG_NAME	"SharedFolders"
#define FOLDER_FOLDERS_PREFIX_CFG_NAME	"Folders/"
#define FOLDER_ALL_CFG_NAME				"All"

// MAPI Globals
//LPMAPISESSION pses = NULL;
//LPMDB pmdb = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPSTProc::CPSTProc()
{
	m_verbose = false;
	m_language[0] = _T('\0');
	m_server[0] = _T('\0');
	m_basedn[0] = _T('\0');
	m_container[0] = _T('\0');
	m_groupcontainer[0] = _T('\0');
	m_pstfilePassword[0] = _T('\0');
	m_inputdir[0] = _T('\0');
	m_logFile[0] = _T('\0');
	m_outputdir[0] = _T('\0');
	m_onlyPSTFile[0] = _T('\0');
	m_smtpAddrFile[0] = _T('\0');
	pPSTProc = this;

	listSuppl = new CList();
}

CPSTProc::~CPSTProc()
{

}

// Get/Set m_language
_TCHAR const * CPSTProc::GetLanguage()
{
	return &m_language[0];
}
void CPSTProc::SetLanguage(const _TCHAR *language)
{
	if (language != NULL) {
		::_tcscpy(m_language, language);
	}
}

// Get/Set m_filter
_TCHAR const * CPSTProc::GetFilter()
{
	return m_filter;
}
void CPSTProc::SetFilter(const _TCHAR *filter)
{
	if (filter != NULL) {
		::_tcscpy(m_filter, filter);
	}
}

// Get/Set m_container
_TCHAR const * CPSTProc::GetContainer()
{
	return m_container;
}
void CPSTProc::SetContainer(const _TCHAR *container)
{
	if (container != NULL) {
		::_tcscpy(m_container, container);
	}
}

// Get/Set m_groupcontainer
_TCHAR const * CPSTProc::GetGroupContainer()
{
	return m_groupcontainer;
}
void CPSTProc::SetGroupContainer(const _TCHAR *groupcontainer)
{
	if (groupcontainer != NULL) {
		::_tcscpy(m_groupcontainer, groupcontainer);
	}
}

// Get/Set m_pstfilePassword
_TCHAR const * CPSTProc::GetPSTFilePassword()
{
	return &m_pstfilePassword[0];
}
void CPSTProc::SetPSTFilePassword(const _TCHAR *pstfilePassword)
{
	if (pstfilePassword != NULL) {
		::_tcscpy(m_pstfilePassword, pstfilePassword);
	}
}

// Get/Set m_inputdir
_TCHAR const * CPSTProc::GetInputDir()
{
	return &m_inputdir[0];
}
void CPSTProc::SetInputDir(const _TCHAR *inputDir)
{
	if (inputDir != NULL) {
		::_tcscpy(m_inputdir, inputDir);
	}
}

// Get/Set m_logFile
_TCHAR const * CPSTProc::GetLogFile()
{
	return &m_logFile[0];
}
void CPSTProc::SetLogFile(const _TCHAR *logFile)
{
	if (logFile != NULL) {
		::_tcscpy(m_logFile, logFile);
	}
}

// Get/Set m_logSizeWrap
unsigned int CPSTProc::GetLogSizeWrap()
{
	return m_logSizeWrap;
}
void CPSTProc::SetLogSizeWrap(unsigned int logSizeWrap)
{
	m_logSizeWrap = logSizeWrap;
}

// Get/Set m_outputdir
_TCHAR const * CPSTProc::GetOutputDir()
{
	return &m_outputdir[0];
}
void CPSTProc::SetOutputDir(const _TCHAR *outputDir)
{
	if (outputDir != NULL) {
		::_tcscpy(m_outputdir, outputDir);
	}
}

// Get/Set m_onlyPSTFile
_TCHAR const * CPSTProc::GetOnlyPSTFile()
{
	return &m_onlyPSTFile[0];
}
void CPSTProc::SetOnlyPSTFile(const _TCHAR *onlyPSTFile)
{
	if (onlyPSTFile != NULL) {
		::_tcscpy(m_onlyPSTFile, onlyPSTFile);
	}
}

// Get/Set m_smtpAddrFile
_TCHAR const * CPSTProc::GetSmtpAddrFile()
{
	return &m_smtpAddrFile[0];
}
void CPSTProc::SetSmtpAddrFile(const _TCHAR *smtpAddrFile)
{
	if (smtpAddrFile != NULL) {
		::_tcscpy(m_smtpAddrFile, smtpAddrFile);
	}
}

// Get/Set m_smtpPartsSep
_TCHAR const * CPSTProc::GetSmtpPartsSep()
{
	return &m_smtpPartsSep[0];
}
void CPSTProc::SetSmtpPartsSep(const _TCHAR *smtpPartsSep)
{
	if (smtpPartsSep != NULL) {
		::_tcscpy(m_smtpPartsSep, smtpPartsSep);
	}
}

// Get/Set m_verbose
bool const CPSTProc::GetVerbose()
{
	return m_verbose;
}
void CPSTProc::SetVerbose(const bool verbose)
{
	m_verbose = verbose;
}

// Get/Set m_appendLogFile
bool const CPSTProc::GetAppendLogFile()
{
	return m_appendLogFile;
}
void CPSTProc::SetAppendLogFile(const bool appendLogFile)
{
	m_appendLogFile = appendLogFile;
}

int CPSTProc::Start(_TCHAR* exec, CPST2MBOXApp *theApp)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CPSTProc::Start");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}

	int retCode = MIGRATION_SUCCESS;
	HRESULT hr;

	bool localMode = true;

	theParentApp = theApp;
	mailbox = theParentApp->getMailboxInfo();
	
	if (!localMode) {
		//retCode = PerformMigration(fullpath_name);
	}
	else {
		retCode = PerformMigrationLocal((_TCHAR *)this->GetInputDir());
	}

cleanup:
	if (FAILED(hr)) {
//		char szErrorString[512];
//		OSLoadString(NULLHANDLE, ERR(error),
//			szErrorString, 512);
//		::LoadString(hInst, IDS_ERR_OUTLOOK, usage, 256);
//		::_tprintf(_TEXT("%s "), usage);
//		printf("%s\n", szErrorString);
	}
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

// Go through local pst files and process them
int CPSTProc::PerformMigrationLocal(_TCHAR *inputDir)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CPSTProc::PerformMigrationLocal");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	int retCode = MIGRATION_STEP_SUCCESS;
	HRESULT hr;
	WIN32_FIND_DATA fd;
	HANDLE hSearch;
	bool fFinished = FALSE;
	_TCHAR fullmask[1024];
	_TCHAR pstFileFull[1024];
	UINT totalFiles = 0, totalFilesError = 0;

	if (inputDir == NULL || _tcslen(inputDir) == 0) {
		theParentApp->PrefixWithCurDir(_T(""), fullmask);
	}
	else {
		_tcscpy(fullmask, inputDir);
		_tcscat(fullmask, _T("/"));
	}
	_tcscat(fullmask, PSTMASK);

	INI_LogWriteWithResourcePrefix(IDS_LOG_FULLMASK, fullmask);

	if (theParentApp->IsSMTPLookupRequested()) {
		if (this->GetSmtpAddrFile() != NULL && _tcslen(this->GetSmtpAddrFile()) > 0)
			parseSMTPFile(this->GetSmtpAddrFile(), this->GetSmtpPartsSep());
	}

	if (this->GetOnlyPSTFile() != NULL && _tcslen(this->GetOnlyPSTFile()) > 0) {
		_TCHAR outputdir[512];
		_tcscpy(outputdir, this->GetOutputDir());
		_tcscat(outputdir, _T("/"));
		PerformOneMailboxMigration(NULL, NULL, this->GetOnlyPSTFile(), outputdir, &totalFiles, &totalFilesError);
	}
	else
	{
		// go through all files with .nsf extension in inputDir
		INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_OPEN, _T(""));
		hSearch = FindFirstFile(fullmask, &fd);
		if (hSearch != INVALID_HANDLE_VALUE) {
			while (!fFinished) { 
				if (fd.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_HIDDEN ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_COMPRESSED ||
					fd.dwFileAttributes == FILE_ATTRIBUTE_NORMAL) {
					_TCHAR outputdir[512];
					_tcscpy(outputdir, this->GetOutputDir());
					_tcscat(outputdir, _T("/"));
					if (inputDir == NULL || _tcslen(inputDir) == 0) {
						theParentApp->PrefixWithCurDir(_T(""), pstFileFull);
					}
					else {
						_tcscpy(pstFileFull, inputDir);
						_tcscat(pstFileFull, _T("/"));
					}
					_tcscat(pstFileFull, fd.cFileName);
					INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_FILE, _T(""));
//					_tprintf(_T("-->pstFileFull: %s\n"), pstFileFull);
					PerformOneMailboxMigration(NULL, NULL, pstFileFull, outputdir, &totalFiles, &totalFilesError);
				}
				if (!FindNextFile(hSearch, &fd)) {
					INI_LogWriteWithResourcePrefix(IDS_LOG_FIND_HANDLE_CLOSE, _T(""));
					FindClose(hSearch);
					fFinished = TRUE;
				}
			}
		}
	}

	_tprintf(_T("\n"));
	INI_LogWriteWithResourcePrefixNum(IDS_LOG_TOTAL_FILES_DONE, totalFiles);
	INI_LogWriteWithResourcePrefixNum(IDS_LOG_TOTAL_FILES_ERROR, totalFilesError);

	if (totalFilesError > 0)
		retCode = MIGRATION_SUCCESS_SOME_ERRORS;

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}
//-------------------------------------------------------
// Open standard Names.nsf database and process all Persons/Groups entries into LDIF format
int CPSTProc::PerformMigration(char *dbFullPath)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CPSTProc::PerformMigration");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	
	int retCode = MIGRATION_STEP_SUCCESS;
	HRESULT hr;

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

int CPSTProc::PerformOneMailboxMigration(LPMAPISESSION pSes, LPMDB pMdb, const _TCHAR *pstFile, const _TCHAR *outputdir,
										   UINT *totalFiles, UINT *totalFilesError)
{
	int retCode = MIGRATION_STEP_SUCCESS;
	_TCHAR usage[1024];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CPSTProc::PerformOneMailboxMigration");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
//	::_tprintf(_TEXT("-->outputdir: %s\n"), outputdir);
	HRESULT hr;
	LPMAPISESSION pses = NULL;
	LPMDB pmdb = NULL;

	UINT NumItems = 0;
	UINT totalMsgs = 0, totalMsgsDone = 0, 
		totalMsgsSkipped = 0, totalMsgsError = 0;

	CObjectArray resolvedFolders;
	int findex = 0;

	char field_text[256];
	char field_textUTF8[256];
	wchar_t field_textW[256];


	LIST *lstelem = NULL, *pstname;

	LPMAPIFOLDER lpfRootF = NULL;
	ULONG  ulObjType = 0;

	const _TCHAR *output_dir;
	bool local = (pstFile != NULL ? true : false);

	if (local) {
		::INI_LogWriteWithResourcePrefix(IDS_LOG_LOCAL_PSTFILE, (_TCHAR *)pstFile);
	}
	else {
		goto cleanup;
	}

    // Open the message file.

//	else {
//		// pstFile here is with path, drop it before passing
//		::INI_LogWriteWithResourcePrefix(IDS_LOG_NOTES_MAILFILE, _T("-->1"));
//		_tcscpy(firstLast, pstFile);
//		_TCHAR *pos = _tcsrchr(firstLast, _T('.'));
//		if (pos != NULL)
//			*pos = _T('\0');
//		pos =_tcsrchr(firstLast, _T('\\'));
//		if (pos != NULL)
//			_tcscpy(firstLast, pos + 1);
//		pos = _tcsrchr(firstLast, _T('/'));
//		if (pos != NULL)
//			_tcscpy(firstLast, pos + 1);
//
//		::INI_LogWriteWithResourcePrefix(IDS_LOG_NOTES_MAILFILE, _T("-->2"));
//		::INI_LogWriteWithResourcePrefix(IDS_LOG_NOTES_MAILFILE, firstLast);
//		// Retrieve FirstName/LastName/Domain from first Memo message in ($Sent) folder
//		RetrieveFirstLastDomainFromSent(hMessageFile, firstLast);
//		::INI_LogWriteWithResourcePrefix(IDS_LOG_NOTES_MAILFILE, _T("-->3"));
//	}

	output_dir = this->GetOutputDir();
	if (CDirUtils::MakeDir(output_dir) != NO_ERROR) {
		::LoadString(hInst, IDS_ERR_DIR_NOT_CREATED1, usage, 512);
		::_tprintf(usage, output_dir);
		goto cleanup;
	}

	if (pses == NULL || pmdb == NULL) {
		// First - delete same profile if exists
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECHECK, PROFILE_NAME);
		if (!DeleteProfileWithIProfAdmin(PROFILE_NAME)) {
			retCode = MIGRATION_ERR_FAILED;
			goto cleanup;
		}

		// Create MAPI profile
		pstname = listSuppl->LST_Lookup(&lst, ID_OPT_PSTFILENAME);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECR, PROFILE_NAME);
		if (!CreateProfileWithIProfAdmin(PROFILE_NAME, (_TCHAR *)pstFile)) {
			retCode = MIGRATION_ERR_FAILED;
			goto cleanup;
		}
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILECR, _T("-->Done"));
	}

	if (mailbox != NULL) {
		if (m_verbose) {
			::LoadString(hInst, IDS_PROCESSING_CHOICE, usage, sizeof(usage));
			::_tprintf(_TEXT("%s folders: %s\n"), usage, mailbox->GetFolders());
		}
	}

	if (pSes != NULL) {
		pses = pSes;
	}
	else {
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_INIT, _T("-->1"));
		MAPIInitialize(NULL);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_INIT, _T("-->2"));
//		printf("-->here 00\n");
#if defined(UNICODE)
		if (FAILED(hr = MAPILogonEx((ULONG)NULL, PROFILE_NAME, NULL,
			MAPI_UNICODE | MAPI_EXTENDED | MAPI_EXPLICIT_PROFILE |	MAPI_NEW_SESSION, &pses))) {
#else
		if (FAILED(hr = MAPILogonEx((ULONG)NULL, PROFILE_NAME, NULL,
			MAPI_EXTENDED | MAPI_EXPLICIT_PROFILE |	MAPI_NEW_SESSION, &pses))) {
#endif
//			item2 = LST_LookupAdd(&lst, ID_OPT_TEXT2, MAX_LINE);
//			LoadString(hInst, IDS_PROFILE_NOTOPEN, item2->data, MAX_LINE);
//				MessageBox(hDlg, PROFILE_NAME, item2->data, MB_OK);
			::INI_LogWriteWithResourcePrefix(IDS_PROFILE_NOTOPEN, PROFILE_NAME);
			return FALSE;
		}
	}

//	printf("-->here 01\n");

	if (pMdb != NULL) {
		pmdb = pMdb;
	}
	else {
//		printf("-->here 02\n");
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENSTORE, _T("-->1"));
		pmdb = OpenDefaultStore(pses);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENSTORE, _T("-->2"));

		// Open Root Folder first, i.e. "Personal Folders"
//							pmdb->lpVtbl->OpenEntry(pmdb, 0, (ULONG)NULL, NULL, MAPI_MODIFY, &ulObjType, (LPUNKNOWN FAR *)&lpfRootF);
//							spvMsg.ulPropTag = PR_DISPLAY_NAME;
//							spvMsg.Value.lpszA = "Renamed Folder";
//							HrSetOneProp((LPMAPIPROP)lpfRootF, &spvMsg);
//							ULRELEASE(lpfRootF);
	}

	// Open "Top Of Personal Folders" folder
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENFOLDER, _T("-->1"));
	lpfRootF = OpenMsgStoreFolderByName(pmdb, MAPI_FOLDER_TOP);
	INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_OPENFOLDER, _T("-->2"));

	if (hr = ResolveFolders(pses, pmdb, lpfRootF, MAPI_FOLDER_TOP, 
		(_TCHAR *)output_dir,
		resolvedFolders, 
		&totalMsgs, &totalMsgsDone, 
		&totalMsgsSkipped, &totalMsgsError)) {
//		MessageBoxW(NULL, L"ResolveFolders error", L"ResolveFolders", MB_OK);
		goto cleanup;
	}
	_tprintf(_T("\n"));
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_FILE, totalMsgs, (_TCHAR *)pstFile, true);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_FILE_DONE, totalMsgsDone, (_TCHAR *)pstFile, true);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_FILE_ERROR, totalMsgsError, (_TCHAR *)pstFile, true);

	if (local) {
		(*totalFiles) += 1;
		if (totalMsgsError > 0)
			(*totalFilesError) += 1;
	}

cleanup:
	ULRELEASE(lpfRootF);
	if (pSes == NULL && pses != NULL) {
		ULRELEASE(pmdb);
		pses->Logoff((ULONG)NULL, (ULONG)NULL, 0);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_LOGOFF, _T(""));

		ULRELEASE(pses);
		MAPIUninitialize();
		pmdb = NULL;
		pses = NULL;
	}
	if (pSes == NULL) {
		// Delete profile if still exists
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILEDELETED, _T("-->10"));
		DeleteProfileWithIProfAdmin(PROFILE_NAME);
		INI_LogWriteWithResourcePrefix(IDS_LOG_MAPI_PROFILEDELETED, _T("-->11"));
	}

	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s %d\n"), usage, current_method, retCode);
	}
	return retCode;
}

/* Local constants */

#define  MAX_ITEMS          20
#define  MAX_ITEM_LEN       1024
#define  DATATYPE_SIZE      sizeof(USHORT)
#define  ITEM_LENGTH_SIZE   sizeof(USHORT)

HRESULT CPSTProc::ResolveFolders(LPMAPISESSION pses, LPMDB pmdb, LPMAPIFOLDER lpfRootF, _TCHAR *rootFolderName, 
								  _TCHAR *output_dir,
								  CObjectArray& resolvedFolders, 
								  UINT *totalMsgs, UINT *totalMsgsDone, 
								  UINT *totalMsgsSkipped, UINT *totalMsgsError)
{
	HRESULT hr = S_OK;
	LPMAPITABLE lpTable = NULL;
	LPSRowSet lpRow = NULL;
	LPMAPITABLE lpChildTable = NULL;
	LPSRowSet lpChildRow = NULL;
	LPSPropValue lpRowProp = NULL;
	ULONG i = 0L;
	ULONG child = 0L;
	_TCHAR *TitleW, *containerClassW;
	_TCHAR folderToOpen[1024];
	LPMAPIFOLDER childF;

	static const enum {IDISPNAME, IENTRYID, ICONTCLASS};
	static SizedSPropTagArray( 3, rgColProps) = { 3, { 
		PR_DISPLAY_NAME_W, PR_ENTRYID, PR_CONTAINER_CLASS_W}};

	UINT totalFolderMsgs = 0, totalFolderMsgsDone = 0, 
		totalFolderMsgsSkipped = 0, totalFolderMsgsError = 0;

	//Open the public folder supplied as parameter using GetHierarchyTable
	hr = MAPICALL(lpfRootF)->GetHierarchyTable( MAPI_DEFERRED_ERRORS, &lpTable);
	if (FAILED(hr))
	{
		goto cleanup;
	}

	//Get the list of public folders using HrQueryAllRows
	hr = HrQueryAllRows( lpTable, (LPSPropTagArray)&rgColProps, NULL, NULL, 0L, &lpRow);
	if( FAILED( hr))
	{
		goto cleanup;
	}

	// Search rows for the folder in question
	for(i = 0; i < lpRow->cRows; i++)
	{
		lpRowProp = lpRow->aRow[i].lpProps;
		TitleW = lpRowProp[IDISPNAME].Value.lpszW;
		if (_tcscmp(rootFolderName, MAPI_FOLDER_TOP) != 0) {
			_tcscpy(folderToOpen, rootFolderName);
		}
		else {
			_tcscpy(folderToOpen, MAPI_FOLDER_TOP);
		}
		_tcscat(folderToOpen, _T("\\"));
		_tcscat(folderToOpen, TitleW);
		INI_LogWrite(_T("-->ResolveFolders check"));
		INI_LogWrite(folderToOpen);
		if (lpRowProp[ICONTCLASS].ulPropTag == PR_CONTAINER_CLASS) {
			_TCHAR buf[128];
			containerClassW = lpRowProp[ICONTCLASS].Value.lpszW;
			_stprintf(buf, _T("-->PR_CONTAINER_CLASS: %s"), containerClassW);
			INI_LogWrite(buf);
			if (_tcscmp(containerClassW, _T("IPM.Post")) != 0 &&
				_tcscmp(containerClassW, _T("IPF.Note")) != 0) {
				continue;
			}
		}

		if (IsIgnoredFolder((_TCHAR *)mailbox->GetIgnoredFolders(), TitleW)) {
			ProcessOneSkippedFolder(pmdb, folderToOpen, 
						&totalFolderMsgsSkipped);
			(*totalMsgs) += totalFolderMsgsSkipped;
//			printf("-->totalMsgs: %d, totalMsgsSkipped: %d\n", (*totalMsgs), (*totalMsgsSkipped));
			(*totalMsgsSkipped) += totalFolderMsgsSkipped;
			totalFolderMsgsSkipped = 0;
			continue;
		}

		INI_LogWrite(_T("-->ProcessOneFolder"));
		ProcessOneFolder(pses, pmdb, folderToOpen, output_dir, 
			&totalFolderMsgs, &totalFolderMsgsDone, &totalFolderMsgsSkipped,
			&totalFolderMsgsError);
		INI_LogWrite(_T("<--ProcessOneFolder"));

		(*totalMsgs) += totalFolderMsgs;
		(*totalMsgsDone) += totalFolderMsgsDone;
		(*totalMsgsSkipped) += totalFolderMsgsSkipped;
//		printf("-->totalMsgs: %d, totalMsgsSkipped: %d\n", (*totalMsgs), (*totalMsgsSkipped));
		(*totalMsgsError) += totalFolderMsgsError;
		totalFolderMsgs = 0;
		totalFolderMsgsDone = 0;
		totalFolderMsgsSkipped = 0;
		totalFolderMsgsError = 0;

		childF = OpenMsgStoreFolderByName(pmdb, folderToOpen);

		//Open the public folder supplied as parameter using GetHierarchyTable
		hr = MAPICALL(childF)->GetHierarchyTable( MAPI_DEFERRED_ERRORS, &lpChildTable);
		if (FAILED(hr))
		{
			goto cleanup;
		}

		//Get the list of public folders using HrQueryAllRows
		hr = HrQueryAllRows( lpChildTable, (LPSPropTagArray)&rgColProps, NULL, NULL, 0L, &lpChildRow);
		if( FAILED( hr))
		{
			goto cleanup;
		}

		_tprintf(_T("lpChildRow->cRows: %d\n"), lpChildRow->cRows);
		if (lpChildRow->cRows > 0) {
			INI_LogWrite(_T("-->ProcessSubfolders"));
			ResolveFolders(pses, pmdb, childF, folderToOpen, output_dir,
								resolvedFolders, 
								  totalMsgs, totalMsgsDone, 
								  totalMsgsSkipped, totalMsgsError);
			ULRELEASE(childF);
			INI_LogWrite(_T("<--ProcessSubfolders"));
		}
		FREEPROWS(lpChildRow);
		ULRELEASE(lpChildTable);
	} 

cleanup:
	FREEPROWS(lpRow);
	ULRELEASE(lpTable);

	return hr;
}

HRESULT CPSTProc::ProcessOneSkippedFolder(LPMDB pmdb, _TCHAR *folderName,
									UINT *totalFolderMsgsSkipped)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);

	HRESULT hr = S_OK;
	ULONG    items_found = 0;             	/* items of notes found */
	LPMAPIFOLDER skippedF;

	INI_LogWrite(_T("-->ProcessOneSkippedFolder"));
	INI_LogWrite(folderName);
	skippedF = OpenMsgStoreFolderByName(pmdb, folderName);

	static const enum {ICOUNT};
	static SizedSPropTagArray(1, cols) = { 1, {PR_CONTENT_COUNT} };
	ULONG pcount; 
	SPropValue *props;
	hr = skippedF->GetProps((SPropTagArray*)&cols, 0, &pcount, &props);
	if (FAILED(hr))
	{
		goto cleanup;
	}

	items_found = props[0].Value.ul;
	MAPIFreeBuffer(props);

	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_SKIPPED, items_found, folderName);
	if (m_verbose) {
		::LoadString(hInst, IDS_LOG_TOTAL_MSGS_SKIPPED, usage, sizeof(usage));
		::_tprintf(usage, folderName);
		::_tprintf(_TEXT(" %ld\n"), items_found);
	}

cleanup:
	INI_LogWrite(_T("<--ProcessOneSkippedFolder"));
	return hr;
}

HRESULT CPSTProc::ProcessOneFolder(LPMAPISESSION pSes, LPMDB pmdb, _TCHAR *folderName, _TCHAR *output_dir,
									UINT *totalFolderMsgs, UINT *totalFolderMsgsDone, 
									UINT *totalFolderMsgsSkipped, UINT *totalFolderMsgsError)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);

	HRESULT hr = S_OK;
	_TCHAR fullFolderName[_MAX_PATH];
	_TCHAR *fullFolderNamePtr = NULL;
	_TCHAR* folderToken = NULL, *ptrSlash = NULL;
	_TCHAR fullFolderNamePath[_MAX_PATH];
	_TCHAR folderTokenAdj[512];

	ULONG    items_found = 0;             	/* items of notes found */
	UINT NumItemsTotal = 0, NumItemsError = 0, NumItemsSkipped = 0;
	LPMAPIFOLDER folderF;
	LPMAPITABLE lpContentsTable = NULL;

	FILE *f;
	_TCHAR *ptr = NULL;

	folderF = OpenMsgStoreFolderByName(pmdb, folderName);

	static const enum {IENTRYID};
	static SizedSPropTagArray( 1, rgColProps) = { 1, { 
		PR_ENTRYID}};
	LPSRowSet lpRow = NULL;
	LPSPropValue lpRowProp = NULL;
	ULONG i = 0L;
	ULONG ulObjType;
	LPMESSAGE lpMsg = NULL;
	ULONG cbEid = 0;          // number of bytes in entry identifier
	LPENTRYID lpEid = NULL;

	static const enum {ICOUNT};
	static SizedSPropTagArray(1, cols) = { 1, {PR_CONTENT_COUNT} };
	ULONG pcount; 
	SPropValue *props;
	hr = folderF->GetProps((SPropTagArray*)&cols, 0, &pcount, &props);
	if (FAILED(hr))
	{
		goto cleanup;
	}

	items_found = props[0].Value.ul;
	MAPIFreeBuffer(props);

	// We assume here that folderName passed is in form of
	// token1\token2\token3\...
	// If upper token is not created as folder, create it first.
	// Drop "Top of Personal Folders" if present
	ptr = _tcsstr(folderName, MAPI_FOLDER_TOP);
	if (ptr != NULL)
		_tcscpy(fullFolderName, ptr + _tcslen(MAPI_FOLDER_TOP) + 1);
	else
		_tcscpy(fullFolderName, folderName);

	// Change folder name if specified via conversion
	theParentApp->resetConversionRuleIndex();
	CConversionRuleEntry *conversionEntry;
	while ((conversionEntry = theParentApp->getNextConversionRuleEntry(MAILBOX_FOLDERS_SELECTOR)) != NULL) {
		if (_tcscmp(conversionEntry->GetInput(), fullFolderName) == 0) {
			_tcscpy(fullFolderName, conversionEntry->GetOutput());
			delete(conversionEntry);
			break;
		}
		delete(conversionEntry);
	}


	// Don't use _tcstok here, cause MakeDir uses it and nested strtok are not possible!!!
	_tcscpy(fullFolderNamePath, output_dir);
	ptrSlash = _tcschr(fullFolderName, _TEXT('\\'));
	fullFolderNamePtr = fullFolderName;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// IMPORTANT! For Mozilla Thunderbird we need to create folders with .sbd extensions!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (ptrSlash == NULL) {
		folderToken = fullFolderNamePtr;
//		_tprintf(_T("-->folderToken: %s\n"), folderToken);
		for (unsigned int idx = 0; idx < _tcslen(folderToken); idx++) {
			if (folderToken[idx] == _T('/') || folderToken[idx] == _T('\\') ||
				folderToken[idx] == _T(':') || folderToken[idx] == _T('*') ||
				folderToken[idx] == _T('?') || folderToken[idx] == _T('|') ||
				folderToken[idx] == _T('<') || folderToken[idx] == _T('>'))
			folderToken[idx] = _T('_');
		}
		if (CDirUtils::MakeDir(fullFolderNamePath) != NO_ERROR) {
			::LoadString(hInst, IDS_ERR_DIR_NOT_CREATED1, usage, 512);
			::_tprintf(usage, fullFolderNamePath);
			goto cleanup;
		}
	}
	else {
		while(ptrSlash != NULL) {
			*ptrSlash = _T('\0');
			folderToken = fullFolderNamePtr;
			_tcscpy(folderTokenAdj, folderToken);
			folderToken = folderTokenAdj;
			// Check here if we need to convert folderToken, i.e. eliminate "/", "\"
			for (unsigned int idx = 0; idx < _tcslen(folderToken); idx++) {
				if (folderToken[idx] == _T('/') || folderToken[idx] == _T('\\') ||
					folderToken[idx] == _T(':') || folderToken[idx] == _T('*') ||
					folderToken[idx] == _T('?') || folderToken[idx] == _T('|') ||
					folderToken[idx] == _T('<') || folderToken[idx] == _T('>'))
				folderToken[idx] = _T('_');
			}

			// It's a subfolder, so append!
			_tcscat(folderToken, _TEXT(".sbd"));
//				_tprintf(_T("-->fullFolderNamePath: %s\n"), fullFolderNamePath);
//				_tprintf(_T("-->folderToken: %s\n"), folderToken);

			if (CDirUtils::MakeDir2(fullFolderNamePath, folderToken) != NO_ERROR) {
				::LoadString(hInst, IDS_ERR_DIR_NOT_CREATED2, usage, 512);
				::_tprintf(usage, fullFolderNamePath, folderToken);
				goto cleanup;
			}
			_tcscat(fullFolderNamePath, _TEXT("/"));
			_tcscat(fullFolderNamePath, folderToken);
			ptrSlash = ptrSlash + 1;
			fullFolderNamePtr = ptrSlash;
			ptrSlash = _tcschr(ptrSlash, _TEXT('\\'));
		}
		folderToken = fullFolderNamePtr;
	}

	//_tprintf(_T("-->fullFolderNamePath: %s, folderToken: %s\n"), fullFolderNamePath, folderToken);
	f = CDirUtils::OpenFile(fullFolderNamePath, NULL, folderToken, _TEXT("w+b"));
	if (f == NULL) {
		::LoadString(hInst, IDS_ERR_FILE_NOT_CREATED2, usage, 512);
		::_tprintf(usage, fullFolderNamePath, folderToken);
		goto cleanup;
	}
#ifndef DONT_USE_OUTPUT_UNICODE
	_ftprintf(f, _TEXT("%c"), 0xfeff);
#endif

	if (items_found > 0) {
		// Go through messages
		if(FAILED(hr = folderF->GetContentsTable(0L, &lpContentsTable)))
		{
			goto cleanup;
		}

		hr = HrQueryAllRows( lpContentsTable, (LPSPropTagArray)&rgColProps, NULL, NULL, 0L, &lpRow);
		if( FAILED( hr))
		{
			goto cleanup;
		}

//		printf("lpRow->cRows: %d\n", lpRow->cRows);
		for(i=0; i < lpRow->cRows; i++)
		{
			lpRowProp = lpRow->aRow[i].lpProps;
//			TitleW = lpRowProp[IDISPNAME].Value.lpszW;
//			printf("%s\n",lpRow->aRow[i].lpProps[1].Value.lpszA);
			cbEid = lpRowProp[IENTRYID].Value.bin.cb;
//			printf("cbEid: %d\n", cbEid);
			lpEid = (LPENTRYID)lpRowProp[IENTRYID].Value.bin.lpb; //Value.bin.cb = cbEid;
//			hr = folderF->OpenEntry(cbEid, lpEid, NULL, MAPI_MODIFY, 
//				&ulObjType, (LPUNKNOWN FAR *)&lpMsg);
			hr = folderF->OpenEntry(cbEid, lpEid, NULL, MAPI_MODIFY, 
				&ulObjType, (LPUNKNOWN FAR *)&lpMsg);
			printf("OpenEntry: %lx, %lx\n", hr, lpMsg);
			if( FAILED( hr))
			{
				goto cleanup;
			}
			if (lpMsg) {
				INI_LogWrite(_T("-->ProcessOneMessage"));
				hr = ProcessOneMessage(lpMsg, f);
				INI_LogWrite(_T("<--ProcessOneMessage"));
			}
			ULRELEASE(lpMsg);
		}

	}

	ptr = _tcsstr(folderName, MAPI_FOLDER_TOP);
	if (ptr != NULL)
		_tcscpy(fullFolderName, ptr + _tcslen(MAPI_FOLDER_TOP) + 1);
	else
		_tcscpy(fullFolderName, folderName);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS, items_found, fullFolderName);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_DONE, NumItemsTotal, fullFolderName);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_SKIPPED, NumItemsSkipped, fullFolderName);
	INI_LogWriteWithResourcePrefixNumP(IDS_LOG_TOTAL_MSGS_ERROR, NumItemsError, fullFolderName);
	if (m_verbose) {
		::LoadString(hInst, IDS_LOG_TOTAL_MSGS, usage, sizeof(usage));
		::_tprintf(usage, fullFolderName);
		::_tprintf(_TEXT(" %ld\n"), items_found);
		::LoadString(hInst, IDS_LOG_TOTAL_MSGS_DONE, usage, sizeof(usage));
		::_tprintf(usage, fullFolderName);
		::_tprintf(_TEXT(" %ld\n"), NumItemsTotal);
		::LoadString(hInst, IDS_LOG_TOTAL_MSGS_SKIPPED, usage, sizeof(usage));
		::_tprintf(usage, fullFolderName);
		::_tprintf(_TEXT(" %ld\n"), NumItemsSkipped);
		::LoadString(hInst, IDS_LOG_TOTAL_MSGS_ERROR, usage, sizeof(usage));
		::_tprintf(usage, fullFolderName);
		::_tprintf(_TEXT(" %ld\n"), NumItemsError);
	}

cleanup:
	if (f != NULL) {
		fclose(f);
	}

	return hr;
}

HRESULT CPSTProc::ProcessOneMessage(LPMESSAGE lpMsg, FILE *outputFile)
{
	HRESULT hr = S_OK;

	CMessageItem* item;
	theParentApp->resetEntryListIndex();
	theParentApp->InitializeMessageItemList();

	INI_LogWrite(_T("-->Process message items"));
	while ((item = theParentApp->GetNextMessageItem()) != NULL) {
		const _TCHAR* condition = item->GetCondition();

		if (condition != NULL && _tcslen(condition) > 0 && 
			!CNotesCondition::IsNotesCondition(lpMsg, condition, &hr)) {
			delete(item);
			continue;
		}
		ApplyTransformers(lpMsg, item->GetName(), item->GetValue(), outputFile);
		delete(item);
	}
	INI_LogWrite(_T("<--Process message items"));

	// now process message body
	INI_LogWrite(_T("-->ProcessBody"));
	hr = ProcessBody(lpMsg, outputFile);
	INI_LogWrite(_T("<--ProcessBody"));
	if (FAILED(hr)) {
		goto cleanup;
	}

	// now process file attachments
	INI_LogWrite(_T("-->ProcessAttachments"));
	hr = ProcessAttachments(lpMsg, outputFile);
	INI_LogWrite(_T("<--ProcessAttachments"));

cleanup:
	return hr;
}

HRESULT CPSTProc::ApplyTransformers(LPMESSAGE lpMsg,  const _TCHAR *name, 
	const _TCHAR *value, FILE *outputFile)
{
#ifdef DONT_USE_OUTPUT_UNICODE
	char valueCh[1024];
	char propertyCh[512];
#endif

	HRESULT hr = S_OK;
	WORD field_len = 0;

	_TCHAR formattedValue[512];
	bool ignoreOutput = false;
	int timesTransformed = 0;

	_TCHAR inputValue[512];
	_TCHAR* ptr = inputValue;
	_tcscpy(inputValue, value);
	_TCHAR* ptrprev = inputValue;
	CEntryTransformer *transformerEntry;
	CConversionRuleEntry *conversionEntry;

	memset(formattedValue, 0, sizeof(formattedValue)*sizeof(TCHAR));
	while((transformerEntry = theParentApp->getNextEntryTransformer()) != NULL) {
		int offset = 0;
		int valueLen = _tcslen(inputValue);
		bool eatNextComma = false;

		const _TCHAR* transformerName = transformerEntry->GetName();
		const _TCHAR* placeholder = transformerEntry->GetPlaceholder();
		int placeholderLen = _tcslen(placeholder);
		_tprintf(_T("-->transformerName: %s, placeholder: %s<--\n"), transformerName, placeholder);

		while ((ptr = _tcsstr(ptr, placeholder)) != NULL) {
			_tcsncpy(formattedValue+offset, ptrprev, ptr - ptrprev);
			offset += (ptr - ptrprev);
			int placeholderValueLen = 0;

//			_tprintf(_T("-->at1\n"));
			if (transformerEntry->GetLotusDocField()) {
				_TCHAR outputW[512];

//				_tprintf(_T("-->at2\n"));
				ULONG xmlPropTag = GetXMLPropTagValue(transformerName);
//				static const enum {IPROP};
				SizedSPropTagArray(1, cols) = { 1, {xmlPropTag} };
				ULONG pcount = 0; 
				LPSPropValue props = NULL;
				hr = lpMsg->GetProps((LPSPropTagArray)&cols, MAPI_UNICODE, 
					&pcount, &props);

				_tprintf(_T("-->at29: %d, hr: %lx<--\n"), pcount, hr);
				if (hr == S_OK && pcount > 0) {
					_tprintf(_T("-->at3 %lx\n"), props[0].ulPropTag);
//					if (props[0].Value.lpszW != NULL) {
//						_tprintf(_T("-->at31 Uni: %s<--\n"), props[0].Value.lpszW);
//					}
//					else if (props[0].Value.lpszA != NULL) {
//						printf("-->at31 Asc: %s<--\n", props[0].Value.lpszA);
//					}
					switch (LOWORD(props[0].ulPropTag)) {
						case PT_STRING8:
							{
								mbstowcs(outputW, props[0].Value.lpszA, strlen(props[0].Value.lpszA));
								outputW[strlen(props[0].Value.lpszA)] = _T('\0');
								field_len = _tcslen(outputW);
							}
							break;
						case PT_UNICODE:
							{
								_tcscpy(outputW, props[0].Value.lpszW);
								field_len = _tcslen(outputW);
							}
							break;
						case PT_SYSTIME:
							{
								SYSTEMTIME systemTime;
								FileTimeToSystemTime(&props[0].Value.ft, &systemTime);

//#define DATEMASK_DWK_ALL			_T("DWK, DD MMM YYYY HH:MI:SS TZHH")
//#define DATEMASK_DWK2				_T("DWK MMM DD HH:MI:SS YYYY")
								if (transformerEntry->GetDateMask() != NULL &&
									_tcscmp(transformerEntry->GetDateMask(), DATEMASK_DWK_ALL) == 0) {

									_TCHAR datePart[64];
									_TCHAR timePart[64];
									GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
										_T("ddd', 'd MMM yyyy"), datePart, sizeof(datePart));
									GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
										_T(" HH':'mm':'ss"), timePart, sizeof(timePart));
									_tcscpy(outputW, datePart);
									_tcscat(outputW, timePart);
									field_len = _tcslen(outputW);
								}
								else if (transformerEntry->GetDateMask() != NULL &&
									_tcscmp(transformerEntry->GetDateMask(), DATEMASK_DWK2) == 0) {

									_TCHAR datePart1[32];
									_TCHAR datePart2[32];
									_TCHAR timePart[64];

									if (systemTime.wDay < 10)
										GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
											_T("ddd MMM  d"), datePart1, sizeof(datePart1));
									else
										GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
											_T("ddd MMM d"), datePart1, sizeof(datePart1));

									GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
										_T(" yyyy"), datePart2, sizeof(datePart2));
									GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, 
										_T(" HH':'mm':'ss"), timePart, sizeof(timePart));
									_tcscpy(outputW, datePart1);
									_tcscat(outputW, timePart);
									_tcscat(outputW, datePart2);
									field_len = _tcslen(outputW);
								}
							}
							break;
						default:
							{
								field_len = _tcslen(_T("test")); //props[0].Value.lpszW);
								_tcscpy(outputW, _T("test")); //props[0].Value.lpszW);
							}
					}

					const _TCHAR *valueModifier = NULL;
					if (field_len > 0 &&
						(LOWORD(props[0].ulPropTag) == PT_STRING8 || LOWORD(props[0].ulPropTag) == PT_UNICODE) &&
						(valueModifier = transformerEntry->GetValueModifier()) != NULL) {
						if (_tcscmp(transformerEntry->GetValueModifier(), MODIFIER_BASE64) == 0) {
						}
						else if (_tcscmp(transformerEntry->GetValueModifier(), MODIFIER_SMTPLOOKUP) == 0) {
							LIST *item;
							_TCHAR usage[256];
							HINSTANCE hInst = ::GetModuleHandle(NULL);
							if (m_verbose) {
								::LoadString(hInst, IDS_SMTP_NAME_FOR_LOOKUP, usage, 256);
								::_tprintf(_TEXT("%s %s\n"), usage, outputW);
							}

							_TCHAR field_textW[128];
							_tcscpy(field_textW, outputW);

							item = smtpAdr->LST_Lookup(&notFoundLst, outputW);
							if (item != NULL) {
								if (m_verbose) {
									::LoadString(hInst, IDS_SMTP_NAME_UNRESOLVED, usage, 256);
									::_tprintf(_TEXT("%s %s\n"), usage, outputW);
								}
							}
							else {
								item = smtpAdr->LST_Lookup(&lst, outputW, false);
								if (item != NULL) {
									_tcscpy(outputW, item->data);
									field_len = _tcslen(outputW);
									if (m_verbose) {
										::LoadString(hInst, IDS_SMTP_NAME_SUBSTITUTED, usage, 256);
										::_tprintf(_TEXT("%s %s\n"), usage, outputW);
									}
								}
								else {
									if (m_verbose) {
										::LoadString(hInst, IDS_SMTP_NAME_ADDTOUNRESOLVED, usage, 256);
										::_tprintf(_TEXT("%s %s\n"), usage, outputW);
									}
									smtpAdr->LST_LookupAdd(&notFoundLst, outputW, ENTRY_LEN);
								}
							}
						}
					}

					// now check if any lotus field conversion is needed
					theParentApp->resetConversionRuleIndex();
					while ((conversionEntry = theParentApp->getNextConversionRuleEntry()) != NULL) {
						if (!conversionEntry->IsDefault()) {
							// not case sensitive right now !!!
	//						_tprintf(_TEXT("-->conversion: %s %s\n"), outputW, conversionEntry->GetInput());
							if (_tcsicmp(outputW, conversionEntry->GetInput()) == 0) {
								_tcscpy(outputW, conversionEntry->GetOutput());
								delete(conversionEntry);
								break;
							}
						}
						delete(conversionEntry);
					}
					
					placeholderValueLen = _tcslen(outputW);
					if (placeholderValueLen == 0) {
						eatNextComma = true;
					}
					_tcsncpy(formattedValue+offset, outputW, placeholderValueLen);
					offset+=placeholderValueLen;
				}
				else {
//					_tprintf(_T("-->at4\n"));
					offset+=placeholderValueLen;
				}
			}
			else {
				const _TCHAR* configSettingValue = NULL;
				configSettingValue = theParentApp->getConfigSettingValue(transformerName);
				placeholderValueLen = _tcslen(configSettingValue);
				_tcsncpy(formattedValue+offset, configSettingValue, placeholderValueLen);
				offset+=placeholderValueLen;
			}
			timesTransformed++;
			ptr = ptr + placeholderLen;
			ptrprev = ptr;
		}
		// remove comma or space after empty value
		if (ptrprev != _T('\0') && _tcslen(ptrprev)>0) {
			if ((*ptrprev == _T(',') && (eatNextComma || offset == 0)) ||
				(*ptrprev == _T(' ') && (eatNextComma || offset == 0))) {
				_tcsncpy(formattedValue+offset, ptrprev+1, _tcslen(ptrprev+1));
				*(formattedValue+offset+_tcslen(ptrprev+1)) = _T('\0');
			}
			else {
				_tcsncpy(formattedValue+offset, ptrprev, _tcslen(ptrprev));
				*(formattedValue+offset+_tcslen(ptrprev)) = _T('\0');
			}
		}
		else {
			*(formattedValue+offset) = _T('\0');
		}
		if (transformerEntry->IsBlankIfAbsent() && _tcscmp(formattedValue, _TEXT("=\?utf-8?B\?\?=")) == 0) {
			*(formattedValue) = _T('\0');
		}
		_tcscpy(inputValue, formattedValue);
		ptr = inputValue;
		ptrprev = ptr;
		delete(transformerEntry);
	}

//		_tprintf(_T("-->at5\n"));
		if (!ignoreOutput) {
			if (timesTransformed == 0) {
//				_ftprintf(f, mask2, attribute, value);
				if (name == NULL || _tcslen(name) == 0) {
#ifndef DONT_USE_OUTPUT_UNICODE
					_ftprintf(outputFile, _TEXT("%s\r\n"), value);
#else
					wcstombs(valueCh, value, sizeof(valueCh));
					fprintf(outputFile, "%s\r\n", valueCh);
#endif
				}
				else {
					if (_tcscmp(name, _T("Cc")) == 0) {
						// skip it
					}
					else {
#ifndef DONT_USE_OUTPUT_UNICODE
						_ftprintf(outputFile, _TEXT("%s: %s\r\n"), name, value);
#else
						wcstombs(valueCh, value, sizeof(valueCh));
						wcstombs(propertyCh, name, sizeof(propertyCh));
						fprintf(outputFile, "%s: %s\r\n", propertyCh, valueCh);
#endif
					}
				}
			}
			else {
//				if (m_verbose){
//					::LoadString(hInst, IDS_PROCESSING_CHOICE, usage, 256);
//					::_tprintf(_TEXT("%s formattedValue: %s\n"), usage, formattedValue);
//				}
//				_ftprintf(f, mask2, attribute, formattedValue);
				if (name == NULL || _tcslen(name) == 0) {
#ifndef DONT_USE_OUTPUT_UNICODE
					_ftprintf(outputFile, _TEXT("%s\r\n"), formattedValue);
#else
					wcstombs(valueCh, formattedValue, sizeof(valueCh));
					fprintf(outputFile, "%s\r\n", valueCh);
#endif
				}
				else {
#ifndef DONT_USE_OUTPUT_UNICODE
					_ftprintf(outputFile, _TEXT("%s: %s\r\n"), name, formattedValue);
#else
					wcstombs(valueCh, formattedValue, sizeof(valueCh));
					wcstombs(propertyCh, name, sizeof(propertyCh));
					fprintf(outputFile, "%s: %s\r\n", propertyCh, valueCh);
#endif
				}
			}
		}

cleanup:
	return hr;
}

void CPSTProc::FilterFolders(CObjectArray& resolvedFolders, _TCHAR const *requestedFolders)
{
	wchar_t requestedFoldersW[1024];
	int startIndex = 0;
	int i = 0;
	int j = 0;
//	for (j = 0; j<resolvedFolders.getLength(); j++) {
//		_tprintf(_TEXT("j: %d, GetName(): %s\n"), j, resolvedFolders.getElemAt(j).GetName());
//	}
	while(resolvedFolders.getLength()> 0 && startIndex < resolvedFolders.getLength()) {
		_tcscpy(requestedFoldersW, requestedFolders);
		bool matched = false;
		_TCHAR* folderToken = _tcstok(requestedFoldersW, L",");
		while (folderToken != NULL) {
//			_tprintf(L"folderToken: %s, elem: %s %s\n", folderToken, 
//				resolvedFolders.getElemAt(i).GetName(),
//				resolvedFolders.getElemAt(i).GetContentValue());
			if (_tcscmp(folderToken, resolvedFolders.getElemAt(i).GetName()) == 0) {
//				_tprintf(L"Matched! Process it! %d\n", i);
				matched = true;
				startIndex = i + 1;
				i++;
				break;
			}
			folderToken = _tcstok(NULL, L",");
		}
		if (!matched) {
//			_tprintf(L"Not matched! Remove it: %d!\n", i);
			resolvedFolders.removeElemAt(i);
			startIndex = i;
//			if (startIndex < resolvedFolders.getLength()) {
//				_tprintf(L"After removal: %s\n", resolvedFolders.getElemAt(i).GetName());
//			}
		}
	}

}

void CPSTProc::DoSmtpLookup(_TCHAR *field_text, WORD *field_len) 
{
//	char field_textUTF8[512];
	// we could have here Internet email, instead of Lotus DN
	_TCHAR field_text_orig[128];
	_tcscpy(field_text_orig, field_text);
	if (field_text[0] == '<') {
		_tcscpy(field_text, field_text_orig + 1);
		int len = _tcslen(field_text);
		if (field_text[len - 1] == '>')
			field_text[len - 1] = '\0';
	}
	if (this->GetSmtpAddrFile() != NULL && _tcslen(this->GetSmtpAddrFile()) > 0) {
		LIST *item;
		_TCHAR usage[256];
		HINSTANCE hInst = ::GetModuleHandle(NULL);
		if (m_verbose) {
			::LoadString(hInst, IDS_SMTP_NAME_FOR_LOOKUP, usage, 256);
			::_tprintf(_TEXT("%s "), usage);
			printf("%s\n", field_text);
		}

		_TCHAR field_textW[128];
//	#ifndef OUTLOOK_UNICODE
//		OSTranslate(OS_TRANSLATE_LMBCS_TO_UTF8, field_text, strlen(field_text), field_textUTF8, 128);
//		mbstowcs(field_textW, field_textUTF8, 128);
//	//							::_tprintf(_T("-->1 %s<--\n"), field_textW);
//	#else
//		OSTranslate(OS_TRANSLATE_LMBCS_TO_UNICODE, field_text, strlen(field_text), field_textUTF8, 128);
//		_tcscpy(field_textW, (_TCHAR *)field_textUTF8);
//	//							::_tprintf(_T("-->2 %s<--\n"), field_textW);
//	#endif
		_tcscpy(field_textW, field_text);

		item = smtpAdr->LST_Lookup(&notFoundLst, field_textW);
		if (item != NULL) {
			if (m_verbose) {
				::LoadString(hInst, IDS_SMTP_NAME_UNRESOLVED, usage, 256);
				::_tprintf(_TEXT("%s "), usage);
				printf("%s\n", field_text);
			}
		}
		else {
			item = smtpAdr->LST_Lookup(&lst, field_textW, false);
			if (item != NULL) {
				_tcscpy(field_textW, item->data);
//				wcstombs(field_text, field_textW, _tcslen(field_textW));
//				field_text[_tcslen(field_textW)] = '\0';
				_tcscpy(field_text, field_textW);
				*field_len = _tcslen(item->data);
				if (m_verbose) {
					::LoadString(hInst, IDS_SMTP_NAME_SUBSTITUTED, usage, 256);
					::_tprintf(_TEXT("%s %s\n"), usage, field_textW);
				}
			}
			else {
				if (m_verbose) {
					::LoadString(hInst, IDS_SMTP_NAME_ADDTOUNRESOLVED, usage, 256);
					::_tprintf(_TEXT("%s %s\n"), usage, field_textW);
				}
				smtpAdr->LST_LookupAdd(&notFoundLst, field_textW, ENTRY_LEN);
			}
		}
	}

}

bool CPSTProc::IsIgnoredFolder(_TCHAR *ignoredFolders, _TCHAR *folderName)
{
	_TCHAR allIgnoredFolders[1024];
	_tcscpy(allIgnoredFolders, ignoredFolders);
	_TCHAR* folderToken = _tcstok(allIgnoredFolders, _T(","));
	bool ignored = false;
	while (folderToken != NULL) {
		if (_tcscmp(folderToken, folderName) == 0) {
			ignored = true;
			break;
		}
		folderToken = _tcstok(NULL, _T(","));
	}
	return ignored;
}

void CPSTProc::parseSMTPFile(const _TCHAR *smtpAddrFile, const _TCHAR *smtpPartsSep)
{
	_TCHAR usage[256];
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	_TCHAR current_method[] = _TEXT("CPSTProc::parseSMTPFile");
	if (m_verbose){
		::LoadString(hInst, IDS_ENTER_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
	smtpAdr = new CList();
	FILE *fin = NULL;
	_TCHAR buf[BUFFER_SIZE];
	_TCHAR entry[ENTRY_LEN];
	int bread;
	unsigned long offset = 0;
	int found = 0;
	_TCHAR *ptr1, *ptr2, *ptr3;
	_TCHAR delimPartsSep[32];
	_TCHAR delimLineBreak[] = _T("\r\n");
	int lend1 = 0;
	int lend2 = 0;
	unsigned long line = 1;
	_TCHAR end = _T('\0');
	LIST *item;

	if (smtpAddrFile == NULL || _tcslen(smtpAddrFile) == 0) {
		INI_LogWriteWithResourcePrefix(IDS_LOG_SMTPADR_IGNOREEMPTY, _T(""));
		goto cleanup;
	}

//	wcstombs(smtpAddrFileCh, smtpAddrFile, _tcslen(smtpAddrFile));
//	smtpAddrFileCh[_tcslen(smtpAddrFile)] = '\0';
//	wcstombs(delimPartsSep, smtpPartsSep, _tcslen(smtpPartsSep));
//	delimPartsSep[_tcslen(smtpPartsSep)] = '\0';
	delimPartsSep[0] = _T('\0');
	_tcscpy(delimPartsSep, smtpPartsSep);
	lend1 = _tcslen(delimPartsSep);
	lend2 = _tcslen(delimLineBreak);
	
	fin = _tfopen(smtpAddrFile, _T("rb"));
	if (fin == NULL) {
		_tprintf(_T("Error opening file: %s"), smtpAddrFile);
		goto cleanup;
	}

	/*
	ptr1 - pointer to the begin of our two entries
	ptr2 - pointer to the end of our two entries
	ptr3 - pointer to the SMTP part of entries
	*/
	entry[0] = end;
	while (!feof(fin)) {
		bread = fread(buf, 1, BUFFER_SIZE - 1, fin);
		if (bread == 0)
			break;
		offset += bread;
		buf[bread] = end;
		ptr1 = buf;
#ifdef _UNICODE
		if (line == 1) {
			if (buf[0] == 0xfeff)
				ptr1 = buf + 1;
			else {
				::LoadString(hInst, IDS_LOG_SMTPADR_SKIP, usage, 256);
				_tprintf(_T("%s\n"), usage);
				INI_LogWriteWithResourcePrefix(IDS_LOG_SMTPADR_SKIP, _T(""));
				fclose(fin);
				return;
			}
		}
#endif
		ptr3 = _tcsstr(ptr1, delimLineBreak);
		if (ptr3 == NULL) {
			if (bread >= ENTRY_LEN - 1 || offset >= ENTRY_LEN - 1) {
				// Error
				found = 0;
				break;
			}
			else {
				_tcscat(entry, buf);
				ptr3 = _tcsstr(entry, delimLineBreak);
				if (ptr3 != NULL) {
					entry[ptr3-entry] = end;
					ptr2 = _tcsstr(entry, delimPartsSep);
					if ((ptr2 != NULL) && (_tcslen(ptr2 + lend1) != 0)) {
//						fseek(fin, (int)((ptr3 - entry) - offset + lend2), SEEK_CUR);
						fseek(fin, (long)(((byte *)ptr3 - (byte *)entry) - offset + 
							((byte *)(delimLineBreak + lend2) - (byte *)delimLineBreak)), SEEK_CUR);
#ifdef _UNICODE
						if (buf[0] == 0xfeff)
							fseek(fin, 2, SEEK_CUR);
#endif
						offset = 0;
						found = 1;
						line++;
						*(ptr1+(ptr2-entry)) = end;
						entry[0] = end;
						item = smtpAdr->LST_LookupAdd(&lst, ptr1, ENTRY_LEN);
						if (item != NULL) {
							_tcscpy(item->data, ptr2 + lend1);
							_tprintf(_T("key: %s, value: %s\n"), ptr1, ptr2 + lend1);
						}
					}
					else {
						found = 0;
						::LoadString(hInst, IDS_ERR_SMTP_NO_DELIM, usage, 256);
						::_tprintf(_TEXT("%s %s"), usage, entry);
						fseek(fin, (long)(((byte *)ptr3 - (byte *)entry) - offset + 
							((byte *)(delimLineBreak + lend2) - (byte *)delimLineBreak)), SEEK_CUR);
#ifdef _UNICODE
						if (buf[0] == 0xfeff)
							fseek(fin, 2, SEEK_CUR);
#endif						
						offset = 0;
					}
				}
			}
		}
		else {
			if (ptr3 - ptr1 >= ENTRY_LEN - 1) {
				// Error
				found = 0;
				break;
			}
			else {
				*(ptr1 + (ptr3 - ptr1)) = end;
				_tcscat(entry, ptr1);
				ptr2 = _tcsstr(entry, delimPartsSep);
				if ((ptr2 != NULL) && (_tcslen(ptr2 + lend1) != 0)) {
					fseek(fin, (long)(((byte *)ptr3 - (byte *)ptr1) - bread + ((byte *)(delimLineBreak + lend2) - (byte *)delimLineBreak)), SEEK_CUR);
#ifdef _UNICODE
					if (buf[0] == 0xfeff)
						fseek(fin, 2, SEEK_CUR);
#endif
					offset = 0;
					found = 1;
					line++;
					*(ptr1+(ptr2-entry)) = end;
					entry[0] = end;
					item = smtpAdr->LST_LookupAdd(&lst, ptr1, ENTRY_LEN);
					if (item != NULL) {
						_tcscpy(item->data, ptr2 + lend1);
						_tprintf(_T("key: %s, value: %s\n"), ptr1, ptr2 + lend1);
					}
				}
				else {
					found = 0;
					::LoadString(hInst, IDS_ERR_SMTP_NO_DELIM, usage, 256);
					::_tprintf(_TEXT("%s %s"), usage, entry);
					fseek(fin, (long)(((byte *)ptr3 - (byte *)ptr1) - bread + ((byte *)(delimLineBreak + lend2) - (byte *)delimLineBreak)), SEEK_CUR);
#ifdef _UNICODE
					if (buf[0] == 0xfeff)
						fseek(fin, 2, SEEK_CUR);
#endif						
					offset = 0;
				}
			}
		}
	}

cleanup:
	if (fin != NULL)
		fclose(fin);
	if (m_verbose){
		::LoadString(hInst, IDS_EXIT_METHOD, usage, 256);
		::_tprintf(_TEXT("%s %s\n"), usage, current_method);
	}
}

ULONG CPSTProc::GetXMLPropTagValue(const _TCHAR *propTag)
{
	ULONG found = 0;
	for (int i=0; i<sizeof(PropTags)/sizeof(PropTag); i++) {
		if (_tcscmp((PropTags + i)->prop, propTag) == 0) {
			found = (PropTags + i)->value;
			break;
		}
	}
	// If we need to set special not well known MAPI properties
	if (!found) {
		_tprintf(_T("-->before tcstol: %s\n"), propTag);
		ULONG l1 = (ULONG)_tcstol(propTag, NULL, 0);
		_tprintf(_T("-->after tcstol: %u\n"), l1);
		if (l1 > 0) found = l1;
	}
	return found;
}

HRESULT CPSTProc::ProcessAttachments(LPMESSAGE lpMsg, FILE* outputFile)
{
	HRESULT hr = S_OK;

	INI_LogWrite(_T("-->PutFileHeader"));
	PutFileHeader(lpMsg, outputFile);
	INI_LogWrite(_T("<--PutFileHeader"));

	INI_LogWrite(_T("-->PutFilesFooter"));
	PutFilesFooter(lpMsg, outputFile);
	INI_LogWrite(_T("<--PutFilesFooter"));

	return hr;
}

HRESULT CPSTProc::ProcessBody(LPMESSAGE lpMsg, FILE* outputFile)
{
	HRESULT hr = S_OK;
	bool attachmentsExists = false;

	LPSTREAM pStream=NULL;
	LPSTREAM pUnStream=NULL;
	LPSTREAM lpStream = NULL;

	if (attachmentsExists) {
		INI_LogWrite(_T("-->PutTextHeader"));
		PutTextHeader(lpMsg, outputFile);
		INI_LogWrite(_T("<--PutTextHeader"));
	}

	hr = lpMsg->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream,
			STGM_READ, MAPI_MODIFY,
			(LPUNKNOWN FAR *) &pStream);

	hr = WrapCompressedRTFStream(pStream, MAPI_MODIFY |
		STORE_UNCOMPRESSED_RTF, &pUnStream);

#if defined(UNICODE)
	hr = lpMsg->OpenProperty(PR_BODY,
			&IID_IStream, STGM_READ, MAPI_MODIFY, (LPUNKNOWN *)&lpStream);
#else
	hr = lpMsg->OpenProperty(PR_BODY,
			&IID_IStream, 0, MAPI_MODIFY, (LPUNKNOWN *)&lpStream);
#endif

	unsigned char buf[8192];
	unsigned char buf64[16384];
	ULONG cb = sizeof(buf);
	ULONG cbRead = cb;
	while (cbRead == cb) {
		hr = lpStream->Read(buf, cb, &cbRead);
		to64buf2((const unsigned char *)buf, cbRead, (unsigned char *)buf64, 0);

	}

	pUnStream->Release();

	pStream->Release();

	UlRelease(lpStream);

	return hr;
}

HRESULT CPSTProc::PutTextHeader(LPMESSAGE lpMsg, FILE *outputFile)
{
	HRESULT hr = S_OK;
	CMessageItem *item;
	while ((item = theParentApp->GetNextMessageItem(CONDITION_TEXTPART)) != NULL) {
		ApplyTransformers(lpMsg, item->GetName(), item->GetValue(), outputFile);
		delete(item);
	}
	return hr;
}

HRESULT CPSTProc::PutFileHeader(LPMESSAGE lpMsg, FILE *outputFile)
{
	HRESULT hr = S_OK;
	CMessageItem *item;
	// We need to put everything with condition="AttachmentPart"
	while ((item = theParentApp->GetNextMessageItem(CONDITION_ATTACHMENTPART)) != NULL) {
		ApplyTransformers(lpMsg, item->GetName(), item->GetValue(), outputFile);
		delete(item);
	}
	return hr;
}

HRESULT CPSTProc::PutFilesFooter(LPMESSAGE lpMsg, FILE *outputFile)
{
	HRESULT hr = S_OK;
	CMessageItem *item;
	// We need to put everything with condition="AttachmentPart"
	while ((item = theParentApp->GetNextMessageItem(CONDITION_ENDOFPARTS)) != NULL) {
		ApplyTransformers(lpMsg, item->GetName(), item->GetValue(), outputFile);
		delete(item);
	}
	return hr;
}
