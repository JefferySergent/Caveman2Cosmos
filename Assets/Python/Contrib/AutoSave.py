## AutoSave
from CvPythonExtensions import *
GAME = CyGame()

def init():
	import SystemPaths as SP
	global _saveDir, options
	_saveDir = SP.rootDir + "\\Saves"
	import BugCore
	options = BugCore.game.AutoSave
	import CvEventInterface
	CvEventInterface.eventManager.addEventHandler("endTurnReady", onEndTurnReady)

def onEndTurnReady(argsList):
	iTurn, = argsList
	inter = options.getInterval()
	if inter < 2 or not iTurn % inter:
		autoSave("[Late]", iTurn)

def save(type, prefix, iTurn):

	dir = "%s\\%s\\" %(_saveDir, type)
	if prefix:
		dir += prefix + "'"
	MAP = CyMap()
	GC = CyGlobalContext()
	CyPlayer = GC.getActivePlayer()

	if iTurn:
		dir += "#" + str(iTurn)
	else:
		dir += MAP.getMapScriptName()

	if options.isUsePlayerName():
		dir += '-' + CyPlayer.getName()[:8]
	else:
		dir += '-' + GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getText()[:8]

	dir += '-' + GC.getEraInfo(GAME.getCurrentEra()).getText()[:8]
	dir += '-' + GC.getGameSpeedInfo(GAME.getGameSpeedType()).getText()[:5]
	dir += '-' + GC.getWorldInfo(MAP.getWorldSize()).getText()[:5]
	dir += '-' + GC.getHandicapInfo(CyPlayer.getHandicapType()).getText()[:5]
	dir += ".CivBeyondSwordSave"

	GAME.saveGame(str(dir))
	print "AutoSave.save\n\t%s" % dir

def autoSave(prefix="", iTurn=None):
	# Filter by option
	if(prefix == "[Start]" and not options.isCreateStartSave()
	or prefix == "[End]" and not options.isCreateEndSave()
	or prefix == "[Exit]" and not options.isCreateExitSave()
	or prefix == "[Late]" and not options.isCreateLateSave()
	):
		return
	if iTurn is None:
		iTurn = GAME.getGameTurn()
	# pick folder
	if GAME.isGameMultiPlayer():
		if GAME.isHotSeat():
			type = "hotseat"
		elif GAME.isPbem():
			type = "pbem"
		elif GAME.isPitboss():
			type = "pitboss"
		else:
			type = "multi"
	else:
		type = "single"
	type += "\\auto"
	save(type, prefix, iTurn)
