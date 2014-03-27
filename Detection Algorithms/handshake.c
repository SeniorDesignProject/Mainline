/*
	Author: Chris Garry, chrislgarry AT gmail DOT com
	
	Description:
		The following code implements a handshake detection and a handshake
		matching algorithm given accelerometer data from a Pebble Smart
		Watch. 

	License:

		This code is for education purposes only as part of the senior 
		design project of the Electrical and Computer Engineering Department
		at the University of Massachusetts Amherst.

		Reference for allocAccBuf(), releaseAccBuf, DTWdistance():
			Jiayang Liu, Zhen Wang, Lin Zhong, Jehan Wickramasuriya, and Venu Vasudevan, 
			"uWave: Accelerometer-based personalized gesture recognition and its applications," 
			in Proc. IEEE Int. Conf. Pervasive Computing and Communication (PerCom), March 2009.
		
		Reference for cross_correlation():
			Paul Bourke, August 1996, Cross Correlation, AutoCorrelation -- 2D Pattern Identification 
			http://paulbourke.net/miscellaneous/correlate/
*/

#include "handshake.h"

//HELPER FUNCTIONS==========================================================================
	
	//Computes the cross correlation coefficients for series_x and series_y
	double *cross_correlation(int *series_x, int *series_y){	

		double sum_x = 0.0;
		double sum_y = 0.0;
		double mean_x = 0.0;
		double mean_y = 0.0;
		double sqr_series_x = 0.0;
		double sqr_series_y = 0.0;
		int delay_max = SERIES_X_SIZE;
		double *cross_correlate = (double*)malloc(delay_max*2*sizeof(double));

		//Calculate the sum of each series
			int z;
			for(z = 0; z<SERIES_X_SIZE; z++){
				sum_x += series_x[z];
				sum_y += series_y[z];
			}

		//Calculate mean of each series.
			mean_x = sum_x/SERIES_X_SIZE;
			mean_y = sum_y/SERIES_Y_SIZE;

		//Compute cross-correlate coefficients
			int delay;
			for(delay = -delay_max; delay<delay_max; delay++){

				double numerator = 0;
				sqr_series_x = 0;
				sqr_series_y = 0;

				for(int i =0; i<SERIES_X_SIZE; i++){

					//Skip cofficients with out of bound indicies
					int j = i + delay;
					if (j < 0 || j >= SERIES_X_SIZE){
						double term_1 = (series_x[i]-mean_x);
						double term_2 = (-1*mean_y);	
						numerator += term_1*term_2;
						sqr_series_x += term_1*term_1;
						sqr_series_y += term_2*term_2;			
					}
					else{
						double term_1 = (series_x[i]-mean_x);
						double term_2 = (series_y[j]-mean_y);	
						numerator += term_1*term_2;
						sqr_series_x += term_1*term_1;
						sqr_series_y += term_2*term_2;
					}

				}

				cross_correlate[delay+delay_max] = numerator/sqrt(sqr_series_x*sqr_series_y);

			}

		return cross_correlate;
	}

	//Allocate space for accelerometer data matrix (each row is an [x,y,z]) for DTW
    int** allocAccBuf(int len){
        int** ret = (int**)malloc(sizeof(int*)*len);
        int i;
        for( i = 0; i < len; i++)
            ret[i] = (int*)malloc(sizeof(int)*DIMENSION);
        return ret;
    }

    //Free the space of an accelerometer data matrix for DTW
    void releaseAccBuf(int** p, int len) {
        int i;
        for( i = 0; i < len; i++)
            free(p[i]);
        free(p);
    }

    //Dynamic Time Warping Distance: Return integer distance between two discrete time signals.
    int DTWdistance(int **sample1, int length1, int **sample2, int length2, int i, int j, int *table) {
        
        if( i < 0 || j < 0)
            return 100000000;
        int tableWidth = length2;
        int localDistance = 0;
        int k;
        for( k = 0; k < DIMENSION; k++)
            localDistance += ((sample1[i][k]-sample2[j][k])*(sample1[i][k]-sample2[j][k]));
        
        int sdistance, s1, s2, s3;
        
        if( i == 0 && j == 0) {
            if( table[i*tableWidth+j] < 0)
                table[i*tableWidth+j] = localDistance;
            return localDistance;
        } else if( i==0) {
            if( table[i*tableWidth+(j-1)] < 0)
                sdistance = DTWdistance(sample1, length1, sample2, length2, i, j-1, table);
            else
                sdistance = table[i*tableWidth+j-1];
        } else if( j==0) {
            if( table[(i-1)*tableWidth+ j] < 0)
                sdistance = DTWdistance(sample1, length1, sample2, length2, i-1, j, table);
            else
                sdistance = table[(i-1)*tableWidth+j];  
        } else {
            if( table[i*tableWidth+(j-1)] < 0)
                s1 = DTWdistance(sample1, length1, sample2, length2, i, j-1, table);
            else
                s1 = table[i*tableWidth+(j-1)];
            if( table[(i-1)*tableWidth+ j] < 0)
                s2 = DTWdistance(sample1, length1, sample2, length2, i-1, j, table);
            else
                s2 = table[(i-1)*tableWidth+ j];
            if( table[(i-1)*tableWidth+ j-1] < 0)
                s3 = DTWdistance(sample1, length1, sample2, length2, i-1, j-1, table);
            else
                s3 = table[(i-1)*tableWidth+ j-1];
            sdistance = s1 < s2 ? s1:s2;
            sdistance = sdistance < s3 ? sdistance:s3;
        }
        table[i*tableWidth+j] = localDistance + sdistance;
        return table[i*tableWidth+j];
    }
    
//END HELPER FUNCTIONS======================================================================


//LIBRARY FUNCTIONS=========================================================================
	int handshake_match(int *series_x, int *series_y){

		//Compute the cross correlation coefficients
			double *coefficients = cross_correlation(series_x,series_y);

		//Determine if a correlation coefficient >XCORR_THRESH exists.
			int i;
			for(i=0; i<SERIES_X_SIZE*2; i++){
				if(coefficients[i] > XCORR_THRESH){
					return MATCH;
				}
			}

		return NO_MATCH;
	}

	int handshake_detect(int **handshake_test){

	    int i, distance, *table, **sample, handshake_sig_size = WINDOW_SIZE;

	    //Initialize handshake signature
	    int **handshake_sig = allocAccBuf(WINDOW_SIZE);

	    //Populate handshake signature matrix pointer
	    for(i=0; i<WINDOW_SIZE;i++){
	        handshake_sig[i][X] = handshake_signature[i][X];
	        handshake_sig[i][Y] = handshake_signature[i][Y];
	        handshake_sig[i][Z] = handshake_signature[i][Z];
	    }

	    //Compute DTW Distance
	        //Extract candidate sample from input handshake_test
	            sample = allocAccBuf(WINDOW_SIZE);
	            for(i = 0; i < (int)WINDOW_SIZE; i++){
	                sample[i][X] = handshake_test[i][X];
	                sample[i][Y] = handshake_test[i][Y];
	                sample[i][Z] = handshake_test[i][Z];
	            }

	        //Initialize DTW table
	            table = (int*) malloc(WINDOW_SIZE * handshake_sig_size*sizeof(int));
	            for(i = 0; i < WINDOW_SIZE*handshake_sig_size; i++)
	                table[i] = -1;

	        //Compute DTW Distance between candidate sample and reference
	            distance = DTWdistance(sample,WINDOW_SIZE,handshake_sig,handshake_sig_size,
	            	WINDOW_SIZE-1,handshake_sig_size-1,table);

	        //Clean up
	            free(table);
	            releaseAccBuf(sample, WINDOW_SIZE);
	            releaseAccBuf(handshake_sig, WINDOW_SIZE);

		if((distance/WINDOW_SIZE*2)<DTW_THRESH){
			return MATCH;
		}
		return NO_MATCH;
	}
//END LIBRARY FUNCTIONS=====================================================================


//TEST======================================================================================
	int main(){

		//TEST HANDSHAKE_MATCH==============================================================
			int handshake_signature_x[14] = {-417,-398,-393,-506,-531,-350,-24,-158,14,-267,
												1,-29,-27,-9};
			int handshake_test_x[14] 	 = {-417,-398,-393,-506,-531,-350,-24,-158,14,-267,
												1,-29,-27,-9};

		    if(handshake_match(handshake_signature_x, handshake_test_x)){
		    	printf("\nRESULT: MATCH.\n");
		    }
		    else{
		    	printf("\nRESULT: NO MATCH.\n");
		    }
		//END TEST HANDSHAKE_MATCH==========================================================


		//TEST HANDSHAKE_DETECT=============================================================
		    int **handshake_sig = allocAccBuf(WINDOW_SIZE);
		    int i;
		    for(i=0; i<WINDOW_SIZE;i++){
			        handshake_sig[i][X] = handshake_signature[i][X];
			        handshake_sig[i][Y] = handshake_signature[i][Y];
			        handshake_sig[i][Z] = handshake_signature[i][Z];
			}

			//See if function detects our reference handshake as handshake.
		    if(handshake_detect(handshake_sig)){
		    	printf("\nRESULT: HANDSHAKE DETECTED.\n");
		    }
		    else{
		    	printf("\nRESULT: NO HANDSHAKE DETECTED.\n");
		    }
		//END TEST HANDSHAKE_DETECT=========================================================

		return 0;
	 }