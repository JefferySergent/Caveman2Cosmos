//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfoWater.cpp
//
//  AUTHOR:	
//					
//					
//
//  PURPOSE: The base class for all info classes to inherit from.  
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvWaterPlaneInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWaterPlaneInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWaterPlaneInfo::CvWaterPlaneInfo() :
	m_fMaterialAlpha(0.0f),
	m_BaseTextureScale(0.0f),
	m_fURate(0.0f),
	m_fVRate(0.0f)
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWaterPlaneInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWaterPlaneInfo::~CvWaterPlaneInfo()
{
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getMaterialAlpha() const		// The water plane's material alpha
{
	return m_fMaterialAlpha;
}
//------------------------------------------------------------------------------------------------------
NiColor	CvWaterPlaneInfo::getMaterialDiffuse() const	// The water plane's material diffuse color
{
	return m_kMaterialDiffuse;
}
//------------------------------------------------------------------------------------------------------
NiColor	CvWaterPlaneInfo::getMaterialSpecular() const// The water plane's material specular color
{
	return m_kMaterialSpecular;
}
//------------------------------------------------------------------------------------------------------
NiColor	CvWaterPlaneInfo::getMaterialEmmisive() const// The water plane's material emmisive color
{
	return m_kMaterialEmmisive;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
void CvWaterPlaneInfo::setBaseTexture(const TCHAR* szVal)			// The filename of the base texture
{
	m_szBaseTexture=szVal;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvWaterPlaneInfo::getTransitionTexture() const
{
	return m_szTransitionTexture;
}
//------------------------------------------------------------------------------------------------------
void CvWaterPlaneInfo::setTransitionTexture(const TCHAR* szVal)		// The filename of the detail texture
{
	m_szTransitionTexture=szVal;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScaling() const
{
	return m_BaseTextureScale;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScrollRateU() const
{
	return m_fURate;
}
//------------------------------------------------------------------------------------------------------
float CvWaterPlaneInfo::getTextureScrollRateV() const
{
	return m_fVRate;
}
//------------------------------------------------------------------------------------------------------
bool CvWaterPlaneInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	if (pXML->TryMoveToXmlFirstChild(L"WaterMaterial"))
	{
		if (pXML->TryMoveToXmlFirstChild(L"MaterialColors"))
		{
			if (pXML->TryMoveToXmlFirstChild(L"DiffuseMaterialColor"))
			{
				pXML->GetChildXmlValByName( &m_kMaterialDiffuse.r, L"r");
				pXML->GetChildXmlValByName( &m_kMaterialDiffuse.g, L"g");
				pXML->GetChildXmlValByName( &m_kMaterialDiffuse.b, L"b");
				pXML->MoveToXmlParent();
			}
			if (pXML->TryMoveToXmlFirstChild(L"SpecularMaterialColor"))
			{
				pXML->GetChildXmlValByName( &m_kMaterialSpecular.r, L"r");
				pXML->GetChildXmlValByName( &m_kMaterialSpecular.g, L"g");
				pXML->GetChildXmlValByName( &m_kMaterialSpecular.b, L"b");
				pXML->MoveToXmlParent();
			}
			if (pXML->TryMoveToXmlFirstChild(L"EmmisiveMaterialColor"))
			{
				pXML->GetChildXmlValByName( &m_kMaterialEmmisive.r, L"r");
				pXML->GetChildXmlValByName( &m_kMaterialEmmisive.g, L"g");
				pXML->GetChildXmlValByName( &m_kMaterialEmmisive.b, L"b");
				pXML->MoveToXmlParent();
			}
			pXML->MoveToXmlParent();
		}

		pXML->GetChildXmlValByName( &m_fMaterialAlpha, L"MaterialAlpha");

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"WaterTextures"))
	{
		if (pXML->TryMoveToXmlFirstChild(L"WaterBaseTexture"))
		{
			pXML->GetChildXmlValByName( szTextVal, L"TextureFile");
			setBaseTexture(szTextVal);

			pXML->GetChildXmlValByName( &m_BaseTextureScale, L"TextureScaling");
			pXML->GetChildXmlValByName( &m_fURate, L"URate");
			pXML->GetChildXmlValByName( &m_fVRate, L"VRate");

			pXML->MoveToXmlParent();

		}
		if (pXML->TryMoveToXmlFirstChild(L"WaterTransitionTexture"))
		{
			pXML->GetChildXmlValByName( szTextVal, L"TextureFile");
			setTransitionTexture(szTextVal);
		}
		
		pXML->MoveToXmlParent();
	}

	pXML->MoveToXmlParent();
	return true;
}
/************************************************************************************************/
/* XMLCOPY                                 11/20/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvWaterPlaneInfo::copyNonDefaults(CvWaterPlaneInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	// This is a bogus CopyNonDefault. Modders/Users should set the XML completely with all tags
}
/************************************************************************************************/
/* XMLCOPY                                 END                                                  */
/************************************************************************************************/
//------------------------------------------------------------------------------------------------------

//======================================================================================================
//					CvTerrainPlaneInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainPlaneInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainPlaneInfo::CvTerrainPlaneInfo() :
	m_bVisible(false),
	m_bGroundPlane(false),
	m_fMaterialAlpha(0.0f),
	m_fCloseAlpha(0.0f),
	m_BaseTextureScaleU(0.0f),
	m_BaseTextureScaleV(0.0f),
	m_fURate(0.0f),
	m_fVRate(0.0f),
	m_fZHeight(0.0f),
	m_eFogType(FOG_TYPE_NONE)
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainPlaneInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainPlaneInfo::~CvTerrainPlaneInfo()
{
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::isVisible() const
{
	return m_bVisible;
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::isGroundPlane() const
{
	return m_bGroundPlane;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getMaterialAlpha() const		// The water plane's material alpha
{
	return m_fMaterialAlpha;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getCloseAlpha() const		// The water plane's material alpha
{
	return m_fCloseAlpha;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvTerrainPlaneInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
void CvTerrainPlaneInfo::setBaseTexture(const TCHAR* szVal)			// The filename of the base texture
{
	m_szBaseTexture=szVal;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScalingU() const
{
	return m_BaseTextureScaleU;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScalingV() const
{
	return m_BaseTextureScaleV;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScrollRateU() const
{
	return m_fURate;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getTextureScrollRateV() const
{
	return m_fVRate;
}
//------------------------------------------------------------------------------------------------------
float CvTerrainPlaneInfo::getZHeight() const
{
	return m_fZHeight;
}
//------------------------------------------------------------------------------------------------------
FogTypes CvTerrainPlaneInfo::getFogType() const
{
	return m_eFogType;
}
//------------------------------------------------------------------------------------------------------
bool CvTerrainPlaneInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName( &m_bVisible, L"bVisible");
	pXML->GetChildXmlValByName( &m_bGroundPlane, L"bGroundPlane");
	pXML->GetChildXmlValByName( &m_fMaterialAlpha, L"MaterialAlpha");
	pXML->GetChildXmlValByName( &m_fCloseAlpha, L"CloseAlpha");

	pXML->GetChildXmlValByName( szTextVal, L"TextureFile");
	setBaseTexture(szTextVal);

	pXML->GetChildXmlValByName( &m_BaseTextureScaleU, L"TextureScalingU");
	pXML->GetChildXmlValByName( &m_BaseTextureScaleV, L"TextureScalingV");
	pXML->GetChildXmlValByName( &m_fURate, L"URate");
	pXML->GetChildXmlValByName( &m_fVRate, L"VRate");
	pXML->GetChildXmlValByName( &m_fZHeight, L"ZHeight");

	pXML->GetChildXmlValByName( szTextVal, L"FogType");
	if(szTextVal.CompareNoCase("FOG_TYPE_NONE") == 0)
		m_eFogType = FOG_TYPE_NONE;
	else if(szTextVal.CompareNoCase("FOG_TYPE_PARALLEL") == 0)
		m_eFogType = FOG_TYPE_PARALLEL;
	else if(szTextVal.CompareNoCase("FOG_TYPE_PROJECTED") == 0)
		m_eFogType = FOG_TYPE_PROJECTED;
	else
	{
		FAssertMsg(false, "[Jason] Unknown fog type.");
		m_eFogType = FOG_TYPE_NONE;
	}

	return true;
}
/************************************************************************************************/
/* XMLCOPY                                 11/20/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvTerrainPlaneInfo::copyNonDefaults(CvTerrainPlaneInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1	
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isVisible() == bDefault) m_bVisible = pClassInfo->isVisible();
	if (isGroundPlane() == bDefault) m_bGroundPlane = pClassInfo->isGroundPlane();
	if (getMaterialAlpha() == fDefault) m_fMaterialAlpha = pClassInfo->getMaterialAlpha();
	if (getCloseAlpha() == fDefault) m_fCloseAlpha = pClassInfo->getCloseAlpha();

	if (getBaseTexture() == cDefault) setBaseTexture(pClassInfo->getBaseTexture());

	if (getTextureScalingU() == fDefault) m_BaseTextureScaleU = pClassInfo->getTextureScalingU();
	if (getTextureScalingV() == fDefault) m_BaseTextureScaleV = pClassInfo->getTextureScalingV();
	if (getTextureScrollRateU() == fDefault) m_fURate = pClassInfo->getTextureScrollRateU();
	if (getTextureScrollRateV() == fDefault) m_fVRate = pClassInfo->getTextureScrollRateV();
	if (getZHeight() == fDefault) m_fZHeight = pClassInfo->getZHeight();

	if ( getFogType() == FOG_TYPE_NONE )
	{
		m_eFogType = pClassInfo->getFogType();
	}
}
/************************************************************************************************/
/* XMLCOPY                                 END                                                  */
/************************************************************************************************/
//------------------------------------------------------------------------------------------------------

//======================================================================================================
//					CvCameraOverlayInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCameraOverlayInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCameraOverlayInfo::CvCameraOverlayInfo() :
	m_bVisible(false),
	m_eCameraOverlayType(CAMERA_OVERLAY_DECAL)
{

}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCameraOverlayInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCameraOverlayInfo::~CvCameraOverlayInfo()
{
}
//------------------------------------------------------------------------------------------------------
bool CvCameraOverlayInfo::isVisible() const
{
	return m_bVisible;
}
//------------------------------------------------------------------------------------------------------
const TCHAR * CvCameraOverlayInfo::getBaseTexture() const
{
	return m_szBaseTexture;
}
//------------------------------------------------------------------------------------------------------
void CvCameraOverlayInfo::setBaseTexture(const TCHAR* szVal)			// The filename of the base texture
{
	m_szBaseTexture = szVal;
}
//------------------------------------------------------------------------------------------------------
CameraOverlayTypes CvCameraOverlayInfo::getCameraOverlayType() const
{
	return m_eCameraOverlayType;
}
//------------------------------------------------------------------------------------------------------
bool CvCameraOverlayInfo::read(CvXMLLoadUtility* pXML)
{
	CvString  szTextVal;
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName( &m_bVisible, L"bVisible");
	
	pXML->GetChildXmlValByName( szTextVal, L"TextureFile");
	setBaseTexture(szTextVal);

	pXML->GetChildXmlValByName( szTextVal, L"CameraOverlayType");
	if(szTextVal.CompareNoCase("CAMERA_OVERLAY_DECAL") == 0)
		m_eCameraOverlayType = CAMERA_OVERLAY_DECAL;
	else if(szTextVal.CompareNoCase("CAMERA_OVERLAY_ADDITIVE") == 0)
		m_eCameraOverlayType = CAMERA_OVERLAY_ADDITIVE;
	else
	{
		FAssertMsg(false, "[Jason] Unknown camera overlay type.");
		m_eCameraOverlayType = CAMERA_OVERLAY_DECAL;
	}

	return true;
}
/************************************************************************************************/
/* XMLCOPY                                 11/20/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvCameraOverlayInfo::copyNonDefaults(CvCameraOverlayInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1	
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isVisible() == bDefault) m_bVisible = pClassInfo->isVisible();
	if (getBaseTexture() == cDefault) setBaseTexture(pClassInfo->getBaseTexture());

	if ( getCameraOverlayType() == CAMERA_OVERLAY_DECAL )
	{
		m_eCameraOverlayType = pClassInfo->getCameraOverlayType();
	}	
}
/************************************************************************************************/
/* XMLCOPY                                 END                                                  */
/************************************************************************************************/
//------------------------------------------------------------------------------------------------------
