#ifndef ACME_BOOLEAN_PROMPT
#define ACME_BOOLEAN_PROMPT

varargs void boolean_prompt( string question, mixed yesmethod, mixed nomethod, 
    mixed quitmethod, int default_choice );

#define DEFAULT_YES       1
#define DEFAULT_NO        2
#define DEFAULT_QUIT      3

#endif /* ACME_BOOLEAN_PROMPT */
