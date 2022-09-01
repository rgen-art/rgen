#!/usr/bin/python
##-----------------------------------------------------------------------------
## rgen.art
##-----------------------------------------------------------------------------
import os
import re
import sys

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
##TODO:ouput line numbers
##TODO:command line switch
##  --underflow
##  --linelength
##  --lineending
##  --headerguard
##  --footer
##  --ignore
##TODO: EMP_ATTRIBUTE_UNUSED
##TODO: indentation
##TODO: Line counts
##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
##TODO:per file ignorlist
gIgnoreTokens = [ "EMP_PP_ITERATION_USER_UNDEF" ]

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
gLine_SPY = "##-----------------------------------------------------------------------------"
gLine_S0 = "//-----------------------------------------------------------------------------"
gLine_UNDEF = "#undef"
gLine_ENDIF = "#endif"
gLine_EMPTY = ""

g_szHeader = gLine_S0 + "\n" + "// rgen.art\n" + gLine_S0 + "\n"

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def IsExtension(szFile, szExtension):
    aSplit = os.path.splitext(szFile)
    if aSplit[1] == szExtension:
        return True
    return False

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def IsRFX(szFile):
    aSplit = os.path.splitext(szFile)
    if IsExtension(aSplit[0], ".rfx"):
        return True
    return False

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def IsHeaderFile(szFile):
    return  IsExtension(szFile, ".h") or IsExtension(szFile, ".hpp") or \
        IsExtension(szFile, ".def") or IsExtension(szFile, ".undef")

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def IsSourceFile(szFile):
    return  IsExtension(szFile, ".cpp") or IsExtension(szFile, ".cxx") or \
        IsExtension(szFile, ".mm")

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
gszDefine = "^\s*#\s*define\s+([^\s\\\\\(]+)"
greDefine = re.compile(gszDefine)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
gszUndef = "^\s*#\s*undef\s+([^\s\\\\\(]+)"
greUndef = re.compile(gszUndef)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def GetToken(a_szLine, a_reToken):
    mToken = a_reToken.match(a_szLine)
    if mToken:
        szToken = mToken.group(1)
        return szToken, len(szToken) > 0
    return "", False

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def ComputeHeaderGuard(a_szFile):
    return a_szFile.upper().replace(".", "_").replace("/", "_").replace("\\",
        "_")

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
##TODO:work on parsed tokens.
def CheckUnderflow( a_aUnderLines, a_szUnderFile,
                    a_reUnderToken, a_reOverToken,
                    a_szExtUnder, a_szExtOver):
    aUnderflow = []
    szOverFile = a_szUnderFile.replace(a_szExtUnder, a_szExtOver)
    szOverHeader = ComputeHeaderGuard(os.path.split(szOverFile)[1])
    pOverFile = open(szOverFile, "r")
    aOverLines = pOverFile.readlines()
    for index_0, szOverLine in enumerate(aOverLines):
        szOverToken, bOverToken = GetToken(szOverLine, a_reOverToken)
        if bOverToken and szOverToken != szOverHeader and \
            (szOverToken.find("XARG_") == 0 or szOverToken.find("XTMP_") == 0):
            bIgnore = False
            for szIgnoreToken in gIgnoreTokens:
                if szOverToken == szIgnoreToken:
                    bIgnore = True
            if bIgnore == False:
                bUnderflow = True
                for index_1, szUnderLine in enumerate(a_aUnderLines):
                    szUnderToken, bUnderToken = GetToken(szUnderLine,
                        a_reUnderToken)
                    if bUnderToken and szOverToken == szUnderToken:
                        bUnderflow = False
                if bUnderflow:
                    aUnderflow.append((index_0, "-- " + szOverToken))
    return aUnderflow, len(aUnderflow)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckLineLength(a_aLines):
    aLineLength = []
    for index, szLine in enumerate(a_aLines):
        szLine = szLine.rstrip('\n')
        if len(szLine) > 127:
            aLineLength.append((index, "++ " + szLine))
    return aLineLength, len(aLineLength)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckLineEnding(a_aLines):
    aLineEnding = []
    for index, szLine in enumerate(a_aLines):
        szLine = szLine.rstrip('\n')
        if szLine.find('\t') >= 0:
            aLineEnding.append((index, "++ " + szLine.replace('\t', '$')))
        if len(szLine) > len(szLine.rstrip()):
            aLineEnding.append((index, "++ " + szLine + "$"))
    return aLineEnding, len(aLineEnding)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckLineToken(a_aLines, a_sToken):
    aTokenFound = []
    for index, szLine in enumerate(a_aLines):
        szLine = szLine.rstrip('\n')
        if szLine.lower().find(a_sToken) >= 0:
            aTokenFound.append((index, ("++ " + szLine)))
    return aTokenFound, len(aTokenFound)

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckExternalGuard(a_aLines):
    aExternalGuard = []
    uiErrorCount = 0;
    for index, szLine in enumerate(a_aLines):
        szLine = szLine.rstrip('\n')
        if szLine.find('include') >= 0:
            if szLine.find('#include ') == 0:
                ##print szLine
                cChar = szLine[9]
                ##print cChar
                if cChar == '"':
                    iLineLen = len(szLine)
                    iBackSlash = 0
                    for iInclude in range(10, iLineLen):
                        cInclude = szLine[iInclude]
                        ##print cInclude
                        if cInclude == '"':
                            szPP = ""
                            szPPUndef = ""
                            if iInclude > 4:
                                szPP = szLine[iInclude - 4:iInclude]
                            if iInclude > 6:
                                szPPUndef = szLine[iInclude - 6: iInclude]
                            if szPP != ".def" and szPPUndef != ".undef":
                                szInclude = szLine[10:iInclude]
                                ##print szInclude
                                szGuard = ComputeHeaderGuard(
                                    os.path.split(szInclude)[1])
                                ##print szGuard
                                szPrevious = a_aLines[index - 1].rstrip('\n');
                                szNext = a_aLines[index + 1].rstrip('\n');
                                szExpPrevious = "#ifndef " + szGuard
                                szExpNext = "#endif"
                                if szPrevious != szExpPrevious:
                                    aExternalGuard.append(
                                        (index, "-- EXP : "+szExpPrevious+"$"))
                                    aExternalGuard.append(
                                        (index, "++ GOT : " + szPrevious + "$"))
                                    uiErrorCount = uiErrorCount + 1
                                if szNext != szExpNext:
                                    aExternalGuard.append(
                                        (index, "-- EXP : " + szExpNext + "$"))
                                    aExternalGuard.append(
                                        (index, "++ GOT : " + szNext + "$"))
                                    uiErrorCount = uiErrorCount + 1
                elif cChar == ' ':
                    aExternalGuard.append((index, "++ " + szLine + "$"))
                    uiErrorCount = uiErrorCount + 1
                elif cChar == '\t':
                    aExternalGuard.append((index, "++" + szLine + "$"))
                    uiErrorCount = uiErrorCount + 1
                ##else:
            else:
                aExternalGuard.append((index, "++ " + szLine + "$"))
                uiErrorCount = uiErrorCount + 1

    return aExternalGuard, uiErrorCount

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckFooter(a_aLines, a_aFooter):
    aFooterDiff = []
    iLineLen = len(a_aLines)
    iFooterLen = len(a_aFooter)
    for iFooter in range(0, iFooterLen):
        szFooterLine = a_aFooter[iFooter].rstrip('\n')
        uiLineIndex = max(0, iLineLen - iFooterLen + iFooter)
        szFileLine = ""
        if iLineLen > uiLineIndex:
            szFileLine = a_aLines[uiLineIndex].rstrip('\n')
        if szFooterLine != szFileLine:
            aFooterDiff.append((uiLineIndex, "-- EXP : " + szFooterLine))
            aFooterDiff.append((uiLineIndex, "++ GOT : " + szFileLine))
    return aFooterDiff, len(aFooterDiff) > 0

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def PrintList(a_List):
    for info in a_List:
        print "Line %d : %s" % (info[0] + 1, info[1])

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckHeader(szFile, a_bCheckTodo):
    pFile = open(szFile, "r")
    aLines = pFile.readlines()

    uiError = 0
    uiErrorHeader = 0

    szHeader = aLines[0] + aLines[1] + aLines[2]
    uiErrorHeader = 0 
    #szHeader != g_szHeader
    uiError += uiErrorHeader

    uiLineIfndef = 3
    szIfndef = aLines[uiLineIfndef].rstrip('\n')

    uiLineDefine = 4
    szDefine = aLines[uiLineDefine].rstrip('\n')

    uiErrorDefineUnderflow = 0
    uiErrorUndefUnderflow = 0

    szComputedFilename = os.path.split(szFile)[1]

    szComputedGuard = ComputeHeaderGuard(szComputedFilename)
    szCheckIfndef = "#ifndef " + szComputedGuard
    uiErrorIfndef = szCheckIfndef != szIfndef
    uiError += uiErrorIfndef

    szCheckDefine = "#define " + szComputedGuard
    uiErrorDefine = szCheckDefine != szDefine
    uiError += uiErrorDefine

    szCheckUndef = "#undef " + szComputedGuard
    iComputedFilenameLen = len(szComputedFilename)

    szPpH = ".def"
    szPpHUndef= ".undef"

    iPpHFound = szComputedFilename.rfind(szPpH)
    iPpHUndefFound = szComputedFilename.rfind(szPpHUndef)

    aFooter = []
    aDefineUnderflow = []
    aUndefUnderflow = []

    uiErrorFileExtension = 0
    if iPpHFound >= 0:
        if iPpHFound + len(szPpH) == iComputedFilenameLen:
            aFooter = [ gLine_EMPTY,
                        gLine_S0,
                        gLine_S0,
                        gLine_ENDIF,
                        gLine_EMPTY,
                        gLine_S0,
                        gLine_S0,
                        szCheckUndef,
                        gLine_EMPTY ]
        else:
            uiErrorFileExtension = 1
            uiError += uiErrorFileExtension
    elif iPpHUndefFound >= 0:
        if iPpHUndefFound + len(szPpHUndef) == iComputedFilenameLen:
            aFooter = [ gLine_EMPTY,
                        gLine_S0,
                        gLine_S0,
                        gLine_ENDIF,
                        gLine_EMPTY,
                        gLine_S0,
                        gLine_S0,
                        szCheckUndef,
                        gLine_EMPTY ]
        else:
            uiErrorFileExtension = 1
            uiError += uiErrorFileExtension
    else:
        aFooter = [ gLine_EMPTY,
        gLine_S0,
        gLine_S0,
        gLine_ENDIF,
        gLine_EMPTY ]

    uiErrorFooter = 0
    aFooterDiff, uiErrorFooter = CheckFooter(aLines, aFooter)
    uiError += uiErrorFooter

    aLineLength = 0
    uiErrorLineLength = 0
    if IsRFX(szFile) == False:
        aLineLength, uiErrorLineLength = CheckLineLength(aLines)
        uiError += uiErrorLineLength

    aLineEnding, uiErrorLineEnding  = CheckLineEnding(aLines)
    uiError += uiErrorLineEnding

    uiErrorLineTodo = 0
    if a_bCheckTodo == 1:
        aLineTodo, uiErrorLineTodo = CheckLineToken(aLines, "todo")
    uiError += uiErrorLineTodo
    
    if  uiError:
        print gLine_SPY
        print "## File : %s" % (szFile)
        if uiErrorHeader:
            print "Line %d : %s != %s" % (0, szHeader, g_szHeader)
        if uiErrorIfndef:
            print "Line %d : %s != %s" % (uiLineIfndef+1, szIfndef, szCheckIfndef)
        if uiErrorDefine:
            print "Line %d : %s != %s" % (uiLineDefine+1, szDefine, szCheckDefine)
        if uiErrorFileExtension:
            print "## Invalid file extension : %s" % (szComputedFilename)
        if uiErrorFooter:
            print "## Invalid footer"
            PrintList(aFooterDiff)
        if uiErrorDefineUnderflow:
            print "## Define underflow"
            PrintList(aDefineUnderflow)
        if uiErrorUndefUnderflow:
            print "## Undef underflow"
            PrintList(aUndefUnderflow)
        if uiErrorLineLength:
            print "## Invalid line length"
            PrintList(aLineLength)
        if uiErrorLineEnding:
            print "## Invalid line ending"
            PrintList(aLineEnding)
        if uiErrorLineTodo:
            print "## Todo found"
            PrintList(aLineTodo)
    return uiError, szDefine

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def CheckSource(szFile, a_bCheckTodo):
    pFile = open(szFile, "r")
    aLines = pFile.readlines()

    uiError = 0
    uiErrorHeader = 0

    szHeader = aLines[0] + aLines[1] + aLines[2]
    uiErrorHeader = 0
    #szHeader != g_szHeader
    uiError += uiErrorHeader

    uiLineInclude = 3
    szInclude = ""
    if len(aLines) > uiLineInclude:
        szInclude = aLines[uiLineInclude].rstrip('\n')

    szPath = os.path.split(szFile)[0]
    szComputedFilename = os.path.split(szFile)[1]

    szComputed_H = os.path.splitext(szComputedFilename)[0] + ".h"
    szComputed_HPP = os.path.splitext(szComputedFilename)[0] + ".hpp"

    szCheckInclude = "#include \"" + szComputed_H + "\""
    if os.path.isfile(szPath + "/" + szComputed_HPP):
        szCheckInclude = "#include \"" + szComputed_HPP + "\""

    uiErrorInclude = szCheckInclude != szInclude
    uiError += uiErrorInclude

    uiErrorFooter = 0
    aFooter = [ gLine_EMPTY, gLine_S0, gLine_S0, gLine_EMPTY ]
    aFooterDiff, uiErrorFooter = CheckFooter(aLines, aFooter)
    uiError += uiErrorFooter

    aLineLength, uiErrorLineLength = CheckLineLength(aLines)
    uiError += uiErrorLineLength

    aLineEnding, uiErrorLineEnding = CheckLineEnding(aLines)
    uiError += uiErrorLineEnding

    uiErrorLineTodo = 0
    if  a_bCheckTodo == 1:
        aLineTodo, uiErrorLineTodo = CheckLineToken(aLines, "todo")
    uiError += uiErrorLineTodo

    if  uiError:
        print gLine_SPY
        print "## File : %s" % (szFile)
        if uiErrorHeader:
            print "Line %d : %s != %s" % (0, szHeader, g_szHeader)
        if uiErrorInclude:
            print "Line %d : %s != %s" % (uiLineInclude+1, szInclude, szCheckInclude)
        if uiErrorFooter:
            print "## Invalid footer"
            PrintList(aFooterDiff)
        if uiErrorLineLength:
            print "## Invalid line length"
            PrintList(aLineLength)
        if uiErrorLineEnding:
            print "## Invalid line ending"
            PrintList(aLineEnding)
        if uiErrorLineTodo:
            print "## Todo found"
            PrintList(aLineTodo)
    return uiError

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def StcHeaders(a_aFiles, a_bCheckTodo):
    uiStcCount = 0
    uiStcFile = 0
    uiDuplicate = 0
    uiCount = 0
    mGuardMap = {}
    mGuardFile = {}

    for szFile in a_aFiles:
        if IsHeaderFile(szFile):
            uiCount += 1
            uiErrorCount, szGuard = CheckHeader(szFile, a_bCheckTodo)
            if szGuard in mGuardMap:
                print "Duplicate guard %s, %s, (%s)" % (
                    szGuard, szFile, mGuardFile[szGuard])
                uiDuplicate += 1
            else:
                mGuardMap[szGuard] = szGuard
                mGuardFile[szGuard] = szFile
            if uiErrorCount:
                uiStcCount += uiErrorCount
                uiStcFile += 1

    return uiCount, uiStcFile, uiStcCount, uiDuplicate

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def StcSources(a_aFiles, a_bCheckTodo):
    uiStcCount = 0
    uiStcFile = 0
    uiCount = 0

    for szFile in a_aFiles:
        if IsSourceFile(szFile):
            uiCount += 1
            uiErrorCount = CheckSource(szFile, a_bCheckTodo)
            if uiErrorCount:
                uiStcCount += uiErrorCount
                uiStcFile += 1

    return uiCount, uiStcFile, uiStcCount, 0

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
def Main():
    aFiles = []
    bCheckTodo = 0
    if len(sys.argv) > 1:
        for szDir in sys.argv:
            if szDir[0] == '-':
                if len(szDir) > 1:
                    if szDir[1] == 't':
                        bCheckTodo = 1
            else:
                if os.path.isdir(szDir):
                    for szFile in os.listdir(szDir):
                        aFiles.append(szDir + szFile)
                elif os.path.isfile(szDir):
                    aFiles.append(szDir)
                else:
                    print "Invalid File : %s" % (szDir)
    else:
         aFiles = os.listdir(".")

    uiFile_H, uiCount_H, uiError_H, uiDuplicate_H = StcHeaders(
        aFiles, bCheckTodo)

    uiFile_CPP, uiCount_CPP, uiError_CPP, uiDuplicate_CPP = StcSources(
        aFiles, bCheckTodo)

    uiFile = uiFile_H + uiFile_CPP
    uiCount = uiCount_H + uiCount_CPP
    uiError = uiError_H + uiError_CPP
    uiDuplicate = uiDuplicate_H + uiDuplicate_CPP

    print gLine_SPY
    if uiCount > 0 or uiDuplicate > 0:
        print "## Static Check FAILURE : %d/%d file(s)." % (uiCount, uiFile)
    else:
        print "## Static Check success : %d/%d file(s)." % (uiFile, uiFile)

    if uiError > 0:
        print "## Errors : %d" % (uiError)
    if uiDuplicate > 0:
        print "## Duplicates : %d" % (uiDuplicate)
    print gLine_SPY

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------
Main()

##-----------------------------------------------------------------------------
##
##-----------------------------------------------------------------------------

