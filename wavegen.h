#ifndef _INC_WAVEGEN_H
#define _INC_WAVEGEN_H

#include "arbduino.h"

typedef millitime_t wavet_t;
typedef float wavelevel_t;

class WaveGenerator {
  public:
	WaveGenerator( size_t num_pts, wavelevel_t* levels, wavet_t* times );
	// TODO:  tune the interface
	// - use times[0] to specify cycle_time (maybe search backward)
	// - times specifies incremental distances rather than absolute
	// - give scaling factor for times
	// - figure out how to specify a discontinuous jump
	wavelevel_t gen( wavet_t t );
  private:
	size_t _num_pts;  // number of defining points
	wavelevel_t* _levels;
	wavet_t* _times;
	wavet_t _cycle_time;
};

#endif
