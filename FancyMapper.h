// FancyMapper.h: interface for the FancyMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FANCYMAPPER_H__3FE8B060_8D06_11D2_82FB_444553540001__INCLUDED_)
#define AFX_FANCYMAPPER_H__3FE8B060_8D06_11D2_82FB_444553540001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct visibilityStruct {
	int mag;
	BYTE r;
	BYTE g;
	BYTE b;
};

class FancyMapper  
{
public:
	int Load(char * fname);	
	int project(double lati, double longi);
	void loadVisibilty();
	void saveVisibilty();
	void DrawMaps(CDC *pDC, void *view, int flat, int mag, double centerLat, double centerLon);
	void SetOptions(void *view);
	FancyMapper();
	virtual ~FancyMapper();

	visibilityStruct visibility[64];
	int mapsDetected;
	double viewlatD, viewlongD, viewdistD, viewscaleD;
	int flatMap;
	int showall;
	int decimateD;
};

#endif // !defined(AFX_FANCYMAPPER_H__3FE8B060_8D06_11D2_82FB_444553540001__INCLUDED_)
