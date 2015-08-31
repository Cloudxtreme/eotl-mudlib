#ifndef ACME_STAT_INC
#define ACME_STAT_INC

private status valid_sample( float *sample );
 
float sum( float *sample );
float mean( float *sample );
float median( float *sample );
float mode( float *sample );
float high( float *sample );
float low( float *sample );
float first_quartile( float *sample );
float third_quartile( float *sample );
float standard_deviation( float *sample );
float variance( float *sample );
float iqr( float *sample );
float percentile( float *sample, float percentile );
 
mapping freq( float *sample );
mapping rel_freq( float *sample );
mapping cum_rel_freq( float *sample );
mapping deviation( float *sample );

#endif
