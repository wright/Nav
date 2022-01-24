// Navview.h : interface of the CNavView class
//
/////////////////////////////////////////////////////////////////////////////
#include "FancyMapper.h"
#include <winsock.h>

#define MAXPLAN 500

// define a structure for network data
struct netData {
  double lat;		       // latitude  
  double lon;			   // longitude  
  double heading;		   // heading 
  double alt;			   // longitude 
  double tas;			   // true airspeed  
  double mag;			   // magnetic variation  
  char callsign[30];	   // name to use on display
};

class drawData {
public:
	struct netData nData;
	struct sockaddr_in source;
	int drawn;
	time_t timeStamp;
	drawData(struct netData *nD=NULL, struct sockaddr_in *from=NULL);
	drawData(drawData *dd);
};

class drawDataHolder {
public:
	CArray<drawData*, drawData*> data;
	~drawDataHolder();
	void add(drawData *dd);
	void deleteItem(int index) ;
	void deleteAll();
	int GetSize();
};

struct idPlace { // structure for "smart" ID placement
	int x;  // first corner
	int y;
	int sx; // opposite corner
	int sy;
};

struct windStruct {
	int speed;
	int direction;
};

class CNavView : public CView {
// Operations
protected: // create from serialization only
	CNavView();
	DECLARE_DYNCREATE(CNavView) 

// Attributes
public:
	FancyMapper *fm;
	void LinkDraw(CDC *pDC, drawData *dd);
	double lastLatRead, lastLonRead;
	HANDLE hthread, hflythread, hflyevent, hShutdown, hlinkdrawingthread;
	int linkFirst;
	double linkUpdateTime;
	double linkFlyUpdateTime;
	int linkFlySetMode;
	double flyingLat;
	double flyingLon;
	double flyingAlt;
	int flying;
	int link;
	CMap<int, int, windStruct, windStruct> windtable;
	void DrawGrid(CDC *pDC);
	int Mready;  // view point matrix data
	double M11;
	double M12;
	double M13;
	double M21;
	double M22;
	double M23;
	double M31;
	double M32;
	double M33;
	void Mcalc();
	void DrawMaps(CDC* pDC);
	double dmax;
	int gcDistDraw(double lat1, double lon1, double lat2, double lon2, double &dist, double &bearing, double &proj);
	double sign(double x);
	double centerLon;
	double centerLat;
	double convY;
	double convX;
	int drawMessageLoop();
	void CenterAt(double lat, double lon);
	int showAirportZones;
	int showMaps;
	int showFancyMaps;
	void ZoomToFit();
	LOGFONT lf;
	int OS;
	int symbolTransition;
	int largeSymbols;
	int distToPixels(double dist);
	int flightTrackDensity;
	int flightPaths;
	void LocationBar();
	int plancount;
	int plan[MAXPLAN];
	HWND debugHwnd;
	CNavDoc* GetDocument();
	FlightPlan *planD;
	int planning;
	double idealDist;
	int planVOR;
	int planNDB;
	int planAirport;
	int planWaypoints;
	int planContinue;
	int planOnly;
	int mag;
	int mag2;
	int abort;
	int init;
	int initAfterPrint;
	int grid;
	int vor;
	int ils;
	int ndb;
	int id;
	int freq;
	int airports;
	int markers;
	int atis;
	int planInfo;
	int dynscn;
	int dynobj;
	int waypoints;
	int routes;
	double top;
	double bottom;
	double right;
	double left;
	CRect clientRect;
	int screenWidth;
	int screenHeight;
	int LtoC(double xl, double yl, int &xc, int &yc);
	int CtoL(int xc, int yc, double &xl, double &yl);
	void Size(int cx, int cy);
	LOGFONT fontstruct;
	int fontSet;
	CString title;
	CString search;
	double gcDist(double lat1, double lon1, double lat2, double lon2);
	void gcDistInterpolate(double lat1, double lon1, double lat2, double lon2, double percent, double &lat3, double &lon3);
	void gcLine(int x1, int y1, int x2, int y2, CDC *dc, int black=1);
	void StoC(double &x, double &y, double &z, double lat, double lon, double r);
	void CtoS(double x, double y, double z, double &lat, double &lon, double &r);
	void StoCe(double &x, double &y, double &z, double lat, double lon);
	int measuring;
	int imx, omx, imy, omy;
	double bearing;
	int flatWorld;
	void fixup(double &lon, double &lat);
	void planOpts(CWnd *parent = NULL);
	CString orgDir;
	void idPlacement(int centerx, int centery, idPlace &idP, CArray<idPlace, idPlace> *idlist, int ils=0);
	int xLinkExtent, yLinkExtent;
	int forceBlackPrinting;
	int linesIntersect(CPoint p1, CPoint p2, CPoint p3, CPoint p4);
	int linesIntersect(CPoint p1, CPoint p2, CRect r);
	int gcLine(double slon, double slat, double elon, double elat, CDC *pdc, int black, int depth = 0);
	int networkInitialized;

// Attributes          
public:
	virtual void OnDropFiles(HDROP hDropInfo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetAllDrawData(drawDataHolder &all);
	int centerOnAircraft;
	int lastViewZoomFinished;
	void ZoomLastView();
	int lastViewZoom;
	double lastViewLon;
	double lastViewLat;
	int startWithLastView;
	void midPoint(double slat, double slon, double elat, double elon, double &mlat, double &mlon);
	void addCraft(CString name, int cls, int clf, int clr, int crs, int crf, int alt, int des, int def, int der);
	char callsign[30];
	void LinkDrawText(CDC *pDC, int x, int y, char *text);
	int drawing;
	void DoLinkDrawing();
	int linkDrawing;
	int enableNetwork;
	void netSend(char *stuff, int size);
	void shutdownNetwork();
	void initNetwork();
	CString lastSceneryList;
	int startWithLastList;
	void drawDist(CDC *pDC, int dist, CPoint from, CPoint to);
	double pointDistance(CPoint r, CPoint s);
	double closestCorner(CPoint target, CRect rect, CPoint &closest);
	int isLinked();
	void stopFlyTo();
	BOOL SendToPlan(int index);
	int SearchNavAidNearPoint(double startDist,CPoint point);
	void ConvertDistHeadToCoord(double distance,double heading, double& latitude, double& longitude);
	BOOL fromNavAid;
	int navAidIndex;
	void IncrementFlytoWaypoint();
	int flyToWaypoint;
	void FlyToWaypoint(int waypointNumber);
	virtual ~CNavView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNavView)
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewGridlines();
	afx_msg void OnUpdateViewGridlines(CCmdUI* pCmdUI);
	afx_msg void OnViewNdb();
	afx_msg void OnViewIls();
	afx_msg void OnViewVor();
	afx_msg void OnViewAirports();
	afx_msg void OnUpdateViewAirports(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewIls(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNdb(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewVor(CCmdUI* pCmdUI);
	afx_msg void OnViewId();
	afx_msg void OnUpdateViewId(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFrequency(CCmdUI* pCmdUI);
	afx_msg void OnViewFrequency();
	afx_msg void OnEditChangefont();
	afx_msg void OnEditSettitle();
	afx_msg void OnViewSearch();
	afx_msg void OnViewDatabase();
	afx_msg void OnEditSaveoptions();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewFlatworld();
	afx_msg void OnUpdateViewFlatworld(CCmdUI* pCmdUI);
	afx_msg void OnEditFlightplan();
	afx_msg void OnUpdateEditFlightplan(CCmdUI* pCmdUI);
	afx_msg void OnPlanPlanoptions();
	afx_msg void OnMapShowPlandataonly();
	afx_msg void OnUpdateMapShowPlandataonly(CCmdUI* pCmdUI);
	afx_msg void OnViewMarkers();
	afx_msg void OnUpdateViewMarkers(CCmdUI* pCmdUI);
	afx_msg void OnMapShowPlaninformation();
	afx_msg void OnUpdateMapShowPlaninformation(CCmdUI* pCmdUI);
	afx_msg void OnFileImport();
	afx_msg void OnUpdateMapClearflighttrack(CCmdUI* pCmdUI);
	afx_msg void OnMapClearflighttrack();
	afx_msg void OnMapSetflighttrackdensity();
	afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
	afx_msg void OnMapLargesymbols();
	afx_msg void OnUpdateMapLargesymbols(CCmdUI* pCmdUI);
	afx_msg void OnMapSettransition();
	afx_msg void OnMapZoomtofit();
	afx_msg void OnUpdateMapZoomtofit(CCmdUI* pCmdUI);
	afx_msg void OnMapShowMapsymbolsAirportzones();
	afx_msg void OnUpdateMapShowMapsymbolsAirportzones(CCmdUI* pCmdUI);
	afx_msg void OnMapShowMaps();
	afx_msg void OnUpdateMapShowMaps(CCmdUI* pCmdUI);
	afx_msg void OnMapCenterat();
	afx_msg void OnMapAtis();
	afx_msg void OnUpdateMapAtis(CCmdUI* pCmdUI);
	afx_msg void OnFs6Link();
	afx_msg void OnUpdateFs6Link(CCmdUI* pCmdUI);
	afx_msg void OnFs6Set();
	afx_msg void OnFs6Fs6linkoptions();
	afx_msg void OnFs6Flyto();
	afx_msg void OnUpdateFs6Flyto(CCmdUI* pCmdUI);
	afx_msg void OnHelpviewdocs();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewWaypoints();
	afx_msg void OnUpdateViewWaypoints(CCmdUI* pCmdUI);
	afx_msg void OnViewRoutes();
	afx_msg void OnUpdateViewRoutes(CCmdUI* pCmdUI);
	afx_msg void OnMapShowFancymaps();
	afx_msg void OnUpdateMapShowFancymaps(CCmdUI* pCmdUI);
	afx_msg void OnSetFancymapOptions();
	afx_msg void OnUpdateSetFancymapOptions(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlan0(CCmdUI* pCmdUI);
	afx_msg void OnPlan0();
	afx_msg void OnFileStartwithlastlist();
	afx_msg void OnUpdateFileStartwithlastlist(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMapStartwithlastview();
	afx_msg void OnUpdateMapStartwithlastview(CCmdUI* pCmdUI);
	afx_msg void OnFlightsimMapfollowsaircraftNone();
	afx_msg void OnUpdateFlightsimMapfollowsaircraftNone(CCmdUI* pCmdUI);
	afx_msg void OnFlightsimMapfollowsaircraftLocal();
	afx_msg void OnUpdateFlightsimMapfollowsaircraftLocal(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Navview.cpp
inline CNavDoc* CNavView::GetDocument()
   { return (CNavDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
