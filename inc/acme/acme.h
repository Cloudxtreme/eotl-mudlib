/*
    NAME
        acme.h - ACME global include file
    LAST MODIFIED
       Zamboni 970303
*/

#ifndef ACME_INC
#define ACME_INC

#define AcmeDir       "/zone/null/acme/"
#define AcmeLibDir    "/lib/acme/"
#define AcmeIncDir    "/inc/acme/"
#define AcmeObjDir    "/obj/acme/"
#define AcmeSecureDir "/secure/acme/"
#define AcmeEtcDir    AcmeDir "etc/"

#define OpieDir            AcmeDir "opie/"
#define OpieInc            AcmeIncDir "opie.h"
#define OpieClient         AcmeIncDir "opie_client.h"

#define AcmeTadjiDir       AcmeDir "tadji/"
#define AcmeTadjiInc       AcmeTadjiDir "tadji.h"
#define AcmeTadji          AcmeTadjiDir "tadji.c"

//#define AcmeHomeDir        AcmeDir "home/"
#define AcmeHomeDir        "/usr/aleron/home/"
#define AcmeHomeSaveDir    AcmeHomeDir "save/"
#define AcmeHomeHelpDir    AcmeHomeDir "help/"
#define AcmeHomeInc        AcmeHomeDir "home.h"
#define AcmeHomeEditor     AcmeHomeDir "editor.c"
#define AcmeHomeServer     AcmeHomeDir "server.c"
#define AcmeHomeRoom       AcmeHomeDir "room.c"

#define AcmeDaemonDir      AcmeDir "daemon/"
#define AcmeClosureServer  AcmeDaemonDir "closure_eval_server"
#define AcmeSentinelDir    AcmeDaemonDir "sentinel/"
#define AcmeSentinel       AcmeSentinelDir "sentinel.c"
#define AcmeSentinelInc    AcmeSentinelDir "sentinel.h"
#define AcmeHashServer     AcmeDaemonDir "hash.c"
#define AcmeHashServerInc  AcmeIncDir    "hash.h"

//------------------------------ include files ------------------------------//

#define AcmeCommandInc     AcmeIncDir "tadji_command.h"

//------------------------------ library files ------------------------------//

#define AcmeActionInc      AcmeIncDir "action.h"
#define AcmeAction         AcmeLibDir "action.c"
#define AcmeAction2        AcmeLibDir "action2.c"

#define AcmeAllocInc       AcmeIncDir "alloc.h"
#define AcmeAlloc          AcmeLibDir "alloc.c"

#define AcmeAncestryInc    AcmeIncDir "ancestry.h"
#define AcmeAncestry       AcmeLibDir "ancestry.c"

#define AcmeAnsiInc        AcmeIncDir "ansi.h"
#define AcmeAnsi           AcmeLibDir "ansi.c"

#define AcmeArgsInc        AcmeIncDir "args.h"
#define AcmeArgs           AcmeLibDir "args.c"

#define AcmeArrayInc       AcmeIncDir "array.h"
#define AcmeArray          AcmeLibDir "array.c"

#define AcmeBooleanPromptInc AcmeIncDir "boolean_prompt.h"
#define AcmeBooleanPrompt    AcmeLibDir "boolean_prompt.c"

#define AcmeBreadthInc     AcmeIncDir "breadth.h"
#define AcmeBreadth        AcmeLibDir "breadth.c"

#define AcmeBWInc          AcmeIncDir "bw.h"
#define AcmeBW             AcmeLibDir "bw.c"

#define AcmeCallSecurityInc AcmeIncDir "callsec.h"
#define AcmeCallSecurity    AcmeLibDir "callsec.c"

#define AcmeClosuresInc    AcmeIncDir "closures.h"
#define AcmeClosures       AcmeLibDir "closures.c"

#define AcmeCmdInc         AcmeIncDir "cmd.h"
#define AcmeCmd            AcmeLibDir "cmd.c"

#define AcmeColoredCodeInc   AcmeIncDir "color_code.h"
#define AcmeColoredCode      AcmeLibDir "color_code.c"

#define AcmeCryptInc       AcmeIncDir "crypt.h"
#define AcmeCrypt          AcmeLibDir "crypt.c"

#define AcmeConversionInc  AcmeIncDir "conversion.h"
#define AcmeConversion     AcmeLibDir "conversion.c"

#define AcmeCVarInc        AcmeIncDir "cvar.h"
#define AcmeCvarInc        AcmeCVarInc
#define AcmeCVar           AcmeLibDir "cvar.c"
#define AcmeCvar           AcmeCVar

#define AcmeDepthInc       AcmeIncDir "depth.h"
#define AcmeDepth          AcmeLibDir "depth.c"

#define AcmeErrorInc       AcmeIncDir "error.h"
#define AcmeError          AcmeLibDir "error.c"

#define AcmeFileInc        AcmeIncDir "file.h"
#define AcmeFile           AcmeLibDir "file.c"

#define AcmeFindTargetInc  AcmeIncDir "find_target.h"
#define AcmeFindTarget     AcmeLibDir "find_target.c"

#define AcmeForceLoadInc   AcmeIncDir "force_load.h"
#define AcmeForceLoad      AcmeLibDir "force_load.c"

#define AcmeFormatStringsInc AcmeIncDir "format_strings.h"
#define AcmeFormatStrings  AcmeLibDir "format_strings.c"

#define AcmeFunctionInc    AcmeIncDir "function.h"
#define AcmeFunction       AcmeLibDir "function.c"

#define AcmeGetDataInc     AcmeIncDir "getdata.h"
#define AcmeGetData        AcmeLibDir "getdata.c"

#define AcmeGetOptsInc     AcmeIncDir "getopts.h"
#define AcmeGetoptsInc     AcmeIncDir "getopts.h"
#define AcmeGetOpts        AcmeLibDir "getopts.c"
#define AcmeGetopts        AcmeLibDir "getopts.c"

#define AcmeGraphInc       AcmeIncDir "graph.h"
#define AcmeGraph          AcmeLibDir "graph.c"

#define AcmeInterpret      AcmeLibDir "interpret.c"
#define AcmeInterpretInc   AcmeIncDir "interpret.h"

#define AcmeKqmlInc        AcmeIncDir "kqml.h"
#define AcmeKQMLInc        AcmeIncDir "kqml.h"
#define AcmeKqml           AcmeLibDir "kqml.c"
#define AcmeKQML           AcmeLibDir "kqml.c"

#define AcmeLoggerInc      AcmeIncDir "logger.h"
#define AcmeLoggerFactory  AcmeSecureDir "logger_factory.c"
#define AcmeLogger         AcmeObjDir "logger.c"

#define AcmeManInc         AcmeIncDir "man.h"
#define AcmeMan            AcmeLibDir "man.c"

#define AcmeMathInc        AcmeIncDir "math.h"
#define AcmeMath           AcmeLibDir "math.c"

#define AcmeMenuInc        AcmeIncDir "menu.h"
#define AcmeMenu           AcmeLibDir "menu.c"

#define AcmeMethodInc      AcmeIncDir "method.h"
#define AcmeMethod         AcmeLibDir "method.c"

#define AcmeMiscInc        AcmeIncDir "misc.h"
#define AcmeMisc           AcmeLibDir "misc.c"

#define AcmeOutputInc      AcmeIncDir "output.h"
#define AcmeOutput         AcmeLibDir "output.c"

#define AcmeOSpecInc       AcmeIncDir "ospec.h"
#define AcmeOspecInc       AcmeIncDir "ospec.h"
#define AcmeOSpec          AcmeLibDir "ospec.c"
#define AcmeOspec          AcmeLibDir "ospec.c"

#define AcmeQueue          AcmeLibDir "queue.c"
#define AcmeQueueInc       AcmeIncDir "queue.h"

#define AcmeRoomInc        AcmeIncDir "room.h"
#define AcmeRoom           AcmeLibDir "room.c"

#define AcmeShadowInc      AcmeIncDir "shadow.h"
#define AcmeShadow         AcmeLibDir "shadow.c"

#define AcmeStack          AcmeLibDir "stack.c"
#define AcmeStackInc       AcmeIncDir "stack.h"

#define AcmeStat           AcmeLibDir "stat.c"
#define AcmeStatInc        AcmeIncDir "stat.h"

#define AcmeString         AcmeLibDir "strings.c"
#define AcmeStringInc      AcmeIncDir "strings.h"
#define AcmeStringsInc     AcmeIncDir "strings.h"
#define AcmeStrings        AcmeLibDir "strings.c"

#define AcmeStructInc      AcmeIncDir "struct.h"
#define AcmeStruct         AcmeLibDir "struct.c"
#define AcmeUserInc        AcmeIncDir "user.h"
#define AcmeUser           AcmeLibDir "user.c"

#define AcmeVarSpaceInc    AcmeIncDir "varspace.h"
#define AcmeVarSpace       AcmeLibDir "varspace.c"


#endif  // ACME_INC
