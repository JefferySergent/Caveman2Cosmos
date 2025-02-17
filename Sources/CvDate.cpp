//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvDate.h
//
//  PURPOSE: Class to keep a Civ4 date and methods related to the time/turn relationship
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvDate.h"

CvDate::CvDate()
{
	m_iTick = 0;
}

CvDate::CvDate(unsigned int iTick) :
m_iTick(iTick)
{
}

int CvDate::getYear() const
{
	return GC.getGameINLINE().getStartYear() + (m_iTick / 360);
}

int CvDate::getDay() const
{
	return ((m_iTick % 360) % 30) + 1;
}

unsigned int CvDate::GetTick() const
{
	return m_iTick;
}

int CvDate::getMonth() const
{
	return (m_iTick % 360) / 30;
}

int CvDate::getWeek() const
{
	return ((getDay() - 1) / 7) + 1;
}

SeasonTypes CvDate::getSeason() const
{
	int month = getMonth();

	if (month <= 1 || month >= 11)
	{
		return SEASON_WINTER; // Winter
	}
	if (month >= 2 && month <= 4)
	{
		return SEASON_SPRING; // Spring
	}
	if (month >= 5 && month <= 7)
	{
		return SEASON_SUMMER; // Summer
	}
	if (month >= 8 && month <= 10)
	{
		return SEASON_AUTUMN; // Autumn
	}
	return NO_SEASON; // This will never be executed
}

CvDateIncrement CvDate::getIncrement(GameSpeedTypes eGameSpeed) const
{
	GameSpeedTypes eActualGameSpeed = eGameSpeed;
	if (eGameSpeed == NO_GAMESPEED)
	{
		eActualGameSpeed = GC.getGameINLINE().getGameSpeedType();
	}
	CvGameSpeedInfo& kInfo = GC.getGameSpeedInfo(eActualGameSpeed);
	std::vector<CvDateIncrement>& aIncrements = kInfo.getIncrements();
	if (!kInfo.getEndDatesCalculated())
	{
		calculateEndDates(eActualGameSpeed);
	}

	for (std::vector<CvDateIncrement>::iterator it = aIncrements.begin(); it != aIncrements.end(); ++it)
	{
		if (*this < it->m_endDate)
		{
			return *it;
		}
	}
	return aIncrements[aIncrements.size()-1];
}

void CvDate::increment(GameSpeedTypes eGameSpeed)
{
	CvDateIncrement inc = getIncrement(eGameSpeed);
	m_iTick += inc.m_iIncrementDay;
	m_iTick += (inc.m_iIncrementMonth * 30);
}

void CvDate::increment(int iTurns, GameSpeedTypes eGameSpeed)
{
	for (int i=0; i<iTurns; i++)
	{
		increment(eGameSpeed);
	}
}

bool CvDate::operator !=(const CvDate &kDate) const
{
	return !(*this == kDate);
}

bool CvDate::operator ==(const CvDate &kDate) const
{
	return m_iTick == kDate.GetTick();
}

bool CvDate::operator <(const CvDate &kDate) const
{
	return m_iTick < kDate.GetTick();
}

bool CvDate::operator >(const CvDate &kDate) const
{
	return m_iTick > kDate.GetTick();
}

bool CvDate::operator <=(const CvDate &kDate) const
{
	return ! (*this > kDate);
}

bool CvDate::operator >=(const CvDate &kDate) const
{
	return ! (*this < kDate);
}

CvDate CvDate::getStartingDate()
{
	return CvDate();
}

CvDate CvDate::getDate(int iTurn, GameSpeedTypes eGameSpeed)
{
	CvDate date;
	int iRemainingTurns = 0;

	GameSpeedTypes eActualGameSpeed = eGameSpeed;
	if (eGameSpeed == NO_GAMESPEED)
	{
		eActualGameSpeed = GC.getGameINLINE().getGameSpeedType();
	}
	CvGameSpeedInfo& kInfo = GC.getGameSpeedInfo(eActualGameSpeed);
	std::vector<CvDateIncrement>& aIncrements = kInfo.getIncrements();
	if (!kInfo.getEndDatesCalculated())
	{
		calculateEndDates(eActualGameSpeed);
	}
	

	for (int i=0; i<(int)aIncrements.size(); i++)
	{
		if (iTurn <= aIncrements[i].m_iendTurn)
		{
			if (i==0)
			{
				iRemainingTurns = iTurn;
				date = getStartingDate();
			}
			else
			{
				iRemainingTurns = iTurn - aIncrements[i-1].m_iendTurn;
				date = aIncrements[i-1].m_endDate;
			}
			break;
		}
		else if (i==(int)aIncrements.size())
		{
			iRemainingTurns = iTurn - aIncrements[i].m_iendTurn;
			date = aIncrements[i].m_endDate;
		}
	}
	date.increment(iRemainingTurns, eGameSpeed);
	return date;
}

void CvDate::calculateEndDates(GameSpeedTypes eGameSpeed)
{
	CvGameSpeedInfo& kInfo = GC.getGameSpeedInfo(eGameSpeed);
	std::vector<CvDateIncrement>& aIncrements = kInfo.getIncrements();
	kInfo.setEndDatesCalculated(true);
	for (int i=0; i<(int)aIncrements.size(); i++)
	{
		aIncrements[i].m_endDate = CvDate(MAX_UNSIGNED_INT);
		aIncrements[i].m_endDate = getDate(aIncrements[i].m_iendTurn, eGameSpeed);
	}
}