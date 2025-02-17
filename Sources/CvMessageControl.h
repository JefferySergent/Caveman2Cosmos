#ifndef CV_MESSAGE_CONTROL
#define CV_MESSAGE_CONTROL

class CvMessageControl
{
public:
	DllExport static CvMessageControl& getInstance();
	void sendExtendedGame();
	void sendAutoMoves();
	void sendTurnComplete();
	void sendPushOrder(int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
	void sendPopOrder(int iCity, int iNum);
	void sendDoTask(int iCityID, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void sendUpdateCivics(const std::vector<CivicTypes>& aeCivics);
	void sendResearch(TechTypes eTech, int iDiscover, bool bShift);
	void sendEspionageSpendingWeightChange(TeamTypes eTargetTeam, int iChange);
	DllExport void sendAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd);
	void sendModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5);
	void sendConvert(ReligionTypes eReligion);
	void sendEmpireSplit(PlayerTypes ePlayer, int iAreaId);
	void sendFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, ReligionTypes eSlotReligion);
	DllExport void sendLaunch(PlayerTypes ePlayer, VictoryTypes eVictory);
	void sendEventTriggered(PlayerTypes ePlayer, EventTypes eEvent, int iEventTriggeredId);
	DllExport void sendJoinGroup(int iUnitID, int iHeadID);
	void sendPushMission(int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift);
	void sendAutoMission(int iUnitID);
	void sendDoCommand(int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt);
	void sendPercentChange(CommerceTypes eCommerce, int iChange);
	void sendChangeVassal(TeamTypes eMasterTeam, bool bVassal, bool bCapitulated);
	void sendChooseElection(int iSelection, int iVoteId);
	void sendDiploVote(int iVoteId, PlayerVoteTypes eChoice);
	DllExport void sendChangeWar(TeamTypes eRivalTeam, bool bWar);
	DllExport void sendPing(int iX, int iY);

// BUG - Reminder Mod - start
	void sendAddReminder(PlayerTypes ePlayer, int iGameTurn, CvWString szMessage);
// BUG - Reminder Mod - end
/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void sendGlobalDefineUpdate(CvWString szName, int iValue, float fValue, CvWString szValue);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	void sendRecalculateModifiers();
	void sendBuildLists();
	void sendBuildListPushOrder(int iID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
	void sendBuildListPopOrder(int iID, int iNum);
	void sendBuildListEdit(int iID, CvString szName, bool bDelete = false);
	void sendChooseBuildUp(PromotionLineTypes ePromotionLine, int iID);
	void sendChooseTrait(TraitTypes eTrait, bool bNewValue);
	void sendMergeUnit(int iUnitID);
	void sendSplitUnit(bool bConfirm);
	void sendImprovementUpgrade(ImprovementTypes eImprovement, int iX, int iY, bool bConfirm);
	void sendArrestUnit(int iUnitID, PlayerTypes ePlayerT);
	void sendAmbushConfirmation(bool bConfirm);
};


#endif