// Mailbox.h: interface for the CMailbox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILBOX_H__16563A3F_26DC_4EF9_8EDB_6431B7CED278__INCLUDED_)
#define AFX_MAILBOX_H__16563A3F_26DC_4EF9_8EDB_6431B7CED278__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef NMAILXML_EXPORTS
#define NMAILXML_API __declspec(dllexport)
#else
#define NMAILXML_API __declspec(dllimport)
#endif

class CMailbox  
{
public:
	CMailbox();
	virtual ~CMailbox();

	// Get/Set m_folders
	NMAILXML_API _TCHAR const * GetFolders();
	NMAILXML_API void SetFolders(const _TCHAR *folders);

	// Get/Set m_ignoredFolders
	NMAILXML_API _TCHAR const * GetIgnoredFolders();
	NMAILXML_API void SetIgnoredFolders(const _TCHAR *folders);

	// Get/Set m_views
	NMAILXML_API _TCHAR const * GetViews();
	NMAILXML_API void SetViews(const _TCHAR *views);

	// Get/Set m_dateFilterField
	NMAILXML_API _TCHAR const * GetDateFilterField();
	NMAILXML_API void SetDateFilterField(const _TCHAR *dateFilterField);

	// Get/Set m_dateFilterAfter
	NMAILXML_API _TCHAR const * GetDateFilterAfter();
	NMAILXML_API void SetDateFilterAfter(const _TCHAR *dateFilterAfter);

	// Get/Set m_dateFilterBefore
	NMAILXML_API _TCHAR const * GetDateFilterBefore();
	NMAILXML_API void SetDateFilterBefore(const _TCHAR *dateFilterBefore);

	// Get/Set m_dateMask
	NMAILXML_API _TCHAR const * GetDateMask();
	NMAILXML_API void SetDateMask(const _TCHAR *dateMask);

protected:
	_TCHAR m_folders[1024];
	_TCHAR m_ignoredFolders[1024];
	_TCHAR m_views[1024];
	_TCHAR m_dateFilterField[64];
	_TCHAR m_dateFilterAfter[64];
	_TCHAR m_dateFilterBefore[64];
	_TCHAR m_dateMask[64];
};

#define FOLDER_NAME_SPACE _T("#x20")

#endif // !defined(AFX_MAILBOX_H__16563A3F_26DC_4EF9_8EDB_6431B7CED278__INCLUDED_)
