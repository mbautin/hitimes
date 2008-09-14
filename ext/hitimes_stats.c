/**
 * Copyright (c) 2008 Jeremy Hinegardner
 * All rights reserved.  See LICENSE and/or COPYING for details.
 *
 * vim: shiftwidth=4 
 */ 

#include "hitimes_stats.h"

/* classes defined here */
VALUE cH_Stats;         /* Hitimes::Stats */

/**
 * Allocator and Deallocator for Stats classes
 */

VALUE hitimes_stats_free(hitimes_stats_t* s) 
{
    xfree( s );
}

VALUE hitimes_stats_alloc(VALUE klass)
{
    VALUE obj;
    hitimes_stats_t* s = xmalloc( sizeof( hitimes_stats_t ) );

    s->min = 0.0;
    s->max = 0.0;
    s->count = 0;
    s->sum = 0.0;
    s->sumsq = 0.0;

    obj = Data_Wrap_Struct(klass, NULL, hitimes_stats_free, s);

    return obj;
}


/**
 * call-seq:
 *    stat.update( val ) -> nil
 * 
 * Update the running stats with the new value.
 */
VALUE hitimes_stats_update( VALUE self, VALUE v )
{
    double           new_v = NUM2DBL( v );
    hitimes_stats_t *stats;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    if ( 0 == stats->count ) {
      stats->min = new_v;
      stats->max = new_v;
    } else {
      stats->min = ( new_v < stats->min) ? ( new_v ) : ( stats->min );
      stats->max = ( new_v > stats->max) ? ( new_v ) : ( stats->max );
    }

    stats->count += 1;
    stats->sum   += new_v;
    stats->sumsq += ( new_v * new_v );

    return Qnil;
}

/**
 * call-seq:
 *    stat.mean -> Float
 * 
 * Return the arithmetic mean of the values put into the Stats object.  If no
 * values have passed through the stats object then 0.0 is returned;
 */
VALUE hitimes_stats_mean( VALUE self )
{
    hitimes_stats_t *stats;
    double           mean = 0.0;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    if ( stats->count > 0 ) {
      mean = stats->sum / stats->count ;
    }

    return rb_float_new( mean );
}

/**
 * call-seq:
 *    stat.max -> Float
 * 
 * Return the maximum value that has passed through the Stats object
 */
VALUE hitimes_stats_max( VALUE self )
{
    hitimes_stats_t *stats;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    return rb_float_new( stats->max );
}



/**
 * call-seq:
 *    stat.min  -> Float
 * 
 * Return the minimum value that has passed through the Stats object
 */
VALUE hitimes_stats_min( VALUE self )
{
    hitimes_stats_t *stats;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    return rb_float_new( stats->min );
}


/**
 * call-seq:
 *    stat.count -> Integer
 * 
 * Return the number of values that have passed through the Stats object.
 */
VALUE hitimes_stats_count( VALUE self )
{
    hitimes_stats_t *stats;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    return LONG2NUM( stats->count );
}


/**
 * call-seq:
 *    stat.sum -> Float
 * 
 * Return the sum of all the values that have passed through the Stats object.
 */
VALUE hitimes_stats_sum( VALUE self )
{
    hitimes_stats_t *stats;

    Data_Get_Struct( self, hitimes_stats_t, stats );

    return rb_float_new( stats->sum );
}


/**
 * call-seq:
 *    stat.stddev -> Float
 * 
 * Return the standard deviation of all the values that have passed through the
 * Stats object.  The standard deviation has no meaning unless the count is > 1,
 * therefore if the current _stat.count_ is < 1 then 0.0 will be returned;
 */
VALUE hitimes_stats_stddev ( VALUE self )
{
    hitimes_stats_t *stats;
    double          stddev = 0.0;

    Data_Get_Struct( self, hitimes_stats_t, stats );
    if ( stats->count > 1 ) {
      stddev = sqrt( ( stats->sumsq - ( stats->sum * stats->sum  / stats->count ) ) / ( stats->count - 1 ) );
    }

    return rb_float_new( stddev );
}


/**
 * Document-class: Hitimes::Stats
 *
 * The Stats class encapulsates capturing and reporting statistics.  It is
 * modeled after the RFuzz::Sampler class, but implemented in C.  For general use
 * you allocate a new Stats object, and then update it with new values.  The
 * Stats object will keep track of the _min_, _max_, _count_ and _sum_ and when
 * you want you may also retrieve the _mean_ and _stddev_.
 *
 * this contrived example shows getting a list of all the files in a directory
 * and running stats on file sizes.
 *
 *   stats = Hitimes::Stats.new
 *   File.open
 *
 */
void Init_hitimes_stats()
{

    mH = rb_define_module("Hitimes"); 

    cH_Stats = rb_define_class_under( mH, "Stats", rb_cObject ); /* in hitimes_stats.c */
    rb_define_alloc_func( cH_Stats, hitimes_stats_alloc );

    rb_define_method( cH_Stats, "update", hitimes_stats_update, 1 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "mean", hitimes_stats_mean, 0 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "max", hitimes_stats_max, 0 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "min", hitimes_stats_min, 0 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "count", hitimes_stats_count, 0 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "sum", hitimes_stats_sum, 0 ); /* in hitimes_stats.c */
    rb_define_method( cH_Stats, "stddev", hitimes_stats_stddev, 0 ); /* in hitimes_stats.c */
}
 