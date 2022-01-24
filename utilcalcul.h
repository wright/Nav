// utilcalcul.h
// code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>


void LatToString(double lat, CString& strlat);

void LongToString(double longit, CString& strlong);

void DistHeadToCoord(double distance,double heading, double& latitude, double& longitude);
