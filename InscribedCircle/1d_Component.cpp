/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2020] TaiZe Inc
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


#include "1d_Component.h"
#include "1d_Stage.h"
#include "display_TDObject.h"
#include "1dd_StageAddComp.h"
#include "wz_TWizardExample.h"
#include "1dd_ComponentDlg.h"
#include "1d_machine.h"
#include "w_ComponentSelection.h"
#include "w_TTaskWindow.h"
#include "w_TModule.h"
#include "w_PropertyHolderDialog.h"
#include "w_Property.h"
#include "util_OUtil.h"
#include "util_DisplayConst.h"
#include "io_XML.h"
#include "post_ObjectDialogPropertySelector.h"
#include "1d_Window.h"
#include "1d_voluteTwinInlet.h"
#include "util_HtmlReportTitle.h"
#include "1d_3DInterface.h"
#include "util_util.h"
#include "util_Translator.h"
#include "InscribedCircleCalculatorWidget.h"

REGISTER_OBJECT_CLASS(T1dComponent, "component", TPlotItem);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(T1dComponent, "Component");
QStringList T1dComponent::configTabList = QStringList() << "Passage" << "Blade" << "Design" << "Geometry"
<< "Loss" << "Deviation" << "Blockage" << "Range" << "Vane";
T1dComponent::T1dComponent(QString object_n, TObject *iparent) :
  TPlotItem(object_n, iparent), in(0), out(0), T1DFluidHelper(this), pHubContour1(0),
  pLNsolver(0), //laval nozzle solver
  pThroat(0), // throat: smallest area for the component
  pSolverSetting(0), //solver setting, temporary variable
  pHubContour(0),
  pShroudContour(0),
  pMeanContour(0),
  pInlet(0),
  pOutlet(0),
  pHubContour2(0),  //defined from LE to TE
  pHubBeta(0),
  pShroudBeta(0),
  pHubThick(0),
  pShroudThick(0),
  pMeanBeta(0),
  pMeanThick(0),
  _useGeomCurve(false)
{
	INIT_OBJECT;
	DEFINE_SCALAR_INIT(bool, isMultiSeg, "Multi segment", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, nType, "Type", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, AxRad, "Axial/radial/mix", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, HMT, "Meanline/HMT", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, L_axial, "Axial length", 0, NULL, TUnit::length);
	DEFINE_SCALAR_INIT(double, RPM, "RPM", 0, NULL, TUnit::RPM);
	DEFINE_SCALAR_INIT(double, RPM_hub, "RPM hub", 0, NULL, TUnit::RPM);
	DEFINE_SCALAR_INIT(double, RPM_shroud, "RPM shroud", 0, NULL, TUnit::RPM);
	DEFINE_SCALAR_INIT(double, e_pass, "Passage roughness", 0, NULL, TUnit::length); property("e_pass")->setRange(0., 1.);
	DEFINE_SCALAR_INIT(bool, isFirstComp, "Is first comp", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, geomValid, "Geometry valid", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, _useGeomCurve, "Use 3D geometry curves", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, inletGeomOption, "Inlet geometry option", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, outletGeomOption, "Outlet geometry option", 0, NULL, NULL);
	//DEFINE_SCALAR_INIT(int, rFlag, "rFlag", 0, NULL, NULL);
	//DEFINE_SCALAR_INIT(int, bFlag, "bFlag", 0, NULL, NULL);
	//DEFINE_SCALAR_INIT(double, r2r1, "Outlet to inlet radius ratio", 0, NULL, NULL);
	//DEFINE_SCALAR_INIT(double, b2b1, "Outlet to inlet width ratio", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, inputRatio, NULL, 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, Rout_Rin, "Rout/Rin", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, Bout_Bin, "Bout/Bin", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, phi1_hub, "Hub contour angle at inlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, phi1_shroud, "Shroud contour angle at inlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, phi1_mean, "Mean contour angle at inlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, phi2_hub, "Hub contour angle at outlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, phi2_mean, "Mean contour angle at outlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, phi2_shroud, "Shroud contour angle at outlet", 0, NULL, TUnit::deltaAngle);
	DEFINE_SCALAR_INIT(double, Dh_avg, "hydraulic diameter", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, Dh_in, "hydraulic diameter(inlet)", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, Dh_out, "hydraulic diameter(outlet)", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, lc_user, "Loss coefficient specified", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, lc_offset, "Loss coefficient offset", 0.0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(dlc_dr_vec, 0, 0, 0, NULL, 0);
	property("lc_offset")->setDataReductionVectorName("dlc_dr_vec"); // set the parameter vector
	DEFINE_SCALAR_INIT(double, lc_offset_in, "Loss coefficient offset specified", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, deltaEta, "Efficiency decrement", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, LossModelType, "Loss model type", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_inc, "LC multiplier: Incidence", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_shock, "LC multiplier: Shock", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_frict, "LC multiplier: Friction", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_load, "LC multiplier: Loading", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_dif, "LC multiplier: Diffusion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_choke, "LC multiplier: Choke", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_distort, "LC multiplier: Distortion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_mix, "LC multiplier: Mixing", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_clr, "LC multiplier: Clearance", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_recirc, "Recirculation loss multiplier", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_cr, "LC multiplier: Mach", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_moist, "LC multiplier: Moist", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_disk, "LC multiplier: Disk friction", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_leakage, "LC multiplier: Leakage", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_pat, "LC multiplier: Partial admission", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_pro, "LC multiplier: Profile", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_sf, "LC multiplier: Second flow", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_te, "LC multiplier: Trailing", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_ex, "LC multiplier: Expansion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_lw, "LC multiplier: Lashing wire", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_parasitic, "Parasitic loss multiplier", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_total, "LC multiplier: total", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_bearingLoss, "Bearing loss factor", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mlc_SealLoss, "Seal loss factor", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, block_user, "Blockage specified", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mf_block, "Blockage multiplier", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, delta_block, "Blockage offset", 0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(dblk_dr_vec, 0, 0, 0, NULL, 0);
	property("delta_block")->setDataReductionVectorName("dblk_dr_vec"); // set the parameter vector
	DEFINE_SCALAR_INIT(double, delta_block_in, "Blockage offset specified", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, blockModel, "Block model", 0, NULL, NULL);

	//leakage path inlet static pressure and temperature and tangent velocity multipelier
	DEFINE_SCALAR_INIT(double, mlc_leakPs, "Leakage path inlet static pressure multiplier", 0.0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(mlc_leakPs_dr_vec, 0, 0, 0, NULL, 0);
	property("mlc_leakPs")->setDataReductionVectorName("mlc_leakPs_dr_vec"); // set the parameter vector

	DEFINE_SCALAR_INIT(double, mlc_leakTs, "Leakage path inlet static temperature multiplier", 0.0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(mlc_leakTs_dr_vec, 0, 0, 0, NULL, 0);
	property("mlc_leakTs")->setDataReductionVectorName("mlc_leakTs_dr_vec"); // set the parameter vector

	DEFINE_SCALAR_INIT(double, mlc_leakCt, "Leakage path inlet tangent velocity multiplier", 0.0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(mlc_leakCt_dr_vec, 0, 0, 0, NULL, 0);
	property("mlc_leakCt")->setDataReductionVectorName("mlc_leakCt_dr_vec"); // set the parameter vector

	//for turbine
	DEFINE_SCALAR_INIT(double, mlc_leakPs_out, "Leakage path out static pressure multiplier", 0.0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(mlc_leakPs_out_dr_vec, 0, 0, 0, NULL, 0);
	property("mlc_leakPs_out")->setDataReductionVectorName("mlc_leakPs_out_dr_vec");

	//output
	DEFINE_SCALAR_INIT(double, LC_inc, "LC: Incidence", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_shock, "LC: Shock", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_frict, "LC: Friction", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_load, "LC: Load", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_dif, "LC: Diffusion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_choke, "LC: Choke", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_distort, "LC: Distortion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_mix, "LC: Mixing", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_clr, "LC: Clearance", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_cr, "LC: Mach", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_pro, "LC: Profile", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_sf, "LC: Secondflow", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_te, "LC: Trailing", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_ex, "LC: Expansion", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_lw, "LC: Lashingwire", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_moist, "LC: Moisture", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_disk, "LC: Disk friction", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_leakage, "LC: Leakage", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC_pat, "LC: Partial admission", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, LC, "LC: total", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, CP, "CP: total", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, SC, "SC", 0.0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, SC_offset, "SC offset", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, SC_offset_in, "SC offset specified", 0, NULL, NULL);
	DEFINE_QDOUBLE3_VECTOR_INIT(dsc_dr_vec, 0, 0, 0, NULL, 0);
	property("SC_offset")->setDataReductionVectorName("dsc_dr_vec"); // set the parameter vector
	DEFINE_SCALAR_INIT(double, eta_dif, "Diffuser efficiency", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, eta_noz, "Nozzle efficiency", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, chokeFlag, "Choke flag", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, isStalled, "Is stalled", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, isChoked, "Is choked", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, showPlot, "Show plot", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, mu_opt, "Optimum Mu", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, f_mu_min, "Loss multiplier at minimum Mu", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, f_mu_max, "Loss multiplier at maximum Mu", 0, NULL, NULL);

	DEFINE_SCALAR_INIT(bool, phi1h_set, "Is hub contour angle at inlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, phi1m_set, "Is hub contour angle at inlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, phi1s_set, "Is shroud contour angle at inlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, phi2h_set, "Is hub contour angle at outlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, phi2m_set, "Is mean contour angle at outlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(bool, phi2s_set, "Is shroud contour angle at outlet set", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, tipContourType, "Tip contour type", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, hubContourType, "Hub contour type", 0, NULL, NULL);
  	DEFINE_SCALAR_INIT(double, mChoke, "Choke mass flow rate", 0, NULL, TUnit::mass_flow);
  DEFINE_SCALAR_INIT(bool, useREQS, "use REQS", 0, NULL, NULL);

  DEFINE_SCALAR_INIT(int, compRunMode, "Component run mode", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, compDesTargetOption, "Component design target option", 0, NULL, NULL);
	DEFINE_QSTRINGLIST_INIT(textReport, "Text report", 0, NULL, NULL);
  
	DEFINE_LIST(segList);
	DEFINE_CHILD(in, T1dStation, 0);
	DEFINE_CHILD(out, T1dStation, 0);
  DEFINE_CHILD_HIDDEN(pInlet, TNurbsCurve, "plot-inlet");
  DEFINE_CHILD_HIDDEN(pShroudContour, TNurbsCurve, "plot-shroud");
  DEFINE_CHILD_HIDDEN(pOutlet, TNurbsCurve, "plot-outlet");
  DEFINE_CHILD_HIDDEN(pHubContour, TNurbsCurve, "plot-hub");
  DEFINE_CHILD_HIDDEN(pMeanContour, TNurbsCurve, 0);
	DEFINE_CHILD(pLNsolver, T1dLavalNoz, 0);
	DEFINE_CHILD_HIDDEN(pHubContour1, TNurbsCurve, 0);
	Init();
	isDataReduction = false;
	tipContourType = ContourTypes::contourType_tapered;
	hubContourType = ContourTypes::contourType_tapered;
	meanContourType = ContourTypes::contourType_tapered;
	dr_data_option = 0;
	compRunMode = comp_localmode_analysis;
  compDesTargetOption = compDesTarget_CP;
	deltaEta = 0;
	set_icon_name(":images/1d-component.png");
}

void T1dComponent::scaling(double sf, bool scale_thick, double min_thick, bool scale_clr, double min_clr)
{
	L_axial *= sf;
	in->scaling(sf, scale_thick, min_thick, scale_clr, min_clr);
	out->scaling(sf, scale_thick, min_thick, scale_clr, min_clr);
  pHubContour1->scale(sf);
  pShroudContour->scale(sf);
  pHubContour->scale(sf);
  pMeanContour->scale(sf);
  getRmsContour()->scale(sf);
}

void T1dComponent::flowCut(double delta_flow)
{
	in->flowCut(delta_flow);
	out->flowCut(delta_flow);
  out->Zs = out->Zh - out->b*sin(out->phi);
  if (getUseGeomCurve())
  {
    pShroudContour->Interpolate(pShroudContour, pHubContour1, delta_flow);
    double y = 0.;
    pShroudContour->getPoint(1, out->Zs, y);
  }
  out->Za = 0.5*(out->Zs + out->Zh);
  L_axial = out->Za - in->Za;
}

T1dComponent::~T1dComponent()
{

}

void T1dComponent::SetComponentLossParameter(T1dComponent *c)
{
	if (!c) return;

	c->LossModelType = LossModelType;
	//c->lc_offset = lc_offset;
	c->mlc_inc = mlc_inc;
	c->mlc_shock = mlc_shock;
	c->mlc_frict = mlc_frict;
	c->mlc_load = mlc_load;
	c->mlc_dif = mlc_dif;
	c->mlc_choke = mlc_choke;
	c->mlc_distort = mlc_distort;
	c->mlc_mix = mlc_mix;
	c->mlc_clr = mlc_clr;
	c->mlc_cr = mlc_cr;
	c->mlc_te = mlc_te;
	c->mlc_total = mlc_total;
	c->lc_user = lc_user;
	c->mlc_disk = mlc_disk;
}

void T1dComponent::SetComponentBlockageParameter(T1dComponent *c)
{
	c->blockModel = blockModel;
	c->block_user = block_user;
	//c->delta_block = delta_block;
	c->mf_block = mf_block;
	c->block_bl = block_bl;
}

void T1dComponent::HideObj()
{
	SET_OBJ_HIDE(pInlet);
	SET_OBJ_HIDE(pShroudContour);
	SET_OBJ_HIDE(pOutlet);
	SET_OBJ_HIDE(pHubContour);
	SET_OBJ_HIDE(pMeanContour);
	//in->HideObj();
	//out->HideObj();
	SET_OBJ_HIDE(pLNsolver);
	SET_OBJ_HIDE(filledCurveData);
}

TFluid * T1dComponent::GetFluid()
{
	TFluid *pfld = 0;
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
	if (pMachine)
	{
		pfld = pMachine->fluid;
	}
	return pfld;
}

bool T1dComponent::isUserInput(TObject *o, QString p_str)
{
	property_t *  p = o->property(p_str);
	if (!p) return false;
	bool userInput = IF_PROPERTY_USER_OVERWRITE(p);
	return userInput;
}

void T1dComponent::Init()
{
	//setStringProperty("_window_name", "T1dDockWidget");
	setStringProperty("_window_name", "T1dWindow");
	nType = type_none;
	isFirstComp = false;
	RPM = 0;
	deltaEta = 1.0;
	SC = 1.0;
	LC_inc = 0;
	LC_shock = 0;
	LC_frict = 0;
	LC_load = 0;
	LC_dif = 0;
	LC_choke = 0;
	LC_distort = 0;
	LC_mix = 0;
	LC_clr = 0;
	LC_cr = 0;
	LC_pro = 0;
	LC_sf = 0;
	LC_te = 0;
	LC_ex = 0;
	LC_lw = 0;
	LC_leakage = 0;
	LC_pat = 0;
	LC = 0;
	Delta_leak = 0;
	Delta_adm = 0;
	Delta_df = 0;
	Delta_gap = 0;
	Delta_q = 0;
	lc_flag = 0;
	mlc_inc = 1.0;
	mlc_shock = 1.0;
	mlc_frict = 1.0;
	mlc_load = 1.0;
	mlc_dif = 1.0;
	mlc_choke = 1.0;
	mlc_distort = 1.0;
	mlc_mix = 1.0;
	mlc_clr = 1.0;
	mlc_cr = 1.0;
	mlc_parasitic = 1.0;
	mlc_leakage = 1.0;
	mlc_recirc = 1.0;
	mlc_pat = 1.0;
	mlc_pro = 1.0;
	mlc_sf = 1.0;
	mlc_te = 1.0;
	mlc_ex = 1.0;
	mlc_lw = 1.0;
	mlc_disk = 1.0;
	//loss adjustment
	LossModelType = 0;
	lc_user = 0;
	lc_offset = 0;
	mlc_total = 1.0;
	//blockage
	blockModel = 0;
	block_user = 0;
	delta_block = 0;
	mf_block = 1.0;
  //SC
  SC_offset = 0;
  SC_offset_in = 0;

	L_axial = 0.0;
	L_merid = 0.0;
	Dh_in = 0.0;
	Dh_out = 0.0;
	Dh_avg = 0.0;
	e_pass = 20.0E-6; //passage is assumed to be sand casted, N8, 20 micro-meter peak-to-valley
	isMultiSeg = false;
	numSeg = 0;
	isCompressor = true;
	isVaned = false;
	isStalled = false;
	geomValid = true;
	inletGeomOption = 0;
	outletGeomOption = 0;
	showPlot = true;
	mu_opt = 0.8;
	f_mu_min = 1.1;
	f_mu_max = 1.5;
	phi1h_set = false;
	phi1s_set = false;
	phi2h_set = false;
	phi2s_set = false;
	//bearing loss and seal loss
	mlc_bearingLoss = 0.01;
	mlc_SealLoss = 0.01;
}

void T1dComponent::initSegs() // virtual interface
{

}

void T1dComponent::SetBezier(bool is_bezier)
{
#if 1 //use Bezier curve, not NURBS for now because OCC uses Bezier
	pInlet->useBezier(is_bezier);
	pShroudContour->useBezier(is_bezier);
	pOutlet->useBezier(is_bezier);
	pHubContour->useBezier(is_bezier);
	pHubContour1->useBezier(is_bezier);
	pMeanContour->useBezier(is_bezier);
  	getRmsContour()->useBezier(is_bezier);
	//pHubContour2->useBezier(is_bezier);
#endif
}

void T1dComponent::create(void *win, bool modify)
{
#if 0 // skip certain component from plotting
	if (
		(parent("T1dMachine") && QString("output_dp") == parent("T1dMachine")->oname()) ||
		//QString("impeller") == oname() ||
		QString("inducer") == oname() ||
		QString("imp") == oname() ||
		QString("exducer") == oname()
		)
		return;
#endif

	if (win)
		getWindow() = win;

	if (getWindow())
	{
		createItemPlot(modify);
	}

	TPlotItem::create(win, modify);

	displayItemPlot();

}

void T1dComponent::calcDisplacement(T1dStation *st1, T1dStation *st2, double &dZ, double &dR)
{
	dZ = st2->Za - st1->Za;
	dR = st2->Ra - st1->Ra;
}

void T1dComponent::setDisplacement(double dZ, double dR)
{
	T1dStation *st = in;
	st->Za = st->Za + dZ;
	st->Zh = st->Zh + dZ;
	st->Zs = st->Zs + dZ;
	st->Ra = st->Ra + dR;
	st->Rh = st->Rh + dR;
	st->Rs = st->Rs + dR;

	st = out;
	st->Za = st->Za + dZ;
	st->Zh = st->Zh + dZ;
	st->Zs = st->Zs + dZ;
	st->Ra = st->Ra + dR;
	st->Rh = st->Rh + dR;
	st->Rs = st->Rs + dR;
}

void T1dComponent::displayItemPlot(void)
{
	if (ifHide(this))
		return;
	QString dObjectClassName = "TDObject1D";
	QString displayName = "MRPlane";
	TDObject * od = TDObject::getDisplayObject(this, dObjectClassName, "", -1, displayName);
	if (od && showPlot)
		od->addObject(this);
}

void T1dComponent::ArcContour(double X1, double Y1, double dx, double dy, double alpha1, double alpha2, TNurbsCurve *pZR)
{
	double L = sqrt(dx*dx + dy * dy);
	double sinphi = dy / L;
	double cosphi = dx / L;
	double denom = 1 - cos(alpha1 - alpha2);
	double dL = (dy*(sin(alpha2) - sin(alpha1)) - dx * (cos(alpha1) - cos(alpha2))) / denom; //L2-L1
	int nl = fabs(dL) / (L / 15); //number of points for the straightline
	if (nl < 1)nl = 1;
	double L1 = 0;
	double L2 = 0;
	if (dL > 0)
	{
		L2 = dL;
		L1 = 0;
	}
	else
	{
		L1 = -dL;
		L2 = 0;
	}

	double Rc = dx - (L2*cos(alpha2) + L1 * cos(alpha1)) / (sin(alpha2) - sin(alpha1));
	double Xc = X1 + L1 * cos(alpha1) - Rc * sin(alpha1);
	double Yc = Y1 + L1 * sin(alpha1) + Rc * cos(alpha1);
	//now get the points on curve
	double x[50] = {0.}, y[50] = { 0. };
	int i = 0;
	x[i] = X1;
	y[i] = Y1;
	//first segment
	if (L1 > 0)
	{
		double dl = L1 / nl;
		for (int j = 0; j < nl + 1; j++)
		{
			i++;
			x[i] = x[i - 1] + dl * cos(alpha1);
			y[i] = y[i - 1] + dl * sin(alpha1);
		}
	}
	//arc part, from alphac1 to 0.5*PI-alpha2
	int np_arc = 10;
	double dalpha = (alpha2 - alpha1) / np_arc;
	for (int j = 1; j < np_arc + 1; j++)
	{
		double a_i = alpha1 + dalpha * j;
		x[j + i] = Xc + Rc * cos(1.5*PI + a_i);
		y[j + i] = Yc + Rc * sin(1.5*PI + a_i);
	}
	int np = i + np_arc + 1;

	//last segment
	if (L2 > 0)
	{
		double ddl = L2 / nl;
		for (int j = 0; j < nl; j++)
		{
			x[np] = x[np - 1] + ddl * cos(alpha2);
			y[np] = y[np - 1] + ddl * sin(alpha2);
			np++;
		}
	}

	//fit bezier
	int nc = 6;
	pZR->fitBezier(x, y, np, nc, 1, 3, 1);
}

void T1dComponent::contourCurve(double z0, double r0, double phi1, double z4, double r4, double phi4, double *ZR, double t1)
{
	int npt = 5;
	QVector<Double2> pts; pts.resize(npt);
	pts[0] = Double2(z0, r0);
	pts[4] = Double2(z4, r4);
	Double2 L04 = pts[4] - pts[0];
	Double2 Z_axis = { L04.length(), 0. };

	Double2 Line_1[2] = { pts[0], pts[0] + Z_axis.rotate(phi1) };
	Double2 Line_2[2] = { pts[4], pts[4] + Z_axis.rotate(phi4) };
	Double2 p_insert;
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0 || sit == 4)
		p_insert = (pts[0] + pts[4]) / 2.;
	pts[2] = p_insert;
	// i = 1
	Double2 _tmp = pts[2] - pts[0];
	_tmp = Double2(_tmp.length() / 4., 0.);
	pts[1] = pts[0] + _tmp.rotate(phi1);
	// i = 3
	_tmp = pts[4] - pts[2];
	_tmp = Double2(_tmp.length() / 4., 0.);
	pts[3] = pts[4] + _tmp.rotate(phi4);

	Double2 pt_23 = (pts[1] + pts[3]) / 2.;
	pts[2] = (p_insert + pt_23) / 2.;

	for (int i = 0; i < 5; i++)
	{
		ZR[2 * i] = pts[i][0];
		ZR[2 * i + 1] = pts[i][1];
	}
}

#if 0
// original
void T1dComponent::contourCurve(double z0, double r0, double alf0, double z4, double r4, double alf4, double *ZR, double t1)
{
	double k1 = 0;
	double k2 = 0;
	double da1 = alf0 - 0.5*PI;
	double da2 = alf4 - 0.5*PI;
	double da1a = alf0 + 0.5*PI;
	double da2a = alf4 + 0.5*PI;

	double small_d = 1.0E-2;
	bool vert1 = false;
	bool vert2 = false;
	bool vert1a = false;
	bool vert2a = false;
	if (fabs(da1) > small_d && fabs(da1a) > small_d)
	{
		k1 = tan(alf0);
	}
	else if (fabs(da1) <= small_d)
	{
		k1 = 1.0 / small_d;
		vert1 = true;
	}
	else if (fabs(da1a) <= small_d)
	{
		k1 = -1.0 / small_d;
		vert1a = true;
	}

	if (fabs(da2) > small_d && fabs(da2a) > small_d)
	{
		k2 = tan(alf4);
	}
	else if (fabs(da2) <= small_d)
	{
		k2 = 1.0 / small_d;
		vert2 = true;
	}
	else if (fabs(da2a) <= small_d)
	{
		k2 = -1.0 / small_d;
		vert2a = true;
	}

	if ((vert1 & vert2) || (vert1a&&vert2a))
	{
		ZR[0] = z0;
		ZR[1] = r0;
		ZR[2] = z0 + 0.25*(z4 - z0);
		ZR[3] = r0 + 0.25*(r4 - r0);
		ZR[4] = z0 + 0.50*(z4 - z0);
		ZR[5] = r0 + 0.50*(r4 - r0);
		ZR[6] = z0 + 0.75*(z4 - z0);
		ZR[7] = r0 + 0.75*(r4 - r0);
		ZR[8] = z4;
		ZR[9] = r4;
		return;
	}
	double dk = k2 - k1;
	double z2 = 0.5*(z0 + z4);
	double r2 = 0.5*(r0 + r4);
	if (fabs(dk) > small_d)
	{
		z2 = (r0 - r4 + k2 * z4 - k1 * z0) / dk;
#if 0
		double zchk = (z2 - z0)*(z2 - z4);
		if (zchk > small)
		{
			z2 = 0.5*(z0 + z4);
		}
#endif
		r2 = r0 + k1 * (z2 - z0);
#if 0
		double rchk = (r2 - r0)*(r2 - r4);
		if (rchk > small)
		{
			r2 = r0;
		}
#endif

	}

	//fill the array
	double t = 0.2;
	ZR[0] = z0;
	ZR[1] = r0;
	ZR[2] = z0 + t * (z2 - z0);
	ZR[3] = r0 + t * (r2 - r0);
	ZR[6] = z4 - t * (z4 - z2);
	ZR[7] = r4 - t * (r4 - r2);
	ZR[8] = z4;
	ZR[9] = r4;
	double z2a = 0.5*(ZR[2] + ZR[6]);
	double r2a = 0.5*(ZR[3] + ZR[7]);
	ZR[4] = t1 * z2a + (1 - t1)*z2;
	ZR[5] = t1 * r2a + (1 - t1)*r2;
}


#endif

void T1dComponent::setContourPhi()
{
  // this funtion just handl general situation.
  // please creat a new virtual fuction for spefical component.
#if 0
	if (this->haveBaseClass("T1dImpeller"))
	{
		bool isImpeller = true;
		// tell me wheather is T1dImpeller?
	}
#endif 

	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));// this -> 
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(pStage->parent("T1dMachine"));
	if (this->haveBaseClass("T1dInlet") && (pStage->stageID > 1))
	{
		int iStage = pStage->stageID - 1;
		int nComp = pMachine->stageList[iStage - 1]->compList.size();
		phi1_hub = pMachine->stageList[iStage - 1]->compList[nComp - 1]->phi2_hub;
		phi1_shroud = pMachine->stageList[iStage - 1]->compList[nComp - 1]->phi2_shroud;
	}
	else
	{
		if (!isUserInput(this, "phi1_hub") && !phi1h_set)
		{
			//phi1_hub = in->phi+PI/18;
			phi1_hub = in->phi;
			if (this->haveBaseClass("T1dRotor"))
			{
				if (in->phi > -PI * 60 / 180)
					phi1_hub = -PI * 60 / 180;
			}
		}
		if (!isUserInput(this, "phi1_shroud") && !phi1s_set)
		{
			phi1_shroud = in->phi;
			if (this->haveBaseClass("T1dRotor"))
			{ 
				if (in->phi > -PI * 30 / 180)
					phi1_shroud = -PI * 30 / 180;
			}
		}
	}
	if (this->haveBaseClass("T1dInlet"))
	{
		T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
		T1dInlet* pInlet = dynamic_cast<T1dInlet*>(this);
		if (pInlet && pInlet->createdByImport)
		{
			phi2_hub = 0.;
			phi2_shroud = 0.;
		}
		else if (pStage && pStage->compList.size() > 1)
		{
			T1dComponent *m_pNextComp = pStage->compList[1];
			phi2_hub = m_pNextComp->phi1_hub;
			phi2_shroud = m_pNextComp->phi1_shroud;
		}
	}
	else
	{
		if (!isUserInput(this, "phi2_hub") && !phi2h_set)
		{
			phi2_hub = out->phi;
		}
		if (!isUserInput(this, "phi2_shroud") && !phi2s_set)
		{
			phi2_shroud = out->phi;
		}
	}
}

void T1dComponent::CreateInletOutlet()
{
	double ule_hub = 0, ute_hub = 1.0, ule_shroud = 0, ute_shroud = 1.0;
	if (haveBaseClass("T1dVaned"))
	{
		T1dVaned *p1D = dynamic_cast<T1dVaned *>(this);
		ule_hub = p1D->ule_hub;
		ute_hub = p1D->ute_hub;
		ule_shroud = p1D->ule_shroud;
		ute_shroud = p1D->ute_shroud;
	}

	//inlet contour
	pHubContour1->getPoint(0, in->Zh, in->Rh);
	pHubContour1->getPoint(1.0, out->Zh, out->Rh);

	//outlet contour
	pShroudContour->getPoint(0, in->Zs, in->Rs);
	pShroudContour->getPoint(1.0, out->Zs, out->Rs);

	double ZR_in[4] = { in->Zh, in->Rh, in->Zs, in->Rs };
	SET_CURVE_DATA(pInlet, ZR_in, 2);
	double ZR_out[4] = { out->Zs, out->Rs, out->Zh, out->Rh };
	SET_CURVE_DATA(pOutlet, ZR_out, 2);
}

void T1dComponent::updatePhiFromContourCurve(int contourCurveType)
{
  /*
  just update as need
  */
  enum contourCurveTypes
  {
    Hub = 0,
    Mean = 1,
    Shroud = 2
  };
  TNurbsCurve* pContour = 0;
  if (contourCurveType == Hub)
    pContour = pHubContour1;
  else if (contourCurveType == Mean)
    pContour = pMeanContour;
  else if (contourCurveType == Shroud)
    pContour = pShroudContour;

  double tan_phi1 = 0., tan_phi2 = 0.;
  pContour->getSlope(0., tan_phi1);
  pContour->getSlope(1., tan_phi2);

  Double2 Z_axis = {1., 0.};
  Double2 tang_1;
  pContour->getTangentialDirection(0., tang_1);
  Double2 tang_2;
  pContour->getTangentialDirection(1., tang_2);

  double phi1 = Z_axis.angleSigned(tang_1);
  double phi2 = Z_axis.angleSigned(tang_2);

  if (contourCurveType == Hub)
  {
    phi1_hub = phi1;
    phi2_hub = phi2;
  }
  else if (contourCurveType == Mean)
  {
    phi1_mean = phi1;
    phi2_mean = phi2;
  }
  else if (contourCurveType == Shroud)
  {
    phi1_shroud = phi1;
    phi2_shroud = phi2;
  }
}

void T1dComponent::UpdateContourBase()
{

	//create hub and shroud line
	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;
	double R2h = out->Rh;
	double R2s = out->Rs;
	double Z2h = out->Zh;
	double Z2s = out->Zs;

	//initialize all curves
	pHubContour1->SetOrder(4);
	pHubContour->SetOrder(4);
	pShroudContour->SetOrder(4);
	pInlet->SetOrder(4);
	pOutlet->SetOrder(4);

	pHubContour1->freeSISLCurve();
	pHubContour->freeSISLCurve();
	pShroudContour->freeSISLCurve();
	pInlet->freeSISLCurve();
	pOutlet->freeSISLCurve();

	double chub1[10] = {0.}, chub[10] = { 0. };
	double cshrd[10] = { 0. };
	int nc = 5;
  
	// shroud
	if (tipContourType == contourType_conical)
	{
    double cshrdm[4] = { 0.0, };
    cshrdm[0] = in->Zs;
    cshrdm[1] = in->Rs;
    cshrdm[2] = out->Zs;
    cshrdm[3] = out->Rs;
    SET_CURVE_DATA(pShroudContour, cshrdm, 2);
		updatePhiFromContourCurve(2);
	}
	else 
  {
		contourCurve(in->Zs, in->Rs, phi1_shroud, out->Zs, out->Rs, phi2_shroud + PI, cshrd);
    SET_CURVE_DATA(pShroudContour, cshrd, SMOOTH_CURVE_POINT);
	}

	// hub1
	if (hubContourType == contourType_conical)
	{
    double chrdm[4] = { 0.0, };
    chrdm[0] = in->Zh;
    chrdm[1] = in->Rh;
    chrdm[2] = out->Zh;
    chrdm[3] = out->Rh;
    SET_CURVE_DATA(pHubContour1, chrdm, 2);
    updatePhiFromContourCurve(0);
	}
	else 
  {
		contourCurve(in->Zh, in->Rh, phi1_hub, out->Zh, out->Rh, phi2_hub + PI, chub1);
		SET_CURVE_DATA(pHubContour1, chub1, SMOOTH_CURVE_POINT);
	}

  if (fabs(Z1h - Z2h) < 1.0E-5 && fabs(R1h - R2h) < 1.0E-5)//same point
  {
    double hubhub[4] = { Z1h, R1h, Z2h, R2h };
    SET_CURVE_DATA(pHubContour, hubhub, 2);
  }
  else
  {// hub
    pHubContour->CopyFrom(pHubContour1);
    pHubContour->Reverse();
  }
	// Inlet & outlet
	double ZR_in[4] = { Z1h, R1h, Z1s, R1s };
	SET_CURVE_DATA(pInlet, ZR_in, 2);
	double ZR_out[4] = { Z2s, R2s, Z2h, R2h };
	SET_CURVE_DATA(pOutlet, ZR_out, 2);

	//mean contour
	pMeanContour->Average(pHubContour1, pShroudContour);

	calculateMeanContourAngle();

  UpdateRmsContour();
}

void T1dComponent::updateBladeSectionCurve()
{
}

void T1dComponent::calcLenRatio(QVector<double>& Len, double* x, double* y, int nc)
{
	//Calculate the length of control polygon
	double Len_total = 0;
	for (int i = 1; i < nc; i++)
	{
		Len_total += sqrt(SQR(y[i] - y[i - 1]) + SQR(x[i] - x[i - 1]));
	}
	//Calculate the length ratio for each control points
	Len.clear();
	Len.push_back(0);
	Len.resize(nc);
	for (int i = 1; i < nc; i++)
	{
		if (Len_total == 0)
			Len[i] = 0;
		else
			Len[i] = Len[i - 1] + sqrt(SQR(y[i] - y[i - 1]) + SQR(x[i] - x[i - 1])) / Len_total;
	}
}

void T1dComponent::sanityCheck()
{

}

void T1dComponent::adjustCoordinate(double* Coor, QVector<double>& Len, double C1, double C2, double delta_C1, double delta_C2, int nc)
{
	if (fabs(delta_C2) < SMALL && fabs(delta_C1) < SMALL)
		return;

	double kC = 0;
	kC = (delta_C2 - delta_C1);
	forLoop(i, nc)
	{
		double dC_i = delta_C1 + kC * Len[i];
		Coor[i] += dC_i;
	}
}

/*When user config component imported from 3D module, we need to update its contour also*/
void T1dComponent::updateCurve4Geom(TNurbsCurve* curve)
{
	if (!curve)
		return;

	int nc = curve->getNumCtrl();
	int dim = curve->getDimension();
	if (dim > 1 && nc > 0)
	{
		//1.Calculate the length of control polygon and the length ratio for each control points
		double* x = curve->GetCtrlPoints(0);
		double* y = curve->GetCtrlPoints(1);
		QVector<double> Len;
		calcLenRatio(Len, x, y, nc);

		//2.adjust Z coordinate
		double Z1_old = x[0];
		double Z2_old = x[nc - 1];
		double Z1_new = in->Za;
		double Z2_new = out->Za;
		if (curve->oname().toLower().indexOf("hub") >= 0)
		{
			Z1_new = in->Zh;
			Z2_new = out->Zh;
		}
		else
		{
			Z1_new = in->Zs;
			Z2_new = out->Zs;
		}
		double delta_Z1 = Z1_new - Z1_old;
		double delta_Z2 = Z2_new - Z2_old;
		adjustCoordinate(x, Len, Z1_old, Z2_old, delta_Z1, delta_Z2, nc);

		//3.adjust R coordinate
		double R1_old = y[0];
		double R2_old = y[nc - 1];
		double R1_new = in->Ra;
		double R2_new = out->Ra;
		if (curve->oname().toLower().indexOf("hub") >= 0)
		{
			R1_new = in->Rh;
			R2_new = out->Rh;
		}
		else
		{
			R1_new = in->Rs;
			R2_new = out->Rs;
		}
		double delta_R1 = R1_new - R1_old;
		double delta_R2 = R2_new - R2_old;
		adjustCoordinate(y, Len, R1_old, R2_old, delta_R1, delta_R2, nc);

		//4.create contour curve
		QVector<double> CtrPoints;
		forLoop(i, nc)
		{
			CtrPoints.push_back(x[i]);
			CtrPoints.push_back(y[i]);
		}
		int order = curve->getOrder();
		int input_type = curve->getInputType();
		double* vknots = curve->getKnotVector();
		curve->setInputData(nc, CtrPoints.data(), SMOOTH_CURVE_POINT, order, input_type, vknots);
	}
}

void T1dComponent::updateContour4Geom()
{/*make sure that LineupComp is called before*/
	updateCurve4Geom(pShroudContour);
	updateCurve4Geom(pHubContour1);
	pHubContour->CopyFrom(pHubContour1);
	pHubContour->Reverse();
	pMeanContour->Average(pHubContour1, pShroudContour);
  UpdateRmsContour();
}

void T1dComponent::GetAvailableInputPropertyListBase(QStringList &SL, int iflag)
{
  QVector<property_t*> properties;
  int solverMode = 0;
  if (T1dObject* o = dynamic_cast<T1dObject*>(CURRENT_MODULE_OBJECT))
  {
    solverMode = o->getSolverMode();
  }
  if (solverMode == 0 || solverMode == 2)
  {
    GetConfigPassagePropertyList(properties, 1);
    if(haveBaseClass("T1dImpeller"))
      GetConfigBladePropertyList(properties);
  }
  else
  {
    GetConfigDesignPropertyList(properties, 1);
    GetConfigGeometryPropertyList(properties, 1);
  }
  foreach(property_t* p, properties)
  {
    if (!IF_PROPERTY_READONLY(p))
    {
      SL += p->getObject()->path() + "/" + p->oname();
    }
  }
}

void T1dComponent::UpdateContour()
{
	if (getUseGeomCurve())//use geometry curve
	{
		updateContour4Geom();
		updateBladeSectionCurve();
		CreateInletOutlet();
		return;
	}
	if (!in || !out ||
		!pInlet || !pShroudContour || !pOutlet ||
		!pHubContour || !pHubContour1)
		return;

	setContourPhi();
	UpdateContourBase();

	UpdateRmsContour();
}

TNurbsCurve* T1dComponent::getRmsContour()
{
	QString name = "rms contour";
	pRmsContour = dynamic_cast<TNurbsCurve*>(child(name));
	if (!pRmsContour)
	{
		pRmsContour = (TNurbsCurve*)TObject::new_object("TNurbsCurve", name, this);
		SET_OBJ_HIDE(pRmsContour);
	}
	return pRmsContour;
}

void T1dComponent::setRmsContour(TNurbsCurve* rmsContour)
{
	if (rmsContour)
		pRmsContour->CopyFrom(rmsContour);
}

void T1dComponent::UpdateRmsContour()
{
	int npt = npts;
	// 0. peparing
	QVector<TNurbsCurve*> pSourceContours = QVector<TNurbsCurve*>() << pHubContour1 << pShroudContour;
	//1. divide pSourceContours into npts points
	QVector<QVector<Double2>> zrs_AllSource;
	for (int i = 0; i < pSourceContours.size(); i++)
	{
		QVector<Double2> zrs_Currentsource;
		double du = 1. / (npt-1.);
		for (int j = 0; j < npt; j++)
		{
			double u = j * du;
			Double2 zr;
			pSourceContours[i]->getPoint(u, zr[0], zr[1]);
			zrs_Currentsource.push_back(zr);
		}
		zrs_AllSource.push_back(zrs_Currentsource);
	}

	// 2. core: calculate & get zr point of contour
	QVector<double> z_Rms, r_Rms;
	ConePlaneMapping conePane;
	for (int i = 0; i < (zrs_AllSource.first()).size(); i++)
	{
		Double2 zr = getPointbyPecentConeArea(&conePane, zrs_AllSource.first()[i], zrs_AllSource.last()[i], 0.5);
		z_Rms.push_back(zr[0]);
		r_Rms.push_back(zr[1]);
	}

	// 3. fitting nurbs
	int nCtrl = 7;
	getRmsContour()->fitBezier(&z_Rms[0], &r_Rms[0], npt, nCtrl);

	return;
}

Double2 T1dComponent::getPointbyPecentConeArea(ConePlaneMapping* pConePane, Double2& pt0, Double2& pt1, double ratioOfArea)
{
	Double2 ZAxis = { 1., 0 };

	double coneAngle = ZAxis.angleSigned(pt1 - pt0);

	pConePane->initConePlane(coneAngle);

	double deg = PI / 180.;
	bool isCloseZero = (abs(coneAngle) < 0.01 * deg);
	bool isClosePI = ( abs((abs(coneAngle)-PI)) < 0.01 * deg);
	if (isCloseZero || isClosePI)
	{
		return  (1 - ratioOfArea) * pt0 + ratioOfArea * pt1;
	}

	Polar2 pl0 = pConePane->setPlor2(pt0);
	Polar2 pl1 = pConePane->setPlor2(pt1);

	Polar2 pl_avg = 0.5 * (pl0 + pl1);

	//double R_pl_pecent = pl_avg.R() * sqrt(ratioOfArea *SQR( /) );
	//Polar2 pl_pecent;

	double R0 = pl0.R();
	double R1 = pl1.R();
	double Ravg = pl_avg.R();
	double ratio2 = (1. - ratioOfArea) * SQR(R0 / Ravg) + ratioOfArea * SQR(R1 / Ravg);
	double Rpercent = Ravg * sqrt(ratio2);

	double t = (Rpercent - R0) / (R1 - R0);

	Double2 pt_pecentByConeArea = (1 - t) * pt0 + t * pt1;

	return pt_pecentByConeArea;
}

void T1dComponent::calculateMeanContourAngle()
{
	double slop_phi_m[2] = { 0. };
	double phi_m[2] = { 0. };
	double u[2] = { 0., 1. };
	for (int i = 0; i < 2; i++)
	{
		pMeanContour->getSlope(u[i], slop_phi_m[i]);
		if (slop_phi_m[i] > 1000.)
		{
			phi_m[i] = PI / 2.;
		}
		else if (slop_phi_m[i] < -1000.)
		{
			phi_m[i] = -PI / 2.;
		}
		else
		{
			phi_m[i] = atan(slop_phi_m[i]);
		}
	}
	phi1_mean = phi_m[0];
	phi2_mean = phi_m[1];
}

void T1dComponent::clearPlotDS()
{
#if 0
	double PlotPts[] = { 0 };
	SET_CURVE_DATA(pInletDS, PlotPts, 0);
	SET_CURVE_DATA(pHubContourDS, PlotPts, 0);
	SET_CURVE_DATA(pShroudContourDS, PlotPts, 0);
	SET_CURVE_DATA(pHubContour1DS, PlotPts, 0);
	SET_CURVE_DATA(pMeanContourDS, PlotPts, 0);
	SET_CURVE_DATA(pOutletDS, PlotPts, 0);
#endif
	if (TNurbsCurve* pInletDS = (TNurbsCurve*)child("plot-inlet-DS")) TObject::delete_object(pInletDS);
	if (TNurbsCurve* pShroudContourDS = (TNurbsCurve*)child("plot-shroud-DS")) TObject::delete_object(pShroudContourDS);
	if (TNurbsCurve* pOutletDS = (TNurbsCurve*)child("plot-outlet-DS")) TObject::delete_object(pOutletDS);
	if (TNurbsCurve* pHubContourDS = (TNurbsCurve*)child("plot-hub-DS")) TObject::delete_object(pHubContourDS);
	if (TNurbsCurve* pMeanContourDS = (TNurbsCurve*)child("mean-DS")) TObject::delete_object(pMeanContourDS);
	if (TNurbsCurve* pHubContour1DS = (TNurbsCurve*)child("hub1-DS")) TObject::delete_object(pHubContour1DS);
#if 0
	if (child("plot-inlet-DS") && child("plot-inlet-DS")->getTreeItem())
		child("plot-inlet-DS")->UpdateTreeItem(false);
	if (child("plot-shroud-DS") && child("plot-shroud-DS")->getTreeItem())
		child("plot-shroud-DS")->UpdateTreeItem(false);
	if (child("plot-outlet-DS") && child("plot-outlet-DS")->getTreeItem())
		child("plot-outlet-DS")->UpdateTreeItem(false);
	if (child("plot-hub-DS") && child("plot-hub-DS")->getTreeItem())
		child("plot-hub-DS")->UpdateTreeItem(false);
	if (child("mean-DS") && child("mean-DS")->getTreeItem())
		child("mean-DS")->UpdateTreeItem(false);
	if (child("hub1-DS") && child("hub1-DS")->getTreeItem())
		child("hub1-DS")->UpdateTreeItem(false);
#endif
}

void T1dComponent::PlotDSCurve(TNurbsCurve *curve, TNurbsCurve *curveDS, double Z)
{
	int nctrl = curve->getNumCtrl();
	int np = 21;
	double xv[21] = { 0. }, yv[21] = { 0. };
	double du = 1.0 / (np - 1);
	double x = 0., y = 0.;
	for (int i = 0; i < np; i++)
	{
		double u = i * du;
		curve->getPoint(u, x, y);
		xv[i] = Z + (Z - x);
		yv[i] = y;
	}
	curveDS->fitBezier(xv, yv, np, nctrl, 1, 3, 1);
}

void T1dComponent::createItemPlot(bool modify)
{
	//UpdateContour();


	// collect them into items
	//if(!modify)
	collectPlotsIntoItems();
}

void T1dComponent::update_after_property_changed(property_t *changedProp)
{
	TPlotItem::update_after_property_changed(changedProp);
}

void T1dComponent::update_after_read(bool recursive, bool updateProperty)
{
	if (updateProperty && IF_OBJ_SHOW(this))
		UpdateGeometry();
	TObject::update_after_read(recursive, updateProperty);
}

int T1dComponent::Validate()
{
	return 0;
}

void T1dComponent::LineupComp(double Za)
{
	in->geomOption = inletGeomOption;
	out->geomOption = outletGeomOption;
	in->Za = Za;
	in->RecalcGeom();
	out->Za = Za + L_axial;
	out->Zh = in->Zh+ L_axial;
	out->RecalcGeom();
}

void T1dComponent::UpdateBlade()
{

}

void T1dComponent::fromRatio()
{
	out->Ra = in->Ra*Rout_Rin;
	out->b = in->b*Bout_Bin;
	outletGeomOption = 0;
}

void T1dComponent::toRatio()
{
	if (in->Ra != 0)
	{
		if (!isUserInput(this, "Rout_Rin"))
		{
			Rout_Rin = out->Ra / in->Ra;
		}
	}
	else
	{
		if (!isUserInput(this, "Rout_Rin"))
		{
			Rout_Rin = 0;
		}
	}

	if (in->b != 0)
	{
		Bout_Bin = out->b / in->b;
	}
	else
	{
		Bout_Bin = 0;
	}
}

void T1dComponent::updateGUI(bool reloadTree)
{
	TObject::updateGUI(reloadTree);

	loop_child_have_base_class(c, TPlotItem)
	{
		c->updateGUI(reloadTree);
	}
}

void T1dComponent::UpdateGeometry()
{
	if (inputRatio)
	{
		fromRatio();
	}
	SetBezier(false); //set all the curves for Bezier for now  
	LineupComp(in->Za);
	UpdateContour();
	if (geomValid)
	{
		UpdateBlade();
		L_merid = pMeanContour->lengthCurve();
		calcHydroDiameter();
		in->SetHMTGeom();
		out->SetHMTGeom();
	}
	else
	{
		// color = "grey";
	}
	HideObj();
#if 0
	pHubContour1->printCurve();
	pShroudContour->printCurve();
	pInlet->printCurve();
	pOutlet->printCurve();
	pHubContourDS->printCurve();
#endif
}

void T1dComponent::calcHydroDiameter()
{
	//for passage
	double A = in->Area_geom;
	double P = 2 * PI*(in->Rh + in->Rs);
	Dh_in = 4 * A / P;
	A = out->Area_geom;
	P = 2 * PI*(out->Rh + out->Rs);
	Dh_out = 4 * A / P;
	Dh_avg = 0.5*(Dh_in + Dh_out);
}

QStringList T1dComponent::getActions(void)
{
	QStringList L;
	L << "insertComponent";
	L << TObject::getActions();

	return L;
}

void T1dComponent::initFromGeom(QList<T3DInterfaceData*> data)
{
	if (data.size() == 0)
		return;
	T1dComponent::initFromGeomBase(data);
	T3DInterfaceData* p3D = data[0];
	//get curve
	pHubContour1->CopyFrom(p3D->phubContour);
	pHubContour->CopyFrom(p3D->phubContour);
	pHubContour->Reverse();
	pShroudContour->CopyFrom(p3D->pshroudContour);
	pMeanContour->Average(pHubContour1, pShroudContour);
}

bool T1dComponent::getUseGeomCurve() 
{
  if (T1dMachine::ifUseGeomCurve(this))//for old case's backward compatible
  {
    return true;
  }

  return _useGeomCurve;
}

void T1dComponent::setUseGeomCurve(bool flag)
{
  _useGeomCurve = flag;
}

int T1dComponent::ExitBlockage(double &blockage)
{
	if (blockModel == 1)
		blockage = block_user;
	else
	{
		blockage = delta_block; //no model for T1dCompoent
	}
	return 0;
}

void T1dComponent::InitGeomBack(T1dStation *sta_out)
{
	double small_d = 1.0E-5;
	TObject::copyObject(out, sta_out, true, true);
	double r_ratio = 1.0;
	if (fabs(out->phi) < small_d)
	{
		r_ratio = 1.0;
	}
	else if (out->phi < -small_d)
	{
		r_ratio = 0.9;
		if (isVaned)r_ratio = 0.8;
	}
	else
	{
		r_ratio = 1.2;
		if (isVaned)r_ratio = 1.25;
	}
	in->Ra = out->Ra / r_ratio;
	in->b = out->b / r_ratio;
	in->phi = out->phi;
	//set axial length
	double dr = out->Ra - in->Ra;
	double dphi = 0.5*PI - in->phi;

	if (fabs(in->phi) < small_d) //axial
	{
		L_axial = 0.5*in->b;
	}
	else if (fabs(dphi) < small_d) //radial
	{
		L_axial = 0.0;
	}
	else
	{
		L_axial = dr / tan(in->phi);
	}
	in->Za = out->Za - L_axial;

	inletGeomOption = 0;
	outletGeomOption = 0;
  in->RecalcGeom();

	geomValid = true;
}
void T1dComponent::InitGeom(T1dStation *sta_in)
{
	TObject::copyObject(in, sta_in, true, true);
	double r_ratio = 0;
	double small_d = 1.0E-5;
	if (in->phi > small_d)
	{
		r_ratio = 1.4;
		QString MachineType = GetMachineType();
		bool isPump = (MachineType == "pump");
		bool isFan = (MachineType == "fan");
		if (isPump || isFan)r_ratio = 1.05;
		if (isVaned)r_ratio = 1.25;
	}
	else if (in->phi < -small_d)
	{
		r_ratio = 0.9;
		QString MachineType = GetMachineType();
		bool isTurbine = (MachineType == "turbine");
		if (isTurbine&&haveBaseClass("T1dInlet")) r_ratio = 1.0;
		if (isVaned)r_ratio = 0.8;
	}
	else
	{
		r_ratio = 1.0;
		L_axial = 1.0*in->b;
	}

	out->Ra = r_ratio * in->Ra;
	out->phi = in->phi;
	out->b = in->b;
	double dr = out->Ra - in->Ra;
	double dphi = 0.5*PI - in->phi;

	if (fabs(in->phi) < small_d) //axial
	{
		L_axial = 1.0*in->b;
	}
	else if (fabs(dphi) < small_d) //radial
	{
		L_axial = 0.0;
	}
	else
	{
		L_axial = dr / tan(in->phi);
	}
	out->Za = in->Za + L_axial;
	inletGeomOption = 0;
	if (this->haveBaseClass("T1dConicalDiffuser"))
	{
		in->RecalcGeom();
		TObject::copyObject(out, in, true, true);
		outletGeomOption = 1;
	}
	else
		outletGeomOption = 0;
  out->RecalcGeom();
	geomValid = true;
}

int T1dComponent::PropagateQN(T1dStation *st1, T1dStation *st2)
{
	//passing the p0rel and h0rel to the next station (quasi-normal)
	T1dProperty *pHMT1[3];
	T1dProperty *pHMT2[3];
	pHMT1[0] = st1->phub;
	pHMT1[1] = st1->prms;
	pHMT1[2] = st1->ptip;
	pHMT2[0] = st2->phub;
	pHMT2[1] = st2->prms;
	pHMT2[2] = st2->ptip;

	for (int i = 0; i < 3; i++)
	{
		double I1 = pHMT1[i]->h0 - pHMT1[i]->U*pHMT1[i]->C;
		pHMT2[i]->h0rel = I1 + SQR(0.5*pHMT2[i]->U);
		double p02rel_isen = p_sh(pHMT1[i]->s, pHMT2[i]->h0rel);
		double q = pHMT1[i]->p0rel - pHMT1[i]->p;
		pHMT2[i]->p0 = p02rel_isen - lc[i] * q;
	}
	return 0;
}

void T1dComponent::CalculatePerformance()
{
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(pStage->parent("T1dMachine"));
	bool isTurbine = (pMachine->machineType == mach_turbine);
	if (isTurbine)
	{
		lc_flag = 1;
	}
	else
	{
		lc_flag = 0;
	}

	double p02s_rel = p_sh(in->s, out->h0rel);
	if (lc_flag == 0)
	{
		LC = (p02s_rel - out->p0rel) / (in->p0rel - in->p);
		CP = (out->p - in->p) / (in->p0rel - in->p);
	}
	else
	{
		LC = (p02s_rel - out->p0rel) / (out->p0rel - out->p);
		CP = (out->p - in->p) / (out->p0rel - out->p);
	}
	double s1 = s_ph(in->p, in->h);
	double s2 = s_ph(out->p, out->h);
	double hd2 = h_ps(out->p, s2);
	double hd1 = h_ps(in->p, s2);
	double hi2 = h_ps(out->p, s1);
	double hi1 = h_ps(in->p, s1);
	eta_dif = (hi2 - hi1) / (hd2 - hd1);
	if (out->p < in->p)
    eta_dif = 0;

	double h0i = h_ps(in->p0, s1);
	double Cd2 = 2 * (h0i - hd2);
	double Ci2 = 2 * (h0i - hi1);
	double Cad2 = 2 * (hd1 - hi1);
	eta_noz = (Cd2 - Ci2) / (Cad2 - Ci2);
	if (out->p > in->p)
    eta_noz = 0;
}

void T1dComponent::makeSegList()
{
  segList.clear();
  numSeg = segList.size();
}

T1dComponent * T1dComponent::insertCompBefore(int comp_type)
{
	T1dComponent *pNewComp = 0;
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	pNewComp = pStage->CreateNewComponent(comp_type);
	if (pNewComp)
	{
		pStage->insertChildBefore(this, pNewComp);
		pNewComp->InitBefore(in);
		pStage->UpdateGeometry();
	}
	return pNewComp;
}

T1dComponent * T1dComponent::insertCompAfter(int comp_type)
{
	T1dComponent *pNewComp = 0;
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	pNewComp = pStage->CreateNewComponent(comp_type);
	if (pNewComp)
	{
		pStage->insertChildAfter(this, pNewComp);
		pNewComp->InitAfter(out);
		pStage->UpdateGeometry();
	}
	return pNewComp;
}

void T1dComponent::InitAfter(T1dStation *sta_out)
{
	copyObject(in, sta_out, true, true);
	double phi = sta_out->phi;
	double b = sta_out->b;
	double dr = b * sin(phi);
	double dz = b * cos(phi);
	L_axial = dz;
	out->Za = in->Za + dz;
	out->Ra = in->Ra + dr;
	out->b = in->b;
	out->phi = phi;
	out->RecalcGeom();
}

void T1dComponent::InitBefore(T1dStation *sta_in)
{
	copyObject(out, sta_in, true, true);
	double phi = sta_in->phi;
	double b = sta_in->b;
	double dr = b * sin(phi);
	double dz = b * cos(phi);
	in->Za = out->Za - dz;
	in->Ra = out->Ra - dr;
	in->b = out->b;
	in->phi = phi;
	L_axial = dz;
	in->RecalcGeom();
}

bool T1dComponent::isPartofMultSeg()
{
  TObject *pParent = parent();
  if (pParent && (pParent->haveBaseClass("T1dMultiSegComp") || pParent->haveBaseClass("T1dInletChamber")))
		return true;
	else
		return false;
}

void T1dComponent::reportOutput(QString prefix, bool child)
{
	if (!child)
	{
		textReport.clear();
		QString s = prefix;
		QString cName = getComponentType(nType);
		if (nType == type_ac_rotor)
			cName = QObject::tr("rotor");
		else if (nType == type_ac_stator)
			cName = QObject::tr("stator");
		QString cUName = cName.left(1).toUpper() + cName.right(cName.size() - 1);
		QString componentType = s + cUName + QObject::tr(" geometry and property report");
		util_HtmlReportTitle title;
		title.addItem(componentType, 1, 4);
		textReport << title.toText();
	}
	QString componentType = getComponentType(this->nType);
	QString line;
	QStringList plist = QStringList() << "Rh" << "Rs" << "Rm" << "phi" << "b" << "deltaEta";
	int np = plist.size();
	QString value, unit, displayName;
	bool reportRelative = (RPM > 0);
	in->reportRelative = reportRelative;
	out->reportRelative = reportRelative;
	in->reportAllProperty(componentType, textReport, true);
}

QString T1dComponent::getComponentType(int comp_type)
{
	QString ctype;
	switch (comp_type)
	{
	case type_igv:
		ctype = QObject::tr("IGV");
		break;
	case type_vanelessInlet:
		ctype = QObject::tr("vaneless inlet");
		break;
	case type_impeller:
		ctype = QObject::tr("impeller");
		break;
	case type_wedge:
		ctype = QObject::tr("wedge vane");
		break;
	case type_airfoil_rad:
		ctype = QObject::tr("radial airfoil vane");
		break;
	case type_airfoil_ax:
		ctype = QObject::tr("axial airfoil vane");
		break;
	case type_deswirl:
		ctype = QObject::tr("deswirl vane");
		break;
	case type_conical:
		ctype = QObject::tr("conical");
		break;
	case type_vane:
		ctype = QObject::tr("generic vane");
		break;
	case type_vaneless:
		ctype = QObject::tr("vaneless");
		break;
	case type_bend_90:
		ctype = QObject::tr("90-bend");
		break;
	case type_bend:
		ctype = QObject::tr("bend");
		break;
	case type_bend_180:
		ctype = QObject::tr("180-bend");
		break;
	case type_dif_generic:
		ctype = QObject::tr("generic diffuser");
		break;
	case type_dif_airfoil:
		ctype = QObject::tr("airfoil diffuser");
		break;
	case type_dif_wedge:
		ctype = QObject::tr("wedge diffuser");
		break;
	case type_dif_generic_vaned:
		ctype = QObject::tr("generic vaned diffuser");
		break;
	case type_volute_outlet:
		ctype = QObject::tr("downstream volute");
		break;
	case type_ccx:
		ctype = QObject::tr("continuous cross over");
		break;
	case type_rc:
		ctype = QObject::tr("return channel");
		break;
	case type_volute_inlet:
		ctype = QObject::tr("inlet volute");
		break;
	case type_volute_twin_inlet:
		ctype = QObject::tr("inlet twin volute");
		break;
	case type_nozzle:
		ctype = QObject::tr("nozzle");
		break;
	case type_rotor:
		ctype = QObject::tr("rotor");
		break;
	case type_generic_loss:
		ctype = QObject::tr("generic loss component");
		break;
	case type_intercooler:
		ctype = QObject::tr("intercooler");
		break;
	case type_sidestream:
		ctype = QObject::tr("side-stream");
		break;
	case type_extraction:
		ctype = QObject::tr("extraction");
		break;
	case type_ax_vaned:
		ctype = QObject::tr("axial vane");
		break;
	case type_ax_rotor:
		ctype = QObject::tr("axial rotor");
		break;
	case type_ax_stator:
		ctype = QObject::tr("axial stator");
		break;
	case type_ax_prestator:
		ctype = QObject::tr("axial pre-stator");
		break;
	case type_ac_rotor:
		ctype = QObject::tr("axial compressor rotor");
		break;
	case type_ac_stator:
		ctype = QObject::tr("axial compressor stator");
		break;
	case type_at_rotor:
		ctype = QObject::tr("axial turbine rotor");
		break;
	case type_at_stator:
		ctype = QObject::tr("axial turbine stator");
		break;
	case type_interstage:
		ctype = QObject::tr("inter-stage");
		break;
	case type_conical_diffuser:
		ctype = QObject::tr("conical diffuser");
		break;
	case type_inducer:
		ctype = QObject::tr("Inducer");
		break;
	default:
		ctype = QObject::tr("undefined");
	}
	return ctype;
}

bool T1dComponent::getStageParameter(double &rpm, double &phi, double &mu)
{
	T1dStage *stg = (T1dStage *)parent("T1dStage");
	if (stg)
	{
		rpm = stg->GetRPM();
		phi = stg->getFlowCoefficient();
		mu = stg->getMu();
		return true;
	}
	else
	{
		rpm = 0;
		phi = 0;
		mu = 0;
	}
	return false;
}

void T1dComponent::GetAvailableInputPropertyList(QStringList &SL, int iflag)
{
  GetAvailableInputPropertyListBase(SL, iflag);
#if 0
  QVector<property_t*> properties;
  GetConfigPassagePropertyList(properties, 1);
  foreach(property_t* p, properties)
  {
    SL += path() + "/" + p->oname();
  }

	QStringList sl;
	if (iflag == 0)
	{
		sl = QStringList()
			<< "mlc_total"
			<< "lc_offset"
			<< "lc_user"
			<< "mf_block"
			<< "delta_block"
			<< "block_user"
			<< "L_axial"
			<< "out/Ra"
			<< "out/b";
	}
	else
	{
		sl = QStringList()
			<< "L_axial"
			<< "out/Ra"
			<< "out/b"
			<< "out/phi";
	}
	QString path = this->path();
	forLoop(i, sl.size())
	{
		SL += path + "/" + sl[i];
	}
#endif
}

void T1dComponent::GetDataReductionPList_AxialThrust(QStringList &SL)
{

}

void T1dComponent::GetAvailableThrustParameterPropertyList(QStringList& L,int flag)
{

}

void T1dComponent::GetReportPropertyList(QVector<property_t*>& properties, int mach_type, bool includeGeom)
{
	in->GetReportPropertyList(properties, mach_type, includeGeom);
	out->GetReportPropertyList(properties, mach_type, includeGeom);

	QStringList L = QStringList() << "isStalled" << "textReport" << "LC" << "deltaEta";
	forLoop(i, L.size())
	{
		if (property_t* p = property(L[i]))
			properties.push_back(p);
	}
	return;
}

void T1dComponent::GetAvailableDataReductionPropertyList(QStringList &SL)
{
  if (haveBaseClass("T1dInlet"))
    return;

  if (haveBaseClass("T1dIGV") || haveBaseClass("T1dNozzle"))
    if (property("stagger_angle"))
      SL += path() + "/" + "stagger_angle";

	if (out)
	{
    QStringList sl = QStringList() << "p" << "p0";
    if (haveBaseClass("T1dImpeller") || haveBaseClass("T1dRotor") || haveBaseClass("T1dAxRotor") || haveBaseClass("T1dInducer"))
      sl << "t0";// << "t" << "C" << "Beta";

		QString path = out->path();
		forLoop(i, sl.size())
		{
			if (out->property(sl[i]))
				SL += path + "/" + sl[i];
		}
	}
}

void T1dComponent::GetGeometryModPropertyList(QStringList &sl)
{
	QStringList L = QStringList() << "in/Ra"
		<< "in/phi"
		<< "in/b"
		<< "in/Km"
		<< "in/Rh"
		<< "in/Rs"
		<< "in/Rh_Rs"
		<< "out/Ra"
		<< "out/phi"
		<< "out/b"
		<< "out/Km"
		<< "out/Rh"
		<< "out/Rs"
		<< "out/Rh_Rs"
		<< "L_axial"
		<< "RLE_hub"
		<< "RLE_shroud";

	QString dir = path();
	forLoop(i, L.size())
	{
		if (property(L[i]))
			sl += dir + "/" + L[i];
	}
}

void T1dComponent::GetConfigPropertyList(QVector<property_t*>& properties)
{
	QString tab = getTag("_tab").toString();
	int i = configTabList.indexOf(tab);
	switch (i)
	{
	case 0://Passage
		GetConfigPassagePropertyList(properties);
		break;
	case 1://Blade
		GetConfigBladePropertyList(properties);
		break;
	case 2://Design
		GetConfigDesignPropertyList(properties);
		break;
	case 3://Geometry
		GetConfigGeometryPropertyList(properties);
		break;
	case 4://Loss
		GetConfigLossPropertyList(properties);
		break;
	case 5://Deviation
		GetConfigDeviationPropertyList(properties);
		break;
	case 6://Blockage
		GetConfigBlockagePropertyList(properties);
		break;
	case 7://Range
		GetConfigRangePropertyList(properties);
		break;
	case 8://Vane
		GetConfigVanePropertyList(properties);
		break;
	default:
		break;
	}
}

void T1dComponent::GetPropertyList(QVector<property_t*>& properties, property_t* p, const QString name, int source/*0: config 1:optimization*/) const
{
  switch (source)
  {
  case 0:
    p->setDisplayName(name);
    if (!isFirstComp && !haveBaseClass("T1dImpeller"))
      SET_PROPERTY_READONLY(p);
    if (haveBaseClass("T1dAxVaned") || haveBaseClass("T1dAxVaneless"))//axial compressor
      SET_PROPERTY_WRITABLE(p);
    properties.push_back(p);
    break;
  case 1:
    if (!(!isFirstComp && !haveBaseClass("T1dImpeller")))
      properties.push_back(p);
    if (haveBaseClass("T1dAxVaned") || haveBaseClass("T1dAxVaneless"))//axial compressor
      properties.push_back(p);
    break;
  default:
    break;
  }
  
}

void T1dComponent::GetConfigPassagePropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
  auto checkAxVaneless = [&](bool &inputRatio)
  {
    if (T1dMachine* pMachine = T1dMachine::getCurrentMachine())
    {
      QString MachineType = pMachine->getMachineTypeString();
      bool isPump = (MachineType == "pump");
      bool isAx = (pMachine->AxRad == T1dMachine::exit_type_axial);
      if (isPump && isAx)
        inputRatio = false;
    }
  };
  
  switch (inletGeomOption)
	{
	case 0:
		if (property_t* p = in->property("Ra"))
		{
      GetPropertyList(properties, p,"Inlet radius (avg)", source);
		}
		if (property_t* p = in->property("b"))
		{
      GetPropertyList(properties, p, "Inlet width", source);
		}
		break;
	case 1:
		if (property_t* p = in->property("Rh"))
		{
      GetPropertyList(properties, p, "Inlet hub radius", source);
		}
		if (property_t* p = in->property("Rs"))
		{
      GetPropertyList(properties, p, "Inlet shroud radius", source);
		}
		break;
	default:
		break;
	}
	if (property_t* p = in->property("phi"))
	{
    GetPropertyList(properties, p, "Inlet inclination angle", source);
	}

	bool inputRatio = getTag("_bInputRatio").toBool();

  bool hasAxVan = this->haveBaseClass("T1dAxVaneless");
  if (hasAxVan)
    checkAxVaneless(inputRatio);

  if (source == 1)
    inputRatio = this->inputRatio;
	switch (outletGeomOption)
	{
	case 0:
		if (!inputRatio)
		{
			if (property_t* p = out->property("Ra"))
			{
				p->setDisplayName("Outlet radius (avg)");
        if (IF_PROPERTY_READONLY(p))
          SET_PROPERTY_WRITABLE(p);
				properties.push_back(p);
			}
			if (property_t* p = out->property("b"))
			{
				p->setDisplayName("Outlet width");
        if (IF_PROPERTY_READONLY(p))
          SET_PROPERTY_WRITABLE(p);
        properties.push_back(p);
			}
		}
		break;
	case 1:
		if (!inputRatio)
		{
			if (property_t* p = out->property("Rh"))
			{
				p->setDisplayName("Outlet hub radius");
        if (IF_PROPERTY_READONLY(p))
          SET_PROPERTY_WRITABLE(p);
        properties.push_back(p);
			}
			if (property_t* p = out->property("Rs"))
			{
				p->setDisplayName("Outlet shroud radius");
        if (IF_PROPERTY_READONLY(p))
          SET_PROPERTY_WRITABLE(p);
        properties.push_back(p);
			}
		}
		break;
	default:
		break;
	}

  if (property_t* p = out->property("phi"))
  {
    p->setDisplayName("Outlet inclination angle");
    if (IF_PROPERTY_READONLY(p))
      SET_PROPERTY_WRITABLE(p);
    properties.push_back(p);
  }

	if (!(haveBaseClass("T1dBend90") || haveBaseClass("T1dBend180")))
	{
		if (property_t* p = property("L_axial"))
		{
			properties.push_back(p);
		}
	}
}

void T1dComponent::GetConfigBladePropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::GetConfigDesignPropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
}

void T1dComponent::GetConfigGeometryPropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
}

void T1dComponent::GetConfigLossPropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::GetConfigDeviationPropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::GetConfigBlockagePropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::GetConfigRangePropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::GetConfigVanePropertyList(QVector<property_t*>& properties)
{
}

void T1dComponent::initFromGeomBase(QList<T3DInterfaceData*> data)
{
	if (data.size() == 0)
		return;
	T3DInterfaceData* p3D = data[0];
	in->Rh = p3D->R_in_hub;
	out->Rh = p3D->R_out_hub;
	in->Rs = p3D->R_in_shroud;
	out->Rs = p3D->R_out_shroud;
	in->Zh = p3D->Z_in_hub;
	in->Zs = p3D->Z_in_shroud;
  in->Za = 0.5*(in->Zh + in->Zs);
	out->Zh = p3D->Z_out_hub;
	out->Zs = p3D->Z_out_shroud;
  out->Za = 0.5*(out->Zh + out->Zs);
	in->Ra = p3D->Ra_in;
	in->b = p3D->b_in;
	in->phi = p3D->phi_in;
	if (T1dMachine::getMachineType() == T1dMachine::MachType_Turbine && in->phi > 0)
	{
		in->phi = (-1)*in->phi;
	}

  double small_d = 1.0e-6;
  if (fabs(in->phi - 0.5*PI) < small_d || fabs(in->phi + 0.5*PI) < small_d)
    inletGeomOption = 0; //require Ra, b, phi input
  else
    inletGeomOption = 1;
  out->Ra = p3D->Ra_out;
  out->b = p3D->b_out;
  out->phi = p3D->phi_out;
  if (fabs(out->phi - 0.5*PI) < small_d || fabs(out->phi + 0.5*PI) < small_d)
    outletGeomOption = 0; //require Ra, b, phi input
  else
    outletGeomOption = 1;
	L_axial = p3D->axialLength;
}

int T1dComponent::getMachineType()
{
  int machineType = T1dMachine::MachType_Compressor;
  if (T1dMachine* pMachine = dynamic_cast<T1dMachine*>(parent("T1dMachine")))
  {
    machineType = pMachine->machineType;
  }
	return machineType;
}

int T1dComponent::getMachine_exitType()
{
  // to get property "exit type" of Machine
  int exit_type = 0;
  if (T1dMachine* pMachine = dynamic_cast<T1dMachine*>(parent("T1dMachine")))
  {
    exit_type = pMachine->AxRad;
  }
  return exit_type;
}

int T1dComponent::getMachineAppType()
{
	int machineAppType = 0;
	if (T1dMachine* pMachine = dynamic_cast<T1dMachine*>(parent("T1dMachine")))
	{
		machineAppType = pMachine->AppType;
	}
	return machineAppType;
}

void T1dComponent::execMethod(QString command)
{
	if (command.size())
	{
		TObject *pStage = 0;
		TObject * machine = 0;
		pStage = this->parent("T1dStage");
		if (pStage)
			if (pStage->haveBaseClass("T1dStage"))
			{
				machine = pStage->parent("T1dMachine");
				if (command == "removeStage")
				{
					TObject * p = pStage->parent();
					QList<TObject*> L = p->allChild("T1dStage");
					if (L.size() > 1)
						pStage->execute(command);
				}
				else
					pStage->execute(command);
				if (machine)
				{
					((T1dMachine*)machine)->UpdateGeometry();
					((T1dMachine*)machine)->updateGUI(true);
					machine->update_link_list_from_path(true);
				}
				TModule * m = CURRENT_MODULE;
				if (!m)
					return;
				if (!m->defaultTaskWindow)
					return;
				m->defaultTaskWindow->redraw();
			}
	}
}

void T1dComponent::insertComponent(int position)
{
	TObject * m = this->parent("T1dMachine");
  int mType = 0, localAxRad = 0;
	if (m->property("AxRad"))
    localAxRad = m->property("AxRad")->toInt();
	if (m->property("machineType"))
		mType = m->property("machineType")->toInt();
  	
	bool isFan = (mType == T1dMachine::MachType_Fan);	
	bool isPump = (mType == T1dMachine::MachType_Pump);

	if (T1dStage * pStage = dynamic_cast<T1dStage *>(this->parent("T1dStage")))
	{
		T1dComponent *inserted = 0;
    bool isLoadFromDB = false;
    TObject * oo = this;
    if (localAxRad != pStage->stageAxRad)
      localAxRad = pStage->stageAxRad;    
    bool isAxial = (localAxRad == T1dMachine::exit_type_axial);
    bool isRadial= (localAxRad == T1dMachine::exit_type_radial);
    bool isMixed = (localAxRad == T1dMachine::exit_type_mixed);
    
    if ((isFan && (isMixed || isAxial)) || (isPump && isAxial))
    {
      w_PropertyHolderDialog dlg(core_Application::core());
      dlg.setWindowTitle(QObject::tr("Add Component"));
      dlg.addText("<br>");
      dlg.addText(QObject::tr("Stage Component"));
      dlg.addText("<hr>");
      int compType = 0;
      QStringList LL = QStringList() << QObject::tr("Gap");
      if (pStage)
      {
        QString name = this->dname();
        if (position == 0)
        {
          for (int i = 0; i < pStage->compList.size(); i++)
            if (pStage->compList[i]->dname() == name && i == 1)
            {
              if (!(this->haveBaseClass("T1dAxRotor") || this->haveBaseClass("T1dAxStator")))
              {
                LL << QObject::tr("PreStator");
              }
            }
        }
        if (position == 1)
        {
          if (this->haveBaseClass("T1dAxRotor"))
          {
            LL << QObject::tr("Exit Diffuser");
          }
          else if (this->haveBaseClass("T1dAxPreStator"))
          {
            ;
          }
          else if (this->haveBaseClass("T1dAxVaneless"))
          {
            LL << QObject::tr("Stator") << QObject::tr("Exit Diffuser");
          }
          else if (this->haveBaseClass("T1dAxStator"))
          {
            LL << QObject::tr("Exit Diffuser");
          }

        }
      }
      if (w_Property* wp = dlg.addProperty(&compType, QObject::tr("Component Type"), &LL)) {
        wp->hideLabel();
        wp->setAutoSave();
      }
      bool bLoad = false;
      dlg.addProperty(&bLoad, QObject::tr("Load From Database"));
      QString TComponent = "";
      QString TName = "";
      if (dlg.exec() == w_QDialog::Accepted)
      {
        switch (compType)
        {
        case 0:
          TName = "Gap";
          TComponent = "T1dAxVaneless";
          break;
        case 1://Stator
          if (position == 0)
          {
            TName = "Prestator";
            TComponent = "T1dAxPreStator";
          }
          else
          {
            if (this->haveBaseClass("T1dAxStator") || this->haveBaseClass("T1dAxRotor"))
            {
              TName = "Exit Diffuser";
              TComponent = "T1dConicalDiffuser";
            }
            else
            {
              TName = "stator";
              TComponent = "T1dAxStator";
            }
          }
          break;
        case 2:
          TName = "Exit Diffuser";
          TComponent = "T1dConicalDiffuser";
          break;
        default:
          break;
        }
        if (TComponent.size())
        {
          while (pStage && pStage->object(TName.ascii()))
          {
            TName = util_OUtil::increaseDigitRight(TName);
          }
          inserted = (T1dComponent *)TObject::new_object(TComponent.ascii(), TName, 0);
          if (bLoad && inserted)
          {
            TObject *o = VARIANT2OBJECT(inserted->execute("ImportFromDatabase"));
            if (o)
              isLoadFromDB = true;
            else
              return;
          }
        }
      }
    }
		else if (isAxial)//for axial machine
		{
			w_PropertyHolderDialog dlg(core_Application::core());
			dlg.setWindowTitle(QObject::tr("Add Component"));
			dlg.addText("<br>");
			dlg.addText(QObject::tr("Stage Component"));
			dlg.addText("<hr>");
			int compType = 0;
      QStringList LL = QStringList() << QObject::tr("Rotor") << QObject::tr("Stator") << QObject::tr("Gap");
			bool addIGV = false;
			bool addExitDiffuser = false;
			if (pStage)
			{
				QString name = this->dname();
				if (position == 0)
				{
					for (int i = 0; i < pStage->compList.size(); i++)
						if (pStage->compList[i]->dname() == name && i == 1)
						{

							addIGV = true;
							if ((mType == 3 || mType == 4) && this->haveBaseClass("T1dAxRotor"))
							{
								LL << QObject::tr("PreStator");
							}
							else
							{
								LL << QObject::tr("IGV");
							}
						}
				}
				else if (position == 1)
				{
					int ncomp = pStage->compList.size();
					if (pStage->compList[ncomp - 1]->dname() == name)
					{
						LL << QObject::tr("Exit Diffuser");
						addExitDiffuser = true;
					}
				}
			}
			if (w_Property* wp = dlg.addProperty(&compType, QObject::tr("Component Type"), &LL)) {
				wp->hideLabel();
				wp->setAutoSave();
			}
			bool bLoad = false;
			dlg.addProperty(&bLoad, QObject::tr("Load From Database"));
			QString TComponent = "";
			QString TName = "";
			if (dlg.exec() == w_QDialog::Accepted)
			{
				switch (compType)
				{
				case 0://Rotor
					TName = "rotor";
					if (mType == 1) //compressor machine
						TComponent = "T1dAxCRotor";
					else if (mType == 2) //turbine machine
						TComponent = "T1dAxTRotor";
					else
						TComponent = "T1dAxRotor";
					break;
				case 1://Stator
					TName = "stator";
					if (mType == 1) //compressor machine
						TComponent = "T1dAxCStator";
					else if (mType == 2) //turbine machine
						TComponent = "T1dAxTStator";
					else
						TComponent = "T1dAxStator";
					break;
				case 2:
					TName = "Gap";
					TComponent = "T1dAxVaneless";
					break;
				case 3:
					if (addIGV)
					{
						if ((mType == 3 || mType == 4) && this->haveBaseClass("T1dAxRotor"))
						{
							TName = "Prestator";
							TComponent = "T1dAxPreStator";
						}
						else
						{
							TName = "IGV";
							TComponent = "T1dAxIGV";
						}
					}
					else if (addExitDiffuser)
					{
						TName = "Exit Diffuser";
						TComponent = "T1dConicalDiffuser";
					}
					break;
				default:
					break;
				}
				if (TComponent.size())
				{
					while (pStage && pStage->object(TName.ascii()))
					{
						TName = util_OUtil::increaseDigitRight(TName);
					}
					inserted = (T1dComponent *)TObject::new_object(TComponent.ascii(), TName, 0);
					if (bLoad && inserted)
					{
						TObject *o = VARIANT2OBJECT(inserted->execute("ImportFromDatabase"));
            if (o)
              isLoadFromDB = true;
            else
              return;
					}
				}
			}
		}
		else if (isRadial || isMixed)//for radial machine
		{    
			if (mType != 2) {//not turbine machine
				if (parent())
					if (parent()->haveBaseClass("T1dMultiSegComp") || parent()->haveBaseClass("T1dImpeller") || parent()->haveBaseClass("T1dRotor"))
						oo = parent();
					else {
						if (parent("T1dMultiSegComp") && (parent("T1dMultiSegComp")->haveBaseClass("T1dAirfoilDiffuser") || parent("T1dMultiSegComp")->haveBaseClass("T1dWedgeDiffuser") || parent("T1dMultiSegComp")->haveBaseClass("T1dVanedDiffuser")))
							oo = parent("T1dMultiSegComp");
					}
				T1dStageAddCompDlg dialog(oo, position);
				dialog.setObject(pStage);
				if (dialog.exec() == w_QDialog::Accepted) {
					inserted = dialog.getComponent();
          if (inserted)
            isLoadFromDB = dialog.isLoadFromDB;
          else
						return;
				}
				else {
					return;
				}

			}
			else {//radial turbine machine
				QString className = cName();
				if (pStage) {
					if (className == "T1dVaneless" && position == 0) {//we need to judge if it is the first component after T1dInlet
						if (!(pStage->compList.indexOf(this) >= 1 && pStage->compList.at(pStage->compList.indexOf(this) - 1)->haveBaseClass("T1dInlet"))) {
							printError("No component can be added before it.");
							return;
						}
					}
					else
						if (className == "T1dRotor" && position == 1) {
							if (!(pStage->compList.last() == this)) {//we need to judge if it is the last componet of stage
								printError("No component can be addeded after it.");
								return;
							}
						}
				}


				w_PropertyHolderDialog dlg(core_Application::core());
				dlg.setWindowTitle(QObject::tr("Add Component"));
				dlg.addText("<br>");
				dlg.addText(QObject::tr("Stage Component"));
				dlg.addText("<hr>");
				int compType = 0;
				QStringList insertBeforeRules = QStringList()
					<< "T1dVoluteInlet" << ""
					<< "T1dVaneless" << QObject::tr("inlet volute")
					<< "T1dVaneless" << QObject::tr("inlet twin volute")
					<< "T1dVaneless" << QObject::tr("nozzle")
					<< "T1dNozzle" << QObject::tr("gap1")
					<< "T1dRotor" << QObject::tr("gap2")
					<< "T1dConicalDiffuser" << "";

				QStringList insertAfterRules = QStringList()
					<< "T1dVoluteInlet" << QObject::tr("gap1")
					<< "T1dVoluteTwinInlet" << QObject::tr("gap1")
					<< "T1dVaneless" << QObject::tr("nozzle")
					<< "T1dNozzle" << QObject::tr("gap1")
					<< "T1dRotor" << QObject::tr("conical diffuser")
					<< "T1dRotor" << QObject::tr("exit bend")
					<< "T1dExitBend" << QObject::tr("deswirl")
					<< "T1dExitBend" << QObject::tr("cross over")
					<< "T1dDeswirl" << QObject::tr("cross over")
					<< "T1dConicalDiffuser" << ""
					<< "T1dBend180" << "";

				QStringList nameMapRules = QStringList()
					<< QObject::tr("gap2") << "T1dVaneless"
					<< QObject::tr("inlet volute") << "T1dVoluteInlet"
					<< QObject::tr("inlet twin volute") << "T1dVoluteTwinInlet"
					<< QObject::tr("nozzle") << "T1dNozzle"
					<< QObject::tr("conical diffuser") << "T1dConicalDiffuser"
					<< QObject::tr("exit bend") << "T1dExitBend"
					<< QObject::tr("deswirl") << "T1dDeswirl"
					<< QObject::tr("cross over") << "T1dBend180"
					<< QObject::tr("gap1") << "T1dVaneless";

				QStringList LL;
				QStringList insertRules;
				if (position == 0)//insert before
				{
					insertRules = insertBeforeRules;
					//when insert component before, we set the after components parameter "isFirstComp" to false
					QList<T1dComponent*> complist;
					T1dStage * stage = dynamic_cast<T1dStage*>(parent());
					complist = stage->compList;
					for (int i = 1; i < complist.size(); i++)
					{
						complist[i]->isFirstComp = false;
					}
				}
				else //insert after
					insertRules = insertAfterRules;
				forLoop(i, insertRules.size())
				{
					if (i % 2 == 0 && insertRules[i] == className && i + 1 < insertRules.size() && insertRules[i + 1].size())
					{
						LL << insertRules[i + 1];
					}
				}
				/*if (insertRules.indexOf(className) >= 0)
					if (insertRules.size() > insertRules.indexOf(className) + 1 && insertRules[insertRules.indexOf(className) + 1].size())
						LL << insertRules[insertRules.indexOf(className) + 1];*/
				if (w_Property* wp = dlg.addProperty(&compType, QObject::tr("Component Type"), &LL)) {
					wp->hideLabel();
					wp->setAutoSave();
				}

				QString TName = "";
				if (dlg.exec() == w_QDialog::Accepted)
				{
					if (compType < LL.size() && nameMapRules.indexOf(LL[compType]) >= 0 && nameMapRules.indexOf(LL[compType]) + 1 < nameMapRules.size()) {
						TName = nameMapRules[nameMapRules.indexOf(LL[compType]) + 1];
						if (TName.size()) {
							QString objName = TObject::shortName(TName.ascii());
							QString dispName = LL[compType];
							if (pStage && pStage->object(objName)) {//insure object name is unique with the same type
								forLoop(i, 1000) {//suppose the number of objects with the same class will not beyond 1000
									QString newObjName = objName + QString("%1").arg(i + 1);
									QString newDispName = dispName + QString("%1").arg(i + 1);
									if (!(pStage->object(newObjName))) {
										objName = newObjName;
										dispName = newDispName;
										break;
									}
								}
							}
							if (inserted = (T1dComponent *)TObject::new_object(TName.ascii(), objName, 0))
								inserted->set_display_name(dispName);
						}

					}
				}
			}
		}
		if (inserted && pStage)
		{
			switch (position)
			{
			case 0://insert before
        pStage->InsertComponentBefore((T1dComponent*)oo, inserted, isLoadFromDB);
        inserted->isFirstComp = true;//add by Jian
        break;
      case 1://insert after
        pStage->InsertComponentAfter((T1dComponent*)oo, inserted, isLoadFromDB);
				break;
			default:
				break;
			}
			pStage->UpdateGeometry();
			if (inserted && inserted->haveBaseClass("T1dVoluteInlet")) {
				T1dVoluteInlet * voluteInlet = (T1dVoluteInlet*)inserted;
				voluteInlet->SetGeometry();
			}
			if (inserted && inserted->haveBaseClass("T1dVoluteTwinInlet")) {
				T1dVoluteTwinInlet * voluteTwinInlet = (T1dVoluteTwinInlet*)inserted;
				voluteTwinInlet->SetGeometry(0);
			}
			pStage->updateGUI(true);
			pStage->update_link_list_from_path(true);
#if REFACTOR_UPDATEGUI //for refactor of updateGUI:Take out the create function from it
			//create plot curve and attach the curve to plot for the newly inserted component
			O_MODULE(inserted)->createViewFromReadObject(inserted);
			if (T1dMachine* machine = dynamic_cast<T1dMachine*>(pStage->parent("T1dMachine")))
			{
				machine->updatePlot();//clear and replot
			}
			/*if (TTaskWindow* win = static_cast<TTaskWindow*>(inserted->getWindow()))
			{
				win->refresh();
			}*/
#else
			TModule* m = CURRENT_MODULE;
			if (m && m->defaultTaskWindow)
				m->defaultTaskWindow->redraw();
			inserted->sanityCheck();
#endif
			
		}
	}
}

static bool scalingPropertyChangedCallback(w_Property* w, w_PropertyHolderDialog* dlg)
{
	QStringList L = QStringList()
		<< QObject::tr("Minimum thickness") << QObject::tr("Scaling thickness?")
		<< QObject::tr("Minimum clearance") << QObject::tr("Scaling clearance?");

	return w_PropertyHolderDialog::propertyShowDepend(w, dlg, L);
}

O_METHOD_BEGIN(T1dComponent, InsertComponentBefore, "Insert Component Before", 0, FLG_METHOD_SKIP_GUI_ARG, "insert a component before the current object")
{
	if (VARLID_ARG)
	{
		if (object->isPartofMultSeg())
		{
			TObject * o = object->parent();
			if (o)
			{
				o->execute("InsertComponentBefore");
			}
		}
		else
			object->insertComponent(0);
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, InsertComponentAfter, "Insert Component After", "Insert Component After", FLG_METHOD_SKIP_GUI_ARG, "insert a component after the current object")
{
	if (VARLID_ARG)
	{
		if (object->isPartofMultSeg())
		{
			TObject * o = object->parent();
			if (o)
			{
				o->execute("InsertComponentAfter");
			}
		}
		else
			object->insertComponent(1);
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, ReplaceComponent, "Replace Component", "Replace Component", FLG_METHOD_SKIP_GUI_ARG, "Replace with other component")
{
	if (VARLID_ARG)
	{
		T1dStage *pStage = (T1dStage *)object->parent("T1dStage");
#if 0
		TObject *pParent = object->parent();
		if (object->isPartofMultSeg())
		{
			pStage = (T1dStage *)pParent->parent();
		}
		else
		{
			pStage = (T1dStage *)pParent;
		}
#endif

		QStringList diffusers, exitElements;
		diffusers << "T1dVaneless"
			<< "T1dVaned"
			<< "T1dVanedDiffuser"
			<< "T1dAirfoilDiffuser"
			<< "T1dWedgeDiffuser";

		exitElements << "T1dVolute"
			<< "T1dReturnChannel";

		QStringList diffusersMap, exitMap;
		diffusersMap << "Vaneless"
			<< "Vaned"
			<< "Vaned Diffuser"
			<< "Airfoil Diffuser"
			<< "Wedge Diffuser";
		exitMap << "Volute"
			<< "Return Channel";

		QString baseClass = object->cName();
		if (diffusers.indexOf(baseClass) >= 0 || exitElements.indexOf(baseClass) >= 0)
		{
			QStringList availList, availClass;
			int selected;
			if (diffusers.indexOf(baseClass) >= 0)
			{
				forLoop(i, diffusersMap.size())
				{
					if (i != diffusers.indexOf(baseClass))
					{
						availList << diffusersMap[i];
						availClass << diffusers[i];
					}
				}
			}
			else
			{
				forLoop(i, exitMap.size())
				{
					if (i != exitElements.indexOf(baseClass))
					{
						availList << exitMap[i];
						availClass << exitElements[i];
					}

				}
			}
			w_ComponentSelection dlg(availList);
			if (dlg.exec() == QDialog::Accepted)
			{
				selected = dlg.getSelected();
				QString objName = TObject::shortName(availClass[selected].ascii());
				T1dComponent * newObj = (T1dComponent *)TObject::new_object(availClass[selected].ascii(), objName, pStage);
				if (diffusers.indexOf(availClass[selected]) >= 0)
				{
					TObject::copyObject(newObj->in, object->in);
					TObject::copyObject(newObj->out, object->out);
				}
				if (pStage)
				{
					if (pStage->ReplaceComponent(object, newObj))
					{
						pStage->UpdateGeometry();
						pStage->updateGUI(true);
					}
				}
			}
		}
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, RemoveComponent, "Remove Component", 0, FLG_METHOD_SKIP_GUI_ARG, "remove current object")
{
	if (VARLID_ARG)
	{
		if (object->haveBaseClass("T1dImpeller") || object->haveBaseClass("T1dRotor")
			|| object->haveBaseClass("T1dInlet") || object->haveBaseClass("T1dRearLeakPath") || object->haveBaseClass("T1dInlet")
			|| object->haveBaseClass("T1dToNextStage") || object->haveBaseClass("T1dAxRotor")) {
			QString s = QObject::tr("The component can not be removed.");
			printError("%s", qPrintable(s));
		}
		else {
			T1dStage *pStage = dynamic_cast<T1dStage *>(object->parent("T1dStage"));
			TObject *pParent = object->parent();

			TTaskWindow *tWindow = 0;
			tWindow = static_cast<TTaskWindow*>(object->getWindow());

			if (object->isPartofMultSeg())
			{
				T1dComponent *pMultiSeg = (T1dComponent *)pParent;

				if ((pMultiSeg->parent())->cName() == "T1dPositiveNegativeVane")
					pMultiSeg = dynamic_cast<T1dComponent *>(pParent->parent());

        if (pMultiSeg->haveBaseClass("T1dInletChamber"))
          pStage->inlet->drawInlet = true;

        if (pMultiSeg->haveBaseClass("T1dIGV"))
          pStage->igvOption = 0;
				pStage->RemoveComponent(pMultiSeg);
			}
			else
			{
				pStage->RemoveComponent(object);
			}
			//pStage->UpdateCompList();
			pStage->updateGUI(true);
#if REFACTOR_UPDATEGUI //for refactor of updateGUI:Take out the create function from it
			if (T1dMachine* machine = dynamic_cast<T1dMachine*>(pStage->parent("T1dMachine")))
			{
				machine->updatePlot();//update plot
			}
#else
			if (tWindow)
				tWindow->redraw();
#endif
		}
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, config, "Config", ":/images/set.png", FLG_METHOD_SKIP_GUI_ARG, "config component property")
{
	if (VARLID_ARG)
	{
		TObject *p = object->parent();
		if (p)
		{
			p->highlight();
		}
		T1dComponent *o = (T1dComponent *)object;
		if (p)
		{
			if (p->haveBaseClass("T1dMultiSegComp"))
			{
				o = (T1dComponent *)p;
			}
      else if (p->haveBaseClass("T1dInletChamber"))
        o = (T1dComponent *)p;
		}
		//T1dComponent *o = (T1dComponent *)object;
		T1dComponentDlg* dlg = new T1dComponentDlg(o);

		dlg->exec();
		object->highlight();

		if (dlg)
			delete dlg;
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, CloneStage, "Clone Stage", ":/images/fea-copy.png", FLG_METHOD_SKIP_GUI_ARG, "Clone stage")
{
	if (VARLID_ARG)
	{
		object->execMethod("cloneStage");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, RemoveStage, "Remove Stage", ":/images/delete.png", FLG_METHOD_SKIP_GUI_ARG, "Remove stage")
{
	if (VARLID_ARG)
	{
		object->execMethod("removeStage");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, AddStage, "Add Stage", ":/images/add-stage.png", FLG_METHOD_SKIP_GUI_ARG, "Add stage")
{
	if (VARLID_ARG)
	{
		const char *sname = TObject::shortName("T1dObject");
		T1dObject *oned = (T1dObject *)CURRENT_STUDY->object(sname);
		if (oned)
			if (oned->firstChild("T1dMachine"))
				oned->firstChild("T1dMachine")->execute("addNewStage");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, StageScaling, "Stage Scaling", ":/images/map-extension.png", FLG_METHOD_SKIP_GUI_ARG, "Stage Scaling")
{
	if (VARLID_ARG)
	{
		object->execMethod("scaling");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, Scaling, "Scaling", ":/images/map-extension.png", FLG_METHOD_SKIP_GUI_ARG, "Scaling")
{
	if (VARLID_ARG)
	{
		w_PropertyHolderDialog dlg(core_Application::core());
    dlg.setWindowTitle(QObject::tr("Scaling ") + TTRO(object));
		dlg.setCallback(scalingPropertyChangedCallback);

		double sf = 1.0;
		bool scale_thick = true;
		double min_thick = 0.0001; // m
		bool scale_clr = true;
		double min_clr = 0.0001; //m
		dlg.addProperty(&sf, QObject::tr("Scaling factor"));

		dlg.addProperty(&scale_thick, QObject::tr("Scaling thickness?"));
		dlg.addProperty(&min_thick, QObject::tr("Minimum thickness"), TUnit::length);

		dlg.addProperty(&scale_clr, QObject::tr("Scaling clearance?"));
		dlg.addProperty(&min_clr, QObject::tr("Minimum clearance"), TUnit::length);

		if (dlg.exec() == w_QDialog::Accepted)
		{
			if (sf <= 0) {
				printError(qPrintable(QObject::tr("Scaling factor must greater than zero.")));
				return 0;
			}

			object->scaling(sf, scale_thick, min_thick, scale_clr, min_clr);
#if REFACTOR_UPDATEGUI //for refactor of updateGUI:Take out the create function from it
			if (T1dMachine* machine = (T1dMachine*)object->parent("T1dMachine"))
			{
				machine->UpdateGeometry();
				machine->updatePlot();
			}
#else
			if (T1dMachine* machine = dynamic_cast<T1dMachine*>(object->parent("T1dMachine")))
				machine->UpdateView();
			TTaskWindow* tWindow = 0;
			TModule* module = CURRENT_MODULE;
			tWindow = static_cast<TTaskWindow*>(module->defaultTaskWindow);

			if (tWindow) {
				tWindow->redraw();
				tWindow->zoomAll();
			}
#endif
		}

		return 0;
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, Trimming, "Trimming", ":/images/geom_Trimming.png", FLG_METHOD_SKIP_GUI_ARG, "Trimming")
{
	if (VARLID_ARG)
	{
		object->execMethod("trimming");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, FlowCut, "Flow Cut", ":/images/geom_Flow_cut.png", FLG_METHOD_SKIP_GUI_ARG, "Flow Cut")
{
	if (VARLID_ARG)
	{
		object->execMethod("flowCut");
	}
}
O_METHOD_END;

O_METHOD_BEGIN(T1dComponent, InscribedCircle, "Inscribed Circle", 0, FLG_METHOD_SKIP_GUI_ARG, "Inscribed Circle")
{
	if (VARLID_ARG)
	{
		// 1.
		int error = object->updateInscribedCircleCalculator();
		if (error != 0)
			return error;
		// 2.
		if (!object->getInscribedCircleCalculator(false))
			return -1;
		// 3.
		w_QDialog dlg(core_Application::core());
		dlg.setWindowTitle("Inscribed Circle");
		QVBoxLayout* grid = new QVBoxLayout(&dlg);
		InscribedCircleCalculatorWidget* _InscribedCircleCalculatorWidget = new InscribedCircleCalculatorWidget();
		grid->addWidget(_InscribedCircleCalculatorWidget);
		// 4.
		_InscribedCircleCalculatorWidget->setInscribedCircleCalculator(object->getInscribedCircleCalculator());

		if (dlg.exec() == w_QDialog::Accepted)
		{

		}
		delete _InscribedCircleCalculatorWidget;
	}
}
O_METHOD_END;

InscribedCircleCalculator* T1dComponent::getInscribedCircleCalculator(bool CreatedAsNeeded)
{
	QString name = "InscribedCircleCalculator";
	InscribedCircleCalculator* _InscribedCircleCalculator = dynamic_cast<InscribedCircleCalculator*>(child(name));

	if (!_InscribedCircleCalculator && CreatedAsNeeded)
	{
		_InscribedCircleCalculator = dynamic_cast<InscribedCircleCalculator*>(TObject::new_object("InscribedCircleCalculator", name, this));
	}
	if (_InscribedCircleCalculator)
	{
		SET_OBJ_HIDE(_InscribedCircleCalculator);
		return _InscribedCircleCalculator;
	}
	return nullptr;
}

int T1dComponent::setInscribedCircleCalculator()
{
	// pHubContour, pShroudContour
	int error = -1;

	// 1.
	QVector<TNurbsCurve*> TNCurves;
	// 2.
	if (pHubContour1)
		TNCurves.push_back(pHubContour1);
	if (pShroudContour)
		TNCurves.push_back(pShroudContour);
	else
		return error;
	// 3.
	InscribedCircleCalculator* InscribedCircleCalculator = getInscribedCircleCalculator();
	if (!InscribedCircleCalculator)
		return error;
	// 4.
	error = InscribedCircleCalculator->setTNCurve(TNCurves);

	return error;
}

int T1dComponent::updateInscribedCircleCalculator()
{
	int error = -1;
	// 1.
	error = setInscribedCircleCalculator();
	if (error != 0)
		return error;
	// 2.
	InscribedCircleCalculator* InscribedCircleCalculator = getInscribedCircleCalculator();
	// 3.
	error = InscribedCircleCalculator->createInscribedCircle();

	return error;
}

O_METHOD_BEGIN(T1dComponent, SetStageRunCondition, "Set Stage Run Condition", ":/images/settings.png", FLG_METHOD_SKIP_GUI_ARG, "Set Stage Run Condition")
{
	if (VARLID_ARG)
	{
		object->execMethod("SetStageRunCondition");
	}
}
O_METHOD_END;

static QVector<w_Property*> subcomponentWidgets;
static QMap<QString, QVector<w_Property*>> subcomponentWidgetMap;
static bool importComponentPropertyChangedCallback(w_Property* w, w_PropertyHolderDialog* dlg)
{
	if (w_Property* wholeStage = dlg->findProperty("Add whole stage?"))
	{
		foreach(w_Property* w, subcomponentWidgets)
		{
			w->hideAll(true);
		}

		if (!wholeStage->getValue().toBool())
		{
			if (w_Property* stageChoice = dlg->findProperty("Stages"))
			{
				QString s = stageChoice->getValue().toString();

				for (int i = 0; i < subcomponentWidgetMap[s].size(); i++)
					subcomponentWidgetMap[s][i]->hideAll(false);
			}
		}

		if (w_Property* w = dlg->findProperty("Operation"))
		{
			w->hideAll(!wholeStage->getValue().toBool());
		}

		if (w_Property* w = dlg->findProperty("Cover the available component as needed?"))
		{
			w->hideAll(wholeStage->getValue().toBool());
		}
	}

	dlg->resize(dlg->sizeHint());

	return false;
}

static QString getComponentChoice(QStringList L, bool& cover, QString& operations)
{
	QString choice;

	QStringList all;
	QStringList stages;
	QMap<QString, QStringList> stageComponents;

	foreach(QString s, L)
	{

		QStringList LL = s.split("/");
		if (LL.size() > 0)
		{
			if (all.indexOf(LL.first()) < 0)
			{
				all.push_back(LL.first());
				stages.push_back(LL.first());
			}

			if (stageComponents[LL.first()].indexOf(s) < 0)
				stageComponents[LL.first()].push_back(s);
		}

		if (all.indexOf(s) < 0)
			all.push_back(s);
	}

	if (all.size())
	{
		w_PropertyHolderDialog dlg(core_Application::core());
		dlg.setWindowTitle(QObject::tr("Choose Components to Import"));

		QString stageChoice = stages.first();
		dlg.addText(QObject::tr("Stage choice"), false, false, true);
		dlg.addProperty(&stageChoice, QObject::tr("Stages"), &stages);


		dlg.addSpace();

		QVector<QString> subChoices(stages.size());
		subcomponentWidgets.clear();
		subcomponentWidgetMap.clear();
		for (int i = 0; i < stages.size(); i++)
		{
			subChoices[i] = stageComponents[stages[i]].first();
      QString s = QObject::tr("All available components of") + QString(" %1").arg(stages[i]);
			QLabel* label = dlg.addText(s, false, false, true);
			w_Property* wp = dlg.addProperty(&subChoices[i], "Components", &stageComponents[stages[i]]);
			wp->addRelatedWidgets(label);
			subcomponentWidgets.push_back(wp);
			subcomponentWidgetMap[stages[i]].push_back(wp);
		}

		dlg.addSpace();
		dlg.addText(QObject::tr("Options"), false, false, true);

		bool wholeStage = true;
		dlg.addProperty(&wholeStage, QObject::tr("Add whole stage?"));

		//cover = true;
		dlg.addProperty(&cover, QObject::tr("Cover the available component as needed?"));

		//append = true;
		//dlg.addProperty(&append, "Append stage?");
		static QStringList operationsL = QStringList() << QObject::tr("Append") << QObject::tr("Prepend") << QObject::tr("Cover");
		if (!operations.size())
			operations = operationsL.first();
		dlg.addProperty(&operations, "Operation", &operationsL);


		//dlg.addSpace();

		dlg.setCallback(importComponentPropertyChangedCallback);
		importComponentPropertyChangedCallback(0, &dlg);

		if (dlg.exec() == QDialog::Accepted)
		{
			if (wholeStage)
			{
				choice = stageChoice;
			}
			else
			{
				int index = stages.indexOf(stageChoice);
				choice = subChoices[index];
				operations = operationsL.last();
			}
		}
	}

	subcomponentWidgets.clear();
	subcomponentWidgetMap.clear();
	return choice;
}

static void copyStageObject(TObject* from, TObject* to, QString operations)
{
	if (TObject* fromParent = from->parent())
	{
		if (TObject* fromStudy = from->parent("StudyObject"))
		{
			QString newName = from->oname();
			QString newDisplayName = from->dname();
			while (fromParent->object(newName.ascii()))
			{
				newName = util_OUtil::increaseDigitRight(newName);
				newDisplayName = newName;
			}

			// update  all the relations from study
			if (TObject *newObject = TObject::dupObject(to, newName.ascii(), 0, 0, true))
			{
				// insert
				if (operations == "Prepend")
					fromParent->insertChildBefore(from, newObject);
				else
					fromParent->insertChildAfter(from, newObject);

				// clean old
				if (operations == "Cover")
				{
					newName = from->oname();
					newDisplayName = from->dname();
					eprintf("delete %s", qPrintable(from->path()));
					TObject::delete_object(from);
					// name
					newObject->set_object_name(newName);
					newObject->set_display_name(newDisplayName);
				}
			}

			TObject::inReading = true;
			// update the link list
			fromStudy->update_link_list_from_path(true);

			// o->find_all_children_and_links (true);
			fromStudy->update_after_read();
			TObject::inReading = false;

			if (util_ModuleRoot* mo = (util_ModuleRoot*)fromParent->moduleRoot())
				mo->updateAtClick = true;
		}
	}

}

QString T1dComponent::GetMachineType()
{
	QString mtype = "";
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
	if (pMachine)
	{
		mtype = pMachine->getMachineTypeString();
	}
	return mtype;
}

T1dStage* T1dComponent::getStage()
{
	T1dStage* pStage = dynamic_cast<T1dStage*>(parent("T1dStage"));
	if (pStage)
		return pStage;
	return nullptr;
}

void T1dComponent::Import1dComponent(T1dComponent* object)
{
	if (TObject* thisStudy = object->parent("StudyObject"))
	{
		if (TObject* db = (TObject*)util_OUtil::getCurrentDBObject(false))
		{
			QString dlgName = "meanlineComponentSearch";
			if (TObject* searchHolder = db->firstChild("db_SearchHolder"))
			{
				TObject* search = searchHolder->child(dlgName);
				if (!search)
					search = TObject::new_object("db_Search", dlgName, searchHolder);

				if (search)
				{
					QString searchPath = "Meanline";
					property_t* p = search->property("searchPath");
					if (p && !p->getValue().toString().size())
						p->setValue(searchPath);

					QString command = QString("showSearchImport1dComponentDialog %2 ").arg(searchPath);

					// get selected record
					if (TObject* record = VARIANT2OBJECT(search->execute(command)))
					{
						// load the 1d study
						// import component
						eprintf("Select %s", qPrintable(record->oname()));
						if (TObject* geometry = record->firstChild("db_Geometry")) // need to be modified
						{
							if (property_t* components = geometry->property("_1dComponents")) // need to be modified
							{
								QStringList L = components->toString().split(";");

								bool cover = true;
								QString operations;
								QString choice = getComponentChoice(L, cover, operations);


								if (choice.size())
								{
									if (property_t* studyP = record->property("TTStudy"))
									{
										QString recordPath = record->execute("getRecordPath").toString();
										QString studyFileName = recordPath + "/";
										studyFileName += studyP->getValue().toString();// +".tml";
										if (!studyFileName.toLower().endsWith(".tml"))
											studyFileName += ".tml";

										if (w_QFile(studyFileName).exists())
										{
											if (TObject* study = io::Interface::f2o(studyFileName, false))
											{
												if (TObject* oned = study->child("1d"))
												{
													if (TObject* machine = oned->firstChild("T1dMachine", true))
													{
#if 1                            
														if (TObject* selectedComponent = machine->object(choice))
														{
															if (selectedComponent->haveBaseClass("T1dStage"))
															{
																if (TObject* from = object->parent("T1dStage", true))
																{
																	copyStageObject(from, selectedComponent, operations);
																}
															}
															else
															{
																util_OUtil::copyToOtherStudy(selectedComponent, thisStudy, cover);
															}
														}
														else
														{
															printError("%s not found under %s", qPrintable(choice), qPrintable(machine->oname()));
														}
#else
														post_ObjectDialogPropertySelector dialog(core_Application::core());
														// set selections
														QVector<TObject *> xselections;
														dialog.addObject("Choose object:", machine,
															xselections,
															0,
															false,
															false,
															false
														);

														if (dialog.exec() == w_QDialog::Accepted)
														{
															if (xselections.size() == 1)
															{
																bool cover = true;
																util_OUtil::copyToOtherStudy(xselections.first(), thisStudy, cover);
															}
														}
#endif
													}

													//TObject::refreshTreeWithVisable(oned);
													core_Application::core()->go1D();
												}

												// remove the read study

												TObject::delete_object(study);
											}
										}
										else
										{
											eprintf("Error: File not found, %s", qPrintable(studyFileName));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


#if 1
int T1dComponent::intersectionPointBetween2Lines(Double2 L1s[2], Double2 L2s[2], Double2 & p_insert)
{
	int pos_value = 0;
	//(x,y) intersection Point
	//intersection Point at Line1 inside : return 1
	//intersection Point at Line2 inside : return 2
	//intersection Point at Line1 and Line2 inside : return 3
	//intersection Point at Line1 outside : return 10
	//intersection Point at Line2 outside : return 20
	//intersection Point at outside not at Line1 and Line2 : return 4
	//no intersection Point : return 0
	QVector<QVector<Double2>> Pts = {
		{ QVector<Double2>() << L1s[0] << L1s[1] },
		{ QVector<Double2>() << L2s[0] << L2s[1] } };
	//       L1s
	// p00---------p01
	//  |           |
	//  |           |
	//  |           |
	// p10---------p11
	//       L2s

	// Check
	// situation N_1: line close to point
	Double2 L_00_01 = Pts[0][1] - Pts[0][0]; // L0s
	Double2 L_10_11 = Pts[1][1] - Pts[1][0]; // L1s
	if (abs(L_00_01.length()) < tiny || abs(L_10_11.length()) < tiny)
		return pos_value;

	// situation N_2: L0 paraller with L1
	Double3 D3_k_L0_L1 = L_00_01.cross(L_10_11);
	double k_L0_L1 = fabs(D3_k_L0_L1[2]);
	if (k_L0_L1 < tiny)
		return pos_value;

	// following : has insection point between L0 with L1
	QVector<QVector<Double2>> Ls;
	Ls.resize(2);
	Ls[0].resize(2);
	Ls[1].resize(2);
	//
	Ls[0][0] = Pts[1][0] - Pts[0][0]; // Line_10_00
	Ls[0][1] = Pts[1][1] - Pts[0][0]; // Line_11_00
	Ls[1][0] = Pts[0][0] - Pts[1][0]; // Line_00_10
	Ls[1][1] = Pts[0][1] - Pts[1][0]; // Line_01_10
	QVector<QVector<double>> k_Ls;
	k_Ls.resize(2);
	k_Ls[0].resize(2);
	k_Ls[1].resize(2);


	QVector<Double2> LL = { QVector<Double2>() << L_00_01 << L_10_11 };
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			Double3 D3_temp1 = LL[j].cross(Ls[j][i]);
			Double3 D3_temp2 = LL[j].cross(Ls[j][i]);
			k_Ls[j][i] = D3_temp1[2];
			k_Ls[j][i] = D3_temp2[2];
		}
	}

	int sum_vertox = 0;
	QVector<int> record;
	int sum_vertox_L0 = 0;
	int sum_vertox_L1 = 0;
	QVector<int> record_L0;
	QVector<int> record_L1;
	record.clear();
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (abs(k_Ls[i][j]) < tiny)
			{
				sum_vertox++;
				record.push_back(i);
				record.push_back(j);
				if (i == 0)
				{
					sum_vertox_L0++;
					record_L0.push_back(i);
					record_L0.push_back(j);
				}
				else
				{
					sum_vertox_L1++;
					record_L1.push_back(i);
					record_L1.push_back(j);
				}
			}
		}
	}

	if (sum_vertox == 0)
	{
		// situation Y_1
		double sign_L0 = sign(k_Ls[0][0] * k_Ls[0][1]);
		double sign_L1 = sign(k_Ls[1][0] * k_Ls[1][1]);
		if (sign_L0*sign_L1 > 0)
		{
			if (sign_L0 < 0 && sign_L1 < 0)
				pos_value = 3;
			else if (sign_L0 > 0 && sign_L1 > 0)
				pos_value = 4;
		}
		else
		{
			if (sign_L0 > 0 && sign_L1 < 0)
				pos_value = 10;
			else if (sign_L0 < 0 && sign_L1 > 0)
				pos_value = 20;
		}
	}
	else if (sum_vertox == 1)
	{
		if (record_L0.size() == 1)
			pos_value = 2;
		else
			pos_value = 1;
	}
	else if (sum_vertox == 2)
	{
		pos_value = 3;
	}

	int value = intersectionPointBetween2LinesBase(L1s, L2s, p_insert);
	return pos_value;
}

#endif
int T1dComponent::InsPointBetween2Lines(double x_ins1, double y_ins1, double x_end1, double y_end1)
{
	int ins_value = 0;
	double theta_ins2 = atan(fabs(x_ins1 / y_ins1)) * 180. / PI;
	double theta_ending2 = atan(fabs((x_end1 - x_ins1) / (y_ins1 - y_end1))) * 180. / PI;
	double theta_ins3 = atan(fabs(x_ins1 / y_ins1)) * 180. / PI;
	double theta_ending3 = atan(fabs(x_end1 / y_end1)) * 180. / PI;
	if (x_ins1 > 0.)
	{
		if (x_ins1 > x_end1)
		{
			if (theta_ins3 < theta_ending3)
				ins_value = 4;
			else
				ins_value = 5;
		}
		else
		{
			ins_value = 3;
		}
	}
	else if (x_ins1 < 0.)
	{
		if (theta_ins2 > theta_ending2) 
			ins_value = 2;
		else
			ins_value = 1;
	}
	return ins_value;
}


int T1dComponent::intersectionPointBetween2LinesBase(Double2 L1s[2], Double2 L2s[2], Double2 & p_insert)
{
	double X1 = L1s[0][0];
	double Y1 = L1s[0][1];
	double X2 = L1s[1][0];
	double Y2 = L1s[1][1];

	double X3 = L2s[0][0];
	double Y3 = L2s[0][1];
	double X4 = L2s[1][0];
	double Y4 = L2s[1][1];
	double &x = p_insert[0];
	double &y = p_insert[1];

	double k1 = 0., k2 = 0.;
	if ((X1 == X2) && (Y1 == Y2) && (Y3 == Y4) && (X3 == X4))
		return 0;

	if ((X1 == X2) && (Y1 == Y2))
	{
		if ((X3 == X4) && (X1 == X3))
		{
			x = X1;
			y = Y1;
			return 1;
		}
		else if (Y2 == ((X2 - X3)*(Y4 - Y3) / (X4 - X3) + Y3))
		{
			x = X1;
			y = Y1;
			return 1;
		}
	}

	if ((Y3 == Y4) && (X3 == X4))
	{
		if ((X2 == X1) && (X1 == X3))
		{
			x = X3;
			y = Y3;
			return 2;
		}
		else if (Y3 == ((X3 - X1)*(Y2 - Y1) / (X2 - X1) + Y1))
		{
			x = X3;
			y = Y3;
			return 2;
		}
	}

	bool isNotY_axis = (abs(X1 - X2) > 0.00000001);
	//if (X1 != X2)
	if (isNotY_axis)
	{
		k1 = (Y2 - Y1) / (X2 - X1);
		if (X3 != X4)
		{
			k2 = (Y4 - Y3) / (X4 - X3);
			if (k1 == k2)
			{
				return 0;
			}
			x = (Y3 - Y1 - k2 * X3 + X1 * k1) / (k1 - k2);
			y = k1 * (x - X1) + Y1;
			if (((X1 - x)*(X2 - x) < 0 || (X1 - x)*(X2 - x) == 0) && ((Y1 - y)*(Y2 - y) < 0 || (Y1 - y)*(Y2 - y) == 0))
				return 1;
			if (((X3 - x)*(X4 - x) < 0 || (X3 - x)*(X4 - x) == 0) && ((Y3 - y)*(Y4 - y) < 0 || (Y3 - y)*(Y4 - y) == 0))
				return 2;
			if ((X3 - x)*(X4 - x) > 0 && (Y3 - y)*(Y4 - y) > 0 && (X1 - x)*(X2 - x) > 0 && (Y1 - y)*(Y2 - y) > 0)
				return 3;
		}
		if (X3 == X4)
		{
			x = X3;
			y = k1 * (X3 - X1) + Y1;
			return 1;
		}
	}
	else
	{
		if (X3 != X4)
		{
			k2 = (Y4 - Y3) / (X4 - X3);
			x = double(X1);
			y = k2 * (X1 - X3) + Y3;
			return 2;
		}
		if (X3 == X4)
		{
			return 0;
		}
	}
	return 0;
}

void T1dComponent::updateContourbaseWithStraight(Double2 Pt1, double &phi1, Double2 Pt2, double &phi2, TNurbsCurve *pCurve)
{
	int nc = 5;
	QVector<Double2> Pts; Pts.resize(nc);
	Pts[0] = Pt1;
	Pts[4] = Pt2;
	for (int i = 1; i < 4; i++)	{
		double t = i / (nc - 1.);
		Pts[i] = (1 - t)*Pt1+ t * Pt2;
	}
	double cs[10] = {0.};
	for (int i = 0; i < nc; i++) {
		cs[2 * i] = Pts[i][0];
		cs[2 * i + 1] = Pts[i][1];
	}
	pCurve->setInputData(nc, cs, 51, 3);

	double dZ = Pt2[0] - Pt1[0];
	double dR = Pt2[1] - Pt1[1];
	double phi = 0.;
	if (abs(dR) > abs(dZ))
	{
		double cos_phi = dZ/sqrt(SQR(dR) +SQR(dZ));
		phi = acos(cos_phi);
	}
	else {
		double tan_phi = dR/dZ;
		phi = atan(tan_phi);
	}
	phi1 = phi;
	phi2 = phi;
}

void T1dComponent::updateContourbaseWithStraight_Straight(Double2 Pt1, double phi1, Double2 Pt2, double phi2, TNurbsCurve *pCurve)
{
	QVector<double> phis = QVector<double>() << phi1 << phi2;
	Double2 p_insert;
	Double2 Z_axis = { 1., 0. };
	Double2 Line_1[2] = { Pt1, Pt1 + Z_axis.rotate(phis[0]) };
	Double2 Line_2[2] = { Pt2, Pt2 - Z_axis.rotate(phis[1]) };
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0)
	{
		p_insert = (Pt1 + Pt2) / 2.;
		//eprintf("Check input value.");
	}

	int np = 3;
	QVector<Double2> pZR; pZR.resize(np);
	pZR[0] = Pt1;
	pZR[1] = p_insert;
	pZR[2] = Pt2;
	double cs[6] = {0.};
	for (int i = 0; i < 3; i++) {
		cs[2 * i] = pZR[i][0];
		cs[2 * i + 1] = pZR[i][1];
	}
	//phubContour
	double d1 = (pZR[1] - pZR[0]).length();
	double d2 = (pZR[2] - pZR[0]).length();
	double u1 = d1 / d2;
	if (u1 > 0.99) {
		u1 = 0.99;
	}
	double vknot[] = { 0, 0, u1, 1, 1 };
	pCurve->setInputData(np, cs, 11, 2, 0, vknot);
}

void T1dComponent::updateContourbaseWithCycle_Straight_auto(Double2 Pt1, double phi1, double* _Rc, Double2 Pt2, double phi2, TNurbsCurve* pCurve)
{
	//Straght + cycle  + straight for shroud and hub
	// Inserting 2 points of boudary for Shoud and Hub
	// Stragedy : 

	/*

	phi_1 need to re-calculate
	*/


	double Rc = *_Rc;
	Double2 Z_axis = { 1., 0. };
	{
		Double2 Line_1_new[2] = { Pt1, Pt1 + Z_axis.rotate(phi2 - PI / 2.) };
		Double2 Line_2_new[2] = { Pt2, Pt2 - Z_axis.rotate(PI - phi2) };
		Double2 p_insert;
		int sit = intersectionPointBetween2Lines(Line_1_new, Line_2_new, p_insert);
		if (sit == 0)  eprintf("Check input value.");
		double distance_pt1_rearLine = (p_insert - Pt1).length();
		double cos_theta = (Rc - distance_pt1_rearLine) / Rc;
		double theta_new = acos(cos_theta);
		double distance_pt2_rearLine = Rc * sin(theta_new);
		Double2 pt2_rearLine = p_insert + Z_axis.rotate(phi2) * distance_pt2_rearLine;
		Double2 ZR_cycle_new = pt2_rearLine + (Z_axis.rotate(phi2)).rotate(PI / 2.) * Rc;
		double sin_phi_temp = (ZR_cycle_new[1] - Pt1[1]) / Rc;
		double phi_temp = asin(sin_phi_temp);
		phi1 = PI / 2 - phi_temp;
	
	}
	QVector<double> phis = QVector<double>() << phi1 << phi2;
	Double2 p_insert;
	Double2 Line_1[2] = { Pt1, Pt1 + Z_axis.rotate(phis[0]) };
	Double2 Line_2[2] = { Pt2, Pt2 - Z_axis.rotate(phis[1]) };
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0)  eprintf("Check input value.");


	int np = 7;
	QVector<Double2> pZR; pZR.resize(np); // In, Out
	double theta = phis[1] - phis[0];
	Double2 ZR_cycle;
	double _length_OC = Rc / cos(theta / 2);
	ZR_cycle = p_insert + _length_OC * (-1. * Z_axis.rotate(-1. * (PI / 2 - theta / 2 - phis[0])));

	QVector<Double2> pTangent; pTangent.resize(2); // Tangent point between stright with Cycle
	double _length_BC = Rc * tan(theta / 2);
	pTangent[0] = p_insert + _length_BC * (-1. * Z_axis.rotate(phis[0]));
	double _length_DC = Rc * tan(theta / 2);
	pTangent[1] = p_insert + _length_DC * (Z_axis.rotate(phis[1]));

	Double2 ZR_cycle2;
	Double2 wLine_1[2] = { pTangent[0], pTangent[0] + (Z_axis.rotate(phis[0])).rotate(PI / 2.) };
	Double2 wLine_2[2] = { pTangent[1], pTangent[1] + (Z_axis.rotate(phis[1])).rotate(PI / 2.) };
	sit = intersectionPointBetween2Lines(wLine_1, wLine_2, ZR_cycle);

	Double2 L1 = pTangent[0] - ZR_cycle;
	Double2 L2 = pTangent[1] - ZR_cycle;
	Double2 L11 = Line_1[1] - Line_1[0];
	Double2 L22 = Line_2[1] - Line_2[0];
	double _tmp1 = L1.dot(L11);
	double _tmp2 = L2.dot(L22);


	// Calculate the control poins
	int npt = 21;
	QVector<Double2> pZR_cycle;  pZR_cycle.resize(npt);
	Double2 Line_OB = pTangent[0] - ZR_cycle;
	double dTheta = theta / (npt - 1.);
	for (int i = 0; i < 21; i++)
	{
		double phi_i = i * dTheta;
		Double2 Line_OB_rotated = Line_OB.rotate(phi_i);
		pZR_cycle[i] = ZR_cycle + Line_OB_rotated;
	}
	double Z_center[21] = { 0. }, R_center[21] = { 0. };
	for (int i = 0; i < 21; i++)
	{
		Z_center[i] = pZR_cycle[i][0];
		R_center[i] = pZR_cycle[i][1];
	}

	TNurbsCurve* pContourCycle = new TNurbsCurve;
	pContourCycle->fitBezier(Z_center, R_center, npt, 4, 1, 3, 0);
	double* vec_CtrlP1_1 = pContourCycle->GetCtrlPoints(0);
	double* vec_CtrlP1_2 = pContourCycle->GetCtrlPoints(1);

	// control point 0-> 3
	int NC = pContourCycle->getNumCtrl();
	for (int i = 0; i <= 3; i++)
	{
		pZR[i] = Double2(vec_CtrlP1_1[i], vec_CtrlP1_2[i]);
	}

	double t[2] = { 1. / 3, 2. / 3 };
	// control point 6
	pZR[6] = Pt2;
	// control point 4 -> 5
	for (int i = 4; i <= 5; i++) {
		int j = i - 4;
		pZR[i] = (1 - t[j]) * pZR[3] + t[j] * pZR[6];
	}
	double cs[14] = { 0. };
	for (int i = 0; i < np; i++)
	{
		cs[2 * i] = pZR[i][0];
		cs[2 * i + 1] = pZR[i][1];
	}

	double d1 = (pZR[3] - pZR[0]).length();
	double d0 = (pZR[6] - pZR[0]).length();
	double u1 = d1 / d0;
	if (u1 > 0.99)
		u1 = 0.99;
	double vknot[14] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pCurve->setInputData(np, cs, 11, 4, 0, vknot);
  delete pContourCycle;
  return;
}

void T1dComponent::updateContourbaseWithStraight_Cycle_Straight(Double2 Pt1, double phi1, double *_Rc, Double2 Pt2, double phi2, TNurbsCurve *pCurve)
{
	//Straght + cycle  + straight for shroud and hub
	// Inserting 2 points of boudary for Shoud and Hub
	// Stragedy : 
	double Rc = *_Rc;
	QVector<double> phis = QVector<double>() << phi1 << phi2;
	Double2 p_insert;
	Double2 Z_axis = { 1., 0. };
	Double2 Line_1[2] = { Pt1, Pt1 + Z_axis.rotate(phis[0]) };
	Double2 Line_2[2] = { Pt2, Pt2 - Z_axis.rotate(phis[1]) };
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0)  eprintf("Check input value.");

	QVector<Double2> pZR; pZR.resize(10); // In, Out
	int np = 10;
	double theta = phis[1] - phis[0];
	Double2 ZR_cycle;
	double _length_OC = Rc / cos(theta / 2);
	ZR_cycle = p_insert + _length_OC * (-1.*Z_axis.rotate(-1.*(PI / 2 - theta / 2 - phis[0])));

	QVector<Double2> pTangent; pTangent.resize(2); // Tangent point between stright with Cycle
	double _length_BC = Rc * tan(theta / 2);
	pTangent[0] = p_insert + _length_BC * (-1.*Z_axis.rotate(phis[0]));
	double _length_DC = Rc * tan(theta / 2);
	pTangent[1] = p_insert + _length_DC * (Z_axis.rotate(phis[1]));

	Double2 ZR_cycle2;
	Double2 wLine_1[2] = { pTangent[0], pTangent[0] + (Z_axis.rotate(phis[0])).rotate(PI / 2.) };
	Double2 wLine_2[2] = { pTangent[1], pTangent[1] + (Z_axis.rotate(phis[1])).rotate(PI / 2.) };
	sit = intersectionPointBetween2Lines(wLine_1, wLine_2, ZR_cycle);

	Double2 L1 = pTangent[0] - ZR_cycle;
	Double2 L2 = pTangent[1] - ZR_cycle;
	Double2 L11 = Line_1[1] - Line_1[0];
	Double2 L22 = Line_2[1] - Line_2[0];
	double _tmp1 = L1.dot(L11);
	double _tmp2 = L2.dot(L22);


	// Calculate the control poins
	int npt = 21;
	QVector<Double2> pZR_cycle;  pZR_cycle.resize(npt);
	Double2 Line_OB = pTangent[0] - ZR_cycle;
	double dTheta = theta / (npt - 1.);
	for (int i = 0; i < 21; i++)
	{
		double phi_i = i * dTheta;
		Double2 Line_OB_rotated = Line_OB.rotate(phi_i);
		pZR_cycle[i] = ZR_cycle + Line_OB_rotated;
	}
	double Z_center[21] = { 0. }, R_center[21] = { 0. };
	for (int i = 0; i < 21; i++)
	{
		Z_center[i] = pZR_cycle[i][0];
		R_center[i] = pZR_cycle[i][1];
	}

	TNurbsCurve *pContourCycle = new TNurbsCurve;
	pContourCycle->fitBezier(Z_center, R_center, npt, 4, 1, 3, 0);
	double * vec_CtrlP1_1 = pContourCycle->GetCtrlPoints(0);
	double * vec_CtrlP1_2 = pContourCycle->GetCtrlPoints(1);
	// control point 0
	pZR[0] = Pt1;
	// control point 3-> 6
	int NC = pContourCycle->getNumCtrl();
	for (int i = 3; i <= 6; i++)
	{
		int j = i - 3;
		pZR[i] = Double2(vec_CtrlP1_1[j], vec_CtrlP1_2[j]);
	}

	// control point 1 -> 2
	double t[2] = { 1. / 3, 2. / 3 };
	for (int i = 1; i <= 2; i++) {
		int j = i - 1;
		pZR[i] = (1 - t[j])*pZR[0] + t[j] * pZR[3];
	}
	// control point 9
	pZR[9] = Pt2;
	// control point 7 -> 8
	for (int i = 7; i <= 8; i++) {
		int j = i - 7;
		pZR[i] = (1 - t[j])*pZR[6] + t[j] * pZR[9];
	}
	double cs[20] = { 0. };
	for (int i = 0; i < np; i++)
	{
		cs[2 * i] = pZR[i][0];
		cs[2 * i + 1] = pZR[i][1];
	}

	double d1 = (pZR[3] - pZR[0]).length();
	double d2 = (pZR[6] - pZR[0]).length();
	double d0 = (pZR[9] - pZR[0]).length();
	double u1 = d1 / d0;
	double u2 = d2 / d0;
	if (u1 > 0.99)
		u1 = 0.99;
	double vknot[14] = { 0, 0, 0, 0, u1, u1, u1, u2, u2, u2, 1, 1, 1, 1 };
	pCurve->setInputData(np, cs, 11, 4, 0, vknot);
  delete pContourCycle;
  return;
}

void T1dComponent ::updateContourbaseWithStraight_Cycle_Straight_CheckRC(Double2 Pt1, double phi1, double *_Rc, Double2 Pt2, double phi2, TNurbsCurve *pCurve,
	Double2 Pt1_checked, Double2 Pt2_checked, bool isCheck)
{
	//Straght + cycle  + straight for shroud and hub
  // Inserting 2 points of boudary for Shoud and Hub
  // Stragedy : 
	double Rc = *_Rc;
	if (Rc < tolr1) {
		Rc = 0;
		updateContourbaseWithStraight_Straight(Pt1, phi1, Pt2, phi2, pCurve);
		return;
	}

	QVector<double> phis = QVector<double>() << phi1 << phi2;
	Double2 p_insert;
	Double2 Z_axis = { 1., 0. };
	Double2 Line_1[2] = { Pt1, Pt1 + Z_axis.rotate(phis[0]) };
	Double2 Line_2[2] = { Pt2, Pt2 - Z_axis.rotate(phis[1]) };
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0)  eprintf("Check input value.");

	QVector<Double2> pZR; pZR.resize(10); // In, Out
	int np = 10;
	double theta = phis[1] - phis[0];
	Double2 ZR_cycle;
	double _length_OC = Rc / cos(theta / 2);
	ZR_cycle = p_insert + _length_OC * (-1.*Z_axis.rotate(-1.*(PI / 2 - theta / 2 - phis[0])));

	QVector<Double2> pTangent; pTangent.resize(2); // Tangent point between stright with Cycle
	double _length_BC = Rc * tan(theta / 2);
	pTangent[0] = p_insert + _length_BC * (-1.*Z_axis.rotate(phis[0]));
	double _length_DC = Rc * tan(theta / 2);
	pTangent[1] = p_insert + _length_DC * (Z_axis.rotate(phis[1]));

	// Check 2 Points
	double _lenght_C1check = (Pt1_checked - p_insert).length();
	double _lenght_C2check = (Pt2_checked - p_insert).length();
	double Rc_1check = _lenght_C1check/ tan(theta / 2);
	double Rc_2check = _lenght_C2check / tan(theta / 2);
	//
	//
	T1dVaned* pVaned = 0;
	if (haveBaseClass("T1dVaned"))
		T1dVaned* pVaned = dynamic_cast<T1dVaned*>(this);
	bool is2DBlade = false;
	if (pVaned)
	{
		int bladeType = pVaned->blade->bladeType;
		if (bladeType == T1dBlade::bladeTypes::Radial_2D_SCA_Type
			|| bladeType == T1dBlade::bladeTypes::Radial_2D_Free_Type
			|| bladeType == T1dBlade::bladeTypes::Radial_2D_Straight_Type
			|| bladeType == T1dBlade::bladeTypes::Radial_2D_DCA_Type
			|| bladeType == T1dBlade::bladeTypes::Radial_2D_SCA_Straight_Type
			)
			is2DBlade = true;
	}

	bool isNeedToAdjustPhi = false;
	if (Rc > Rc_1check)
	{
		isNeedToAdjustPhi = true;
		double phi1_new = 0.;

	}

	if (is2DBlade)
	{
		if (Rc > Rc_1check || Rc > Rc_2check) {
			if (Rc_1check > Rc_2check) {
				Rc = 0.95 * Rc_2check;
			}
			else {
				Rc = 0.95 * Rc_1check;
			}
			*_Rc = Rc;
			eprintf("Plase check Radius of curvature at hub, and reset now.");
		}
	}
	// re-set phi_1
	
	double delta_Rc_1check = (Rc_1check - Rc) / Rc_1check;
	double delta_Rc_2check = (Rc_2check - Rc) / Rc_2check;
	if (abs(delta_Rc_1check) < 1.E-3)
	{
		updateContourbaseWithCycle_Straight(
			Pt1, phi1, &Rc, Pt2, phi2, pCurve);
	}
	else if ( abs(delta_Rc_2check) < 1.E-3)
	{
		updateContourbaseWithCycle_Straight(
			Pt1, phi1, &Rc, Pt2, phi2, pCurve);
	}
	else if ( delta_Rc_1check < -1.E-3)
		updateContourbaseWithCycle_Straight_auto(
			Pt1, phi1, &Rc, Pt2, phi2, pCurve);
	else
		updateContourbaseWithStraight_Cycle_Straight(
			Pt1, phi1, &Rc, Pt2, phi2, pCurve);

#if 0

	updateContourbaseWithCycle_Straight(
		Pt1_s, phi1_shroud, &RCs, Pt2_s, phi2_shroud, pShroudContour);
#endif
}

void T1dComponent::updateContourbaseWithCycle(double* Rc, Double2& Pt1, double& phi1, Double2& Pt2, double& phi2, TNurbsCurve* pCurve, int whichPosition)
{
	double phi = phi2;
	double _Rc = *Rc;
	Double2 Pnt_tangential = Pt2;
	Double2 Pnt_pass = Pt1;
	Double2 zAxis = { 1., 0. };
	Double2 rAxis = {0., -1.};
	Double2 tangentialDir = zAxis.rotate(phi);
	Double2 normalDir = tangentialDir.rotate(PI/2.);

	Double2 centerPnt_Cycle = Pnt_tangential + _Rc *normalDir;

	double cos_theta = (centerPnt_Cycle[1] - Pnt_pass[1]) / _Rc;
	double theta_start = acos(cos_theta);
	double theta_end = phi2;

	QVector<Double2> Pnts_Arc;
	int np = 21;

	double dTheta = (theta_end - theta_start) / (np-1.);

  rAxis *= _Rc;
	for (int i = 0; i< np; i++)
	{
		double theta = theta_start + i* dTheta;
		Double2 Pnt = centerPnt_Cycle + rAxis.rotate(theta);
		Pnts_Arc.push_back(Pnt);
	}

	QVector<double> Z_arc, R_arc;
	for (int i = 0; i < np; i++)
	{
		Z_arc.push_back(Pnts_Arc[i][0]);
		R_arc.push_back(Pnts_Arc[i][1]);
	}

	pCurve->fitBezier(&Z_arc[0], &R_arc[0], np, 5);

	// 3. update
	Pt1 = Pnts_Arc.first();

}

QVector<double> T1dComponent::getUAtKontPoint(TNurbsCurve* nubrs)
{
	QVector<double> uKnot;
	if (!nubrs)
		return uKnot;// empty

	QVector<double> knotsVector = nubrs->GetKnotVector();
	if (knotsVector.size() == 0)
		return uKnot;


	int NC = nubrs->getNumCtrl();
	if(NC == 2)
		return uKnot;


	int deg = nubrs->getDegree();
	int order = nubrs->getOrder();
	int numKnots = nubrs->getNumKnots();

	QVector<double> knotsValue;

	// find 
	// remove Frist and Last knot vector with degree
	for (int i = 0; i< order; i++)
		knotsVector.removeLast();
	for (int i = 0; i < order; i++)
		knotsVector.removeFirst();

	int size = knotsVector.size();
	if (deg == 1 && size == 1)
		return knotsVector;
	for (int i = 0; i < size - 1; i++)
	{
		double value = knotsVector[i];
		int numRepetition = 0;
		for (int j=i+1; j< size; j++)
		{
			double delta = abs(value - knotsVector[j]);
			if (delta < tolr1)
				numRepetition++;
		}

		if (numRepetition == order)
			knotsValue.push_back(value);
	}


#if 0
	int order_1 = order - 1;
	if (order_1 < 1)
		order_1 = 1;


	int min_numKnots = 2 * deg + NC;

	if (numKnots >= min_numKnots)
		return uKnot;

	int num_Knot = (knotsVector.size() - 2 * order);



	for (int i = 1; i <= num_Knot; i++)
	{
		int index = order + order_1 * i - 1;
		double u = knotsVector[index];
		knotsValue.push_back(u);
	}

#endif

	return knotsValue;
}

const QStringList& T1dComponent::getConfigTabList()
{
	return configTabList;
}

void T1dComponent::updateContourbaseWithCycle_Straight(Double2 Pt1, double phi1, double *_Rc, Double2 Pt2, double phi2, TNurbsCurve *pCurve)
{
	// Straght + cycle  + straight for shroud and hub
	// Inserting 2 points of boudary for Shoud and Hub
	// Stragedy : 

	QVector<double> phis = QVector<double>() << phi1 << phi2;
	double eff_g = 1.;
	double theta = phis[1] - phis[0];
	if ( (Pt2[0] - Pt1[0]) < 0.)
	 eff_g = -1.;

	Double2 p_insert;
	Double2 Z_axis = { 1., 0. };
	Double2 Line_1[2] = { Pt1, Pt1 + eff_g*Z_axis.rotate(phis[0]) };
	Double2 Line_2[2] = { Pt2, Pt2 - eff_g*Z_axis.rotate(phis[1]) };
	int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
	if (sit == 0)
	{
		eprintf("Check Radius Curvature input value, and this value will be reset. ");
		*_Rc = 0;
	}
	
	QVector<Double2> pZR; pZR.resize(10); // In, Out
	int np = 7;

	//theta *= eff_g;
	double Rc = (p_insert - Pt1).length() / tan(theta / 2);
	Double2 ZR_cycle;
	double _length_OC = Rc / cos(theta / 2);
	QVector<Double2> pTangent; pTangent.resize(2); // Tangent point between stright with Cycle
	double _length_BC = Rc * tan(theta / 2);
	pTangent[0] = p_insert + eff_g*_length_BC * (-1.*Z_axis.rotate(phis[0]));
	double _length_DC = Rc * tan(theta / 2);
	pTangent[1] = p_insert + eff_g*_length_DC * (Z_axis.rotate(phis[1]));

	Double2 ZR_cycle2;
	Double2 wLine_1[2] = { pTangent[0], pTangent[0] + eff_g*(Z_axis.rotate(phis[0])).rotate(PI / 2.) };
	Double2 wLine_2[2] = { pTangent[1], pTangent[1] + eff_g*(Z_axis.rotate(phis[1])).rotate(PI / 2.) };
	sit = intersectionPointBetween2Lines(wLine_1, wLine_2, ZR_cycle);

	// Calculate the control poins
	int npt = 21;
	QVector<Double2> pZR_cycle;  pZR_cycle.resize(npt);
	Double2 Line_OB = pTangent[0] - ZR_cycle;
	double dTheta = theta / (npt - 1.);
	for (int i = 0; i < 21; i++)
	{
		double phi_i = i * dTheta;
		Double2 Line_OB_rotated = Line_OB.rotate(phi_i);
		pZR_cycle[i] = ZR_cycle + Line_OB_rotated;
	}
	double Z_center[21] = { 0. }, R_center[21] = { 0. };
	for (int i = 0; i < 21; i++)
	{
		Z_center[i] = pZR_cycle[i][0];
		R_center[i] = pZR_cycle[i][1];
	}

	TNurbsCurve *pContourCycle = new TNurbsCurve;
	pContourCycle->fitBezier(Z_center, R_center, npt, 4, 1, 3, 0);
	double * vec_CtrlP1_1 = pContourCycle->GetCtrlPoints(0);
	double * vec_CtrlP1_2 = pContourCycle->GetCtrlPoints(1);

	// control point 0 -> 3
	int NC = pContourCycle->getNumCtrl();
	for (int i = 0; i <= 3; i++)
		pZR[i] = Double2(vec_CtrlP1_1[i], vec_CtrlP1_2[i]);

	// control point 6
	pZR[6] = Pt2;
	// control point 4 -> 5
	double t[2] = { 1. / 3, 2. / 3 };
	for (int i = 4; i <= 5; i++) {
		int j = i - 4;
		pZR[i] = (1 - t[j])*pZR[3] + t[j] * pZR[6];
	}

#if 0
	if (eff_g < 0.)
	{
		Double2 Line = pZR[6] - pZR[0];
		for (int i = 1; i < 6; i++)
		{
			Double2 Line_i = pZR[i] - pZR[0];
			double angle = Line_i.angle(Line);
			double t = Line_i.length()*cos(angle) / Line.length();
			Double2 pt_media = (1-t)*pZR[0]  + t*pZR[6];
			pZR[i] = 2.*pt_media - pZR[i];
		}
	}
#endif
	double cs[14] = { 0. };
	for (int i = 0; i < np; i++) {
		cs[2 * i] = pZR[i][0];
		cs[2 * i + 1] = pZR[i][1];
	}

	double d1 = (pZR[3] - pZR[0]).length();
	double d0 = (pZR[6] - pZR[0]).length();
	double u1 = d1 / d0;
	if (u1 > 0.99)
		u1 = 0.99;
	double vknot[11] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pCurve->setInputData(np, cs, 11, 4, 0, vknot);
  delete pContourCycle;
  return;
}

void T1dComponent::getCompPerfFromHMT(QVector<QVector<double>> &QNlineAveProperty, QVector<TObject*> &HMTproperty)
{
  //for test
  enum
  {
    P_Ave, T_Ave, P0_Ave, T0_Ave, Cm_Ave, Ct_Ave, C_Ave,
    Wm_Ave, Wt_Ave, W_Ave, Beta_Ave, rho_Ave, h_Ave, s_Ave, h0_Ave, MAX_VARSATAVE
  };
  int QNlineIndex = 0;//QNlineIndex = 0 component inlet , QNlineIndex = 1 component outlet

  //get avarage value after hmt analysis
  out->p = QNlineAveProperty[P_Ave][QNlineIndex + 1];
  out->h = QNlineAveProperty[h_Ave][QNlineIndex + 1];
  out->Cm = QNlineAveProperty[Cm_Ave][QNlineIndex + 1];
  out->Ct = QNlineAveProperty[Ct_Ave][QNlineIndex + 1];
  out->C = QNlineAveProperty[C_Ave][QNlineIndex + 1];
  out->Wm = QNlineAveProperty[Wm_Ave][QNlineIndex + 1];
  out->Wt = QNlineAveProperty[Wt_Ave][QNlineIndex + 1];
  out->W = QNlineAveProperty[W_Ave][QNlineIndex + 1];
  out->W = QNlineAveProperty[W_Ave][QNlineIndex + 1];
  out->Wm = QNlineAveProperty[Wm_Ave][QNlineIndex + 1];
  out->U = out->Ct - out->Wt;//Recalculate U based on new velocity triangle not U=rms*Omega
  out->Beta = QNlineAveProperty[Beta_Ave][QNlineIndex + 1];
  out->fill_ph();
  copyObject(out->phub, HMTproperty[3]);
  copyObject(out->prms, HMTproperty[4]);
  copyObject(out->ptip, HMTproperty[5]);

  bool isFirstStageInlet = false;
  T1dStage* pStage = dynamic_cast<T1dStage*>(parent("T1dStage"));
  if (pStage->stageID == 1 && haveBaseClass("T1dInlet"))
    isFirstStageInlet = true;

  if (isFirstStageInlet)
  {
    in->p0 = QNlineAveProperty[P0_Ave][QNlineIndex];
    in->h0 = QNlineAveProperty[h0_Ave][QNlineIndex];
    in->Cm = QNlineAveProperty[Cm_Ave][QNlineIndex];
    in->Ct = QNlineAveProperty[Ct_Ave][QNlineIndex];
    in->C = QNlineAveProperty[C_Ave][QNlineIndex];
    in->Wm = QNlineAveProperty[Wm_Ave][QNlineIndex];
    in->Wt = QNlineAveProperty[Wt_Ave][QNlineIndex];
    in->W = QNlineAveProperty[W_Ave][QNlineIndex];
    in->W = QNlineAveProperty[W_Ave][QNlineIndex];
    in->Wm = QNlineAveProperty[Wm_Ave][QNlineIndex];
    in->U = in->Ct - in->Wt;
    in->Beta = QNlineAveProperty[Beta_Ave][QNlineIndex];
    in->fill_ph0();
    copyObject(in->phub, HMTproperty[0]);
    copyObject(in->prms, HMTproperty[1]);
    copyObject(in->ptip, HMTproperty[2]);
    ((T1dInlet*)this)->p0inlet = in->p0;
    ((T1dInlet*)this)->t0inlet = in->t0;
  }
}

O_METHOD_BEGIN(T1dComponent, UpdateAfterImportFromDatabaseBase, "Update after import from database", 0, FLG_METHOD_SKIP_GUI | FLG_METHOD_SKIP_TUI, "Update after import from database")
{
  if (VARLID_ARG)
  {
    if (object->getWindow())
    {
      if (TTaskWindow* w = dynamic_cast<TTaskWindow*>((QObject*)object->getWindow()))
        w->redraw();
    }

    return true;
  }
}
O_METHOD_END;
