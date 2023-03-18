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

#include "1d_impeller.h"
#include "1d_seal.h"
#include "1d_Stage.h"
#include "1d_stage_pump.h"
#include "1d_FrontLeakPath.h"
#include "1d_RearLeakPath.h"
#include "1d_object.h"
#include "util_OUtil.h"
#include "util_HtmlReportTitle.h" 
#include "w_TModule.h"
#include "1d_3DInterface.h"

REGISTER_OBJECT_CLASS(T1dImpeller, "impeller", T1dVaned);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(T1dImpeller, "impeller");

T1dImpeller::T1dImpeller(QString object_n, TObject *iparent) :
  T1dVaned(object_n, iparent)
{
  INIT_OBJECT;
  DEFINE_SCALAR_INIT(int, trim, "Trim", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Mu, "Rotation Mach number", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Ma_Wmax, "Mach number of Wmax", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, power, "Power", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, D2_d, "Design impeller diameter", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, U2_d, "Design impeller wheel speed", 0, NULL, TUnit::velocity);
  DEFINE_SCALAR_INIT(double, mu_d, "Design impeller target head coefficient", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, beta2b_d, "Design impeller exit blade angle", 0, NULL, TUnit::bladeAngle);

  DEFINE_SCALAR_INIT(double, axLen_disk, "Axial length of disk", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, thick_shroud, "Thickness of shroud", 0, NULL, TUnit::length); property("thick_shroud")->setRange(0., 9999.);
  DEFINE_SCALAR_INIT(double, thick_disk, "Thickness of disk", 0, NULL, TUnit::length);
  //DEFINE_SCALAR_INIT(double, d_shaft, "Shaft diameter", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, axF_total, "Axial thrust, total", 0, NULL, TUnit::force);
  DEFINE_SCALAR_INIT(double, axF_shroud, "Aixal thrust, shroud", 0, NULL, TUnit::force);
  DEFINE_SCALAR_INIT(double, axF_back, "Axial thrust, back", 0, NULL, TUnit::force);
  DEFINE_SCALAR_INIT(double, axF_inlet, "Axial thrust, inlet", 0, NULL, TUnit::force);
  DEFINE_SCALAR_INIT(double, axF_outlet, "Axial thrust, outlet", 0, NULL, TUnit::force);
	DEFINE_SCALAR_INIT(double, axThurst_balanced, "Balanced axial thrust target", 0, NULL, TUnit::force);
  DEFINE_SCALAR_INIT(bool, excludeShaftFront, "Exclude shaft front", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, excludeShaftRear, "Exclude shaft rear", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, isDoubleSuction, "Is double suction", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, isOverHang, "Is over hang", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, hasFrontSeal, "Has front seal", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, hasRearSeal, "Has rear seal", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, hasBalancePiston, "Has balance piston", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, nrow_splitter, "Number of splitter rows specified", 0, NULL, NULL);

  DEFINE_SCALAR_INIT(double, d_piston, "Balance piston diameter", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, Psuction, "Suction line inlet pressure", 0, NULL, TUnit::pressure);
  DEFINE_SCALAR_INIT(double, dp_suction_line, "Suction line pressure loss", 0, NULL, TUnit::deltaP);

  //parasitic power loss
  DEFINE_SCALAR_INIT(double, Peuler, "Euler power", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, Precirc, "Recirculation power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, Pclr, "Clearance power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, PleakF, "Front leakage power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, PleakR, "Rear leakage power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, Pdisk, "Disk friction power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, Pparasitic, "Total parasitic power loss", 0, NULL, TUnit::power);
  DEFINE_SCALAR_INIT(double, k_parasitic, "Fractional parasitic power loss", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, eta_tt_rotor_ad, "Rotor isentropic efficiency (T-T)", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, eta_ts_rotor_ad, "Rotor isentropic efficiency (T-S)", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, deltaEta_parasitic, "Parasitic efficiency decrement", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, mLeakF, "Front leakage mass flow rate", 0, NULL, TUnit::mass_flow);
  DEFINE_SCALAR_INIT(double, mLeakR, "Rear leakage mass flow rate", 0, NULL, TUnit::mass_flow);

  DEFINE_SCALAR_INIT(double, p_parasitic_offset, "Parasitic power loss offset", 0.0, NULL, NULL);
  DEFINE_QDOUBLE3_VECTOR_INIT(dp_para_dr_vec, 0, 0, 0, NULL, 0);
  property("p_parasitic_offset")->setDataReductionVectorName("dp_para_dr_vec"); // set the parameter vector

  DEFINE_SCALAR_INIT(double, Lamda_w_offset, "Lamda_w offset", 0.0, NULL, NULL);
  DEFINE_QDOUBLE3_VECTOR_INIT(lamda_w_dr_vec, 0, 0, 0, NULL, 0);
  property("Lamda_w_offset")->setDataReductionVectorName("lamda_w_dr_vec"); // set the parameter vector


  DEFINE_SCALAR_INIT(double, p_parasitic_offset_in, "Parasitic power loss offset input", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, chokeMargin, "Choke margin", 0.0, NULL, NULL);
//for ported shroud
  DEFINE_SCALAR_INIT(double, Z_bsi, "Axial location of bleed slot from impeller inlet", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, gap_bsi, "Ported shroud gap width", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, gap_bse, "Bleed channel discharge gap width", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, Kt, "Bleed channel resistance factor", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, K_pbsi, "Multiplier for bleed channel inlet pressure", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, K_pbse, "Multiplier for bleed channel discharge pressure", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, swirl_factor, "Swirl factor (Ctheta/U) at the bleed slot", 0.0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, power_ported_shroud, "Ported-shroud power loss", 0, NULL, TUnit::power);

  nType = type_impeller;
  color = "cornflowerblue";
  isHMT = true;
  Init();
  isDoubleSuction = false;
  //DEFINE_CHILD(frontLeakPath, T1dLeakPath, 0);
  DEFINE_CHILD(frontLeakPath, T1dFrontLeakPath, 0);
  DEFINE_CHILD(rearLeakPath, T1dRearLeakPath, 0);
  DEFINE_CHILD(rearDisk, T1dRearDisk, 0);
  DEFINE_CHILD(shroud, T1dShroud, 0);
	DEFINE_CHILD(sta_le, T1dStation, 0);
	DEFINE_CHILD(sta_te, T1dStation, 0);
	DEFINE_CHILD_HIDDEN(inducer, T1dVaneless, 0);
	DEFINE_CHILD_HIDDEN(imp, T1dVaned, 0);
	DEFINE_CHILD_HIDDEN(exducer, T1dVaneless, 0);

	DEFINE_CHILD(pShroudPlot, TCurve, "plot-shroud");
  //DEFINE_CHILD(pFrontLeakPlot, TCurve, "plot-frontleak");
  DEFINE_CHILD(pFrontLeakPathContour, TNurbsCurve, "plot-frontleak");
  DEFINE_CHILD(pRearLeakPlot, TCurve, "plot-rearleak");

  //design option
  DEFINE_SCALAR_INIT(int, inletDesignOption, "Inlet design option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, outletDesignOption, "Outlet design option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, R1hOption, "R1h option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, OutletRaOption, "Outlet Ra option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, OutletBOption, "Outlet b option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, R1h_specified, "Inlet hub radius specified", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, R1t_specified, "Inlet tip radius specified", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, fdS, "Min.rel.velocity factor", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Km1, "Intake number(Km1)", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Km2, "Outlet number(Km2)", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Kd, "Outlet radius coefficient", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Kb, "Outlet width coefficient", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, workCoeff_d, "Optimum design work coefficient", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Ku2, "Outlet coefficient(Ku2)", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Lamda_c, "NPSHr coefficient: Lambda_c", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Lamda_w, "NPSHr coefficient: Lambda_w", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, K0, "Speed coefficient(K0),3.5~5.5", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, R1t_R2a, "R1t and R2a ratio", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, Laxial_D, "Axial length to diameter ratio", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, L_boss_AL, "Shaft end axial length ratio", 0, NULL, NULL); // Added by zhaoj
  DEFINE_SCALAR_INIT(double, b2d2, "Outlet width to diameter ratio", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, b2_specified, "Outlet width specified", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, D1D2, "D1/D2", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, phi_d, "Design flow coefficient", 0, NULL, NULL); property("phi_d")->setRange(0.01, 0.2);
  DEFINE_SCALAR_INIT(double, d1dE, "D_le/D_eye", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, alpha2_target, "Target outlet flow angle", 0, NULL, TUnit::bladeAngle);
  DEFINE_SCALAR_INIT(double, dHd2, "Ratio of inlet hub dia. to OD", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, xi, "Deceleration coefficient", 0, NULL, NULL); // Add from Yue
  DEFINE_SCALAR_INIT(double, RatioCone, "Radius of cone at hub", 0, NULL, NULL); // Add from Yue
  DEFINE_SCALAR_INIT(double, ConeAngle, "Angle of cone at hub", 0, NULL, TUnit::deltaAngle); // Add from Yue
  DEFINE_SCALAR_INIT(double, R_boss, "Hub radius of shaft end", 0, NULL, TUnit::length); // Added by zhaoj
  DEFINE_SCALAR_INIT(double, L_boss, "Axial length of shaft end", 0, NULL, TUnit::length); // Added by zhaoj
  DEFINE_SCALAR_INIT(double, phi_boss, "Hub contour angle of shaft end", 0, NULL, TUnit::deltaAngle); // Added by zhaoj
  DEFINE_SCALAR_INIT(double, xi2, "Cm2/Cm1", 0, NULL, NULL); // Add from Yue
  DEFINE_SCALAR_INIT(double, AeA0, "Aeye/A1", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, T0d2, "Thick0/d2", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(bool, is2d, "2D blade", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(double, GammaThick, "Angle of blade and hub", 0, NULL, TUnit::deltaAngle);// Add by zj
  DEFINE_SCALAR_INIT(double, RCs, "Radius of curvature at shroud contour", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(double, RCh, "Radius of curvature at hub contour", 0, NULL, TUnit::length);
  DEFINE_SCALAR_INIT(int, contourType, "Contour type", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, dr_data_option, "dr_data_option", 0, NULL, NULL);
  DEFINE_SCALAR_INIT(int, AxLengthOption, "Axial Length Type", 0, NULL, NULL);

  K0 = 4.0;
  b2d2 = 0.13;
  fdS = 1.05;
  Km1 = 0.212;
  Km2 = 0.161;
  Ku2 = 1.58;
  workCoeff_d = 0.82;
  Kd = 1.1;
  Kb = 1.05;
  Lamda_c = 1.15;
  Lamda_w = 0.2;
  alpha2_target = 65.0 / 180 * PI;
  setPropertyCollectable("RPM");
  RPM = 10000;
  hasInletGeom = true;
  d1dE = 1.05;
  GammaThick = 4;
  e_pass = 20.0E-6; //impeller is assumed to be milled, N8, 20 micro-meter Rt (peak-to-valley)
  chokeMargin = 0.1;
  contourType = 0;
  dr_data_option = 0;
  setShowText(false);
	AxRad = 1;
  isSizing = false;
  Kt = 1.0; //bleed channel resistence
  K_pbsi = 1.0; //multiplier for bleed slot inlet pressure
  K_pbse = 1.0; //multiplier for bleed slot exit pressure
  swirl_factor = 0.5; //the Ctheta/U at the bleed slot
	blockage_throat = 0.02;
  set_icon_name(":images/1d_impeller.png");

}

void T1dImpeller::createItemPlot(bool modify)
{
  T1dComponent::createItemPlot(modify);
}

void T1dImpeller::GetAvailableInputPropertyList(QStringList &SL, int iflag)
{
  GetAvailableInputPropertyListBase(SL, iflag);
  QStringList sl = QStringList() << "devMultiplier" << "mlc_parasitic"
    << "mlc_inc" << "mlc_frict" << "mlc_load" << "mlc_distort" << "mlc_mix" << "mlc_shock" << "mlc_clr" << "mlc_choke" << "mf_block";
  QString path = this->path();
  forLoop(i, sl.size())
  {
    SL += path + "/" + sl[i];
  }
  return;
}

void T1dImpeller::GetDataReductionPList_AxialThrust(QStringList &SL)
{
	QStringList L = QStringList();
	QString path = this->path();

	L.push_back("out/p0");
	L.push_back("out/t0");
	L.push_back("out/p");
	L.push_back("out/t");
	L.push_back("out/Ct");
	L.push_back("out/Cm");
	L.push_back("in/p");
	L.push_back("in/Cm");

	if (frontLeakPath)
	{
		L.push_back("frontLeakPath/in/p");
		L.push_back("frontLeakPath/in/t");
		L.push_back("frontLeakPath/in/Ct");
		L.push_back("frontLeakPath/axThrust_target");
		L.push_back("frontLeakPath/leakFlow_target");
	}
	if (rearLeakPath)
	{
		L.push_back("rearLeakPath/in/p");
		L.push_back("rearLeakPath/in/t");
		L.push_back("rearLeakPath/in/Ct");
		L.push_back("rearLeakPath/axThrust_target");
		L.push_back("rearLeakPath/leakFlow_target");
		L.push_back("rearLeakPath/p_ex");
	}
	L.push_back("axThurst_balanced");

	forLoop(i, L.size())
	{
		SL += path + "/" + L[i];
	}
	return;

}

void T1dImpeller::GetAvailableThrustParameterPropertyList(QStringList& L,int flag)
{
	QStringList LL = QStringList();
	LL << "out/p" << "out/t" << "out/Ct" << "out/Cm" << "in/p" << "in/Cm";
	if (frontLeakPath)
		LL.push_back("frontLeakPath/mLeak");
	if (rearLeakPath)
	{
		LL.push_back("rearLeakPath/mLeak");
		LL.push_back("rearLeakPath/p_ex");
		LL.push_back("rearLeakPath/pin_angap"); 
		LL.push_back("rearLeakPath/tin_angap");
		LL.push_back("rearLeakPath/Ctin_angap");
	}
	LL.push_back("in/Rh");
	LL.push_back("in/Rs");
	LL.push_back("out/Ra");
	LL.push_back("out/b");
  LL.push_back("rearDisk/RdiskRear");

  QString p = path(true);
  forLoop(i, LL.size())
  {
    if (property(LL[i]))
      L += p + "/" + LL[i];
  }
}

void T1dImpeller::setTargetValue_axialthrust(QString type, double propertyValue)
{
	if (type=="front")
	{
		if (frontLeakPath)
		{
			property_t* prop = frontLeakPath->property("axThrust_target");
			if (prop)
				prop->setValue(propertyValue);
		}
	}
	if (rearLeakPath && type=="rear")
	{
		property_t* prop = rearLeakPath->property("axThrust_target");
		if(prop)
			prop->setValue(propertyValue);
	}
}

void T1dImpeller::update_after_read(bool recursive, bool updateProperty)
{
	T1dVaned::update_after_read(recursive, updateProperty);

	if (updateProperty)
		updateWidgetsGeometry();
  T1dVaned::update_after_read(recursive, updateProperty);
}

void T1dImpeller::setDR_axialThrust(int ipoint, TMPASetting *dr)
{
	T1dStage *pStage = (T1dStage *)parent("T1dStage");
	QString stageX = pStage->oname() + QString("->");

	QVector<QString> propertyNames;
	QVector<double> propertyValues;
	dr->GetParameterSet(ipoint, propertyNames, propertyValues);
	int nparam = propertyNames.size();

	//set default
	if (frontLeakPath)
	{
		frontLeakPath->inputMLeak = false;
		frontLeakPath->axThrust_target = 0;
		frontLeakPath->leakFlow_target = 0;
	}
	if (rearLeakPath)
	{
		rearLeakPath->inputMLeak = false;
		rearLeakPath->axThrust_target = 0;
		rearLeakPath->leakFlow_target = 0;
	}
	for (int j = 0; j < nparam; j++)
	{
		if (propertyNames[j].endsWith(stageX + "impeller->out->p"))
		{
			out->p = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->t"))
		{
			out->t = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->Ra"))
		{
			out->Ra = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->Ct"))
		{
			out->Ct = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->Cm"))
		{
			out->Cm = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->in->Cm"))
		{
			in->Cm = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->in->p"))
		{
			in->p = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->in->t"))
		{
			in->t = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->axThurst_balanced"))
		{
			axThurst_balanced = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->mLeak"))
		{
			frontLeakPath->mLeak = propertyValues[j];
			frontLeakPath->inputMLeak = true;
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->mLeak"))
		{
			rearLeakPath->mLeak = propertyValues[j];
			rearLeakPath->inputMLeak = true;
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->in->p"))
		{
			frontLeakPath->in->p = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->in->t"))
		{
			frontLeakPath->in->t = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->in->Ct"))
		{
			frontLeakPath->in->Ct = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->axThrust_target"))
		{
			frontLeakPath->axThrust_target = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->frontLeakPath->leakFlow_target"))
		{
			frontLeakPath->leakFlow_target = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->in->p"))
		{
			rearLeakPath->in->p = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->in->t"))
		{
			rearLeakPath->in->t = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->in->Ct"))
		{
			rearLeakPath->in->Ct = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->axThrust_target"))
		{
			rearLeakPath->axThrust_target = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->leakFlow_target"))
		{
			rearLeakPath->leakFlow_target = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->p_ex"))
		{
			rearLeakPath->p_ex = propertyValues[j];
			rearLeakPath->LeakOutletOption = 2;
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->pin_angap"))
		{
			rearLeakPath->pin_angap = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->tin_angap"))
		{
			rearLeakPath->tin_angap = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->rearLeakPath->Ctin_angap"))
		{
			rearLeakPath->Ctin_angap = propertyValues[j];
		}
	}
}

void T1dImpeller::setGeometry_axialThrust(TMPASetting *dr)
{
	T1dStage *pStage = (T1dStage *)parent("T1dStage");
	QString stageX = pStage->oname() + QString("->");

	QVector<QString> propertyNames;
	QVector<double> propertyValues;
	dr->GetParameterSet(0, propertyNames, propertyValues);
	int nparam = propertyNames.size();
	//get imp index
	int num_imp = 0;
	bool findImp = false;
	if (pStage)
	{
		forLoop(i, pStage->compList.size())
		{
			if (pStage->compList[i] == this)
			{
				num_imp = i;
				findImp = true;
			}
		}
	}

	for (int j = 0; j < nparam; j++)
	{
		if (propertyNames[j].endsWith(stageX + "impeller->in->Rh"))
		{
			in->Rh = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->in->Rs"))
		{
			in->Rs = propertyValues[j];
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->Ra"))
		{
			out->Ra = propertyValues[j];
			if (pStage && findImp)
			{
				if (pStage->compList.size() >= num_imp + 1)
					pStage->compList[num_imp + 1]->out->Ra = propertyValues[j] * 1.2;
			}
			L_axial = propertyValues[j] * 0.4;
		}
		if (propertyNames[j].endsWith(stageX + "impeller->out->b"))
		{
			out->b = propertyValues[j];
			if (pStage && findImp)
			{
				if (pStage->compList.size() >= num_imp + 1)
					pStage->compList[num_imp + 1]->out->b = propertyValues[j];
			}
		}
    if (propertyNames[j].endsWith(stageX + "impeller->rearDisk->RdiskRear"))
    {
      rearDisk->RdiskRear = propertyValues[j];
    }
	}
}

void T1dImpeller::calculateLETE()
{
	// This fuction is avaliable to calculate LE & TE with pMeanContour precisely.
	// To find (Z, R) at LE & TE along mean contour

	QVector<double *> u_mean ={ QVector<double *>() << &ule_mean << &ute_mean};
	QVector<TNurbsCurve *> pBladeEdges = { QVector<TNurbsCurve *>() << blade->pLE << blade->pTE };

  for (int i = 0; i < 2; i++)
  {
    if (!pBladeEdges[i])
      continue;
    QVector<double> uEdges = pMeanContour->Intersection_between2Curves(pBladeEdges[i]);
		if (uEdges.size() == 0)
			continue;
		*u_mean[i] = uEdges.first();
  }

	blade->getMeanSection()->u1 = ule_mean;
	blade->getMeanSection()->u2 = ute_mean;

	blade->hubSection->u1 = ule_hub;
	blade->hubSection->u2 = ute_hub;

	blade->tipSection->u1 = ule_shroud;
	blade->tipSection->u2 = ute_shroud;
}

T1dStation* T1dImpeller::getBladeEdgeStation(int edgeType)
{
	// edgeType = 0 for LE(or in),  edgeType = 1 for TE( or out)
	T1dStation* edgeStation = sta_le;
	if (edgeType == 1)
		edgeStation = sta_te;
	return edgeStation;
}

void T1dImpeller::UpdateBlade()
{
  trim = SQR(in->Rs / out->Ra) * 100;
  updateBladeContour();
  SetEdgeGeometry();
  LinkCurvePointer();
  calculateLETE();

  UpdateBeta();  //update beta bezier curve   

  if (!getUseGeomCurve())//use geometry curve
  { 
    UpdateThick(); //update thickness Bezier curve
  }

  UpdateSplitter();
  CalculateGeometry();
  setThroat(0);
  isLSD = false;
  UpdateBladeSection();
  LinkCurvePointer();
  PlotBladeMeridionalView();// blade->PlotMeridionalView();
}

void T1dImpeller::updateBladeContour()
{
	double u1s = 0., u1h = 0., u2s = 1., u2h = 1.;
	QString mtype = GetMachineType();
	bool found = false;
  if (mtype == "pump" || mtype == "compressor")
		FindU(u1s, u1h, u2s, u2h);
	else if (mtype == "fan")
	{
		T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
		if (pMachine->AppType == T1dMachine::AppType_MVR)
		{
			u1s = ule_shroud;
			u1h = ule_hub;
			double Z = 0., R = 0.;
			pShroudContour->getPoint(u1s, Z, R);
			RLE_shroud = R;
			Rs1_RsE = RLE_shroud / in->Rs;
			pHubContour1->getPoint(u1h, Z, R);
			RLE_hub = R;
			Rh1_RhE = RLE_hub / in->Rh;

			if (!isUserInput(this, "RTE_shroud"))
			{
				Rs2_RsO = 1.;
				RTE_shroud = Rs2_RsO*out->Rs;
			}
			if(RTE_shroud > out->Rs)
				RTE_shroud = out->Rs;
			Rs2_RsO = RTE_shroud / out->Rs;
			u2s = pShroudContour->getUfromY(RTE_shroud, found);
			ute_shroud = u2s;
			if (!isUserInput(this, "RTE_hub"))
			{
				Rh2_RhO = 1.;
				RTE_hub = Rh2_RhO*out->Rh;
			}
			if (RTE_hub > out->Rh)
				RTE_hub = out->Rh;
			Rh2_RhO = RTE_hub / out->Rh;
			u2h = pHubContour1->getUfromY(RTE_hub, found);
			ute_hub = u2h;
		}
		else
		{
			Rs1_RsE = RLE_shroud / in->Rs;
			Rh1_RhE = RLE_hub / in->Rh;
			u1h = pHubContour1->getUfromY(RLE_hub, found);
			u1s = pShroudContour->getUfromY(RLE_shroud, found);

			u2s = ute_shroud;
			u2h = ute_hub;
			double Z = 0., R = 0.;
			pShroudContour->getPoint(u2s, Z, R);
			RTE_shroud = R;
			Rs2_RsO = RTE_shroud / out->Rs;
			pHubContour1->getPoint(u2h, Z, R);
			RTE_hub = R;
			Rh2_RhO = RTE_hub / out->Rh;
		}
	}
	else
	{
		double Zs1, Rs1, Zh1, Rh1;
		if (Rh1_RhE < 1) Rh1_RhE = 1;
		if (Rs1_RsE < 1) Rs1_RsE = 1;
		Rs1 = Rs1_RsE * in->Rs;
		Rh1 = Rh1_RhE * in->Rh;
		u1s = pShroudContour->getUfromY(Rs1, found);
		u1h = pHubContour1->getUfromY(Rh1, found);

		double Rs2, Rh2;
		if (Rs2_RsO > 1) Rs2_RsO = 1;
		if (Rh2_RhO > 1) Rh2_RhO = 1;
		Rs2 = Rs2_RsO * out->Rs;
		Rh2 = Rh2_RhO * out->Rh;
		u2s = pShroudContour->getUfromY(Rs2, found);
		u2h = pHubContour1->getUfromY(Rh2, found);
	}

	if (!(isUserInput(this, "ule_hub")) && !getUseGeomCurve()) {
		ule_hub = u1h;
	}
	if (!(isUserInput(this, "ule_shroud")) && !getUseGeomCurve()) {
		ule_shroud = u1s;
	}
	if (!(isUserInput(this, "ute_hub")) && !getUseGeomCurve()) {
		ute_hub = u2h;
	}
	if (!(isUserInput(this, "ute_shroud")) && !getUseGeomCurve()) {
		ute_shroud = u2s;
	}

  SetBladeEdges();

	blade->LE->NB = blade->numMainBlade;
	blade->numSplitterRows = blade->SplitterLeLoc.size();
	blade->TE->NB = blade->numMainBlade*(1 + blade->numSplitterRows);
	if ( isDoubleSuction)
		UpdateBladeContourDS(u1h, u2h, u1s, u2s);
	else if (this->child("plot-shroud-DS"))
		clearPlotDS();
}

void T1dImpeller::SetEdgeGeometry()
{
	in->RecalcGeom();
	out->RecalcGeom();
	UpdateContour();
	pHubContour1->getPoint(ule_hub, sta_le->Zh, sta_le->Rh);
	pHubContour1->getPoint(ute_hub, sta_te->Zh, sta_te->Rh);
	pShroudContour->getPoint(ule_shroud, sta_le->Zs, sta_le->Rs);
	pShroudContour->getPoint(ute_shroud, sta_te->Zs, sta_te->Rs);
	//LE
	double dR_le = sta_le->Rs - sta_le->Rh;
	double dZ_le = sta_le->Zs - sta_le->Zh;
	if (dR_le == 0)
	{
		sta_le->phi = 0.5*PI;
	}
	else
	{
		sta_le->phi = atan(dZ_le / dR_le);
	}
	sta_le->Za = 0.5 * (sta_le->Zs + sta_le->Zh);
	sta_le->Ra = 0.5*(sta_le->Rs + sta_le->Rh);
	sta_le->b = sqrt(dR_le*dR_le + dZ_le * dZ_le);
	//TE
	double dR_te = sta_te->Rs - sta_te->Rh;
	double dZ_te = sta_te->Zs - sta_te->Zh;
	if (dR_te == 0)
	{
		sta_te->phi = 0.5*PI;
	}
	else
	{
		sta_te->phi = atan(dZ_te / dR_te);
	}
	sta_te->Za = 0.5 * (sta_te->Zs + sta_te->Zh);
	sta_te->Ra = 0.5*(sta_te->Rs + sta_te->Rh);
	sta_te->b = sqrt(dR_te*dR_te + dZ_te * dZ_te);

	sta_le->geomOption = 0;
	sta_te->geomOption = 0;
	sta_le->RecalcGeom();
	sta_te->RecalcGeom();

	sta_le->phub->R = sta_le->Rh;
	sta_le->phub->Z = sta_le->Zh;
	sta_le->pmean->R = sta_le->Ra;
	sta_le->pmean->Z = sta_le->Za;
	sta_le->prms->R = sta_le->Rm;
	sta_le->prms->Z = sta_le->Zm;
	sta_le->ptip->R = sta_le->Rs;
	sta_le->ptip->Z = sta_le->Zs;

	T1dVaned::SetEdgeGeometry();
}

void T1dImpeller::Beta_K(double K, int np, double *betas, double * betah, double *betam, double *ms, double *mh, double *mm)
{  
  //double ule_hub; //LE(hub) relative location on length
  //double ule_shroud; //LE(shroud) relative location on length
  //double ute_hub; //TE(hub) relative location on length
  //double ute_shroud; //TE(shroud) relative location on length
#if 1
//for hub
  double beta1h = blade->hubSection->beta1b * 180 / PI + 90;
  double beta1s = blade->tipSection->beta1b * 180 / PI + 90;
  double beta1= blade->getMeanSection()->beta1b * 180 / PI + 90;
  double beta2 = blade->getMeanSection()->beta2b * 180 / PI + 90;
  double beta2s = blade->tipSection->beta2b * 180 / PI + 90;
  double beta2h = blade->hubSection->beta2b * 180 / PI + 90;
  if (!isHMT)
  {
    beta1h = beta1;
    beta1s = beta2;
    beta2s = beta2;
    beta2h = beta2;
  }
  double deltaB = 0;
  double ksih = 0., ksis = 0.;
  double duh = (ute_hub - ule_hub) / (np - 1);
  double dus = (ute_shroud - ule_shroud) / (np - 1);
  double dksi = 1.0 / (np - 1);
  for (int i = 0; i < np; i++)
  {
    double uh = ule_hub + i*duh;
    double us = ule_shroud + i*dus;
    ksih = dksi*i;
    ksis = dksi*i;
    double ule_m = 0.5*(ule_hub + ule_shroud);
    double um = 0.5*(us + uh);
    ms[i] = pShroudContour->calculate2DLength(ule_shroud, us);
    mh[i] = pHubContour1->calculate2DLength(ule_hub, uh);
    mm[i] = pMeanContour->calculate2DLength(ule_m, um);
    betas[i] = beta1s + (beta2s - beta1s)*(3 * ksis*ksis - 2 * ksis*ksis*ksis);
    double bbar = 90 * K + 0.5*(1 - K)*(beta2h + beta1h-deltaB) - beta_bar_adjust;
    double A = -4 * (beta2h - 2 * bbar + beta1h);
    double B = 11 * beta2h - 16 * bbar + 5 * beta1h;
    double C = -6 * beta2h + 8 * bbar - 2 * beta1h;
    betah[i] = beta1h + A*ksih + B*ksih*ksih + C*ksih*ksih*ksih;
    //to radian
    betas[i] = (betas[i] - 90)*PI / 180.0;
    betah[i] = (betah[i] - 90)*PI / 180.0;
    betam[i] = 0.5*(betas[i] + betah[i]);
  }

#else
  double beta1h = blade->hubSection->beta1b * 180 / PI + 90;
  double beta1s = blade->tipSection->beta1b * 180 / PI + 90;
  double beta2 = blade->rmsSection->beta2b * 180 / PI + 90;
  double ksi;
  double dksi = 1.0 / (np - 1);
  for (int i = 0; i < np; i++)
  {
    ksi = dksi*i;
    ms[i] = pShroudContour->calculate2DLength(0, ksi);
    mh[i] = pHubContour1->calculate2DLength(0, ksi);
    mm[i] = pMeanContour->calculate2DLength(0, ksi);
    betas[i] = beta1s + (beta2 - beta1s)*(3 * ksi*ksi - 2 * ksi*ksi*ksi);
    double bbar = 90 * K + 0.5*(1 - K)*(beta2 + beta1h);
    double A = -4 * (beta2 - 2 * bbar + beta1h);
    double B = 11 * beta2 - 16 * bbar + 5 * beta1h;
    double C = -6 * beta2 + 8 * bbar - 2 * beta1h;
    betah[i] = beta1h + A*ksi + B*ksi*ksi + C*ksi*ksi*ksi;
    //to radian
    betas[i] = (betas[i] - 90)*PI / 180.0;
    betah[i] = (betah[i] - 90)*PI / 180.0;
    betam[i] = 0.5*(betas[i] + betah[i]);
  }
#endif

}

double T1dImpeller::ThroatArea(int station)
{
  double throat_area = T1dVaned::ThroatArea(station);
  double adjust = 1;
  throat_area *= adjust; //this is due to the radial effect
  return throat_area;
}

void T1dImpeller::CurveBeta(double K)
{
  int np = 20; //number of line points
  int nctrl = 6;  //number of control points
	double betah[20] = {0.}, betas[20] = { 0. }, betam[20] = { 0. }, mh[20] = { 0. }, ms[20] = { 0. }, mm[20] = { 0. };
  if (!(getUseGeomCurve()))//use geometry curve
  {
    Beta_K(K, np, betas, betah, betam, ms, mh, mm);
    pHubBeta->fitBezier(mh, betah, np, nctrl, 1, 3, 0);
    pShroudBeta->fitBezier(ms, betas, np, nctrl, 1, 3, 0);
    pMeanBeta->fitBezier(mm, betam, np, nctrl, 1, 3, 0);
  }
  else
  {
    pMeanBeta->Average(pHubBeta, pShroudBeta); 
  }
#if 0
  int p = pHubBeta->getDegree();
  int nk = pHubBeta->getNumKnots();
  int nc = pHubBeta->getNumCtrl();
  int check = nk - nc - p - 1;

  TNurbsCurve *ph = new   TNurbsCurve;
  ph->fitBezier(mh, betah, np, nctrl, 1, 3, 0);
#endif

#if 0
  double du = 1.0 / (np - 1);
  eprintf("fit test, mh, betah, ms, betas, m, xh, yh, xs, ys\n");
  for (int i = 0; i < 20; i++)
  {
    double u = i*du;
    double xh, yh, xs, ys;
    pHubBeta->getPoint(u, xh, yh);
    pShroudBeta->getPoint(u, xs, ys);
    eprintf("%f, %f, %f, %f, %f, %f, %f %f\n", mh[i], betah[i], ms[i], betas[i], xh, yh, xs, ys);
  }
  nctrl = pHubBeta->getNumCtrl();
  double *cpx = pHubBeta->GetCtrlPoints(0);
  double *cpy = pHubBeta->GetCtrlPoints(1);
  eprintf("hub control points\n");
  for (int i = 0; i < nctrl; i++)
  {
    eprintf("%f, %f\n", cpx[i], cpy[i]);
  }

  nctrl = pShroudBeta->getNumCtrl();
  cpx = pShroudBeta->GetCtrlPoints(0);
  cpy = pShroudBeta->GetCtrlPoints(1);
  eprintf("shroud control points\n");
  for (int i = 0; i < nctrl; i++)
  {
    eprintf("%f, %f\n", cpx[i], cpy[i]);
  }

#endif

}

double T1dImpeller::calcLean()
{
  //assume LE theta_h=theta_t
  T1dStreamLine hub(pHubContour1, pHubBeta, pHubThick);
  T1dStreamLine shroud(pShroudContour, pShroudBeta, pShroudThick);
  double ule_h, ute_h, ule_s, ute_s;
#if 0
  pHubContour1->getUrange(ule_h, ute_h);
  pShroudContour->getUrange(ule_s, ute_s);
#else
  ule_h = ule_hub;
  ute_h = ute_hub;
  ule_s = ule_shroud;
  ute_s = ute_shroud;
#endif

  hub.SetEdge(ule_h, ute_h);
  shroud.SetEdge(ule_s, ute_s);
  double theta_hub = hub.calculate_theta(0, ute_h);
  double theta_shroud = shroud.calculate_theta(0, ute_s);
  return theta_shroud - theta_hub;
}

void T1dImpeller::validateBladeType()
{
	if (getUseGeomCurve())
		return;

	int machType = T1dMachine::getMachineType();
	// 0. compressor
	if (machType == mach_compressor)
	{
		// 0. for old case of pump
		if (blade->bladeType == T1dBlade::Ruled_3D_Type
			|| blade->bladeType == T1dBlade::Ruled_3D_Free_Type
			|| blade->bladeType == T1dBlade::Free_3D_Type
			|| blade->bladeType == T1dBlade::blade_usebladeCurves)
		{
			; // do nothing
		}
		else
		{
			blade->bladeType = T1dBlade::Ruled_3D_Type;
		}
	}

	//1. pump
  if (machType == mach_pump)
  {
    // 0. for old case of pump
		if (blade->bladeType == T1dBlade::Ruled_3D_Free_Type
			|| blade->bladeType == T1dBlade::Free_3D_Type 
			|| blade->bladeType == T1dBlade::Axial_Element_Free_Type)
		{
			;// do nothing
		}
		else
		{
			blade->bladeType = T1dBlade::Ruled_3D_Free_Type;
		}
  }

	//2. fan
	if (machType == mach_fan)
	{
		// 0. for old case of fan
		if (blade->bladeType == T1dBlade::Radial_2D_SCA_Type
			|| blade->bladeType == T1dBlade::Radial_2D_DCA_Type
			|| blade->bladeType == T1dBlade::Radial_2D_Straight_Type
			|| blade->bladeType == T1dBlade::Radial_2D_Straight_Type
			|| blade->bladeType == T1dBlade::Radial_2D_SCA_Straight_Type
			|| blade->bladeType == T1dBlade::Ruled_3D_Free_Type
			|| blade->bladeType == T1dBlade::Compisition_Type
			)
		{
			;// do nothing
		}
		else
		{
			blade->bladeType = T1dBlade::Radial_2D_SCA_Type;
		}
		setHasMeanlineGeometry(false);
	}

	// 3. set
	if (blade->bladeType == T1dBlade::Ruled_3D_Type)
		setHasMeanlineGeometry(false);
	if (blade->bladeType == T1dBlade::Ruled_3D_Free_Type)
		setHasMeanlineGeometry(false);
	if (blade->bladeType == T1dBlade::Free_3D_Type)
		setHasMeanlineGeometry(true);
	if (blade->bladeType == T1dBlade::Axial_Element_Free_Type)
		setHasMeanlineGeometry(true);
}

void T1dImpeller::UpdateBeta()
{
	if (getUseGeomCurve())//use geometry curve
	{
		int error = 0;
    if (!isTandemBlade)
    {
      if (hasMeanGeometry)
      {
        error = updateRmsSectionFromGeneralBladeGenerator();
      }
      else
      {
        error = updateMeanSectionFromRuledGenerator();
        error = updateRmsSectionFromRuledGenerator();
      }
    }
		return;
	}
	
	validateBladeType();

  if (blade->bladeType == blade->bladeTypes::Compisition_Type)
  {
    UpdateBetaWithCompistions();
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Type)
  {
    //UpdateBetaWithCycle();
    UpdateBeta_SCA_Radial();
  }
  else 	if (blade->bladeType == blade->bladeTypes::Radial_2D_DCA_Type)
  {
    UpdateBeta_DCA_Radial();
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_Straight_Type)
  {
    UpdateBeta_Flat_Radial();
  }
  else if (blade->bladeType == blade->bladeTypes::Ruled_3D_Free_Type)
  {
		hasMeanGeometry = false;

    UpdateBeta_Ruled_Free();

		if (!hasMeanGeometry)
			updateMeanSectionFromRuledGenerator();

		updateRmsSectionFromRuledGenerator();
  }
	else if (blade->bladeType == blade->bladeTypes::Free_3D_Type)
	{
		UpdateBetaWithWrapAngle();

		//int error = updateRmsSectionFromGeneralBladeGenerator();

	}
  else if (blade->bladeType == blade->bladeTypes::Ruled_3D_Type)
  {
    UpdateBeta3DRuleBlade();

    if (!hasMeanGeometry)
      updateMeanSectionFromRuledGenerator();

    updateRmsSectionFromRuledGenerator();
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Straight_Type)
  {
    UpdateBeta_SCA_Flat_Radial();
  }
	else if(blade->bladeType == blade->bladeTypes::blade_usebladeCurves)
	{
		updateBladeByBladeEditor();
	}
	else if (blade->bladeType == blade->bladeTypes::Axial_Element_Free_Type)
	{
		UpdateBetaAxialElementFreeBlade();
	}
  return;
}

// by Feihong for axial element free blade
double T1dImpeller::getbetam(double betar, double phi)
{
	auto item_temp = sqrt(1 + 1 / (pow(tan(phi), 2)));
	auto tan_betam = tan(betar) / item_temp;
	return atan(tan_betam);
}

double T1dImpeller::getbetar(double betam, double phi)
{
	auto item_temp = sqrt(1 + 1 / (pow(tan(phi), 2)));
	auto tan_betar = tan(betam) * item_temp;
	return atan(tan_betar);
}

void T1dImpeller::getMeanbetab(double beta1b, double beta2b, double& beta1br, double& beta2br)
{
	// 1.mean
	Double2 TM_le, TM_te;
	pMeanContour->getTangentialDirection(ule_mean, TM_le);
	pMeanContour->getTangentialDirection(ute_mean, TM_te);
	beta1br = getbetar(beta1b, TM_le.angle());
	beta2br = getbetar(beta2b, TM_te.angle());
}

void T1dImpeller::getAllbetab(double beta1br, double beta2br)
{
	// 2.tip
	Double2 TT_le, TT_te;
	pShroudContour->getTangentialDirection(ule_shroud, TT_le);
	pShroudContour->getTangentialDirection(ute_shroud, TT_te);
	blade->tipSection->beta1b = getbetam(beta1br, TT_le.angle());
	blade->tipSection->beta2b = getbetam(beta2br, TT_te.angle());

	// 3.hub
	Double2 TH_le, TH_te;
	pHubContour1->getTangentialDirection(ule_hub, TH_le);
	pHubContour1->getTangentialDirection(ute_hub, TH_te);
	blade->hubSection->beta1b = getbetam(beta1br, TH_le.angle());
	blade->hubSection->beta2b = getbetam(beta2br, TH_te.angle());
}

void T1dImpeller::UpdateBetaAxialElementFreeBlade()
{	
	// 0.
	auto beta1b = blade->getMeanSection()->beta1b;
	auto beta2b = blade->getMeanSection()->beta2b;
	auto wrapAngle = blade->getMeanSection()->wrapAngle;
	// 1. 
	auto beta1br = 0., beta2br = 0.;
	getMeanbetab(beta1b, beta2b, beta1br, beta2br);
	// 2.
	Get_MbarTheta_conformalMapping_all(beta1br, beta2br, wrapAngle);
	// 3.
	getAllbetab(beta1br, beta2br);
}

void T1dImpeller::Get_MbarTheta_conformalMapping_all(double beta1br, double beta2br, double wrapAngle)
{
	auto error = 0;
	// 1. mean (set Mbar-Theta distribution)
	// 1.1 calculate Start- and Endpoint
	auto Mbar_start = 0.;
	auto Mbar_end = cal_Mbar_conformalMapping(pMeanContour, pMeanContour, ule_mean, ute_mean);
	auto Theta_start = 0.;
	auto Theta_end = wrapAngle;
	Double2 pt_start = { Mbar_start, Theta_start };
	Double2 pt_end = { Mbar_end, Theta_end };

	// 1.2 drawing Theta-Mbar-diagram(pMbarThetaCurve)
	QVector<Double2> Mbar_Theta;
	TNurbsCurve* pMbarThetaCurve = new TNurbsCurve;
	get_MThetaCurve(pt_start, pt_end, tan(beta1br), tan(beta2br), Mbar_Theta, pMbarThetaCurve);

	// 1.3 tip
	// 1.3.1 u_tip
	auto Mbar_start_tip = cal_Mbar_conformalMapping(pMeanContour, pShroudContour, ule_mean, ule_shroud);
	auto fo = false;
	auto ut = pMbarThetaCurve->getUfromX(Mbar_start_tip, fo);
	// 1.3.2 Tip_StartPoint
	double Theta_start_tip;
	pMbarThetaCurve->getPoint(ut, Mbar_start_tip, Theta_start_tip);
	blade->tipSection->wrapAngle = wrapAngle - Theta_start_tip;

	// 1.4 hub_StartPoint
	auto Mbar_start_hub = cal_Mbar_conformalMapping(pMeanContour, pHubContour1, ule_mean, ule_hub);
	auto Theta_start_hub = Mbar_start_hub / tan(beta1br);
	blade->hubSection->wrapAngle = wrapAngle - Theta_start_hub;

	// 2. transform from Mbar-theta to m-beta
	// 2.1 Mean
	get_mBetafromMbarTheta(pMeanContour, ule_mean, ute_mean, pMeanBeta, pMbarThetaCurve);
	// 2.2 Tip
	get_mBetafromMbarTheta(pShroudContour, ule_shroud, ute_shroud, pShroudBeta, pMbarThetaCurve);
	// 2.3 Hub
	get_mBetafromMbarTheta(pHubContour1, ule_hub, ute_hub, pHubBeta, pMbarThetaCurve);
}

void T1dImpeller::get_mBetafromMbarTheta(TNurbsCurve* pZRCurve, double u1, double u2, TNurbsCurve* pmBetaCurve, TNurbsCurve* pMbarThetaCurve)
{
	// 1.getPoint from MbarTheta
	auto Mbar_start = 0., theta_start = 0.;
	pMbarThetaCurve->getPoint(0, Mbar_start, theta_start);
	auto Mbar_end = 0., theta_end = 0.;
	pMbarThetaCurve->getPoint(1., Mbar_end, theta_end);

	// 2.1 init
	auto np = NUMPTS;
	auto du = (u2 - u1) / (np - 1.);
	auto slop = 0.;
	QVector<Double2> mBeta; mBeta.resize(np);
	QVector<Double2> TP; TP.resize(np);
	QVector<double> Phi;

	// 2.2 loop
	for (auto i = 0; i < np; i++)
	{
		// 2.2.1 get ui/m
		auto ui = u1 + du * i;
		auto m = pZRCurve->calculate2DLength(u1, ui);
		
		// 2.2.2 getmBeta
		// a.calculate Mbar
		auto Mbar = cal_Mbar_conformalMapping(pMeanContour, pZRCurve, ule_mean, ui);

		// b.get betabr
		if (Mbar < Mbar_start)
			pMbarThetaCurve->getSlope(0., slop);
		else if (Mbar > Mbar_end)
			pMbarThetaCurve->getSlope(1., slop);
		else
		{
			auto found = false;
			auto u = pMbarThetaCurve->getUfromX(Mbar, found);
			if (found) 
				pMbarThetaCurve->getSlope(u, slop);
			else 
				eprintf("can't find Mbar parameters");
		}
		// c.Phi
		pZRCurve->getTangentialDirection(ui, TP[i]);
		Phi.push_back(TP[i].angle());

		// d.mBeta
		auto betabr = atan(slop);
		auto betabm = getbetam(betabr, Phi[i]);
		mBeta[i] = Double2(m, betabm);
	}

	// 3.get pmBetaCurve
	QVector<double> ms, Betab;
	for (auto i = 0; i < np; i++)
	{
		ms.push_back(mBeta[i][0]);
		Betab.push_back(mBeta[i][1]);
	}
	pmBetaCurve->fitBezier(&ms[0], &Betab[0], np, 8, 1, 3);
}

double T1dImpeller::cal_Mbar_conformalMapping(TNurbsCurve* pZRCurve1, TNurbsCurve* pZRCurve2, double u1, double u2)
{
	auto Z1 = 0., R1 = 0., Z2 = 0., R2 = 0.;
	pZRCurve1->getPoint(u1, Z1, R1); 
	pZRCurve2->getPoint(u2, Z2, R2);
	return log(R2 / R1);
}

// 
void T1dImpeller::UpdateBeta3DRuleBlade()
{
	double tol = 1.0E-3;
	double ax = 0.4;
	double bx = 0.5;
	double cx = 0., fa = 0., fb = 0., fc = 0.;
	double K = 0;
	beta_bar_adjust = 0;
	for (int i = 0; i < 5; i++)
	{
		int error = bracketBetaK(ax, bx, cx, fa, fb, fc);
		if (error == 0)break;
		beta_bar_adjust += 10;
	}
	TNR::golden(this, ax, bx, cx, T1dImpeller::minFuncLeanWrapper, tol, K);
	if (K < 0)K = 0;
	if (K > 1)K = 1;
	fc = calcLean();
	CurveBeta(K);

	blade->hubSection->wrapAngle = calcWrapAngleFromBladeSectionBetaCurve(pHubBeta, pHubContour1, ule_hub, ute_hub);
	blade->tipSection->wrapAngle = calcWrapAngleFromBladeSectionBetaCurve(pShroudBeta,pShroudContour, ule_shroud, ute_shroud);
}

void T1dImpeller::UpdateBetaWithWrapAngle()
{
	// hub
	double beta1b = blade->hubSection->beta1b;
	double beta2b = blade->hubSection->beta2b;
	double wrapAngle = -fabs(blade->hubSection->wrapAngle);
	QVector<Double2> ZR_hub;
	QVector<Double2> mBeta_hub;
	Get_MTheta_conformalMapping(pHubContour1, ule_hub, ute_hub, beta1b, beta2b, wrapAngle, pHubBeta, ZR_hub, mBeta_hub);
	// tip
	beta1b = blade->tipSection->beta1b;
	beta2b = blade->tipSection->beta2b;
	wrapAngle = -fabs(blade->tipSection->wrapAngle);
	QVector<Double2> ZR_tip;
	QVector<Double2> mBeta_tip;
	Get_MTheta_conformalMapping(pShroudContour, ule_shroud, ute_shroud, beta1b, beta2b, wrapAngle, pShroudBeta, ZR_tip, mBeta_tip);
	// mean
	beta1b = blade->getMeanSection()->beta1b;
	beta2b = blade->getMeanSection()->beta2b;
	wrapAngle = -fabs(blade->getMeanSection()->wrapAngle);
	QVector<Double2> ZR_mean;
	QVector<Double2> mBeta_mean;
	Get_MTheta_conformalMapping(pMeanContour, ule_mean, ute_mean, beta1b, beta2b, wrapAngle, pMeanBeta, ZR_mean, mBeta_mean);
	//pMeanBeta->Average(pHubBeta, pShroudBeta);

}

void T1dImpeller::UpdateBetaWithCycle()
{
	//if (isMultiVane())
		//return; // TODO: review by Huanjing
	double beta1h_u = 0., beta2h_u = 0.;
	double beta2 = blade->getMeanSection()->beta2b;
	if (beta2 < 0.)
	{
		beta1h_u = blade->hubSection->beta1b + PI / 2.;
		beta2h_u = blade->hubSection->beta2b + PI / 2.;
	}
	else
	{
		beta1h_u = -blade->hubSection->beta1b + PI / 2.;
		beta2h_u = PI/2 - blade->hubSection->beta2b;
	}

	double Z1h_le = 0., R1h_le = 0.;
	pHubContour1->getPoint(blade->ule_hub, Z1h_le, R1h_le);
	double Z2h_te = 0., R2h_te = 0.;
	pHubContour1->getPoint(blade->ute_hub, Z2h_te, R2h_te);

	// Total parameters
	double Rb = (SQR(R2h_te) - SQR(R1h_le)) / (2.*(R2h_te*cos(beta2h_u) - R1h_le*cos(beta1h_u)));
	double Rm = sqrt(SQR(Rb) + SQR(R2h_te) - 2.*R2h_te*Rb*cos(beta2h_u));
	double phi1h = acos((SQR(Rm) + SQR(Rb) - SQR(R1h_le)) / (2.*Rm*Rb));
	double phi2h = acos((SQR(Rm) + SQR(Rb) - SQR(R2h_te)) / (2.*Rm*Rb));

	const int npt = 50;
	// For Hub
	double mh[npt + 1] = { 0. }, Mh[npt + 1] = { 0. }, thetah[npt + 1] = { 0. }, betah[npt + 1] = { 0. };
	double R_h[npt + 1] = { 0. };
	double Rh_old = 0., Zh_old = 0., Rh_now = 0., Zh_now = 0.;
	mh[0] = 0.;
	Mh[0] = 0.;
	thetah[0] = 0.;
	pHubContour1->getPoint(blade->ule_hub, Zh_old, Rh_old);
	R_h[0] = Rh_old;
	double phih_old = phi1h;
	double duh = (ute_hub - ule_hub) / npt;
	
	// calculate the critical radius : determine the sign of theta
	double cos_phi1 = (SQR(Rm) + SQR(R1h_le) - SQR(Rb)) / (2.*Rm*R1h_le);
	double L_critial = 2*(Rm*cos_phi1 - R1h_le);

	// For Shroud
	double Z1s_le = 0., R1s_le = 0.;
	double ms[npt + 1] = { 0. }, Ms[npt + 1] = { 0. }, thetas[npt + 1] = { 0. }, betas[npt + 1] = { 0. };
	double R_s[npt + 1] = { 0. };
	double Rs_old = 0., Zs_old = 0., Rs_now = 0., Zs_now = 0.;

	pShroudContour->getPoint(blade->ule_shroud, Z1s_le, R1s_le);
	pShroudContour->getPoint(blade->ule_shroud, Zs_old, Rs_old);
	double phi1s = acos((SQR(Rm) + SQR(Rb) - SQR(Rs_old)) / (2.*Rm*Rb));
	ms[0] = 0.;
	Ms[0] = 0.;
	R_s[0] = R1s_le;
	thetas[0] = thetah[0] - acos((SQR(R1h_le) + SQR(Rs_old) - SQR(Rb*(phi1s - phi1h))) / (2.*R1h_le*Rs_old));
	double phis_old = phi1s;
	double dus = (ute_shroud - ule_shroud) / npt;

	// search ule_mean
	double Z1_mean = 0.5*(Z1h_le + Z1s_le);
	double R1_mean = 0.5*(R1h_le + R1s_le);
	double ule_mean = 0.;

	double R1m[3] = { 0. }, Z1m[3] = { 0. };
	double u1m[3] = {0., 0.5, 1.};
	double L1m[3] = { 0. };
	double tol = 1.e-6;
	for (int i =0; i<3;i++)
	{
		pMeanContour->getPoint(u1m[i], Z1m[i], R1m[i]);
		L1m[i] = fabs( R1m[i] - R1_mean);
	}
	int upper = 0;
	while (L1m[1] > tol )
	{
		if (L1m[0] <= L1m[2])
		{
			u1m[2] = u1m[1] + 0.5*(u1m[2] - u1m[1]);
		}
		else
		{
			u1m[0] = u1m[0] + 0.5*(u1m[1] - u1m[0]);
		}
		u1m[1] = 0.5*(u1m[0]+ u1m[2]);
		for (int i = 0; i<3; i++)
		{
			pMeanContour->getPoint(u1m[i], Z1m[i], R1m[i]);
			L1m[i] = fabs(R1m[i] - R1_mean);
		}
		upper++;
		if (upper > 100) break;
	}
	ule_mean = u1m[1];
	// For Shroud
	double Z1m_le = 0., R1m_le = 0.;
	double mm[npt + 1] = { 0. }, Mm[npt + 1] = { 0. }, thetam[npt + 1] = { 0. }, betam[npt + 1] = { 0. };
	double R_m[npt + 1] = { 0. };
	double Rm_old = 0., Zm_old = 0., Rm_now = 0., Zm_now = 0.;

	pMeanContour->getPoint(ule_mean, Z1m_le, R1m_le);
	pMeanContour->getPoint(ule_mean, Zm_old, Rm_old);
	R_m[0] = R1m_le;
	double phi1m = acos((SQR(Rm) + SQR(Rb) - SQR(Rm_old)) / (2.*Rm*Rb));
	mm[0] = 0.;
	Mm[0] = 0.;
	thetam[0] = thetah[0] - acos((SQR(R1h_le) + SQR(Rm_old) - SQR(Rb*(phi1m - phi1h))) / (2.*R1h_le*Rm_old));
	double phim_old = phi1m;
	double dum = (1. - ule_mean) / npt;

	for (int i = 1; i <= npt; i++)
	{
		// For Hub
		pHubContour1->getPoint((ule_hub + i*duh), Zh_now, Rh_now);
		R_h[i] = Rh_now;
		double dmh = sqrt(SQR(Rh_now - Rh_old) + SQR(Zh_now - Zh_old));

		double phih_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rh_now)) / (2.*Rm*Rb));
		double Li2 = 2 * SQR(Rb)*(1 - cos(phih_now - phi1h));
		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetah[i] = acos((SQR(R1h_le) + SQR(Rh_now) - Li2) / (2.*R1h_le*Rh_now));
		}
		else
		{
			thetah[i] = thetah[0] - acos((SQR(R1h_le) + SQR(Rh_now) - Li2) / (2.*R1h_le*Rh_now));
		}

		mh[i] = mh[i - 1] + dmh;
		double dM = sqrt(1 + SQR((Zh_now-Zh_old)/(Rh_now-Rh_old)))*log(Rh_now/Rh_old);
		//double dM = log(Rh_now / Rh_old);
		Mh[i] = Mh[i - 1] + dM;
		//Mh[i] = Mh[i - 1] + 2.*dmh / (Rh_now + Rh_old);
		phih_old = phih_now;
		Rh_old = Rh_now;
		Zh_old = Zh_now;
		// For Shroud
		pShroudContour->getPoint((ule_shroud + i*dus), Zs_now, Rs_now);
		R_s[i] = Rs_now;
		double dms = sqrt(SQR(Rs_now - Rs_old) + SQR(Zs_now - Zs_old));

		double phis_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rs_now)) / (2.*Rm*Rb));
		Li2 = 2 * SQR(Rb)*(1 - cos(phis_now - phi1h));

		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetas[i] = acos((SQR(R1h_le) + SQR(Rs_now) - Li2) / (2.*R1h_le*Rs_now));
		}
		else
		{
			thetas[i] = thetah[0] - acos((SQR(R1h_le) + SQR(Rs_now) - Li2) / (2.*R1h_le*Rs_now));
		}
		
		ms[i] = ms[i - 1] + dms;
		dM = sqrt(1 + SQR((Zs_now - Zs_old) / (Rs_now - Rs_old)))* log(Rs_now /Rs_old);
		Ms[i] = Ms[i - 1] + dM;
		//Ms[i] = Ms[i - 1] + 2.*dms / (Rs_now + Rs_old);
		phis_old = phis_now;
		Rs_old = Rs_now;
		Zs_old = Zs_now;
		// For Mean
		pMeanContour->getPoint((ule_mean + i*dum), Zm_now, Rm_now);
		R_m[i] = Rm_now;
		double dmm = sqrt(SQR(Rm_now - Rm_old) + SQR(Zm_now - Zm_old));

		double phim_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rm_now)) / (2.*Rm*Rb));
		Li2 = 2 * SQR(Rb)*(1 - cos(phim_now - phi1h));

		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetam[i] = acos((SQR(R1h_le) + SQR(Rm_now) - Li2) / (2.*R1h_le*Rm_now));
		}
		else
		{
			thetam[i] = thetah[0] - acos((SQR(R1h_le) + SQR(Rm_now) - Li2) / (2.*R1h_le*Rm_now));
		}

		mm[i] = mm[i - 1] + dmm;
		dM = sqrt(1 + SQR((Zm_now - Zm_old) / (Rm_now - Rm_old)))* log(Rm_now / Rm_old);
		Mm[i] = Mm[i - 1] + dM;
		phim_old = phim_now;
		Rm_old = Rm_now;
		Zm_old = Zm_now;
	}

	betah[0] = blade->hubSection->beta1b;
	betas[0] = atan((thetas[1] - thetas[0]) / (Ms[1] - Ms[0]));
	betam[0] = atan((thetam[1] - thetam[0]) / (Mm[1] - Mm[0]));
	for (int i = 1; i < npt; i++)
	{
		betah[i] = atan((thetah[i + 1] - thetah[i - 1]) / (Mh[i + 1] - Mh[i - 1]));
		betas[i] = atan((thetas[i + 1] - thetas[i - 1]) / (Ms[i + 1] - Ms[i - 1]));
		betam[i] = atan((thetam[i + 1] - thetam[i - 1]) / (Mm[i + 1] - Mm[i - 1]));
	}
	betah[npt] = blade->hubSection->beta2b;
	betas[npt] = atan((thetas[npt] - thetas[npt - 1]) / (Ms[npt] - Ms[npt - 1]));
	betam[npt] = atan((thetam[npt] - thetam[npt - 1]) / (Mm[npt] - Mm[npt - 1]));

	int nctrl = 7;
	pHubBeta->fitBezier(mh, betah, (npt+1), nctrl, 1, 3, 0);
	pShroudBeta->fitBezier(ms, betas, (npt + 1), nctrl, 1, 3, 0);
	pMeanBeta->fitBezier(mm, betam, (npt + 1), nctrl, 1, 3, 0);
    pMeanBeta->Average(pHubBeta, pShroudBeta);


	// update axial blade angle
	blade->tipSection->beta1b = betas[0];
	blade->tipSection->beta2b = betas[npt];
	blade->getMeanSection()->beta1b = betam[0];
	blade->getMeanSection()->beta2b = betam[npt];

	// update LE geometry
	pHubContour1->getPoint(blade->ule_hub, Z1h_le, R1h_le);
	pShroudContour->getPoint(blade->ule_shroud, Z1s_le, R1s_le); 
	double delta_Rle = R1s_le - R1h_le;
	double delta_Zle = Z1h_le - Z1s_le;
	double Lle = sqrt(SQR(delta_Rle) + SQR(delta_Zle));
	sta_le->phi = 0.5*PI-atan(delta_Rle / delta_Zle);
	sta_le->b = Lle;
	sta_le->Ra = 0.5*(R1h_le + R1s_le);
	sta_le->Za = 0.5*(Z1h_le + Z1s_le);
	sta_le->Rh = R1h_le;
	sta_le->Rs = R1s_le;
	sta_le->geomOption = 0;
	sta_le->RecalcGeom();

	in->RecalcGeom();
	AeA0 = in->Area_geom / sta_le->Area_geom;
}

void T1dImpeller::getZRForBladeCompistions(
	int position, // 0. LE - > 1. turing axial to flat -> 2.tring flat->sca
	QVector<Double2> &ZRs, // size of ZRs will be ?
	QVector<double>& us)
{
	ZRs.clear();
	ZRs.resize(3);
	// Calculate the Radius at shroud
	double phi_centre = phi2_shroud;
	double Rc_scenter = in->Rs + RCs;
	// Calculate the R of Radius
	double temp_R = Rc_scenter - RCs * cos(phi_centre);
	double delta_R = out->Rs - temp_R;
	double delta_Z = delta_R * tan(0.5 * PI - phi_centre);
	double Zc_scenter = out->Zs - delta_Z - RCs * sin(phi_centre);
	double tZ = (Zc_scenter - in->Zs) / (out->Zs - in->Zs);
	double tR = (Rc_scenter - in->Rs) / (out->Rs - in->Rs);

	QVector<TNurbsCurve*> pZRCurves 
		= QVector<TNurbsCurve*>() << pHubContour1 << pMeanContour << pShroudContour;

	TNurbsCurve* pLine = new TNurbsCurve;
	if (position == 0)
	{
		pHubContour1->getPoint(ule_hub, ZRs[0][0], ZRs[0][1]);
		pShroudContour->getPoint( ule_shroud, ZRs[2][0], ZRs[2][1]);
		{
			{
				QVector<double> pZ(2); //double* pZ = new double[2]; 
				QVector<double> pR(2); //double* pR = new double[2];
				for (int k = 0; k < 2; k++)
				{
					int j = 0;
					if (k == 1) j = 2;
					pZ[k] = ZRs[j][0];
					pR[k] = ZRs[j][1];
				}
				pLine->fitBezier(pZ.data(), pR.data(), 2, 2);
			}
			QVector<Double2> pIntersections;
			int err = pLine->Intersection_between2Curves(pZRCurves[1], pIntersections);
			if (pIntersections.size() != 0)
				ZRs[1] = pIntersections[0];
			else
			{
				eprintf("can't find intersection between QN with ZR curve of meansection ");
				ZRs[1] = (ZRs[0] + ZRs[2]) / 2.;
			}
		}
	}
	else if (position == 1)
	{
		double Z = (1 - tZ) * in->Zs + tZ * out->Zs;
		for (int i = 0; i < 3; i++)
		{
			// get R
			bool found_Z = false;
			double u_Z = pZRCurves[i]->getUfromX(Z, found_Z);
			if (found_Z)
				pZRCurves[i]->getPoint(u_Z, ZRs[i][0], ZRs[i][1]);
			else
				eprintf("can't found Z.");
		}
	}
	else if (position == 2)
	{
		double R = (1 - tR) * in->Rs + tR * out->Rs;
		for (int i = 0; i < 3; i++)
		{
			// get Z
			bool found_R = false;
			double u_R = pZRCurves[i]->getUfromY(R, found_R);
			if (found_R)
				pZRCurves[i]->getPoint(u_R, ZRs[i][0], ZRs[i][1]);
			else
				eprintf("can't found R.");
		}
	}
	else if (position == 3)
	{
		pHubContour1->getPoint(ute_hub, ZRs[0][0], ZRs[0][1]);
		pShroudContour->getPoint(ute_shroud, ZRs[2][0], ZRs[2][1]);
		{
			{
				QVector<double> pZ(2); //double* pZ = new double[2]; 
				QVector<double> pR(2); //double* pR = new double[2];
				for (int k = 0; k < 2; k++)
				{
					int j = 0;
					if (k == 1) j = 2;
					pZ[k] = ZRs[j][0];
					pR[k] = ZRs[j][1];
				}
				pLine->fitBezier(pZ.data(), pR.data(), 2, 2);
			}
			QVector<Double2> pIntersections;
			int err = pLine->Intersection_between2Curves(pZRCurves[1], pIntersections);
      if (pIntersections.size() != 0)
        ZRs[1] = pIntersections[0];
      else
      {
        eprintf("can't find intersection between QN with ZR curve of meansection ");
        ZRs[1] = (ZRs[0] + ZRs[2]) / 2.;
      }
		}
  }

	us.resize(3);
  for (int i = 0; i < 3; i++)
  {
    double Z = ZRs[i][0];
    double R = ZRs[i][1];
    bool found_Z = false;
    bool found_R = false;
    double u_Z = pZRCurves[i]->getUfromX(Z, found_Z);
		double tan_phi_Z = 0.;
		pZRCurves[i]->getSlope(u_Z, tan_phi_Z);
    double u_R = pZRCurves[i]->getUfromY(R, found_R);
		double tan_phi_R = 0.;
		pZRCurves[i]->getSlope(u_R, tan_phi_R);
    if (!found_Z && !found_R)
      eprintf("can't found ZR");
		// check
    if (found_Z && found_R)
    {
			Double2 pt_foundZ;
			pZRCurves[i]->getPoint(u_Z, pt_foundZ[0], pt_foundZ[1]);
			double distance = (pt_foundZ - ZRs[i]).length();
			if (distance > 1.E-5)
				found_Z = false;
			Double2 pt_foundR;
			pZRCurves[i]->getPoint(u_R, pt_foundR[0], pt_foundR[1]);
			distance = (pt_foundR - ZRs[i]).length();
			if (distance > 1.E-5)
				found_R = false;
    }
    if (found_Z && abs(tan_phi_Z) <1.)
			us[i] = u_Z;
    else if (found_R && abs(tan_phi_R) > 1.)
			us[i] = u_R;
  }

	delete pLine;
}

void T1dImpeller::UpdateBetaWithCompistions()
{
	// MVR is specifial machine 
	// The compistions of blade are 3 blades;
	//	blade_1. free axial
	//	blade_2: 2D Radial
	//	blade_3: Radial Blade
	blade->bladeType = 6;
	int np = 51;
	QVector<QVector<Double2>> ZRs; ZRs.resize(4);
	QVector<QVector<double>> us; us.resize(4);
	for (int i = 0; i < 4; i++)
	{
		int posion = i;
		getZRForBladeCompistions(posion, ZRs[i], us[i]);
	}

	int spans = 3;

	int num_bladeEdge_axial = 2;
	QVector<TNurbsCurve*> pZRCurves = QVector<TNurbsCurve*>() << pHubContour1 << pMeanContour << pShroudContour;
	QVector<TNurbsCurve*> pBetaCurves_freeAxial; pBetaCurves_freeAxial.resize(3);
	QVector<double> betab_base_freeAxial; // bladeMetaAngle based
	double wrapAngle_base_freeAxial = 0.; // blade wrap Angle based
	QVector<QVector<double>> betab_edge_freeAxial; betab_edge_freeAxial.resize(spans); //bladeMetaAngle of edge
	QVector<QVector<Double2>> ZR_edge_freeAxial; //ZR coordinate of edge
	QVector<QVector<Double2>> mbeta_freeAxial; mbeta_freeAxial.resize(spans);// Beta Curve
	QVector<QVector<Double2>> ZRs_freeAxial; ZRs_freeAxial.resize(spans); // ZR Curve
	QVector<QVector<double>> us_axial = QVector<QVector<double>>() << us[0] << us[1];
	// setting
	{
		betab_base_freeAxial.resize(num_bladeEdge_axial);
		betab_base_freeAxial[0] = blade->tipSection->beta1b;
		betab_base_freeAxial[1] = 0.;
		wrapAngle_base_freeAxial = -15. * PI / 180;
		if (!isUserInput(blade->tipSection, "wrapAngle"))
			blade->tipSection->wrapAngle = -15. * PI / 180;
		wrapAngle_base_freeAxial = blade->tipSection->wrapAngle;
		ZR_edge_freeAxial.push_back(ZRs[0]);
		ZR_edge_freeAxial.push_back(ZRs[1]);
		for (int i = 0; i < pBetaCurves_freeAxial.size(); i++)
			if (!pBetaCurves_freeAxial[i])
				pBetaCurves_freeAxial[i] = new TNurbsCurve;
	}

	int npt_Axial = 21;
	double cantAngle = 0.;
	int span_specified = 2;
	UpdateBetaWithFreeAxial_base(
		pZRCurves,
		pBetaCurves_freeAxial,
		betab_base_freeAxial,
		wrapAngle_base_freeAxial,
		us_axial,
		ZR_edge_freeAxial,
		// will be got
		betab_edge_freeAxial,
		mbeta_freeAxial,
		ZRs_freeAxial,
		cantAngle,
		npt_Axial,
		span_specified
	);

	// blade_2: 2D flat Radial
	int num_edg_flat = 2;
	QVector<TNurbsCurve*> pBetaCurves_Flat; pBetaCurves_Flat.resize(3);
	int npt_flat = 11;
	QVector<QVector<double>> u_edge_flat; u_edge_flat.resize(spans);
	QVector<QVector<Double2>> mbeta_flat; mbeta_flat.resize(spans); //
	QVector<QVector<double>> betab_flat;  betab_flat.resize(spans);//HMT LE & TE
	QVector<QVector<Double2>> ZR_flat; ZR_flat.resize(spans);//HMT LE & TE
	// setting blade
	{

		for (int i = 0; i < spans; i++)
		{
			u_edge_flat[i].resize(2);
			u_edge_flat[i][0] = us[1][i];
			u_edge_flat[i][1] = us[2][i];
		}
		for (int i = 0; i < spans; i++)
			if (!pBetaCurves_Flat[i])
				pBetaCurves_Flat[i] = new TNurbsCurve;
	}

	UpdateBeta_Flat_Radial_pure(
		pZRCurves,
		u_edge_flat,
		pBetaCurves_Flat, 
		betab_flat,
		mbeta_flat, 
		ZR_flat,
		npt_flat
	);

	//blade_3:  SCA Radial Blade
	QVector<double> betab_base_SCA; betab_base_SCA.resize(2);
	QVector<QVector<double>> betab_edge_SCA; betab_edge_SCA.resize(spans);
	QVector<QVector<double>> theta_edge_SCA; theta_edge_SCA.resize(spans);
	QVector<QVector<Double2>> ZR_base_SCA; ZR_base_SCA.resize(spans);
	QVector<QVector<Double2>> ZR_SCA; ZR_SCA.resize(spans);
	QVector<QVector<Double2>> mbeta_SCA; mbeta_SCA.resize(spans);
	QVector<TNurbsCurve*> pBetaCurves_SCA; pBetaCurves_SCA.resize(spans);
	// setting blade
	{
		betab_base_SCA[0] = 0.;
		betab_base_SCA[1] = blade->hubSection->beta2b;
		// ZR_base_SCA
		for (int i = 0; i < spans; i++)
		{
			ZR_base_SCA[i].resize(2);
			ZR_base_SCA[i][0] = ZRs[2][i];
			ZR_base_SCA[i][1] = ZRs[3][i];
			if (!pBetaCurves_SCA[i])
				pBetaCurves_SCA[i] = new TNurbsCurve; // will be delete
		}

	}

	int npt_SCA = 21;
	UpdateBeta_SCA_Radial_base(
		pBetaCurves_SCA, 
		betab_base_SCA, 
		betab_edge_SCA,
		ZR_base_SCA, 
		mbeta_SCA, 
		ZR_SCA, 
		theta_edge_SCA,
		npt_SCA
	);

  // compisition these blades
	// 1.
	QVector<QVector<Double2>> ZR_HMT_blade; ZR_HMT_blade.resize(3);
	QVector<QVector<Double2>> mBeta_HMT_blade; mBeta_HMT_blade.resize(3);
	for (int j = 0; j < spans; j++)
	{
		ZR_HMT_blade[j].resize(NUMPTS);
		mBeta_HMT_blade[j].resize(NUMPTS);
		// blade1
		int index = 0;
		for (int i1 = 0; i1<ZRs_freeAxial[j].size(); i1++)
		{
			mBeta_HMT_blade[j][index] = mbeta_freeAxial[j][i1];
			ZR_HMT_blade[j][index] = ZRs_freeAxial[j][i1];
			index = index + 1;
		}
		double m_start = mBeta_HMT_blade[j][index-1][0];
		// blade2
		for (int i2 = 1; i2 < ZR_flat[j].size(); i2++)
		{
			mBeta_HMT_blade[j][index] = mbeta_flat[j][i2] + Double2(m_start, 0.);
			ZR_HMT_blade[j][index] = ZR_flat[j][i2];
			index = index + 1;
		}
		m_start = mBeta_HMT_blade[j][index - 1][0];
		// blade3
		for (int i3 = 1; i3 < ZR_SCA[j].size(); i3++)
		{
			mBeta_HMT_blade[j][index] = mbeta_SCA[j][i3] + Double2(m_start, 0.);
			ZR_HMT_blade[j][index] = ZR_SCA[j][i3];
			index = index + 1;
		}
	}
	QVector<TNurbsCurve*> pBetaCurves = QVector<TNurbsCurve*>()
		<< pHubBeta << pMeanBeta << pShroudBeta;

	TNurbsCurve* pCurve = new TNurbsCurve;
	for (int j = 0; j < spans; j++)
	{
		Double2 pt_close;
		pBetaCurves_freeAxial[j]->getPoint(1., pt_close[0], pt_close[1]);
		pCurve->CopyFrom(pBetaCurves_freeAxial[j]);
		pCurve->Concatenate(pBetaCurves_freeAxial[j], pBetaCurves_Flat[j], pt_close);
		//
		pCurve->getPoint(1., pt_close[0], pt_close[1]);
		pBetaCurves[j]->Concatenate(pCurve, pBetaCurves_SCA[j], pt_close);
	}
	
	blade->stackingType = blade->stacking_Option::Stacking_TE;
	delete pCurve;

	// set meta blade angle
	for (int j = 0; j < spans; j++)
	{
		// set LE meta blade angle
		blade->pBldSect[j]->beta1b = betab_edge_freeAxial[j][0];
		// set TE meta blade angle
		blade->pBldSect[j]->beta2b = betab_edge_SCA[j][1];
	}

	// set  blade theta angle
	for (int j = 0; j < spans; j++)
	{
		// set TE theta angle
		blade->pBldSect[j]->Theta2 = theta_edge_SCA[j].last();
	}
	blade->stackingType = blade->stacking_Option::Stacking_TE;
	blade->updateStackingTangential();
	return;
}

void T1dImpeller::UpdateBetaWithFreeRadial(double tZ, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h)
{
	// Solve free Radial of Inlet of MVR 
	// Using Comformal mapping to calculate beta distribution
	// Just need to define beta distribution of Shroud; the hub can be calculated by Free Radial


  if (!isUserWrapAngleDefine)
  {
    UpdateBetaWithFreeRadial_quadratic(tZ, npt, m_s, m_h, beta_s, beta_h);
    return;
  }

	int np = 21;
	double Z1_end = tZ*out->Zs + (1 - tZ)*in->Zs;
	double Z1_start = in->Zs;
	double M[21] = { 0. };

	if (!isUserInput(blade->tipSection, "wrapAngle"))
	{
		blade->tipSection->wrapAngle = -15.*PI / 180;
	}
	double WAtip = blade->tipSection->wrapAngle;
	blade->hubSection->wrapAngle = WAtip;
	blade->getMeanSection()->wrapAngle = WAtip;
	double theta = WAtip;

	// Step 1. Intergrate M, Get ponit from Shroud Contour of Segement1
	// Shroud
	bool found = false;
	double Zi_1 = in->Zs; // is R_(i-1)
	double Ri_1 = in->Rs;
	double Zi = 0, Ri = 0; // is R_(i)
	double Zs[21] = { 0. };
	double Rs[21] = { 0. };
	Zs[0] = Zi_1;
	Rs[0] = Ri_1;

	// Hub
	double Zh[21] = { 0. };
	double Rh[21] = { 0. };
	Zh[0] = in->Zh;
	Rh[0] = in->Rh;
	double Zi_h = 0., Ri_h = 0.;

	// RMS
	double Zm[21] = { 0. };
	double Rm[21] = { 0. };
	Zm[0] = 0.5*(in->Zh+in->Zs);
	Rm[0] = 0.5*(in->Rh+in->Rs);
	double Zi_m = 0., Ri_m = 0.;
	double m_m[21] = { 0. };
	double beta_m[21] = {0.};

	double delata_Z = (Z1_end - Z1_start) / (np - 1);

	for (int i = 1; i < np; i++)
	{
		Zi = Z1_start + i*delata_Z;
		Zi_h = Zi;
		double ui_1 = pShroudContour->getUfromX(Zi_1, found);
		if (found)
		{
			; // Handel the other conditions
		}
		double ui = pShroudContour->getUfromX(Zi, found);
		pShroudContour->getPoint(ui, Zi, Ri);

		double Zi_0p5 = 0.5*(Zi_1 + Zi); //_0p5 is mean i-0.5 between  i-1 with i.
		double Ri_0p5 = 0.5*(Ri_1 + Ri);
		double dmi = sqrt(SQR(Zi - Zi_1) + SQR(Ri - Ri_1));
		m_s[i] = m_s[i - 1] + dmi;
		double dMi = dmi / Ri_0p5;
		M[i] = M[i - 1] + dMi;

		// For next loop, update[i-1], [i-0.5], [i]
		ui_1 = ui;
		Zi_1 = Zi;
		Ri_1 = Ri;
		Zs[i] = Zi;
		Rs[i] = Ri;

		// For Hub
		double ui_h = pHubContour1->getUfromX(Zi, found);
		pHubContour1->getPoint(ui_h, Zi_h, Ri_h);
		Zh[i] = Zi_h;
		Rh[i] = Ri_h;
		double dmi_h = sqrt(SQR(Zh[i] - Zh[i - 1]) + SQR(Rh[i] - Rh[i - 1]));
		m_h[i] = m_h[i - 1] + dmi_h;
		// 
		// For RMS
		double ui_m = pMeanContour->getUfromX(Zi, found);
		pMeanContour->getPoint(ui_m, Zi_m, Ri_m);
		Zm[i] = Zi_m;
		Rm[i] = Ri_m;
		double dmi_m = sqrt(SQR(Zm[i] - Zm[i - 1]) + SQR(Rm[i] - Rm[i - 1]));
		m_m[i] = m_m[i - 1] + dmi_m;
		// 
	}
	// Step 2. Define 3-order bezier
	double Matrix[4][4] = { { -1., 3., -3., 1. },{ 3., -6., 3., 0. },{ -3., 3., 0., 0. },{ 1., 0, 0., 0. } };
	double dMatrix[4][4] = { { -3., 9., -9., 3. },{ 6., -12., 6., 0. },{ -3., 3., 0., 0. },{ 0., 0., 0., 0. } };

	// Set Control points 
	double beta1s = blade->tipSection->beta1b;
	double beta2s = -0. * PI / 180;
	double MC[4] = { 0. };
	double ThetaC[4] = { 0. };

#if 1
	double kFacotr1 = 0.5, kFacotr2 = 1.;
	if (!isUserInput(blade->tipSection, "kFactor1"))
	{
		blade->tipSection->kFactor1 = 0.5;
	}
	kFacotr1 = blade->tipSection->kFactor1;

	if (!isUserInput(blade->tipSection, "kFactor2"))
	{
		blade->tipSection->kFactor2 = 1.;
	}
	kFacotr2 = blade->tipSection->kFactor2;

	bool specifyX = true;
	double M_insert = 0.;
	if (M[20] > theta)
	{
		M_insert = theta / tan(beta1s);
		MC[1] = kFacotr1*M_insert;
		ThetaC[1] = kFacotr1*theta;
	}
	else
	{
		specifyX = false; // Specify Y
		ThetaC[1] = theta;
		MC[1] = ThetaC[1] / tan(beta1s);
	}
	// 3rd control point coor-
	if (specifyX)
	{
		MC[2] = 0.5*(M_insert+ M[20]);
	}
	else
	{
		MC[2] = 1. / 2 * M[20];
	}
	MC[2] = kFacotr2*M_insert+(1- kFacotr2)*M[20]; //
	ThetaC[2] = theta;
#else
	//2nd control point coor-
	bool specifyX = true;
	MC[1] = 1. / 4 * M[20]; //
	ThetaC[1] = MC[1] * tan(beta1s);
	if (fabs(ThetaC[1]) > fabs(theta) )
	{
		specifyX = false; // Specify Y
		ThetaC[1] = theta;
		MC[1] = ThetaC[1]/tan(beta1s);
	}

	// 3rd control point coor-
	MC[2] = 1. / 2 * M[20];
	ThetaC[2] = theta;
#endif
	// 4th control point coor-
	MC[3] = M[20];
	ThetaC[3] = theta;

	// Step 3. Get  slop of fitting M-theta Bezier curve
	double t[21] = { 0. };
	double Theta_s[21] = { 0. };
	double M_s[21] = { 0. };
	double Ms_mean[21] = { 0. };
	double Matrix_Mi[4] = { 0. };
	double Matrix_Thetai[4] = { 0. };
	// Have M to search t to find Beta at M-Theta curve 
	for (int k = 0; k < 4; k++) // Row
	{
		for (int h1 = 0; h1 < 4; h1++) // Cloumn
		{
			Matrix_Mi[k] = Matrix_Mi[k] + Matrix[k][h1] * MC[h1];
			Matrix_Thetai[k] = Matrix_Thetai[k] + Matrix[k][h1] * ThetaC[h1];
		}
	}

	for (int i = 1; i < np - 1; i++)
	{
		double t_start = 0.;
		double t_mean = 0.5;
		double t_end = 1.0;

		for (int j = 0; j < 50; j++)
		{
			double M_start = 0., M_mean = 0., M_end = 0.;
			for (int h2 = 0; h2 < 4; h2++) // Cloumn
			{
				double n = 3. - h2;
				M_start = M_start + pow(t_start, n)*Matrix_Mi[h2];
				M_mean = M_mean + pow(t_mean, n)*Matrix_Mi[h2];
				M_end = M_end + pow(t_end, n)*Matrix_Mi[h2];
			}
			double delta_Mi = fabs((M_mean - M[i]) / M[i]);
			if (delta_Mi < 1.E-3)
			{
				t[i] = t_mean;
				Ms_mean[i] = M_mean;
				break;
			}
			// judge the relation between M[i] with Boundary
			double t_temp = 0.;
			if (M_mean < M[i])
			{
				t_temp = 0.5*(t_mean + t_end);
				t_start = t_mean;
				t_mean = t_temp;
			}
			else
			{
				t_temp = 0.5*(t_start + t_mean);
				t_end = t_mean;
				t_mean = t_temp;
			}
		}
		// 
		for (int h2 = 0; h2 < 4; h2++) // Cloumn
		{
			double n = 3. - h2;
			Theta_s[i] = Theta_s[i] + pow(t[i], n)*Matrix_Thetai[h2];
			M_s[i] = M_s[i] + pow(t[i], n)*Matrix_Mi[h2];
		}
	}
	Theta_s[20] = theta;
	t[20] = 1.;

	// Step 4. Calculate the beta of Shroud
	//	double beta_s[21];
	double t_dMatrix[4] = { 0. };
	double dMatrix_Mi[4] = { 0. };
	double dMatrix_Thetai[4] = { 0. };
	for (int k = 0; k < 4; k++) // Row
	{
		for (int h1 = 0; h1 < 4; h1++) // Cloumn
		{
			dMatrix_Mi[k] = dMatrix_Mi[k] + dMatrix[k][h1] * MC[h1];
			dMatrix_Thetai[k] = dMatrix_Thetai[k] + dMatrix[k][h1] * ThetaC[h1];
		}
	}

	for (int i = 1; i < np-1; i++)
	{
		double dTheta_dt = 0.;
		double dM_dt = 0.;
		for (int j1 = 0; j1 < 4; j1++)
		{
			int n0 = (4 - j1 - 2);
			double n = 1.*n0; // need to be conformed
			double t_coeff = 1.;
			if (n0 < 0)
			{
				t_coeff = 0.;
				n = 0.;
			}
			dTheta_dt = dTheta_dt + t_coeff*pow(t[i], n)*dMatrix_Thetai[j1];
			dM_dt = dM_dt + t_coeff*pow(t[i], n)*dMatrix_Mi[j1];
		}
		beta_s[i] = atan(dTheta_dt / dM_dt);
	}
	beta_s[0] = beta1s;
	beta_s[20] = 0.;
	// Step 5. set point of Hub according the shroud setting with equal Z
	// We get Beta of shroud
	for (int i = 0; i < 21; i++)
	{
		beta_h[i] = atan(tan(beta_s[i])*Rh[i] / Rs[i]);
		beta_m[i] = atan(tan(beta_s[i])*Rm[i] / Rs[i]);
	}

	// Update LE blade angle
	blade->hubSection->beta1b = beta_h[0];
	blade->getMeanSection()->beta1b = beta_m[0];
	blade->tipSection->beta1b = beta_s[0];
#if 0
	ofstream ZR_file, beta_file;
	ZR_file.open("D:\\ZR1.csv", ios::out | ios::trunc);
	beta_file.open("D:\\beta1.csv", ios::out | ios::trunc);
	ZR_file << "" << "," << "Zs" << "," << "Rs[i]" << "," << "Zh[i]" << "," << "Rh[i]" << endl;
	beta_file << "" << "," << "m_s[i]" << "," << "beta_s[i]" << "," << "mm[i]" << "," << "beta_m[i]" << "," << "mh[i]" << "," << "beta_h[i]" << "," << "M[i]" << "," << "Theta_s[i]" << endl;
	for (int i = 0; i< 4; i++)
	{
		beta_file << i << ", " << MC[i] << "," << ThetaC[i] << endl;
	}

	for (int i = 0; i < 21; i++)
	{
		ZR_file << i << "," << Zs[i] << "," << Rs[i] << "," << Zm[i] << "," << Rm[i] << "," << Zh[i] << "," << Rh[i] << endl;
		beta_file << i << "," << m_s[i] << "," << beta_s[i] << ", " << m_m[i] << "," << beta_m[i] << "," << m_h[i] << "," << beta_h[i] << "," << M[i] << "," << Theta_s[i] << "," << M_s[i] << ", " << Ms_mean[i] << "," << t[i] << endl;
	}

	ZR_file.close();
	beta_file.close();
#endif

}

void T1dImpeller::UpdateBetaWithFreeRadial_quadratic(double tZ, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h)
{
  // Solve free Radial of Inlet of MVR 
  // Just need to define beta distribution of Shroud; the hub can be calculated by Free Radial
  int np = 21;
  double Z1_start = in->Zs, R1_start = 0.;
  pShroudContour->getPoint( ule_shroud,Z1_start, R1_start);
  double Z1_end = tZ * out->Zs + (1 - tZ)*in->Zs;
  bool found = false;
  double uend_shroud = pShroudContour->getUfromX(Z1_end, found);
  if (!found)
    return;

  double R1_end = 0.;
  QVector<Double2> m_rtheta; m_rtheta.resize(np);
  QVector<Double2> ZR_s; ZR_s.resize(np);
  QVector<double> phi_s; phi_s.resize(np);
  double u1 = ule_shroud, u2 = uend_shroud;
  // cant angle is default 0 with radial direction
  // step 1. calculate m value from start to end
  double du = (u2-u1) / (np - 1.);
  for (int i = 0; i < np; i++)
  {
    double ui = u1 + i * du;
    double m = pShroudContour->calculate2DLength(u1, ui);
    double Z = 0., R = 0.;
    pShroudContour->getPoint(ui, Z, R);
    ZR_s[i] = Double2(Z, R);
    m_rtheta[i] = Double2(m, 0.);
    double slop = 0.;
    pShroudContour->getSlope(ui, slop);
    phi_s[i] = atan(slop);

    m_s[i] = m;
  }
  // 1-1. rearrage m
  double m_end = m_rtheta[np - 1][0];
  for (int i = 0; i < np; i++)
  {
    m_rtheta[i][0] -= m_end;
  }

  double beta1 = blade->tipSection->beta1b;
  double beta2 = 0.;

  // step2. get beta distrbution of shroud
  double a = tan(beta1) / (2*m_rtheta[0][0]);
  QVector<double> beta_shroud; beta_shroud.resize(np);
  for (int i = 0; i < np; i++)
  {
    double _rtheta = a*SQR(m_rtheta[i][0]);
    m_rtheta[i][1] = _rtheta;
    double slop = 2 * a* m_rtheta[i][0];
    beta_shroud[i] = atan(slop);
    beta_s[i] = beta_shroud[i];
  }

  /*
    tan_beta = (r*dTheta)/dm;
    tan_beta / r = dTheta/(dz/cos_phi) =  (dTheta*cos_phi) /dz
    For hub, will be got:
    {tan_beta /( r*cos_phi)}_hub = {tan_beta / (r*cos_phi)}_shroud
*/

  // step3. get beta distrbution of hub
  double Z_start_hub = 0.;
  double u1_hub = pHubContour1->getUfromX(Z1_start, found);
  double u2_hub = pHubContour1->getUfromX(Z1_end, found);
  QVector<double> beta_hub;
  QVector<double> m_hub;
  for (int i = 0; i < np; i++)
  {
    double Z_shroud = ZR_s[i][0];
    bool found = false;
    double u = pHubContour1->getUfromX(Z_shroud, found);
    double R_shoud = 0.;
    //
    if (!found)
      break;
    double Z, R = 0.;
    pHubContour1->getPoint(u, Z, R);
    double slop = 0.;
    pHubContour1->getSlope(u, slop);
    double phi_hub = atan(slop);
    // -----------------------
    double R_shroud = ZR_s[i][1];

    double tan_beta =  tan(beta_s[i])/( R_shroud * cos(phi_s[i])) * ( R/ cos(phi_hub) );
    double beta = atan(tan_beta);
    double m = pHubContour1->calculate2DLength(u1_hub, u);
    beta_hub.push_back(beta);
    m_hub.push_back(m);
  }
  for (int i = 0; i < np; i++)
  {
    m_h[i] = m_hub[i];
    beta_h[i] = beta_hub[i];
  }

  // Update LE blade angle
  blade->hubSection->beta1b = beta_h[0];
  blade->getMeanSection()->beta1b = (beta_h[0] + beta_s[0])/2.;
  blade->tipSection->beta1b = beta_s[0];

}

void T1dImpeller::UpdateBetaWith2DRadial(double tZ, double tR, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h)
{
	//Solve 2D Radial of MVR
	// Calculate meridition lenght of the start points at hub and shroud 

	double m_start_h = 0;
	double m_start_m = 0;
	double m_start_s = 0; // This need to calculate by itself, and is passed form Compistions of Blade of Main function.

						  // method is different between station 1 with station 2 
  bool found = false;
	// Station 1;
	double Z1_s = (1. - tZ)*in->Zs + tZ*out->Zs;
	double R1_s = 0; // need to be calculated
	double u1_s = pShroudContour->getUfromX(Z1_s, found);
	pShroudContour->getPoint(u1_s, Z1_s, R1_s);

	double Z1_h = Z1_s;
	double R1_h = 0.; // need to be calculated
	double u1_h = pHubContour1->getUfromX(Z1_h, found);
	pHubContour1->getPoint(u1_h, Z1_h, R1_h);

	double Z1_m = Z1_s;
	double R1_m = 0.; // need to be calculated
	double u1_m = pMeanContour->getUfromX(Z1_m, found);
	pMeanContour->getPoint(u1_m, Z1_m, R1_m);

	// Station 2
	double R2_s = (1. - tR)*in->Rs + tR*out->Rs;
	double Z2_s = 0.; // need to be calculated
	double u2_s = pShroudContour->getUfromY(R2_s, found);
	pShroudContour->getPoint(u2_s, Z2_s, R2_s);

	double R2_h = R2_s;
	double Z2_h = 0.; // need to be calculated
	double u2_h = pHubContour1->getUfromY(R2_h, found);
	pHubContour1->getPoint(u2_h, Z2_h, R2_h);

	double R2_m = R2_s;
	double Z2_m = 0.; // need to be calculated
	double u2_m = pMeanContour->getUfromY(R2_m, found);
	pMeanContour->getPoint(u2_m, Z2_m, R2_m);

	double du_h = (u2_h - u1_h) / (npt - 1);
	double du_m = (u2_m - u1_m) / (npt - 1);
	double du_s = (u2_s - u1_s) / (npt - 1);

	double Zi_1s = Z1_s;
	double Ri_1s = R1_s;
	
	double Zi_1m = Z1_s;
	double Ri_1m = R1_s;

	double Zi_1h = Z1_h;
	double Ri_1h = R1_h;

	m_s[0] = 0.;
	m_h[0] = 0.;
	double m_m[11] = { 0. };
	double beta_m[11] = { 0. };
	// This loop is focus to calculate the merdition length
	for (int i = 1; i < npt; i++)
	{
		double ui_h = u1_h + i*du_h;
		double ui_m = u1_m + i*du_m;
		double ui_s = u1_s + i*(du_s);
		double Zi = 0., Ri = 0.;
		// Shroud
		pShroudContour->getPoint(ui_s, Zi, Ri);
		double dm = sqrt(SQR(Zi - Zi_1s) + SQR(Ri - Ri_1s));
		m_s[i] = m_s[i - 1] + dm;
		// Update
		Zi_1s = Zi;
		Ri_1s = Ri;
		// hub
		pHubContour1->getPoint(ui_h, Zi, Ri);
		dm = sqrt(SQR(Zi - Zi_1h) + SQR(Ri - Ri_1h));
		m_h[i] = m_h[i - 1] + dm;
		// Update
		Zi_1h = Zi;
		Ri_1h = Ri;
		// Mean
		pMeanContour->getPoint(ui_m, Zi, Ri);
		dm = sqrt(SQR(Zi - Zi_1m) + SQR(Ri - Ri_1m));
		m_m[i] = m_m[i - 1] + dm;
		// Update
		Zi_1m = Zi;
		Ri_1m = Ri;
		// Force beta is equal to 0.
		beta_h[i] = 0;
		beta_m[i] = 0;
		beta_s[i] = 0;
	}

#if 0
	ofstream beta_file2;
	beta_file2.open("D:\\beta2.csv", ios::out | ios::trunc);
	beta_file2 << "" << "," << "ms[i]" << "," << "betas[i]" << "," << "mh[i]" << "," << "betah[i]" << endl;
	for (int i = 0; i < npt; i++)
	{
		beta_file2 << i << "," << m_s[i] << "," << beta_s[i] << "," << m_m[i] << "," << beta_m[i] << "," << m_h[i] << "," << beta_h[i] << endl;
	}
	beta_file2.close();
#endif
}

void T1dImpeller::UpdateBetaWith2DAxial(double tR, int npt, double *m_s, double *m_h, double *beta_s, double *beta_h)
{
	//Solve 2D Axial of MVR
	// Calculate meridition lenght of the start points at hub and shroud 
	double m_start_h = 0; 
	double m_start_m = 0;
	double m_start_s = 0; // This need to calculate by itself, and is passed form Compistions of Blade of Main function.

	// method is different between station 1 with station 2 
	bool found = false;
	// Station 1;
	double R1_s = (1. - tR)*in->Rs + tR*out->Rs; // need to be calculated
	double Z1_s = 0; // need to be calculated
	double u1_s = pShroudContour->getUfromY(R1_s, found);
	pShroudContour->getPoint(u1_s, Z1_s, R1_s);

	double R1_h = R1_s;
	double Z1_h = 0.; // need to be calculated
	double u1_h = pHubContour1->getUfromY(R1_h, found);
	pHubContour1->getPoint(u1_h, Z1_h, R1_h);

	double R1_m = R1_s;
	double Z1_m = 0.; // need to be calculated
	double u1_m = pMeanContour->getUfromY(R1_m, found);
	pMeanContour->getPoint(u1_m, Z1_m, R1_m);

	// Station 2
	double Z2_s = out->Zs;
	double R2_s = out->Rs;
	double u2_s = 1.;

	double R2_h = out->Rh;
	double Z2_h = out->Zh; // need to be calculated
	double u2_h = 1.;

	double R2_m = out->Ra;
	double Z2_m = out->Za; // need to be calculated
	double u2_m = 1.;

	double du_h = (u2_h - u1_h) / (npt - 1);
	double du_m = (u2_m - u1_m) / (npt - 1);
	double du_s = (u2_s - u1_s) / (npt - 1);

	double Zi_1s = Z1_s;
	double Ri_1s = R1_s;
	double Zi_1m = Z1_m;
	double Ri_1m = R1_m;
	double Zi_1h = Z1_h;
	double Ri_1h = R1_h;

	double m_m[21] = { 0. };
	double beta_m[21] = {0.};

	m_h[0] = 0.;
	m_m[0] = 0.;
	m_s[0] = 0.;

	// This loop is focus to calculate the merdition length
	for (int i = 1; i < npt; i++)
	{
		double ui_h = u1_h + i*du_h;
		double ui_m = u1_m + i*du_m;
		double ui_s = u1_s + i*du_s;
		double Zi = 0., Ri = 0.;
		// Shroud
		pShroudContour->getPoint(ui_s, Zi, Ri);
		double dm = sqrt(SQR(Zi - Zi_1s) + SQR(Ri - Ri_1s));
		m_s[i] = m_s[i - 1] + dm;
		// Update
		Zi_1s = Zi;
		Ri_1s = Ri;

		// hub
		pHubContour1->getPoint(ui_h, Zi, Ri);
		dm = sqrt(SQR(Zi - Zi_1h) + SQR(Ri - Ri_1h));
		m_h[i] = m_h[i - 1] + dm;
		// Update
		Zi_1h = Zi;
		Ri_1h = Ri;
		// mean
		pMeanContour->getPoint(ui_m, Zi, Ri);
		dm = sqrt(SQR(Zi - Zi_1m) + SQR(Ri - Ri_1m));
		m_m[i] = m_m[i - 1] + dm;
		// Update
		Zi_1m = Zi;
		Ri_1m = Ri;
		// Force beta is equal to 0.
		beta_h[i] = 0.;
		beta_m[i] = 0.;
		beta_s[i] = 0.;
	}


	// Update TE blade angle
	//blade->hubSection->beta2b = 0.;
	blade->getMeanSection()->beta2b = 0.;
	blade->tipSection->beta2b = 0.;

#if 0
	ofstream beta_file3;
	beta_file3.open("D:\\beta3.csv", ios::out | ios::trunc);
	beta_file3 << "" << "," << "ms[i]" << "," << "betas[i]" << "," << "mh[i]" << "," << "betah[i]" << endl;
	for (int i = 0; i < npt; i++)
	{
		beta_file3 << i << "," << m_s[i] << "," << beta_s[i] << "," << m_h[i] << "," << beta_h[i] << endl;
	}
	beta_file3.close();
#endif
}

void T1dImpeller::UpdateBetaWith2DAxial_Cycle(double tR, int npt1, double *m_s, double *m_h, double *beta_s, double *beta_h)
{
	// 2D Axial blade just for Straight or Cycle
	double beta2 = blade->hubSection->beta2b;
	const int npt = 20;
	double beta1_h = 0.;
	double beta2_h = blade->hubSection->beta2b;
	if (beta2 <= 0)
	{
		beta1_h = 0. + PI / 2.;
		beta2_h = blade->hubSection->beta2b + PI / 2.;
	}
	else
	{
		beta1_h = 0. + PI / 2.;
		beta2_h = PI / 2 - blade->hubSection->beta2b;
	}

	//double R2 = in->Rs*0.7 + 0.3*out->Rs;
	double R2 = in->Rs*(1-tR) + tR*out->Rs;
	// Frist to calculate the front point with U
	bool found = true;
	double um_h = pHubContour1->getUfromY(R2, found);

	double Zm_h = 0., Rm_h = 0.;
	pHubContour1->getPoint(um_h, Zm_h, Rm_h);
	double Z2h_te = 0., R2h_te = 0.;
	pHubContour1->getPoint(blade->ute_hub, Z2h_te, R2h_te);

	// Total parameters
	double Rb = (SQR(R2h_te) - SQR(Rm_h)) / (2.*(R2h_te*cos(beta2_h) - Rm_h*cos(beta1_h)));
	double Rm = sqrt(SQR(Rb) + SQR(R2h_te) - 2.*R2h_te*Rb*cos(beta2_h));
	double phi1h = acos((SQR(Rm) + SQR(Rb) - SQR(Rm_h)) / (2.*Rm*Rb));
	double phi2h = acos((SQR(Rm) + SQR(Rb) - SQR(R2h_te)) / (2.*Rm*Rb));

	// For Hub
	double mh[npt + 1] = { 0. }, Mh[npt + 1] = { 0. }, thetah[npt + 1] = { 0. }, betah[npt + 1] = { 0. };
	double Rh_old = 0., Zh_old = 0., Rh_now = 0., Zh_now = 0.;
	mh[0] = 0.;
	Mh[0] = 0.;
	thetah[0] = 0.;
	pHubContour1->getPoint(um_h, Zh_old, Rh_old);
	double phih_old = phi1h;
	double duh = (ute_hub - um_h) / npt;

	// calculate the critical radius : determine the sign of theta
	double cos_phi1 = (SQR(Rm) + SQR(Rm_h) - SQR(Rb)) / (2.*Rm*Rm_h);
	double L_critial = 2 * (Rm*cos_phi1 - Rm_h);

	// For Shroud
	double Zm_s = 0., Rm_s = 0.; // this need to rename
	double ms[npt + 1], Ms[npt + 1], thetas[npt + 1], betas[npt + 1];
	double Rs_old = 0., Zs_old = 0., Rs_now = 0., Zs_now = 0.;


	// get the vaule of u for shroud with Segemnt 3
	double um_s = 0.;
	um_s = pShroudContour->getUfromY(R2, found);
	pShroudContour->getPoint(um_s, Zm_s, Rm_s);
	pShroudContour->getPoint(um_s, Zs_old, Rs_old);
	double phi1s = acos((SQR(Rm) + SQR(Rb) - SQR(Rs_old)) / (2.*Rm*Rb));
	ms[0] = 0.;
	Ms[0] = 0.;
	double temp = (SQR(Rm_h) + SQR(Rs_old) - SQR(Rb*(phi1s - phi1h))) / (2.*Rm_h*Rs_old);
	if (fabs(temp-1.)< 1.E-4)
	{
		thetas[0] = thetah[0] - 0.;
		//thetas[0] = thetah[0] - acos(temp);
	}
	double phis_old = phi1s;
	double dus = (ute_shroud - um_s) / npt;

	// search ule_mean
#if 1
	//double Z1m_old, R1m_old, Z1m_now, R1m_now;
	double Z1_mean = 0.5*(Zm_h + Zm_s);
	double R1_mean = 0.5*(Rm_h + Rm_s);
	double um_mean = 0.;
	//double ule_now;

	double R1m[3] = { 0. }, Z1m[3] = { 0. };
	double u1m[3] = { 0., 0.5, 1. };
	double L1m[3] = { 0. };
	double tol = 1.e-6;
	for (int i = 0; i<3; i++)
	{
		pMeanContour->getPoint(u1m[i], Z1m[i], R1m[i]);
		L1m[i] = fabs(R1m[i] - R1_mean);
	}
	int upper = 0;
	while (L1m[1] > tol)
	{
		if (L1m[0] <= L1m[2])
		{
			u1m[2] = u1m[1] + 0.5*(u1m[2] - u1m[1]);
		}
		else
		{
			u1m[0] = u1m[0] + 0.5*(u1m[1] - u1m[0]);
		}
		u1m[1] = 0.5*(u1m[0] + u1m[2]);
		for (int i = 0; i<3; i++)
		{
			pMeanContour->getPoint(u1m[i], Z1m[i], R1m[i]);
			L1m[i] = fabs(R1m[i] - R1_mean);
		}
		upper++;
		if (upper > 100) break;
	}
	um_mean = u1m[1];
#endif
	// For Shroud
	double Z1m_le = 0., R1m_le = 0.;
	double mm[npt + 1] = { 0. }, Mm[npt + 1] = { 0. }, thetam[npt + 1] = { 0. }, betam[npt + 1] = { 0. };
	double Rm_old = 0., Zm_old = 0., Rm_now = 0., Zm_now = 0.;

	pMeanContour->getPoint(um_mean, Z1m_le, R1m_le);
	pMeanContour->getPoint(um_mean, Zm_old, Rm_old);
	double phi1m = acos((SQR(Rm) + SQR(Rb) - SQR(Rm_old)) / (2.*Rm*Rb));
	mm[0] = 0.;
	Mm[0] = 0.;
	thetam[0] = thetah[0] - acos((SQR(Rm_h) + SQR(Rm_old) - SQR(Rb*(phi1m - phi1h))) / (2.*Rm_h*Rm_old));
	double phim_old = phi1m;
	double dum = (1. - um_mean) / npt;

	for (int i = 1; i <= npt; i++)
	{

		pHubContour1->getPoint((um_h + i*duh), Zh_now, Rh_now);
		double dmh = sqrt(SQR(Rh_now - Rh_old) + SQR(Zh_now - Zh_old));

		double phih_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rh_now)) / (2.*Rm*Rb));
		double Li2 = 2 * SQR(Rb)*(1 - cos(phih_now - phi1h));
		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetah[i] = acos((SQR(Rm_h) + SQR(Rh_now) - Li2) / (2.*Rm_h*Rh_now));
		}
		else
		{
			thetah[i] = thetah[0] - acos((SQR(Rm_h) + SQR(Rh_now) - Li2) / (2.*Rm_h*Rh_now));
		}

		mh[i] = mh[i - 1] + dmh;
		double dM = sqrt(1 + SQR((Zh_now - Zh_old) / (Rh_now - Rh_old)))*log(Rh_now / Rh_old);
		Mh[i] = Mh[i - 1] + dM;

		phih_old = phih_now;
		Rh_old = Rh_now;
		Zh_old = Zh_now;

		// For Shroud
		pShroudContour->getPoint((um_s + i*dus), Zs_now, Rs_now);
		double dms = sqrt(SQR(Rs_now - Rs_old) + SQR(Zs_now - Zs_old));

		double phis_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rs_now)) / (2.*Rm*Rb));
		Li2 = 2 * SQR(Rb)*(1 - cos(phis_now - phi1h));

		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetas[i] = acos((SQR(Rm_h) + SQR(Rs_now) - Li2) / (2.*Rm_h*Rs_now));
		}
		else
		{
			thetas[i] = thetah[0] - acos((SQR(Rm_h) + SQR(Rs_now) - Li2) / (2.*Rm_h*Rs_now));
		}

		ms[i] = ms[i - 1] + dms;
		dM = sqrt(1 + SQR((Zs_now - Zs_old) / (Rs_now - Rs_old)))* log(Rs_now / Rs_old);
		Ms[i] = Ms[i - 1] + dM;
		phis_old = phis_now;
		Rs_old = Rs_now;
		Zs_old = Zs_now;

		// For Mean
		pMeanContour->getPoint((um_mean + i*dum), Zm_now, Rm_now);
		double dmm = sqrt(SQR(Rm_now - Rm_old) + SQR(Zm_now - Zm_old));

		double phim_now = acos((SQR(Rm) + SQR(Rb) - SQR(Rm_now)) / (2.*Rm*Rb));
		Li2 = 2 * SQR(Rb)*(1 - cos(phim_now - phi1h));

		if (Li2 > SQR(L_critial) && (beta2 > 0))
		{
			thetam[i] = acos((SQR(Rm_h) + SQR(Rm_now) - Li2) / (2.*Rm_h*Rm_now));
		}
		else
		{
			thetam[i] = thetah[0] - acos((SQR(Rm_h) + SQR(Rm_now) - Li2) / (2.*Rm_h*Rm_now));
		}

		mm[i] = mm[i - 1] + dmm;
		dM = sqrt(1 + SQR((Zm_now - Zm_old) / (Rm_now - Rm_old)))* log(Rm_now / Rm_old);
		Mm[i] = Mm[i - 1] + dM;
		phim_old = phim_now;
		Rm_old = Rm_now;
		Zm_old = Zm_now;
	}

	betah[0] = 0.;
	betas[0] = atan((thetas[1] - thetas[0]) / (Ms[1] - Ms[0]));
	betam[0] = atan((thetam[1] - thetam[0]) / (Mm[1] - Mm[0]));
	for (int i = 1; i < npt; i++)
	{
		betah[i] = atan((thetah[i + 1] - thetah[i - 1]) / (Mh[i + 1] - Mh[i - 1]));
		betas[i] = atan((thetas[i + 1] - thetas[i - 1]) / (Ms[i + 1] - Ms[i - 1]));
		betam[i] = atan((thetam[i + 1] - thetam[i - 1]) / (Mm[i + 1] - Mm[i - 1]));
	}
	betah[npt] = blade->hubSection->beta2b;
	betas[npt] = atan((thetas[npt] - thetas[npt - 1]) / (Ms[npt] - Ms[npt - 1]));
	betam[npt] = atan((thetam[npt] - thetam[npt - 1]) / (Mm[npt] - Mm[npt - 1]));

	for (int i = 0; i < npt1; i++)
	{
		m_h[i] = mh[i];
		m_s[i] = ms[i];
		beta_h[i] = betah[i];
		beta_s[i] = betas[i];
	}

	// Update TE beta angle
	blade->getMeanSection()->beta2b = betam[npt];
	blade->tipSection->beta2b = betas[npt];
#if 0
	ofstream beta_file4;
	beta_file4.open("D:\\beta4.csv", ios::out | ios::trunc);
	beta_file4 << "" << "," << "ms[i]" << "," << "betas[i]" << "," << "mm[i]" << "," << "betam[i]" << "," << "mh[i]" << "," << "betah[i]" << endl;
	for (int i = 0; i < npt1; i++)
	{
		beta_file4 << i << "," << m_s[i] << "," << beta_s[i] * 180 / PI << ", " << mm[i] << "," << betam[i] * 180 / PI << "," << m_h[i] << "," << beta_h[i] * 180 / PI << endl;
	}
	beta_file4.close();
#endif
}

void T1dImpeller::UpdateThick()
{


  double m_hub = pHubContour1->lengthCurve(); // TODO: shoud be length of blade section 
  double m_shroud = pShroudContour->lengthCurve();
  pMeanContour->Average(pHubContour1, pShroudContour);
  double m_rms = pMeanContour->lengthCurve();
  double tin_h = blade->hubSection->thick1;
  double tin_s = blade->tipSection->thick1;
  double tin_m = blade->getMeanSection()->thick1;

  double tex_h = blade->hubSection->thick2;
  double tex_s = blade->tipSection->thick2;
  double tex_m = blade->getMeanSection()->thick2;
	bool isPiecewise_linear[3] = {false};

  //hub max thickness
  double tmax_h = blade->hubSection->thickMax;
  double m_max_h = blade->hubSection->thickmax_loc_chord;
  if (tmax_h < tin_h)
  {
    tmax_h = 1.5*tin_h;
    m_max_h = 0.5;
  }
  if (tmax_h < tex_h)
  {
    tmax_h = 1.5*tex_h;
    m_max_h = 0.5;
  }
	if ( abs(tmax_h - tin_h) < tolr1 || abs(tmax_h - tex_h) < tolr1) {
		isPiecewise_linear[0] = true;
	}
  //mid max thickness
  double tmax_m = blade->getMeanSection()->thickMax;
  double m_max_m = blade->getMeanSection()->thickmax_loc_chord;
  if (tmax_m < tin_m)
  {
    tmax_m = 1.5*tin_m;
    m_max_m = 0.5;
  }
  if (tmax_m < tex_m)
  {
    tmax_m = 1.5*tex_m;
    m_max_m = 0.5;
  }
	if (abs(tmax_m - tin_m) < tolr1 || abs(tmax_m - tex_m) < tolr1) {
		isPiecewise_linear[1] = true;
	}
  //tip max thickness
  double tmax_s = blade->tipSection->thickMax;
  double m_max_s = blade->tipSection->thickmax_loc_chord;
  if (tmax_s < tin_s)
  {
    tmax_s = 1.5*tin_s;
    m_max_s = 0.5;
  }
  if (tmax_s < tex_s)
  {
    tmax_s = 1.5*tex_s;
    m_max_s = 0.5;
  }
	if (abs(tmax_s - tin_s) < tolr1 || abs(tmax_s - tex_s) < tolr1) {
		isPiecewise_linear[2] = true;
	}

#if 0
	double thub = 0.5*(tin_h + tex_h);
	double ttip = 0.5*(tin_s + tex_s);
	double dmh = 0.25*m_hub;
	double dms = 0.25*m_shroud;
	double dmm = 0.25*m_rms;


  t_hub[0] = 0;
  t_hub[1] = tin_h;
  t_hub[2 * nctrl - 2] = m_hub;
  t_hub[2 * nctrl - 1] = tex_h;
  t_shroud[0] = 0;
  t_shroud[1] = tin_s;
  t_shroud[2 * nctrl - 2] = m_shroud;
  t_shroud[2 * nctrl - 1] = tex_s;
  t_rms[0] = 0;
  t_rms[1] = tin_m;
  t_rms[2 * nctrl - 2] = m_rms;
  t_rms[2 * nctrl - 1] = tex_m;

  //mid control point
  t_hub[4] = m_max_h*m_hub;
  t_hub[5] = tmax_h;
  t_rms[4] = m_max_m*m_rms;
  t_rms[5] = tmax_m;
  t_shroud[4] = m_max_s*m_shroud;
  t_shroud[5] = tmax_s;
  //second control points
  t_hub[2] = 0.8*t_hub[0] + 0.2*t_hub[4];
  t_hub[3] = 0.8*t_hub[1] + 0.2*t_hub[5];
  t_rms[2] = 0.8*t_rms[0] + 0.2*t_rms[4];
  t_rms[3] = 0.8*t_rms[1] + 0.2*t_rms[5];
  t_shroud[2] = 0.8*t_shroud[0] + 0.2*t_shroud[4];
  t_shroud[3] = 0.8*t_shroud[1] + 0.2*t_shroud[5];
  //4th control points
  t_hub[6] = 0.2*t_hub[8] + 0.8*t_hub[4];
  t_hub[7] = 0.2*t_hub[9] + 0.8*t_hub[5];
  t_rms[6] = 0.2*t_rms[8] + 0.8*t_rms[4];
  t_rms[7] = 0.2*t_rms[9] + 0.8*t_rms[5];
  t_shroud[6] = 0.2*t_shroud[8] + 0.8*t_shroud[4];
  t_shroud[7] = 0.2*t_shroud[9] + 0.8*t_shroud[5];

  SET_CURVE_DATA(pHubThick, t_hub, SMOOTH_CURVE_POINT);
  SET_CURVE_DATA(pShroudThick, t_shroud, SMOOTH_CURVE_POINT);
  SET_CURVE_DATA(pMeanThick, t_rms, SMOOTH_CURVE_POINT);
#else
	double mh[3] = {0.}, th[3] = { 0.}, ms[3] = { 0.}, ts[3] = { 0.}, mm[3] = { 0.}, tm[3] = { 0.};
  mh[0] = 0;
  th[0] = tin_h;
  mh[2] = m_hub;
  th[2] = tex_h;
  ms[0] = 0;
  ts[0] = tin_s;
  ms[2] = m_shroud;
  ts[2] = tex_s;
  mm[0] = 0;
  tm[0] = tin_m;
  mm[2] = m_rms;
  tm[2] = tex_m;

  //mid control point
  mh[1] = m_max_h*m_hub;
  th[1] = tmax_h;
  mm[1] = m_max_m*m_rms;
  tm[1] = tmax_m;
  ms[1] = m_max_s*m_shroud;
  ts[1] = tmax_s;

  int np = 3;
  int nc = 3;
	if (!isPiecewise_linear[0]) {
		pHubThick->fitBezier(mh, th, np, nc, 1, 2, 0);
	}
	else {
		double chub[6] = {0.};
		for (int i = 0; i < 3; i++) {
			chub[2 * i] = mh[i];
			chub[2 * i + 1] = th[i];
		}
		pHubThick->setInputData(3, chub, 11, 2, 0, 0);
	}
	if (!isPiecewise_linear[1]) {
		pMeanThick->fitBezier(mm, tm, np, nc, 1, 2, 0);
	}
	else {
		double cmean[6] = { 0. };
		for (int i = 0; i < 3; i++) {
			cmean[2 * i] = mm[i];
			cmean[2 * i + 1] = tm[i];
		}
		pMeanThick->setInputData(3, cmean, 11, 2, 0, 0);
	}

	if (!isPiecewise_linear[2]) {
		pShroudThick->fitBezier(ms, ts, np, nc, 1, 2, 0);
	}
	else {
		double cshrd[6] = { 0. };
		for (int i = 0; i < 3; i++) {
			cshrd[2 * i] = ms[i];
			cshrd[2 * i + 1] = ts[i];
		}
		pShroudThick->setInputData(3, cshrd, 11, 2, 0, 0);
	}

#endif
  return;
}

double T1dImpeller::minFuncLean(double K)
{
  CurveBeta(K);
  double lean = calcLean();
  return fabs(lean);
}

double T1dImpeller::getInletFlowCoefficent()
{
	double Q = sta_le->massflow / sta_le->rho;
	double D2 = 2 * out->Ra;
	double U2 = out->U;
	double phi_1t = Q/(U2*D2*D2);

	return phi_1t;
}

double T1dImpeller::getOutletFlowCoefficent()
{
	double phi2 = out->Cm/out->U;
	return phi2;
}

void T1dImpeller::initFromGeom(QList<T3DInterfaceData*> data)
{
  if (data.size() == 0)
    return;
  initFromGeomBase(data);
  T3DInterfaceData* p3D = data[0];
  //get RLE_hub and RLE_shroud from geometry
  isDoubleSuction = p3D->isDoubleSuction;
  double xo = 0., yo = 0.;
  pShroudContour->getPoint(ule_shroud, xo, yo);
  RLE_shroud = yo;
  pHubContour1->getPoint(ule_hub, xo, yo);
  RLE_hub = yo;
	pShroudContour->getPoint(ute_shroud, xo, yo);
	RTE_shroud = yo;
	pHubContour1->getPoint(ute_hub, xo, yo);
	RTE_hub = yo;

	int machineType = getMachineType();
	if (machineType == mach_compressor)
	{
		DevModelOption = QiuDev;
		blade->bladeType = blade->bladeTypes::Ruled_3D_Type;
	}
	else if (machineType == mach_fan)
	{
		DevModelOption = Wiesner_Busemann;
		// TODO: considering more fan application type
		blade->bladeType = blade->bladeTypes::Radial_2D_SCA_Type;
		phi1h_set = true;
		phi1s_set = true;
		phi2h_set = true;
		phi2s_set = true;
		Double2 Z_axis = {1., 0.};
		phi1_shroud = 0.;
		// Calculate Rcs & Rch
		Double2 ZR_InOut_S[2] = {Double2(in->Zs, in->Rs),  Double2(out->Zs, out->Rs)};
		Double2 Line_00_01[2] = { ZR_InOut_S[0],  ZR_InOut_S[0] + Z_axis.rotate(phi1_shroud) };
		Double2 Line_11_01[2] = { ZR_InOut_S[1],  ZR_InOut_S[1] - Z_axis.rotate(phi2_shroud) };
		Double2 p_insert;
		int sit = intersectionPointBetween2Lines(Line_00_01, Line_11_01, p_insert);
		double delta_phi = phi2_shroud - phi1_shroud;
		double delta_phi_half = delta_phi /2.;
		if (sit == 0)
		{
			RCs = 0.;
		}
		else
		{
			double _length = (p_insert - ZR_InOut_S[0]).length();
			RCs = _length / tan(delta_phi_half);
		}
		RCh = 0.;
	}
	else
	{
		DevModelOption = Wiesner_Busemann;
		blade->bladeType = blade->bladeTypes::Free_3D_Type;

		if (machineType == mach_pump)
			isUserWrapAngleDefine = true;
	}
}

void T1dImpeller::InitGeom(T1dStation *sta_in)
{
  if (sta_in)
  {
    TObject::copyObject(in, sta_in, true, true);
  }
  else
  {
    in->Ra = 0.0925;
    in->phi = 0;
    in->b = 0.095;
  }

  T1dStage *pStage = (T1dStage *)parent("T1dStage");
  T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  isFirstComp = true; //impeller is the first component  
  QString MachineType = GetMachineType();
  bool isPump = (MachineType == "pump");
  bool isFan = (MachineType == "fan");
  if (isPump)
  {
    in->Rh = 0.015;
    in->Rs = 0.06;
    in->phi = 0;
    L_axial = 0.05;
    out->b = 0.022;
    blade->numMainBlade = 7;
    DevModelOption = Wiesner_Busemann;
    isLERadius = false;
    ule_shroud = 0.25;
    ule_hub = 0.40;
    double dleta_b = in->Rs - in->Rh;
    RCh = 0.8*dleta_b; // TODO
    RCs = 0.6*dleta_b;
		isUserWrapAngleDefine = true;
		blade->bladeType = T1dBlade::Free_3D_Type;

    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      inletDesignOption = 2;
      out->phi = 0.5*PI;
      out->Ra = 0.10;

      blade->stackingType = blade->stacking_Option::Stacking_TE;
      blade->useTangentialStacking = true;
    }
    else if (pMachine->AxRad == T1dMachine::exit_type_mixed)
    {
      DevModelOption = QiuDev;
			in->Rh = 0.03;
			in->Rs = 0.16;
			in->phi = 0.;
			L_axial = 0.145;
			ule_shroud = 0.18;
			ule_hub = 0.55;
			blade->numMainBlade = 5;
			out->Ra = 0.222;
			out->b = 0.08;
			out->phi = 60.*PI / 180.;
			phi1h_set = true;
			phi1_hub = 0.* PI / 180;
			phi1s_set = true;
			phi1_shroud = 0.* PI / 180;
			phi2s_set = true;
			phi2_shroud = 55. * PI / 180;
			phi2h_set = true;
			phi2_hub = 66. * PI / 180;
			inletDesignOption = 2;
			R1hOption = 1;
      blade->stackingType = blade->stacking_Option::Stacking_LE;
      blade->useTangentialStacking = true;
    }
  }
  else if (isFan)
  {
    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      if (pMachine->AppType == T1dMachine::AppType_MVR)
      {
	    blade->bladeType = blade->bladeTypes::Compisition_Type;
        out->Ra = 0.92473;
        in->Rh = 0.15;
        in->Rs = 0.45;
        in->phi = 0.;
        phi2_shroud = 85 * PI / 180;
        Rs1_RsE = 1.;
        Rh1_RhE = 1.;
        Rs2_RsO = 1.;
        Rh2_RhO = 1.;
        ute_shroud = 1.;
        ute_hub = 1.;
        L_axial = 0.4;
        out->b = 0.16;
        axLen_disk = L_axial + out->b / 2.;

        isUserWrapAngleDefine = true;
        RatioCone = (0.65*in->Rs + 0.35*in->Rh) / in->Rh; //
        ConeAngle = 45.*PI / 180;
        RCs = 0.08;
        RLE_shroud = Rs1_RsE * in->Rs;
        RLE_hub = Rh1_RhE * in->Rh;
        RTE_shroud = Rs2_RsO * out->Ra;
        RTE_hub = Rh2_RhO * out->Ra;
        blade->numMainBlade = 12;
        rearDisk->d_shaft = 0.1;
        rearDisk->RdiskRear = 1.4*(rearDisk->d_shaft / 2.);
      }
      else if (pMachine->AppType == T1dMachine::AppType_MultiVane)
      { // Multi Vane
        blade->bladeType = T1dBlade::Radial_2D_SCA_Type;
        out->Ra = 0.25;
        in->Rh = 0.02;
        in->Rs = 0.175;
        in->phi = 30. *PI / 180;
        out->b = 0.15;
        beta2b_d = PI / 3;
        rearDisk->d_shaft = 0.02;
        rearDisk->RdiskRear = 0.05;
        RCs = 0.;
        RLE_shroud = 0.2;
        RLE_hub = 0.2;
        Rs1_RsE = RLE_shroud / in->Rs;
        Rh1_RhE = RLE_hub / in->Rh;
        blade->numMainBlade = 32;
        DesIncidence = 0.26;//15 degree
        b2d2 = 0.4;
        phi1_hub = 0.;
        phi1h_set = true;
        phi2_shroud = 89.9 * PI / 180;
        L_axial = 0.9423;
        axLen_disk = out->b;
        if (frontLeakPath)
          frontLeakPath->gap = 0.0;
        if (rearLeakPath)
          rearLeakPath->gap = 0.0;
        DevModelOption = Empirical_correlation;
      }
      else if (pMachine->AppType == T1dMachine::AppType_3D_Vaned_Fan)
      { //default Radial Fan
        blade->bladeType = blade->bladeTypes::Ruled_3D_Type;
        out->Ra = 0.3;
        in->Rh = 0.025;
        in->Rs = 0.15;
        in->phi = 0.;
        phi2_shroud = 82. * PI / 180;
        Rs1_RsE = 1.05;
        Rh1_RhE = 0.7*in->Rs / in->Rh;
        out->b = 0.06;
        in->b = 0.13152;
				RCs = 0.028;
				RCh = 1.25*(in->Rs - in->Rh);
        RLE_shroud = Rs1_RsE * in->Rs;
        RLE_hub = Rh1_RhE * in->Rh;
        blade->numMainBlade = 8;
        L_axial = 0.12;
        axLen_disk = in->b*sin(in->phi) + L_axial + out->b / 2.;

        rearDisk->d_shaft = 0.04;
        rearDisk->RdiskRear = 0.05;
      }
      else
      { 
	    //default Radial Fan
	    blade->bladeType = T1dBlade::Radial_2D_SCA_Type;
        out->Ra = 0.3;
        in->Rh = 0.025;
        in->Rs = 0.118;
        in->phi = 45. *PI / 180;
        phi2_shroud = 82. * PI / 180;
        Rs1_RsE = 1.1;
        Rh1_RhE = 1.1*in->Rs / in->Rh;
        out->b = 0.074;
        in->b = 0.13152;
        RCs = 0.028;
        RLE_shroud = Rs1_RsE * in->Rs;
        RLE_hub = Rh1_RhE * in->Rh;
        blade->numMainBlade = 8;
        L_axial = 0.04;
        axLen_disk = in->b*sin(in->phi) + L_axial + out->b / 2.;

        rearDisk->d_shaft = 0.04;
        rearDisk->RdiskRear = 0.05;
      }
      out->phi = 90. * PI / 180;
      phi2s_set = true;
      //L_axial = 0.5*(in->Rs - in->Rh) / tan(in->phi) - out->b / 2;
      //L_axial = 0.095;
      DevModelOption = Wiesner_Busemann;
      isLERadius = false;
      isShrouded = true;
      if (pMachine->AppType == T1dMachine::AppType_MultiVane)
        DevModelOption = Empirical_correlation;
    }
    else if (pMachine->AxRad == T1dMachine::exit_type_mixed)
    {
      blade->bladeType = 0;
      in->Rh = 0.05;
      in->Rs = 0.15;
      in->phi = 30.* PI / 180;
      out->phi = 45.* PI / 180;
      out->Ra = 0.2;
      out->b = 0.08;

      phi1h_set = false;
      //phi1_hub = 45.* PI / 180;
      phi1s_set = false;
      //phi1_shroud = 40.* PI / 180;
      phi2s_set = false;
      //phi2_shroud = 45. * PI / 180;
      phi2h_set = false;
      //phi2_hub = 60. * PI / 180;

      L_axial = 0.1;
      blade->numMainBlade = 6;
      DevModelOption = QiuDev;
      isLERadius = false;
      Rs1_RsE = 1.;
      Rh1_RhE = 1.;
      RLE_shroud = Rs1_RsE * in->Rs;
      RLE_hub = Rh1_RhE * in->Rh;
      rearDisk->d_shaft = 0.005;
      inletDesignOption = 5;
    }
  }
  else  // For compressor
  {
    if (sta_in && pMachine->AxRad == T1dMachine::exit_type_axial)
    {
      L_axial = 2.0*in->b;
      out->Ra = 2.0*in->Ra;
      out->b = 0.25*in->b;
    }
    else
    {
      in->Rh = 0.06;
      in->Rs = 0.14;
      in->phi = 0;
      L_axial = 0.08;
      out->Ra = 0.20;
      out->b = 0.026;
    }

    blade->numMainBlade = 20;
    DevModelOption = QiuDev;

    if (pMachine->AxRad != T1dMachine::exit_type_axial)
      if (pStage->stageAxRad != pMachine->AxRad)
        pStage->stageAxRad = pMachine->AxRad;

    if (pStage->stageAxRad == T1dMachine::exit_type_radial)
    {
      out->phi = 0.5*PI;
    }
    else if (pStage->stageAxRad == T1dMachine::exit_type_mixed)
    {
      out->phi = PI / 3;
      phi1_hub = in->phi;
      phi1_shroud = in->phi;
      phi2_hub = out->phi;
      phi2_shroud = out->phi;
      phi1h_set = true;
      phi1s_set = true;
      phi2h_set = true;
      phi2s_set = true;
      AxRad = 2;
    }
    hasDbeta_dm = false;
    dbeta_dm_outlet = -4.0;
  }

  inletGeomOption = 1;
  outletGeomOption = 0;
  in->geomOption = inletGeomOption;
  out->geomOption = outletGeomOption;
  in->RecalcGeom();
  out->RecalcGeom();

  //blade
  if (isPump)
  {
    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      blade->hubSection->beta1b = -65.0 / 180.0*PI;
      blade->getMeanSection()->beta1b = -68.0 / 180.0*PI;
      blade->tipSection->beta1b = -72.0 / 180.0*PI;
      blade->hubSection->beta2b = -63.0 / 180.0*PI;
      blade->getMeanSection()->beta2b = -63.0 / 180.0*PI;
      blade->tipSection->beta2b = -63.0 / 180.0*PI;
      isUserWrapAngleDefine = true;

      blade->hubSection->thick1 = 0.006;
      blade->getMeanSection()->thick1 = 0.0045;
      blade->tipSection->thick1 = 0.003;
      blade->hubSection->thick2 = 0.006;
      blade->getMeanSection()->thick2 = 0.0045;
      blade->tipSection->thick2 = 0.003;
      //hub 
      blade->hubSection->thickMax = 1.5*blade->hubSection->thick1;
      blade->hubSection->thickmax_loc_chord = 0.3;
      //rms 
      blade->getMeanSection()->thickMax = 1.5*blade->getMeanSection()->thick1;
      blade->getMeanSection()->thickmax_loc_chord = 0.3;
      //tip
      blade->tipSection->thickMax = 1.5*blade->tipSection->thick1;
      blade->tipSection->thickmax_loc_chord = 0.3;
    }
    else if (pMachine->AxRad == T1dMachine::exit_type_mixed) //
    {
			blade->hubSection->beta1b = -50.0 / 180.0*PI;
			blade->getMeanSection()->beta1b = -65.0 / 180.0*PI;
			blade->tipSection->beta1b = -73.0 / 180.0*PI;
			blade->hubSection->beta2b = -62.0 / 180.0*PI;
			blade->getMeanSection()->beta2b = -66.0 / 180.0*PI;
			blade->tipSection->beta2b = -69.0 / 180.0*PI;
			isUserWrapAngleDefine = true;
			blade->setBldSect();
			blade->pBldSect[0]->wrapAngle = -110 * (PI / 180.);
			blade->pBldSect[1]->wrapAngle = -106 * (PI / 180.);
			blade->pBldSect[2]->wrapAngle = -103 * (PI / 180.);
			blade->hubSection->thick1 = 0.008;
			blade->getMeanSection()->thick1 = 0.006;
			blade->tipSection->thick1 = 0.004;
			blade->hubSection->thick2 = 0.01;
			blade->getMeanSection()->thick2 = 0.007;
			blade->tipSection->thick2 = 0.005;
			//hub 
			blade->hubSection->thickMax = 1.5*blade->hubSection->thick1;
			blade->hubSection->thickmax_loc_chord = 0.4;
			//rms 
			blade->getMeanSection()->thickMax = 1.5*blade->getMeanSection()->thick1;
			blade->getMeanSection()->thickmax_loc_chord = 0.4;
			//tip
			blade->tipSection->thickMax = 1.5*blade->tipSection->thick1;
			blade->tipSection->thickmax_loc_chord = 0.4;
    }

  }
  else if (isFan)
  {
    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      if (pMachine->AppType == T1dMachine::AppType_MVR)
      {
        blade->hubSection->beta1b = -33.0 / 180.0*PI;
        blade->getMeanSection()->beta1b = -55.0 / 180.0*PI;
        blade->tipSection->beta1b = -63.0 / 180.0*PI;
        blade->hubSection->beta2b = -30.0 / 180.0*PI;
        blade->getMeanSection()->beta2b = -30.0 / 180.0*PI;
        blade->tipSection->beta2b = -30.0 / 180.0*PI;
        isUserWrapAngleDefine = false;
      }
      else if (pMachine->AppType == T1dMachine::AppType_MultiVane)
      {
        blade->hubSection->beta1b = -30. / 180.0*PI;
        blade->getMeanSection()->beta1b = -30.0 / 180.0*PI;
        blade->tipSection->beta1b = -30.0 / 180.0*PI;
        blade->hubSection->beta2b = 60. / 180.0*PI;
        blade->getMeanSection()->beta2b = 60.0 / 180.0*PI;
        blade->tipSection->beta2b = 60.0 / 180.0*PI;
        isUserWrapAngleDefine = false;
      }
      else if (pMachine->AppType == T1dMachine::AppType_3D_Vaned_Fan)
      {
				blade->bladeType = blade->bladeTypes::Ruled_3D_Free_Type;
				blade->hubSection->wrapAngle = -30 / 180.0 * PI;
				blade->getMeanSection()->wrapAngle = -30 / 180.0 * PI;
				blade->tipSection->wrapAngle = -30 / 180.0 * PI;
        blade->hubSection->beta1b = -70 / 180.0*PI;
        blade->getMeanSection()->beta1b = -55.0 / 180.0*PI;
        blade->tipSection->beta1b = -63.0 / 180.0*PI;
        blade->hubSection->beta2b = -30.0 / 180.0*PI;
        blade->getMeanSection()->beta2b = -30.0 / 180.0*PI;
        blade->tipSection->beta2b = -30.0 / 180.0*PI;
        isUserWrapAngleDefine = false;
      }
      else
      {
        blade->hubSection->beta1b = -70 / 180.0*PI;
        blade->getMeanSection()->beta1b = -55.0 / 180.0*PI;
        blade->tipSection->beta1b = -63.0 / 180.0*PI;
        blade->hubSection->beta2b = -56 / 180.0*PI;
        blade->getMeanSection()->beta2b = -30.0 / 180.0*PI;
        blade->tipSection->beta2b = -30.0 / 180.0*PI;
        isUserWrapAngleDefine = false;
      }
    }
    else // For mixed
    {
      blade->hubSection->beta1b = -33.0 / 180.0*PI;
      blade->getMeanSection()->beta1b = -55.0 / 180.0*PI;
      blade->tipSection->beta1b = -63.0 / 180.0*PI;
      blade->hubSection->beta2b = -30.0 / 180.0*PI;
      blade->getMeanSection()->beta2b = -30.0 / 180.0*PI;
      blade->tipSection->beta2b = -30.0 / 180.0*PI;
      isUserWrapAngleDefine = false;
    }

    blade->hubSection->thick1 = 0.004;
    blade->getMeanSection()->thick1 = 0.004;
    blade->tipSection->thick1 = 0.004;
    blade->hubSection->thick2 = 0.004;
    blade->getMeanSection()->thick2 = 0.004;
    blade->tipSection->thick2 = 0.004;
    //hub 
    blade->hubSection->thickMax = 1.*blade->hubSection->thick1;
    blade->hubSection->thickmax_loc_chord = 0.5;
    //rms 
    blade->getMeanSection()->thickMax = 1.*blade->getMeanSection()->thick1;
    blade->getMeanSection()->thickmax_loc_chord = 0.5;
    //tip
    blade->tipSection->thickMax = 1.*blade->tipSection->thick1;
    blade->tipSection->thickmax_loc_chord = 0.5;
  }
  else
  {
    // Compressor
    blade->hubSection->beta1b = -33.0 / 180.0*PI;
    blade->getMeanSection()->beta1b = -55.0 / 180.0*PI;
    blade->tipSection->beta1b = -63.0 / 180.0*PI;
    blade->hubSection->beta2b = -30.0 / 180.0*PI;
    blade->getMeanSection()->beta2b = -30.0 / 180.0*PI;
    blade->tipSection->beta2b = -30.0 / 180.0*PI;
    isUserWrapAngleDefine = false;

    blade->hubSection->thick1 = 0.006;
    blade->getMeanSection()->thick1 = 0.0045;
    blade->tipSection->thick1 = 0.003;
    blade->hubSection->thick2 = 0.006;
    blade->getMeanSection()->thick2 = 0.0045;
    blade->tipSection->thick2 = 0.003;
    //hub 
    blade->hubSection->thickMax = 1.5*blade->hubSection->thick1;
    blade->hubSection->thickmax_loc_chord = 0.3;
    //rms 
    blade->getMeanSection()->thickMax = 1.5*blade->getMeanSection()->thick1;
    blade->getMeanSection()->thickmax_loc_chord = 0.3;
    //tip
    blade->tipSection->thickMax = 1.5*blade->tipSection->thick1;
    blade->tipSection->thickmax_loc_chord = 0.3;
  }
  if (isShrouded)
  {
    blade->LE->clr = 0;
    blade->TE->clr = 0;
  }
  else
  {
    if (isPump)
    {
      blade->LE->clr = 0.001;
      blade->TE->clr = 0.001;
    }
    else if (isFan)
    {
      blade->LE->clr = 0.003;
      blade->TE->clr = 0.003;
    }
    else
    {
      blade->LE->clr = 0.0003;
      blade->TE->clr = 0.0003;
    }
  }

  //blade->AddSplitter(0.3);
  //blade->LE->Psi = 5.0 / 180.0*PI;
  //blade->TE->Psi = -5.0 / 180.0*PI;  
  hasRearSeal = true;
  AddSeals();
  geomValid = true;
}

void T1dImpeller::InitGeomNonClog(T1dStation *sta_in)
{
  if (sta_in)
  {
    TObject::copyObject(in, sta_in, true, true);
  }
  else
  {
    in->Ra = 0.0925;
    in->phi = 0;
    in->b = 0.095;
  }
  T1dStage *pStage = (T1dStage *)parent("T1dStage");
  T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  isFirstComp = true;

  DevModelOption = Stechkin;
  in->Rh = 0.00005;
  in->Rs = 0.052;
  L_axial = 0.1;
  ule_shroud = 0.25;
  ule_hub = 0.7;
  blade->numMainBlade = 4;
  out->Ra = 0.1;
  out->b = 0.035;
  out->phi = 83 * PI / 180.;
  phi1h_set = true;
  phi1_hub = 0.* PI / 180;
  phi1s_set = true;
  phi1_shroud = 0.* PI / 180;
  phi2s_set = true;
  phi2_shroud = 78. * PI / 180;
  phi2h_set = true;
  phi2_hub = 84. * PI / 180;
  inletDesignOption = 2;
  R1hOption = 1;
  blade->stackingType = blade->stacking_Option::Stacking_LE;
  blade->useTangentialStacking = true;
  isShrouded = true;
  rearDisk->d_shaft = 0.0001;

  R_boss = 0.0045;
  Z_boss = 0.09;
  phi_boss = 40. * PI / 180;

  inletGeomOption = 1;
  outletGeomOption = 0;
  in->geomOption = inletGeomOption;
  out->geomOption = outletGeomOption;
  in->RecalcGeom();
  out->RecalcGeom();

  //blade
  blade->hubSection->beta1b = -65.0 / 180.0*PI;
  blade->getMeanSection()->beta1b = -68.0 / 180.0*PI;
  blade->tipSection->beta1b = -72.0 / 180.0*PI;
  blade->hubSection->beta2b = -63.0 / 180.0*PI;
  blade->getMeanSection()->beta2b = -63.0 / 180.0*PI;
  blade->tipSection->beta2b = -63.0 / 180.0*PI;
  isUserWrapAngleDefine = true;

  blade->hubSection->thick1 = 0.015;
  blade->getMeanSection()->thick1 = 0.012;
  blade->tipSection->thick1 = 0.01;
  blade->hubSection->thick2 = 0.015;
  blade->getMeanSection()->thick2 = 0.012;
  blade->tipSection->thick2 = 0.01;
  //hub 
  blade->hubSection->thickMax = 1.35 * blade->hubSection->thick1;
  blade->hubSection->thickmax_loc_chord = 0.4;
  //rms 
  blade->getMeanSection()->thickMax = 1.35 * blade->getMeanSection()->thick1;
  blade->getMeanSection()->thickmax_loc_chord = 0.4;
  //tip
  blade->tipSection->thickMax = 1.35 * blade->tipSection->thick1;
  blade->tipSection->thickmax_loc_chord = 0.4;
  if (isShrouded)
  {
    blade->LE->clr = 0;
    blade->TE->clr = 0;
  }
  else
  {
    blade->LE->clr = 0.001;
    blade->TE->clr = 0.001;
  }
  hasRearSeal = true;
  AddSeals();
  geomValid = true;
}

void T1dImpeller::CreateFrontLeakPath()
{
  //set path geometry 
  if (!isShrouded) return; //do nothing

  frontLeakPath->in->Ra = out->Rs;
  frontLeakPath->out->Ra = in->Rs;

	//multivane may not have leakage path
	if (!isMultiVane())
	{
		if (frontLeakPath->gap <= 0)
			frontLeakPath->gap = 0.001;
	}
  int np = 101;
	double Z[101] = { 0. }, R[101] = { 0. }; //the contour points for the cover
  double du = 0.01;
  double u = 0., Z0 = 0., R0 = 0., k = 0., n = 0.;
  double d = frontLeakPath->gap + thick_shroud;
  double Z1 = 0., R1 = 0., Za = 0., Ra = 0.;
  double p = 0;
  int iSeg = 0;
  frontLeakPath->isFront = true;
  frontLeakPath->omega = RPM*PI / 30;
  frontLeakPath->LeakSegList.clear();
  frontLeakPath->AddLeakSeg();
  T1dLeakPathSeg *seg = frontLeakPath->LeakSegList[iSeg]; //current segment
  seg->hasSeal = false;
  T1dSeal *seal; //current seal
  int nSeal = frontLeakPath->sealList.size();
  int sealLeft = nSeal;
  if (sealLeft > 0)
  {
    seal = frontLeakPath->sealList[nSeal - sealLeft];
  }
  int numSealPoint = 0;
  for (int i = 0; i < np; i++) //going through each point
  {
    u = 1 - i*du;
    pShroudContour->getPoint(u, Z0, R0);
    pShroudContour->getSlope(u, k);
    if (k < 1.0e-10)
    {
      n = -1.0e10;
    }
    else
    {
      n = -1.0 / k;  //normal to shroud contour
    }
    double nr = -n / sqrt(1 + n*n); //unit vector in R direction
    double nz = -1 / sqrt(1 + n*n); //unit vector in Z direction
    R1 = R0 + d*nr;
    Z1 = Z0 + d*nz;
    Z[i] = Z1;
    R[i] = R1;
    Za = 0.5*(Z0 + Z1);
    Ra = 0.5*(R0 + R1);
    Z[i] = Z1;
    R[i] = R1;
    seg->AddPoint(Za, Ra, p);
    if (sealLeft > 0) //check if we have seal
    {
      if (Ra < seal->Rseal_start) //now we've found seal
      {
        if (numSealPoint == 0 && sealLeft > 0)   //add the seal segment
        {
          //add seal to the segment
          frontLeakPath->AddLeakSeg();
          iSeg++;
          seg = frontLeakPath->LeakSegList[iSeg];
          seg->AddPoint(Za, Ra, p);
          seg->hasSeal = true;
          seg->seal = seal;
          sealLeft--;
          numSealPoint++;
        }
        else if (numSealPoint >= 1 && numSealPoint <= 2)//add total of three points for the seal
        {
          numSealPoint++;
          //do nothing
        }
        else if (numSealPoint == 3)//done with the seal segment, move on
        {
          //create another vaneless segment for continued check
          numSealPoint++;
          frontLeakPath->AddLeakSeg();
          iSeg++;
          seg = frontLeakPath->LeakSegList[iSeg];
          seg->AddPoint(Za, Ra, p);
          seg->hasSeal = false;
          if (sealLeft > 0)
          {
            //set the current seal to the next seal
            seal = frontLeakPath->sealList[nSeal - sealLeft];
          }
        }
      }
    }
  }
}

void T1dImpeller::CreateRearLeakPath()
{
  T1dStage *pStage = (T1dStage *)parent("T1dStage");
  if (isDoubleSuction) return; //do nothing

  rearLeakPath->in->Ra = out->Rh;
  rearLeakPath->out->Ra = rearDisk->RdiskRear;
  //set path geometry 
  int np = 51;
  double Z[101] = { 0. }, R[101] = { 0. }; //the contour points for the cover
  double du = 0.01;
  double u = 0., Z0 = 0., R0 = 0., k = 0., n = 0.;
  double Z1 = 0., R1 = 0.;
  double p = 0;
  int iSeg = 0;
  if (rearLeakPath->gap <= 0)
    rearLeakPath->gap = 0.0001;
  rearLeakPath->isFront = false;
  rearLeakPath->omega = RPM*PI / 30;
  rearLeakPath->LeakSegList.clear();
  rearLeakPath->AddLeakSeg();
  T1dLeakPathSeg *seg = rearLeakPath->LeakSegList[iSeg]; //current segment
  seg->hasSeal = false;
  T1dSeal *seal; //current seal
  int nSeal = rearLeakPath->sealList.size();
  int sealLeft = nSeal;
  double Rseal = rearDisk->RdiskRear;
  if (sealLeft > 0)
  {
    seal = rearLeakPath->sealList[nSeal - sealLeft];
    Rseal = seal->Rseal_start;
  }
  Z0 = out->Zh;
  R0 = out->Rh;
  double dr = (R0 - Rseal) / (np - 1.0);
  int numSealPoint = 4;
  if (nSeal == 0)
    numSealPoint = 0;
  for (int i = 0; i < np - numSealPoint; i++) //add straight sealless section
  {
    Z1 = Z0 + thick_disk;
    R1 = R0 - i*dr;
    Z[i] = Z1;
    R[i] = R1;
    seg->AddPoint(Z1, R1, p);
  }
  if (nSeal == 0)
    return;
  //add seal segment
  rearLeakPath->AddLeakSeg();
  iSeg++;
  seg = rearLeakPath->LeakSegList[iSeg];
  for (int k = 0; k < numSealPoint; k++)
  {
    int index = np - numSealPoint + k;
    Z1 = Z0 + thick_disk;
    R1 = R0 - index*dr;
    Z[index] = Z1;
    R[index] = R1;
    seg->AddPoint(Z1, R1, p);
    seg->hasSeal = true;
    seg->seal = seal;
  }
  //add segment between Rseal and Rshaft
  rearLeakPath->AddLeakSeg();
  iSeg++;
  seg = rearLeakPath->LeakSegList[iSeg];
  seg->hasSeal = false;
  int np2 = 21;
  dr = (Rseal - rearDisk->RdiskRear) / (np2 - 1);
  for (int j = 0; j < np2; j++)
  {
    Z1 = Z0 + thick_disk;
    R1 = Rseal - j*dr;
    Z[j + np] = Z1;
    R[j + np] = R1;
    seg->AddPoint(Z1, R1, p);
  }
}

void T1dImpeller::updateGUI(bool reloadTree)
{
  TObject::updateGUI(reloadTree);

  loop_child_have_base_class(c, TPlotItem)
  {
    if (c != inducer && c != imp && c != exducer)
    {
      c->updateGUI(reloadTree);
    }
  }
  if (child("shroud"))
  {
    child("shroud")->UpdateTreeItem(isShrouded);
  }
}

void T1dImpeller::UpdateGeometry()
{
	T1dComponent::UpdateGeometry();
	updateWidgetsGeometry();
	setThroat(0);
}

void T1dImpeller::AddSeals()
{
  return;
  if (isShrouded)
  {
    frontLeakPath->gap = out->Ra*0.005;
    if (frontLeakPath->gap > 0.005)
    {
      frontLeakPath->gap = 0.005;
    }
    else if (frontLeakPath->gap < 0.001)
    {
      frontLeakPath->gap = 0.001;
    }
    frontLeakPath->RemoveAllSeal();
    frontLeakPath->AddSeal();
    int nseal = frontLeakPath->sealList.size();
    T1dSeal *seal = frontLeakPath->sealList[0];
    T1dLabyrinth *lbseal = (T1dLabyrinth *)seal;
    //set the default seal geometry for design mode. TODO: accept user input when available
    lbseal->Rseal_start = in->Rs + thick_shroud + frontLeakPath->gap;
    lbseal->clr_seal = 0.1*frontLeakPath->gap;
    lbseal->t = 2.5*lbseal->clr_seal;
    lbseal->P = lbseal->t*2.0;
    lbseal->N = 8;
  }

  if (true)
  {
    rearLeakPath->gap = out->Ra*0.001;
    if (rearLeakPath->gap > 0.005)
    {
      rearLeakPath->gap = 0.005;
    }
    else if (rearLeakPath->gap < 0.001)
    {
      rearLeakPath->gap = 0.001;
    }
    rearLeakPath->RemoveAllSeal();
    rearLeakPath->AddSeal();
    int nseal = rearLeakPath->sealList.size();
    T1dSeal *seal = rearLeakPath->sealList[0];
    T1dLabyrinth *lbseal = (T1dLabyrinth *)seal;
    //set the default seal geometry for design mode. TODO: accept user input when available
    lbseal->Rseal_start = rearDisk->RdiskRear;
    lbseal->clr_seal = 0.1*rearLeakPath->gap;
    lbseal->t = 2.5*lbseal->clr_seal;
    lbseal->P = lbseal->t*2.0;;
    lbseal->N = 8;
  }
}

void T1dImpeller::SetDefaultThick()
{
  // Modified by Yue at 2019/2/21
  bool has_t01 = (blade->hubSection->thick1 > 0);
  bool has_t11 = (blade->getMeanSection()->thick1 > 0);
  bool has_t21 = (blade->tipSection->thick1 > 0);
  bool has_t02 = (blade->hubSection->thick2 > 0);
  bool has_t12 = (blade->getMeanSection()->thick2 > 0);
  bool has_t22 = (blade->tipSection->thick2 > 0);
  has_t01 = isUserInput(blade->hubSection, "thick1");
  has_t11 = isUserInput(blade->getMeanSection(), "thick1");
  has_t21 = isUserInput(blade->tipSection, "thick1");
  has_t02 = isUserInput(blade->hubSection, "thick2");
  has_t12 = isUserInput(blade->getMeanSection(), "thick2");
  has_t22 = isUserInput(blade->tipSection, "thick2");
  bool has_t03 = isUserInput(blade->hubSection, "thickMax");
  bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
  bool has_t23 = isUserInput(blade->tipSection, "thickMax");
  bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
  bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
  bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");

  if (has_t01 && has_t11 && has_t21 && has_t02 && has_t12 && has_t22
    && has_t03 && has_t13 && has_t23 && has_t04 && has_t14 && has_t24)
    return;

  UpdateContour();
  double mlen = pMeanContour->lengthCurve();
  thick_shroud = 0.002;
  thick_disk = 0.002;
  //rearDisk->d_shaft = 2.0*in->Rh*0.9;
  double tedge = 0.008*mlen;
  double minT = 0.0004;
  double maxT = 0.005;
  if (tedge < minT)
  {
    tedge = minT;
  }
  else if (tedge > maxT)
  {
    tedge = maxT;
  }
  double AngleCone1 = 4 * PI / 180; // Avoid too big at hub thickness
  double AngleCone2 = 8 * PI / 180;
  double b_le = in->b;
  double b_te = out->b;
  double dt1 = tan(AngleCone1)*b_le;
  double dt2 = tan(AngleCone2)*b_te;

  if (!has_t21 && !has_t22)
  {
	  if (!has_t01) blade->hubSection->thick1 = tedge + dt1;
	  if (!has_t11)blade->getMeanSection()->thick1 = tedge + 0.5*dt1;
	  if (!has_t21)blade->tipSection->thick1 = tedge;
	  if (!has_t02)blade->hubSection->thick2 = tedge + dt2;
	  if (!has_t12)blade->getMeanSection()->thick2 = tedge + 0.5*dt2;
	  if (!has_t22)blade->tipSection->thick2 = tedge;
	  if (!has_t03)blade->hubSection->thickMax = 1.1*blade->hubSection->thick1;
	  if (!has_t13)blade->getMeanSection()->thickMax = 1.1*blade->getMeanSection()->thick1;
	  if (!has_t23)blade->tipSection->thickMax = 1.1*blade->tipSection->thick1;
	  if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
	  if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
	  if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
  }
  else
  {
	  if (has_t21 && has_t22)
	  {
		  tedge = blade->tipSection->thick1;
		  if (dt1 > 2.*tedge)
		  {
			  dt1 = tedge;
		  }
		  if (!has_t01) blade->hubSection->thick1 = tedge + dt1;
		  if (!has_t11)blade->getMeanSection()->thick1 = tedge + 0.5*dt1;
		  tedge = blade->tipSection->thick2;
		  if (dt2 > 2.*tedge)
		  {
			  dt2 = tedge;
		  }
		  if (!has_t02)blade->hubSection->thick2 = tedge + dt2;
		  if (!has_t12)blade->getMeanSection()->thick2 = tedge + 0.5*dt2;

		  if (!has_t03)blade->hubSection->thickMax = 1.1*blade->hubSection->thick1;
		  if (!has_t13)blade->getMeanSection()->thickMax = 1.1*blade->getMeanSection()->thick1;
		  if (!has_t23)blade->tipSection->thickMax = 1.1*blade->tipSection->thick1;
		  if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
		  if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
		  if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
	  }
	  else if (has_t21)
	  {
		  if (!has_t02)blade->hubSection->thick2 = tedge + dt2;
		  if (!has_t12)blade->getMeanSection()->thick2 = tedge + 0.5*dt2;
		  if (!has_t22)blade->tipSection->thick2 = tedge;

		  tedge = blade->tipSection->thick1;
		  if (!has_t01) blade->hubSection->thick1 = tedge + dt1;
		  if (!has_t11)blade->getMeanSection()->thick1 = tedge + 0.5*dt1;

		  if (!has_t03)blade->hubSection->thickMax = 1.1*blade->hubSection->thick1;
		  if (!has_t13)blade->getMeanSection()->thickMax = 1.1*blade->getMeanSection()->thick1;
		  if (!has_t23)blade->tipSection->thickMax = 1.1*blade->tipSection->thick1;
		  if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
		  if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
		  if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
	  }
	  else if (has_t22)
	  { 
		  if (!has_t01) blade->hubSection->thick1 = tedge + dt1;
		  if (!has_t11)blade->getMeanSection()->thick1 = tedge + 0.5*dt1;
		  if (!has_t11)blade->tipSection->thick1 = tedge;

		  tedge = blade->tipSection->thick2;
		  if (!has_t02)blade->hubSection->thick2 = tedge + dt2;
		  if (!has_t12)blade->getMeanSection()->thick2 = tedge + 0.5*dt2;

		  if (!has_t03)blade->hubSection->thickMax = 1.1*blade->hubSection->thick1;
		  if (!has_t13)blade->getMeanSection()->thickMax = 1.1*blade->getMeanSection()->thick1;
		  if (!has_t23)blade->tipSection->thickMax = 1.1*blade->tipSection->thick1;
		  if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
		  if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
		  if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
	  }
  }

}

#if 0
void T1dImpeller::SetDefaultThick()
{
	bool has_t01 = (blade->hubSection->thick1 > 0);
	bool has_t11 = (blade->rmsSection->thick1 > 0);
	bool has_t21 = (blade->tipSection->thick1 > 0);
	bool has_t02 = (blade->hubSection->thick2 > 0);
	bool has_t12 = (blade->rmsSection->thick2 > 0);
	bool has_t22 = (blade->tipSection->thick2 > 0);
	has_t01 = isUserInput(blade->hubSection, "thick1");
	has_t11 = isUserInput(blade->rmsSection, "thick1");
	has_t21 = isUserInput(blade->tipSection, "thick1");
	has_t02 = isUserInput(blade->hubSection, "thick2");
	has_t12 = isUserInput(blade->rmsSection, "thick2");
	has_t22 = isUserInput(blade->tipSection, "thick2");
	bool has_t03 = isUserInput(blade->hubSection, "thickMax");
	bool has_t13 = isUserInput(blade->rmsSection, "thickMax");
	bool has_t23 = isUserInput(blade->tipSection, "thickMax");
	bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
	bool has_t14 = isUserInput(blade->rmsSection, "thickmax_loc_chord");
	bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");

	if (has_t01 && has_t11 && has_t21 && has_t02 && has_t12 && has_t22
		&& has_t03 && has_t13 && has_t23 && has_t04 && has_t14 && has_t24)
		return;

	UpdateContour();
	double mlen = pMeanContour->lengthCurve();
	thick_shroud = 0.002;
	thick_disk = 0.002;
	//rearDisk->d_shaft = 2.0*in->Rh*0.9;
	double tedge = 0.008*mlen;
	double minT = 0.0004;
	double maxT = 0.005;
	if (tedge < minT)
	{
		tedge = minT;
	}
	else if (tedge > maxT)
	{
		tedge = maxT;
	}
	double AngleCone = 8 * PI / 180;
	double dt1 = tan(AngleCone)*blade->H_le;
	double dt2 = tan(AngleCone)*blade->H_te;

	if (!has_t01) blade->hubSection->thick1 = tedge + dt1;
	if (!has_t11)blade->rmsSection->thick1 = tedge + 0.5*dt1;
	if (!has_t11)blade->tipSection->thick1 = tedge;
	if (!has_t02)blade->hubSection->thick2 = tedge + dt2;
	if (!has_t12)blade->rmsSection->thick2 = tedge + 0.5*dt2;
	if (!has_t22)blade->tipSection->thick2 = tedge;
	if (!has_t03)blade->hubSection->thickMax = 1.1*blade->hubSection->thick1;
	if (!has_t13)blade->rmsSection->thickMax = 1.1*blade->rmsSection->thick1;
	if (!has_t23)blade->tipSection->thickMax = 1.1*blade->tipSection->thick1;
	if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
	if (!has_t14)blade->rmsSection->thickmax_loc_chord = 0.5;
	if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;

}

#endif

void T1dImpeller::SetDefaultThickOfPump()
{
	T1dStage *pStage = (T1dStage *)parent("T1dStage");
	T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);
  bool isNonCaviataionPump = (pMachine->AppType == T1dMachine::AppType_Non_Caviatation_Pump && pMachine->machineType == T1dMachine::MachType_Pump);
	double Ns = pStage->Ns_CN;

	double m_hub = pHubContour1->lengthCurve(); // TODO: shoud be length of blade section 
	double m_shroud = pShroudContour->lengthCurve();

	double D2 = 2 * out->Rm;
	int np = 11;
	double dm_hub[11] = { 0. }, dm_shroud[11] = { 0. };
	double dt_hub[11] = { 0. }, dt_shroud[11] = { 0. };

	T1dOpsingle * opc = pSolverSetting->opCondition->dp;
	double H = opc->TDH_design;
	int Z = blade->numMainBlade;


	if (!isMixedPump)
	{
		// LE thickness
		double t1 = 0.001;
		bool hasT21 = isUserInput(blade->tipSection, "thick1");
		bool hasT22 = isUserInput(blade->tipSection, "thick2");
		if (!hasT21)
		{
			double Ra = out->Ra;
			if (Ra < 0.05)
				t1 = 0.001;
			else if (Ra < 0.1)
				t1 = 0.002;
			else if (Ra < 0.2)
				t1 = 0.003;
			else if (Ra < 0.5)
				t1 = 0.004;
			else
				t1 = 0.005;
			blade->tipSection->thick1 = t1;
		}
		t1 = blade->tipSection->thick1;
		blade->getMeanSection()->thick1 = t1;
		blade->hubSection->thick1 = t1;
		//TE thickness
		double t2 = 0.001;
		if (!hasT22)
		{
			double Ra = out->Ra;
			if (Ra < 0.05)
				t2 = 0.001;
			else if (Ra < 0.1)
				t2 = 0.002;
			else if (Ra < 0.2)
				t2 = 0.003;
			else if (Ra < 0.5)
				t2 = 0.004;
			else
				t2 = 0.005;
			blade->tipSection->thick2 = t2;
		}
		t2 = blade->tipSection->thick2;
		blade->getMeanSection()->thick2 = t2;
		blade->hubSection->thick2 = t2;

		double K = 9;//the balde thickness coefficient of Cast Iron  
		double maxdelta = (K*D2*sqrt(H / Z)) / 1000;//blade max thickness by GuanXingfan
		double tin_h = t1;
		double mindelta = tin_h;//blade min thickness
		if (maxdelta < mindelta)
			maxdelta = mindelta;
		else if (maxdelta > 2.0*mindelta)
			maxdelta = 2.0*mindelta;

		QVector<double> t_eff = { 0., 0.489, 0.778, 0.920, 0.978, 1., 0.883, 0.756, 0.544, 0.356, 0. };
		for (int i = 0; i < np; i++)
		{
			dm_hub[i] = i * m_hub / (np - 1);
			dm_shroud[i] = i * m_shroud / (np - 1);
			dt_shroud[i] = mindelta + t_eff[i] * (maxdelta - mindelta);
			dt_hub[i] = dt_shroud[i];
		}
		if (isNonCaviataionPump)
			maxdelta = t2;
		// Max thickness
		bool has_t03 = isUserInput(blade->hubSection, "thickMax");
		bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
		bool has_t23 = isUserInput(blade->tipSection, "thickMax");
		double tMax = 0.;
		if (!hasT21 && !hasT22)
		{
			tMax = maxdelta;
			blade->hubSection->thickMax = tMax;
			blade->getMeanSection()->thickMax = tMax;
			blade->tipSection->thickMax = tMax;
		}
		else
		{
			blade->hubSection->thickMax = t2;
			blade->getMeanSection()->thickMax = t2;
			blade->tipSection->thickMax = t2;
		}

		bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
		bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
		bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");
		if (!hasT21 && !hasT22)
		{
			blade->hubSection->thickmax_loc_chord = 0.5;
			blade->getMeanSection()->thickmax_loc_chord = 0.5;
			blade->tipSection->thickmax_loc_chord = 0.5;
		}
    else if (!hasT22)
    {
      blade->hubSection->thickmax_loc_chord = 0.5;
      blade->getMeanSection()->thickmax_loc_chord = 0.5;
      blade->tipSection->thickmax_loc_chord = 0.5;
    }
    else
    {
      blade->hubSection->thickmax_loc_chord = 1.;
      blade->getMeanSection()->thickmax_loc_chord = 1.;
      blade->tipSection->thickmax_loc_chord = 1.;

    }
  }
  else //mixed flow pump
  {
    double k_iron = K_Iron(Ns);
    double B1 = 0.96;
    double B2 = 0.98;
    SetDefaultThickfromK(B1, B2, k_iron);
  }
}

void T1dImpeller::SetDefaultThickfromK(double metal_inblockage, double metal_outblockage, double K_IRON)
{
	// by zj
	bool has_t01 = (blade->hubSection->thick1 > 0);
	bool has_t11 = (blade->getMeanSection()->thick1 > 0);
	bool has_t21 = (blade->tipSection->thick1 > 0);
	bool has_t02 = (blade->hubSection->thick2 > 0);
	bool has_t12 = (blade->getMeanSection()->thick2 > 0);
	bool has_t22 = (blade->tipSection->thick2 > 0);
	bool has_th01 = isUserInput(blade->hubSection, "thick1");
	bool has_th11 = isUserInput(blade->getMeanSection(), "thick1");
	bool has_th21 = isUserInput(blade->tipSection, "thick1");
	bool has_th02 = isUserInput(blade->hubSection, "thick2");
	bool has_th12 = isUserInput(blade->getMeanSection(), "thick2");
	bool has_th22 = isUserInput(blade->tipSection, "thick2");
	bool has_t03 = isUserInput(blade->hubSection, "thickMax");
	bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
	bool has_t23 = isUserInput(blade->tipSection, "thickMax");
	bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
	bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
	bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");

	if (has_t01 && has_t11 && has_t21 && has_t02 && has_t12 && has_t22
		&& has_t03 && has_t13 && has_t23 && has_t04 && has_t14 && has_t24)
	return;
	UpdateContour();
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	double Ns = pStage->Ns_CN;
	T1dOpsingle * opc = pSolverSetting->opCondition->dp;
	double H = opc->TDH_design;
	int Z = blade->numMainBlade;
	double D2 = 2. * out->Ra;
	double K = K_IRON;  //the balde thickness coefficient of Cast Iron 
	double A_in = sta_le->Area_geom;
	double A_out = sta_te->Area_geom;
	double A_le = A_in * (1. - metal_inblockage) / blade->numMainBlade; // one blade in area
	double A_te = A_out * (1. - metal_outblockage) / blade->numMainBlade; // one blade out area
	double mean_thick1 = A_le / sta_le->b;
	double mean_thick2 = A_te / out->b;
	
	double max_limi = K * D2 * sqrt(H / Z); //blade max thickness 
    max_limi = floor(max_limi) / 1000.;
	double min_limi = 0.003;   // minimal thickness
	
	if (!has_th11)blade->getMeanSection()->thick1 = 0.65 * max_limi;
	if (!has_th01)blade->hubSection->thick1 = 1.15 * blade->getMeanSection()->thick1;
	if (!has_th21)blade->tipSection->thick1 = 0.8 * blade->getMeanSection()->thick1;

	double l = mean_thick1; //mid thick from area at in
	double t = mean_thick2;   //mid thick from area at out
	double maxth = 1.5 * t;  //  max thick of all

	if (!has_th02)blade->hubSection->thick2 = 1.15 * blade->hubSection->thick1;
	if (!has_th12)blade->getMeanSection()->thick2 = 1.15 * blade->getMeanSection()->thick1;
	if (!has_th22)blade->tipSection->thick2 = 1.15 * blade->tipSection->thick1;

	if (!has_t03)blade->hubSection->thickMax = 1.3 * blade->hubSection->thick1;
	if (!has_t13)blade->getMeanSection()->thickMax = 1.3 * blade->getMeanSection()->thick1;
	if (!has_t23)blade->tipSection->thickMax = 1.3 * blade->tipSection->thick1;

	if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
	if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
	if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
}

void T1dImpeller::SetThickfromBlockage(double inblockage, double outblockage, double gamma01, double gamma02)
{
	bool has_t01 = (blade->hubSection->thick1 > 0);
	bool has_t11 = (blade->getMeanSection()->thick1 > 0);
	bool has_t21 = (blade->tipSection->thick1 > 0);
	bool has_t02 = (blade->hubSection->thick2 > 0);
	bool has_t12 = (blade->getMeanSection()->thick2 > 0);
	bool has_t22 = (blade->tipSection->thick2 > 0);
	bool has_th01 = isUserInput(blade->hubSection, "thick1");
	bool has_th11 = isUserInput(blade->getMeanSection(), "thick1");
	bool has_th21 = isUserInput(blade->tipSection, "thick1");
	bool has_th02 = isUserInput(blade->hubSection, "thick2");
	bool has_th12 = isUserInput(blade->getMeanSection(), "thick2");
	bool has_th22 = isUserInput(blade->tipSection, "thick2");
	bool has_t03 = isUserInput(blade->hubSection, "thickMax");
	bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
	bool has_t23 = isUserInput(blade->tipSection, "thickMax");
	bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
	bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
	bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");

	if (has_t01 && has_t11 && has_t21 && has_t02 && has_t12 && has_t22
		&& has_t03 && has_t13 && has_t23 && has_t04 && has_t14 && has_t24)
		return;
	UpdateContour();
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);
  double Ns = pStage->Ns_CN;
	T1dOpsingle * opc = pSolverSetting->opCondition->dp;
	double H = opc->TDH_design;
	int Z = blade->numMainBlade;
	double A_le = sta_le->Area_geom * (1. - inblockage) / blade->numMainBlade; // one blade in area
	double A_te = sta_te->Area_geom * (1. - outblockage) / blade->numMainBlade; // one blade out area
	double tedge01 = A_le / sta_le->b;
	double tedge02 = A_te / out->b;
	double in_gam = gamma01;
	double out_gam = gamma02;
	in_gam = 2. * PI / 180.;
	out_gam = 4. * PI / 180.;
	double dt1 = tan(in_gam) * sta_le->b;
	double dt2 = tan(out_gam) * out->b;
	if (!has_th11)blade->getMeanSection()->thick1 = 0.5 * dt1 + tedge01;
	if (!has_th01)blade->hubSection->thick1 = dt1 + tedge01;
	if (!has_th21)blade->tipSection->thick1 = tedge01;
	if (!has_th02)blade->hubSection->thick2 = dt2 + tedge02;
	if (!has_th12)blade->getMeanSection()->thick2 = 0.5 * dt2 + tedge02;
	if (!has_th22)blade->tipSection->thick2 = tedge02;
	if (!has_t03)blade->hubSection->thickMax = 1.3 * blade->hubSection->thick1;
	if (!has_t13)blade->getMeanSection()->thickMax = 1.3 * blade->getMeanSection()->thick1;
	if (!has_t23)blade->tipSection->thickMax = 1.3 * blade->tipSection->thick1;
	if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
	if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
	if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;
}

void T1dImpeller::SetDefaultThickOfNonClog()
{
  T1dStage *pStage = (T1dStage *)parent("T1dStage");
  T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);
  double Ns = pStage->Ns_CN;

  double m_hub = pHubContour1->lengthCurve(); // TODO: shoud be length of blade section 
  double m_shroud = pShroudContour->lengthCurve();

  double D2 = 2 * out->Rm;
  int np = 11;
  double dm_hub[11] = { 0. }, dm_shroud[11] = { 0. };
  double dt_hub[11] = { 0. }, dt_shroud[11] = { 0. };

  T1dOpsingle * opc = pSolverSetting->opCondition->dp;
  double H = opc->TDH_design;
  int Z = blade->numMainBlade;


  if (!isMixedPump)
  {
    // LE thickness
    double t1 = 0.001;
    bool hasT21 = isUserInput(blade->tipSection, "thick1");
    bool hasT22 = isUserInput(blade->tipSection, "thick2");
    if (!hasT21)
    {
      double Ra = out->Ra;
      if (Ra < 0.05)
        t1 = 0.001;
      else if (Ra < 0.1)
        t1 = 0.002;
      else if (Ra < 0.2)
        t1 = 0.003;
      else if (Ra < 0.5)
        t1 = 0.004;
      else
        t1 = 0.005;
      blade->tipSection->thick1 = t1;
    }
    t1 = blade->tipSection->thick1;
    blade->getMeanSection()->thick1 = t1;
    blade->hubSection->thick1 = t1;
    //TE thickness
    double t2 = 0.001;
    if (!hasT22)
    {
      double Ra = out->Ra;
      if (Ra < 0.05)
        t2 = 0.001;
      else if (Ra < 0.1)
        t2 = 0.002;
      else if (Ra < 0.2)
        t2 = 0.003;
      else if (Ra < 0.5)
        t2 = 0.004;
      else
        t2 = 0.005;
      blade->tipSection->thick2 = t2;
    }
    t2 = blade->tipSection->thick2;
    blade->getMeanSection()->thick2 = t2;
    blade->hubSection->thick2 = t2;

    double K = 9;//the balde thickness coefficient of Cast Iron  
    double maxdelta = (K*D2*sqrt(H / Z)) / 1000;//blade max thickness by GuanXingfan
    double tin_h = t1;
    double mindelta = tin_h;//blade min thickness
    if (maxdelta < mindelta)
      maxdelta = mindelta;
    else if (maxdelta > 2.0*mindelta)
      maxdelta = 2.0*mindelta;

    QVector<double> t_eff = { 0., 0.489, 0.778, 0.920, 0.978, 1., 0.883, 0.756, 0.544, 0.356, 0. };
    for (int i = 0; i < np; i++)
    {
      dm_hub[i] = i * m_hub / (np - 1);
      dm_shroud[i] = i * m_shroud / (np - 1);
      dt_shroud[i] = mindelta + t_eff[i] * (maxdelta - mindelta);
      dt_hub[i] = dt_shroud[i];
    }

    // Max thickness
    bool has_t03 = isUserInput(blade->hubSection, "thickMax");
    bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
    bool has_t23 = isUserInput(blade->tipSection, "thickMax");
    double tMax = 0.;
    if (!hasT21 && !hasT22)
    {
      tMax = maxdelta;
      blade->hubSection->thickMax = tMax;
      blade->getMeanSection()->thickMax = tMax;
      blade->tipSection->thickMax = tMax;
    }
    else
    {
      blade->hubSection->thickMax = t2;
      blade->getMeanSection()->thickMax = t2;
      blade->tipSection->thickMax = t2;
    }

    bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
    bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
    bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");
    if (!hasT21 && !hasT22)
    {
      blade->hubSection->thickmax_loc_chord = 0.5;
      blade->getMeanSection()->thickmax_loc_chord = 0.5;
      blade->tipSection->thickmax_loc_chord = 0.5;
    }
    else if (!hasT22)
    {
      blade->hubSection->thickmax_loc_chord = 0.5;
      blade->getMeanSection()->thickmax_loc_chord = 0.5;
      blade->tipSection->thickmax_loc_chord = 0.5;
    }
    else
    {
      blade->hubSection->thickmax_loc_chord = 1.;
      blade->getMeanSection()->thickmax_loc_chord = 1.;
      blade->tipSection->thickmax_loc_chord = 1.;

    }
  }
  else //mixed flow pump
  {
    double k_iron = K_Iron(Ns);
    double B1 = 0.96;
    double B2 = 0.98;
    SetDefaultThickfromK(B1, B2, k_iron);
  }
}

void T1dImpeller::UpdateStreamline()
{
	const int span = 3;
	int num_CP = 4;
	double MM[3] = { 0. };
	double Theta[3] = { 0. };
	double MK_CPs[2] = { 0.7, 0.3 };
	const int np = NUMPTS; //number of line points
	double u_scope[3][2] = { {ule_hub, ute_hub}, {ule_mean, ute_mean}, {ule_shroud, ute_shroud} };
	double dt[3] = { 0. };
	double MM_arr[3][NUMPTS] = { 0. };
	// set MM-T curve control points
	QVector <QVector<Double2>> CPs; // control points
	CPs.resize(span);
	// bezier matrix
	int order_conformalMapping[3] = { 3, 3, 3 };
	// 3th Beziser
	double Ma_3rd[4][4] = { { -1, 3, -3, 1 },{ 3,-6, 3, 0 },{ -3, 3,0, 0 },{ 1, 0, 0, 0 } }; // 3th Beziser curve parameters matrix
	double dMa_3rd[4][4] = { { -3, 9, -9, 3 },{ 6,-12, 6, 0 },{ -3, 3,0, 0 },{ 0, 0, 0, 0 } };// 3th Beziser slop curve parameters matrix
	// step 1. calculate M & theta of conformal mapping
	for (int i = 0; i < 3; i++)
	{
		double u1 = u_scope[i][0];
		double u2 = u_scope[i][1];
		dt[i] = (u2 - u1) / (np - 1);
		double ZRj[3][2] = { 0. }, ZRj_1[3][2] = { 0. };
		for (int j = 0; j < np; j++)
		{
			double tj = j * dt[i] + u1;
			double mj = 0., mj_1 = 0., delta_mj = 0.;
			if (i == 0)
			{
				blade->pHubContour1->getPoint(tj, ZRj[0][0], ZRj[0][1]);
			}
			else if (i == 1)
			{
				blade->pHubContour1->getPoint(tj, ZRj[0][0], ZRj[0][1]);
				blade->pShroudContour->getPoint(tj, ZRj[2][0], ZRj[2][1]);
				ZRj[1][0] = 0.5*(ZRj[0][0] + ZRj[2][0]);
				ZRj[1][1] = 0.5*(ZRj[0][1] + ZRj[2][1]);
			}
			else if (i == 2)
			{
				blade->pShroudContour->getPoint(tj, ZRj[2][0], ZRj[2][1]);
			}
			blade->pBldSect[i]->zc[j] = ZRj[i][0];
			blade->pBldSect[i]->rc[j] = ZRj[i][1];
			if (j == 0)
			{
				blade->pBldSect[i]->m[j] = 0.;
				ZRj_1[i][0] = ZRj[i][0];
				ZRj_1[i][1] = ZRj[i][1];
				continue;
			}
			delta_mj = sqrt(SQR(ZRj[i][0] - ZRj_1[i][0]) + SQR(ZRj[i][1] - ZRj_1[i][1]));
			blade->pBldSect[i]->m[j] = blade->pBldSect[i]->m[j - 1] + delta_mj;
			mj_1 = blade->pBldSect[i]->m[j - 1];
			mj = blade->pBldSect[i]->m[j - 1] + delta_mj;
			double R_avg = (ZRj[i][1] + ZRj_1[i][1]) / 2.;
			MM_arr[i][j] = MM_arr[i][j - 1] + delta_mj / R_avg;
			ZRj_1[i][0] = ZRj[i][0];
			ZRj_1[i][1] = ZRj[i][1];
		}
		MM[i] = MM_arr[i][np - 1];
		Theta[i] = blade->pBldSect[i]->wrapAngle;
	}
	// step 2. Set four control points according to the situation
	for (int i = 0; i < 3; i++)
	{
		if (!isUserInput(blade->pBldSect[i], "beta2b"))
		{
			double beta1b_rad = blade->pBldSect[i]->beta1b;
			double beta2b_rad = blade->pBldSect[i]->beta2b;
			double beta1b = blade->pBldSect[i]->beta1b * 180. / PI + 90.;
			double beta2b = blade->pBldSect[i]->beta2b * 180. / PI + 90.;
			Double2 CP_starting = Double2(0., 0.);
			Double2 CP_ending = Double2(MM[i], Theta[i]);
			double length = (CP_ending - CP_starting).length();
			Double2 M_axis = { 1., 0. };
			M_axis *= length;
			Double2 Line_1[2] = { CP_starting, CP_starting + M_axis.rotate(beta1b_rad) };
			Double2 Line_2[2] = { CP_ending,  CP_ending - M_axis.rotate(beta2b_rad) };
			Double2 p_insert;
			int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
			double x_sta = CP_starting[0];
			double y_sta = CP_starting[1];
			double x_in1 = p_insert[0];
			double y_in1 = p_insert[1];
			double x_ending1 = CP_ending[0];
			double y_ending1 = CP_ending[1];
			int pst1 = InsPointBetween2Lines(x_in1, y_in1, x_ending1, y_ending1);
			// pst: 4\5 in 4nd Quadrant; 1/2 in 2nd Quadrant; 3 insert proper
			// adjust streamline angle and wrapangle
			double theta_ins = atan(fabs(x_in1 / y_in1)) * 180. / PI;
			double theta_ending = atan(fabs(x_ending1 / y_ending1)) * 180. / PI;
			double det = 0.1;
			// up in 4nd Quadrant
			if (pst1 == 5)
			{
				double check1b = (theta_ins - theta_ending) / 3.;
				double check2b = beta2b - theta_ending;
				for (int k = 1; k < 121; k++)
				{
					double old_beta2b = beta2b;
					beta2b = old_beta2b - det;
					if (fabs(check2b - det * k) < 0.4)
						break;
				}
				blade->pBldSect[i]->beta1b = (beta1b - check1b) * PI / 180. - PI / 2.;
				blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
			}
			// down in 4nd Quadrant
			else if (pst1 == 4)
			{
				double check2b = (theta_ending - beta2b) / 4.;
				double check1b = theta_ending - beta1b + 3. * check2b;
				for (int k = 1; k < 121; k++)
				{
					double old_beta1b = beta1b;
					beta1b = old_beta1b + det;
					if (fabs(check1b - det * k) < 0.4)
						break;
				}
				blade->pBldSect[i]->beta1b = beta1b * PI / 180. - PI / 2.;
				blade->pBldSect[i]->beta2b = (beta2b + check2b) * PI / 180. - PI / 2.;
			}
			// up in 2nd Quadrant
			else if (pst1 == 1)
			{
				double check2b = (theta_ending - beta2b) / 3.;
				double check1b = theta_ending - theta_ins + 2. * check2b;
				for (int k = 1; k < 121; k++)
				{
					double old_beta1b = beta2b;
					beta2b = old_beta1b + det;
					if (fabs(check1b - det * k) < 0.4)
						break;
				}
				blade->pBldSect[i]->beta1b = (beta1b - check1b) * PI / 180. - PI / 2.;
				blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
			}
			// down in 2nd Quadrant
			else if (pst1 == 2)
			{
				double check2b = beta2b - theta_ending;
				beta2b = beta2b - check2b;
				for (int k = 1; k < 301; k++)
				{
					det = 0.004;
					double old_beta1b = beta1b;
					double old_beta2b = beta2b;
					beta1b = old_beta1b - det;
					beta2b = old_beta2b + det;
					double checka = k * det;
					double checkb = theta_ins - theta_ending - k * det;
					double a = fabs(checka - checkb);
					if (a < 0.002)
						break;
				}
				blade->pBldSect[i]->beta1b = beta1b * PI / 180. - PI / 2.;
				blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
			}
			// have insertion
			else if (pst1 == 3)
			{
				
					blade->pBldSect[i]->beta1b *= 1.;
					blade->pBldSect[i]->beta2b *= 1.;
				
			}
		}
	}
}
// rewrite the adjust mapbeta part by zj
void T1dImpeller::MeridionalDistance(double *MM)
{
	const int span = 3;
	int num_CP = 4;
	double MK_CPs[2] = { 0.7, 0.3 };
	const int np = NUMPTS; //number of line points
	double u_scope[3][2] = { {ule_hub, ute_hub}, {ule_mean, ute_mean}, {ule_shroud, ute_shroud} };
	double dt[3] = { 0. };
	double MM_arr[3][NUMPTS] = { 0. };
	// set MM-T curve control points
	QVector <QVector<Double2>> CPs; // control points
	CPs.resize(span);
	// step 1. calculate M 
	for (int i = 0; i < 3; i++)
	{
		double u1 = u_scope[i][0];
		double u2 = u_scope[i][1];
		dt[i] = (u2 - u1) / (np - 1);
		double ZRj[3][2] = { 0. }, ZRj_1[3][2] = { 0. };
		for (int j = 0; j < np; j++)
		{
			double tj = j * dt[i] + u1;
			double mj = 0., mj_1 = 0., delta_mj = 0.;
			if (i == 0)
			{
				blade->pHubContour1->getPoint(tj, ZRj[0][0], ZRj[0][1]);
			}
			else if (i == 1)
			{
				blade->pHubContour1->getPoint(tj, ZRj[0][0], ZRj[0][1]);
				blade->pShroudContour->getPoint(tj, ZRj[2][0], ZRj[2][1]);
				ZRj[1][0] = 0.5*(ZRj[0][0] + ZRj[2][0]);
				ZRj[1][1] = 0.5*(ZRj[0][1] + ZRj[2][1]);
			}
			else if (i == 2)
			{
				blade->pShroudContour->getPoint(tj, ZRj[2][0], ZRj[2][1]);
			}
			blade->pBldSect[i]->zc[j] = ZRj[i][0];
			blade->pBldSect[i]->rc[j] = ZRj[i][1];
			if (j == 0)
			{
				blade->pBldSect[i]->m[j] = 0.;
				ZRj_1[i][0] = ZRj[i][0];
				ZRj_1[i][1] = ZRj[i][1];
				continue;
			}
			delta_mj = sqrt(SQR(ZRj[i][0] - ZRj_1[i][0]) + SQR(ZRj[i][1] - ZRj_1[i][1]));
			blade->pBldSect[i]->m[j] = blade->pBldSect[i]->m[j - 1] + delta_mj;
			mj_1 = blade->pBldSect[i]->m[j - 1];
			mj = blade->pBldSect[i]->m[j - 1] + delta_mj;
			double R_avg = (ZRj[i][1] + ZRj_1[i][1]) / 2.;
			MM_arr[i][j] = MM_arr[i][j - 1] + delta_mj / R_avg;
			ZRj_1[i][0] = ZRj[i][0];
			ZRj_1[i][1] = ZRj[i][1];
		}
		MM[i] = MM_arr[i][np - 1];
	}
}

void T1dImpeller::AdjustCamberBase()
{
	int pos[3] = { 0 };
	double x_in[3] = { 0. };
	double y_in[3] = { 0. };
	double x_end[3] = { 0. };
	double y_end[3] = { 0. };
	InsertPointData(x_in, y_in, x_end, y_end, pos);
 
	for (int i = 0; i < 3; i++)
	{
    if (!isUserInput(blade->pBldSect[i], "beta2b"))
    {
      int po = pos[i];
      double beta1b = blade->pBldSect[i]->beta1b * 180. / PI + 90.;
      double beta2b = blade->pBldSect[i]->beta2b * 180. / PI + 90.;
      double t_ins = atan(fabs(x_in[i] / y_in[i])) * 180. / PI;
      double t_end = atan(fabs(x_end[i] / y_end[i])) * 180. / PI;
      double det = 0.1;
      double delta1b = beta1b - t_ins;
      double delta2b = t_end - beta2b;
      if (po == 5)
      {
        double check1b = (t_ins - t_end) / 3.;
        double check2b = beta2b - t_end;
        for (int k = 0; k < 120; k++)
        {
          double old_beta2b = beta2b;
          beta2b = old_beta2b - det;
          if (fabs(check2b - det * k) < 0.4)
            break;
        }
        blade->pBldSect[i]->beta1b = (beta1b - check1b) * PI / 180. - PI / 2.;
        blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
      }
      // down in 4nd Quadrant
      else if (po == 4)
      {
        double check2b = (t_end - beta2b) / 4.;
        double check1b = t_end - beta1b + 3. * check2b;
        for (int k = 0; k < 120; k++)
        {
          double old_beta1b = beta1b;
          beta1b = old_beta1b + det;
          if (fabs(check1b - det * k) < 0.4)
            break;
        }
        blade->pBldSect[i]->beta1b = beta1b * PI / 180. - PI / 2.;
        blade->pBldSect[i]->beta2b = (beta2b + check2b) * PI / 180. - PI / 2.;
      }
      // up in 2nd Quadrant
      else if (po == 1)
      {
        double check2b = (t_end - beta2b) / 3.;
        double check1b = t_end - t_ins + 2. * check2b;
        for (int k = 0; k < 120; k++)
        {
          double old_beta1b = beta2b;
          beta2b = old_beta1b + det;
          if (fabs(check1b - det * k) < 0.4)
            break;
        }
        blade->pBldSect[i]->beta1b = (beta1b - check1b) * PI / 180. - PI / 2.;
        blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
      }
      // down in 2nd Quadrant
      else if (po == 2)
      {
        double check2b = beta2b - t_end;
        beta2b = beta2b - check2b;
        for (int k = 0; k < 300; k++)
        {
          det = 0.004;
          double old_beta1b = beta1b;
          double old_beta2b = beta2b;
          beta1b = old_beta1b - det;
          beta2b = old_beta2b + det;
          double checka = k * det;
          double checkb = t_ins - t_end - k * det;
          double a = fabs(checka - checkb);
          if (a < 0.002)
            break;
        }
        blade->pBldSect[i]->beta1b = beta1b * PI / 180. - PI / 2.;
        blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
      }
      else if (po == 3)
      {
        if (delta1b > 1.3 * delta2b)
          beta1b *= 0.93;
        else if (delta2b > 1.3 * delta1b)
          beta2b *= 0.95;
        blade->pBldSect[i]->beta1b = beta1b * PI / 180. - PI / 2.;
        blade->pBldSect[i]->beta2b = beta2b * PI / 180. - PI / 2.;
      }
    }
	}
}

void T1dImpeller::InsertPointData(double *x_in, double *y_in, double *x_end, double *y_end, int *pos)
{
	
	double MM[3] = { 0. };
	MeridionalDistance(MM);
	double Theta[3] = { 0. };
	for (int i = 0; i < 3; i++)
	{
		Theta[i] = blade->pBldSect[i]->wrapAngle;
		double beta1b_rad = blade->pBldSect[i]->beta1b;
		double beta2b_rad = blade->pBldSect[i]->beta2b;
		double beta1b = blade->pBldSect[i]->beta1b * 180. / PI + 90.;
		double beta2b = blade->pBldSect[i]->beta2b * 180. / PI + 90.;
		Double2 CP_starting = Double2(0., 0.);
		Double2 CP_ending = Double2(MM[i], Theta[i]);
		double length = (CP_ending - CP_starting).length();
		Double2 M_axis = { 1., 0. };
		M_axis *= length;
		Double2 Line_1[2] = { CP_starting, CP_starting + M_axis.rotate(beta1b_rad) };
		Double2 Line_2[2] = { CP_ending,  CP_ending - M_axis.rotate(beta2b_rad) };
		Double2 p_insert;
		int sit = intersectionPointBetween2Lines(Line_1, Line_2, p_insert);
		x_in[i] = p_insert[0];
		y_in[i] = p_insert[1];
		x_end[i] = CP_ending[0];
		y_end[i] = CP_ending[1];
		int ins_value = 0;
		double theta_ins2 = atan(fabs(x_in[i] / y_in[i])) * 180. / PI;
		double theta_ending2 = atan(fabs((x_end[i] - x_in[i]) / (y_in[i] - y_end[i]))) * 180. / PI;
		double theta_ins3 = atan(fabs(x_in[i] / y_in[i])) * 180. / PI;
		double theta_ending3 = atan(fabs(x_end[i] / y_end[i])) * 180. / PI;
		if (x_in[i] > 0.)
		{
			if (x_in[i] > x_end[i])
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
		else if (x_in[i] < 0.)
		{
			if (theta_ins2 > theta_ending2)
				ins_value = 2;
			else
				ins_value = 1;
		}
		pos[i] = ins_value;
	}
	
}

void T1dImpeller::AdjustCamberWrapU()
{

}

void T1dImpeller::SetDefaultThickOfFan()
{
	// LE and TE thickness
	double t1 = 0.002;
	if (!isUserInput(blade->tipSection, "thick1"))
	{
		double Ra = out->Ra;
		if (Ra < 0.05)
		{
			t1 = 0.001;
		}
		else if (Ra < 0.1)
		{
			t1 = 0.002;
		}
		else if (Ra < 0.2)
		{
			t1 = 0.003;
		}
		else if (Ra < 0.5)
		{
			t1 = 0.004;
		}
		else
		{
			t1 = 0.005;
		}

		blade->tipSection->thick1 = t1;
		blade->getMeanSection()->thick1 = t1;
		blade->hubSection->thick1 = t1;
	}
	else
	{
		t1 = blade->tipSection->thick1;
		blade->getMeanSection()->thick1 = t1;
		blade->hubSection->thick1 = t1;
	}

	double t2 = 0.002;
	if (!isUserInput(blade->tipSection, "thick2"))
	{
		double Ra = out->Ra;
		if (Ra < 0.05)
		{
			t2 = 0.001;
		}
		else if (Ra < 0.1)
		{
			t2 = 0.002;
		}
		else if (Ra < 0.2)
		{
			t2 = 0.003;
		}
		else if (Ra < 0.5)
		{
			t2 = 0.004;
		}
		else
		{
			t2 = 0.005;
		}

		blade->tipSection->thick2 = t2;
		blade->getMeanSection()->thick2 = t2;
		blade->hubSection->thick2 = t2;
	}
	else
	{
		t2 = blade->tipSection->thick2;
		blade->getMeanSection()->thick2 = t2;
		blade->hubSection->thick2 = t2;
	}

	bool has_t03 = isUserInput(blade->hubSection, "thickMax");
	bool has_t13 = isUserInput(blade->getMeanSection(), "thickMax");
	bool has_t23 = isUserInput(blade->tipSection, "thickMax");
	if (!has_t03)blade->hubSection->thickMax = 0.5*(t1+t2);
	if (!has_t13)blade->getMeanSection()->thickMax = 0.5*(t1 + t2);
	if (!has_t23)blade->tipSection->thickMax = 0.5*(t1 + t2);

	bool has_t04 = isUserInput(blade->hubSection, "thickmax_loc_chord");
	bool has_t14 = isUserInput(blade->getMeanSection(), "thickmax_loc_chord");
	bool has_t24 = isUserInput(blade->tipSection, "thickmax_loc_chord");
	if (!has_t04)blade->hubSection->thickmax_loc_chord = 0.5;
	if (!has_t14)blade->getMeanSection()->thickmax_loc_chord = 0.5;
	if (!has_t24)blade->tipSection->thickmax_loc_chord = 0.5;

}

double T1dImpeller::getExitBeta()
{
	bool is2DRadial = (blade->bladeType == blade->bladeTypes::Radial_2D_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_Free_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_Straight_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Straight_Type)
		;
	double beta2b = 0;
	beta2b = blade->getMeanSection()->beta2b;
	if(is2DRadial)
		beta2b = blade->hubSection->beta2b;
	else if (blade->bladeType == blade->bladeTypes::Ruled_3D_Type)
	{
		beta2b = 0.5*(blade->hubSection->beta2b + blade->tipSection->beta2b);
	}
  return beta2b;
}

void T1dImpeller::setExitBeta(double beta2b)
{
	blade->getMeanSection()->beta2b = beta2b;
	double Di_tanBeta2b = tan(beta2b) * 2 * out->Ra;
	double _tan_beta2b_h = Di_tanBeta2b / (2 * out->Rh);
	double beta2b_h = atan(_tan_beta2b_h);
	blade->hubSection->beta2b = beta2b_h;
	double _tan_beta2b_t = Di_tanBeta2b / (2 * out->Rs);
	double beta2b_t = atan(_tan_beta2b_t);
	blade->tipSection->beta2b = beta2b_t;

  if (getMachineType() == mach_pump)
  {
    T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
    double Ns = pStage->Ns_CN;
    recommendedWrapAngleForPumpImp(Ns);
  }
  else
  {
    recommandWrapAngleForFanImp();
  }
}

void T1dImpeller::recommandWrapAngleForFanImp()
{
	if (blade->bladeType != T1dBlade::bladeTypes::Ruled_3D_Free_Type && blade->bladeType != T1dBlade::bladeTypes::Free_3D_Type)
		return;

	// hub
	double beta1b_hub = blade->hubSection->beta1b;
	double beta2b_hub = blade->hubSection->beta2b;
	double WrapAngle_hub = optWrapAngleFromMerdinalSizeAndMetalBladeAngle(pHubContour1, ule_hub, ute_hub, beta1b_hub, beta2b_hub);
	// tip
	double beta1b_tip = blade->tipSection->beta1b;
	double beta2b_tip = blade->tipSection->beta2b;
	double WrapAngle_tip = optWrapAngleFromMerdinalSizeAndMetalBladeAngle(pShroudContour, ule_shroud, ute_shroud, beta1b_tip, beta2b_tip);

	double t = 1;
	double WrapAngle_avg = t*WrapAngle_tip + (1-t)*WrapAngle_hub;
	blade->hubSection->wrapAngle = WrapAngle_avg;
	blade->getMeanSection()->wrapAngle = WrapAngle_avg;
	blade->tipSection->wrapAngle = WrapAngle_avg;

}

void T1dImpeller::setExitBetaOfFan(double beta2b)
{
	// beta2t, beta2m and beta2h are different.
	// Have to adjust beta2h to match with beta2b
	T1dMachine *pMachine = (T1dMachine *)parent("T1dMachine");
	QString MachineType = GetMachineType();

	if (pMachine->AppType == T1dMachine::AppType_MVR)
	{
		setExitBeta(beta2b);
		return;
	}
	bool is2DRadial = (blade->bladeType == blade->bladeTypes::Radial_2D_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_Free_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_Straight_Type)
		|| (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Straight_Type)
		;
	bool is3DVaned = (blade->bladeType == blade->bladeTypes::Ruled_3D_Type)
		|| (blade->bladeType == blade->bladeTypes::Ruled_3D_Free_Type)
		;
	if (is2DRadial)
	{
		blade->hubSection->beta2b = beta2b;
		UpdateBeta();
		return;
	}
	if (is3DVaned)
	{
		blade->hubSection->beta2b = beta2b;
		blade->getMeanSection()->beta2b = beta2b;
		blade->tipSection->beta2b = beta2b;
		recommandWrapAngleForFanImp();
		UpdateBeta();
		return;
	}
	
	double beta2h_adjust = beta2b + 2*PI/180.;
	double x0 = 0., x1 = 0., y0 = 0., y1 = 0.;
	double tolr = 1.0E-6;
	double beta2m_critial = beta2b + 2 * PI / 180.;
	int k = 0;
	double dbeta2m = beta2m_critial - beta2b;
	for (; k <= 20; k++)
	{
		bool lastTry = AdjustBeta2bOfFan(k, dbeta2m, beta2h_adjust, 1, x0, x1, y0, y1);
		blade->hubSection->beta2b = beta2h_adjust;
		if (pMachine->AppType == T1dMachine::AppType_MVR)
		{
			UpdateBetaWithCompistions();
		}
		else
		{
			UpdateBetaWithCycle();
		}
		dbeta2m = fabs(blade->getMeanSection()->beta2b - beta2b);
		if (dbeta2m < tolr) 
			break;
	}

}

double T1dImpeller::KU2a_pump_Stepanoff(double _Ns)
{
	double Ku2a = 0.;
	double Ns = 0.;
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 

	double Ku2_a = -0.015 * pow(Ns * 12.16 / 1000, 2) + 0.125 * Ns * 12.16 / 1000 + 0.83;
	double Ku2_b = 0.00117 * (Ns - 250.) + 1.098;
	if(Ku2_a > Ku2_b)
		Ku2a = Ku2_a;
	else
		Ku2a = Ku2_b;
	return Ku2a;
}

double T1dImpeller::KU2a_pump_Lobanoff()
{
	if (getMachineType() != mach_pump)
		return 0.;

	T1dStage* pStage = dynamic_cast<T1dStage*> (this->parent());
	double Ns_US = pStage->getSpecificSpeed(2);

	QVector<double> data_Ns_US = QVector<double>()
		<< 0. << 404.044 << 804.42 << 1201.96 << 1600.19 << 2001.32 << 2402.36 << 2805.57 << 3205.95 << 3605.61;

	QVector<double> data_K3_Lobanoff = QVector<double>()
		<< 0.931676 << 0.965298 << 0.999097 << 1.03343 << 1.06795 << 1.10032 << 1.13555 << 1.16899 << 1.20279 << 1.23677;

	// 2. check
	if (Ns_US < data_Ns_US.first())
		Ns_US = data_Ns_US.first();
	else if (Ns_US > data_Ns_US.last())
		Ns_US = data_Ns_US.last();
	// 3. fit bezier curve from source data 
	TNurbsCurve* sourceCurve = new TNurbsCurve;
	sourceCurve->fitBezier(&data_Ns_US[0], &data_K3_Lobanoff[0], data_Ns_US.size(), 5);
	//4. find Ku by NS_US on sourceCurve
	bool found = false;
	double u = sourceCurve->getUfromX(Ns_US, found);
	double Ku_Lobanoff = 0.2;
	if (found)
	{
		sourceCurve->getPoint(u, Ns_US, Ku_Lobanoff);
    delete sourceCurve;
		return Ku_Lobanoff;
	}

	if (!found)
	{
		// 5. TODO:Please check this!
		eprintf("Please check the input value for Speed Coefficient Method Of Lobanoff!");
	}
  delete sourceCurve;
	return Ku_Lobanoff;
}

double T1dImpeller::KD2_corr_pump(double _Ns)
{
  double Ns = 0.;
  T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
  int num_stage = pMachine->nStage;
  bool isSignal_stage = (num_stage == 1);
  T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));

  if (_Ns > tolr1)
    Ns = _Ns; // From searching
  else
    Ns = pStage->Ns_CN; // From design 
  double KD2_corr = 0.;
  // signal stage && !isDoubleSuction
  if (!isDoubleSuction) 
  {
	  // TODO : improvemnt for vary type of stage and impeller
	  double a[42] = { 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
		210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 350, 400, 450, 500, 550, 600, 650,
		700, 750, 800, 850, 900, 950, 1000 };
	  double b[42] = { 1.175, 1.129, 1.09, 1.06, 1.038, 1.021, 1.009, 1, 0.995, 0.994, 0.994, 0.995, 0.997,
		0.999, 1.001, 1.002, 1.004, 1.006, 1.007, 1.009, 1.011, 1.012, 1.014, 1.016, 1.018,
		1.021, 1.023, 1.027, 1.035, 1.042, 1.052, 1.064, 1.073, 1.084, 1.092, 1.107, 1.123, 1.14,
		1.16, 1.181, 1.203, 1.226 };
	  if (Ns < a[0]) 
	  {
		  KD2_corr = b[0];
	  }
	  else if (Ns > a[41]) 
	  {
		  KD2_corr = b[41];
	  }
	  else
	  {
		  for (int i = 0; i < 41; i++)
		  {
			  if ((Ns >= a[i]) && (Ns <= a[i + 1]))
			  {
				  KD2_corr = b[i] + (Ns - a[i])*(b[i + 1] - b[i]) / (a[i + 1] - a[i]);
				  break;
			  }
		  }
	  }
  }
  //signal stage && isDoubleSuction
  else if (isDoubleSuction)
  {
	  double a_d[34] = { 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
	   210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360 };
	  double b_d[34] = { 1.134, 1.105, 1.08, 1.062, 1.047, 1.033, 1.02, 1.01, 1.004, 1.001, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.001, 1.003, 1.005, 1.007, 1.01, 1.013, 1.016, 1.021, 1.025, 1.03,
		1.036, 1.043, 1.05, 1.058, 1.067, 1.077, 1.089, 1.102, 1.117 };
	  if (Ns < a_d[0])
	  {
		  KD2_corr = b_d[0];
	  }
	  else if (Ns > a_d[33])
	  {
		  KD2_corr = b_d[33];
	  }
	  else
	  {
		  for (int i = 0; i < 33; i++)
		  {
			  if ((Ns >= a_d[i]) && (Ns <= a_d[i + 1]))
			  {
				  KD2_corr = b_d[i] + (Ns - a_d[i])*(b_d[i + 1] - b_d[i]) / (a_d[i + 1] - a_d[i]);
				  break;
			  }
		  }
	  }
  }
  if (isSignal_stage)
	return KD2_corr;

  //  multi stage
  double a_m[25] = { 20 , 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190,
    200, 210, 220, 230, 240, 250, 260 };
  double b_m[25] = { 1.145, 1.112, 1.085, 1.065, 1.052, 1.04, 1.032, 1.025, 1.022, 1.022, 1.022, 1.022, 1.022,
    1.024, 1.027, 1.029, 1.032, 1.038, 1.047, 1.058, 1.074, 1.093, 1.117, 1.146, 1.182 };
  if (Ns < a_m[0])
  {
    KD2_corr = b_m[0];
  }
  else if (Ns > a_m[24]) {
    KD2_corr = b_m[24];
  }
  else {
    for (int i = 0; i < 24; i++) 
    {
      if ((Ns > a_m[i]) && (Ns < a_m[i + 1]))
      {
        KD2_corr = b_m[i] + (Ns - a_m[i]) / (a_m[i + 1] - a_m[i]) *(b_m[i + 1] - b_m[i]);
        break;
      }
    }
  }

	return KD2_corr;
}

double T1dImpeller::PreWorkCo_pump(double _Ns)
{
	double Ns = 0.;
	double pres_co = 0.;
	//T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
	int num_stage = pMachine->nStage;
	bool isSignal_stage = (num_stage == 1);
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design
  pres_co = 1.21 * pow(2.7183, (-0.77 * Ns / 365));
	return pres_co;
}

double T1dImpeller::Kb2_pump(double _Ns)
{
	double Ns = 0.;
    T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine")); 
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
  if (_Ns > tolr1)
    Ns = _Ns; // From searching
  else
    Ns = pStage->Ns_CN; // From design 
  if (!isUserInput(this, "Kb"))
    Kb = Kb2_corr_pump();
  double Kb2 = 0.;
	if (Ns < 80.)
	{
		Kb2 = 0.3598 + 0.003767 * Ns;
		if (Ns < 30.)
      Kb = 1.;
		Kb2 *= Kb;
	}
	else if (Ns < 120.)
	{
		Kb2 = 0.78 * sqrt(Ns / 100.);
	}
	else
	{
		Kb2 = 0.62 * Kb * pow(Ns / 100, 5.0 / 6.0);
	}
	return Kb2;
}

double T1dImpeller::B_correct(double _Ns)
{
  double Ns = 0.;
  double b_modif = 0;
  T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
  T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
  if (_Ns > tolr1)
    Ns = _Ns; // From searching
  else
    Ns = pStage->Ns_CN; // From design 
  
  if (Ns < 300.)
    b_modif = 1.0;
  else if ((Ns >= 300.) && (Ns < 360.))
    b_modif = 1.08;
  else if ((Ns >= 360.) && (Ns < 415.))
    b_modif = 1.08 - 0.001455 * (Ns - 360);
  else if ((Ns >= 415.) && (Ns < 500.))
    b_modif = 1.0 - 0.00179 * (Ns - 415);
  else if ((Ns >= 500.) && (Ns < 650.))
    b_modif = 0.9 - 0.0000182 * (Ns - 500);

  return b_modif;
}

double T1dImpeller::K_Iron(double _Ns) // by zj
{
	double Ns = 0.;
	T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
	int num_stage = pMachine->nStage;
	bool isSignal_stage = (num_stage == 1);
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 
	double k_iron = 0.;
	double a[12] = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600. };
	double b[12] = { 5.0, 6.5, 8.0, 9.5, 10.7, 11.5, 13., 14.5, 16, 17.5, 19., 20.5 };
	if (Ns < a[0])
	{
		k_iron = b[0];
	}
	else if (Ns > a[11])
	{
		k_iron = b[11];
	}
	else
	{
		for (int i = 0; i < 12; i++)
		{
			if ((Ns >= a[i]) && (Ns <= a[i + 1]))
			{
				k_iron = b[i] + (Ns - a[i]) * (b[i + 1] - b[i]) / (a[i + 1] - a[i]);
				break;
			}
		}
	}
	double D_b_ratio = out->Ra / out->b;
	if (Ns > 450.&& out->Ra > 0.5)
		k_iron *= 0.88;
	return k_iron;
}

double T1dImpeller::InExcretion(double _Ns, double D2_A)
{
	return 0.;
}

double T1dImpeller::OutExcretion(double _Ns, double D2_A)
{
	return 0.;
}

double T1dImpeller::Kb2_corr_pump(double _Ns)
{
	double Ns = 0.;
  T1dMachine *pMachine = dynamic_cast<T1dMachine *>(parent("T1dMachine"));
  int num_stage = pMachine->nStage;
  bool isSignal_stage = (num_stage == 1);
  T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
  if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 

	double Kb2_corr = 0.;
	if (!isDoubleSuction)
	{
		double a[42] = { 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
										210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 350, 400, 450, 500, 550, 600, 650,
										700, 750, 800, 850, 900, 950, 1000. };
		double b[42] = { 1.946, 1.566, 1.39, 1.3, 1.236, 1.183, 1.139, 1.1, 1.067, 1.037, 1.011, 0.987, 0.965,
											0.946, 0.928, 0.911, 0.896, 0.881, 0.868, 0.855, 0.843, 0.831, 0.82, 0.809, 0.798,
											0.788, 0.777, 0.767, 0.72, 0.677, 0.637, 0.602, 0.57, 0.54, 0.513, 0.489, 0.466, 0.446,
											0.427, 0.409, 0.392, 0.375 };

		if (Ns < a[0]) {
			Kb2_corr = b[0];
		}
		else if (Ns > a[41]) {
			Kb2_corr = b[41];
		}
		else {
			for (int i = 0; i < 41; i++)
			{
				if ((Ns >= a[i]) && (Ns <= a[i + 1]))
				{
					Kb2_corr = b[i] + (Ns - a[i])*(b[i + 1] - b[i]) / (a[i + 1] - a[i]);
					break;
				}
			}
		}
	}
	// signal and isDoubleSuction
	else if (isDoubleSuction)
	{
		double a_d[34] = { 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
	         210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360 };
		double b_d[34] = { 1.212, 1.179, 1.15, 1.127, 1.108, 1.093, 1.081, 1.071, 1.062, 1.054, 1.046, 1.039, 1.032,
		     1.025, 1.018, 1.01, 1.003, 0.996, 0.983, 0.98, 0.972, 0.963, 0.954, 0.945, 0.937, 0.927, 0.917, 0.907, 0.898,
			 0.889, 0.88, 0.87, 0.86, 0.85 };

		if (Ns < a_d[0])
		{
			Kb2_corr = b_d[0];
		}
		else if (Ns > a_d[33]) 
		{
			Kb2_corr = b_d[33];
		}
		else {
			for (int i = 0; i < 33; i++)
			{
				if ((Ns >= a_d[i]) && (Ns <= a_d[i + 1]))
				{
					Kb2_corr = b_d[i] + (Ns - a_d[i]) * (b_d[i + 1] - b_d[i]) / (a_d[i + 1] - a_d[i]);
					break;
				}
			}
		}
	}
  if (isSignal_stage)
    return Kb2_corr;
  // multi 
  double a_m[25] = { 20 , 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190,
    200, 210, 220, 230, 240, 250, 260 };
  double b_m[25] = { 2.6, 2.315, 2.033, 1.765, 1.556, 1.41, 1.31, 1.246, 1.2, 1.161, 1.128, 1.101, 1.078,
    1.055, 1.03, 1.003, 0.976, 0.95, 0.924, 0.898, 0.87, 0.842, 0.814, 0.787, 0.76 };
  if (Ns < a_m[0])
  {
    Kb2_corr = b_m[0];
  }
  else if (Ns > a_m[24]) {
    Kb2_corr = b_m[24];
  }
  else {
    for (int i = 0; i < 24; i++)
    {
      if ((Ns > a_m[i]) && (Ns < a_m[i + 1]))
      {
        Kb2_corr = b_m[i] + (Ns - a_m[i]) / (a_m[i + 1] - a_m[i]) *(b_m[i + 1] - b_m[i]);
        break;
      }
    }
  }
	return Kb2_corr;
}

double T1dImpeller::NPSHr_Minimal(double _Ns)
{
		double Ns = 0.;
		T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
		if (_Ns > tolr1)
			Ns = _Ns; // From searching
		else
			Ns = pStage->Ns_CN; // From design 
		double _Rs1 = 0.;
		double Qin = in->massflow / in->rho;
		if (R1hOption == 1)
			_Rs1 = 0.5* sqrt(4 * in->Rh*in->Rh + 10.6* pow(Qin / RPM, 2.0 / 3.0)*pow((Lamda_c + Lamda_w) / Lamda_w, 1.0 / 3.0));

		else {
			_Rs1 = 0.5*sqrt(10.6* pow(Qin / RPM, 2.0 / 3.0)*pow((Lamda_c + Lamda_w) / Lamda_w, 1.0 / 3.0) / (1 - SQR(in->Rh_Rs)));

		}
		return _Rs1;
}

double T1dImpeller::Deye_D2_ratio_Lobanoff()
{
	if (getMachineType() != mach_pump)
		return 0.;

	T1dStage* pStage = dynamic_cast<T1dStage*> (this->parent());
	double Ns_US = pStage->getSpecificSpeed(2);

	// upper limit of Acceptable area
	QVector<double> Deye_D2_limits;
	for (int i = 0; i < 2; i++)
	{
		QVector<double> data_Ns_US;
		QVector<double> data_Deye_D2_Lobanoff;

		// lowwer limit of Acceptable area
		if (i == 0)
		{
			data_Ns_US = QVector<double>()
				<< 400<< 779.349	<< 1152.9	<< 1525.88 << 1899.66	
				<< 2281.05 << 2578.23	<< 2825.58 << 3156.07<< 3400;

			data_Deye_D2_Lobanoff = QVector<double>()
				<< 0.281632	<< 0.340532	<< 0.398391	<< 0.455207	<< 0.509763	<< 0.563793
				<< 0.60292<< 0.623127	<< 0.636859	<< 0.641249;
		}
		else
		// upper limit of Acceptable area
		{
			data_Ns_US = QVector<double>()
				<< 400.	<< 778.979	<< 1154.69 << 1526.52<< 1901.85<< 2279.76
				<< 2659.5	<< 2869.03<< 3031.94 << 3244.25 << 3400.;
			data_Deye_D2_Lobanoff = QVector<double>()
				<< 0.484126	<< 0.511216	<< 0.537613	<< 0.565403	<< 0.588151	<< 0.610028
				<< 0.632947	<< 0.643093	<< 0.651872	<< 0.658192	<< 0.661934;
		}

		// 3. fit bezier curve from source data 
		TNurbsCurve* sourceCurve = new TNurbsCurve;
		sourceCurve->fitBezier(&data_Ns_US[0], &data_Deye_D2_Lobanoff[0], data_Ns_US.size(), 5);
		//4. find K3 by NS_US on sourceCurve
		bool found = false;
		double u = sourceCurve->getUfromX(Ns_US, found);
		double Deye_D2_Lobanoff = 0.2;
		if (found)
		{
			sourceCurve->getPoint(u, Ns_US, Deye_D2_Lobanoff);
			Deye_D2_limits.push_back(Deye_D2_Lobanoff);
		}
		else
		{
			// 5. TODO:Please check this!
			eprintf("Please check the input value for Speed Coefficient Method Of Lobanoff!");
		}
	}

	double Deye_D2_Lobanoff = (Deye_D2_limits.first() + Deye_D2_limits.last())/2.;

	return Deye_D2_Lobanoff;
}

double T1dImpeller::Km1_pump(double _Ns)
{
  T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
  T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);
  double Ns = 0.;
	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 
	double Km1_local = 0.;

	// Rui
#if 0
	//double Km1 = (-0.02*pow(Ns *12.16 / 1000, 2) + 0.55*Ns *12.16 / 1000 + 0.83) / 10.0;
	double p1 = 2.381e-12, p2 = -2.964e-09, p3 = 1.569e-06, p4 = 1.569e-06, p5 = 1.569e-06;
	double Km1M10 = p1 * pow(Ns, 4) + p2 * pow(Ns, 3) + p3 * pow(Ns, 2) + p4 * Ns + p5;
#endif

	QVector<double > poly = QVector<double>() << 0.06087 << -0.525 << 1.84 << -2.6 << 1.264;
	int n = poly.size();
	if (Ns < 35.) 
		Ns = 35.;
	double x = log10(Ns);
	double log10_Km1 = 0.;
	double log10_Ns = log10(Ns);
	for (int i = 0; i < n; i++)
	{
		double ni = (n - 1.) - i;
		log10_Km1 += poly[i] * pow(log10_Ns, ni);
	}
	Km1_local = pow(10., log10_Km1) / 11;
  if (isMixedPump)
    Km1_local *= 1.1;

	return Km1_local;
}

double T1dImpeller::Km2_pump(double _Ns)
{
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);

	double Ns = 0.;
	
	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 

	double Km2_local = 0.;
	if (!isMixedPump)
	{
		double p1 = -1.243e-13, p2 = 1.628e-10, p3 = -7.619e-08, p4 = 0.0005518, p5 = 0.06102;
		double p_1 = p1 * pow(Ns, 4);
		double p_2 = p2 * pow(Ns, 3);
		double p_3 = p3 * pow(Ns, 2);
		double p_4 = p4 * Ns;
		Km2_local = p_1 + p_2 + p_3 + p_4 + p5;
	}
	else
	{
		double b2_modif = 0.;
		double p1 = -1.243e-13, p2 = 1.628e-10, p3 = -7.619e-08, p4 = 0.0005518, p5 = 0.06102;
		double p_1 = p1 * pow(Ns, 4);
		double p_2 = p2 * pow(Ns, 3);
		double p_3 = p3 * pow(Ns, 2);
		double p_4 = p4 * Ns;
		Km2_local = p_1 + p_2 + p_3 + p_4 + p5;
		if (Ns < 300.)
			b2_modif = 1.0;
		else if ((Ns >= 300.) && (Ns < 360.))
			b2_modif = 1.20 + (Ns - 300.) * 0.0007272;
		else if ((Ns >= 360.) && (Ns < 405.))
			b2_modif = 1.23 + (Ns - 360.) * 0.001333;
		else if ((Ns >= 405.) && (Ns < 500.))
			b2_modif = 1.28 - (Ns - 405.) * 0.001702;
		else if (Ns >= 500.)
			b2_modif = 0.82;
		Km2_local = Km2_local * b2_modif;
	}
	return Km2_local;

}

double T1dImpeller::Km2_pump_Lobanoff()
{
	if (getMachineType() != mach_pump)
		return 0.;
	T1dStage* pStage = dynamic_cast<T1dStage*> (this->parent());

	double Ns_US = pStage->getSpecificSpeed(2);
	QVector<double> data_Ns_US;
	QVector<double> data_Km2_Lobanoff;
	int Z = 0;
	if (Z >= 7) // Z is at [7,8]
	{
		data_Ns_US = QVector<double>()
			<< 400.	<< 596.003	<< 777.577<< 987.584<< 1148.35 << 1385.01	<< 1522.1	<< 1731.15
			<< 1899.97<< 2109.09	<< 2276	<< 2535.43<< 2650.29<< 2864.88	<< 3036.6	<< 3400.;
		data_Km2_Lobanoff = QVector<double>()
			<< 0.04	<< 0.0639336 << 0.0809507	<< 0.0986557 << 0.110302	<< 0.125861	<< 0.135328	<< 0.147931
			<< 0.158225		<< 0.170049	<< 0.179281		<< 0.191352	<< 0.196866	<< 0.206276	<< 0.213025	<< 0.228489;
	}
	else if (Z >= 5) // Z is at [5,6]
	{
		data_Ns_US = QVector<double>()
			<< 400.	<< 626.375 << 799.276	<< 1022.75<< 1199.43 << 1425.9 << 1590.01 << 1801.85
			<< 1994.79 << 2267.01 << 2391.2	<< 2614.41<< 2789.17<< 3017.28<< 3200.;
		data_Km2_Lobanoff = QVector<double>()
			<< 0.04	<< 0.0597272 << 0.073109	<< 0.0881673 << 0.0982165	<< 0.110935	<< 0.119714	
			<< 0.130455	<< 0.139717	<< 0.152768	<< 0.15859<< 0.167979	<< 0.17569<< 0.18536	<< 0.193209;

	}
	else // Z is at [3,4]
	{
		data_Ns_US = QVector<double>()
			<< 400.	<< 798.178	<< 987.441	<< 1196.46	<< 1408.36<< 1592.74			
			<< 1833	<< 1995.52<< 2202.67<< 2404.2	<< 2617.9	<< 2801.95	;

		data_Km2_Lobanoff = QVector<double>()
			<< 0.04	<< 0.0604029<< 0.0692708<< 0.0792662<< 0.0887646 << 0.096571
			<< 0.105918	<< 0.112386	<< 0.119051	<< 0.125151	<< 0.129759	<< 0.132746;
	}

	// 2. check
	if (Ns_US < data_Ns_US.first())
		Ns_US = data_Ns_US.first();
	else if (Ns_US > data_Ns_US.last())
		Ns_US = data_Ns_US.last();
	// 3. fit bezier curve from source data 
	TNurbsCurve* sourceCurve = new TNurbsCurve;
	sourceCurve->fitBezier(&data_Ns_US[0], &data_Km2_Lobanoff[0], data_Ns_US.size(), 5);
	//4. find K3 by NS_US on sourceCurve
	bool found = false;
	double u = sourceCurve->getUfromX(Ns_US, found);
	double Km2_Lobanoff = 0.1;
	if (found)
	{
		sourceCurve->getPoint(u, Ns_US, Km2_Lobanoff);
    delete sourceCurve;
		return Km2_Lobanoff;
	}

	if (!found)
	{
		// 5. TODO:Please check this!
		eprintf("Please check the input value for Km2 Of Lobanoff!");
	}
  delete sourceCurve;
	return Km2_Lobanoff;
}

double T1dImpeller::b2D2_pump(double _Ns)
{
	double Ns = 0.;
	T1dStage *pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
	T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);

	if (_Ns > tolr1)
		Ns = _Ns; // From searching
	else
		Ns = pStage->Ns_CN; // From design 
	if (!isMixedPump)
	{
		QVector<double > poly = QVector<double>() << 0.1075 << -0.6049 << 1.313 << 0.0559;
		int n = poly.size();
		if (Ns < 36.5) Ns = 36.5;
		if (Ns > 547.5) Ns = 547.5;
		double x = Ns / 547.5;
		double b2D2_local = 0.;
		for (int i = 0; i < n; i++)
		{
			double ni = (n - 1.) - i;
			b2D2_local += poly[i] * pow(x, ni);
		}
		double b2D2_margin = 0.3;
		b2D2_local *= b2D2_margin;
		return b2D2_local;
	}
	else
	{
		double b2_modif2 = 0.;
		double Ns_local = 365.;
		double K_ns = Ns / Ns_local;
		double b2D2_local = 0.;
		b2D2_local = 0.017 + 0.262 * K_ns - 0.08 * pow(K_ns, 2.) + 0.0093 * pow(K_ns, 3);
		if (Ns < 300.)
			b2_modif2 = 1.1;
		else if ((Ns >= 300.) && (Ns < 360.))
			b2_modif2 = 1.355 + (Ns - 300.) * 0.00155;
		else if ((Ns >= 360.) && (Ns < 405.))
			b2_modif2 = 1.44 + (Ns - 360.) * 0.0011555;
		else if ((Ns >= 405.) && (Ns < 500.))
			b2_modif2 = 1.494 - (Ns - 405.) * 0.001447;
		else if (Ns >= 500.)
			b2_modif2 = 1.2;
		b2D2_local = b2D2_local * b2_modif2;
		return b2D2_local;
	}
}

void T1dImpeller::setContourPhi2(double Ns)
{
	double eps_hub, eps_shroud, eps_phi2, _phi2_hub, _phi2_shroud;
	QString MachineType = GetMachineType();
	bool isPump = (MachineType == "pump");
	if (isPump)
	{
		T1dStage *pStage = (T1dStage *)parent("T1dStage");
		T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
    bool isMixedPump = (pMachine->AxRad == T1dMachine::exit_type_mixed && pMachine->machineType == T1dMachine::MachType_Pump);
    if (!isMixedPump) // radial
		{
			//phi2_hub
			if ((Ns >= 81.5) && (Ns < 300.))
				eps_hub = 0.142466*Ns - 11.6;
			else
				eps_hub = 0;
			phi2_hub = PI / 2 - eps_hub / 180. * PI;
			phi2h_set = true;
			//phi_shroud
			if ((Ns >= 36.5) && (Ns < 81.5))
				eps_shroud = 0;
			else if ((Ns >= 81.5) && (Ns < 300.))
				eps_shroud = 0.208209*Ns - 17;
			phi2_shroud = PI / 2 - eps_shroud / 180. * PI;
			phi2s_set = true;
			//out->phi2
			if (!isUserInput(out, "phi"))
			{
				out->phi = 0.5*PI;
				if ((Ns >= 36.5) && (Ns < 146.))
					eps_phi2 = 0;
				else if ((Ns >= 146) && (Ns < 300.))
					eps_phi2 = 0.015343*Ns + 10.2;
				out->phi = PI / 2 - eps_phi2 / 180 * PI;
			}
		}
		else // Mixed
		{
			if (!isUserInput(out, "phi"))
			{
				// out mean phi2 & shroud\hub phi2
				if (Ns < 240.)
					eps_phi2 = 0;
				else if ((Ns >= 240.) && (Ns < 300.))
					eps_phi2 = 0.09 * (Ns - 240.) + 5.;
				else if ((Ns >= 300.) && (Ns < 360.))
					eps_phi2 = 0.1666667 * (Ns - 300.) + 10.;
				else if ((Ns >= 360.) && (Ns < 410.))
					eps_phi2 = 30.;
				else if ((Ns >= 410.) && (Ns < 500.))
					eps_phi2 = 0.0168 * (Ns - 410.) + 30.;
				else if ((Ns >= 500.) && (Ns < 650.))
					eps_phi2 = 0.06 * (Ns - 500.) + 31.5;
				else if (Ns >= 650.)
					eps_phi2 = 0.043332 * (Ns - 650.) + 41.;

				out->phi = PI / 2 - eps_phi2 / 180. * PI;
				out->RecalcGeom();  // calculate the outlet para basing phi for rs rh & b, axial len
			}
			if (!isUserInput(this, "phi2_shroud"))
			{
				//phi2_shroud
				if (Ns < 240.)
					_phi2_shroud = 75.;
				else if ((Ns >= 240.) && (Ns < 300.)) //
					_phi2_shroud = 75. - 0.16667 * (Ns - 240);
				else if ((Ns >= 300.) && (Ns < 360.))
					_phi2_shroud = 65. - 0.338 * (Ns - 300);
				else if ((Ns >= 360.) && (Ns < 405.))
					_phi2_shroud = 45.;
				else if ((Ns >= 405.) && (Ns < 500.))
					_phi2_shroud = 45. - 0.1042 * (Ns - 405.);
				else if ((Ns >= 500.) && (Ns < 650.))
					_phi2_shroud = 34. - 0.04667 * (Ns - 500.);
				else if (Ns >= 650.)
					_phi2_shroud = 27. - 0.02 * (Ns - 600.);

				phi2_shroud = _phi2_shroud * PI / 180.;
				phi2s_set = true;
			}
			if (!isUserInput(this, "phi2_hub"))
			{
				//phi2_hub
				if (Ns < 240.)
					_phi2_hub = 80.;
				else if ((Ns >= 240.) && (Ns < 300.))     //
					_phi2_hub = 80. - 0.083333 * (Ns - 240.);
				else if ((Ns >= 300.) && (Ns < 360.))
					_phi2_hub = 75. - 0.25 * (Ns - 300.);
				else if ((Ns >= 360.) && (Ns < 405.))
					_phi2_hub = 60. - 0.05555 * (Ns - 360.);
				else if ((Ns >= 405.) && (Ns < 500.))
					_phi2_hub = 57.5 - 0.0822 * (Ns - 405.);
				else if ((Ns >= 500.) && (Ns < 650.))
					_phi2_hub = 49. - 0.06667 * (Ns - 500.);//
				else if (Ns >= 650.)
					_phi2_hub = 40. - 0.025 * (Ns - 650.);

				phi2_hub = _phi2_hub * PI / 180.;
				phi2h_set = true;
			}
		}
	}
}

#if 0
void T1dImpeller::Trimming(double deltaR, double deltaB_percent)
{
  double R2new = out->Ra - deltaR;
  out->Ra = R2new;
  //calculate the new b2
  double zh[51], rh[51], mh[51], betah[51], thickh[51];
  double zs[51], rs[51], ms[51], betas[51], thicks[51];
  int np = 51;
  bool found = false;
  //hub
  double uh = pHubContour1->getUfromY(R2new, found);
  if (!found)
  {
    return;
  }
  double du = uh / (np - 1);
  for (int i = 0; i < np; i++)
  {
    double u = i*du;
    pHubContour1->getPoint(u, zh[i], rh[i]);
    ms[i] = pHubContour1->calculate2DLength(0, u);
    pHubBeta->getPoint(u, mh[i], betah[i]);
    pHubThick->getPoint(u, mh[i], thickh[i]);
  }
  int nc = 6;
  pHubContour1->fitBezier(zh, rh, np, nc);
  pHubBeta->fitBezier(mh, betah, np, nc, 1, 3, 0);
  pHubThick->fitBezier(ms, thickh, np, nc, 1, 3, 0);
  //shroud
  double us = pShroudContour->getUfromY(R2new, found);
  if (!found)
  {
    return;
  }
  du = us / (np - 1);
  for (int i = 0; i < np; i++)
  {
    double u = i*du;
    pShroudContour->getPoint(u, zs[i], rs[i]);
    ms[i] = pShroudContour->calculate2DLength(0, u);
    pShroudBeta->getPoint(u, ms[i], betas[i]);
    pShroudThick->getPoint(u, ms[i], thicks[i]);
  }
  pShroudContour->fitBezier(zs, rs, np, nc);
  pShroudBeta->fitBezier(ms, betas, np, nc, 1, 3, 0);
  pShroudThick->fitBezier(ms, thicks, np, nc, 1, 3, 0);
  pMeanContour->Average(pHubContour1, pShroudContour);
  pMeanBeta->Average(pHubBeta, pShroudBeta);
  //update the TE parameter
  out->b = fabs(zh[np - 1] - zs[np - 1]);
  //blade outlet geometry: beta and thickness
  double m2, beta2, t2;
  pHubBeta->getPoint(1, m2, beta2);
  blade->hubSection->beta2b = beta2;
  pMeanBeta->getPoint(1, m2, beta2);
  blade->rmsSection->beta2b = beta2;
  pShroudBeta->getPoint(1, m2, beta2);
  blade->tipSection->beta2b = beta2;
  pHubThick->getPoint(1, m2, t2);
  blade->hubSection->thick2 = t2;
  pMeanThick->getPoint(1, m2, t2);
  blade->rmsSection->thick2 = t2;
  pShroudThick->getPoint(1, m2, t2);
  blade->tipSection->thick2 = t2;

  //update contour
  double hub[16], hub1[16], shroud[16];
  double *xch;
  double *ych;
  nc = pHubContour->getNumCtrl();
  if (nc != 8)
  {
    Q_ASSERT(0);
  }
  xch = pHubContour->GetCtrlPoints(0);
  ych = pHubContour->GetCtrlPoints(1);

  for (int i = 0; i < nc; i++)
  {
    hub[2 * i] = xch[i];
    hub[2 * i + 1] = ych[i];
    hub1[2 * (nc - 1 - i)] = xch[i];
    hub1[2 * (nc - 1 - i) + 1] = ych[i];
    //eprintf("hub: %f, %f \n ", hub1[2 * i], hub1[2 * i + 1]);
  }
  SET_CURVE_DATA(pHubContour, hub, SMOOTH_CURVE_POINT);
  SET_CURVE_DATA(pHubContour1, hub1, SMOOTH_CURVE_POINT); //reversed hub, very awkward!
  double *xcs;
  double *ycs;
  xcs = pShroudContour->GetCtrlPoints(0);
  ycs = pShroudContour->GetCtrlPoints(1);

  for (int i = 0; i < nc; i++)
  {
    shroud[2 * i] = xcs[i];
    shroud[2 * i + 1] = ycs[i];
    //eprintf("shroud: %f, %f \n ", shroud[2 * i], shroud[2 * i + 1]);
  }
  SET_CURVE_DATA(pShroudContour, shroud, SMOOTH_CURVE_POINT);
}
#endif

void T1dImpeller::Trimming(double deltaR)
{
  double R2new = out->Ra - deltaR;
  out->Ra = R2new;
  //calculate the new b2
  bool found = false;
  //hub
  double uh = pHubContour1->getUfromY(R2new, found);
  if (!found)
  {
    return;
  }
  double zh = 0., rh = 0., mh = 0., betah = 0., thickh = 0.;
  double zs = 0., rs = 0., ms = 0., betas = 0., thicks = 0.;
  double beta_m = 0., thick_m = 0.;

  pHubContour1->getPoint(uh, zh, rh);
  mh = pHubContour1->calculate2DLength(0, 1);
  pHubBeta->getPoint(uh, mh, betah);
  pHubThick->getPoint(uh, mh, thickh);
  if (getUseGeomCurve())
  {
    pHubContour1->trim(0, uh);
    pHubBeta->trim(0, uh);
    pHubThick->trim(0, uh);
  }
  //shroud
  double us = pShroudContour->getUfromY(R2new, found);
  if (!found)
  {
    return;
  }
  pShroudContour->getPoint(us, zs, rs);
  ms = pShroudContour->calculate2DLength(0, us);
  pShroudBeta->getPoint(us, ms, betas);
  pShroudThick->getPoint(us, ms, thicks);
  if (getUseGeomCurve())
  {
    pShroudContour->trim(0, us);
    pShroudBeta->trim(0, us);
    pShroudThick->trim(0, us);
  }

  beta_m = 0.5*(betah + betas);
  thick_m = 0.5*(thickh + thicks);

  //update the TE parameter
  out->b = fabs(zh - zs);
  out->geomOption = 0;
  //blade outlet geometry: beta and thickness
  blade->hubSection->beta2b = betah;
  blade->getMeanSection()->beta2b = beta_m;
  blade->tipSection->beta2b = betas;
  blade->hubSection->thick2 = thickh;
  blade->getMeanSection()->thick2 = thick_m;
  blade->tipSection->thick2 = thicks;
  
  out->Za = 0.5*(zh + zs);
  out->Zh = zh;
  out->Zs = zs;
  L_axial = out->Za - in->Za;
  UpdateGeometry();
}

void T1dImpeller::UpdateContour()
{
  plotPortedShroud();
  if (getUseGeomCurve())//use geometry curve
  {
    updateContour4Geom();
    updateBladeSectionCurve();
    CreateInletOutlet();

    //set default thickness and shroud
    double t = 0.1 * out->b;
    if (t < 0.005)t = 0.005;
    thick_disk = t;
    thick_shroud = t;
    if (isDoubleSuction)
    {
      UpdateContourDS();
    }
    else if (child("plot-inlet-DS"))
    {
      clearPlotDS();
    }
    return;
  }
  if (!in || !out ||
    !pInlet || !pShroudContour || !pOutlet ||
    !pHubContour || !pHubContour1)
    return;

  T1dMachine* pMachine = (T1dMachine*)parent("T1dMachine");
  QString MachineType = GetMachineType();
  if (MachineType == "pump")
  {
    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      if (!isUserInput(this, "phi1_hub") && !phi1h_set)
        phi1_hub = 0;
      if (!isUserInput(this, "phi1_shroud") && !phi1s_set)
        phi1_shroud = 0;
      if (!isUserInput(this, "phi2_hub") && !phi2h_set)
        phi2_hub = 0.5 * PI;
      if (!isUserInput(this, "phi2_shroud") && !phi2s_set)
        phi2_shroud = 0.5 * PI;
      if (isDoubleSuction)
        phi2_hub = 0.5 * PI;

      if (pMachine->AppType == T1dMachine::AppType_Non_Clogging_Pump)
      {
        UpdateContourOverHang();
      }
      else
      {
        if (contourType == 1)
        {
          UpdateContourWithCycleStraight();
        }
        else
        {
          UpdateContourPump();
        }
      }
    }
    else if (pMachine->AxRad == T1dMachine::exit_type_mixed)
    {
      UpdateContourPump();
      //UpdateContourOfMixed();
    }
    return;
  }

  else if (MachineType == "fan")
  {
    if (pMachine->AxRad == T1dMachine::exit_type_radial)
    {
      if (!isUserInput(this, "phi1_hub") && !phi1h_set)
        phi1_hub = in->phi;
      if (!isUserInput(this, "phi1_shroud") && !phi1s_set)
        phi1_shroud = 0;
      if (!isUserInput(this, "phi2_hub") && !phi2h_set)
        phi2_hub = 0.5 * PI;
      if (!isUserInput(this, "phi2_shroud") && !phi2s_set)
        phi2_shroud = 0.5 * PI;

      if (pMachine->AppType == T1dMachine::AppType_MVR)
      {
        UpdateContourMVR();     // Inserted MVR
      }
      else if (pMachine->AppType == T1dMachine::AppType_3D_Vaned_Fan)
      {
        //phi1_hub = 0;
        //phi1_shroud = 0;
        //phi2_hub = 0.5*PI;
        //phi2_shroud = 0.5*PI;
        //T1dComponent::UpdateContour();
        UpdateContourFan();
      }
      else
      {
        UpdateContourFan();
      }
    }
    else if (pMachine->AxRad == T1dMachine::exit_type_mixed)
    {
      UpdateContourOfMixed();
    }
  }
  else // For compressor
  {
    T1dStage* pStage = dynamic_cast<T1dStage*>(parent("T1dStage"));
    if (pMachine->AxRad != T1dMachine::exit_type_axial)
      if (pStage->stageAxRad != pMachine->AxRad)
        pStage->stageAxRad = pMachine->AxRad;

    if (pStage->stageAxRad == T1dMachine::exit_type_radial) // Radail
    {
      phi1_hub = 0;
      phi1_shroud = 0;
      phi2_hub = 0.5 * PI;
      phi2_shroud = 0.5 * PI;
      //T1dComponent::UpdateContour();
      UpdateContourRadialCompressor();
    }
    else if (pStage->stageAxRad == T1dMachine::exit_type_mixed) // Mixed
    {
      UpdateContourOfMixed();
    }
  }

 // UpdateRmsContour();
}

void T1dImpeller::setGeomUse3DCurve()
{
  pMeanContour->Average(pHubContour1, pShroudContour);
  CreateInletOutlet();
  updateBladeSectionCurve();
}

void T1dImpeller::updateWidgetsGeometry()
{
  bool hasCasing = false;
  bool isCompressor = false;

  if (T1dStage* pStage = (T1dStage*)parent("T1dStage"))
  {
    if (pStage->firstChild("T1dCasing"))
      hasCasing = true;
  }
  if (T1dMachine* pMachine = dynamic_cast<T1dMachine*>(parent("T1dMachine")))
    isCompressor = (pMachine->machineType == T1dMachine::MachType_Compressor);

	if (!hasCasing)
	{
		CreateFrontLeakPath();
		CreateRearLeakPath();
		rearDisk->clearPlot();
		if (isParaDisk)
		{
			// todo:maybe more infor
			rearLeakPath->clearPlot();
			SET_OBJ_HIDE(rearLeakPath);
			rearDisk->updatePlotParaDisk();
		}
		else
		{
			// todo:maybe more infor
			rearDisk->PlotDisk();
		}
		if (isShrouded) {
			shroud->PlotShroud();
			frontLeakPath->plotLeakPath();
			SET_OBJ_SHOW(frontLeakPath);
			SET_OBJ_SHOW(shroud);
			SET_OBJ_SHOW_DISPLAY(frontLeakPath);
			SET_OBJ_SHOW_DISPLAY(shroud);
			QList<TObject*> L = frontLeakPath->allChild("T1dSeal");
			forLoop(i, L.size()) {
				T1dSeal* o = (T1dSeal*)L[i];
				if (frontLeakPath->sealList.indexOf(o) < 0)
					frontLeakPath->sealList.append(o);
				o->plotSeal();
			}
		}
		else {
			shroud->clearShroud();
			frontLeakPath->clearPlot();
			SET_OBJ_HIDE(frontLeakPath);
			SET_OBJ_HIDE(shroud);
			SET_OBJ_HIDE_DISPLAY(frontLeakPath);
			SET_OBJ_HIDE_DISPLAY(shroud);
			QList<TObject*> LL = frontLeakPath->allChild("T1dSeal");
			forLoop(i, LL.size()) {
				T1dSeal* o = (T1dSeal*)LL[i];
				o->clearPlot();
				TObject::delete_object(o);
			}
			frontLeakPath->sealList.clear();
		}

		if (!isDoubleSuction)
		{
			if (isParaDisk)
			{
				rearLeakPath->clearPlot();
				rearLeakPath->plotLeakPathS();
				SET_OBJ_SHOW(rearLeakPath);
				SET_OBJ_SHOW_DISPLAY(rearLeakPath);
				QList<TObject*> LL = rearLeakPath->allChild("T1dSeal");
				forLoop(i, LL.size()) {
					T1dSeal* o = (T1dSeal*)LL[i];
					if (rearLeakPath->sealList.indexOf(o) < 0)
						rearLeakPath->sealList.append(o);
					o->plotSeal();
				}
			}
			else
			{
				rearLeakPath->clearPlot();
				rearLeakPath->plotLeakPath();
				SET_OBJ_SHOW(rearLeakPath);
				SET_OBJ_SHOW_DISPLAY(rearLeakPath);
				QList<TObject*> LL = rearLeakPath->allChild("T1dSeal");
				forLoop(i, LL.size()) {
					T1dSeal* o = (T1dSeal*)LL[i];
					if (rearLeakPath->sealList.indexOf(o) < 0)
						rearLeakPath->sealList.append(o);
					o->plotSeal();
				}

			}
		}
		else
		{
			rearLeakPath->clearPlot();
			SET_OBJ_HIDE(rearLeakPath);
			SET_OBJ_HIDE_DISPLAY(rearLeakPath);
			QList<TObject*> LL = rearLeakPath->allChild("T1dSeal");
			forLoop(i, LL.size()) {
				T1dSeal* o = (T1dSeal*)LL[i];
				o->clearPlot();
				TObject::delete_object(o);
			}
		}
	}
	else
	{
		//rearDisk->updatePlotParaDisk();
		rearDisk->PlotDisk();
		if (isShrouded) {
			shroud->PlotShroud();
			SET_OBJ_SHOW(shroud);
			SET_OBJ_SHOW_DISPLAY(shroud);
		}
		else {
			shroud->clearShroud();
			SET_OBJ_HIDE(shroud);
			SET_OBJ_HIDE_DISPLAY(shroud);

		}
		frontLeakPath->clearPlot();
		SET_OBJ_HIDE(frontLeakPath);
		SET_OBJ_HIDE_DISPLAY(frontLeakPath);
		QList<TObject*> LL = frontLeakPath->allChild("T1dSeal");
		forLoop(i, LL.size()) {
			T1dSeal* o = (T1dSeal*)LL[i];
			o->clearPlot();
			TObject::delete_object(o);
		}
		rearLeakPath->clearPlot();
		SET_OBJ_HIDE(rearLeakPath);
		SET_OBJ_HIDE_DISPLAY(rearLeakPath);
		LL.clear();
		LL = rearLeakPath->allChild("T1dSeal");
		forLoop(i, LL.size()) {
			T1dSeal* o = (T1dSeal*)LL[i];
			o->clearPlot();
			TObject::delete_object(o);
		}
	}
	//hide some of object
	loop_child_with_base_class(curve, TCurve)
	{
		SET_OBJ_HIDE(curve);
	}

	HideObj();

}

void T1dImpeller::UpdateContourPump()
{
  if (getUseGeomCurve())//use geometry curve
  {
    setGeomUse3DCurve();
    return;
  }
  //create hub and shroud line
  double R1h = in->Rh;
  double R1s = in->Rs;
  double Z1h = in->Zh;
  double Z1s = in->Zs;
  double R2h = out->Rh;
  double R2s = out->Rs;
  double Z2h = out->Zh;
  double Z2s = out->Zs;
  double tolr = 1.0E-5;

  double Rc_scenter = 0., Zc_scenter = 0., Rc_s1 = 0., Zc_s1 = 0., Rc_s2 = 0., Zc_s2 = 0.;  //control points on the shroud
  double n1 = tan(phi1_shroud);//y=nx*b
  double b1 = R1s - n1*Z1s;
	double min_phi2_shroud = atan((R2s - R1s) / (Z2s - Z1s));
  if (phi2_shroud == 0.5*PI)
  {
    Zc_scenter = Z2s;
    Rc_scenter = n1*Zc_scenter + b1;
  }
  else if(phi2_shroud <= min_phi2_shroud)
  {
		if (phi2_shroud < min_phi2_shroud)
		{
			phi2_shroud = min_phi2_shroud;
			eprintf("Please check : shroud contour angle at outlet !");
		}
		//phi2_shroud = min_phi2_shroud;
		double n2 = tan(phi2_shroud);
    double b2 = R2s - n2*Z2s;
    Zc_scenter = (b2 - b1) / (n1 - n2);
    Rc_scenter = n1*Zc_scenter + b1;
  }
	else
	{
		double n2 = tan(phi2_shroud);
		double b2 = R2s - n2 * Z2s;
		Zc_scenter = (b2 - b1) / (n1 - n2);
		Rc_scenter = n1 * Zc_scenter + b1;
	}
  //shroud control points close to the LE  
  Rc_s1 = R1s + 0.75*(Rc_scenter - R1s);
  Zc_s1 = Z1s + 0.75*(Zc_scenter - Z1s);
  //shroud control points close to the TE  
  Rc_s2 = Rc_scenter + 0.60*(R2s - Rc_scenter);
  Zc_s2 = Zc_scenter + 0.60*(Z2s - Zc_scenter);

  double Rc_hcenter = 0., Zc_hcenter = 0., Rc_h1 = 0., Zc_h1 = 0., Rc_h2 = 0., Zc_h2 = 0.;  //control points on the hub
  double nn1 = tan(phi1_hub);//y=nx*b
  double bb1 = R1h - nn1*Z1h;
  if (phi2_hub == 0.5*PI)
  {
    Zc_hcenter = Z2h;
    Rc_hcenter = nn1*Zc_hcenter + bb1;
  }
  else
  {
    double n2 = tan(phi2_hub);
    double b2 = R2h - n2*Z2h;
    Zc_hcenter = (b2 - bb1) / (nn1 - n2);
    Rc_hcenter = nn1*Zc_hcenter + bb1;
  }
  
  //hub control points close to the LE
  Rc_h1 = R1h + 0.75*(Rc_hcenter - R1h);;
  Zc_h1 = Z1h + 0.75*(Zc_hcenter - Z1h);
  //hub control points close to the TE
  Rc_h2 = Rc_hcenter + 0.60*(R2h - Rc_hcenter);
  Zc_h2 = Zc_hcenter + 0.60*(Z2h - Zc_hcenter);

  int nc = 5;
  double left[4] = { Z1h, R1h, Z1s, R1s };
  SET_CURVE_DATA(pInlet, left, 4);
  {
    double top[] = { Z1s, R1s, Zc_s1, Rc_s1,Zc_scenter, Rc_scenter, Zc_s2, Rc_s2, Z2s, R2s };
    pShroudContour->setInputData(nc, top, 51, 5);
    double bottom[] = { Z2h, R2h, Zc_h2, Rc_h2, Zc_hcenter, Rc_hcenter,Zc_h1, Rc_h1, Z1h, R1h };
    pHubContour->setInputData(nc, bottom, 51, 5);
    //revert hub
    double bottom1[] = { Z1h, R1h,Zc_h1, Rc_h1,Zc_hcenter, Rc_hcenter,Zc_h2, Rc_h2,Z2h, R2h };
    pHubContour1->setInputData(nc, bottom1, 51, 5);
  }
  double right[4] = { Z2s, R2s, Z2h, R2h };
  SET_CURVE_DATA(pOutlet, right, 4);

  pMeanContour->Average(pHubContour1, pShroudContour);
  calculateMeanContourAngle();
  if (isDoubleSuction)
  {
    UpdateContourDS();
  }
  else if (child("plot-inlet-DS"))
  {
    clearPlotDS();
  }
}

void T1dImpeller::UpdateContourFan()
{
	if (getUseGeomCurve())//use geometry curve
	{
    setGeomUse3DCurve();
		return;
	}

	T1dMachine *pMachine = (T1dMachine *)parent("T1dMachine");
	if (pMachine->AppType == T1dMachine::AppType_MultiVane)
	{
		UpdateContourSimpleFan();
		return;
	}

	// version 3: for straght + cycle for shroud and hub

	// validate check 
	// axLen_disk calculation
	// Check
  if (in->Rs >= RLE_shroud)
  {
    RLE_shroud = 1.001*in->Rs;
    eprintf("Please check : LE shroud radius must be greater than Inlet shroud radius !");
  }
	if (phi1_shroud < 0)
	{
		phi1_shroud = 0.;
		eprintf("Please check : shroud contour angle at inlet !");
	}
	if (phi2_shroud < PI / 3. || phi2_shroud > PI / 2)
	{
		if (phi2_shroud < PI / 3.)
			phi2_shroud = PI / 3.;
		else if (phi2_shroud > PI / 2.)
			phi2_shroud = PI / 2.;
		eprintf("Please check : shroud contour angle at outlet !");
	}

	double RCs_max = (out->Rs - in->Rs) / (cos(phi1_shroud) - sin(0.5 * PI - phi2_shroud));
	
	Double2 pInsec;
	Double2 Z_Axis = { 1., 0. };
	double R_Insec = 0.;
	double Theta = phi2_shroud - phi1_shroud;
	Double2 Pt1_s = { in->Zs, in->Rs };
	pInsec = Pt1_s + (RCs*tan(Theta / 2.))*Z_Axis.rotate(phi1_shroud);
	Double2 Pt2_s = pInsec + (out->Rs - pInsec[1]) / sin(phi2_shroud)*Z_Axis.rotate(phi2_shroud);
	Double2 Pt2_h = Pt2_s + out->b*Z_Axis.rotate(PI / 2 - out->phi);
	if (RCs < 1.0E-5)
	{
		RCs = 0.;
		if (fabs(phi2_shroud - PI / 2) < 1.0E-5)
		{
			axLen_disk = out->b;
		}
		else
		{
			axLen_disk = out->b + (out->Rs - in->Rs) / tan(phi2_shroud);
		}
	}
	else
	{
		if (fabs(phi2_shroud - PI / 2) < tolr1)
		{
			axLen_disk = RCs + out->b;
		}
		else if (RCs < RCs_max)
		{
			double phiCs = 0.5*(PI - phi2_shroud);
			//axLen_disk = RCs / tan(phiCs) + out->b + (out->Rs - in->Rs) / tan(phi2_shroud);
			axLen_disk = Pt2_h[0] - Pt1_s[0];
		}
		else if (RCs >= RCs_max)
		{
			updateContourbaseWithCycle(&RCs, Pt1_s, phi1_shroud, Pt2_s, phi2_shroud, pShroudContour);

			axLen_disk = (Pt2_s - Pt1_s)[0]+ out->b * sin(out->phi);
			//axLen_disk = RCs*sin(phi2_shroud) + out->b *sin(out->phi);
		}
	}
	L_axial = axLen_disk - 0.5*in->b*sin(in->phi) - 0.5*out->b*sin(out->phi);
	LineupComp(in->Za);

	Pt1_s = Double2(in->Zs, in->Rs);
	Pt2_s = Double2(out->Zs, out->Rs);
	Double2 Pt1_h = { in->Zh, in->Rh };
	Pt2_h = Double2(out->Zh, out->Rh);

	double th = 0.5;
	int np = 7;
	double d1 = 0., d2 = 0., u1 = 0.;
	double Rc_scenter = 0., Zc_scenter = 0.;
	// For shroud
	pShroudContour->freeSISLCurve();
	if (RCs < 1.0E-5) 
	{
		updateContourbaseWithStraight(Pt1_s, phi1_shroud, Pt2_s, phi2_shroud, pShroudContour);
	}
	else if (RCs < RCs_max)
	{
		updateContourbaseWithCycle_Straight(Pt1_s, phi1_shroud, &RCs, Pt2_s, phi2_shroud, pShroudContour);
	}
	else
	{
		updateContourbaseWithCycle(&RCs, Pt1_s, phi1_shroud,  Pt2_s, phi2_shroud, pShroudContour);
	}


	// For hub
	// Check
	if (phi1_hub < 0) {
		phi1_hub = 0.;
		eprintf("Please check Hub Contour angle at inlet !");
	}
	Double2 Pt1_h_Checked = { in->Zh, in->Rh };
	Double2 Pt2_h_Checked = { out->Zh, RLE_hub };
	pHubContour1->freeSISLCurve();
	
	//To Check : there 3 situations to check
	if (RCh < tolr1)
	{
		//Calculate the intersection point of two lines of hub contuor.
		double slop_line1 = sin(phi1_hub) / cos(phi1_hub);
		double slop_line2 = sin(phi2_hub) / cos(phi2_hub);
		double line1_b = Pt1_h[1] - slop_line1*Pt1_h[0];
		double line2_b = Pt2_h[1] - slop_line2*Pt2_h[0];
		Double2 Point_line12 = { (line2_b - line1_b) / (slop_line1 - slop_line2),(line2_b - line1_b) / (slop_line1 - slop_line2)*slop_line1 + line1_b };
		if (Pt1_h[1] > Point_line12[1])
		{
	//		eprintf("Please reduce Shroud Contour angle at outlet !");
		//	phi2_shroud = PI / 3.;
		}
		//To calculate min phi2_hub 
		double min_phi2_hub = atan((Pt2_h[1] - Pt1_h[1]) / (Pt2_h[0] - Pt1_h[0]));
		if (phi2_hub > min_phi2_hub)
		{
			updateContourbaseWithStraight_Straight(Pt1_h, phi1_hub, Pt2_h, phi2_hub, pHubContour1);
		}
		else if (phi2_hub != min_phi2_hub)
		{
			phi2_hub = min_phi2_hub;
			updateContourbaseWithStraight(Pt1_h, phi1_hub, Pt2_h, phi2_hub, pHubContour1);
			eprintf("Please check Hub Contour angle at outlet !");
		}
		else
		{
			updateContourbaseWithStraight(Pt1_h, phi1_hub, Pt2_h, phi2_hub, pHubContour1);
		}
	}
	else
	{
		updateContourbaseWithStraight_Cycle_Straight_CheckRC(Pt1_h, phi1_hub, &RCh, Pt2_h, phi2_hub, pHubContour1, Pt1_h_Checked, Pt2_h_Checked);
	}

	pHubContour->freeSISLCurve();
	pHubContour->CopyFrom(pHubContour1);
	pHubContour->Reverse();

	double left[4] = { in->Zh, in->Rh, in->Zs, in->Rs };
	double right[4] = { out->Zs, out->Rs, out->Zh, out->Rh };
	pInlet->freeSISLCurve();
	SET_CURVE_DATA(pInlet, left, 4);
	pOutlet->freeSISLCurve();
	SET_CURVE_DATA(pOutlet, right, 4);

	pMeanContour->Average(pHubContour1, pShroudContour);
	calculateMeanContourAngle();
	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
}

void T1dImpeller::UpdateContourRadialCompressor()
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

  double chub1[10] = { 0. }, chub[10] = { 0. };
  double cshrd[10] = { 0. };
  int nc = 5;

  // shroud
  if (tipContourType == contourType_conical)
  {
    for (int i = 0; i < nc; i++)
    {
      double t = i / (nc - 1.);
      cshrd[2 * i] = (1 - t) * in->Zs + t * out->Zs;
      cshrd[2 * i + 1] = (1 - t) * in->Rs + t * out->Rs;
    }
    SET_CURVE_DATA(pShroudContour, cshrd, SMOOTH_CURVE_POINT);
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
    for (int i = 0; i < nc; i++)
    {
      double t = i / (nc - 1.);
      chub1[2 * i] = (1 - t) * in->Zh + t * out->Zh;
      chub1[2 * i + 1] = (1 - t) * in->Rh + t * out->Rh;
    }
    SET_CURVE_DATA(pHubContour1, chub1, SMOOTH_CURVE_POINT);
    updatePhiFromContourCurve(0);
  }
  else
  {
    contourCurve(in->Zh, in->Rh, phi1_hub, out->Zh, out->Rh, phi2_hub + PI, chub1);
    SET_CURVE_DATA(pHubContour1, chub1, SMOOTH_CURVE_POINT);
  }

  // hub
  pHubContour->CopyFrom(pHubContour1);
  pHubContour->Reverse();

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

void T1dImpeller::UpdateContourSimpleFan()
{
	if (getUseGeomCurve())//use geometry curve
	{
    setGeomUse3DCurve();
		return;
	}

	// version 3: for straght + cycle for shroud and hub

	// validate check 
	// axLen_disk calculation
#if 0
	if (!isUserInput(this, "RCs"))
	{
		RCs = 0.2*out->b;
	}
#endif
	double RCs_max = (out->Rs - in->Rs) / (cos(phi1_shroud) - sin(0.5*PI - phi2_shroud));
	//double RCs_min = (Zc_scenter - in->Zs)*tan(0.5*(PI - phi2_shroud));
	double tolr = 1.0E-5;
	if (RCs < 1.0E-5)
	{
		RCs = 0.;
		if (fabs(phi2_shroud - PI / 2) < 1.0E-5)
		{
			axLen_disk = out->b;
		}
		else
		{
			axLen_disk = out->b + (out->Rs - in->Rs) / tan(phi2_shroud);
		}
	}
	else
	{
		if (RCs > RCs_max)
		{
			RCs = RCs_max;
		}
		if (fabs(phi2_shroud - PI / 2) < 1.0E-5)
		{
			axLen_disk = RCs + out->b;
		}
		else
		{
			double phiCs = 0.5*(PI - phi2_shroud);
			axLen_disk = RCs / tan(phiCs) + out->b + (out->Rs - in->Rs) / tan(phi2_shroud);
		}
	}
	L_axial = axLen_disk - 0.5*in->b*sin(in->phi) - 0.5*out->b*sin(out->phi);
	LineupComp(in->Za);

	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;

	double R2h = out->Rh;
	double R2s = out->Rs;

	double Z2h = out->Zh;
	double Z2s = out->Zs;

	double th = 0.5;
	int np = 7;
	double d1 = 0., d2 = 0., u1 = 0.;
	double Rc_scenter = 0., Zc_scenter = 0.;
	// For shroud
	if (RCs < 1.0E-5)
	{
		int nc = 5;
		Zc_scenter = Z1s + 0.5*(Z2s - Z1s);
		Rc_scenter = R1s + 0.5*(R2s - R1s);
		Double2 Pts[5]; // Huanjin to review, added by Yue
		Pts[0] = Double2(Z1s, R1s);
		Pts[4] = Double2(Z2s, R2s) ;
		for (int i = 1; i<4; i++)
		{
			double t = i / (nc-1.);
			Pts[i] = (1-t)*Pts[0]+ t*Pts[4];
		}
		//double top[] = { Z1s, R1s, Z1s, R1s, Zc_scenter, Rc_scenter, Z2s, R2s, Z2s, R2s };
		//pShroudContour->setInputData(nc, top, 51, 5);
		//nc = 3;
		QVector<double> top(2 * nc); // double *top = new double[2*nc];
		for (int i = 0; i < 5; i++)
		{
			top[2 * i] = Pts[i][0];
			top[2 * i+1] = Pts[i][1];
		}
		pShroudContour->setInputData(nc, top.data(), 51, 5);
		// delete top;
	}
	else
	{
		// , Rc_s1, Zc_s1, Rc_s2, Zc_s2;  //control points on the shroud phi2_shroud
		double n1 = tan(phi1_shroud);//y=nx*b
		double b1 = R1s - n1*Z1s;
		if (phi2_shroud == 0.5*PI)
		{
			Zc_scenter = Z2s;
			Rc_scenter = n1*Zc_scenter + b1;
		}
		else
		{
			double n2 = tan(phi2_shroud);
			double b2 = R2s - n2*Z2s;
			Zc_scenter = (b2 - b1) / (n1 - n2);
			Rc_scenter = n1*Zc_scenter + b1;
		}

		double cshroud[14], cshroud1[14];
		//* 1
		cshroud[0] = in->Zs;
		cshroud[1] = in->Rs;
		//* 4
		cshroud[6] = in->Zs + RCs*sin(phi2_shroud);
		cshroud[7] = in->Rs + RCs*(1 - cos(phi2_shroud));
		// 2
		cshroud[2] = cshroud[0] + th*(Zc_scenter - cshroud[0]);
		cshroud[3] = cshroud[1] + th*(Rc_scenter - cshroud[1]);
		// 3
		cshroud[4] = cshroud[6] + th*(Zc_scenter - cshroud[6]);
		cshroud[5] = cshroud[7] + th*(Rc_scenter - cshroud[7]);
		//* 7
		cshroud[12] = out->Zs;
		cshroud[13] = out->Rs;
		// 5
		cshroud[8] = cshroud[6] + 0.5*(cshroud[12] - cshroud[6]);
		cshroud[9] = cshroud[7] + 0.5*(cshroud[13] - cshroud[7]);
		// 6
		cshroud[10] = cshroud[6] + 0.8*(cshroud[12] - cshroud[6]);;
		cshroud[11] = cshroud[7] + 0.8*(cshroud[13] - cshroud[7]);

		//pShroudContour
		d1 = sqrt(SQR(cshroud[6] - cshroud[0]) + SQR(cshroud[7] - cshroud[1]));
		d2 = sqrt(SQR(cshroud[12] - cshroud[0]) + SQR(cshroud[13] - cshroud[1]));
		u1 = d1 / d2;
		if (u1 > 0.99)
		{
			u1 = 0.99;
		}
		double vknot[11] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
		//pShroudContour->setInputData(np, cshroud, 11, 4, 0, vknot);
		SET_CURVE_DATA(pShroudContour, cshroud, SMOOTH_CURVE_POINT);
	}
	// For hub
#if 1
	double Rc_hcenter = 0., Zc_hcenter = 0., Rc_h1 = 0., Zc_h1 = 0., Rc_h2 = 0., Zc_h2 = 0.;  //control points on the hub
	double nn1 = tan(phi1_hub);//y=nx*b
	double bb1 = R1h - nn1*Z1h;
	if (phi2_hub == 0.5*PI)
	{
		Zc_hcenter = Z2h;
		Rc_hcenter = nn1*Zc_hcenter + bb1;
	}
	else
	{
		double n2 = tan(phi2_hub);
		double b2 = R2h - n2*Z2h;
		Zc_hcenter = (b2 - bb1) / (nn1 - n2);
		Rc_hcenter = nn1*Zc_hcenter + bb1;
	}

#if 0
	//hub control points close to the LE
	Rc_h1 = R1h + 0.3*(Rc_hcenter - R1h);;
	Zc_h1 = Z1h + 0.3*(Zc_hcenter - Z1h);
	//hub control points close to the TE
	Rc_h2 = Rc_hcenter + 0.3*(R2h - Rc_hcenter);
	Zc_h2 = Zc_hcenter + 0.3*(Z2h - Zc_hcenter);

	double RCh_max = cos(phi1_hub)*(RLE_hub - in->Rh);
	double RCh_min = (out->Zh - in->Zh) / (1 - sin(phi1_hub));
	if (RCh > RCh_max) RCh = RCh_max;
	if (RCh < RCh_max) RCh = RCh_min;
	RCh = 0.5*(RCh_max + RCh_min);
	RCh = RCh_min;
#endif
	//phubContour
	double chub[14] = { 0. }, chub1[14] = {0.};

	//* 1
	chub1[0] = in->Zh;
	chub1[1] = in->Rh;
	//* 4
	chub1[6] = Zc_hcenter;
	chub1[7] = Rc_hcenter;
	// 2
	double t = 1. / 3.;
	chub1[2] = (1.-t)*chub1[0] + t*chub1[6];
	chub1[3] = (1.-t)*chub1[1] + t*chub1[7];
	// 3
	t = 2. / 3.;
	chub1[4] = (1. - t)*chub1[0] + t*chub1[6];
	chub1[5] = (1. - t)*chub1[1] + t*chub1[7];
	//* 7
	chub1[12] = out->Zh;
	chub1[13] = out->Rh;
	// 5
	t = 1. / 3.;
	chub1[8] = (1. - t)*chub1[6] + t*chub1[12];
	chub1[9] = (1. - t)*chub1[7] + t*chub1[13];
	// 6
	t = 1. / 3.;
	chub1[10] = (1. - t)*chub1[6] + t*chub1[12];
	chub1[11] = (1. - t)*chub1[7] + t*chub1[13];
	for (int i = 0; i < np; i++)
	{
		chub[2 * i] = chub1[2 * np - 2 * i - 2];
		chub[2 * i + 1] = chub1[2 * np - 2 * i - 1];
	}
	//phubContour1
	d1 = sqrt(SQR(chub1[6] - chub1[0]) + SQR(chub1[7] - chub1[1]));
	d2 = sqrt(SQR(chub1[12] - chub1[0]) + SQR(chub1[13] - chub1[1]));
	u1 = d1 / d2;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double 	vknoth1[11] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pHubContour1->setInputData(np, chub1, 11, 4, 0, vknoth1);
	//phubContour
	d1 = sqrt(SQR(chub[6] - chub[0]) + SQR(chub[7] - chub[1]));
	d2 = sqrt(SQR(chub[12] - chub[0]) + SQR(chub[13] - chub[1]));
	u1 = d1 / d2;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double vknoth[11] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pHubContour->setInputData(np, chub, 11, 4, 0, vknoth);
#else
	double Rc_hcenter, Zc_hcenter, Rc_h1, Zc_h1, Rc_h2, Zc_h2;  //control points on the hub
	double nn1 = tan(phi1_hub);//y=nx*b
	double bb1 = R1h - nn1*Z1h;
	if (phi2_hub == 0.5*PI)
	{
		Zc_hcenter = Z2h;
		Rc_hcenter = nn1*Zc_hcenter + bb1;
	}
	else
	{
		double n2 = tan(phi2_hub);
		double b2 = R2h - n2*Z2h;
		Zc_hcenter = (b2 - bb1) / (nn1 - n2);
		Rc_hcenter = nn1*Zc_hcenter + bb1;
	}

	//hub control points close to the LE
	Rc_h1 = R1h + 0.3*(Rc_hcenter - R1h);;
	Zc_h1 = Z1h + 0.3*(Zc_hcenter - Z1h);
	//hub control points close to the TE
	Rc_h2 = Rc_hcenter + 0.3*(R2h - Rc_hcenter);
	Zc_h2 = Zc_hcenter + 0.3*(Z2h - Zc_hcenter);

	double RCh_max = cos(phi1_hub)*(RLE_hub - in->Rh);
	double RCh_min = (out->Zh - in->Zh) / (1 - sin(phi1_hub));
	if (RCh > RCh_max) RCh = RCh_max;
	if (RCh < RCh_max) RCh = RCh_min;
	RCh = 0.5*(RCh_max + RCh_min);
	RCh = RCh_min;
	//phubContour
	double chub[14], chub1[14];

	//* 1
	chub1[0] = in->Zh;
	chub1[1] = in->Rh;
	//* 4
	chub1[6] = out->Zh;
	chub1[7] = in->Rh + RCh*cos(phi1_hub);
	// 2
	chub1[2] = chub1[0] + th*(Zc_hcenter - chub1[0]);
	chub1[3] = chub1[1] + th*(Rc_hcenter - chub1[1]);
	// 3
	chub1[4] = chub1[6] + th*(Zc_hcenter - chub1[6]);
	chub1[5] = chub1[7] + th*(Rc_hcenter - chub1[7]);
	//* 7
	chub1[12] = out->Zh;
	chub1[13] = out->Rh;
	// 5
	chub1[8] = out->Zh;
	chub1[9] = chub1[7] + 0.5*(chub1[13] - chub1[7]);
	// 6
	chub1[10] = out->Zh;
	chub1[11] = chub1[7] + 0.8*(chub1[13] - chub1[7]);
	for (int i = 0; i < np; i++)
	{
		chub[2 * i] = chub1[2 * np - 2 * i - 2];
		chub[2 * i + 1] = chub1[2 * np - 2 * i - 1];
	}
	//phubContour1
	d1 = sqrt(SQR(chub1[6] - chub1[0]) + SQR(chub1[7] - chub1[1]));
	d2 = sqrt(SQR(chub1[12] - chub1[0]) + SQR(chub1[13] - chub1[1]));
	u1 = d1 / d2;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double 	vknoth1[] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pHubContour1->setInputData(np, chub1, 11, 4, 0, vknoth1);
	//phubContour
	d1 = sqrt(SQR(chub[6] - chub[0]) + SQR(chub[7] - chub[1]));
	d2 = sqrt(SQR(chub[12] - chub[0]) + SQR(chub[13] - chub[1]));
	u1 = d1 / d2;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double vknoth[] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
	pHubContour->setInputData(np, chub, 11, 4, 0, vknoth);



#endif

	double left[4] = { Z1h, R1h, Z1s, R1s };
	pInlet->freeSISLCurve();
	SET_CURVE_DATA(pInlet, left, 4);

	double right[4] = { Z2s, R2s, Z2h, R2h };
	pOutlet->freeSISLCurve();
	SET_CURVE_DATA(pOutlet, right, 4);
	axLen_disk = Z2h - Z1s;//
	pMeanContour->Average(pHubContour1, pShroudContour);

	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
}

void T1dImpeller::UpdateContourWithCycleStraight()
{   
	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;

	double R2h = out->Rh;
	double R2s = out->Rs;
	double Z2h = out->Zh;
	double Z2s = out->Zs;

	Double2 Pt1_s = Double2(in->Zs, in->Rs);
	Double2 Pt2_s = Double2(out->Zs, out->Rs);
	updateContourbaseWithCycle_Straight(Pt1_s, phi1_shroud, &RCs, Pt2_s, phi2_shroud, pShroudContour);
	updateContourbaseWithStraight_Cycle_Straight_CheckRC(Pt1_s, phi1_shroud, &RCs, Pt2_s, phi2_shroud, pShroudContour,
		Pt1_s, Pt2_s, true);
	/*
	updateContourbaseWithStraight_Cycle_Straight_CheckRC(Double2 Pt1, double phi1, double *_Rc, Double2 Pt2, double phi2, TNurbsCurve *pCurve,
	Double2 Pt1_checked, Double2 Pt2_checked, bool isCheck)
	*/

	// hub
	Double2 Pt1_h = { in->Zh, in->Rh};
	Double2 Pt2_h = { out->Zh, out->Rh};
	Double2 Pt1_h_Checked = { in->Zh, in->Rh };
	Double2 Pt2_h_Checked = { out->Zh, RLE_hub };
	pHubContour1->freeSISLCurve();
	//updateContourbaseWithStraight_Cycle_Straight(Pt1_h, phi1_hub, &RCh, Pt2_h, phi2_hub, pHubContour1);
	Double2 Pt3_h_Checked = { out->Zh, out->Rh };
	updateContourbaseWithStraight_Cycle_Straight_CheckRC(Pt1_h, phi1_hub, &RCh, Pt2_h, phi2_hub, pHubContour1, Pt1_h_Checked, Pt3_h_Checked);

	pHubContour->freeSISLCurve();
	pHubContour->CopyFrom(pHubContour1);
	pHubContour->Reverse();

	//Inlet
	double left[4] = { Z1h, R1h, Z1s, R1s };
	SET_CURVE_DATA(pInlet, left, 4);
	//Outlet
	double right[4] = { Z2s, R2s, Z2h, R2h };
	pOutlet->SetOrder(2);
	SET_CURVE_DATA(pOutlet, right, 2);

	pMeanContour->Average(pHubContour1, pShroudContour);

	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
}

void T1dImpeller::UpdateContourOfMixed()
{
	if (getUseGeomCurve())//use geometry curve
	{
    setGeomUse3DCurve();
		return;
	}
	// For Bezier
	bool phi_all_input = (isUserInput(this, "phi1_hub") || isUserInput(this, "phi1_shroud") 
		                  || isUserInput(this, "phi2_hub") || isUserInput(this, "phi2_shroud"));
	bool phi_all_set = (phi1h_set && phi1s_set && phi2h_set && phi2s_set);
	if ( !(phi_all_input || phi_all_set))
	{
		UpdateContourWithLine();
		return;
	}

	//create hub and shroud line
	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;

	double R2h = out->Rh;
	double R2s = out->Rs;
	double Z2h = out->Zh;
	double Z2s = out->Zs;

	double tolr = 1.0E-5;

	double Zv = 0., Rv = 0.;
	double dZ = 0., dR = 0.;
	double dL = 0.;

	// For Shroud
	double Rc_scenter = 0., Zc_scenter = 0., Rc_s1 = 0., Zc_s1 = 0., Rc_s2 = 0., Zc_s2 = 0.;  //control points on the shroud phi2_shroud
	double n1 = tan(phi1_shroud);//y=nx*b
	double b1 = R1s - n1*Z1s;
	if (phi2_shroud == 0.5*PI)
	{
		Zc_scenter = Z2s;
		Rc_scenter = n1*Zc_scenter + b1;
	}
	else
	{
		Zc_scenter = 0.5*(Z1s + Z2s);
		Rc_scenter = 0.5*(R1s + R2s);
	}
	//shroud control points close to the LE  
	Zv = cos(phi1_shroud);
	Rv = sin(phi1_shroud);
	dZ = Zc_scenter - Z1s;
	dR = Rc_scenter - R1s;
	dL = 0.5*(Zv*dZ + Rv*dR);
	Zc_s1 = Z1s + dL*Zv;
	Rc_s1 = R1s + dL*Rv;
	//shroud control points close to the TE
	Zv = cos(phi2_shroud);
	Rv = sin(phi2_shroud);
	dZ = Zc_scenter - Z2s;
	dR = Rc_scenter - R2s;
	dL = 0.5*(Zv*dZ + Rv*dR);
	Zc_s2 = Z2s + dL*Zv;
	Rc_s2 = R2s + dL*Rv;

	// Update calculation of the center point again
	Zc_scenter = 0.5*(Zc_s1 + Zc_s2);
	Rc_scenter = 0.5*(Rc_s1+ Rc_s2);

	// For Hub
	double Rc_hcenter = 0., Zc_hcenter = 0., Rc_h1 = 0., Zc_h1 = 0., Rc_h2 = 0., Zc_h2 = 0.;  //control points on the hub
	double nn1 = tan(phi1_hub);//y=nx*b
	double bb1 = R1h - nn1*Z1h;
	if (phi2_hub == 0.5*PI)
	{
		Zc_hcenter = Z2h;
		Rc_hcenter = nn1*Zc_hcenter + bb1;
	}
	else
	{
		Zc_hcenter = 0.5*(Z1h + Z2h);
		Rc_hcenter = 0.5*(R1h + R2h);
	}

	//hub control points close to the LE
	Zv = cos(phi1_hub);
	Rv = sin(phi1_hub);
	dZ = Zc_hcenter - Z1h;
	dR = Rc_hcenter - R1h;
	dL = 0.5*(Zv*dZ + Rv*dR);
	Zc_h1 = Z1h + dL*Zv;
	Rc_h1 = R1h + dL*Rv;

	//hub control points close to the TE
	Zv = cos(phi2_hub);
	Rv = sin(phi2_hub);
	dZ = Zc_hcenter - Z2h;
	dR = Rc_hcenter - R2h;
	dL = 0.5*(Zv*dZ + Rv*dR);
	Zc_h2 = Z2h + dL*Zv;
	Rc_h2 = R2h + dL*Rv;

	// Update calculation of the center point again
	Zc_hcenter = 0.5*(Zc_h1 + Zc_h2);
	Rc_hcenter = 0.5*(Rc_h1 + Rc_h2);

	int nc = 5;
	double left[4] = { Z1h, R1h, Z1s, R1s };
	SET_CURVE_DATA(pInlet, left, 4);
	{
		double top[] = { Z1s, R1s, Zc_s1, Rc_s1,Zc_scenter, Rc_scenter, Zc_s2, Rc_s2, Z2s, R2s };
		pShroudContour->setInputData(nc, top, 51, 5);
		double bottom[] = { Z2h, R2h, Zc_h2, Rc_h2, Zc_hcenter, Rc_hcenter,Zc_h1, Rc_h1, Z1h, R1h };
		pHubContour->setInputData(nc, bottom, 51, 5);
		//revert hub
		double bottom1[] = { Z1h, R1h,Zc_h1, Rc_h1,Zc_hcenter, Rc_hcenter,Zc_h2, Rc_h2,Z2h, R2h };
		pHubContour1->setInputData(nc, bottom1, 51, 5);
	}
	double right[4] = { Z2s, R2s, Z2h, R2h };
	SET_CURVE_DATA(pOutlet, right, 4);

	pMeanContour->Average(pHubContour1, pShroudContour);

	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
}

void T1dImpeller::UpdateContourWithLine()
{
	if (getUseGeomCurve())//use geometry curve
	{
    setGeomUse3DCurve();
		return;
	}

	// For Bezier
	//create hub and shroud line
	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;

	double R2h = out->Rh;
	double R2s = out->Rs;
	double Z2h = out->Zh;
	double Z2s = out->Zs;

	int nc = 5;
	double ZR_s[10] = { 0. };
	double ZR_h[10] = { 0. };
	double ZR_h1[10] = { 0. };

	// Shroud Contour
	for (int i = 0; i<5; i++)
	{
		double  t = i / 4.;
		ZR_s[2 * i] = (1-t)*Z1s+t*Z2s;
		ZR_s[2*i+1] = (1 - t)*R1s + t*R2s;
	}
	pShroudContour->setInputData(nc, ZR_s, 51, 5);
	// update Hub contour angle
	double temp1 = 0.;
	pShroudContour->getSlope(0, temp1);
	phi1_shroud= atan(temp1);
	pShroudContour->getSlope(1, temp1);
	phi2_shroud = atan(temp1);

	// Hub Contour
	for (int i = 0; i<5; i++)
	{
		double  t = i / 4.;
		ZR_h[2 * i] = (1 - t)*Z2h + t*Z1h;
		ZR_h[2 * i + 1] = (1 - t)*R2h + t*R1h;
	}
	pHubContour->setInputData(nc, ZR_h, 51, 5);

	// Hub Contour 1
	for (int i = 0; i<5; i++)
	{
		double  t = i / 4.;
		ZR_h1[2 * i] = (1 - t)*Z1h + t*Z2h;
		ZR_h1[2 * i + 1] = (1 - t)*R1h + t*R2h;
	}
	pHubContour1->setInputData(nc, ZR_h1, 51, 5);

	// update Hub contour angle
	pHubContour1->getSlope(0, temp1);
	phi1_hub = atan(temp1);
	pHubContour1->getSlope(1, temp1);
	phi2_hub = atan(temp1);

	double left[] = { Z1h, R1h, Z1s, R1s };
	SET_CURVE_DATA(pInlet, left, 4);

	double right[] = { Z2s, R2s, Z2h, R2h };
	SET_CURVE_DATA(pOutlet, right, 4);

	pMeanContour->Average(pHubContour1, pShroudContour);

	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
}

void T1dImpeller::scaling(double sf, bool scale_thick, double min_thick, bool scale_clr, double min_clr)
{
  T1dVaned::scaling(sf, scale_thick, min_thick, scale_clr, min_clr);
  //call seal's scaling function here
  loop_child_have_base_class(c, T1dLeakPath)
  {
    c->scaling(sf, scale_thick, min_thick, scale_clr, min_clr);
  }
  // scalling the shaft --Yihang Guo 2022-11-21
  rearDisk->d_shaft *= sf;
}

void T1dImpeller::UpdateContourDS()
{
  TNurbsCurve* pInletDS = (TNurbsCurve*)child("plot-inlet-DS");
  TNurbsCurve* pShroudContourDS = (TNurbsCurve*)child("plot-shroud-DS");
  TNurbsCurve* pOutletDS = (TNurbsCurve*)child("plot-outlet-DS");
  TNurbsCurve* pHubContourDS = (TNurbsCurve*)child("plot-hub-DS");
  TNurbsCurve* pMeanContourDS = (TNurbsCurve*)child("mean-DS");
  TNurbsCurve* pHubContour1DS = (TNurbsCurve*)child("hub1-DS");
  if (!pInletDS)
    pInletDS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "plot-inlet-DS", this);
  if (!pShroudContourDS)
    pShroudContourDS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "plot-shroud-DS", this);
  if (!pOutletDS)
    pOutletDS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "plot-outlet-DS", this);
  if (!pHubContourDS)
    pHubContourDS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "plot-hub-DS", this);
  if (!pMeanContourDS)
    pMeanContourDS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "mean-DS", this);
  if (!pHubContour1DS)
    pHubContour1DS = (TNurbsCurve*)TObject::new_object("TNurbsCurve", "hub1-DS", this);
  Double2 basePoint = getBasePointForDoubleSuctionImpeller();
  Double2 Normal = { -1., 0. };
  pInletDS->mirror(pInlet, basePoint, Normal);
  pShroudContourDS->mirror(pShroudContour, basePoint, Normal);
  pHubContour1DS->mirror(pHubContour1, basePoint, Normal);
  pHubContourDS->mirror(pHubContour, basePoint, Normal);
  pMeanContourDS->mirror(pMeanContour, basePoint, Normal);

#if 0
  double Z_basePoint = basePoint[0];
  PlotDSCurve(pInlet, pInletDS, Z_basePoint);
  PlotDSCurve(pShroudContour, pShroudContourDS, Z_basePoint);
  PlotDSCurve(pHubContour, pHubContourDS, Z_basePoint);
  PlotDSCurve(pHubContour1, pHubContour1DS, Z_basePoint);
  PlotDSCurve(pOutlet, pOutletDS, Z_basePoint);
  PlotDSCurve(pMeanContour, pMeanContourDS, Z_basePoint);
#endif
}

void T1dImpeller::clearExistingCurve(QString curvePrefix) 
{
  QList<TCurve*> curveList;
  loop_child_have_base_class(c, TCurve)
  {
    if (c->oname().indexOf(curvePrefix) >= 0)
    {
      curveList.push_back(c);
    }
  }
  forLoop(i, curveList.size())
  {
    TObject::delete_object(curveList[i]);
  }
  curveList.clear();
}

void T1dImpeller::createCurve(double* const data, int& index)
{
  QString name = "no-fill-ps" + QString("%1").arg(index++);
  if (TCurve* pPortedShroud = (TCurve*)TObject::new_object("TCurve", name, this))
  {
    SET_OBJ_HIDE(pPortedShroud);
    pPortedShroud->setInputData(2, data, 4);
  }
}

void T1dImpeller::plotPortedShroud()
{
  //clear existing curve
  clearExistingCurve("no-fill-ps");
  if (!hasPortedShroud)
  {
    return;
  }
  double X0 = 0, Y0 = 0;
  pShroudContour->getPoint(0, X0, Y0);
  double Xa = X0 + Z_bsi, Ya = 0;
  bool found = false;
  double Ua = pShroudContour->getUfromX(Xa, found);
  if (found)
  {
    pShroudContour->getPoint(Ua, Xa, Ya);
  }
  double Xb = Xa;
  double Yb = Ya + gap_bsi;
  int i = 0;
  QVector<double> locV = { Xa, Ya, Xb, Yb };
  createCurve(locV.data(), i);

  double Yc = Yb, Xc = 0;
  double Uc = 0;
  T1dInlet* inlet = nullptr;
  TNurbsCurve* inletShroudCurve = nullptr;
  if (TObject* stage = parent("T1dStage"))
  {
    inlet = dynamic_cast<T1dInlet*>(stage->firstChild("T1dInlet"));
    if (!inlet)
      return;
    inletShroudCurve = inlet->pShroudContour;
    if (!inletShroudCurve)
      return;
  }

  double Xd = Xa + gap_bsi, Yd = 0;
  double Ud = pShroudContour->getUfromX(Xd, found);
  if (found)
  {
    pShroudContour->getPoint(Ud, Xd, Yd);
    //find C point
    Uc = inletShroudCurve->getUfromY(Yc, found);
    if (found)
    {
      inletShroudCurve->getPoint(Uc, Xc, Yc);
    }
    else
    {//C point does not intersect with inlet shroud
      Yc = Yb;
      Uc = inletShroudCurve->getUfromY(Yc, found);
      if (found)
      {
        inletShroudCurve->getPoint(Uc, Xc, Yc);
      }
      else
      {
        Xc = X0 - 2 * Z_bsi;
        double xx, yy;
        inletShroudCurve->getPoint(0, xx, yy);
        if (Xc < xx)
          Xc = xx;
      }

      double Xg = Xc, Yg = 0;
      double Ug = inletShroudCurve->getUfromX(Xg, found);
      if (found)
        inletShroudCurve->getPoint(Ug, Xg, Yg);
      locV = { Xc,Yc,Xg,Yg };
      createCurve(locV.data(), i);
    }
    locV = { Xb, Yb,Xc,Yc };
    createCurve(locV.data(), i);

    double Xe = Xd, Ye = Yb + gap_bse;
    locV = { Xd, Yd,Xe,Ye };
    createCurve(locV.data(), i);

    double Yf = Ye, Xf = 0;
    double Uf = inletShroudCurve->getUfromY(Yf, found);
    if (found)
    {
      inletShroudCurve->getPoint(Uf, Xf, Yf);
    }
    else
    {//F does not intersect with inlet shroud
      Xf = Xc - gap_bse;
      double xx, yy;
      inletShroudCurve->getPoint(0, xx, yy);
      if (Xf < xx)
        Xf = xx;
      if (Xf > Xe)
        Xf = Xe;
      double Xh = Xf, Yh = 0;
      double Uh = inletShroudCurve->getUfromX(Xh, found);
      inletShroudCurve->getPoint(Uh, Xh, Yh);
      locV = { Xf, Yf,Xh,Yh };
      createCurve(locV.data(), i);
    }
    locV = { Xe, Ye,Xf,Yf };
    createCurve(locV.data(), i);
  }
  loop_child_have_base_class(c, TCurve)
  {
    if (c->oname().indexOf("no-fill-ps") >= 0)
    {
      c->setOutputLineWidth(3);
      c->setColor("blue");
    }
  }
}

void T1dImpeller::UpdateContourMVR()
{
	//Straght + cycle  + straight for shroud and hub
	// Inserting 2 points of boudary for Shoud and Hub
	// Stragedy : 
	//            
	double R1h = in->Rh;
	double R1s = in->Rs;
	double Z1h = in->Zh;
	double Z1s = in->Zs;

	double R2h = out->Rh;
	double R2s = out->Rs;
	double Z2h = out->Zh;
	double Z2s = out->Zs;

	int np = 10;
	double cshroud[20] = {0.};
	double chub[20] = { 0. }, chub1[20] = { 0. };
	double d1=0., d2=0., u1=0., u2=0., d0=0.;
#if 1
	// Calculate the Radius at shroud
	double phi_centre = phi2_shroud;
	double Rc_scenter = R1s+RCs;
	// Calculate the R of Radius
	double temp_R = Rc_scenter - RCs*cos(phi_centre);
	double delta_R= R2s - temp_R;
	double delta_Z = delta_R*tan(0.5*PI-phi_centre);
	double Zc_scenter = Z2s - delta_Z - RCs*sin(phi_centre);
	double tZ = (Zc_scenter - in->Zs) / (out->Zs - in->Zs);
	double tR = (Rc_scenter - in->Rs) / (out->Rs - in->Rs);

	// Calculate the control poins
	int npt = 21;
	double Z2_center[21] = {0.};
	double R2_center[21] = {0.};

	double d_phi = phi_centre / (20);
	for (int i = 0; i< 21; i++)
	{
		double phi_i = i*d_phi;
		R2_center[i] = Rc_scenter -RCs*cos(phi_i);
		Z2_center[i] = Zc_scenter + RCs*sin(phi_i);
	}
	TNurbsCurve *pContourCycle = new TNurbsCurve;
	pContourCycle->fitBezier(Z2_center, R2_center, npt, 4, 1, 3, 0);
	double * vec_CtrlP1_1 = pContourCycle->GetCtrlPoints(0);
	double * vec_CtrlP1_2 = pContourCycle->GetCtrlPoints(1);

#if 0
	ofstream Contour_Cycle;
	Contour_Cycle.open("D:\\Contour_Cycle.csv", ios::out | ios::trunc);
	Contour_Cycle << vec_CtrlP1_1[1] << "," << vec_CtrlP1_2[1] << endl;
	Contour_Cycle << vec_CtrlP1_1[2] << "," << vec_CtrlP1_2[2] << endl;
	double dt2 = 1./ 20;
	for (int i = 0; i < 21; i++)
	{
		double ti = i*dt2;
		double Zi, Ri;
		pContourCycle->getPoint(ti, Zi, Ri);
		Contour_Cycle << Zi << "," << Ri << "," << Z2_center[i] << "," << R2_center[i] << endl;
	}
	Contour_Cycle.close();
#endif

	//* 0
	cshroud[0] = Z1s;
	cshroud[1] = R1s;
	// point 3
	cshroud[6] = vec_CtrlP1_1[0];
	cshroud[7] = vec_CtrlP1_2[0];
	// point 6
	cshroud[12] = vec_CtrlP1_1[3];
	cshroud[13] = vec_CtrlP1_2[3];
	// Calculate the points 4, 5
#if 0
	// 4
	double degrees = 30 * PI / 180;
	double dZ = RCs*sin(degrees);
	double dR = RCs*(1 - cos(degrees));
	cshroud[8] = cshroud[6] + dZ;
	cshroud[9] = cshroud[7] + dR;
	// 5
	degrees = 60 * PI / 180;
	dZ = RCs*sin(degrees);
	dR = RCs*(1 - cos(degrees));
	cshroud[10] = cshroud[6] + dZ;
	cshroud[11] = cshroud[7] + dR;
#else
	cshroud[8] = vec_CtrlP1_1[1];
	cshroud[9] = vec_CtrlP1_2[1];

	cshroud[10] = vec_CtrlP1_1[2];
	cshroud[11] = vec_CtrlP1_2[2];

#endif
	//1
	double t = 2 / 3.;
	cshroud[2] = t*cshroud[0] + (1 - t)*cshroud[6];
	cshroud[3] = t*cshroud[1] + (1 - t)*cshroud[7];
	//2;
	t = 1 / 3.;
	cshroud[4] = t*cshroud[0] + (1 - t)*cshroud[6];
	cshroud[5] = t*cshroud[1] + (1 - t)*cshroud[7];
	// point 9
	cshroud[18] = Z2s;
	cshroud[19] = R2s;
	//7
	t = 2. / 3;
	cshroud[14] = t*cshroud[12] + (1 - t)*cshroud[18];
	cshroud[15] = t*cshroud[13] + (1 - t)*cshroud[19];
	//8
	t = 1. / 3;
	cshroud[16] = t*cshroud[12] + (1 - t)*cshroud[18];
	cshroud[17] = t*cshroud[13] + (1 - t)*cshroud[19];
	//pShroudContour
	d1 = sqrt(SQR(cshroud[6] - cshroud[0]) + SQR(cshroud[7] - cshroud[1]));
	d2 = sqrt(SQR(cshroud[12] - cshroud[0]) + SQR(cshroud[13] - cshroud[1]));
	d0 = sqrt(SQR(cshroud[18] - cshroud[0]) + SQR(cshroud[19] - cshroud[1]));
	u1 = d1 / d0;
	u2 = d2 / d0;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}

	double vknot[14] = { 0, 0, 0, 0, u1, u1, u1, u2, u2, u2, 1, 1, 1, 1 };
#if 0
	pShroudContour->setInputData(np, cshroud, 11, 4, 0, vknot);
#else
	Double2 Pt1 = { in->Zs, in->Rs };
	Double2 Pt2 = { out->Zs, out->Rs };
	updateContourbaseWithStraight_Cycle_Straight(
		Pt1, phi1_shroud, &RCs, Pt2, phi2_shroud, pShroudContour);
#endif

	double Zs[10] = {0.}, Rs[10] = { 0. };
	for (int i = 0; i<10; i++)
	{
		Zs[i] = cshroud[2 * i];
		Rs[i] = cshroud[2 * i + 1];
	}
#endif
	// For hub
#if 1
	// Use ConeAngle and RatioCone
	if (!isUserInput(this, "RatioCone"))
	{
		RatioCone = 0.5*(R1s+R1h)/R1h;
	}
	if (!isUserInput(this, "ConeAngle"))
	{
		ConeAngle = 45.*PI/180;
	}

	double R_Cone = RatioCone*in->Rh ;
	if (R_Cone > Rc_scenter)
	{
		R_Cone = Rc_scenter;
	}

	double Z1_Cone = Z2h - (R_Cone - R1h) / tan(ConeAngle);
	if (Z1_Cone < Zc_scenter)
	{
		Z1_Cone = Zc_scenter;
	}

	//double th = 0.7; 
	//double ts = 0.7;
	//double dZmh = th*(Z2h - Z1h);
	//RCh = (1 - th)*(Z2h - Z1h);
	//RCh = R_Cone;
	//double Zc_hcenter = dZmh + Z1h;
	//double Rc_hcenter = RCh + R1h;
	// 0
	chub1[0] = Z1h;
	chub1[1] = R1h;
	// point 3
	chub1[6] = Z1_Cone; // <<=
	chub1[7] = R1h;
	// point 6
	chub1[12] = Z2h;
	//chub1[13] = R1h + RCh;
	chub1[13] = R_Cone;

	// 4
	chub1[8] = 2 / 3.*chub1[6] + 1 / 3.*chub1[12];
	chub1[9] = 2 / 3.*chub1[7] + 1 / 3.*chub1[13];
	// 5
	chub1[10] = 1 / 3.*chub1[6] + 2 / 3.*chub1[12];
	chub1[11] = 1 / 3.*chub1[7] + 2 / 3.*chub1[13];

	// Set Mean for points : 
	//1;
	t = 2 / 3.;
	chub1[2] = t*chub1[0] + (1 - t)*chub1[6];
	chub1[3] = t*chub1[1] + (1 - t)*chub1[7];
	//2;
	t = 1. / 3.;
	chub1[4] = t*chub1[0] + (1 - t)*chub1[6];
	chub1[5] = t*chub1[1] + (1 - t)*chub1[7];
	// point 9
	chub1[18] = Z2h;
	chub1[19] = R2h;
	//7
	t = 2. / 3;
	chub1[14] = t*chub1[12] + (1 - t)*chub1[18];
	chub1[15] = t*chub1[13] + (1 - t)*chub1[19];
	//8
	t = 1. / 3;
	chub1[16] = t*chub1[12] + (1 - t)*chub1[18];
	chub1[17] = t*chub1[13] + (1 - t)*chub1[19];
	//pHubContour
	d1 = sqrt(SQR(chub1[6] - chub1[0]) + SQR(chub1[7] - chub1[1]));
	d2 = sqrt(SQR(chub1[12] - chub1[0]) + SQR(chub1[13] - chub1[1]));
	d0 = sqrt(SQR(chub1[18] - chub1[0]) + SQR(chub1[19] - chub1[1]));
	u1 = d1 / d0;
	u2 = d2 / d0;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double vknoth1[14] = { 0, 0, 0, 0, u1, u1, u1, u2, u2, u2,1, 1, 1, 1 };
	pHubContour1->setInputData(np, chub1, 50, 4, 0, vknoth1);
#endif
	for (int i = 0; i < np; i++)
	{
		chub[2 * i] = chub1[2 * np - 2 * i - 2];
		chub[2 * i + 1] = chub1[2 * np - 2 * i - 1];
	}
	//phubContour1
	d1 = sqrt(SQR(chub[6] - chub[0]) + SQR(chub[7] - chub[1]));
	d2 = sqrt(SQR(chub[12] - chub[0]) + SQR(chub[13] - chub[1]));
	d0 = sqrt(SQR(chub[18] - chub[0]) + SQR(chub[19] - chub[1]));
	u1 = d1 / d0;
	u2 = d2 / d0;
	if (u1 > 0.99)
	{
		u1 = 0.99;
	}
	double 	vknoth[] = { 0, 0, 0, 0, u1, u1, u1, u2, u2, u2, 1, 1, 1, 1 };
	pHubContour->setInputData(np, chub, 50, 4, 0, vknoth);

	double Zh[10] = { 0. }, Rh[10] = { 0. };
	for (int i = 0; i<10; i++)
	{
		Zh[i] = chub[2 * i];
		Rh[i] = chub[2 * i + 1];
	}
	axLen_disk = Z2h - Z1s;//
	double left[4] = { Z1h, R1h, Z1s, R1s };
	SET_CURVE_DATA(pInlet, left, 4);

	double right[4] = { Z2s, R2s, Z2h, R2h };
	SET_CURVE_DATA(pOutlet, right, 4);

	pMeanContour->Average(pHubContour1, pShroudContour);

	if (isDoubleSuction)
	{
		UpdateContourDS();
	}
	else if (child("plot-inlet-DS"))
	{
		clearPlotDS();
	}
	delete pContourCycle;
}

void T1dImpeller::UpdateContourNonClog()
{
  // Straight + default bezier for hub, default for shroud, by zhaoj
  double R1h = in->Rh;
  double R1s = in->Rs;
  double Z1h = in->Zh;
  double Z1s = in->Zs;
  double R2h = out->Rh;
  double R2s = out->Rs;
  double Z2h = out->Zh;
  double Z2s = out->Zs;
  double Rsp = in->Rh;
  double Zsp = Z_boss;

  //For Shroud
  double Rc_scenter = 0., Zc_scenter = 0., Rc_s1 = 0., Zc_s1 = 0., Rc_s2 = 0., Zc_s2 = 0.;  //control points on the shroud
  double nn1 = tan(phi1_shroud);//y=nx*b
  double bb1 = R1s - nn1 * Z1s;
  if (phi2_shroud == 0.5*PI)
  {
    Zc_scenter = Z2s;
    Rc_scenter = nn1 * Zc_scenter + bb1;
  }
  else
  {
    double nn2 = tan(phi2_shroud);
    double bb2 = R2s - nn2 * Z2s;
    Zc_scenter = (bb2 - bb1) / (nn1 - nn2);
    Rc_scenter = nn1 * Zc_scenter + bb1;
  }
  //shroud control points close to the LE  
  Rc_s1 = R1s + 0.75*(Rc_scenter - R1s);
  Zc_s1 = Z1s + 0.75*(Zc_scenter - Z1s);
  //shroud control points close to the TE  
  Rc_s2 = Rc_scenter + 0.60*(R2s - Rc_scenter);
  Zc_s2 = Zc_scenter + 0.60*(Z2s - Zc_scenter);

  //for hub contour, five points on straight, five on default, 9 points
  int np = 9;
  double cphub[18] = { 0 };
  double d1 = 0., u1 = 0., d0 = 0.;

  // Known points
  // point 0
  cphub[0] = Z1h;
  cphub[1] = R1h;
  // point 4
  cphub[8] = Zsp;
  cphub[9] = Rsp;
  // point 8
  cphub[16] = Z2h;
  cphub[17] = R2h;

  // Calculate the points 5, 6, 7
  // point 6, control point on the default part
  double Rc_hcenter = 0., Zc_hcenter = 0., Rc_h1 = 0., Zc_h1 = 0., Rc_h2 = 0., Zc_h2 = 0.;
  double n1 = tan(phi_boss);//y=nx*b
  double b1 = cphub[9] - n1 * cphub[8];
  if (phi2_hub == 0.5 * PI)
  {
    Zc_hcenter = Z2h;
    Rc_hcenter = n1 * Zc_hcenter + b1;
  }
  else
  {
    double n2 = tan(phi2_hub);
    double b2 = R2h - n2 * Z2h;
    Zc_hcenter = (b2 - b1) / (n1 - n2);
    Rc_hcenter = n1 * Zc_hcenter + b1;
  }
  cphub[12] = Zc_hcenter;
  cphub[13] = Rc_hcenter;

  // point 5, close to hub le 
  Rc_h1 = R1h + 0.25 * (Rc_hcenter - R1h);
  Zc_h1 = cphub[8] + 0.25 * (Zc_hcenter - cphub[8]);
  cphub[10] = Zc_h1;
  cphub[11] = Rc_h1;

  // point 7, close to hub te
  Rc_h2 = Rc_hcenter + 0.60 * (R2h - Rc_hcenter);
  Zc_h2 = Zc_hcenter + 0.60 * (Z2h - Zc_hcenter);
  cphub[14] = Zc_h2;
  cphub[15] = Rc_h2;

  // calculate points on straight
  // point 1
  double t = 3 / 4.;
  cphub[2] = t * cphub[0] + (1 - t) * cphub[8];
  cphub[3] = t * cphub[1] + (1 - t) * cphub[9];
  // point 2;
  t = 2 / 4.;
  cphub[4] = t * cphub[0] + (1 - t) * cphub[8];
  cphub[5] = t * cphub[1] + (1 - t) * cphub[9];
  // point 3;
  t = 1 / 4.;
  cphub[6] = t * cphub[0] + (1 - t) * cphub[8];
  cphub[7] = t * cphub[1] + (1 - t) * cphub[9];

  //pHubContour
  d1 = sqrt(SQR(cphub[8] - cphub[0]) + SQR(cphub[9] - cphub[1]));
  d0 = sqrt(SQR(cphub[16] - cphub[0]) + SQR(cphub[17] - cphub[1]));

  TNurbsCurve* pCurve1 = new TNurbsCurve;
  TNurbsCurve* pCurve2 = new TNurbsCurve;
  QVector<Double2> pts; pts.resize(9);
  {

    for (int i = 0; i < 9; i++)
    {
      pts[i] = Double2(cphub[2 * i], cphub[2 * i + 1]);
    }

    int mid = 4;
    // pCurve1
    {
      int np1 = 5;
			QVector<double> pxy(2 * np1); // double *pxy = new double[2 * np1];
      for (int i = 0; i < np1; i++)
      {
        pxy[2 * i] = pts[i][0];
        pxy[2 * i + 1] = pts[i][1];
      }
      pCurve1->setInputData(np1, pxy.data(), 51, 3);
      // delete pxy;
    }
    // pCurve2
    {
      int np1 = 5;
			QVector<double> pxy(2 * np1); // double *pxy = new double[2 * np1];
      for (int i = 4; i < 9; i++)
      {
        int ii = i - 4;
        pxy[2 * ii] = pts[i][0];
        pxy[2 * ii + 1] = pts[i][1];
      }
      pCurve2->setInputData(np1, pxy.data(), 51, 3);
      // delete pxy;
    }
  }
#if 0
  ofstream Hub;
  Hub.open("D:\\Hub.csv", ios::out | ios::trunc);

  for (int i = 0; i < 18; i += 2)
  {
    double Z, R;

    Z = cphub[i];
    R = cphub[i + 1];

    Hub << Z << "," << R << endl;
  }

  Hub.close();

#endif

  u1 = d1 / d0;
  if (u1 > 0.99)
  {
    u1 = 0.99;
  }
  double vknoth1[14] = { 0, 0, 0, 0, u1, u1, u1, 1, 1, 1, 1 };
  pHubContour1->setInputData(np, cphub, 51, 4, 0);


  {
    pHubContour1->Concatenate(pCurve1, pCurve2, pts[4]);
  }

  delete pCurve1;
  delete pCurve2;

  pHubContour->CopyFrom(pHubContour1);
  pHubContour->Reverse();

  int nc = 5;
  double left[4] = { Z1h, R1h, Z1s, R1s };
  SET_CURVE_DATA(pInlet, left, 4);
  {
    double top[] = { Z1s, R1s, Zc_s1, Rc_s1,Zc_scenter, Rc_scenter, Zc_s2, Rc_s2, Z2s, R2s };
    pShroudContour->setInputData(nc, top, 51, 5);
  }
  double right[4] = { Z2s, R2s, Z2h, R2h };
  SET_CURVE_DATA(pOutlet, right, 4);

  pMeanContour->Average(pHubContour1, pShroudContour);
  calculateMeanContourAngle();

  if (isDoubleSuction)
  {
    UpdateContourDS();
  }
  else if (child("plot-inlet-DS"))
  {
    clearPlotDS();
  }

}

void T1dImpeller::UpdateContourOverHang()
{
  /*
                               * 9
                               |
                               *
                               |
                 5 *-----*-----* 7
                   |
                   *
                   |
    *----*----*----*
    0              3

  */

  // three curves ,default for shroud, by zhaoj
  double R_0 = in->Rh;
  double Z_0 = in->Zh;
  double R_3 = in->Rh;
  double Z_3 = Z_boss;
  double R_5 = R_boss;
  double Z_5 = Z_boss;
  double phi_5 = phi_boss;
  double R_9 = out->Rh;
  double Z_9 = out->Zh;

  TNurbsCurve *pCurve12 = new TNurbsCurve;
  TNurbsCurve *pCurve1 = new TNurbsCurve;
  TNurbsCurve *pCurve2 = new TNurbsCurve;
  TNurbsCurve *pCurve3 = new TNurbsCurve;
  QVector<Double2> pts; pts.resize(10);

  double R1h = in->Rh;
  double R1s = in->Rs;
  double Z1h = in->Zh;
  double Z1s = in->Zs;
  double R2h = out->Rh;
  double R2s = out->Rs;
  double Z2h = out->Zh;
  double Z2s = out->Zs;
  double Rsp = in->Rh;
  double Zsp = Z_boss;

  //For Shroud
  double Rc_scenter = 0., Zc_scenter = 0., Rc_s1 = 0., Zc_s1 = 0., Rc_s2 = 0., Zc_s2 = 0.;  //control points on the shroud
  double nn1 = tan(phi1_shroud);//y=nx*b
  double bb1 = R1s - nn1 * Z1s;
  if (phi2_shroud == 0.5*PI)
  {
    Zc_scenter = Z2s;
    Rc_scenter = nn1 * Zc_scenter + bb1;
  }
  else
  {
    double nn2 = tan(phi2_shroud);
    double bb2 = R2s - nn2 * Z2s;
    Zc_scenter = (bb2 - bb1) / (nn1 - nn2);
    Rc_scenter = nn1 * Zc_scenter + bb1;
  }
  //shroud control points close to the LE  
  Rc_s1 = R1s + 0.75*(Rc_scenter - R1s);
  Zc_s1 = Z1s + 0.75*(Zc_scenter - Z1s);
  //shroud control points close to the TE  
  Rc_s2 = Rc_scenter + 0.60*(R2s - Rc_scenter);
  Zc_s2 = Zc_scenter + 0.60*(Z2s - Zc_scenter);

  //For Hub
  int np = 10;
  double cpz[10] = { 0 }, cpr[10] = { 0 };
  // point 0
  cpz[0] = Z_0;
  cpr[0] = R_0;
  // point 3
  cpz[3] = Z_3;
  cpr[3] = R_3;
  // point 5
  cpz[5] = Z_5;
  cpr[5] = R_5;
  // point 9
  cpz[9] = Z_9;
  cpr[9] = R_9;

  // Calculate the points 6, 7, 8
  // point 7, on the default part
  double Rc_hcenter = 0., Zc_hcenter = 0., Rc_h1 = 0., Zc_h1 = 0., Rc_h2 = 0., Zc_h2 = 0.;
  double a1 = tan(phi_5); //y = ax + b
  double b1 = cpr[5] - a1 * cpz[5];
  if (phi2_hub == 0.5 * PI)
  {
    Zc_hcenter = Z2h;
    Rc_hcenter = a1 * Zc_hcenter + b1;
  }
  else
  {
    double a2 = tan(phi2_hub);
    double b2 = R2h - a2 * Z2h;
    Zc_hcenter = (b2 - b1) / (a1 - a2);
    Rc_hcenter = a1 * Zc_hcenter + b1;
  }
  cpz[7] = Zc_hcenter;
  cpr[7] = Rc_hcenter;
  // point 6, close to hub le 
  cpr[6] = cpr[5] + 0.7 * (Rc_hcenter - cpr[5]);
  cpz[6] = cpz[5] + 0.7 * (Zc_hcenter - cpz[5]);
  // point 8, close to hub te
  cpr[8] = Rc_hcenter + 0.60 * (R2h - Rc_hcenter);
  cpz[8] = Zc_hcenter + 0.60 * (Z2h - Zc_hcenter);
  // real p7
  cpr[7] = (cpr[6] + cpr[8]) / 2.;

  // point on 1st line
  // point 1
  double t = 2 / 3.;
  cpz[1] = t * cpz[0] + (1 - t) * cpz[3];
  cpr[1] = t * cpr[0] + (1 - t) * cpr[3];
  // point 2;
  t = 1 / 3.;
  cpz[2] = t * cpz[0] + (1 - t) * cpz[3];
  cpr[2] = t * cpr[0] + (1 - t) * cpr[3];
  // point in 2st line
  // point 4;
  cpz[4] = (cpz[3] + cpz[5]) / 2.;
  cpr[4] = (cpr[3] + cpr[5]) / 2.;

  for (int i = 0; i < 9; i++)
  {
    pts[i] = Double2(cpz[i], cpr[i]);
  }
  int mid = 4;
  // pCurve1
  int np1 = 4;
	QVector<double> pxy1(2 * np1); // double *pxy1 = new double[2 * np1];
  for (int i = 0; i < np1; i++)
  {
    pxy1[2 * i] = cpz[i];
    pxy1[2 * i + 1] = cpr[i];
  }
  pCurve1->setInputData(np1, pxy1.data(), 51, 3);

#if 0
  ofstream Hub;
  Hub.open("D:\\Hub.csv", ios::out | ios::trunc);

  for (int i = 0; i < 4; i++)
  {
    double Z, R;
    Z = cpz[i];
    R = cpr[i];
    Hub << Z << "," << R << endl;
  }
  Hub.close();
#endif
  // pCurve2
  int np2 = 3;
	QVector<double> pxy2(2 * np2); // double *pxy2 = new double[2 * np2];
  for (int j = 3; j < 6; j++)
  {
    int jj = j - 3;
    pxy2[2 * jj] = cpz[j];
    pxy2[2 * jj + 1] = cpr[j];
  }
  pCurve2->setInputData(np2, pxy2.data(), 51, 3);

#if 0
  ofstream Hub2;
  Hub2.open("D:\\Hub2.csv", ios::out | ios::trunc);
  for (int i = 3; i < 6; i++)
  {
    double Z, R;
    Z = cpz[i];
    R = cpr[i];
    Hub2 << Z << "," << R << endl;
  }
  Hub2.close();
#endif
  // pCurve3
  int np3 = 5;
	QVector<double> pxy3(2 * np3); // double *pxy3 = new double[2 * np3];
  for (int k = 5; k < 10; k++)
  {
    int kk = k - 5;
    pxy3[2 * kk] = cpz[k];
    pxy3[2 * kk + 1] = cpr[k];
  }

#if 0
  ofstream Hub3;
  Hub3.open("D:\\Hub3.csv", ios::out | ios::trunc);
  for (int i = 5; i < 10; i++)
  {
    double Z, R;
    Z = cpz[i];
    R = cpr[i];
    Hub3 << Z << "," << R << endl;
  }
  Hub3.close();
#endif
  pCurve3->setInputData(np3, pxy3.data(), 51, 3);
  pCurve12->Concatenate(pCurve1, pCurve2, pts[3]);
  pHubContour1->Concatenate(pCurve12, pCurve3, pts[5]);

  delete pCurve12;
  delete pCurve1;
  delete pCurve2;
  delete pCurve3;
  pHubContour->CopyFrom(pHubContour1);
  pHubContour->Reverse();
  int nc = 5;
  double left[4] = { Z1h, R1h, Z1s, R1s };
  SET_CURVE_DATA(pInlet, left, 4);
  {
    double top[] = { Z1s, R1s, Zc_s1, Rc_s1,Zc_scenter, Rc_scenter, Zc_s2, Rc_s2, Z2s, R2s };
    pShroudContour->setInputData(nc, top, 51, 5);
  }
  double right[4] = { Z2s, R2s, Z2h, R2h };
  SET_CURVE_DATA(pOutlet, right, 4);
  pMeanContour->Average(pHubContour1, pShroudContour);
  calculateMeanContourAngle();

  if (isDoubleSuction)
  {
    UpdateContourDS();
  }
  else if (child("plot-inlet-DS"))
  {
    clearPlotDS();
  }
}

void T1dImpeller::FindU(double& u1s, double& u1h, double& u2s, double& u2h)
{
  bool found = false;
  double Zs1 = 0., Rs1 = 0., Rs2 = 0., Zh1 = 0., Rh1 = 0., Rh2 = 0.;
  if (isLERadius)
  {
    u1h = pHubContour1->getUfromY(RLE_hub, found);
    u1s = pShroudContour->getUfromY(RLE_shroud, found);
  }
  else
  {
    u1h = ule_hub;
    u1s = ule_shroud;
    pHubContour1->getPoint(ule_hub, Zh1, RLE_hub);
    pShroudContour->getPoint(ule_shroud, Zs1, RLE_shroud);
  }
  Rs1_RsE = RLE_shroud / in->Rs;
  Rh1_RhE = RLE_hub / in->Rh;

  if (Rs2_RsO > 1) Rs2_RsO = 1;
  if (Rh2_RhO > 1) Rh2_RhO = 1;
  Rs2 = Rs2_RsO*out->Rs;
  Rh2 = Rh2_RhO*out->Rh;
  u2s = pShroudContour->getUfromY(Rs2, found);
  u2h = pHubContour1->getUfromY(Rh2, found);
}

void T1dImpeller::calPump_ule_hub()
{
  T1dStage* pStage = dynamic_cast<T1dStage *>(parent("T1dStage"));
  T1dMachine *pMachine = (T1dMachine *)pStage->parent("T1dMachine");
  bool isTorque = pMachine->AxRad == T1dMachine::exit_type_radial;
  int npt = 101;

  if (isUserInput(this, "ule_shroud") && (ule_shroud > 0.4))
    ule_shroud = 0.4;

  double delta_tip = 0;
  double tip_torque = 0;
  double du_s = (1.0 - ule_shroud) / (npt - 1);
  double Z0_s = 0., R0_s = 0.;
  double Z1_s = 0., R1_s = 0.;
  pShroudContour->getPoint(ule_shroud, Z0_s, R0_s);
  pShroudContour->getPoint(ute_shroud, Z1_s, R1_s);

  // cal shroud length or static torque
  for (int j = 1; j < npt; j++)
  {
    double Zp_s = 0., Rp_s = 0., R_s = 0.;
    double u_sj = ule_shroud + j * du_s;
    pShroudContour->getPoint(u_sj, Zp_s, Rp_s);
    double dM_s = sqrt(SQR(Zp_s - Z0_s) + SQR(Rp_s - R0_s));
    R_s = 0.5 * (Rp_s + R0_s);
    delta_tip += dM_s; //
    tip_torque += dM_s * R_s; //
    Z0_s = Zp_s;
    R0_s = Rp_s;
  }

  double u_start = 0;
  double length_tip = delta_tip;
  double length_hub = 0;
  double torque_tip = tip_torque;
  double du_h = 1. / (npt - 1);
  double Z1_h = 0., R1_h = 0.;
  pHubContour->getPoint(u_start, Z1_h, R1_h);
  double torque_hub = 0;
  double k_lengthSafe = 0.8;
  double k_torqueSafe = 1.0;
  double _ule_hub = 0.;
  for (int i = 1; i < npt; i++)
  {
    double Z2_h = 0., R2_h = 0., R_h = 0.;
    double u_h = u_start + i * du_h;
    pHubContour->getPoint(u_h, Z2_h, R2_h);
    double dM_h = sqrt(SQR(Z2_h - Z1_h) + SQR(R2_h - R1_h));
    R_h = 0.5 * (R2_h + R1_h);
    double ds = dM_h;
    length_hub += ds;
    torque_hub += dM_h * R_h;
    if (isTorque) {
      if (torque_hub >= k_torqueSafe * torque_tip)
      {
        _ule_hub = 1 - u_h;
        break;
      }
    }
    else {
      if (length_hub >= k_lengthSafe * length_tip)
      {
        _ule_hub = 1 - u_h;
        break;
      }
    }
    Z1_h = Z2_h;
    R1_h = R2_h;
  }

  sta_le->Rh = R1_h;
  if (!isUserInput(this, "ule_hub"))
  {
    ule_hub = _ule_hub;
    if (!isTorque)
      if (ule_hub > 0.55)
        ule_hub = 0.55;
  }
}

void T1dImpeller::getBladeSetupPropList(QStringList& propList, QStringList& readOnlyPropList, QMap<QString,QString>& dispMapList)
{
  QStringList PL = QStringList();//property list
  QStringList ROPL = QStringList();
  if (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Type) // SCA_radial
  {
    PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    //if (iSection > 0) ROPL << "beta1b" << "beta2b";
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_Straight_Type) // Straight flat
  {
    PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    ROPL << "beta1b";
    //if (iSection > 0) ROPL << "beta2b";
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_DCA_Type) // DCA_radial
  {
    PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    //PL << "Rm" << "beta1b" << "betamb" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    //if (iSection > 0) ROPL << "Rm" << "beta1b" << "betamb" << "beta2b";
  }
  else if (blade->bladeType == blade->bladeTypes::Compisition_Type) // Axial to radial 
  {
    if (isUserWrapAngleDefine)
      PL << "wrapAngle";
		PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
#if (_TT_DEBUG)
		PL << "kFactor1" << "kFactor2";
#endif
    //if (iSection == 0)ROPL << "beta1b";
    //else if (iSection == 1)ROPL << "beta1b" << "beta2b";
    //else if (iSection == 2)ROPL << "beta2b";
  }
  else if (blade->bladeType == blade->bladeTypes::Radial_2D_SCA_Straight_Type) // SCA_radial
  {
    PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    //PL << "Rm" << "beta1b" << "betamb" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
    //ROPL << "betamb";
    //if (iSection > 0) ROPL << "Rm" << "beta1b" << "beta2b";
  }
  else if (blade->bladeType == blade->bladeTypes::Ruled_3D_Free_Type) // Ruled_3D_Free_Type
	{
		PL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
	}  
	else if (blade->bladeType == blade->bladeTypes::Axial_Element_Free_Type) // Axial_Element_Free_Type
	{
		PL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
	}
	else if (blade->bladeType == blade->bladeTypes::Free_3D_Type) // Free_3D_Type
	{
		PL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
	}
	else if (blade->bladeType == blade->bladeTypes::blade_usebladeCurves) // blade_usebladeCurves
	{
		PL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
		ROPL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
	}
  else
  {
		if (isUserWrapAngleDefine)
			PL << "beta1b" << "beta2b" << "wrapAngle" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
		else
			PL << "beta1b" << "beta2b" << "thick1" << "thick2" << "thickMax" << "thickmax_loc_chord";
  }

	if (getUseGeomCurve())
		ROPL << "wrapAngle" << "thickMax" << "thickmax_loc_chord";

  forLoop(i, PL.size())
  {
    propList.push_back(PL[i]);
  }

  foreach(QString s, ROPL)
  {
    readOnlyPropList.push_back(s);
  }
}

void T1dImpeller::adjustStaticTorque()
{
	// Curremtly just applied for Axial to radial of Fan

	double torque_hub = 0., torque_shroud = 0.;
	double x0 = 0., x1 = 0., y0 = 0., y1 = 0.;
	for (int i = 0; i< 50; i++)
	{
		calculateStaticTorque(torque_hub, torque_shroud);
		double delta_StaticTorque = (torque_hub - torque_shroud) / torque_hub;
		if (delta_StaticTorque > 0.02)
		{
			break;
		}
		double multiplier = 1.;
		bool lastTry = AdjustBeta2bOfFan(i, delta_StaticTorque, phi2_shroud, 1., x0, x1, y0, y1);
		UpdateContour();
	}
}

void T1dImpeller::calculateStaticTorque(double &torque_hub, double &torque_shroud)
{
	int npt = 101;
	// shroud 
	double du_shroud = (1.0 - ule_shroud) / (npt - 1);
	double Z1_shroud = 0., R1_shroud = 0.;
	pShroudContour->getPoint(ule_shroud, Z1_shroud, R1_shroud);
	for (int i = 1; i < npt; i++)
	{
		double Z2_shroud = 0., R2_shroud = 0., R_shroud = 0.;
		double u_shroud = ule_shroud + i*du_shroud;
		pShroudContour->getPoint(u_shroud, Z2_shroud, R2_shroud);
		R_shroud = 0.5*(R1_shroud + R2_shroud);
		double dM_shroud = sqrt(SQR(Z2_shroud - Z1_shroud) + SQR(R2_shroud - R1_shroud));
		double dtorque = dM_shroud*R_shroud;
		torque_shroud  += dtorque;
		Z1_shroud = Z2_shroud;
		R1_shroud = R2_shroud;
	}

	// Hub
	double du_hub = (1.0 - ule_hub) / (npt - 1);
	double Z1_hub = 0., R1_hub = 0.;
	pHubContour1->getPoint(ule_hub, Z1_hub, R1_hub);
	for (int i = 1; i < npt; i++)
	{
		double Z2_hub = 0., R2_hub = 0., R_hub = 0.;
		double u_hub = ule_hub + i*du_hub;
		pHubContour1->getPoint(u_hub, Z2_hub, R2_hub);
		R_hub = 0.5*(R1_hub + R2_hub);
		double dM_hub = sqrt(SQR(Z2_hub - Z1_hub) + SQR(R2_hub - R1_hub));
		double dtorque = dM_hub*R_hub;
		torque_hub += dtorque;
		Z1_hub = Z2_hub;
		R1_hub = R2_hub;
	}

	double delta_StaticTorque_HubShroud = (torque_hub - torque_shroud) / torque_hub;
	return;
}

bool T1dImpeller::isMultiVane()
{
	bool multiVane = false;
	if (T1dMachine *pMachine = (T1dMachine *)parent("T1dMachine"))
		if ((pMachine->machineType == T1dMachine::MachType_Fan) && (pMachine->AppType == T1dMachine::AppType_MultiVane))
			multiVane = true;

	return multiVane;
}

void T1dImpeller::reportOutput(QString prefix, bool child)
{
	if (!child)
	{
		textReport.clear();
		QString s = prefix;
		QString cName = getComponentType(nType);
		QString cUName = cName.left(1).toUpper() + cName.right(cName.size() - 1);
		QString componentType = s + cUName + QObject::tr(" geometry and property report");
		util_HtmlReportTitle title;
		title.addItem(componentType, 1, 4);
		textReport << title.toText();
	}
  QString m;
  switch (isShrouded) {//0: analysis 1:design 2: data reduction
  case 0: m = QObject::tr("No"); break;
  case 1: m = QObject::tr("Yes"); break;
  }
  QString sTitle = QObject::tr("Is shrouded: ") + m;
	util_OUtil::makeTitle(textReport, sTitle);

  QString MachineType = GetMachineType();
  bool isPump = (MachineType == "pump");
  if (isPump)
  {
    QString m;
    switch (isDoubleSuction) {//0: analysis 1:design 2: data reduction
    case 0: m = QObject::tr("No"); break;
    case 1: m = QObject::tr("Yes"); break;
    }
    sTitle = QObject::tr("Is double suction: ") + m;
    util_OUtil::makeTitle(textReport, sTitle);
  }

  const char *sname = TObject::shortName("T1dObject");
  TObject* oned = CURRENT_STUDY->object(sname);
  TObject* pSolver= oned->firstChild("T1dSolverOption",true);
  pSolverSetting = (T1dSolverOption*)pSolver;
  if (pSolverSetting && (pSolverSetting->runMode == T1dSolverOption::mode_design))
  {    
    QStringList pList;
    if (R1hOption == 0)
      pList << "Rh_Rs";
    if (isPump)
    {
      /*if (inletDesignOption == 0)
        pList << "fdS";
      else */
      if (inletDesignOption == 2)
        pList << "Km1";
      else if (inletDesignOption == 3)
      {
        pList << "Lamda_c";
        pList << "Lamda_w";
      }
      else if (inletDesignOption == 4)
        pList << "K0";

      if (OutletBOption == 1)
        pList << "b2d2";
      else if (OutletBOption == 2)
        pList << "Km2";

      if (OutletRaOption == 1)
        pList << "workCoeff_d";
    }
    QStringList slData;
    QString D;
    QString disp_name, value, unit;
    if (pList.size())
    {
      sTitle = QObject::tr("User design parameter input:");
      textReport << "<br><font size=""3"" ><strong>" + sTitle + "</strong></font><br>";
      forLoop(i, pList.size())
      {
        if (i == 0 && R1hOption == 0)
        {
          in->getPropertyWithUnit(pList[i], disp_name, value, unit);
          D = disp_name + " (Inlet)" + ";" + value + ";" + unit;
        }
        else
        {
          getPropertyWithUnit(pList[i], disp_name, value, unit);
          D = disp_name + ";" + value + ";" + unit;
        }
        slData << D;
      }
      util_OUtil::makeTable(textReport, QObject::tr("Property name") + ";" + QObject::tr("Value") + ";" + QObject::tr("Unit"), slData);
      sTitle = "";
      textReport << "<br><font size=""4"" ><strong>" + sTitle + "</strong></font><br>";
    }
  }

	T1dComponent::reportOutput(prefix, true);
}

void T1dImpeller::GetReportPropertyList(QVector<property_t*>& properties, int mach_type, bool includeGeom)
{
	QString MachineType = GetMachineType();
	bool isPump = (MachineType == "pump");
	bool isFan = (MachineType == "fan");
  QStringList L = QStringList() << "isStalled" << "textReport"
    << "RPM"
    << "Mu"
    << "incid_hub"
    << "incid_mean"
    << "incidence"
    << "incid_tip"
    << "deviation"
    << "loadingTE"
    << "loadingAvg"
    << "mChoke"
    << "Ath_Astar"
    << "Deq"
    << "Ma_Wmax"
    << "Wmax"
	<< "Ath_Astar"
    << "slipFactor"
    << "devOffset"
    << "L_axial"
    << "blade/numMainBlade"
    << "blade/numSplitterRows"
    << "blade/hubSect/NB2"
    << "ThroatAreaInlet"
    << "ThroatAreaOutlet"
    << "axF_total"
    << "axF_shroud"
    << "axF_back"
    << "axF_inlet"
    << "axF_outlet"
    << "mLeakF"
    << "mLeakR"
    << "power"
    << "Peuler"
    << "Precirc"
    << "PleakF"
    << "PleakR"
    << "Pdisk"
    << "Pparasitic"
    << "eta_tt_rotor_ad"
    << "eta_ts_rotor_ad"
    << "LC_inc"
    << "LC_frict"
    << "LC_load"
    << "LC_distort"
    << "LC_mix"
    << "LC_shock"
    << "LC_clr"
    << "LC_choke"
    << "LC"
    << "lc_offset"
    << "deltaEta"
    << "deltaEta_parasitic"
    << "p_parasitic_offset";
	if (isPump)
	{
    L += QStringList() << "Lamda_w" << "Lamda_w_offset"; // TODO : add caviation parameters
	}
  if (isPump || isFan)
    L += QStringList() << "ule_hub" << "ule_shroud" << "RLE_hub" << "RLE_shroud";
  if (hasPortedShroud)
  {
    L += QStringList() << "isBypass" << "power_ported_shroud" << "mdot_bc";
  }


  QStringList bladeSectionList = QStringList() << "beta1b"
    << "beta2b"
    << "thick1"
    << "thick2";
  loop_o_child_have_base_class(blade, bld, T1dBladeSection)
  {
    if (bld->oname() == "baseSect")continue;
    forLoop(i, bladeSectionList.size())
    {
      QString pname = blade->oname() + "/" + bld->oname() + "/" + bladeSectionList[i];
      L += pname;
    }
  }

	forLoop(i, L.size())
	{
		if (property_t* p = property(L[i]))
			properties.push_back(p);
	}
	in->GetReportPropertyList(properties, mach_type, includeGeom);
  sta_le->GetReportPropertyList(properties, mach_type, includeGeom);
	sta_te->GetReportPropertyList(properties, mach_type, includeGeom);
	out->GetReportPropertyList(properties, mach_type, includeGeom);
	rearLeakPath->GetReportPropertyList(properties, mach_type, includeGeom);
	if(frontLeakPath)
		frontLeakPath->GetReportPropertyList(properties, mach_type, includeGeom);
	return;
}

void T1dImpeller::GetConfigPropertyList(QVector<property_t*>& properties)
{
	T1dComponent::GetConfigPropertyList(properties);
}

void T1dImpeller::GetConfigPassagePropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
  T1dComponent::GetConfigPassagePropertyList(properties, source);
  QStringList L;
  if (T1dMachine* pMachine = T1dMachine::getCurrentMachine())
  {
    QString MachineType = pMachine->getMachineTypeString();
    bool isFan = (MachineType == "fan");
    bool isPump = (MachineType == "pump");
    bool isMixedCompressor = ((MachineType == "compressor") && (pMachine->AxRad == T1dMachine::exit_type_mixed));
    bool isOverhang = (pMachine->AppType == T1dMachine::AppType_Non_Clogging_Pump);
    if (isFan)
    {
      if (pMachine->AppType == T1dMachine::AppType_MVR)
      {
        L.push_back("RCs");
        L += QStringList() << "RatioCone" << "ConeAngle";
      }
      else if (pMachine->AppType == T1dMachine::AppType_MultiVane)
      {
        L.push_back("axLen_disk");
        L.push_back("RCs");
      }
	  else if(pMachine->AppType == T1dMachine::AppType_3D_Vaned_Fan)
			{
//				L.push_back("axLen_disk");
//				L.push_back("RCs");
				L.push_back("RCs");
				L.push_back("RCh");
	  }
      else
      {
        L.push_back("axLen_disk");
        L.push_back("RCs");
        L.push_back("RCh");
      }
    }
    if (isPump && contourType == 1)
    {
      L += QStringList() << "RCs" << "RCh";
    }
    if (isOverhang)
    {
      L += QStringList() << "R_boss" << "L_boss" << "phi_boss";
    }
    if (isPump || isFan || isMixedCompressor)
    {
      L += QStringList() << "phi1_hub" << "phi1_shroud" << "phi2_hub" << "phi2_shroud";

    }

    // 
    if (isFan)
    {
      if (pMachine->AppType == T1dMachine::AppType_MVR && source == 0)
      {
        SET_PROPERTY_READONLY(this->property("phi2_hub"));
      }
      else if (blade->bladeType == blade->Ruled_3D_Type  && source == 0)
      {
        SET_PROPERTY_WRITABLE(this->property("phi2_hub"));
        SET_PROPERTY_WRITABLE(this->property("L_axial"));
      }
      else
      {
        if (source == 0)
        {
          //SET_PROPERTY_READONLY(this->property("phi2_hub"));
          SET_PROPERTY_READONLY(this->property("L_axial"));
          SET_PROPERTY_READONLY(this->property("axLen_disk"));
        }
      }
    }
  }

  int solverMode = 0;
  if (T1dObject* o = dynamic_cast<T1dObject*>(CURRENT_MODULE_OBJECT))
  {
    solverMode = o->getSolverMode();
  }
  bool inputRatio = source == 0 ? getTag("_bInputRatio").toBool() : this->inputRatio;
  if (solverMode == 0)//analysis mode
  {
    // L += QStringList() << "phi1_hub" << "phi1_shroud" << "phi2_hub" << "phi2_shroud";
    if (outletGeomOption == 0 && inputRatio)
    {
      L += QStringList() << "Rout_Rin" << "Bout_Bin";
    }

    if (source == 1)
    {
      QString path = this->path() + "/";
      L += QStringList()
        << path + "blade/hubSect/beta2b"
        << path + "blade/rmsSect/beta2b"
        << path + "blade/tipSect/beta2b"

        << path + "blade/hubSect/thick1"
        << path + "blade/hubSect/thick2"
        << path + "blade/rmsSect/thick1"
        << path + "blade/rmsSect/thick2"
        << path + "blade/tipSect/thick1"
        << path + "blade/tipSect/thick2"
        << path + "blade/hubSect/wrapAngle"
        << path + "blade/rmsSect/wrapAngle"
        << path + "blade/tipSect/wrapAngle"
        << path + "blade/meanSect/wrapAngle";
    }
  }

  foreach(QString s, L)
  {
    if (property_t* p = property(s))
    {
      properties.push_back(p);
    }
  }
}

void T1dImpeller::GetConfigBladePropertyList(QVector<property_t*>& properties)
{
  T1dVaned::GetConfigBladePropertyList(properties);

  bool isFan = false, isPump = false, isComp = false;
  bool axialToRadialFan = false, _3DVaneFan = false, mixedFan = false;
  T1dMachine *pMachine = T1dMachine::getCurrentMachine();
  if (pMachine)
  {
    QString MachineType = pMachine->getMachineTypeString();
    isComp = (MachineType == "compressor"); 
    isFan = (MachineType == "fan");
    isPump = (MachineType == "pump");
    if (isFan)
    {
      mixedFan = (pMachine->AxRad == 2);
      axialToRadialFan = (pMachine->AppType == T1dMachine::AppType_MVR);
    }
  }
  bool shrouded = getTag("_isShrouded").toBool();
  bool portedThroud = getTag("_b_portedThroud").toBool();
  //Clearance set
  if (!shrouded && blade)
  {
    if (TObject* LE = blade->LE)
    {
      if (property_t* p = LE->property("clr"))
      {
        p->setDisplayName("LE clearance");
        properties.push_back(p);
      }
    }
    if (TObject* TE = blade->TE)
    {
      if (property_t* p = TE->property("clr"))
      {
        p->setDisplayName("TE clearance");
        properties.push_back(p);
      }
    }
    if (property_t* p = property("index_semiOpen"))
      properties.push_back(p);
  }

  //LE/TE opsition set
  QStringList L;
  if (!isTandemBlade)
  {
    if ((isFan && !mixedFan) || ((isPump || isComp) && isLERadius))
    {
      L += QStringList() << "RLE_hub" << "RLE_shroud";
    }
    else
    {
      L += QStringList() << "ule_hub" << "ule_shroud";
    }

    if (!axialToRadialFan)
    {
      L += QStringList() << "ute_hub" << "ute_shroud";
    }
    else
    {
      L += QStringList() << "RTE_hub" << "RTE_shroud";
    }
  }
  if (portedThroud)
  {
    L += QStringList() << "Z_bsi" <<
      "gap_bsi" <<
      "gap_bse" <<
      "Kt" <<
      "K_pbsi" <<
      "K_pbse" <<
      "swirl_factor";
  }
  foreach(QString s, L)
  {
    if (property_t* p = property(s))
    {
      properties.push_back(p);
    }
  }

  if (isPump)
  {
    if (property_t* p = property("Lamda_w"))
    {
      QChar lambda = QChar(0xbb, 0x03);
      QString name = QObject::tr("NPSHr coefficient: ") + QString(lambda) + "w";
      p->setDisplayName(name);
      properties.push_back(p);
    }
  }
}

void T1dImpeller::GetConfigDesignPropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
  /*int inletDesignOption = source == 0 ? getTag("inletDesignOption").toInt() : this->inletDesignOption;
  int outletDesignOption = source == 0 ? getTag("outletDesignOption").toInt() : this->outletDesignOption;
  int R1hOption = source == 0 ? getTag("R1hOption").toInt() : this->R1hOption;
  int OutletRaOption = source == 0 ? getTag("OutletRaOption").toInt() : this->OutletRaOption;
  int OutletBOption = source == 0 ? getTag("OutletBOption").toInt() : this->OutletBOption;*/

  const char *sname = TObject::shortName("T1dObject");
  TObject* oned = CURRENT_STUDY->object(sname);
  TObject* machine = oned->firstChild("T1dMachine");
  T1dMachine *pMachine = dynamic_cast<T1dMachine*>(machine);
  bool isComp = (pMachine->machineType == mach_compressor);
  bool isPump = (pMachine->machineType == mach_pump);
  bool isFan = (pMachine->machineType == mach_fan);
  bool isMultiVaneFan = false;
  bool isAxToRadial = false;
  bool isOverhang = (pMachine->AppType == T1dMachine::AppType_Non_Clogging_Pump);
  if (isFan)
  {
    isMultiVaneFan = (pMachine->AppType == T1dMachine::AppType_MultiVane);
    isAxToRadial = (pMachine->AppType == T1dMachine::AppType_MVR);
  }

  if (R1hOption == 0 && in)
    properties.push_back(in->property("Rh_Rs"));
  else if (R1hOption == 1 && in)
  {
    property_t *pp = property("R1h_specified");
    pp->setDisplayName("Inlet hub radius specified");
    properties.push_back(pp);
  }

  if (inletDesignOption == 1 && in)
  {
    property_t *pp = property("R1t_specified");
    pp->setDisplayName("Inlet tip radius specified");
    properties.push_back(pp);
  }
  else if (inletDesignOption == 2)
    properties.push_back(property("Km1"));
  else if (inletDesignOption == 3)
  {
    properties.push_back(property("Lamda_c"));
    QChar lambda = QChar(0xbb, 0x03);
    QString name = "NPSHr coefficient: " + QString(lambda) + "c";
    property("Lamda_c")->setDisplayName(name);
    properties.push_back(property("Lamda_w"));
    name = "NPSHr coefficient: " + QString(lambda) + "w";
    property("Lamda_w")->setDisplayName(name);
  }
  else if (inletDesignOption == 4)
    properties.push_back(property("K0"));

	if (inletDesignOption == 5)
	{
		properties.push_back(property("R1t_R2a"));
	}

	if (inletDesignOption == 6)
	{
		properties.push_back(property("R1t_R2a"));
		this->property("R1t_R2a")->setDisplayName("Deye/D2 ratio(Lobanoff)");
	}

  if (isPump)
  {
    if (outletDesignOption != 2)
    {
      //if (OutletRaOption == 0)
      //  properties.push_back(property("Kd"));
      if (OutletRaOption == 1)
        properties.push_back(property("workCoeff_d"));
			if (OutletRaOption == 2)
			{
				properties.push_back(property("Ku2"));
				this->property("Ku2")->setDisplayName("Ku Head Constant(Stepanoff)");
			}
			if (OutletRaOption == 4)
			{
				properties.push_back(property("Ku2"));
				this->property("Ku2")->setDisplayName("Ku Head Constant(Lobanoff)");
			}
    }
    //if (OutletBOption == 0 )
    //  properties.push_back(property("Kb"));
    if (OutletBOption == 1)
      properties.push_back(property("b2d2"));
		if (OutletBOption == 2)
		{
			properties.push_back(property("Km2"));
			this->property("Km2")->setDisplayName("Km2 Capacity Constant(Stepanoff)");
		}
    if (OutletBOption == 3)
      properties.push_back(property("b2_specified"));
		if (OutletBOption == 4)
		{
			properties.push_back(property("Km2"));
			this->property("Km2")->setDisplayName("Km2 Capacity Constant(Lobanoff)");
		}
  }

  if (outletDesignOption == 1)
    properties.push_back(property("beta2b_d"));
  else if (outletDesignOption == 2 && out)
  {
    if (property("D2_d"))
    {
      if (!isUserInput(this, "D2_d") && source == 0)
        property("D2_d")->setValue(out->Ra * 2.0);
      properties.push_back(property("D2_d"));
    }
  }
  else if (outletDesignOption == 3)
    properties.push_back(property("phi_d"));
  else if (outletDesignOption == 4)
  {
    if (property("phi_d"))
      properties.push_back(property("phi_d"));
    if (property("D2_d"))
    {
      if (!isUserInput(this, "D2_d") && source == 0)
        property("D2_d")->setValue(out->Ra*2.0);
      properties.push_back(property("D2_d"));
    }
    if (property("U2_d"))
      properties.push_back(property("U2_d"));
    if (property("mu_d"))
      properties.push_back(property("mu_d"));
  }

	bool isAxial2Dblade = (blade->bladeType == T1dBlade::bladeTypes::Radial_2D_SCA_Type)
		|| (blade->bladeType == T1dBlade::bladeTypes::Radial_2D_Straight_Type)
		|| (blade->bladeType == T1dBlade::bladeTypes::Radial_2D_SCA_Straight_Type)
		|| (blade->bladeType == T1dBlade::bladeTypes::Radial_2D_DCA_Type);

	if (isAxial2Dblade)
	{
		properties.push_back(property("DesIncidence"));
	}
	else if (blade->bladeType == T1dBlade::bladeTypes::Ruled_3D_Type)
	{
		properties.push_back(property("DesIncidence"));
		properties.push_back(property("DesIncidence_tip"));
	}
	else
	{
		properties.push_back(property("DesIncidence_hub"));
		properties.push_back(property("DesIncidence"));
		properties.push_back(property("DesIncidence_tip"));
	}

  if (isFan)
  {
    if (isAxToRadial)
    {
      properties.push_back(property("xi2"));
      properties.push_back(property("alpha2_target"));
			if (AxLengthOption == 0)
				properties.push_back(property("L_axial"));
			else
				properties.push_back(property("Laxial_D"));
    }
    else if (isMultiVaneFan)
    {
      //properties.push_back(property("xi");
      properties.push_back(property("D1D2"));
      properties.push_back(property("b2d2"));
    }
    else
    {
      properties.push_back(property("xi"));
      properties.push_back(property("d1dE"));
      properties.push_back(property("xi2"));
    }
  }


  if (isComp)
  {
    properties.push_back(property("alpha2_target"));
		if (AxLengthOption == 0)
			properties.push_back(property("L_axial"));
		else
			properties.push_back(property("Laxial_D"));
  }

  if (isPump)
  {
		if (AxLengthOption == 0)
			properties.push_back(property("L_axial"));
		else
			properties.push_back(property("Laxial_D"));
    if(isOverhang)
      properties.push_back(property("L_boss_AL"));
  }
}

void T1dImpeller::GetConfigGeometryPropertyList(QVector<property_t*>& properties, int source/*0: config 1:optimization*/)
{
	bool isFan = false;
	bool isPump = false;
	T1dMachine *pMachine = T1dMachine::getCurrentMachine();
	if (pMachine)
	{
		QString MachineType = pMachine->getMachineTypeString();
		isFan = (MachineType == "fan");
		isPump = (MachineType == "pump");
	}

	QStringList L;
  bool shrouded = source == 0 ? getTag("_isShrouded").toBool() : this->isShrouded;
	if (in)
		if (in->property("phi")) {
			properties.push_back(in->property("phi"));
			in->property("phi")->setDisplayName("Inlet inclination angle");
		}
	if (out)
		if (out->property("phi")) {
			properties.push_back(out->property("phi"));
			out->property("phi")->setDisplayName("Outlet inclination angle");
		}

	if (blade)
	{

#if 1
		QStringList L = QStringList() << "numMainBlade";
		foreach(QString s, L)
		{
			if (property_t* p = blade->property(s))
			{
				properties.push_back(p);
			}
		}
		properties.push_back(this->property("nrow_splitter"));
#else
		// original
		QStringList L = QStringList() << "numMainBlade" << "SplitterLeLoc";
		foreach(QString s, L)
		{
			if (property_t* p = blade->property(s))
			{
				properties.push_back(p);
			}
		}

		L.push_back("nrow_splitter");
#endif

		if (!shrouded)
		{
			if (TObject * LE = blade->LE)
			{
				if (property_t* p = LE->property("clr"))
				{
					p->setDisplayName("LE clearance");
					properties.push_back(p);
				}
			}
				
			if (TObject * TE = blade->TE)
			{
				if (property_t* p = TE->property("clr"))
				{
					p->setDisplayName("TE clearance");
					properties.push_back(p);
				}
			}
				
		}
		if (TObject* tip = blade->tipSection)
		{
			if (property_t* p = tip->property("thick1"))
			{
				p->setDisplayName("Tip LE blade thickness");
				properties.push_back(p);
			}
			if (property_t* p = tip->property("thick2"))
			{
				p->setDisplayName("Tip TE blade thickness");
				properties.push_back(p);
			}
		}
    if (property_t* p = property("throatAreaMultiplier"))
    {
      properties.push_back(p);
    }
	}

	if (pMachine && pMachine->AppType == T1dMachine::AppType_MVR)
	{
		L += QStringList() << "RatioCone" << "ConeAngle";
	}

	// LE location
	if (isPump)
	{
		L += QStringList() << "ule_hub" << "ule_shroud"; // TODO: review that at design
	}

	foreach(QString s, L)
	{
		if (property_t* p = property(s))
		{
			properties.push_back(p);
		}
	}
}

Double2 T1dImpeller::getBasePointForDoubleSuctionImpeller()
{
  Double2 basePoint;
  if (!isDoubleSuction)
  {
    basePoint = Double2(out->Zh, out->Rh);
    return basePoint;
  }
  // get rear disk
  basePoint = rearDisk->basePointPlot();
  return basePoint;
}

void T1dImpeller::UpdateDesincidence()
{

	if (blade->bladeType == T1dBlade::Ruled_3D_Type)
	{
#if 1
		double delta_mean = DesIncidence - incid_mean;
		double delta_tip = DesIncidence_tip - incid_tip;
		double deg = 1 * PI / 180.;
		if (abs(delta_mean) < 0.1 * deg && abs(delta_tip) < 0.1 * deg)
			return;

		blade->getMeanSection()->beta1b = sta_le->pmean->Beta + DesIncidence;
		blade->tipSection->beta1b = sta_le->ptip->Beta + DesIncidence_tip;

		adjustLEHubRuledBlade(blade->getMeanSection()->beta1b);
#else
		blade->getMeanSection()->beta1b = sta_le->pmean->Beta + DesIncidence;
		blade->hubSection->beta1b = sta_le->phub->Beta + DesIncidence_hub;
		blade->tipSection->beta1b = sta_le->ptip->Beta + DesIncidence_tip;
		UpdateBeta();
#endif
	}
	else
	{
		blade->getMeanSection()->beta1b = sta_le->pmean->Beta + DesIncidence;
		blade->hubSection->beta1b = sta_le->phub->Beta + DesIncidence_hub;
		blade->tipSection->beta1b = sta_le->ptip->Beta + DesIncidence_tip;
		UpdateBeta();
	}
}

