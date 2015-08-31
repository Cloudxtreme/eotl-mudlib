/*
    NAME

      hash.c - Secure Hash Server

    DESCRIPTION

      This daemon serves two functions: Creates nearly unique one-way
      hashes, and can be used as a trusted intermediary for
      zero-knowledge proofs.

      HASHING

        A hash function takes some input and outputs a fixed length
        "hash".  A one way function is a function that is relatively
        hard to compute the inverse of.  A one way hash function is
        both of these -- a hash function that is hard to compute the
        reverse of.  That means that while it's not hard to compute
        what the hash of a particular input would be, it is very hard
        to figure out what the input was from the hash, even when you
        know how the hash was computed.  This is useful for many
        security applications.  An additional useful feature of this
        hash is that it is hard to deliberately get a particular hash
        value.  A feature of most hash functions which this server
        lacks is determinism.  The hash values given by this server
        are not guaranteed to be the same for different instances of
        the server.  In fact, they will almost certainly be different.


      PROVING

        This particular daemon also serves a related purpose of secret
        proving.  There are several ways to prove that you know a
        secret without divulging it.  The easiest to implement in this
        environment is a trusted server.  You tell your secret to the
        server, and the server will vouch that you know some secret
        without telling anyone else what it is.  In this way, two
        objects can prove to eachother that they both know some secret
        by having the server tell the other what the hash of their
        secret is.  If they match, they must be the same secret.  The
        drawbacks of this scheme are that you must trust the server
        not to cheat and not to be compromisable.  The advantage is
        not having to do much computation.

      API 

        hash    - Returns the hash of some value.
        prove   - Proves to some other object that you know some secret.

    NOTES

      The server is vulnerable to denial of service attacks through
      destruction.  The hash is based on random(), which is not a good
      source of randomness.  It should be good enough for mud use
      though.

    LAST MODIFIED

      Zamboni 960515

*/

#include <acme.h>
#include AcmeHashServerInc

mapping hashtable;

/* The C library routine rand(), which random() may use, depending on
 * the platform, returns only numbers up to 2^15-1 (14 bits) on some
 * platforms.  To get 32 random bits, we need to call it a few times. */
#define rand32() ((random(0x800) <<21) | (random(0x800) << 10) | random(0x400))


/*----------------------------- hash ---------------------------*/
/*
    Description:
      Returns the hash of some value.
    Parameters:
      val - Value to be hashed
    Returns:
      The hash
    Notes:
      The value is unique for the lifetime of the server.
*/
int
hash( mixed val )
{
  if( !member( hashtable, val ) )
    hashtable[val] = rand32();
  return hashtable[val];
}

/*----------------------------- prove ---------------------------*/
/*
    Description:
      Prove knowledge of some secret.
    Parameters:
         ( mixed secret, closure cl, mixed extra )
      or ( mixed secret, object ob,  string func, mixed extra )

      secret - something you want to prove knowledge of
      cl     - a closure
      ob     - an object in which to call func
      func   - a function to call in ob
      extra  - an extra parameter to pass

      Depending on which form is used, one of these will be done:
            ob->func( hash, extra )
        or  funcall( hash, extra )
      where hash is the hash value of the secret: hash( secret )

    Returns:
      The return value of the call/eval.
    Notes:
      None.
*/
varargs mixed
prove( mixed val, mixed ob_or_cl, mixed func_or_extra, mixed extra )
{
  if( closurep( ob_or_cl ) )
    return funcall( (closure) ob_or_cl, hash( val ), func_or_extra );
  if( !objectp( ob_or_cl ) || !stringp( func_or_extra ))
    return 0;
  return call_other( (object) ob_or_cl, (string) func_or_extra, 
                     hash( val ), extra );
}



void
create()
{
  if( !mappingp( hashtable ) )
    hashtable = ([ ]);
}


status query_prevent_shadow() { return 1; }
status query_move_ok()        { return 1; }
status destruct_signal()      { return 1; }

