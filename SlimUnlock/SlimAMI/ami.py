from pywinauto_recorder.player import *

with UIPath(u"AMIBCP Version 5.02.0031 - [bios.bin]||Window"):
	with UIPath(u"Рабочее пространство||Pane->bios.bin||Window->||Tree"):
		double_click(u"Common RefCode Configuration||TreeItem")
	with UIPath(u"Рабочее пространство||Pane->bios.bin||Window->||Tree->Common RefCode Configuration||TreeItem"):
		double_click(u"IntelRCSetup||TreeItem")
	with UIPath(u"Рабочее пространство||Pane->bios.bin||Window->||Tree->Common RefCode Configuration||TreeItem->IntelRCSetup||TreeItem"):
		double_click(u"Advanced Power Management Configuration||TreeItem")
	with UIPath(u"Рабочее пространство||Pane->bios.bin||Window"):
		double_click(u"||Tree->Common RefCode Configuration||TreeItem->IntelRCSetup||TreeItem->Advanced Power Management Configuration||TreeItem->CPU C State Control||TreeItem")
	with UIPath(u"Рабочее пространство||Pane->bios.bin||Window->||List->(158B)||ListItem"):
		double_click(u"ListItem->C6(Retention) state||Text")
		double_click(u"C2 state||ListItem")
