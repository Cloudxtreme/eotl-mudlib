/*      
    NAME
        stat.c - A statistics package
    DESCRIPTION
        Useful routines for generating statistics based on a sample,
        which is usually array of floats.
 
        sum()                 - find the sum of all the samples
        mean()                - find the mean of the sample
        median()              - find the median of the sample
        mode()                - find the mode of the sample
        first_quartile()      - find the first quartile of the sample
        third_quartile()      - find the third quartile of the sample
        standard_deviation()  - find the standard deviation of the sample
        variance()            - find the variance of the sample
        iqr()                 - find the interquartile range of the sample
        percentile()          - find the percentile of the sample
        freq()                - map each sample to its frequency
        rel_freq()            - map each sample to its relative frequency
        cum_rel_freq()        - map each sample to its cumulative relative 
                                frequency
        deviation()           - map each sample to its deviation
    NOTES
        None.
    LAST MODIFIED
        Devo 980124
*/
 
#include <acme.h>
 
private inherit AcmeArray;
private inherit AcmeMath;
 
#include AcmeArrayInc
#include AcmeMathInc
#include AcmeStatInc
#include <sys/lpctypes.h>
 
/*----------------------------- valid_sample ----------------------------*/
/*
    Description:
      Private function to attempt to validate a sample, and return
      if it could be validated.
    Parameters:
      sample - The sample to be validated.  The value of this variable
               will be changed, if necessary, to make the sample valid.
               This valid should be passed by reference.
    Returns:
      True if the sample is valid.
    Notes:
      Currently, a valid sample is defined as an array of floats, or
      an array containing values which can be converted to floats, if
      necessary.  This definition will be applied to the rest of the
      functions in this library.
*/
private status
valid_sample( mixed *sample )
{
  float *new;
  new = typecast_array( sample, T_FLOAT );
  if( sizeof(new) == sizeof(sample) )
  {
//    if( referencep( sample ) )
      sample = new;
    return 1;
  }
  return 0;
}
 
/*--------------------------------- sum --------------------------------*/
/*
    Description:
      Find the sum of all the elements in the sample.
    Parameters:
      sample - the sample to be sum'd
    Returns:
      The sum of every element in sample
    Notes:
      None.
*/
private static closure sum_cl;
 
float
sum( float *sample )
{
  float sum;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to sum()\n" );
  if( !closurep( sum_cl ) )
    sum_cl = lambda( ({ 'x, 'y }), ({ #'+=, 'y, 'x }) );
  sum = 0.0;
  filter_array( sample, sum_cl, &sum );
  return sum;
}
 
/*--------------------------------- mean --------------------------------*/
/*
    Description:
      Find the mean of the sample.
    Parameters:
      sample - the sample to be mean'd
    Returns:
      The mean of the sample.
    Notes:
      None.
*/
private static closure mean_cl;
 
float
mean( float *sample )
{
  float mean;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to mean()\n" );
  if( !closurep( mean_cl ) )
    mean_cl = lambda( ({ 'x, 'y, 'z }),
      ({ #'+=, 'y, ({ #'/, 'x, 'z }) }) 
    );
  mean = 0.0;
  filter_array( sample, mean_cl, &mean, sizeof(sample) );
  return mean;
}
 
/*------------------------------- median --------------------------------*/
/*
    Description:
      Find the median of the sample.
    Parameters:
      sample - the sample to be median'd
    Returns:
      The median of the sample.
    Notes:
      This is just an alias for the 50'th percentile.
*/
float
median( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to median()\n" );
  return percentile( sample, 50.0 );
}
 
/*--------------------------------- mode --------------------------------*/
/*
    Description:
      Find the mode of the sample.
    Parameters:
      sample - the sample to be mode'd
    Returns:
      The mode of the sample.
    Notes:
      This function doesn't currently have support for multi-moded samples.
      If there is more than one mode in the sample, an arbitrary one will
      be returned.
*/
float
mode( float *sample )
{
  mapping freq_map;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to mode()\n" );
  freq_map = freq(sample);
  return sort_alist( ({ m_values( freq_map ), m_indices( freq_map ) }), 
                     #'> )[1][0];
}
 
/*--------------------------------- high --------------------------------*/
/*
    Description:
      Find the high value in a sample.
    Parameters:
      sample - the sample to be high'd
    Returns:
      The highest value in the sample.
    Notes:
      None.
*/
float
high( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to mode()\n" );
  return sort_array( sample, #'< )[0];
}
 
/*--------------------------------- low --------------------------------*/
/*
    Description:
      Find the low value in a sample.
    Parameters:
      sample - the sample to be low'd
    Returns:
      The lowest value in the sample.
    Notes:
      None.
*/
float
low( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to mode()\n" );
  return sort_array( sample, #'> )[0];
}
 
/*----------------------------- first_quartile --------------------------*/
/*
    Description:
      Find the first quartile of the sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The first quartile of the sample.
    Notes:
      This is just an alias for the 25'th percentile.
*/
float 
first_quartile( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to first_quartile()\n" );
  return percentile( sample, 25.0 );
}
 
/*----------------------------- third_quartile ------------------------*/
/*
    Description:
      Find the third quartile of the sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The third quartile of the sample.
    Notes:
      This is just an alias for the 75'th percentile.
*/
float third_quartile( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to third_quartile()\n" );
  return percentile( sample, 75.0 );
}
 
/*------------------------- standard_deviation -----------------------*/
/*
    Description:
      Find the standard deviation of the sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The standard deviation of the sample.
    Notes:
      This is just the square root of the variance.
*/
float 
standard_deviation( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to standard_deviation()\n" );
  return sqrt( variance( sample ) );
}
 
/*----------------------------- variance ----------------------------*/
/*
    Description:
      Find the variance of the sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The variance of the sample.
    Notes:
      None.
*/
float 
variance( float *sample )
{
  mapping f_map, d_map;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to variance()\n" );
  f_map = freq( sample );
  d_map = deviation( sample );
  walk_mapping( d_map, lambda( ({ 'key, 'val }),
    ({ #'=, 
       'val, 
       ({ #'*, 
          ({ #'*, 'val, 'val }), 
          ({ #'[, f_map, 'key }) 
       }) 
    }) ) );
  return sum( m_values( d_map ) ) / ( sizeof( sample ) - 1 );
}
 
/*---------------------------------- iqr -------------------------------*/
/*
    Description:
      Find the interquartile_range of the sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The interquartile range of the sample.
    Notes:
      This is just the difference betwen the first and third quartiles.
*/
float 
iqr( float *sample )
{
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to iqr()\n" );
  return third_quartile(sample) - first_quartile(sample);
}
 
/*------------------------- standard_deviation -----------------------*/
/*
    Description:
      Find the element in sample that matches percentile.
    Parameters:
      sample     - the sample to be evaluated
      percentile - the percentile to be matched
    Returns:
      Returns the value for which percentile percent of the samples
      fall beneath it.
    Notes:
      None.
*/
 
// highest I can go without overflowing
#define MOD 10000000
 
float 
percentile( float *sample, float p )
{
  float step, lower, lelem, upper, uelem, pstep, delem, diff;
  int size, istep, ip;
 
  p = (float)p;
  if( p < 0.0 || p > 100.0 )
    raise_error( "Bad argument 1 to percentile()\n" );
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to percentile()\n" );
  sample = sort_array( sample, #'> );
 
  size = sizeof( sample );
  step = 100.0 / ( size - 1 );
 
  istep = to_int( step * MOD );
  ip = to_int( p * MOD );
 
  lower = to_float( ip - ip%istep ) / MOD;
  lelem = sample[ to_int( lower / step ) ];
  if( lower == p )
    return lelem;
 
  upper = lower + step;
  uelem = sample[ to_int( upper / step ) ];
  pstep = p - lower;
  delem = uelem - lelem;
  diff = ( ( pstep * delem ) + ( step * lelem ) ) / step;
  return diff;
}
 
/*----------------------------- freq -----------------------------*/
/*
    Description:
      Find the frequency of all the values in a sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The mapping of values in the sample to how many times the occur
      in that sample.
    Notes:
      None.
*/
mapping freq( float *sample )
{
  mapping f_map;
  int i, j;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to freq()\n" );
  f_map = mkmapping( sample, allocate( sizeof(sample) ) );
  for( i=0, j=sizeof(sample); i<j; i++ )
    f_map[sample[i]]++;
  return f_map;
}
 
/*------------------------ rel_freq ---------------------------*/
/*
    Description:
      Find the relative frequency of all the values in a sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The mapping of values in the sample to how many times the occur
      in that sample divided by the number of items in the sample.
    Notes:
      None.
*/
mapping rel_freq( float *sample )
{
  mapping rf_map;
  int size;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to rel_freq()\n" );
  size = sizeof( sample );
  rf_map = freq( sample );
  walk_mapping( rf_map, lambda( ({ 'key, 'val }),
    ({ #'=, 'val, ({ #'/, ({ #'to_float, 'val }), size }) }) ) );
  return rf_map;
}
 
/*------------------------- cum_rel_freq ---------------------------*/
/*
    Description:
      Find the cumulative relative frequency of all the values in a sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The mapping of values in the sample to the sum of the relative
      frequency and all the relative frequencies below that value.
    Notes:
      None.
*/
mapping cum_rel_freq( float *sample )
{
  mapping crf_map;
  mixed *crf_alist;
  int i, j;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to cum_rel_freq()\n" );
  crf_map = rel_freq( sample );
  crf_alist = ({ m_indices( crf_map ), m_values( crf_map ) });
  crf_alist = sort_alist( crf_alist, #'> );
  for( i=1, j=sizeof(crf_alist[0]); i<j; i++ )
    crf_alist[1][i] += crf_alist[1][i-1];
  crf_map = mkmapping( crf_alist[0], crf_alist[1] );
  return crf_map;
}
 
/*----------------------------- deviation --------------------------*/
/*
    Description:
      Find the deviation of all the values in a sample.
    Parameters:
      sample - the sample to be evaluated
    Returns:
      The mapping of values in the sample to their deviation from the
      mean.
    Notes:
      None.
*/
mapping deviation( float *sample )
{
  float meanv;
  mapping d_map;
  if( !valid_sample( &sample ) )
    raise_error( "Bad argument 1 to deviation()\n" );
  meanv = mean( sample );
  d_map = mkmapping( sample, map_array( sample, #'-, meanv ) );
  return d_map;
}
