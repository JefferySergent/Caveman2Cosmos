//	Class representing a set of plots reachable in a fixed number of tiles with given flags
#include "CvGameCoreDLL.h"
#include "CvReachablePlotSet.h"

CvReachablePlotSet::const_iterator::const_iterator(CvReachablePlotSet& parent, stdext::hash_map<CvPlot*,CvReachablePlotInfo>::const_iterator& itr) : m_parent(parent)
{
	m_itr = itr;
}

CvReachablePlotSet::const_iterator& CvReachablePlotSet::const_iterator::operator++()
{
	do
	{
		++m_itr;
	} while( m_itr != m_parent.end().m_itr && m_itr->second.iStepDistance > m_parent.m_iRange );

	return (*this);
}

bool CvReachablePlotSet::const_iterator::operator==(const_iterator& other)
{
	return other.m_itr == m_itr;
}

bool CvReachablePlotSet::const_iterator::operator!=(const_iterator& other)
{
	return other.m_itr != m_itr;
}

CvReachablePlotSet::const_iterator& CvReachablePlotSet::const_iterator::operator=(const_iterator& other)
{
	m_itr = other.m_itr;
	m_parent = other.m_parent;

	return (*this);
}

CvPlot*	CvReachablePlotSet::const_iterator::plot()
{
	if ( m_itr == m_parent.end().m_itr )
	{
		return NULL;
	}
	else
	{
		return m_itr->first;
	}
}

int CvReachablePlotSet::const_iterator::stepDistance()
{
	if ( m_itr == m_parent.end().m_itr )
	{
		return MAX_INT;
	}
	else
	{
		return m_itr->second.iStepDistance;
	}
}

int CvReachablePlotSet::const_iterator::outsideBorderDistance()
{
	if ( m_itr == m_parent.end().m_itr )
	{
		return MAX_INT;
	}
	else
	{
		return m_itr->second.iOutsideOwnedDistance;
	}
}

int CvReachablePlotSet::const_iterator::getOpaqueInfo(int iActivityId) const
{
	if ( m_itr == m_parent.end().m_itr )
	{
		return 0;
	}
	else
	{
		return m_itr->second.iOpaqueInfo[iActivityId];
	}
}

void CvReachablePlotSet::const_iterator::setOpaqueInfo(int iActivityId, int iValue)
{
	if ( m_itr != m_parent.end().m_itr )
	{
		*(int*)&(m_itr->second.iOpaqueInfo[iActivityId]) = iValue;
	}
}

CvReachablePlotSet::CvReachablePlotSet(CvSelectionGroup * group, int iFlags, int iRange, bool bCachable, int iOutsideOwnedRange)
{
	m_group = group;
	//	Always include move-through-enemy flag when constructing reachable sets
	//	because otherwise any flagless move that tries to target a tile to attack
	//	(so has an enemy in its terminal node) will register as unreachable
	m_iFlags = iFlags | MOVE_THROUGH_ENEMY;

	m_proxyTo = NULL;
	m_iRange = -1;
	m_iOutsideOwnedRange = iOutsideOwnedRange;
	m_bCachable = bCachable;
	m_reachablePlots = NULL;

	if ( iRange != -1 )
	{
		Populate(iRange);
	}
}

CvReachablePlotSet::~CvReachablePlotSet()
{
	SAFE_DELETE(m_reachablePlots);
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::begin()
{
	CvReachablePlotSet::const_iterator result = CvReachablePlotSet::const_iterator(*this, (m_proxyTo == NULL ? m_reachablePlots->begin() : m_proxyTo->m_reachablePlots->begin()));
	
	while( result != end() && result.stepDistance() > m_iRange )
	{
		++result;
	};

	return result;
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::end()
{
	return CvReachablePlotSet::const_iterator(*this, (m_proxyTo == NULL ? m_reachablePlots->end() : m_proxyTo->m_reachablePlots->end()));
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::find(CvPlot* plot)
{
	if ( m_proxyTo == NULL )
	{
		return CvReachablePlotSet::const_iterator(*this, m_reachablePlots->find(plot));
	}
	else
	{
		CvReachablePlotSet::const_iterator result = m_proxyTo->find(plot);

		if ( result.stepDistance() <= m_iRange )
		{
			return result;
		}
		else
		{
			return end();
		}
	}
}

int CvReachablePlotSet::getRange() const
{
	if ( m_proxyTo == NULL )
	{
		return m_iRange;
	}
	else
	{
		return m_proxyTo->m_iRange;
	}
}

void CvReachablePlotSet::setRange(int iRange)
{
	if ( m_proxyTo == NULL )
	{
		m_iRange = iRange;
	}
	else
	{
		m_proxyTo->m_iRange = iRange;
	}
}

typedef struct CvReachablePlotSetCacheEntry
{
	CvReachablePlotSet*	plotSet;
	int					iLRUSeq;
} CvReachablePlotSetCacheEntry;

#define	PLOT_SET_CACHE_SIZE	2
typedef struct
{
	CvReachablePlotSetCacheEntry	cacheEntries[PLOT_SET_CACHE_SIZE];
	int								iNextLRU;
} PlotSetCache;

static PlotSetCache* CachedPlotSets = NULL;

void CvReachablePlotSet::ClearCache()
{
	if ( CachedPlotSets != NULL )
	{
		for(int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
		{
			SAFE_DELETE(CachedPlotSets->cacheEntries[iI].plotSet);
		}
	}
}

CvReachablePlotSet* CvReachablePlotSet::findCachedPlotSet(CvSelectionGroup* pGroup, int iFlags, int iOutsideOwnedRange)
{
	if ( CachedPlotSets == NULL )
	{
		CachedPlotSets = new PlotSetCache;

		CachedPlotSets->iNextLRU = 1;

		for(int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
		{
			CachedPlotSets->cacheEntries[iI].iLRUSeq = 0;
			CachedPlotSets->cacheEntries[iI].plotSet = NULL;
		}
	}

	CvReachablePlotSetCacheEntry*	pLRUEntry = NULL;
	int iBestLRU = MAX_INT;

	for(int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
	{
		CvReachablePlotSetCacheEntry* entry = &CachedPlotSets->cacheEntries[iI];

		if ( entry->plotSet == NULL )
		{
			pLRUEntry = entry;
			break;
		}
		else if ( entry->plotSet->m_group == pGroup && entry->plotSet->m_iFlags == iFlags && entry->plotSet->m_iOutsideOwnedRange == iOutsideOwnedRange)
		{
			entry->iLRUSeq = CachedPlotSets->iNextLRU++;
			return entry->plotSet;
		}
		else if ( entry->iLRUSeq < iBestLRU )
		{
			iBestLRU = entry->iLRUSeq;
			pLRUEntry = entry;
		}
	}

	SAFE_DELETE(pLRUEntry->plotSet);

	pLRUEntry->plotSet = new CvReachablePlotSet(pGroup, iFlags);
	pLRUEntry->plotSet->m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
	pLRUEntry->iLRUSeq = CachedPlotSets->iNextLRU++;

	return pLRUEntry->plotSet;
}

void CvReachablePlotSet::Populate(int iRange)
{
	PROFILE_FUNC();

	MEMORY_TRACK_EXEMPT();

	if ( m_bCachable )
	{
		m_proxyTo = findCachedPlotSet(m_group, m_iFlags, m_iOutsideOwnedRange);
	}
	else
	{
		if ( m_reachablePlots == NULL )
		{
			m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
		}
		else
		{
			m_reachablePlots->clear();
		}
	}

	m_iRange = getRange();

	if ( m_iRange < iRange )
	{
		std::vector<std::pair<CvPlot*,int> > seedPlots;

		if ( m_iRange == -1 )
		{
			stdext::hash_map<CvPlot*, CvReachablePlotInfo>* reachablePlots = (m_proxyTo == NULL ? m_reachablePlots : m_proxyTo->m_reachablePlots);

			CvReachablePlotInfo	info;

			memset(&info, 0, sizeof(info));

			(*reachablePlots)[m_group->plot()] = info;
			seedPlots.push_back(std::make_pair(m_group->plot(),0));

			m_iRange = 0;
			setRange(0);
		}
		else
		{
			for(CvReachablePlotSet::const_iterator itr = begin(), itrend = end(); itr != itrend; ++itr)
			{
				if ( itr.stepDistance() == m_iRange )
				{
					seedPlots.push_back(std::make_pair(itr.plot(), itr.outsideBorderDistance()));
				}
			}
		}

		enumerateReachablePlotsInternal(iRange - getRange(), getRange()+1, seedPlots);

		setRange(iRange);
	}

	m_iRange = iRange;
}

bool CvReachablePlotSet::canMoveBetweenWithFlags(CvSelectionGroup* group, CvPlot* pFromPlot, CvPlot* pToPlot, int iFlags)
{
	if (group->getDomainType() == DOMAIN_SEA)
	{
		//	Can't cross diagonally across 'land'
		if (pFromPlot->isWater() && pToPlot->isWater())
		{
			if (!(GC.getMapINLINE().plotINLINE(pFromPlot->getX_INLINE(), pToPlot->getY_INLINE())->isWater()) && !(GC.getMapINLINE().plotINLINE(pToPlot->getX_INLINE(), pFromPlot->getY_INLINE())->isWater()))
			{
				if( !(group->canMoveAllTerrain()) )
				{
					return false;
				}
			}
		}

		//	If told to allow adjacent coasts to be included in the generated set
		//	do so now
		if ( (MOVE_ALLOW_ADJACENT_COASTAL & iFlags) != 0 )
		{
			if (pFromPlot->isWater() && pToPlot->isCoastalLand())
			{
				return true;
			}
			else if ( !pFromPlot->isWater() && !pFromPlot->isFriendlyCity(*group->getHeadUnit(), true) )
			{
				return false;
			}
		}
	}

	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ZOC))
	{
		//	Need to handle ZOCs
		//	ZOCs don't apply into cities of the unit owner
		TeamTypes	eTeam = group->getTeam();
		PlayerTypes eOwner = group->getHeadOwner();
	
		if ( pToPlot->getPlotCity() == NULL || pToPlot->getPlotCity()->getTeam() != eTeam )
		{
			//Fort ZOC
			PlayerTypes eDefender = pFromPlot->controlsAdjacentZOCSource(eTeam);
			if (eDefender != NO_PLAYER)
			{
				const CvPlot* pZoneOfControl = pFromPlot->isInFortControl(true, eDefender, eOwner);
				const CvPlot* pForwardZoneOfControl = pToPlot->isInFortControl(true, eDefender, eOwner);
				if (pZoneOfControl != NULL && pForwardZoneOfControl != NULL)
				{
					if (pZoneOfControl == pToPlot->isInFortControl(true, eDefender, eOwner, pZoneOfControl) && !group->canIgnoreZoneofControl())
					{
						return false;
					}
				}
			}
			
			//City ZoC
			if (pFromPlot->isInCityZoneOfControl(eOwner) && pToPlot->isInCityZoneOfControl(eOwner) && !group->canIgnoreZoneofControl())
			{
				return false;
			}
		}
		//Promotion ZoC
		if (GC.getGameINLINE().isAnyoneHasUnitZoneOfControl())
		{
			if (pFromPlot->isInUnitZoneOfControl(eOwner) && pToPlot->isInUnitZoneOfControl(eOwner) && !group->canIgnoreZoneofControl())
			{
				return false;
			}
		}
	}

	return moveToValid(group, pToPlot, iFlags);
}

void CvReachablePlotSet::enumerateReachablePlotsInternal(int iRange, int iDepth, std::vector< std::pair<CvPlot*,int> >& prevRing)
{
	if ( iRange > 0 && !prevRing.empty() )
	{
		std::vector<std::pair<CvPlot*,int> >	nextRing;

		stdext::hash_map<CvPlot*, CvReachablePlotInfo>* reachablePlots = (m_proxyTo == NULL ? m_reachablePlots : m_proxyTo->m_reachablePlots);

		for( std::vector<std::pair<CvPlot*,int> >::const_iterator itr = prevRing.begin(), itrend = prevRing.end(); itr != itrend; ++itr)
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pAdjacentPlot = plotDirection((*itr).first->getX_INLINE(), (*itr).first->getY_INLINE(), ((DirectionTypes)iI));

				if ( pAdjacentPlot != NULL && reachablePlots->find(pAdjacentPlot) == reachablePlots->end() )
				{
					bool bValidAsTerminus = false;
					bool bValid = ContextFreeNewPathValidFunc(m_group, (*itr).first->getX_INLINE(), (*itr).first->getY_INLINE(), pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), m_iFlags, false, false, 0, NULL, &bValidAsTerminus);

					if ( !bValid )
					{
						bool bDummy;

						bValidAsTerminus |= NewPathDestValid(m_group, pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), m_iFlags, bDummy);
					}

					//if ( canMoveBetweenWithFlags(m_group, (*itr), pAdjacentPlot, m_iFlags) )
					if ( bValid || bValidAsTerminus )
					{
						CvReachablePlotInfo	info;

						memset(&info, 0, sizeof(info));
						info.iStepDistance = iDepth;

						if ( (*itr).first->getTeam() != m_group->getTeam() )
						{
							info.iOutsideOwnedDistance = (*itr).second + 1;

							if ( m_iOutsideOwnedRange != -1 && info.iOutsideOwnedDistance >= m_iOutsideOwnedRange )
							{
								bValid = false;
							}
						}
						else
						{
							info.iOutsideOwnedDistance = 0;
						}

						(*reachablePlots)[pAdjacentPlot] = info;

						if ( bValid && pAdjacentPlot->isRevealed(m_group->getTeam(), false) )
						{
							nextRing.push_back(std::make_pair(pAdjacentPlot, info.iOutsideOwnedDistance));
						}
					}
				}
			}
		}

		enumerateReachablePlotsInternal(iRange - 1, iDepth + 1, nextRing);
	}
}
