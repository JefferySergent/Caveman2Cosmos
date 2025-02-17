#pragma once

// plotGroup.h

#ifndef CIV4_PLOT_GROUP_H
#define CIV4_PLOT_GROUP_H

//#include "CvStructs.h"
#include "LinkedList.h"
#include "CvGame.h"
#include "CvGameAI.h"

typedef struct
{
	int resourceNodesHash;
	int allNodesHash;
} plotGroupHashInfo;

typedef struct
{
	plotGroupHashInfo	hashInfo;
	int					groupGenerationNumber;
} plotGroupCheckInfo;

class CvPlot;
class CvPlotGroup
{
friend class CvPlot;

public:

	CvPlotGroup();
	virtual ~CvPlotGroup();


	void init(int iID, PlayerTypes eOwner, CvPlot* pPlot, bool bRecalculateBonuses = true);
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall=false);

	void addPlot(CvPlot* pPlot, bool bRecalculateBonuses);
	void removePlot(CvPlot* pPlot, bool bRecalculateBonuses = true);
	void recalculatePlots();														

	int getID() const;
	void setID(int iID);

	PlayerTypes getOwner() const;
#ifdef _USRDLL
	inline PlayerTypes getOwnerINLINE() const
	{
		return m_eOwner;
	}
#endif
	int getNumBonuses(BonusTypes eBonus) const;
	bool hasBonus(BonusTypes eBonus);										
	void changeNumBonuses(BonusTypes eBonus, int iChange);

	int getNumCities();

	// for serialization
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
	//	Calculate the hashes after a fresh game load
	void RecalculateHashes();
	void Validate();

	inline int getLengthPlots() { return m_numPlots; }

	void mergeIn(CvPlotGroup* from, bool bRecalculateBonuses);
	static void colorRegion(CvPlot* pStartPlot, PlayerTypes eOwner, bool bRecalculateBonuses);
	static void startBulkRecalculate();
	static void endBulkRecalculate();

private:
	CvPlot* getRepresentativePlot() const;
	void plotEnumerator(bool (*pfFunc)(CvPlotGroup* onBehalfOf, CvPlot*, void*), void* param);
	static CvPlotGroup* colorRegionInternal(CvPlot* pPlot, PlayerTypes eOwner, CvPlotGroup* pPlotGroup, bool bRecalculateBonuses);

	int m_sessionAllocSeq;
	int m_sessionRecalcSeq;
	static int m_allocationSeqForSession;
	static int m_recalcSeqForSession;
	static int m_bulkRecalcStartSeq;

protected:

	int m_iID;

	PlayerTypes m_eOwner;

	int* m_paiNumBonuses;
	mutable int m_seedPlotX;
	mutable int m_seedPlotY;

	plotGroupHashInfo m_zobristHashes;
						//	XOR of the zobrist contributions from all
						//	constituent plots 
public:
	int m_numPlots;
	mutable int m_numCities;
	//CLinkList<XYCoords> m_plots;
};

#endif
