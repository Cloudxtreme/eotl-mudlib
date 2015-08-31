/*
    NAME
        crypt.c - en/de -CRYPTion package
    DESCRIPTION
        A package to encrypt and decrypt strings and files.
        crypt() - encrypts or decrypts strings
        cryptfile()     - encrypts or decrypts files
    NOTES
        To encrypt something, the type should be set to 1, and to decrypt
        soemthing, the type should be set to -1.
    LAST MODIFIED
        Aleron  960202

*/


#include <acme.h>

private inherit AcmeError;

#include AcmeErrorInc
#include AcmeCryptInc

/*---------------------------------- crypt ----------------------------------*/
/*
    description: encrypts or decrypts strings
    parameters: s         - source string to en/de -crypt
                key       - key to use for en/de -crypting
                type      - indicator of encryption or decryption
    returns: string s   (the en/de -crypted string)
    notes: type = 1     encrypt the string
           type = -1    decrypt the string
*/
string crypt ( string s, string key, int type)
{
    int keylen, slen;
    int i;

    if ( !s || "" == s )
        return error ( ERR_ERROR, "Invalid source string.", "" );
    if ( !key || "" == key )
        return error ( ERR_ERROR, "Invalid key string.", "" );
    if ( type != ENCRYPT && type != DECRYPT )
        return error ( ERR_ERROR, "Invalid type token.", "" );

    keylen = strlen ( key );
    slen = strlen ( s );
    for ( i = 0; i < keylen; i++ )
        key[i] = ( int ) key[i] * type;
    for ( i = 0; i < slen; i++ )
        s[i] = ( ( ( int ) s[i] + key[i % keylen] + NCHARS ) % NCHARS );
    return s;
}

/*-------------------------------- cryptfile --------------------------------*/
/*
    description: encrypts or decrypts files
    parameters: infile    - file to be en/de -crypted
                outfile   - resulting output file
                key       - key to use for en/de -crypting
                type      - indicator of encryption or decryption
    returns: status
    notes: type = 1     encrypt the file
           type = -1    decrypt the file
*/
status cryptfile ( string infile, string outfile, string key, int type )
{
    int keylen;
    int seg_size;
    int i, max;

    seteuid ( getuid () );
    infile = THISP->ExpandPath ( infile );
    outfile = THISP->ExpandPath ( outfile );
    if ( !exists ( infile ) )
        return error ( ERR_ERROR, "Source file " + infile +
            " does not exist.", 0 );
    if ( exists ( outfile ) ){
        write ( "Output file " + outfile + " already exists.  Overwriting.\n" );rm ( outfile );
    }
    max = file_size ( infile );
    keylen = strlen ( key );
    // need to align segments w/ key
    seg_size = ( MAX_BYTE_TRANSFER / keylen ) * keylen;
    for ( i = 0; i < max; i += seg_size )
        write_file ( outfile, crypt ( read_bytes ( infile, i,
            ( max - i < seg_size ? max - i : seg_size ) ), key, type ) );
    return 1;
}
