from CvPythonExtensions import *

def GameFontScreen():
	import CvScreenEnums
	import ScreenResolution
	xRes = ScreenResolution.x
	yRes = ScreenResolution.y
	GC = CyGlobalContext()
	GAME = GC.getGame()

	screen = CyGInterfaceScreen("GameFontScreen", CvScreenEnums.GAMEFONT_SCREEN)
	screen.addPanel("", "", "", True, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)

	TABLE = "GameFontTable"
	screen.addTableControlGFC(TABLE, 5, (xRes-768)/2, 0, 768, yRes, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
	screen.setTableColumnHeader(TABLE, 0, "ID", 64)
	screen.setTableColumnHeader(TABLE, 1, "Small", 64)
	screen.setTableColumnHeader(TABLE, 2, "Big", 64)
	screen.setTableColumnHeader(TABLE, 3, "Button", 64)
	screen.setTableColumnHeader(TABLE, 4, "Type", 512)

	eWidGen = WidgetTypes.WIDGET_GENERAL
	iRandom = GAME.getSymbolID(FontSymbols.RANDOM_CHAR)
	iHappy = GAME.getSymbolID(FontSymbols.HAPPY_CHAR)
	aList0 = [
		"HAPPY_CHAR",
		"UNHAPPY_CHAR",
		"HEALTHY_CHAR",
		"UNHEALTHY_CHAR",
		"BULLET_CHAR",
		"STRENGTH_CHAR",
		"MOVES_CHAR",
		"RELIGION_CHAR",
		"STAR_CHAR",
		"SILVER_STAR_CHAR",
		"TRADE_CHAR",
		"DEFENSE_CHAR",
		"GREAT_PEOPLE_CHAR",
		"BAD_GOLD_CHAR",
		"BAD_FOOD_CHAR",
		"EATEN_FOOD_CHAR",
		"GOLDEN_AGE_CHAR",
		"ANGRY_POP_CHAR",
		"OPEN_BORDERS_CHAR",
		"DEFENSIVE_PACT_CHAR",
		"MAP_CHAR",
		"OCCUPATION_CHAR",
		"POWER_CHAR",
		"CITIZEN_CHAR",
		"GREAT_GENERAL_CHAR",
		"AIRPORT_CHAR",
		"ANGRY_CIV_CHAR",
		"UNHAPPY_CIV_CHAR",
		"NORMAL_CIV_CHAR",
		"HAPPY_CIV_CHAR",
		"VERYHAPPY_CIV_CHAR",
		"CROSSED_CHAR",
		"RANDOM_CHAR"
	]
	aList1 = []
	for i in range(GC.getNumReligionInfos()):
		info = GC.getReligionInfo(i)
		aList1.append((info.getChar(), info))

	for i in range(GC.getNumCorporationInfos()):
		info = GC.getCorporationInfo(i)
		aList1.append((info.getChar(), info))

	for i in range(GC.getNumBonusInfos()):
		info = GC.getBonusInfo(i)
		aList1.append((info.getChar(), info))
	iMax = len(aList1)

	for iRow in xrange(650):
		iID = iRow + 8483
		screen.appendTableRow(TABLE)
		screen.setTableText(TABLE, 0, iRow , str(iID), "", eWidGen, 1, 1, 1<<0)
		screen.setTableText(TABLE, 1, iRow , unichr(iID), "", eWidGen, 1, 1, 1<<0)
		screen.setTableText(TABLE, 2, iRow , u"<font=4>%c" % iID, "", eWidGen, 1, 1, 1<<0)

		if iID >= iHappy and iID <= iRandom:
			screen.setTableText(TABLE, 4, iRow , aList0[iID - iHappy], "", eWidGen, 1, 2, 1<<0)
			continue
		i = 0
		while i < iMax:
			if aList1[i][0] == iID:
				info = aList1.pop(i)[1]
				iMax -= 1
				break
			i += 1
		else: continue

		screen.setTableText(TABLE, 3, iRow, "", info.getButton(), eWidGen, 1, 2, 1<<0)
		screen.setTableText(TABLE, 4, iRow, info.getType(), "", eWidGen, 1, 2, 1<<0)

	screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)