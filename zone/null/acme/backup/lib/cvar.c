/*
    NAME
        cvar.c - Choose VARiants
    DESCRIPTION
        Chooses among various weighted options.
        choose_variant()        -chooses among different options
    NOTES
        choose_variant chooses from a 2D array of choice-chance pairs
    LAST MODIFIED
        Zamboni 960111

*/

#include <acme.h>
#include AcmeCVarInc

/*------------------------------ choose_variant -----------------------------*/
/*
    description: chooses among various weighted options
    parameters: choices   - ({ mixed option, int chance })
    returns: mixed (the option chosen)
    notes: the choices parameter is an array of chance-option tuples
*/
mixed choose_variant ( mixed *choices )
{
    int i;
    int rnum;    // random number
    int size;    // size of choices
    int maxchance;

    size = sizeof ( choices );
    maxchance = 0;
    for ( i = 0; i < size; i++ )
        maxchance += ( int ) choices[i][1];
    rnum = random ( maxchance );
    for ( i = 0; i < size; i++ )
        if ( rnum - ( int ) choices[i][1] < 0 )
            return choices[i][0];
    else
        rnum -= ( int ) choices[i][1];
    return 0;
}
