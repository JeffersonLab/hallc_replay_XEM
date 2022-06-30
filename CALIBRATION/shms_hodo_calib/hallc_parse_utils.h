#ifndef HALLC_PARSE_UTILS_H
#define HALLC_PARSE_UTILS_H
#include "parse_utils.h"

/*
  Author: C. Yero
  Date: Jan 22, 2022 
  
  Brief: This header file is intended to have general utility functions for Hall C software work/analysis.
  I have created the first function,  and other users/staff are also welcomed to expand and add other useful
  functions.

*/


//_______________________________________________________________________________________________________________
Double_t GetParam(string fname="", string param_name="", int col_idx=-1, int row_idx=-1, int row_idx_max=-1 )
{

  /*
    Brief: This function is intended to facilitate parameter readout of the standard Hall C format:

    where, val_(row,col) is the numerical value of the parameter at (row index, column, index)
    
    ; col_idx     0        1       2    ->         .    col_idx_max 
    param_name = val_(01),  val_(02),  val_(03), . . .  val_(0, col_idx_max)
                 val_(10),  val_(11),  val_(12), . . . 
		 .           .
    		 .                     .
		 .                               .
		 val_(row_idx_max, 0), . . . . .. . . . val(row_idx_max, col_idx_max)
  
   Typical examples of these parameter format structuca can be found on: 
   1) hallc_replay/PARAM/SHMS/HODO/phodo_TWcalib.param   or 
   2  hallc_replay/PARAM/HMS/CAL/hcal_calib.param    etc.   
   ( you get the idea . . . )
   
   Input Parameter Definitions:
   
   fname       : parameter file name containing the parameters to be extracted 
   param_name  : actual parameter name as is present in the parameter file
   col_idx     : column index, starting from 0
   row_idx     : row index, starting from 0
   row_idx_max : maximum row index, for example for SHMS Hodo Quartz Plane: row_idx_max = 21

  */
  
  Double_t param_val=-1;
  
  if(row_idx==0){
    //cout << parse_line(split(FindString(param_name.c_str(), fname.c_str(), true, row_idx_max)[row_idx], '=')[1], ',')[col_idx] << endl ;
    param_val = stod(parse_line(split(FindString(param_name.c_str(), fname.c_str(), true, row_idx_max)[row_idx], '=')[1], ',')[col_idx]);
    return param_val;
  }
  
  else if(row_idx>0){
    //cout << parse_line(FindString(param_name.c_str(), fname.c_str(), true, row_idx_max)[row_idx], ',')[col_idx] << endl ;
    param_val = stod(parse_line(FindString(param_name.c_str(), fname.c_str(), true, row_idx_max)[row_idx], ',')[col_idx]);
    return param_val;
  }

  else return param_val;
  
}

#endif
