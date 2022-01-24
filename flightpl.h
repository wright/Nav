// flightpl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FlightPlan dialog

struct planRec {
	int index;
	double lat;
	double lon;
	double windVel;
	double windheading;
	double heading;
	double dist;
	double Tdist;
	double ete;
	double Tete;
	double fuel;
	double Tfuel;
	double alt; // in feet
	double correctedheading;
	double correctedvelocity;
	int dpStar;
};

class FlightPlan : public CDialog
{
// Construction
public:
	void mergeDpStar(CString icao, CString route, int isStar);
	int planToSel(int pIndex);
	int selToPlan(int sIndex);
	int getSel();
	int enableDPSTARButton();
	int getImage(int k);
	CString starName, dpName;
	void checkDpStar();
	int wptDp, wptStar;
	void ExportFile(CString filename);
	CString GetFS2000Path();
	void WriteBriefing(CString name, int dep , int arr);
	int updating;
	CString openPlanName;
	CString alternateID, alternateName;
	double alternateLat, alternateLon, alternateAlt;
	double parseFS2000position(char dir, int deg, double minsec);
	void formatFS2000position(char *buf, double decimal, int latFlag);
	void selectPlanItem(int index);
	CImageList *oldImageList, imageList;
	CString PlanTableName(int planItem);
	void pspace(int s, FILE *fp, char c = ' ');
	CString readList(int row, int Column);
	int excessWind;
	void averageWind(double m1, double a1, double m2, double a2, double &m3, double &a3, double percent = .5);
	int wind(double &ihead, double ispeed, double ehead, double &espeed, double whead, double wspeed);
	void setColumnWidth();
	HICON m_hIcon;
	int ibod, itoc, C, D;
	int adjust(int index, int forward, int mode);
	void buildTable();
	FlightPlan(CWnd* pParent = NULL);   // standard constructor
	~FlightPlan();  
	CArray<planRec, planRec> plantable;
	char craftBuffer[50];
	long cruisealtitude;
	double descentfuel;
	double climbfuel;
	double cruisefuel;
	int cruisespeed;
	int climbspeed;
	int descentspeed;
	int climbrate;
	int descentrate;
	int dragging;
	int sel;
	void update();
	void checkButtons();

// Dialog Data
	//{{AFX_DATA(FlightPlan)
	enum { IDD = IDD_FLIGHTPLAN };
	CListCtrl m_list;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FlightPlan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FlightPlan)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnClear();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnDelete();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAuto();
	afx_msg void OnOptions();
	afx_msg void OnPrint();
	afx_msg void OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAircraft();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnWind();
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileExport();
	afx_msg void OnFileExportSquawkBox();
	afx_msg void OnFileExportACSGPS();
	afx_msg void OnFileExportSuperFlightPlanner();
	afx_msg void OnFileImportSuperFlightPlanner();
	afx_msg void OnFileImportOldNav();
	afx_msg void OnFileExportRadarContact();
	afx_msg void OnFileExportFS2000();
	afx_msg void OnFileImportFS2000();
	afx_msg void OnFlytoplan();
	afx_msg void OnDpstar();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
