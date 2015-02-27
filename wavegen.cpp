#include "wavegen.h"

WaveGenerator::WaveGenerator( size_t num_pts, wavelevel_t* levels, wavet_t* times ) :
  _num_pts(num_pts), _levels(levels), _times(times),
  _cycle_time(_times[num_pts-1]) {
}

wavelevel_t WaveGenerator::gen( wavet_t t ) {
	wavet_t t_modded = t % _cycle_time;
	size_t pre_point = _num_pts-1;
	size_t post_point = 0;  // will be first point with time>=t_modded
	while( _times[post_point] < t_modded )
		pre_point = post_point++;
	return
	  ( _levels[pre_point ] * ( _times[post_point] - t_modded ) +
	    _levels[post_point] * ( t_modded - _times[pre_point] ) ) /
	  ( _times[post_point] - _times[pre_point] );
}
