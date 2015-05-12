// EntryTransformer.hpp: interface for the CEntryTransformer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTRYTRANSFORMER_HPP__FF5439F9_FA39_4E7B_B41F_F4CBD3A5E3D6__INCLUDED_)
#define AFX_ENTRYTRANSFORMER_HPP__FF5439F9_FA39_4E7B_B41F_F4CBD3A5E3D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef NMAILXML_EXPORTS
#define NMAILXML_API __declspec(dllexport)
#else
#define NMAILXML_API __declspec(dllimport)
#endif

class CEntryTransformer  
{
public:
	CEntryTransformer();
	virtual ~CEntryTransformer();

	// Get/Set m_name
	NMAILXML_API _TCHAR const * GetName();
	NMAILXML_API void SetName(const _TCHAR *name);

	// Get/Set m_placeholder
	NMAILXML_API _TCHAR const * GetPlaceholder();
	NMAILXML_API void SetPlaceholder(const _TCHAR *placeholder);

	// Get/Set m_resolveDocField
	NMAILXML_API _TCHAR const * GetResolveDocField();
	NMAILXML_API void SetResolveDocField(const _TCHAR *resolveDocField);

	// Get/Set m_resolveDocForm
	NMAILXML_API _TCHAR const * GetResolveDocForm();
	NMAILXML_API void SetResolveDocForm(const _TCHAR *resolveDocForm);

	// Get/Set m_resolveDocLookupField
	NMAILXML_API _TCHAR const * GetResolveDocLookupField();
	NMAILXML_API void SetResolveDocLookupField(const _TCHAR *resolveDocLookupField);

	// Get/Set m_lotusDocField
	NMAILXML_API bool const GetLotusDocField();
	NMAILXML_API void SetLotusDocField(const bool lotusDocField);

	// Get/Set m_multivalue
	NMAILXML_API bool const IsMultivalue();
	NMAILXML_API void SetMultivalue(const bool multivalue);

	// Get/Set m_onlyFirstValue
	NMAILXML_API bool const GetOnlyFirstValue();
	NMAILXML_API void SetOnlyFirstValue(const bool onlyFirstValue);

	// Get/Set m_multivalueSeparator
	NMAILXML_API _TCHAR const * GetMultivalueSeparator();
	NMAILXML_API void SetMultivalueSeparator(const _TCHAR *multivalueSeparator);

	// Get/Set m_orgHierarchy
	NMAILXML_API bool const GetOrgHierarchy();
	NMAILXML_API void SetOrgHierarchy(const bool orgHierarchy);

	// Get/Set m_extractCN
	NMAILXML_API bool const GetExtractCN();
	NMAILXML_API void SetExtractCN(const bool extractCN);

	// Get/Set m_dateMask
	NMAILXML_API _TCHAR const * GetDateMask();
	NMAILXML_API void SetDateMask(const _TCHAR *dateMask);

	// Get/Set m_valueModifier
	NMAILXML_API bool IsConversionRequired();
	NMAILXML_API void SetConversionRequired(bool conversion);

	// Get/Set m_valueModifier
	NMAILXML_API _TCHAR const * GetValueModifier();
	NMAILXML_API void SetValueModifier(const _TCHAR *valueModifier);

	// Get/Set m_blankIfAbsent
	NMAILXML_API bool IsBlankIfAbsent();
	NMAILXML_API void SetBlankIfAbsent(bool wrap);

protected:
	bool m_lotusDocField;
	_TCHAR m_name[128];
	_TCHAR m_placeholder[256];
	_TCHAR m_resolveDocField[128];
	_TCHAR m_resolveDocForm[128];
	_TCHAR m_resolveDocLookupField[128];
	bool m_multivalue;
	bool m_onlyFirstValue;
	_TCHAR m_multivalueSeparator[64];
	bool m_orgHierarchy;
	bool m_extractCN;
	_TCHAR m_dateMask[64];
	bool m_conversion;
	_TCHAR m_valueModifier[64];
	bool m_blankIfAbsent;
};

#endif // !defined(AFX_ENTRYTRANSFORMER_HPP__FF5439F9_FA39_4E7B_B41F_F4CBD3A5E3D6__INCLUDED_)
