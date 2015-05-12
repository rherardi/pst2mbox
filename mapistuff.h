#ifndef _MAPISTUFF_H_
#define _MAPISTUFF_H_

#include <mapix.h>
#include <mapiutil.h>
#include <edk.h>
//#include <edkmapi.h>
//#include <edkmdb.h>

// Important folder
#define MAPI_FOLDER_INBOX _T("Inbox")
#define MAPI_FOLDER_TOP _T("Top of Personal Folders")
#define MAPI_FOLDER_SENT_ITEMS _T("Sent Items")
#define MAPI_FOLDER_SENT_OUTBOX _T("Outbox")
#define MAPI_FOLDER_DELETED_ITEMS _T("Deleted Items")
#define MAPI_FOLDER_DRAFTS _T("Drafts")

#define PROFILE_NAME _T("PST2MBOX profile")
#define PST_DEFAULT_SUFFIX _T(".pst")

// From Outlook Spy tool
#define PR_FOLDER_DN_W 0x6700001F

//#ifdef __cplusplus
//extern "C" {
//#endif
	
BOOL CreateProfileWithIProfAdmin(_TCHAR *szProfile, _TCHAR *szDataFile);
BOOL DeleteProfileWithIProfAdmin(_TCHAR *szProfile);
LPMDB OpenDefaultStore(LPMAPISESSION pses);
LPMAPIFOLDER OpenMsgStoreFolderByName(LPMDB	lpMDB, LPTSTR lpszFolder);
BOOL DeleteFolder(LPMAPIFOLDER lpfRootF, _TCHAR *szSubfolderName);

//#ifdef __cplusplus
//} /* closing brace for extern "C" */
//#endif

#endif
