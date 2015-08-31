#ifndef ACME_FINDTARGET_INC
#define ACME_FINDTARGET_INC

object *resolve_filespec( string arg, status quiet );

varargs object *find_targets(string spec, string priorities, 
                             status quiet, object *prev );

object  find_target ( string targ, string pat );

#define FT_FindPlayer     "p"
#define FT_FindPlayerChar 'p'

#define FT_FindLiving     "l"
#define FT_FindLivingChar 'l'

#define FT_FindObject     "o"
#define FT_FindObjectChar 'o'

#define FT_File       "f"
#define FT_FileChar   'f'

#define FT_InvItem    "i"
#define FT_InvItemChar'i'

#define FT_EnvItem    "e"
#define FT_EnvItemChar'e'

#endif  // ACME_FINDTARGET_INC
