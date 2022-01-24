// DpStar.h: interface for the DpStar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPSTAR_H__89DDEA00_CDAB_11D3_B498_005056816121__INCLUDED_)
#define AFX_DPSTAR_H__89DDEA00_CDAB_11D3_B498_005056816121__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class DpStarWaypoint {
public:
	CString name;
	double latitude;
	double longitude;

	DpStarWaypoint() {};
	DpStarWaypoint(CString n, double lat, double lon) {name=n;latitude=lat;longitude=lon;};
};

class DpStarRoute {
public:
	CString name;
	CArray<DpStarWaypoint, DpStarWaypoint> waypoints;
	int lastWaypoint;

	DpStarWaypoint* nextWaypoint();
	DpStarWaypoint* firstWaypoint();
	void addWaypoint(DpStarWaypoint W);
	DpStarRoute() {lastWaypoint=0;};
	DpStarRoute(CString n) {lastWaypoint=0;name=n;};
	DpStarRoute(DpStarRoute &rhs);
	DpStarRoute &operator=(DpStarRoute &rhs); 
};

class DpStarAirport {
public:
	CString icao;
	CArray<DpStarRoute, DpStarRoute> DPs;
	CArray<DpStarRoute, DpStarRoute> STARs;
	int lastDP, lastSTAR;

	void listSTARs(CArray<CString, CString> &result);
	void listDPs(CArray<CString, CString> &result);
	DpStarRoute* lookupDP(CString name);
	DpStarRoute* lookupSTAR(CString name);
	DpStarRoute* nextSTAR();
	DpStarRoute* firstSTAR();
	DpStarRoute* nextDP();
	DpStarRoute* firstDP();
	void addSTAR(DpStarRoute R);
	void addDP(DpStarRoute R);
	DpStarAirport() {lastDP=0;lastSTAR=0;};
	DpStarAirport(CString i) {lastDP=0;lastSTAR=0;icao=i;};
	DpStarAirport(DpStarAirport &rhs);
	DpStarAirport &operator=(DpStarAirport &);  
};

class DpStar {
public:
	void writeCompiledFile(CString filename);
	void readCompiledFile(CString filename);
	CArray<DpStarAirport, DpStarAirport> airports;

	void readAllFiles();
	void readFile(CString filename);
	void writeFile(CString filename);
	void addAirport(DpStarAirport A);
	DpStarAirport* lookupAirport(CString icao);
	DpStar(DpStar &rhs);
	DpStar();
	~DpStar();
};

#endif // !defined(AFX_DPSTAR_H__89DDEA00_CDAB_11D3_B498_005056816121__INCLUDED_)
