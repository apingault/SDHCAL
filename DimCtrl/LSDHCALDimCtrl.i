%module LSDHCALDimCtrl
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "stdint.i"


 %{
using std::string;
#include "DimDaqControl.h"
#include "DimJobInterface.h"
 %}

%include "DimDaqControl.h"
%include "DimJobInterface.h"
