// Mailbox.cpp: implementation of the CMailbox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mailbox.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailbox::CMailbox()
{

}

CMailbox::~CMailbox()
{

}

// Get/Set m_folders
_TCHAR const * CMailbox::GetFolders()
{
/*
	if (m_folders[0] != '\0') {
		_TCHAR* ptr = m_folders;
		_TCHAR* token = _tcsstr(m_folders, FOLDER_NAME_SPACE);
		if (token == NULL) {
			return &m_folders[0];
		}
		else {
			_TCHAR normalizedFolders[1024];
			while(token != NULL) {
				_tcsncat(normalizedFolders, ptr, token - ptr);
				ptr = token + _tcslen(FOLDER_NAME_SPACE);
//				_tprintf(_T("token: %s, ptr: %s\n"), token, ptr);
				token = _tcsstr(ptr, FOLDER_NAME_SPACE);
				if (token != NULL) {
					_tcscat(normalizedFolders, _T(" "));
				}
			}
			_tcscat(normalizedFolders, ptr);
			return &normalizedFolders[0];
		}
	}
*/
  return &m_folders[0];
}

void CMailbox::SetFolders(const _TCHAR *folders)
{
	if (folders != NULL) {
		::_tcscpy(m_folders, folders);
	}
}

_TCHAR const * CMailbox::GetIgnoredFolders()
{
  return &m_ignoredFolders[0];
}

void CMailbox::SetIgnoredFolders(const _TCHAR *folders)
{
	if (folders != NULL) {
		::_tcscpy(m_ignoredFolders, folders);
	}
}

// Get/Set m_views
_TCHAR const * CMailbox::GetViews()
{
	return &m_views[0];
}
void CMailbox::SetViews(const _TCHAR *views)
{
	if (views != NULL) {
		::_tcscpy(m_views, views);
	}
}

// Get/Set m_dateFilterField
_TCHAR const * CMailbox::GetDateFilterField()
{
	return &m_dateFilterField[0];
}

void CMailbox::SetDateFilterField(const _TCHAR *dateFilterField)
{
	if (dateFilterField != NULL) {
		::_tcscpy(m_dateFilterField, dateFilterField);
	}
}

// Get/Set m_dateFilterAfter
_TCHAR const * CMailbox::GetDateFilterAfter()
{
	return &m_dateFilterAfter[0];
}

void CMailbox::SetDateFilterAfter(const _TCHAR *dateFilterAfter)
{
	if (dateFilterAfter != NULL) {
		::_tcscpy(m_dateFilterAfter, dateFilterAfter);
	}
}

// Get/Set m_dateFilterBefore
_TCHAR const * CMailbox::GetDateFilterBefore()
{
	return &m_dateFilterBefore[0];
}

void CMailbox::SetDateFilterBefore(const _TCHAR *dateFilterBefore)
{
	if (dateFilterBefore != NULL) {
		::_tcscpy(m_dateFilterBefore, dateFilterBefore);
	}
}

// Get/Set m_dateMask
_TCHAR const * CMailbox::GetDateMask()
{
	return &m_dateMask[0];
}

void CMailbox::SetDateMask(const _TCHAR *dateMask)
{
	if (dateMask != NULL) {
		::_tcscpy(m_dateMask, dateMask);
	}
}
