// Navdoc.h : interface of the CNavDoc class
//
/////////////////////////////////////////////////////////////////////////////
struct dbRec {
	short type;
	double lat;
	double lon;
	int alt;
	double freq;
	char id[6];
	char name[31]; 
	double magdev;
	double approach;
	double range;
	short file;	
};

struct airportRec {
	char name[31]; 
	float lat;
	float lon;
};

struct vorRec {
	double freq; 
	double lat;
	double lon;
};

struct latLon {
	double lat;
	double lon;
};

class CNavDoc : public CDocument {
protected: // create from serialization only
	CNavDoc();
	DECLARE_DYNCREATE(CNavDoc)

// Attributes
public:
	CArray<vorRec, vorRec> vorDB;
	CArray<int, int> runwayIndex;
	CArray<int, int> ilsIndex;
	CArray<int, int> vorIndex;
	CArray<int, int> ndbIndex;
	CArray<int, int> markerIndex;
	CArray<int, int> gpsIndex;
	CArray<int, int> airportIndex;
	CArray<int, int> atisIndex;
	CArray<int, int> dynscnIndex;
	CArray<int, int> dynobjIndex;
	CArray<int, int> waypointIndex;
	CArray<int, int> routeIndex;
	CArray<CString, CString> dbFiles;
	CArray<dbRec, dbRec> db;
	CArray<airportRec, airportRec> airportTable;
	CArray<int, int> mapTable;
	CArray<unsigned short, unsigned short> magVarTable;
	CString listFileName;
	int listFiles;
	int numbercounter;
	int mapsExist;
	void ReadMagVarTable();
	int magVarTableExists;
	int idcounter;
	int dbCount;
	CString fName;
	int dbFileIndex;
	int searchingBGLFiles;
	DpStar dpStar;

// Operations
public:
	int OnMapInsertgpsfix(double lat, double lon, CString name = "", CString id = "");
	void ReadMaps();
	CString GenUniqueId();
	CString GenUniqueNumber();
	void Index();
	int newList();
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	double GetMagVar(double lat, double lon);
	int read3bytes(char *b);
	double readDM(char *b);
	double readMicro4(char *b);
	double readMicro6Lat(char *b);
	double readMicro6Lon(char *b);
	int readFile(CString filename);
	void addDB(int type, double lat, double lon, int alt, double freq, char *id, char *name, double magdev, double approach, double range, char* filename = "");
	void formatData(CString& out, int index);
	void findFile(CString path, CArray<CString, CString> &exist, int &count, int recurse, int skip);
	void findAllBGLFiles(CString startPath, CArray<CString, CString> &results);
	void OnFileLoad(CString fileListName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString parseString(CString &line);
	int extractWaypointsAndRoutes(CString filename, CMap< CString, LPCSTR, latLon, latLon& > &waypoints, CArray<CString, CString> &routes);
	int includeAFDinfo;
	double courseDiff(double c1, double c2);
	int findNearbyNavaid(double lat, double lon, CString id, int preferType=0); 
	int findNearestNavaid(double lat, double lon, int type=3); // defaults to VOR
	int findNearestInlineNavaid(double slat, double slon, double elat, double elon, double startDist, int type=3); // defaults to VOR
	int findIndex(int type, double lat, double lon, int alt, double freq, char *id, char *name, double magdev, double approach, double range, int howClose = 50, int findMin = 0);
	int lookUpILS(int runwayIndex, int &firstILS, int &secondILS);
	int lookUpRunways(int airportIndex, int *runwaysIndexList);
	int fileNewer(CString fileName1, CString fileName2);
	int lookUpAirport(CString &name, double lat, double lon, int dbIndex=-100);
	int airportsExist;
	void ReadAirports();
	void ClearDatabases();
	int ReadCompiledScenery(CString fileName);
	int compiledSceneryList;
	int waypointsExist;
	void ReadWaypoints();
	int duplicateAirport(char *name, double lat, double lon);
	virtual ~CNavDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNavDoc)
	afx_msg void OnFileAdd();
	afx_msg void OnUpdateFileAdd(CCmdUI* pCmdUI);
	afx_msg void OnFileLoad();
	afx_msg void OnFileSearch();
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileExport(CCmdUI* pCmdUI);
	afx_msg void OnMapInsertgpsfix();
	afx_msg void OnUpdateFileAddtoscenerylistNew(CCmdUI* pCmdUI);
	afx_msg void OnFileAddtoscenerylistNavlst();
	afx_msg void OnUpdateFileAddtoscenerylistNavlst(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadscenerylistNavlst();
	afx_msg void OnFileSearchforbglfilesNavlst();
	afx_msg void OnFileAddtoscenerylistNew();
	afx_msg void OnFileSearchforbglfilesNew();
	afx_msg void OnFileSavecompiledscenerylist();
	afx_msg void OnUpdateFileSavecompiledscenerylist(CCmdUI* pCmdUI);
	afx_msg void OnFileDeletecompiledscenerylist();
	afx_msg void OnUpdateFileDeletecompiledscenerylist(CCmdUI* pCmdUI);
	afx_msg void OnFileExtractfs2000waypointsandroutes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
