#include "CvGameCoreDLL.h"

CvViewport::CvViewport(
	CvMap*	pMap,
	bool bIsFullMapContext) : m_pMap(pMap),
							  m_transformType(VIEWPORT_TRANSFORM_TYPE_WINDOW),
							  m_inhibitSelection(false),
							  m_bDisplayCityScreen(false),
							  m_bSelectCity(false),
							  m_bAddSelectedCity(false),
							  m_state(VIEWPORT_ACTION_STATE_NONE),
							  m_countdown(0),
							  m_bSwitchInProgress(false),
							  m_eSpoofHiddenGraphics(VIEWPORT_SPOOF_NONE),
							  m_spoofTransitionStartTickCount(-1)
{
	resizeForMap();

	m_mode = (bIsFullMapContext ? VIEWPORT_MODE_FULL_MAP : VIEWPORT_MODE_UNINITIALIZED);
}

CvViewport::~CvViewport()
{
}

void CvViewport::setMapOffset(int iX, int iY)
{
	m_iXOffset = iX;
	m_iYOffset = iY;

	m_mode = VIEWPORT_MODE_INITIALIZED;
}

void CvViewport::getMapOffset(int& iX, int& iY) const
{
	iX = m_iXOffset;
	iY = m_iYOffset;
}

void	CvViewport::setOffsetToShow(int iX, int iY)	//	Make this the centre or near as appropiate
{
	int iNewX = iX - m_iXSize/2;
	int iNewY = iY - m_iYSize/2;

	if ( !m_pMap->isWrapX() )
	{
		iNewX = range(iNewX, 0, m_pMap->getGridWidthINLINE() - m_iXSize);
	}

	if ( !m_pMap->isWrapY() )
	{
		iNewY = range(iNewY, 0, m_pMap->getGridHeightINLINE() - m_iYSize);
	}

	setMapOffset(iNewX, iNewY);
}

void	CvViewport::resizeForMap()
{
	if (m_pMap->getGridWidthINLINE() > 0 && !GC.bugInitCalled())
	{

		//	Force-load the main interface BUG module so we can get at the viewport BUG settings
		CyArgsList argsList;
		argsList.add("BUG Main Interface");

		PYTHON_CALL_FUNCTION(__FUNCTION__, PYCivModule, "forceBUGModuleInit", argsList.makeFunctionArgs());
	}

	m_iXSize = GC.viewportsEnabled() ? GC.getViewportSizeX() : m_pMap->getGridWidthINLINE();
	m_iYSize = GC.viewportsEnabled() ? GC.getViewportSizeY() : m_pMap->getGridHeightINLINE();

	//	For now we don't allow maps smaller than the viewport size
	if ( m_iXSize > m_pMap->getGridWidthINLINE() )
	{
		m_iXSize = m_pMap->getGridWidthINLINE();
	}

	if ( m_iYSize > m_pMap->getGridHeightINLINE() )
	{
		m_iYSize = m_pMap->getGridHeightINLINE();
	}
}

void	CvViewport::bringIntoView(int iX, int iY, CvUnit* pSelectionUnit, bool bLookAt, bool bForceCenter, bool bDisplayCityScreen, bool bSelectCity, bool bAddSelectedCity)
{
	m_pLookatPlot = m_pMap->plotINLINE(iX, iY);
	if ( pSelectionUnit != NULL && !pSelectionUnit->isDead() && !pSelectionUnit->isDelayedDeath() )
	{
		m_preservedHeadSelectedUnitId = pSelectionUnit->getIDInfo();
	}

	m_bDisplayCityScreen = bDisplayCityScreen;
	m_bSelectCity = bSelectCity;
	m_bAddSelectedCity = bAddSelectedCity;
	if ( m_transformType != VIEWPORT_TRANSFORM_TYPE_WINDOW || !isInViewport(iX, iY, GC.getViewportSelectionBorder()) || bForceCenter)
	{
		gDLL->getInterfaceIFace()->clearSelectionList();
		gDLL->getInterfaceIFace()->clearSelectedCities();
		m_inhibitSelection = true;

		setActionState(VIEWPORT_ACTION_STATE_BRING_INTO_VIEW);
	}
	else
	{
		setActionState(VIEWPORT_ACTION_STATE_SET_LOOKAT);
	}
}

void	CvViewport::centerOnSelection()
{
	CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if ( pUnit != NULL )
	{
		bringIntoView( pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit, true, true);
	}
}

void	CvViewport::panLeft()
{
	int iNewCenterX = m_iXOffset - m_iXSize/2;

	if ( iNewCenterX < 0 )
	{
		if ( !m_pMap->isWrapX() )
		{
			iNewCenterX = 0;
		}
		else
		{
			iNewCenterX += m_pMap->getGridWidthINLINE();
		}
	}

	bringIntoView(iNewCenterX, m_iYOffset + m_iYSize/2, NULL, true, true);
}

void	CvViewport::panRight()
{
	int iNewCenterX = m_iXOffset + (3*m_iXSize)/2;

	if ( iNewCenterX > m_pMap->getGridWidthINLINE() )
	{
		if ( !m_pMap->isWrapX() )
		{
			iNewCenterX = m_pMap->getGridWidthINLINE() - 1;
		}
		else
		{
			iNewCenterX -= m_pMap->getGridWidthINLINE();
		}
	}

	bringIntoView(iNewCenterX, m_iYOffset + m_iYSize/2, NULL, true, true);
}

void	CvViewport::panDown()
{
	int iNewCenterY = m_iYOffset - m_iYSize/2;

	if ( iNewCenterY < 0 )
	{
		if ( !m_pMap->isWrapY() )
		{
			iNewCenterY = 0;
		}
		else
		{
			iNewCenterY += m_pMap->getGridHeightINLINE();
		}
	}

	bringIntoView(m_iXOffset + m_iXSize/2, iNewCenterY, NULL, true, true);
}

void	CvViewport::panUp()
{
	int iNewCenterY = m_iYOffset + (3*m_iYSize)/2;

	if ( iNewCenterY > m_pMap->getGridHeightINLINE() )
	{
		if ( !m_pMap->isWrapY() )
		{
			iNewCenterY = m_pMap->getGridHeightINLINE() - 1;
		}
		else
		{
			iNewCenterY -= m_pMap->getGridHeightINLINE();
		}
	}

	bringIntoView(m_iXOffset + m_iXSize/2, iNewCenterY, NULL, true, true);
}


void CvViewport::init(CvMapInitData* pInitData)
{
	m_pMap->init(pInitData);
}

void CvViewport::setupGraphical()
{
	m_mode = VIEWPORT_MODE_INITIALIZED;
	
	m_pMap->setupGraphical();
}

void CvViewport::reset(CvMapInitData* pInitData)
{
	OutputDebugString("Reseting Viewport: Start");
	m_pMap->reset(pInitData);
	OutputDebugString("Reseting Viewport: End");
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
MapTypes CvViewport::getType() const
{
	return m_pMap->getType();
}

void CvViewport::setType(MapTypes eNewType)
{
	m_pMap->setType(eNewType);
}

void CvViewport::beforeSwitch()
{
	m_bSwitchInProgress = true;
	m_pMap->beforeSwitch();
}

void CvViewport::afterSwitch()
{
	setSpoofHiddenGraphics(VIEWPORT_SPOOF_ALL_UNREVEALED);

	m_pMap->afterSwitch();
	m_bSwitchInProgress = false;
}

bool	CvViewport::isMidSwitch() const
{
	return m_bSwitchInProgress;
}

void CvViewport::closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom)
{
	if ( m_transformType != VIEWPORT_TRANSFORM_TYPE_WINDOW )
	{
		//	Minimap clicks (in the advisor) don't tell us where in the minmap the click occured, so this
		//	routine takes the minimap coordinates (within the advisor screen) and figures out where the mouse
		//	is from first principals
		if ( iMinimapLeft != -1 )
		{
			CURSORINFO cursorInfo;

			cursorInfo.cbSize = sizeof(cursorInfo);
			GetCursorInfo(&cursorInfo);

			HWND hWnd = GetActiveWindow();

			WINDOWINFO windowInfo;

			windowInfo.cbSize = sizeof(windowInfo);
			GetWindowInfo(hWnd, &windowInfo);

			int iAdvisorScreenLeft = ((windowInfo.rcClient.right - windowInfo.rcClient.left) - advisorWidth)/2 + windowInfo.rcClient.left;
			int iAdvisorScreenTop = ((windowInfo.rcClient.bottom - windowInfo.rcClient.top) - 768)/2 + windowInfo.rcClient.top;
		
			iMinimapTop += iAdvisorScreenTop;
			iMinimapBottom += iAdvisorScreenTop;
			iMinimapLeft += iAdvisorScreenLeft;
			iMinimapRight += iAdvisorScreenLeft;

			//	Calculate what plot was clicked on
			int iMapX = range(((cursorInfo.ptScreenPos.x - iMinimapLeft)*m_pMap->getGridWidthINLINE())/(iMinimapRight - iMinimapLeft), 0, m_pMap->getGridWidthINLINE()-1);
			int iMapY = range(((iMinimapBottom - cursorInfo.ptScreenPos.y)*m_pMap->getGridHeightINLINE())/(iMinimapBottom - iMinimapTop), 0, m_pMap->getGridHeightINLINE()-1);

			//	Save the lookatPlot for after the viewport switch so it can be set in the new view
			m_pLookatPlot = m_pMap->plotINLINE(iMapX, iMapY);
		}

		bringIntoView(m_pLookatPlot->getX_INLINE(), m_pLookatPlot->getY_INLINE(), ::getUnit(m_preservedHeadSelectedUnitId));
	}
}

//	Process the current action state (which may include transitioning to another state)
void CvViewport::processActionState()
{
	if ( m_countdown > 0 )
	{
		m_countdown--;
		return;
	}

	if ( m_spoofTransitionStartTickCount != -1 )
	{
		if ( GetTickCount() - m_spoofTransitionStartTickCount > 5000 )
		{
			setSpoofHiddenGraphics(VIEWPORT_SPOOF_NOT_ADJACENT_TO_REVEALED);

			for(int iI = 0; iI < numPlots(); iI++)
			{
				CvPlot*	pPlot = plotByIndex(iI);

				if ( pPlot != NULL )
				{
					if ( pPlot->isRiverMask() &&
						 !pPlot->isRevealed(GC.getGame().getActiveTeam(), true) &&
						 pPlot->isAdjacentRevealed(GC.getGame().getActiveTeam(), true) )
					{
						pPlot->updateRiverSymbol(true, false);
					}
				}
			}
		}
	}

	switch(m_state)
	{
	case VIEWPORT_ACTION_STATE_NONE:
		break;
	case VIEWPORT_ACTION_STATE_LAUNCH_MILITARY_ADVISOR:
		gDLL->getInterfaceIFace()->clearSelectionList();
		gDLL->getInterfaceIFace()->clearSelectedCities();

		m_pLookatPlot = gDLL->getInterfaceIFace()->getLookAtPlot();

		m_inhibitSelection = true;

		setActionState(VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHING);
		break;
	case VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHING:
		if ( gDLL->getInterfaceIFace()->getHeadSelectedUnit() != NULL )
		{
			m_preservedHeadSelectedUnitId = gDLL->getInterfaceIFace()->getHeadSelectedUnit()->getIDInfo();
		}
		else
		{
			m_preservedHeadSelectedUnitId.reset();
		}

		beforeSwitch();
		m_transformType = VIEWPORT_TRANSFORM_TYPE_SCALE;
		m_inhibitSelection = true;

		afterSwitch();
		gDLL->getPythonIFace()->callFunction(PYScreensModule, "showMilitaryAdvisor");

		setActionState(VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHED);
		break;
	case VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHED:
		break;
	case VIEWPORT_ACTION_STATE_BRING_INTO_VIEW:
		if ( m_mode == VIEWPORT_MODE_INITIALIZED )
		{
			beforeSwitch();
		}

		m_transformType = VIEWPORT_TRANSFORM_TYPE_WINDOW;

		setMapOffset(range((m_pLookatPlot->getX_INLINE() - m_iXSize/2 + (m_pMap->isWrapXINLINE() ? m_pMap->getGridWidthINLINE() : 0))%m_pMap->getGridWidthINLINE(), 0, m_pMap->getGridWidthINLINE() - (m_pMap->isWrapXINLINE() ? 1 : m_iXSize)),
					 range((m_pLookatPlot->getY_INLINE() - m_iYSize/2 + (m_pMap->isWrapYINLINE() ? m_pMap->getGridHeightINLINE() : 0))%m_pMap->getGridHeightINLINE(), 0, m_pMap->getGridHeightINLINE() - (m_pMap->isWrapYINLINE() ? 1 : m_iYSize)));

		if ( m_mode == VIEWPORT_MODE_INITIALIZED )
		{
			afterSwitch();
		}

		m_inhibitSelection = true;
		setActionState(VIEWPORT_ACTION_STATE_BRING_INTO_VIEW_COMPLETE);
		break;
	case VIEWPORT_ACTION_STATE_BRING_INTO_VIEW_COMPLETE:
		{
			CvUnit* pUnit = ::getUnit(m_preservedHeadSelectedUnitId);
			if ( pUnit != NULL && !pUnit->isDead() && !pUnit->isDelayedDeath() && pUnit->plot()->isInViewport())
			{
				gDLL->getInterfaceIFace()->selectUnit(pUnit, true, true);
			}

			m_pMap->updateFlagSymbolsInternal(true);

			//m_countdown = 20;
			setActionState(VIEWPORT_ACTION_STATE_SET_SELECTION);
		}
		break;
	case VIEWPORT_ACTION_STATE_SET_LOOKAT:
		if ( ::getUnit(m_preservedHeadSelectedUnitId) == NULL && !m_bDisplayCityScreen && !m_bSelectCity )
		{
			GC.getGame().cycleSelectionGroupsInternal(true, true, false, false, false);
		}
		m_preservedHeadSelectedUnitId.reset();
		if ( m_pLookatPlot != NULL && m_pLookatPlot->isInViewport() )
		{
			if ( (m_bDisplayCityScreen || m_bSelectCity) && m_pLookatPlot->getPlotCity() != NULL )
			{
				if ( m_bAddSelectedCity )
				{
					gDLL->getInterfaceIFace()->addSelectedCity(m_pLookatPlot->getPlotCity());
				}
				else
				{
					gDLL->getInterfaceIFace()->selectCity(m_pLookatPlot->getPlotCity(), true);
				}
			}

			if ( !m_bDisplayCityScreen )
			{
				gDLL->getInterfaceIFace()->lookAt(m_pLookatPlot->getPoint(), CAMERALOOKAT_IMMEDIATE);
			}
		}

		m_bDisplayCityScreen = false;
		m_bSelectCity = false;
		m_bAddSelectedCity = false;

		setActionState(VIEWPORT_ACTION_STATE_NONE);
		break;
	case VIEWPORT_ACTION_STATE_SET_SELECTION:
		m_inhibitSelection = false;

		GC.getGame().updateSelectionListInternal(false, false, true);

		//m_countdown = 20;
		setActionState(VIEWPORT_ACTION_STATE_SET_LOOKAT);
		break;
	case VIEWPORT_ACTION_STATE_LOADING:
		{
			CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

			setActionState(VIEWPORT_ACTION_STATE_NONE);

			//	At load tim we need to scroll the viewport to look at the selected unit, or if there is
			//	none get one selected
			if ( pSelectedUnit == NULL )
			{
				GC.getGame().updateSelectionListInternal(true, true);
				pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			}

			if ( pSelectedUnit != NULL )
			{
				bringIntoView(pSelectedUnit->getX_INLINE(), pSelectedUnit->getY_INLINE(), pSelectedUnit, true, true);
			}

#if 0
			//	The above should have set of a new sequence of state transitions,
			//	but in case no unit was found to select default to a middle-of-map
			//	viewport
			if ( m_state == VIEWPORT_ACTION_STATE_NONE && m_state == VIEWPORT_MODE_UNINITIALIZED)
			{
				bringIntoView(m_pMap->getGridWidthINLINE()/2, m_pMap->getGridHeightINLINE()/2, NULL, true, true);
			}
#endif
		}
		break;
	case VIEWPORT_ACTION_STATE_SAVING:
		GC.getGameINLINE().processGreatWall(true);

		setActionState(VIEWPORT_ACTION_STATE_NONE);
		break;
	}
}

//	Transition to a new action state
void CvViewport::setActionState(ViewportDeferredActionState newState, bool bProcessImmediately)
{
	m_state = newState;

	if ( bProcessImmediately )
	{
		processActionState();
	}
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/
	
void CvViewport::erasePlots()
{
	m_pMap->erasePlots();
}

void CvViewport::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	m_pMap->setRevealedPlots(eTeam, bNewValue, bTerrainOnly);
}

void CvViewport::setAllPlotTypes(PlotTypes ePlotType)
{
	m_pMap->setAllPlotTypes(ePlotType);
}

void CvViewport::doTurn()
{
	m_pMap->doTurn();
}

void CvViewport::updateFlagSymbols()
{
	m_pMap->updateFlagSymbols();
}

void CvViewport::updateFog()
{
	m_pMap->updateFog();
}

void CvViewport::updateVisibility()
{
	m_pMap->updateVisibility();
}

void CvViewport::updateSymbolVisibility()
{
	m_pMap->updateSymbolVisibility();
}

void CvViewport::updateSymbols()
{
	m_pMap->updateSymbols();
}

void CvViewport::updateMinimapColor()
{
	m_pMap->updateMinimapColor();
}

void CvViewport::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	m_pMap->updateSight(bIncrement, bUpdatePlotGroups);
}

void CvViewport::updateIrrigated()
{
	m_pMap->updateIrrigated();
}

void CvViewport::updateCenterUnit()
{
	m_pMap->updateCenterUnit();
}

void CvViewport::updateWorkingCity()
{
	m_pMap->updateWorkingCity();
}

void CvViewport::updateMinOriginalStartDist(CvArea* pArea)
{
	m_pMap->updateMinOriginalStartDist(pArea);
}

void CvViewport::updateYield()
{
	m_pMap->updateYield();
}

void CvViewport::verifyUnitValidPlot()
{
	m_pMap->verifyUnitValidPlot();
}

CvPlot* CvViewport::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	return m_pMap->syncRandPlot(iFlags, iArea, iMinUnitDistance, iTimeout);
}

CvCity* CvViewport::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, CvCity* pSkipCity)
{
	return m_pMap->findCity(iX, iY, eOwner, eTeam, bSameArea, bCoastalOnly, eTeamAtWarWith, eDirection, pSkipCity);
}

CvSelectionGroup* CvViewport::findSelectionGroup(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers)
{
	return m_pMap->findSelectionGroup(iX, iY, eOwner, bReadyToSelect, bWorkers);
}

CvArea* CvViewport::findBiggestArea(bool bWater)
{
	return m_pMap->findBiggestArea(bWater);
}

int CvViewport::getMapFractalFlags()
{
	return m_pMap->getMapFractalFlags();
}

bool CvViewport::findWater(CvPlot* pPlot, int iRange, bool bFreshWater)
{
	return m_pMap->findWater(pPlot, iRange, bFreshWater);
}

bool CvViewport::isPlot(int iX, int iY) const
{
	return m_pMap->isPlot(getMapXFromViewportX(iX), getMapYFromViewportY(iY));
}

int CvViewport::numPlots() const
{
	return m_iXSize * m_iYSize;
}

int CvViewport::plotNum(int iX, int iY) const
{
	return iY*m_iXSize + iX;
}

int CvViewport::plotX(int iIndex) const
{
	return iIndex % m_iXSize;
}

int CvViewport::plotY(int iIndex) const
{
	return iIndex/m_iXSize;
}

int CvViewport::pointXToPlotX(float fX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);

	float fNormalizedX = fX + (fWidth/2.0f);
	int iResult = fNormalizedX >= 0 ? (int)((fNormalizedX / fWidth) * getGridWidth()) : INVALID_PLOT_COORD;

	if ( iResult >= m_iXSize )
	{
		iResult = INVALID_PLOT_COORD;
	}

	return iResult;
}

float CvViewport::plotXToPointX(int iX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iX * fWidth) / ((float)getGridWidth())) - (fWidth / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}

int CvViewport::pointYToPlotY(float fY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);

	float fNormalizedY = fY + (fHeight/2.0f);
	int iResult = fNormalizedY >= 0 ? (int)((fNormalizedY / fHeight) * getGridHeight()) : INVALID_PLOT_COORD;

	if ( iResult >= m_iYSize )
	{
		iResult = INVALID_PLOT_COORD;
	}

	return iResult;
}

float CvViewport::plotYToPointY(int iY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iY * fHeight) / ((float)getGridHeight())) - (fHeight / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}

float CvViewport::getWidthCoords()
{
	return (GC.getPLOT_SIZE() * ((float)getGridWidth()));
}

float CvViewport::getHeightCoords()
{
	return (GC.getPLOT_SIZE() * ((float)getGridHeight()));
}

int CvViewport::maxPlotDistance()
{
	return m_pMap->maxPlotDistance();
}

int CvViewport::maxStepDistance()
{
	return m_pMap->maxStepDistance();
}

int CvViewport::getGridWidth() const
{
	return m_iXSize;
}

int CvViewport::getGridHeight() const
{
	return m_iYSize;
}

int CvViewport::getLandPlots()
{
	return m_pMap->getLandPlots();
}

int CvViewport::getOwnedPlots()
{
	return m_pMap->getOwnedPlots();
}

int CvViewport::getTopLatitude()
{
	return m_pMap->getTopLatitude();
}

int CvViewport::getBottomLatitude()
{
	return m_pMap->getBottomLatitude();
}

bool CvViewport::isWrapX() const
{
	//	Viewports never map unles they cover the entire map and the map wraps
	return (getGridWidth() == m_pMap->getGridWidth() ? m_pMap->isWrapX() : false);
}

bool CvViewport::isWrapY() const
{
	//	Viewports never map unles they cover the entire map and the map wraps
	return (getGridHeight() == m_pMap->getGridHeight() ? m_pMap->isWrapY() : false);
}

bool CvViewport::isWrap() const
{
	return isWrapX() || isWrapY();
}

WorldSizeTypes CvViewport::getWorldSize()
{
	return m_pMap->getWorldSize();
}

ClimateTypes CvViewport::getClimate()
{
	return m_pMap->getClimate();
}

SeaLevelTypes CvViewport::getSeaLevel()
{
	return m_pMap->getSeaLevel();
}

int CvViewport::getNumCustomMapOptions()
{
	return m_pMap->getNumCustomMapOptions();
}

CustomMapOptionTypes CvViewport::getCustomMapOption(int iOption)
{
	return m_pMap->getCustomMapOption(iOption);
}

CvPlot* CvViewport::plotByIndex(int iIndex) const
{
	return plot(iIndex % m_iXSize, iIndex/m_iXSize);
}

CvPlot* CvViewport::plot(int iX, int iY) const
{
	return m_pMap->plot(getMapXFromViewportX(iX), getMapYFromViewportY(iY));
}

CvPlot* CvViewport::pointToPlot(float fX, float fY)
{
	return plot(pointXToPlotX(fX), pointYToPlotY(fY));
}

int CvViewport::getNumAreas()
{
	return m_pMap->getNumAreas();
}

int CvViewport::getNumLandAreas()
{
	return m_pMap->getNumLandAreas();
}

// Serialization:
void CvViewport::read(FDataStreamBase* pStream)
{
	m_pMap->read(pStream);
}

void CvViewport::write(FDataStreamBase* pStream)
{
	m_pMap->write(pStream);
}
	
void CvViewport::setSpoofHiddenGraphics(ViewportGraphicalSpoofingState eValue)
{
	m_eSpoofHiddenGraphics = eValue;

	if ( eValue == VIEWPORT_SPOOF_ALL_UNREVEALED )
	{
		m_spoofTransitionStartTickCount = GetTickCount();
	}
	else
	{
		m_spoofTransitionStartTickCount = -1;
	}
}
