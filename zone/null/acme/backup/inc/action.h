#ifndef ACME_ACTION_INC
#define ACME_ACTION_INC

/*
** You really don't ever need to mess with this.  Just ignore what you
** don't want.
*/
#define MASK        0x1f
#define NUM_IN_MASK 5

/* Note:   If you change the MASK, you also have to change these offsets. */
#define VERB       0
#define TYPE       1
#define SHORT_VERB 2
#define OBJECT     3
#define FUNCTION   4

/* I copied these out of the driver. */
#define SENT_PLAIN      0
#define SENT_SHORT_VERB 1
#define SENT_NO_SPACE   2
#define SENT_NO_VERB    3
#define SENT_MARKER     4

        status has_action( object who, string verb );
        string action_type_char( int type );
varargs mixed  get_actions_regexp( object who, string pattern, object *from );
varargs mixed  get_actions_literal( object who, string verb, object *from );
varargs mixed  get_all_actions( object who, object *from );
varargs mixed  get_actions_transposed( object who, int mask );

#endif
