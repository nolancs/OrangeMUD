##############################################################################
# Makefile
# 
# make DEBUG=<level>: 0 default. 2 will allow you to debug in GDB.
# make FINAL=<level>: 0 default. Speed optmizations, 3 is the max.
##############################################################################
APPNAME		= OrangeMUD
ORANGEDIR	= Source
CLOCKWDIR	= Clockwork
COMPILER       ?= g++

##############################################################################
# Determine Which UNIX We Are On
##############################################################################
ifeq ($(OSTYPE), solaris2.7)
OS ?= solaris
endif
ifeq ($(OSTYPE), solaris2.8)
OS ?= solaris
endif
ifeq ($(OSTYPE), solaris)
OS ?= solaris
endif
ifeq ($(HOSTTYPE), macintosh)
OS ?= darwin
COMPILER = cc
endif
OS ?= linux

##############################################################################
# Settings
##############################################################################
WARNINGS    = -Wall -fpermissive -Wno-reorder -Wno-constant-conversion
WARNINGS   += -Wno-writable-strings -Wno-write-strings -Wno-deprecated 
WARNINGS   += -Wno-char-subscripts -Wno-format-security
DEBUG	   ?= 0
FINAL	   ?= 0
FLAGS       = -c
FLAGS      += -DDEBUG
FLAGS      += -I$(ORANGEDIR) -I$(CLOCKWDIR) 
FLAGS      += -include $(ORANGEDIR)/SettingsUnix.h
FLAGS      += -ggdb$(DEBUG) -std=c++11
FLAGS	   += -O$(FINAL)
LINKFLAGS   =
OBJFILES    = $(ORANGEDIR)/*.o $(CLOCKWDIR)/*.o
CPPFILES    = $(ORANGEDIR)/*.cpp $(CLOCKWDIR)/*.cpp
HDRFILES    = $(ORANGEDIR)/*.h $(CLOCKWDIR)/*.h
DEPFILE     = Makefile.dep
LIBS        = -lpthread

ifeq ($(OS), darwin)
LIBS += -lstdc++
endif

ifneq ($(FINAL), 0)
#LINKFLAGS  += -s -Wl\,-O1
endif

$(APPNAME): Orange Clockwork
	$(COMPILER) -o $(APPNAME) $(LINKFLAGS) $(OBJFILES) $(LIBS)

clean:
	rm -rf $(ORANGEDIR)/*.o

cleaner:
	rm -rf $(OBJFILES)

effort lines:
	wc -l Makefile $(HDRFILES) $(CPPFILES)

$(DEPFILE) depends depend dep deps:
	rm -rf $(DEPFILE)
	echo "# Dependency file" > $(DEPFILE)
	makedepend -Y -f$(DEPFILE) -- $(FLAGS) -w $(CPPFILES) 1>&2 2>/dev/null
	rm -rf $(DEPFILE).bak
	@echo "####################################"
	@echo "## Dependency Information Updated ##"
	@echo "####################################"

%.o : %.cpp
	$(COMPILER) -o $@ $(FLAGS) $(WARNINGS) $<

Orange: $(ORANGEDIR)/Act.o
Orange: $(ORANGEDIR)/ActThing.o
Orange: $(ORANGEDIR)/Ambience.o
Orange: $(ORANGEDIR)/ANSI.o
Orange: $(ORANGEDIR)/Area.o
Orange: $(ORANGEDIR)/Attack.o
Orange: $(ORANGEDIR)/Balance.o
Orange: $(ORANGEDIR)/Channel.o
Orange: $(ORANGEDIR)/Classes.o
Orange: $(ORANGEDIR)/CmdsAbilities.o
Orange: $(ORANGEDIR)/CmdsBattle.o
Orange: $(ORANGEDIR)/CmdsCommunication.o
Orange: $(ORANGEDIR)/CmdsImmortal.o
Orange: $(ORANGEDIR)/CmdsInformation.o
Orange: $(ORANGEDIR)/CmdsMovement.o
Orange: $(ORANGEDIR)/CmdsObjects.o
Orange: $(ORANGEDIR)/Conversion.o
Orange: $(ORANGEDIR)/Exit.o
Orange: $(ORANGEDIR)/ExitPopulation.o
Orange: $(ORANGEDIR)/Fight.o
Orange: $(ORANGEDIR)/Help.o
Orange: $(ORANGEDIR)/HelpFile.o
Orange: $(ORANGEDIR)/Immortal.o
Orange: $(ORANGEDIR)/Interpret.o
Orange: $(ORANGEDIR)/LoadOld.o
Orange: $(ORANGEDIR)/Main.o
Orange: $(ORANGEDIR)/MapGenerator.o
Orange: $(ORANGEDIR)/Mobile.o
Orange: $(ORANGEDIR)/MobileIndex.o
Orange: $(ORANGEDIR)/MobilePopulation.o
Orange: $(ORANGEDIR)/MUD.o
Orange: $(ORANGEDIR)/MUDFile.o
Orange: $(ORANGEDIR)/Object.o
Orange: $(ORANGEDIR)/ObjectHolder.o
Orange: $(ORANGEDIR)/ObjectIndex.o
Orange: $(ORANGEDIR)/ObjectPopulation.o
Orange: $(ORANGEDIR)/ObjectProxy.o
Orange: $(ORANGEDIR)/OLC.o
Orange: $(ORANGEDIR)/OLC_Area.o
Orange: $(ORANGEDIR)/OLC_Mobile.o
Orange: $(ORANGEDIR)/OLC_Object.o
Orange: $(ORANGEDIR)/OLC_Room.o
Orange: $(ORANGEDIR)/OrangeSession.o
Orange: $(ORANGEDIR)/OrangeTCP.o
Orange: $(ORANGEDIR)/Part.o
Orange: $(ORANGEDIR)/Person.o
Orange: $(ORANGEDIR)/PersonHolder.o
Orange: $(ORANGEDIR)/Player.o
Orange: $(ORANGEDIR)/Population.o
Orange: $(ORANGEDIR)/Races.o
Orange: $(ORANGEDIR)/RoomIndex.o
Orange: $(ORANGEDIR)/SaveLoadable.o
Orange: $(ORANGEDIR)/ScriptedThing.o
Orange: $(ORANGEDIR)/OrangeScript.o
Orange: $(ORANGEDIR)/OrangeScriptBinary.o
Orange: $(ORANGEDIR)/OrangeScriptCompiler.o
Orange: $(ORANGEDIR)/OrangeScriptDataType.o
Orange: $(ORANGEDIR)/OrangeScriptLibrary.o
Orange: $(ORANGEDIR)/OrangeScriptVariable.o
Orange: $(ORANGEDIR)/Skin.o
Orange: $(ORANGEDIR)/Social.o
Orange: $(ORANGEDIR)/SocialFile.o
Orange: $(ORANGEDIR)/Spot.o
Orange: $(ORANGEDIR)/SpottedThing.o
Orange: $(ORANGEDIR)/StringCW.o
Orange: $(ORANGEDIR)/StringMagic.o
Orange: $(ORANGEDIR)/Tables.o
Orange: $(ORANGEDIR)/TEditor.o
Orange: $(ORANGEDIR)/UIDHolder.o
Orange: $(ORANGEDIR)/VRefThing.o

Clockwork: $(CLOCKWDIR)/EndianUtils.o
Clockwork: $(CLOCKWDIR)/Exception.o
Clockwork: $(CLOCKWDIR)/FileLink.o
Clockwork: $(CLOCKWDIR)/FileManager.o
Clockwork: $(CLOCKWDIR)/FileManagerUnix.o
Clockwork: $(CLOCKWDIR)/MemFileLink.o
Clockwork: $(CLOCKWDIR)/OSUtilsUnix.o
Clockwork: $(CLOCKWDIR)/Script.o
Clockwork: $(CLOCKWDIR)/ScriptBinary.o
Clockwork: $(CLOCKWDIR)/ScriptCodeLine.o
Clockwork: $(CLOCKWDIR)/ScriptCompiler.o
Clockwork: $(CLOCKWDIR)/ScriptConditional.o
Clockwork: $(CLOCKWDIR)/ScriptDataType.o
Clockwork: $(CLOCKWDIR)/ScriptExecutor.o
Clockwork: $(CLOCKWDIR)/ScriptFunction.o
Clockwork: $(CLOCKWDIR)/ScriptLibrary.o
Clockwork: $(CLOCKWDIR)/ScriptVariable.o
Clockwork: $(CLOCKWDIR)/StringUtils.o
Clockwork: $(CLOCKWDIR)/TCPLink.o
Clockwork: $(CLOCKWDIR)/TCPLinkUnix.o
Clockwork: $(CLOCKWDIR)/TCPManager.o
Clockwork: $(CLOCKWDIR)/TCPManagerUnix.o
Clockwork: $(CLOCKWDIR)/XMLFile.o


#########################################################################
tar:
	make cleaner
	rm -rf cwodist.tar cwodist.tar.gz cwodist
	rm -rf $(ORANGEDIR)/*PCH++
	rm -rf $(CLOCKWDIR)/ClockworkLib*
	rm -rf $(CLOCKWDIR)/Clockwork\ Data
	mkdir cwodist
	cp -r $(ORANGEDIR) cwodist/
	cp -r $(CLOCKWDIR) cwodist/
	cp -r AgeOfWonderV2 cwodist/
	cp OrangeMUD.mcp cwodist/
	cp Makefile cwodist/
	cp Makefile.dep cwodist/
	tar -cvf cwodist.tar cwodist/*
	gzip cwodist.tar
	rm -rf cwodist
	@echo "#############################"
	@echo "Done"
	@echo ""
	
#########################################################################
ifndef A
A=cwodist.tar.gz
endif
WORKNAME=Source-New

from_unused_:
	mkdir Temp
	cp $(A) $(WORKNAME).tar.gz
	gunzip $(WORKNAME).tar.gz
	tar -xf $(WORKNAME).tar --directory Temp
	rm -f $(WORKNAME).tar
	mv Temp/* $(WORKNAME)
	rm -rf Temp
	cp -f $(WORKNAME)/Makefile Makefile
	cp -f $(WORKNAME)/*.cpp $(WORKNAME)/*.h $(ORANGEDIR)/
	cp -f $(WORKNAME)/~Clockwork/*.cpp $(WORKNAME)/~Clockwork/*.h $(CLOCKWDIR)/
	rm -rf $(WORKNAME)

#########################################################################
	
include $(DEPFILE)

