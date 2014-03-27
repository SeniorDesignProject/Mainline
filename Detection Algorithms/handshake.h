#ifndef HANDSHAKE_H
#define HANDSHAKE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


//XCORR
	//If handshakes are more than 0.85 correlated, match.
		#define XCORR_THRESH 0.85
	//Sizes of the query handshake(s).
		#define SERIES_X_SIZE 14
		#define SERIES_Y_SIZE 14
//DTW
	//If handshake is less than 140000 from model handshake, match.
		#define DTW_THRESH 140000
	//Dimension of the accelerometer data
		#define DIMENSION 3
	//Accelerometer batch size
		#define WINDOW_SIZE 14
	
		#define X 0
		#define Y 1
		#define Z 2
//Flags
		#define MATCH 1
		#define NO_MATCH 0

//The reference handshake to compare accelerometer data to for DTW
    int handshake_signature[14][3] = {
        {-417,427,-76},
        {-398,531,13},
        {-393,548,126},
        {-506,150,34},
        {-531,929,-372},
        {-350,1053,97},
        {-24,126,-197},
        {-158,-430,-114},
        { 14,15,-181},
        {-267,1028,87},
        { 1,607,-31},
        {-29,464,-45},
        {-27,443,-1},
        {-9,459,-51}
    };

/*====================================================================
	Uses cross correlation to determine if two handshakes match 
	(i.e. >XCORR_THRESH correlated).
	
	Input: The primary axis (y axis) accelerometer data of each handshake.
	Output: 1 if handshakes match, 0 if not.
=====================================================================*/
	int handshake_match(int *series_x, int *series_y);

/*====================================================================
	Uses Dynamic Time Warping to determine if an accelerometer 
	data batch is a handshake (i.e. distance<DTW_THRESH).
	
	Input: Accelerometer data of size accel_data[14][3].
	Output: 1 if handshake, 0 if not.
=====================================================================*/
	int handshake_detect(int **accel_data);

#endif /* HANDSHAKE_H */