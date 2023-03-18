/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2021]  TaiZe Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TaiZe Inc and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TaiZe Inc and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TaiZe Inc.
 */

#ifndef ONED_IMPELLER_H
#define ONED_IMPELLER_H
#include "1d_vaned.h"
#include "1d_leakpath.h"
#include "1d_rearDisk.h"
#include "1d_shroud.h"
#include "1ds_mc_map.h"
#include "1d_vaneless.h"

class TMPASetting;
class T_EXPORT_1D T1dImpeller : public T1dVaned
{
  T_OBJECT;
public:
  T1dImpeller(QString object_n = "", TObject *iparent = NULL);

public:

  /* put your updates of calculated variables after getting data from xml  */
  virtual void createItemPlot(bool modify = false);

  /* exposed public properties, which will be saved and shared with others */
public:
  void Init();
  virtual void initFromGeom(QList<T3DInterfaceData*> data);
  virtual void InitGeom(T1dStation *in); //initialize geometry for design mode view
  virtual int ClearanceFlow(double &dPcl, double &Ucl, double &mcl);
  virtual int ClearanceLoss(double &lc_clr);
  virtual int ClearanceLossBase(double &lc_clr);
  virtual int PassShockLoss(double &lc_shk);
  virtual int IncidenceLoss(double &lc_inc);
  virtual int IncidenceLossBase(double &lc_inc);
  virtual int LoadingLoss(double &lc_load);
  virtual int LoadingLossBase(double &lc_load);
  virtual int FrictionLoss(double &lc_frict);
  virtual int FrictionLossBase(double &lc_frict);
  virtual int DistortionLoss(double &lc_distort);
  virtual int DistortionLossBase(double &lc_distort);
  virtual int MixingLoss(double &lc_mixing);
  virtual int MixingLossBase(double &lc_mixing);
  virtual int ChokeLoss(double &lc_choke);
  virtual int ChokeLossBase(double &lc_choke);
  virtual int LossModel(double &lc);
  virtual int LossModelBase(double &lc);
  void inletBlockage(double &block_in);
  void UpdateDesincidence();
  virtual int ExitBlockage(double &blockage);
  virtual int ExitBlockageBase(double &blockage);
  double Deviation_Stechkin(double &slipFactor);
  double Deviation_WBA(double &slipFactor);//Wisener-Busemann-Aungier slip factor model
  virtual double DeviationModel(double &slipFactor);
  virtual double DeviationModelBase(double &slipFactor);
  double InletRecirculation();
  double OutletRecirculation();
  //double F_mix(double &me_loc);
  virtual double recirculationPowerLoss();
  virtual double recirculationPowerLossBase();
  virtual double diskFrictionPowerLoss();
  virtual double diskFrictionPowerLossBase();
  void CalcParasiticLoss();  
  void AddSeals();
  void CreateFrontLeakPath();
  void CreateRearLeakPath();
  void SolveFrontLeakage();
  void SolveRearLeakage();
  virtual void UpdateGeometry();
  virtual void UpdateContour();
  virtual void UpdateContourPump();
  void UpdateContourFan();
  void UpdateContourRadialCompressor();
  void UpdateContourSimpleFan();
  void UpdateContourWithCycleStraight();
  void UpdateContourWithLine();
  virtual void UpdateContourDS();//double suction
  void plotPortedShroud();
  virtual void FindU(double& u1s, double& u1h, double& u2s, double& u2h);
  void calPump_ule_hub();
	virtual void getBladeSetupPropList(QStringList& propList, QStringList& readOnlyPropList, QMap<QString,QString>& dispMapList);
  virtual void scaling(double sf, bool scale_thick, double min_thick, bool scale_clr, double min_clr);

  void calculateStaticTorque(double &torque_hub, double &torque_shroud);
  void adjustStaticTorque();
	virtual void reportOutput(QString prefix, bool child = false);
  virtual int Analysis();
  virtual bool checkStall();
  virtual void GetReportPropertyList(QVector<property_t*>& properties, int mach_type = 0, bool includeGeom = true);
  virtual void GetConfigPropertyList(QVector<property_t*>& properties);//get property list for configuration dialog
  double Func_DhD2(double phi);
  double Func_R1h_R1s(double phi);
  double Func_D1De();
  double Func_D1De(double phi);
  double Func_Lz(double phi);
  double Func_W1(double R_eye);
  double Func_thick(double R2, double thick_min);
  double Func_R2(double phi);
  double Func_PhiLe(double phi);
  double F_mix(double &me_loc);
  double beta2_vari(double &loc);
  int SolveInducer(bool isSetEdgeGeo = true);
  int SolveExducer();
  double MinimizeW1();
  void Trimming(double deltaR);
  int bracketReye(double& ax, double& bx, double& cx, double &fa, double &fb, double &fc);
  int bracketBetaK(double& ax, double& bx, double& cx, double &fa, double &fb, double &fc);
  void SetLeBladeAngle(T1dStation *le, double incid);
  void SetLeBladeAngle1(T1dStation *le, double incid);
  void SetEdgeGeometry();
  virtual double ThroatArea(int station); //station=0 for inlet, station=1 for outlet
  virtual void updateGUI(bool reloadTree = false);
  void getLeGeom(double rle, double &z_le, double& b_le, double& phi_le);
  virtual T1dStation* getBladeEdgeStation(int edgeType); // edgeType = 0 for LE(or in),  edgeType = 1 for TE( or out)
  int DesignInlet(double phi, int nIter, int nIter_j);
  int DesignOutlet(T1dStation *st1, T1dStation *st2, bool hasVaneDiff, double phi, double I, int nIter);
  int MinimizeW1t(double &r1t);
  int bracketR1t(double& ax, double& bx, double& cx, double &fa, double &fb, double &fc);
  double W1tFunc(double R1t);
  static double W1tFuncWrapper(void *thisPointer, const double R1t)
  {
    T1dImpeller *pthis = (T1dImpeller *)thisPointer;
    return pthis->W1tFunc(R1t);
  }

  static double W1FuncWrapper(void *thisPointer, const double Reye)
  {
    T1dImpeller *pthis = (T1dImpeller *)thisPointer;
    return pthis->Func_W1(Reye);
  }

  void Area2Radius(double area);
  virtual int PrelimSizing(double phi);
  //int ImpPrelimSizing(double phi, bool hasVaneDiff, bool firstTime, double eta );
  int ImpPrelimSizing(double phi, bool hasVaneDiff, double I);
  int PumpSizing(T1dStation *st0 = 0);
  int PumpSizing_modularization(T1dStation *st0 = 0);
  int inletPumpSizing(T1dStation* st0 = 0);
  int outletPumpSizing();
  int outletRadiusPumpSizing();
  virtual void bladeNumberPumpSizing();
  void impPumpSizing();
  
  double getPumpMotorSize(double power);
  double KU2a_pump_Stepanoff(double _Ns = 0.);
  double KU2a_pump_Lobanoff();
	double KD2_corr_pump(double _Ns = 0.);
	double Kb2_corr_pump(double _Ns = 0.);
	double Kb2_pump(double _Ns = 0.);
  double B_correct(double _Ns = 0.);
	double K_Iron(double _Ns = 0.);
	double InExcretion(double _Ns = 0., double D2_A = 0.);
	double OutExcretion(double _Ns = 0., double D2_A = 0.);
	double NPSHr_Minimal(double _Ns = 0.);
  double Deye_D2_ratio_Lobanoff();
	double Km1_pump(double _Ns = 0.);
	double PreWorkCo_pump(double _Ns = 0.);
  double Km2_pump(double _Ns = 0.);
  double Km2_pump_Lobanoff();
	double b2D2_pump(double _Ns = 0.);

  double recommendedBeta2ForPumpImp(double Ns);
  virtual void recommendedWrapAngleForPumpImp(double Ns);
	int recommendedBladeNumber(double Ns);
	double deltaNSforwrapangle(double Ns,int Z_wrap);

  int FanSizing(double P_rise, double Qin, double rpm, double rho, double Ns, double Nq, double phi, bool hasVaneDiff, double I);
  int FanSizingOfAxialRaial2D(double P_rise, double Qin, double rpm, double rho, double Ns, double Nq, double phi, bool hasVaneDiff, double I);
  int MultiVaneFanSizing(double P_rise, double Qin, double rpm, double rho, double Ns, double Nq, double phi, bool hasVaneDiff, double I);
  int FanSizingOf3DVaned(double P_rise, double Qin, double rpm, double rho, double Ns, double Nq, double phi, bool hasVaneDiff, double I);
  int DesignInletOfFans(double phi, double I, double Ns, double Nq, double& b1, int niter);
  int DesignInletOfFansNonBarrierFree(double phi, double I, double Ns, double Nq, double& b1, int niter);
  double WidthOfLE_design();
  int AdjustRChToMatchWidthofLE();
  int DesignInletOfMixedFans(double phi, double I, double Ns, double Nq, double& b1, int niter);
  int DesignOutletOfFans(T1dStation *st1, T1dStation *st2, bool hasVaneDiff, double phi, double Ns, double Nq, double I, double& b1, int OIter, int IIter);
  int DesignOutletOfMVRFans(T1dStation *st1, T1dStation *st2, bool hasVaneDiff, double phi, double Ns, double Nq, double I, double& b1, int OIter, int IIter); // TODO: modified this fuction name
  int DesignOutletOfMixedFans(T1dStation *st1, T1dStation *st2, bool hasVaneDiff, double phi, double Ns, double Nq, double I, double& b1, int OIter, int IIter);
  bool AdjustBeta2bOfFan(int i, double y, double &x, double multipler, double& x0, double& x1, double& y0, double& y1);
  void recommandWrapAngleForFanImp();
  double optWrapAngleFromMerdinalSizeAndMetalBladeAngle(TNurbsCurve* pZRCurve, double u1, double u2, double beta1b, double beta2b);
  virtual void FirstGuess(double phi);
  virtual void UpdateBlade();
  void calculateLETE();
  virtual void updateBladeContour();
  virtual void UpdateBeta();
  void validateBladeType();
  virtual void UpdateBetaWithWrapAngle();//conformal mapping
  virtual void UpdateBetaWithCycle();//conformal mapping
  void UpdateBeta3DRuleBlade();
  // by Feihong for axial element free blade
  inline double getbetam(double betar, double phi);
  inline double getbetar(double betam, double phi);
  void getMeanbetab(double beta1b, double beta2b, double& beta1br, double& beta2br);
  void getAllbetab(double beta1br, double beta2br);
  void UpdateBetaAxialElementFreeBlade();
  void Get_MbarTheta_conformalMapping_all(double beta1br, double beta2br, double wrapAngle);
  double cal_Mbar_conformalMapping(TNurbsCurve* pZRCurve1, TNurbsCurve* pZRCurve2, double u1, double u2);
  void get_mBetafromMbarTheta(TNurbsCurve* pZRCurve, double u1, double u2, TNurbsCurve* pmBetaCurve, TNurbsCurve* pMbarThetaCurve);
  //
  void UpdateContourMVR();
  void UpdateContourNonClog();
  void UpdateContourOverHang();
  void InitGeomNonClog(T1dStation *in); //initialize geometry for design mode view
  int NonClogImpSizing(T1dStation *st0 = 0);
  void UpdateContourOfMixed();
  void getZRForBladeCompistions(
    int position, // 0. LE - > 1. turing axial to flat -> 2.tring flat->sca
    QVector<Double2> &ZRs, // size of ZRs will be 3
    QVector<double>& us
  );
  void UpdateBetaWithCompistions();// Compositions of blade
  void UpdateBetaWithFreeRadial(double tZ, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h);
  void UpdateBetaWithFreeRadial_quadratic(double tZ, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h);
  void UpdateBetaWith2DRadial(double tZ, double tR, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h);
  void UpdateBetaWith2DAxial(double tR, int npt1, double *m_s, double *m_h, double *beta_s, double *beta_h);
  void UpdateBetaWith2DAxial_Cycle(double tR, int npt1, double *m_s, double *m_h, double *beta_s, double *beta_h);
  virtual void UpdateThick();
  void SetDefaultThick();
	void SetDefaultThickOfFan();
	void SetDefaultThickOfPump();
  void SetDefaultThickOfNonClog();
	void UpdateStreamline();
	void InsertPointData(double *x_in, double *y_in, double *x_end, double *y_end, int *pos);
	void MeridionalDistance(double *MM);
	void AdjustCamberBase();
	void AdjustCamberBeta(int pos_real, int i, double theta_ins, double theta_end, double beta2b, double beta1b);
	void AdjustCamberWrapU();
	Double2 getBasePointForDoubleSuctionImpeller();
	void SetDefaultThickfromK(double metal_inblockage, double metal_outblockage, double K_IRON);
	void SetThickfromBlockage(double inblockage, double outblockage, double gamma01, double gamma02);
  void CalculateBladeNumber(T1dStation *st1, T1dStation *st2);
  void Beta_K(double K, int np, double *betas, double * betah, double *betam, double *ms, double *mh, double *mm);
  void CurveBeta(double K);
  double calcLean();
  double minFuncLean(double K);
  double getInletFlowCoefficent();
  double getOutletFlowCoefficent();
  double getExitBeta();
  void setExitBeta(double beta2b);
  void setExitBetaOfFan(double beta2b);
  void setContourPhi2(double Ns);
  bool isMultiVane();
  static double minFuncLeanWrapper(void *thisPointer, double u)
  {
    T1dImpeller *pthis = (T1dImpeller *)thisPointer;
    return pthis->minFuncLean(u);
  }
  T1dmcmap* getMCM();
  int SolveOD();
  void SetupMCMap4DP(T1dmcmap *mcm);
  int SolveImp(double lam, double eff);
  virtual void GetAvailableInputPropertyList(QStringList &SL, int iflag = 0);
	virtual void GetDataReductionPList_AxialThrust(QStringList &SL);
  virtual void GetAvailableThrustParameterPropertyList(QStringList& L,int flag = 0);
  /*flag=0 Calculate axial thrust with TT analysis solver flag=1 Calculate axial thrust without TT analysis solver*/
	void setDR_axialThrust(int ipoint, TMPASetting *dr);
	void setGeometry_axialThrust(TMPASetting *dr);
	void setTargetValue_axialthrust(QString type, double propertyValue);
  virtual void update_after_read(bool recursive = true, bool updateProperty = true) override;

public:
  T1dLeakPath *frontLeakPath = 0;
  T1dLeakPath *rearLeakPath = 0;
  bool hasFrontSeal;
  bool hasRearSeal;
  T1dStation *sta_le = 0;
  T1dStation *sta_te = 0;
  T1dVaneless *inducer = 0;
  T1dVaned *imp = 0;
  T1dVaneless *exducer = 0;
  T1dRearDisk * rearDisk = 0;
  T1dShroud * shroud = 0;
  TCurve * pShroudPlot = 0;
  TCurve * pFrontLeakPlot = 0;
  TCurve * pRearLeakPlot = 0;
  TNurbsCurve* pFrontLeakPathContour = 0;
  bool is2d; //is 2d blade. othewise 3d blade
  int type_app; //0: process compressor, 2d; 1:turbocharger; 
  double Mu;
  double power;
  int trim;
  bool hasInletGeom;
  double eta_tt_rotor_ad; //efficiency include parasitic loss
  double eta_ts_rotor_ad;
  double eta_tt_rotor_nodf; //efficiency without parasitic loss 
  double eta_ts_rotor_nodf;
  double deltaEta_parasitic;
  double lam_nodf; //lamda without parasitic loss

  //LE location
  double d1dE; //D1/De;
  double dHd2; //Dhub/D2
  double AeA0; //Ae/A0
  double T0d2; //thick0/D2
    //design mode
  double alpha2_target; //target design flow angle alpha2
  int  inletDesignOption; //0:minimize W1t; 1:R1t specified; 2:Intake number; 3:Minimal NPSHr; 
                         //4:Speed Coefficient method for pump; 5:R1t and R2a ratio specified; 6. Deye/D2 ratio by Lobanoff
  int outletDesignOption; //0: R2 and beta2b both calculated; 1: fix beta2b, adjust R2; 2, fix R2, adjust beta2b; 
          //3: User specified flow coefficient, adjust beta2b. 4: model machine; 
  int R1hOption; //1: R1h specified; 0: R1h_R1t specified
  int OutletRaOption; //0: Speed coefficient method CN; 1: Work coefficient; 2:Ku2 by Stepanoff; 3: Outlet Radius specified; 4: Ku2 by Lobanoff
  int OutletBOption; //0: no set; 1:Outlet width ratio; 2: Outlet number by Stepanoff; 3: Outlet Width specified; 4. Capacity Constant by Lobanoff
  double b2_specified; // Outlet Width specified;
  double R1h_specified;
  double R1t_specified;
  bool b1_specified; //the inelt width specified 
  double fdS; //Min.rel.velocity factor for pump
  double K0; //Inlet Rs paremeter for speed coefficient method CN
  double Km1; //Intake number
  double Km2; //Outlet number; it should be Capacity Constant
  double Kd; // Outlet Ra coefficient for speed coefficient method CN
  double Kb; // Outlet b coefficient for speed coefficient method CN
  double workCoeff_d; //work coefficient
  double Ku2; //stepanoff outlet coefficient
  double Lamda_c; //NPSHr coefficent: Lamda_c
  double Lamda_w; //NPSHr coefficent: Lamda_w
  double Lamda_w_offset; //Lamda_w offset
  QVector<Double3> lamda_w_dr_vec; //lamda_w offset vector

  double b2d2;//Outlet width and diameter ratio
  double D1D2;//Outlet width and diameter ratio
  double Laxial_D; //Axial length to diameter ratio
  double R1t_R2a;//R1t and R2a ratio, Yue can use for Fan
  double xi; //Decelation coefficient from Inlet tp LE, Yue can use for Fan
  double xi2; //Decelation coefficient from LE tp TE, Yue can use for Fan
  double RCs; //Radius of curvature at shroud Contour
  double RCh; //Radius of curvature at hub Contour 
  double RatioCone; // For MVR, Radius Ratio of Cone  at bottom and top
  double ConeAngle; // Cone Angle
  double GammaThick; // blade lean angle from hub 
  double phi_d; //design input: flow coefficient
  double Lamda_d; //design input: swirl coefficent: Ct2/U2
  double beta2b_d; //design input: exit blade angle
  double U2_d; //design input for model machine, impeller wheel speed
  double D2_d; //design input for model machine, impeller diameter
  double mu_d; //design input for model machine, impeller head coefficient
  bool inducerStall; //inducer stall flag
  //Pecirc, Pclr, PleakF contribute to the flow enthalpy rise
  double Peuler; //euler power dH0*in->massflow
  double Precirc; //recirculation power loss
  double Pclr; //clearance power loss
  double PleakF; //front leakage power loss;
  double Pparasitic; //total parasitic power loss
  double k_parasitic; //parasitic power/euler power
  //PleakR, Pdisk do NOT contribute to the flow enthalpy rise.
  double PleakR; //rear leakage power loss;
  double Pdisk; //disk friction power loss
  double mLeakF;
  double mLeakR;
  bool isDesignPoint;
  bool isDoubleSuction;
  bool isOverHang;
  //axial thrust calculation
  void calcAxialThrust();
  double axF_total;
  double axF_shroud, axF_back;
  double axF_inlet, axF_outlet;
	double axThurst_balanced = 0;
  bool excludeShaftFront, excludeShaftRear;
  int nrow_splitter;
  //impeller disk
  double thick_shroud; //shroud cover thickness
  double thick_disk; //impeller disk thickness measured at the impeller tip
  //double d_shaft; //shaft diameter
  double axLen_disk;
  //balance piston
  bool hasBalancePiston;
  double d_piston;
  double Psuction;
  double dp_suction_line;
  double beta_bar_adjust;
  //temporary variable
  bool nb_fixed;

  double R_boss; //  axle head boss, userdifine or design in GUI
  double Z_boss; // axle head boss z coordinate, internal calculate 
  double L_boss;  // Z_boss - in->Za, in GUI
  double L_boss_AL; // axle head boss length to imp axial length, in design GUI
  double phi_boss; // axle head boss, equals to phi_hub1, in GUI

  double p_parasitic_offset; //parasitic power offset
  QVector<Double3> dp_para_dr_vec; //parasitic power offset vector
  double p_parasitic_offset_in; //lc offset

  //virtual int Analysis();
  //virtual int Design();
  
  //new off-design scheme
  double a01; //sonic speed at inlet
  double Mud, Phid, Etad_tt, Etad_ts, Lamd, DORd; //impeller design point parameter
  double phi0;
  double deltaLam;
  double Pparasitic_d; //parasitic loss at design point
  double chokeMargin;

  double Ma_Wmax;
  int contourType;//0:Bezier; 1:Straight+cycle
  //int SolveImp_choke(double lam, double eta_tt);
  bool isSizing = false;
//ported shroud model
  double Z_bsi = 0; //Z location of bleed slot location
  double gap_bsi = 0; //bleed channel inlet gap width
  double gap_bse = 0; //bleed channel exit gap width
  double Kt=1.0; //bleed channel resistence
  double K_pbsi=1.0; //multiplier for bleed slot inlet pressure
  double K_pbse=1.0; //multiplier for bleed slot exit pressure
  double swirl_factor = 0.5; //the Ctheta/U at the bleed slot
//for recirculation flow at low flow rate
  double power_ported_shroud = 0; //bypass flow power loss
//for bypass flow at high flow rate
  double throat_bypass = 1.0; //throat enlarge factor due to bypass
  int solve_ported_shroud();

private:
  double throatAreaAdjusted=0;
  double block_bypass = 0;

private:
  virtual void GetConfigPassagePropertyList(QVector<property_t*>& properties, int source = 0/*0: config 1:optimization*/);//get property list for configuration dialog
  virtual void GetConfigBladePropertyList(QVector<property_t*>& properties);
  virtual void GetConfigDesignPropertyList(QVector<property_t*>& properties, int source = 0/*0: config 1:optimization*/);
  virtual void GetConfigGeometryPropertyList(QVector<property_t*>& properties, int source = 0/*0: config 1:optimization*/);
  void clearExistingCurve(QString curvePrefix = "");
  void createCurve(double* const data, int& index);
  void setGeomUse3DCurve();
  void updateWidgetsGeometry();

  double LoadingLossBase_correctFor_mixPump(double _lc);

public:
  double getOpcFlowRate();
  double getOpcRPM();
  void shutOffPrediction(double &deviation);
  double getSpecificSpeed();
  virtual void GetDRVariable(double &var1, double &var2);//for datareduction

  public:
    int AxLengthOption = 0;//0.Using axial length 1.Using axial length to diameter ratio
};

#endif // 1D_IMPELLER_H
