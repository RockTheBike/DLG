#ifdef SERIAL_INTERACT
  // let everyone mutate everything!
  #define NDCONST
  #define NDPROTECTED public
#else
  #define NDCONST const
  #define NDPROTECTED protected
#endif
