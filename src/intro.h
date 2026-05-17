//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

int  intro_init( void );

// will display test pattern if audioData is NULL
void intro_do( long time, long deltaTime, float* audioData, int count );

void intro_end( void );

#endif
