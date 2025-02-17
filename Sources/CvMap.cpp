//
//	FILE:	 CvMap.cpp
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvGameCoreDLL.h"
#include "CvGameCoreUtils.h"
#include "CvMapGenerator.h"
#include "CvFractal.h"

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
#include <direct.h>			// for getcwd()
#include <stdlib.h>			// for MAX_PATH
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

// Public Functions...

CvMap::CvMap()
{
}

CvMap::CvMap(MapTypes eType) /* Parallel Maps */ : m_eType(eType)
{
	OutputDebugString("Calling constructor for Map: Start");

	CvMapInitData defaultMapData;

	m_paiNumBonus = NULL;
	m_paiNumBonusOnLand = NULL;

	m_pMapPlots = NULL;

	m_bCitiesDisplayed = true;
	m_bUnitsDisplayed = true;

	reset(&defaultMapData);

	OutputDebugString("Calling constructor for Map: End");

}


CvMap::~CvMap()
{
	uninit();
}

// FUNCTION: init()
// Initializes the map.
// Parameters:
//	pInitInfo					- Optional init structure (used for WB load)
// Returns:
//	nothing.
void CvMap::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	OutputDebugString("Initializing Map: Start");

	int iX, iY;

	PROFILE("CvMap::init");
	gDLL->logMemState( CvString::format("CvMap::init begin - world size=%s, climate=%s, sealevel=%s, num custom options=%6", 
		GC.getWorldInfo(GC.getInitCore().getWorldSize()).getDescription(), 
		GC.getClimateInfo(GC.getInitCore().getClimate()).getDescription(), 
		GC.getSeaLevelInfo(GC.getInitCore().getSeaLevel()).getDescription(),
		GC.getInitCore().getNumCustomMapOptions()).c_str() );

	PYTHON_CALL_FUNCTION2(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "beforeInit");

	//--------------------------------
	// Init saved data
	reset(pInitInfo);

	//--------------------------------
	// Init containers
	m_areas.init();

	//--------------------------------
	// Init non-saved data
	setup();

	//--------------------------------
	// Init other game data
	gDLL->logMemState("CvMap before init plots");
	m_pMapPlots = new CvPlot[numPlotsINLINE()];
	for (iX = 0; iX < getGridWidthINLINE(); iX++)
	{
		gDLL->callUpdater();
		for (iY = 0; iY < getGridHeightINLINE(); iY++)
		{
			plotSorenINLINE(iX, iY)->init(iX, iY);
		}
	}
	calculateAreas();
	gDLL->logMemState("CvMap after init plots");

	OutputDebugString("Initializing Map: End");
}


void CvMap::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumBonus);
	SAFE_DELETE_ARRAY(m_paiNumBonusOnLand);

	SAFE_DELETE_ARRAY(m_pMapPlots);

	m_areas.uninit();

	for(int iI = 0; iI < (int)m_viewports.size(); iI++)
	{
		delete m_viewports[iI];
	}

	m_viewports.clear();
	m_iCurrentViewportIndex = -1;
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	//
	// set grid size
	// initially set in terrain cell units
	//
	m_iGridWidth = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridWidth (): 0;	//todotw:tcells wide
	m_iGridHeight = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridHeight (): 0;

	// allow grid size override
	if (pInitInfo)
	{
		m_iGridWidth	= pInitInfo->m_iGridW;
		m_iGridHeight	= pInitInfo->m_iGridH;
	}
	else
	{
		// check map script for grid size override
		if (GC.getInitCore().getWorldSize() != NO_WORLDSIZE)
		{

			std::vector<int> out;
			CyArgsList argsList;
			argsList.add(GC.getInitCore().getWorldSize());
			bool ok = PYTHON_CALL_FUNCTION4(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "getGridSize", argsList.makeFunctionArgs(), &out);

			if (ok && !gDLL->getPythonIFace()->pythonUsingDefaultImpl() && out.size() == 2)
			{
				m_iGridWidth = out[0];
				m_iGridHeight = out[1];
				FAssertMsg(m_iGridWidth > 0 && m_iGridHeight > 0, "the width and height returned by python getGridSize() must be positive");
			}
		}

		// convert to plot dimensions
		if (GC.getNumLandscapeInfos() > 0)
		{
			m_iGridWidth *= GC.getLandscapeInfo(GC.getActiveLandscapeID()).getPlotsPerCellX();
			m_iGridHeight *= GC.getLandscapeInfo(GC.getActiveLandscapeID()).getPlotsPerCellY();
		}
	}

	m_iLandPlots = 0;
	m_iOwnedPlots = 0;

	if (pInitInfo)
	{
		m_iTopLatitude = pInitInfo->m_iTopLatitude;
		m_iBottomLatitude = pInitInfo->m_iBottomLatitude;
	}
	else
	{
		// Check map script for latitude override (map script beats ini file)

		long resultTop = -1, resultBottom = -1;
		bool okX = PYTHON_CALL_FUNCTION4(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "getTopLatitude", NULL, &resultTop);
		bool overrideX = !gDLL->getPythonIFace()->pythonUsingDefaultImpl();
		bool okY = PYTHON_CALL_FUNCTION4(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "getBottomLatitude", NULL, &resultBottom);
		bool overrideY = !gDLL->getPythonIFace()->pythonUsingDefaultImpl();

		if (okX && okY && overrideX && overrideY && resultTop != -1 && resultBottom != -1)
		{
			m_iTopLatitude = resultTop;
			m_iBottomLatitude = resultBottom;
		}
	}

	m_iTopLatitude = std::min(m_iTopLatitude, 90);
	m_iTopLatitude = std::max(m_iTopLatitude, -90);
	m_iBottomLatitude = std::min(m_iBottomLatitude, 90);
	m_iBottomLatitude = std::max(m_iBottomLatitude, -90);

	m_iNextRiverID = 0;

	//
	// set wrapping
	//
	m_bWrapX = true;
	m_bWrapY = false;
	if (pInitInfo)
	{
		m_bWrapX = pInitInfo->m_bWrapX;
		m_bWrapY = pInitInfo->m_bWrapY;
	}
	else
	{
		// Check map script for wrap override (map script beats ini file)

		long resultX = -1, resultY = -1;
		bool okX = PYTHON_CALL_FUNCTION4(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "getWrapX", NULL, &resultX);
		bool overrideX = !gDLL->getPythonIFace()->pythonUsingDefaultImpl();
		bool okY = PYTHON_CALL_FUNCTION4(__FUNCTION__, gDLL->getPythonIFace()->getMapScriptModule(), "getWrapY", NULL, &resultY);
		bool overrideY = !gDLL->getPythonIFace()->pythonUsingDefaultImpl();

		if (okX && okY && overrideX && overrideY && resultX != -1 && resultY != -1)
		{
			m_bWrapX = (resultX != 0);
			m_bWrapY = (resultY != 0);
		}
	}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	//Koshling - why do we ignore the map size in MapInfos if there is only 1???  Changed that for now
	//if (GC.getNumMapInfos() > 1)
	if (GC.multiMapsEnabled() && GC.getMapInfos().size() > 0)
	{
		if (GC.getMapInfo(getType()).getGridWidth() > 0 && GC.getMapInfo(getType()).getGridHeight() > 0)
		{
			m_iGridWidth = GC.getMapInfo(getType()).getGridWidth();
			m_iGridHeight = GC.getMapInfo(getType()).getGridHeight();
		}
		if (GC.getMapInfo(getType()).getWrapX() > 0 && GC.getMapInfo(getType()).getWrapY() > 0)
		{
			m_bWrapX = (GC.getMapInfo(getType()).getWrapX()) ? true : false;
			m_bWrapY = (GC.getMapInfo(getType()).getWrapY()) ? true : false;
		}
	}
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	if (GC.getNumBonusInfos())
	{
		FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvMap::reset");
		FAssertMsg(m_paiNumBonus==NULL, "mem leak m_paiNumBonus");
		m_paiNumBonus = new int[GC.getNumBonusInfos()];
		FAssertMsg(m_paiNumBonusOnLand==NULL, "mem leak m_paiNumBonusOnLand");
		m_paiNumBonusOnLand = new int[GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiNumBonus[iI] = 0;
			m_paiNumBonusOnLand[iI] = 0;
		}
	}

	m_areas.removeAll();

	//	Create a viewport of the requisite external size without initial positioning (this can be repositioned
	//	at any time before it is graphically initialised, of after with a map switch)
	setCurrentViewport(addViewport(-1,
								   -1,
								   false));

	if ( !GC.viewportsEnabled() )
	{
		//	If no viewports are exposed there will only ever be one, and it's exactly aligned to the map
		getCurrentViewport()->setMapOffset(0, 0);
	}
}


// FUNCTION: setup()
// Initializes all data that is not serialized but needs to be initialized after loading.
void CvMap::setup()
{
	PROFILE("CvMap::setup");

	gDLL->getFAStarIFace()->Initialize(&GC.getPathFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getInterfacePathFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getStepFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), stepDestValid, stepHeuristic, stepCost, stepValid, stepAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getRouteFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, routeValid, NULL, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getBorderFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, borderValid, NULL, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getAreaFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, areaValid, NULL, joinArea, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getPlotGroupFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, plotGroupValid, NULL, countPlotGroup, NULL);
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvMap::setupGraphical()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	if (m_pMapPlots != NULL)
	{
		int iI;
		for (iI = 0; iI < numPlotsINLINE(); iI++)
		{
			if ( (iI % 10) == 0 )
			{
				gDLL->callUpdater();	// allow windows msgs to update
			}
			plotByIndexINLINE(iI)->setupGraphical();
		}
	}
}


void CvMap::erasePlots()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->erase();
	}
}


void CvMap::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->setRevealed(eTeam, bNewValue, bTerrainOnly, NO_TEAM, false);
	}

	GC.getGameINLINE().updatePlotGroups();
}


void CvMap::resetRevealedPlots(TeamTypes eTeam)
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		if (!plotByIndexINLINE(iI)->isVisible(eTeam, false))
		{
			plotByIndexINLINE(iI)->setRevealed(eTeam, false, false, NO_TEAM, false);
		}
	}

	GC.getGameINLINE().updatePlotGroups();
}


void CvMap::setAllPlotTypes(PlotTypes ePlotType)
{
	//float startTime = (float) timeGetTime();

	for(int i=0;i<numPlotsINLINE();i++)
	{
		plotByIndexINLINE(i)->setPlotType(ePlotType, false, false);
	}

	recalculateAreas();

	//rebuild landscape
	gDLL->getEngineIFace()->RebuildAllPlots();

	//mark minimap as dirty
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	
	//float endTime = (float) timeGetTime();
	//OutputDebugString(CvString::format("[Jason] setAllPlotTypes: %f\n", endTime - startTime).c_str());
}


// XXX generalize these funcs? (macro?)
void CvMap::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE("CvMap::doTurn()")

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->doTurn();
	}
}


void CvMap::updateFlagSymbols()
{
	updateFlagSymbolsInternal(false);
}

void CvMap::updateFlagSymbolsInternal(bool bForce)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		pLoopPlot = plotByIndexINLINE(iI);

		if (bForce || pLoopPlot->isFlagDirty())
		{
			pLoopPlot->updateFlagSymbol();
			pLoopPlot->setFlagDirty(false);
		}
	}
}


void CvMap::updateFog()
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateFog();
	}
}


void CvMap::updateVisibility()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateVisibility();
	}
}


void CvMap::updateSymbolVisibility()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSymbolVisibility();
	}
}


void CvMap::updateSymbols()
{
	PROFILE_FUNC();

	//	Ignore this while we are demand-creating symbols
	//	to minimize memory usage - REMOVED FOR NOW FOR VIEWPORTS
	int iI;
	
	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSymbols();
	}
}


void CvMap::updateMinimapColor()
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateMinimapColor();
	}
}


void CvMap::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSight(bIncrement, false);
	}

	if ( bUpdatePlotGroups )
	{
		GC.getGameINLINE().updatePlotGroups();
	}
}


void CvMap::updateIrrigated()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateIrrigated();
	}
}


void CvMap::updateCenterUnit()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateCenterUnit();
	}
}


void CvMap::updateWorkingCity()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateWorkingCity();
	}
}


void CvMap::updateMinOriginalStartDist(CvArea* pArea)
{
	PROFILE_FUNC();

	CvPlot* pStartingPlot;
	CvPlot* pLoopPlot;
	int iDist;
	int iI, iJ;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		pLoopPlot = plotByIndexINLINE(iI);

		if (pLoopPlot->area() == pArea)
		{
			pLoopPlot->setMinOriginalStartDist(-1);
		}
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

		if (pStartingPlot != NULL)
		{
			if (pStartingPlot->area() == pArea)
			{
				for (iJ = 0; iJ < numPlotsINLINE(); iJ++)
				{
					pLoopPlot = plotByIndexINLINE(iJ);

					if (pLoopPlot->area() == pArea)
					{
						
						//iDist = GC.getMapINLINE().calculatePathDistance(pStartingPlot, pLoopPlot);
						iDist = stepDistance(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());

						if (iDist != -1)
						{
						    //int iCrowDistance = plotDistance(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
						    //iDist = std::min(iDist,  iCrowDistance * 2);
							if ((pLoopPlot->getMinOriginalStartDist() == -1) || (iDist < pLoopPlot->getMinOriginalStartDist()))
							{
								pLoopPlot->setMinOriginalStartDist(iDist);
							}
						}
					}
				}
			}
		}
	}
}


void CvMap::updateYield()
{
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateYield();
	}
}


void CvMap::verifyUnitValidPlot()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->verifyUnitValidPlot();
	}
}


void CvMap::combinePlotGroups(PlayerTypes ePlayer, CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses)
{
	CvPlotGroup* pNewPlotGroup;
	CvPlotGroup* pOldPlotGroup;

	FAssertMsg(pPlotGroup1 != NULL, "pPlotGroup is not assigned to a valid value");
	FAssertMsg(pPlotGroup2 != NULL, "pPlotGroup is not assigned to a valid value");

	if (pPlotGroup1 == pPlotGroup2)
	{
		return;
	}

	if (pPlotGroup1->getLengthPlots() > pPlotGroup2->getLengthPlots())
	{
		pNewPlotGroup = pPlotGroup1;
		pOldPlotGroup = pPlotGroup2;
	}
	else
	{
		pNewPlotGroup = pPlotGroup2;
		pOldPlotGroup = pPlotGroup1;
	}

	pNewPlotGroup->mergeIn(pOldPlotGroup, bRecalculateBonuses);
}

CvPlot* CvMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	CvPlot* pPlot;
	CvPlot* pTestPlot;
	CvPlot* pLoopPlot;
	bool bValid;
	int iCount;
	int iDX, iDY;

	pPlot = NULL;

	iCount = 0;

	while (iCount < iTimeout)
	{
/*************************************************************************************************/
/**	Xienwolf Tweak							12/13/08											**/
/**																								**/
/**					Reduction in massive Random Spam in Logger files by using Map				**/
/*************************************************************************************************/
/**								---- Start Original Code ----									**
		pTestPlot = plotSorenINLINE(GC.getGameINLINE().getSorenRandNum(getGridWidthINLINE(), "Rand Plot Width"), GC.getGameINLINE().getSorenRandNum(getGridHeightINLINE(), "Rand Plot Height"));
/**								----  End Original Code  ----									**/
		pTestPlot = plotSorenINLINE(GC.getGameINLINE().getMapRandNum(getGridWidthINLINE(), "Rand Plot Width"), GC.getGameINLINE().getMapRandNum(getGridHeightINLINE(), "Rand Plot Height"));
/*************************************************************************************************/
/**	Tweak									END													**/
/*************************************************************************************************/

		FAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if ((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bValid = true;

			if (bValid)
			{
				if (iMinUnitDistance != -1)
				{
					for (iDX = -(iMinUnitDistance); iDX <= iMinUnitDistance; iDX++)
					{
						for (iDY = -(iMinUnitDistance); iDY <= iMinUnitDistance; iDY++)
						{
							pLoopPlot	= plotXY(pTestPlot->getX_INLINE(), pTestPlot->getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								if (pLoopPlot->isUnit())
								{
									bValid = false;
								}
							}
						}
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_LAND)
				{
					if (pTestPlot->isWater())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_UNOWNED)
				{
					if (pTestPlot->isOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_UNOWNED)
				{
					if (pTestPlot->isAdjacentOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_LAND)
				{
					if (!(pTestPlot->isAdjacentToLand()))
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_PASSIBLE)
				{
					if (pTestPlot->isImpassable())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_VISIBLE_TO_CIV)
				{
					if (pTestPlot->isVisibleToCivTeam())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_CITY)
				{
					if (pTestPlot->isCity())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				pPlot = pTestPlot;
				break;
			}
		}

		iCount++;
	}

	return pPlot;
}


CvCity* CvMap::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, CvCity* pSkipCity)
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	// XXX look for barbarian cities???

	iBestValue = MAX_INT;
	pBestCity = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				if ((eTeam == NO_TEAM) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
					{
						if (!bSameArea || (pLoopCity->area() == plotINLINE(iX, iY)->area()) || (bCoastalOnly && (pLoopCity->waterArea() == plotINLINE(iX, iY)->area())))
						{
							if (!bCoastalOnly || pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
							{
								if ((eTeamAtWarWith == NO_TEAM) || atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), eTeamAtWarWith))
								{
									if ((eDirection == NO_DIRECTION) || (estimateDirection(dxWrap(pLoopCity->getX_INLINE() - iX), dyWrap(pLoopCity->getY_INLINE() - iY)) == eDirection))
									{
										if ((pSkipCity == NULL) || (pLoopCity != pSkipCity))
										{
											iValue = plotDistance(iX, iY, pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

											if (iValue < iBestValue)
											{
												iBestValue = iValue;
												pBestCity = pLoopCity;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestCity;
}


CvSelectionGroup* CvMap::findSelectionGroup(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers)
{
	return findSelectionGroupInternal(iX, iY, eOwner, bReadyToSelect, bWorkers, false);
}

CvSelectionGroup* CvMap::findSelectionGroupInternal(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers, bool bAllowViewportSwitch)
{
	CvSelectionGroup* pLoopSelectionGroup;
	CvSelectionGroup* pBestSelectionGroup;
	CvSelectionGroup* pBestSelectionGroupInViewport;
	int iValue;
	int iBestValue;
	int iBestValueInViewport;
	int iLoop;
	int iI;

	// XXX look for barbarian cities???

	iBestValue = MAX_INT;
	iBestValueInViewport = MAX_INT;
	pBestSelectionGroup = NULL;
	pBestSelectionGroupInViewport = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				for(pLoopSelectionGroup = GET_PLAYER((PlayerTypes)iI).firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = GET_PLAYER((PlayerTypes)iI).nextSelectionGroup(&iLoop))
				{
					if (!bReadyToSelect || pLoopSelectionGroup->readyToSelect())
					{
						if (!bWorkers || pLoopSelectionGroup->hasWorker())
						{
							iValue = plotDistance(iX, iY, pLoopSelectionGroup->getX(), pLoopSelectionGroup->getY());

							if ( getCurrentViewport()->isInViewport(pLoopSelectionGroup->getX(), pLoopSelectionGroup->getY()) )
							{
								if (iValue < iBestValueInViewport)
								{
									iBestValueInViewport = iValue;
									pBestSelectionGroupInViewport = pLoopSelectionGroup;
								}
							}

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestSelectionGroup = pLoopSelectionGroup;
							}
						}
					}
				}
			}
		}
	}

	return pBestSelectionGroupInViewport != NULL ? pBestSelectionGroupInViewport : (bAllowViewportSwitch ? pBestSelectionGroup : NULL);
}

CvArea* CvMap::findBiggestArea(bool bWater)
{
	CvArea* pLoopArea;
	CvArea* pBestArea;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;
	pBestArea = NULL;

	for(pLoopArea = firstArea(&iLoop); pLoopArea != NULL; pLoopArea = nextArea(&iLoop))
	{
		if (pLoopArea->isWater() == bWater)
		{
			iValue = pLoopArea->getNumTiles();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pLoopArea;
			}
		}
	}

	return pBestArea;
}


int CvMap::getMapFractalFlags()
{
	int wrapX = 0;
	if (isWrapXINLINE())
	{
		wrapX = (int)CvFractal::FRAC_WRAP_X;
	}

	int wrapY = 0;
	if (isWrapYINLINE())
	{
		wrapY = (int)CvFractal::FRAC_WRAP_Y;
	}

	return (wrapX | wrapY);
}


//"Check plots for wetlands or seaWater.  Returns true if found"
bool CvMap::findWater(CvPlot* pPlot, int iRange, bool bFreshWater)
{
	PROFILE("CvMap::findWater()");

	CvPlot* pLoopPlot;
	int iDX, iDY;

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (bFreshWater)
				{
					if (pLoopPlot->isFreshWater())
					{
						return true;
					}
				}
				else
				{
					if (pLoopPlot->isWater())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvMap::isPlot(int iX, int iY) const
{
	return isPlotINLINE(iX, iY);
}


int CvMap::numPlots() const																											 
{
	return numPlotsINLINE();
}


int CvMap::plotNum(int iX, int iY) const
{
	return plotNumINLINE(iX, iY);
}


int CvMap::plotX(int iIndex) const
{
	return (iIndex % getGridWidthINLINE());
}


int CvMap::plotY(int iIndex) const
{
	return (iIndex / getGridWidthINLINE());
}


int CvMap::pointXToPlotX(float fX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fX + (fWidth/2.0f)) / fWidth) * getGridWidthINLINE());
}


float CvMap::plotXToPointX(int iX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iX * fWidth) / ((float)getGridWidthINLINE())) - (fWidth / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


int CvMap::pointYToPlotY(float fY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fY + (fHeight/2.0f)) / fHeight) * getGridHeightINLINE());
}


float CvMap::plotYToPointY(int iY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iY * fHeight) / ((float)getGridHeightINLINE())) - (fHeight / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


float CvMap::getWidthCoords()																	
{
	return (GC.getPLOT_SIZE() * ((float)getGridWidthINLINE()));
}


float CvMap::getHeightCoords()																	
{
	return (GC.getPLOT_SIZE() * ((float)getGridHeightINLINE()));
}


int CvMap::maxPlotDistance()
{
	return std::max(1, plotDistance(0, 0, ((isWrapXINLINE()) ? (getGridWidthINLINE() / 2) : (getGridWidthINLINE() - 1)), ((isWrapYINLINE()) ? (getGridHeightINLINE() / 2) : (getGridHeightINLINE() - 1))));
}


int CvMap::maxStepDistance()
{
	return std::max(1, stepDistance(0, 0, ((isWrapXINLINE()) ? (getGridWidthINLINE() / 2) : (getGridWidthINLINE() - 1)), ((isWrapYINLINE()) ? (getGridHeightINLINE() / 2) : (getGridHeightINLINE() - 1))));
}


int CvMap::getGridWidth() const
{
	return getGridWidthINLINE();
}


int CvMap::getGridHeight() const
{
	return getGridHeightINLINE();
}


int CvMap::getLandPlots()
{
	return m_iLandPlots;
}


void CvMap::changeLandPlots(int iChange)
{
	m_iLandPlots = (m_iLandPlots + iChange);
	FAssert(getLandPlots() >= 0);
}


int CvMap::getOwnedPlots()
{
	return m_iOwnedPlots;
}


void CvMap::changeOwnedPlots(int iChange)
{
	m_iOwnedPlots = (m_iOwnedPlots + iChange);
	FAssert(getOwnedPlots() >= 0);
}


int CvMap::getTopLatitude()
{
	return m_iTopLatitude;
}


int CvMap::getBottomLatitude()
{
	return m_iBottomLatitude;
}


int CvMap::getNextRiverID()
{
	return m_iNextRiverID;
}


void CvMap::incrementNextRiverID()
{
	m_iNextRiverID++;
}


bool CvMap::isWrapX() const
{
	return isWrapXINLINE();
}


bool CvMap::isWrapY() const
{
	return isWrapYINLINE();
}

bool CvMap::isWrap() const
{
	return isWrapINLINE();
}

WorldSizeTypes CvMap::getWorldSize()
{
	return GC.getInitCore().getWorldSize();
}


ClimateTypes CvMap::getClimate()
{
	return GC.getInitCore().getClimate();
}


SeaLevelTypes CvMap::getSeaLevel()
{
	return GC.getInitCore().getSeaLevel();
}



int CvMap::getNumCustomMapOptions()
{
	return GC.getInitCore().getNumCustomMapOptions();
}


CustomMapOptionTypes CvMap::getCustomMapOption(int iOption)
{
	return GC.getInitCore().getCustomMapOption(iOption);
}


int CvMap::getNumBonuses(BonusTypes eIndex)													
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumBonus[eIndex];
}


void CvMap::changeNumBonuses(BonusTypes eIndex, int iChange)									
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumBonus[eIndex] = (m_paiNumBonus[eIndex] + iChange);
	FAssert(getNumBonuses(eIndex) >= 0);
}


int CvMap::getNumBonusesOnLand(BonusTypes eIndex)													
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumBonusOnLand[eIndex];
}


void CvMap::changeNumBonusesOnLand(BonusTypes eIndex, int iChange)									
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumBonusOnLand[eIndex] = (m_paiNumBonusOnLand[eIndex] + iChange);
	FAssert(getNumBonusesOnLand(eIndex) >= 0);
}


CvPlot* CvMap::plotByIndex(int iIndex) const
{
	return plotByIndexINLINE(iIndex);
}


CvPlot* CvMap::plot(int iX, int iY) const
{
	return plotINLINE(iX, iY);
}


CvPlot* CvMap::pointToPlot(float fX, float fY)													
{
	return plotINLINE(pointXToPlotX(fX), pointYToPlotY(fY));
}


int CvMap::getIndexAfterLastArea()																
{
	return m_areas.getIndexAfterLast();
}


int CvMap::getNumAreas()																		
{
	return m_areas.getCount();
}


int CvMap::getNumLandAreas()
{
	CvArea* pLoopArea;
	int iNumLandAreas;
	int iLoop;

	iNumLandAreas = 0;

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			iNumLandAreas++;
		}
	}

	return iNumLandAreas;
}


CvArea* CvMap::getArea(int iID)																
{
	return m_areas.getAt(iID);
}


CvArea* CvMap::addArea()
{
	return m_areas.add();
}


void CvMap::deleteArea(int iID)
{
	m_areas.removeAt(iID);
}


CvArea* CvMap::firstArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.beginIter(pIterIdx) : m_areas.endIter(pIterIdx);
}


CvArea* CvMap::nextArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.nextIter(pIterIdx) : m_areas.prevIter(pIterIdx);
}


void CvMap::recalculateAreas()
{
	PROFILE("CvMap::recalculateAreas");

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->setArea(FFreeList::INVALID_INDEX);
	}

	m_areas.removeAll();

	calculateAreas();
}


void CvMap::resetPathDistance()
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getStepFinder());
}


// Super Forts begin *canal* *choke*
int CvMap::calculatePathDistance(CvPlot *pSource, CvPlot *pDest, CvPlot *pInvalidPlot)
// Super Forts end
{
	PROFILE_FUNC();

	FAStarNode* pNode;

	if (pSource == NULL || pDest == NULL)
	{
		return -1;
	}

	// Super Forts begin *canal* *choke*
	// 1 must be added because 0 is already being used as the default value for iInfo in GeneratePath()
	int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : GC.getMapINLINE().plotNum(pInvalidPlot->getX_INLINE(), pInvalidPlot->getY_INLINE()) + 1;

	if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSource->getX_INLINE(), pSource->getY_INLINE(), pDest->getX_INLINE(), pDest->getY_INLINE(), false, iInvalidPlot, true))
	// Super Forts end
	{
		pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

		if (pNode != NULL)
		{
			return pNode->m_iData1;
		}
	}


	return -1;
}


// Super Forts begin *canal* *choke*
void CvMap::calculateCanalAndChokePoints()
{
	int iI;
	for(iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->calculateCanalValue();
		plotByIndexINLINE(iI)->calculateChokeValue();
		// TEMPORARY HARD CODE for testing purposes
		/*if((plotByIndexINLINE(iI)->getChokeValue() > 0) || (plotByIndexINLINE(iI)->getCanalValue() > 0))
		{
			ImprovementTypes eImprovement = (ImprovementTypes) (plotByIndexINLINE(iI)->isWater() ? GC.getInfoTypeForString("IMPROVEMENT_OFFSHORE_PLATFORM") : GC.getInfoTypeForString("IMPROVEMENT_FORT"));
			plotByIndexINLINE(iI)->setImprovementType(eImprovement);
		}
		else
		{
			plotByIndexINLINE(iI)->setImprovementType(NO_IMPROVEMENT);
		}*/
	}
}
// Super Forts end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
// Plot danger cache
void CvMap::invalidateIsActivePlayerNoDangerCache()
{
	PROFILE_FUNC();

	int iI;
	CvPlot* pLoopPlot;

	for( iI = 0; iI < numPlotsINLINE(); iI++ )
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if( pLoopPlot != NULL )
		{
			pLoopPlot->setIsActivePlayerNoDangerCache(false);
			pLoopPlot->setIsActivePlayerHasDangerCache(false);
			pLoopPlot->CachePathValidityResult(NULL,false,false);
			pLoopPlot->CachePathValidityResult(NULL,true,false);
		}
	}
}


void CvMap::invalidateIsTeamBorderCache(TeamTypes eTeam)
{
	PROFILE_FUNC();

	int iI;
	CvPlot* pLoopPlot;

	for( iI = 0; iI < numPlotsINLINE(); iI++ )
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if( pLoopPlot != NULL )
		{
			pLoopPlot->setIsTeamBorderCache(eTeam, false);
		}
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


//
// read object from a stream
// used during load
//
void CvMap::read(FDataStreamBase* pStream)
{
	OutputDebugString("Reading Map: Start");
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvMapInitData defaultMapData;

	// Init data before load
	reset(&defaultMapData);

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvMap", &uiFlag);	// flags for expansion

	WRAPPER_READ(wrapper, "CvMap", &m_iGridWidth);
	WRAPPER_READ(wrapper, "CvMap", &m_iGridHeight);

	getCurrentViewport()->resizeForMap();

	WRAPPER_READ(wrapper, "CvMap", &m_iLandPlots);
	WRAPPER_READ(wrapper, "CvMap", &m_iOwnedPlots);
	WRAPPER_READ(wrapper, "CvMap", &m_iTopLatitude);
	WRAPPER_READ(wrapper, "CvMap", &m_iBottomLatitude);
	WRAPPER_READ(wrapper, "CvMap", &m_iNextRiverID);

	WRAPPER_READ(wrapper, "CvMap", &m_bWrapX);
	WRAPPER_READ(wrapper, "CvMap", &m_bWrapY);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated");
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonus);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonusOnLand);

	if (numPlotsINLINE() > 0)
	{
		m_pMapPlots = new CvPlot[numPlotsINLINE()];
		int iI;
		for (iI = 0; iI < numPlotsINLINE(); iI++)
		{
			m_pMapPlots[iI].read(pStream);
		}
	}

	WRAPPER_SKIP_ELEMENT(wrapper, "CvPlot", &g_plotTypeZobristHashes, SAVE_VALUE_TYPE_INT_ARRAY);

	// call the read of the free list CvArea class allocations
	ReadStreamableFFreeListTrashArray(m_areas, pStream);

	setup();

	WRAPPER_READ_OBJECT_END(wrapper);

	OutputDebugString("Reading Map: End");
}

// save object to a stream
// used during save
//
void CvMap::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	uint uiFlag=0;
	WRAPPER_WRITE(wrapper, "CvMap" ,uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvMap" ,m_iGridWidth);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iGridHeight);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iLandPlots);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iOwnedPlots);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iTopLatitude);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iBottomLatitude);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iNextRiverID);

	WRAPPER_WRITE(wrapper, "CvMap" ,m_bWrapX);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_bWrapY);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated");
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap" ,REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap" ,REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonusOnLand);

	int iI;	
	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		m_pMapPlots[iI].write(pStream);
	}

	// call the read of the free list CvArea class allocations
	WriteStreamableFFreeListTrashArray(m_areas, pStream);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvMap::beforeSwitch()
{
	PROFILE_FUNC();

	int i, iLoop;
	CvUnit* pLoopUnit;
	CvCity* pLoopCity;

	if ( GC.getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW )
	{
		GC.getGame().processGreatWall(false);
	}

	gDLL->getEngineIFace()->setResourceLayer(false);

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			for (pLoopUnit = GET_PLAYER((PlayerTypes)i).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)i).nextUnit(&iLoop))
			{
				if ( !pLoopUnit->isUsingDummyEntities() )
				{
					if (gDLL->getEntityIFace()->IsSelected(pLoopUnit->getEntity()))
					{
						gDLL->getInterfaceIFace()->selectUnit(pLoopUnit, true, true);
					}
					if (GC.IsGraphicsInitialized())
					{
						gDLL->getEntityIFace()->RemoveUnitFromBattle(pLoopUnit);
						pLoopUnit->removeEntity();
					}
					pLoopUnit->destroyEntity();
				}
			}

			for (pLoopCity = GET_PLAYER((PlayerTypes)i).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)i).nextCity(&iLoop))
			{
				if ( pLoopCity->getEntity() != NULL )
				{
					FAssert(pLoopCity->isInViewport());

					if (pLoopCity->isCitySelected())
					{
						gDLL->getInterfaceIFace()->clearSelectedCities();
					}
					pLoopCity->removeEntity();
					pLoopCity->destroyEntity();
				}
			}
		}
	}
	
	GC.clearSigns();

	for (i = 0; i < numPlotsINLINE(); i++)
	{
		plotByIndex(i)->destroyGraphics();
	}
}

void CvMap::afterSwitch()
{
	PROFILE_FUNC();

	int i, iLoop;
	CvUnit* pLoopUnit;
	CvCity* pLoopCity;
	CvMapGenerator& kGenerator = CvMapGenerator::GetInstance();

	if (m_pMapPlots == NULL)		// if it hasn't been initialized yet...
	{
		if (GC.getMapInfo(getType()).getInitialWBMap().GetLength() > 0)
		{
			CyArgsList argsList;
			long lResult;

			char mapPath[MAX_PATH];
			getcwd(mapPath, MAX_PATH);
			strcat(mapPath, GC.getMapInfo(getType()).getInitialWBMap().GetCString());

			argsList.add(mapPath);
			gDLL->getPythonIFace()->callFunction(PYWorldBuilderModule, "readAndApplyDesc", argsList.makeFunctionArgs(), &lResult);
			if (lResult < 0) // failed
			{
				AddDLLMessage((PlayerTypes)0, true, GC.getEVENT_MESSAGE_TIME(), L"Worldbuilder map failed to load");
			}
		}
		else if (GC.getMapInfo(getType()).getMapScript().GetLength() > 0)
		{
			init();
			kGenerator.setUseDefaultMapScript(false);
			kGenerator.generateRandomMap();
			kGenerator.addGameElements();
			kGenerator.setUseDefaultMapScript(true);
		}
		else
		{
			init();
			kGenerator.generateRandomMap();
			kGenerator.addGameElements();
		}
	}
	
	gDLL->getInterfaceIFace()->clearSelectionList();
	gDLL->getInterfaceIFace()->makeSelectionListDirty();

	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
	
	for (i = 0; i < numPlotsINLINE(); i++)
	{
		//	Koshlimg - this is no longer necesary (or correct) with viewports enabled
		if (!GC.viewportsEnabled() && (plotByIndex(i)->getX() > GC.getMapInfo(getType()).getGridWidth() || plotByIndex(i)->getY() > GC.getMapInfo(getType()).getGridHeight()))
		{
			plotByIndex(i)->setNull(true);
		}
		else
		{
			plotByIndex(i)->setLayoutDirty(true);
			plotByIndex(i)->setFlagDirty(true);
		}
	}
	
	{
		PROFILE("CvMap::afterSwitch.RebuildAll");

		gDLL->getEngineIFace()->RebuildAllPlots();
	}

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)i).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)i).nextCity(&iLoop))
			{
				//gDLL->getEntityIFace()->createCityEntity(pLoopCity);
				pLoopCity->setupGraphical();
			}
			
			for (pLoopUnit = GET_PLAYER((PlayerTypes)i).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)i).nextUnit(&iLoop))
			{
				if ( !pLoopUnit->isUsingDummyEntities() )
				{
					gDLL->getEntityIFace()->createUnitEntity(pLoopUnit);
					pLoopUnit->setupGraphical();
				}
			}
		}
	}

	setupGraphical();
	updateFog();
	updateSymbols();
	updateFlagSymbols();
	updateMinimapColor();

	// Reprocess landmarks and signs
	GC.reprocessSigns();
	if ( GC.getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW )
	{
		GC.getGame().processGreatWall(true);
	}

	gDLL->getEngineIFace()->setResourceLayer(GC.getResourceLayer());
	gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
}

int	CvMap::getNumViewports() const
{
	return m_viewports.size();
}

CvViewport* CvMap::getViewport(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	return m_viewports[iIndex];
}

int CvMap::addViewport(int iXOffset, int iYOffset, bool bIsFullMapContext)	//	Returns new viewport index
{
	CvViewport*	viewport = new CvViewport(this, bIsFullMapContext);

	if ( iXOffset >= 0 && iYOffset >= 0 )
	{
		viewport->setMapOffset(iXOffset, iYOffset);
	}
	m_viewports.push_back(viewport);

	return (int)m_viewports.size()-1;
}

void CvMap::deleteViewport(int iIndex)
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	if (m_iCurrentViewportIndex == iIndex)
	{
		m_iCurrentViewportIndex = -1;
	}

	delete m_viewports[iIndex];

	for(unsigned int iI = iIndex; iI < m_viewports.size()-1; iI++)
	{
		m_viewports[iI] = m_viewports[iI+1];
	}

	m_viewports.pop_back();
}

void CvMap::setCurrentViewport(int iIndex)
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	m_iCurrentViewportIndex = iIndex;
}

CvViewport* CvMap::getCurrentViewport() const
{
	FAssert( m_iCurrentViewportIndex == -1 || m_iCurrentViewportIndex < (int)m_viewports.size() );

	return (m_iCurrentViewportIndex == -1 ? NULL : m_viewports[m_iCurrentViewportIndex]);
}
	
MapTypes CvMap::getType() const
{
	return m_eType;
}

void CvMap::setType(MapTypes eNewType)
{
	m_eType = eNewType;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/


//
// used for loading WB maps
//
void CvMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	CvMapInitData initData(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY);

	// Set init core data
	GC.getInitCore().setWorldSize(eWorldSize);
	GC.getInitCore().setClimate(eClimate);
	GC.getInitCore().setSeaLevel(eSeaLevel);
	GC.getInitCore().setCustomMapOptions(iNumCustomMapOptions, aeCustomMapOptions);

	// Init map
	init(&initData);
}


//////////////////////////////////////////////////////////////////////////
// Protected Functions...
//////////////////////////////////////////////////////////////////////////

void CvMap::calculateAreas()
{
	PROFILE("CvMap::calculateAreas");
	CvPlot* pLoopPlot;
	CvArea* pArea;
	int iArea;
	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		pLoopPlot = plotByIndexINLINE(iI);
		gDLL->callUpdater();
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (pLoopPlot->getArea() == FFreeList::INVALID_INDEX)
		{
			pArea = addArea();
			pArea->init(pArea->getID(), pLoopPlot->isWater());

			iArea = pArea->getID();

			pLoopPlot->setArea(iArea);

			gDLL->getFAStarIFace()->GeneratePath(&GC.getAreaFinder(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, -1, pLoopPlot->isWater(), iArea);
		}
	}
}


// Private Functions...
/************************************************************************************************/
/* Afforess	                  Start		 07/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvMap::percentUnoccupiedLand(bool bExcludeWater, bool bIncludeBarbarian, bool bExcludePeaks, CvArea* pArea, int iRange, CvPlot* pRangeFromPlot)
{
	int iNumTiles = 0;
	int iNumTilesValid = 0;
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexINLINE(iI);
		if (!pLoopPlot->isWater() || !bExcludeWater)
		{
			if (pArea == NULL || pLoopPlot->area() == pArea)
			{
				if (!pLoopPlot->isPeak2(true) || !bExcludePeaks)
				{
					if ((iRange == -1 || pRangeFromPlot == NULL) || (plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pRangeFromPlot->getX_INLINE(), pRangeFromPlot->getY_INLINE()) <= iRange))
					{
						iNumTiles++;
						if (pLoopPlot->getOwnerINLINE() == NO_PLAYER || (bIncludeBarbarian && pLoopPlot->isHominid()))
						{
							iNumTilesValid++;
						}
					}
				}
			}
		}
	}
	if (iNumTiles > 0)
	{
		GC.getGame().logMsg("%d Tiles were in %d Range, out of %d total in range tiles", iNumTilesValid, iRange, iNumTiles);
		return (iNumTilesValid * 100) / iNumTiles;
	}
	return 0;
}		
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

void CvMap::toggleCitiesDisplay()
{
	gDLL->getInterfaceIFace()->clearSelectedCities();
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			int iI = 0;
			int iLoop;
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				iI++;
				//if (iI > 1)
				//	break;

				if (m_bCitiesDisplayed)
				{
					//pCity->removeEntity();
					//pCity->destroyEntity();
					//pCity->plot()->setPlotCity(NULL);
					//CvWString szBuffer = "Destroying: ";
					//szBuffer.append(pCity->getName());
					//AddDLLMessage(GC.getGameINLINE().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
					//pCity->killTestCheap();
					pCity->setVisible(false);
				}
				else
				{
					//pCity->createCityEntity(pCity);
					//pCity->setupGraphical();
					//pCity->plot()->setPlotCity(pCity);
					pCity->setVisible(true);
				}
			}
		}
	}
	//gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	m_bCitiesDisplayed = !m_bCitiesDisplayed;
	CvWString szBuffer = "City entities hidden";
	if (m_bCitiesDisplayed)
		szBuffer = "City entities visible";

	MEMORY_TRACK_EXEMPT();

	AddDLLMessage(GC.getGameINLINE().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
}

void CvMap::toggleUnitsDisplay()
{
	int* paiFirstBuilt = new int[GC.getNumBuildingInfos()];
	int** paiBuiltNum = new int*[GC.getNumBuildingInfos()];
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		paiFirstBuilt[iI] = MAX_INT;
		paiBuiltNum[iI] = new int[8];
		for (int iJ = 0; iJ < 8; iJ++)
		{
			paiBuiltNum[iI][iJ] = 0;
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			int iLoop;
			//for (CvUnit* pUnit = kPlayer.firstUnit(&iLoop); pUnit != NULL; pUnit = kPlayer.nextUnit(&iLoop))
			//{
			//	if (m_bUnitsDisplayed)
			//	{
			//		//gDLL->getEntityIFace()->RemoveUnitFromBattle(pUnit);
			//		//pUnit->removeEntity();
			//		//pUnit->destroyEntity();
			//		pUnit->kill(false);
			//	}
			//	else
			//	{
			//		//pUnit->createUnitEntity(pUnit);
			//		//pUnit->setupGraphical();
			//	}
			//}
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					//CvBuildingInfo & kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
					//if (isNationalWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
					if (pCity->getNumRealBuilding((BuildingTypes)iI) > 0)
					{
						int iBuiltTime = pCity->getBuildingOriginalTime((BuildingTypes)iI);
						if (iBuiltTime < paiFirstBuilt[iI])
						{
							paiFirstBuilt[iI] = pCity->getBuildingOriginalTime((BuildingTypes)iI);
						}
						if (iBuiltTime < 0)
						{
							paiBuiltNum[iI][0]++;
						}
						else if (iBuiltTime < 200)
						{
							paiBuiltNum[iI][1]++;
						}
						else if (iBuiltTime < 400)
						{
							paiBuiltNum[iI][2]++;
						}
						else if (iBuiltTime < 600)
						{
							paiBuiltNum[iI][3]++;
						}
						else if (iBuiltTime < 800)
						{
							paiBuiltNum[iI][4]++;
						}
						else if (iBuiltTime < 1000)
						{
							paiBuiltNum[iI][5]++;
						}
						else if (iBuiltTime < 1200)
						{
							paiBuiltNum[iI][6]++;
						}
						else
						{
							paiBuiltNum[iI][7]++;
						}

						//if (pCity->getBuildingOriginalTime((BuildingTypes)iI) > 1000)
						//{
						//	pCity->setNumRealBuilding((BuildingTypes)iI, 0);
						//}
					}
				}
			}
		}
	}

	/*for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			int iLoop;
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					//CvBuildingInfo & kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
					//if (isNationalWonderClass((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType())))
					if (pCity->getNumRealBuilding((BuildingTypes)iI) > 0)
					{
						//if (pCity->getBuildingOriginalTime((BuildingTypes)iI) < pabFirstBuilt[iI])
						//{
						//	pabFirstBuilt[iI] = pCity->getBuildingOriginalTime((BuildingTypes)iI);
						//}
						if (paiFirstBuilt[iI] > 400)
						{
							pCity->setNumRealBuilding((BuildingTypes)iI, 0);
						}
					}
				}
			}
		}
	}*/

	// Assemble table
	CvWString szTable;
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		/*if ((pabFirstBuilt[iI] > 400) && (pabFirstBuilt[iI] <= 600))
		{
			CvWString szBuffer;
			szBuffer.Format(L"%s : %d, %d ", GC.getBuildingInfo((BuildingTypes)iI).getText(), pabFirstBuilt[iI], GC.getGameINLINE().getBuildingClassCreatedCount((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()));
			szBuffer.append(CvWString(GC.getBuildingInfo((BuildingTypes)iI).getArtInfo()->getNIF()));
			AddDLLMessage(GC.getGameINLINE().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
		}*/
		CvWString szBuffer;
		szBuffer.Format(L"%s,%d,%d,", GC.getBuildingInfo((BuildingTypes)iI).getText(), paiFirstBuilt[iI], GC.getGameINLINE().getBuildingClassCreatedCount((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType()));
		szBuffer.append(CvWString(GC.getBuildingInfo((BuildingTypes)iI).getArtInfo()->getNIF()));
		szTable.append(szBuffer);
		szBuffer.clear();
		for (int iJ = 0; iJ < 8; iJ++)
		{
			szBuffer.Format(L",%d", paiBuiltNum[iI][iJ]);
			szTable.append(szBuffer);
		}
		szTable.append(NEWLINE);
	}

	CvString szTableOut;
	szTableOut.Convert(szTable);
	// Create a new file and write table to it
	std::ofstream fTable;
	fTable.open("BuildingsBuiltTable.csv");
	fTable << szTableOut.GetCString();
	fTable.close();

	delete[] paiFirstBuilt;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		delete[] paiBuiltNum[iI];
	}
	delete[] paiBuiltNum;

	m_bUnitsDisplayed = !m_bUnitsDisplayed;
	CvWString szBuffer = "Unit entities destroyed";
	if (m_bUnitsDisplayed)
		szBuffer = "Unit entities created";
			
	MEMORY_TRACK_EXEMPT();

	AddDLLMessage(GC.getGameINLINE().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
}

bool CvMap::generatePathForHypotheticalUnit(const CvPlot *pFrom, const CvPlot *pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags, int iMaxTurns)
{
	return CvSelectionGroup::getPathGenerator()->generatePathForHypotheticalUnit(pFrom, pTo, ePlayer, eUnit, iFlags, iMaxTurns);
}

CvPath& CvMap::getLastPath()
{
	return CvSelectionGroup::getPathGenerator()->getLastPath();
}

int CvMap::getLastPathStepNum()
{
	// length of the path is not the number of steps so we have to count
	CvPath::const_iterator it = getLastPath().begin();
	int i=0;
	while (it.plot())
	{
		i++;
		++it;
	}
	return i;
}

CvPlot* CvMap::getLastPathPlotByIndex(int index)
{
	// we can only start from the beginning if we don't want to expose the iterator to Python
	CvPath::const_iterator it = getLastPath().begin();
	for (int i=0; i<index; i++)
		++it;
	return it.plot();
}