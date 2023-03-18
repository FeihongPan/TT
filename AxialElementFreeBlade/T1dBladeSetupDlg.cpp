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

#include "T1dBladeSetupDlg.h"
#include "w_Property.h"
#include "util_TObject.h"
#include "1d_Vaned.h"
#include "w_VectorPropertyWidgetH.h"
#include "w_Widgets.h"
#include "1d_inducer.h"
#include "1d_machine.h"
#include "util_Study.h"
#include "1dd_BladeEditor.h"
#include "1d_BladeEditor.h"


T1dBladeSetupDlg::T1dBladeSetupDlg(T1dVaned *object, QWidget * parent, Qt::WindowFlags f) :
  w_QDialog(parent, f)
{
  _pVane = object;
  const char *sname = TObject::shortName("T1dObject");
  TObject* p1d = CURRENT_STUDY->object(sname);
  if (p1d) {
    TObject * mach = p1d->firstChild("T1dMachine", true);
    if (mach)
    {
      T1dMachine *pMachine = dynamic_cast<T1dMachine*>(mach);
      if (pMachine)
      {
        if (pMachine->machineType == MachineType::mach_compressor)
          isCompressor = true;
        if (pMachine->machineType == MachineType::mach_turbine)
          isTurbine = true;
        if (pMachine->machineType == MachineType::mach_pump)
          isPump = true;

        if (pMachine->machineType == MachineType::mach_fan)
        {
          isFan = true;
          if (pMachine->AxRad == 2)
            isMixedFan = true;
          if (pMachine->AppType == T1dMachine::AppType_MVR)
            isAxialToRadalFan = true;
          if (pMachine->AppType == T1dMachine::AppType_3D_Vaned_Fan)
            is3DVanelFan = true;
          if (pMachine->AppType == T1dMachine::AppType_Generic)
            isGenericFan = true;
        }
        isAxial = (pMachine->AxRad == T1dMachine::exit_type_axial);
        isRadial = (pMachine->AxRad == T1dMachine::exit_type_radial);
      }
    }
    _HMTList = QStringList() << tr("Define at rms location only") << tr("Define at hub, rms and tip");
    _HMT = false;
    _pVane->blade->setBldSect();
    _hub = _pVane->blade->pBldSect[0];
    _mean = _pVane->blade->pBldSect[1];
    _tip = _pVane->blade->pBldSect[2];
    _HMT = _pVane->isHMT;
    _hasMeanGeometry = _pVane->isShowMeanlineGeometryParameters();
    _wrapAngle = _pVane->isUserWrapAngleDefine;
  }
}

int T1dBladeSetupDlg::getCurrentBladeTypeName()
{
  int bladeTypeIndex = 0;
  if (isCompressor)
  {
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Type)
      bladeTypeIndex = 0;
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Free_Type)
      bladeTypeIndex = 1;
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Free_3D_Type)
      bladeTypeIndex = 2;
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::blade_usebladeCurves)
      bladeTypeIndex = 3;
  }
  else if (isPump)
  {
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Free_Type)
      bladeTypeIndex = 0;
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Free_3D_Type)
      bladeTypeIndex = 1;
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Axial_Element_Free_Type)
      bladeTypeIndex = 2;
  }
  else if (isFan)
  {
    if (is3DVanelFan)
    {
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Free_Type)
        bladeTypeIndex = 0;
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Type)
        bladeTypeIndex = 1;
    }
    else
    {
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Radial_2D_SCA_Type)
        bladeTypeIndex = 0;
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Radial_2D_Straight_Type)
        bladeTypeIndex = 1;
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Radial_2D_DCA_Type)
        bladeTypeIndex = 2;
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Ruled_3D_Free_Type)
        bladeTypeIndex = 3;
      if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Radial_2D_SCA_Straight_Type)
        bladeTypeIndex = 4;
    }
  }
  else if (isTurbine)
  {
    if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Radial_Element_Rotor_Type)
      bladeTypeIndex = 0;
    else if (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Straight_Line_Element_Rotor_Type)
      bladeTypeIndex = 1;
  }

  return bladeTypeIndex;
}

void T1dBladeSetupDlg::initializePage()
{
  if (isVariableVane && (_pVane->haveBaseClass("T1dVane_IGV") || _pVane->haveBaseClass("T1dNozzle")))
  {
    QString title = tr("Information");
    QString message = tr("If you want to change the blade section definition, please uncheck the 'Variable geometry vane' box!");
    QMessageBox::about(this, title, message);
  }

  setWindowTitle(tr("Blade setup"));
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QHBoxLayout* optionLayout = new QHBoxLayout;
  if (_HMTWidget = w_Property::getPropertyWidget(&_HMT, tr("Blade Section Definition"), this, &_HMTList, false, true, w_Property::DLayout))
  {
    _HMTWidget->setAutoSave();
    connect(_HMTWidget, SIGNAL(valueChanged()), this, SLOT(onHMTOptionChange()));
    optionLayout->addWidget(_HMTWidget);
  }

  //blade Setting Options
  QStringList _angleList = QStringList() << tr("Input blade LE and TE angle");
  if (!(_pVane->haveBaseClass("T1dDiffuser_Vane") && _pVane->vaneType == profile_SCA))
    _angleList += tr("Input stagger angle and camber");
  _angle = _pVane->bladeAngleOption;
  if (wangle = w_Property::getPropertyWidget(&_angle, tr("Blade Setting Options"), this, &_angleList, false, true, false, w_Property::DLayout))
  {
    wangle->setAutoSave();
    connect(wangle, SIGNAL(valueChanged()), this, SLOT(angleOptionChange()));
    optionLayout->addWidget(wangle);
  }
  mainLayout->addLayout(optionLayout);

  if (_pVane->haveBaseClass("T1dNozzle")
    || (_pVane->haveBaseClass("T1dAxVaned") && (_pVane->vaneType == profile_SCA || _pVane->vaneType == profile_flat || _pVane->vaneType == profile_SCA_C4))
    || (_pVane->haveBaseClass("T1dDiffuser_Vane") && _pVane->vaneType != profile_wedge && _pVane->vaneType != profile_NACA65))
    wangle->show();
  else
    wangle->hide();

#if 0
  if (_pVane->isShowHasMeanlineGeometryCheckBox())
  {
    QHBoxLayout* hLayout = new QHBoxLayout;
    if (wMeanGeom = w_Property::getPropertyWidget(&_hasMeanGeometry, tr("Has meanline geometry"), this))
    {
      wMeanGeom->setAutoSave(true);
      if (_HMT == 0)
        wMeanGeom->hide();
      else
        wMeanGeom->show();
      connect(wMeanGeom, SIGNAL(valueChanged()), this, SLOT(onMeanGeometryChange()));
      hLayout->addWidget(wMeanGeom);
    }
    mainLayout->addLayout(hLayout);
  }
#endif


  // hidden _HMTWidget
  if(_pVane->haveBaseClass("T1dAxVaned"))
    _HMTWidget->setDisabled(true);
 // int bladeType = _pVane->blade->bladeType;

  // hide _HMTWidget
	if (_pVane->haveBaseClass("T1dImpeller") || _pVane->haveBaseClass("T1dInducer") || _pVane->haveBaseClass("T1dRotor"))
  {
    _HMTWidget->hide();
  }

  if (_pVane->haveBaseClass("T1dAxVaned") && !_pVane->haveBaseClass("T1dInducer"))
  {
    if (_pVane->vaneType == profile_Generic || _pVane->vaneType == profile_MCA || _pVane->vaneType == profile_DCA)
      _pVane->bladeAngleOption = 0;
  }

  if (_pVane->haveBaseClass("T1dAxVaned") && !_pVane->haveBaseClass("T1dInducer") && !isTurbine)
  {
    QHBoxLayout* layBlade = new QHBoxLayout;
    //temporary for axial compressor
    groupAirfoilComp = new QGroupBox();
    QHBoxLayout* layAirfoilComp = new QHBoxLayout;
    QStringList _allAxialCompAirfoilType; 
    if (isCompressor)
    {
      _allAxialCompAirfoilType = QStringList() << tr("NACA65")
        << tr("C4 (Parabolic)")
        << tr("DCA")
        << tr("MCA")
        << tr("Generic");
      //AxialCompAirfoilType->addItem("Pritchard");
    }
    else if (isPump)
    {
      //_allAxialCompAirfoilType = QStringList() << "Flat" << "SCA" << "NACA65" << "Generic";
      _allAxialCompAirfoilType = QStringList() << tr("Flat") << tr("SCA");
    }
    else if (isFan)
    {
      //_allAxialCompAirfoilType = QStringList() << "Flat" << "SCA" << "NACA65" << "Generic" << "C4";
      if (isMixedFan)
      {
        if (_pVane->haveBaseClass("T1dAxRotor"))
          _allAxialCompAirfoilType = QStringList() << tr("Flat") << tr("SCA");
        else
          _allAxialCompAirfoilType = QStringList() << tr("Flat") << tr("SCA") << tr("C4");
      }
      else // Axial fan
      {
        _allAxialCompAirfoilType = QStringList() << tr("Flat") << tr("SCA") << tr("SCA-C4") << tr("NACA65");
      }
    }
    else
      _allAxialCompAirfoilType = QStringList() << tr("Flat") << tr("NACA65") << tr("Generic");

    _AxialCompAirfoilType = getCurrentVaneTypeNameForAxComp();
    if (w_Property* wAxialCompAirfoilType = w_Property::getPropertyWidget(&_AxialCompAirfoilType, tr("Airfoil Type: "), this, &_allAxialCompAirfoilType, false, false, true))
    {
      wAxialCompAirfoilType->setAutoSave();
      layAirfoilComp->addWidget(wAxialCompAirfoilType);
      connect(wAxialCompAirfoilType, SIGNAL(valueChanged()), this, SLOT(airfoilTypeChanged()));
    }
    layAirfoilComp->addStretch(1);
    groupAirfoilComp->setLayout(layAirfoilComp);
    groupAirfoilComp->minimumSize();
    layBlade->addWidget(groupAirfoilComp);
    mainLayout->addLayout(layBlade);

    if (_pVane->vaneType == profile_NACA65)
    {
      _pVane->bladeAngleOption = 1;
      _pVane->blade->setBldSect();
      for (int i = 0; i < 3; i++)
        _pVane->blade->pBldSect[i]->isChordRatio = true;
    }

    gAirfoilHMT = new QGroupBox(tr("NACA65 Series Options"));
    QHBoxLayout* LayoutAirfoil = new QHBoxLayout(gAirfoilHMT);
    //Airfoil type options for aixal comp vantType65
    QStringList _allNACAType = QStringList() << "NACA 65-(0)10"
      << "NACA 65-(4)10"
      << "NACA 65-(8)10"
      << "NACA 65-(12)10"
      << "NACA 65-(15)10"
      << tr("User Define");
    _hubNACAType = _hub->NACA_65_Option;
    if (whub = w_Property::getPropertyWidget(&_hubNACAType, tr("Hub: "), this, &_allNACAType, false, false, true))
    {
      whub->setAutoSave();
      LayoutAirfoil->addWidget(whub);
      connect(whub, SIGNAL(valueChanged()), this, SLOT(NACAtypeChanged()));
    }

    auto setBladesectionProp = [&](T1dBladeSection* pBld)
    {
      if(!_pVane->isUserInput(pBld,"CL"))
        pBld->CL = 0.0;
      if(!_pVane->isUserInput(pBld, "thickmax_chord"))
        pBld->thickmax_chord = 0.05;
      if (!_pVane->isUserInput(pBld, "ac_ratio"))
        pBld->ac_ratio = 0.5;
    };

    if (_hub->NACA_65_Option == 0)
      setBladesectionProp(_hub);

    _meanNACAType = _mean->NACA_65_Option;
    if (wmean = w_Property::getPropertyWidget(&_meanNACAType, tr("Mean: "), this, &_allNACAType, false, false, true))
    {
      wmean->setAutoSave();
      LayoutAirfoil->addWidget(wmean);
      connect(wmean, SIGNAL(valueChanged()), this, SLOT(NACAtypeChanged()));
    }
    if (_mean->NACA_65_Option == 0)
      setBladesectionProp(_mean);

    _tipNACAType = _tip->NACA_65_Option;
    if (wtip = w_Property::getPropertyWidget(&_tipNACAType, tr("Tip: "), this, &_allNACAType, false, false, true))
    {
      wtip->setAutoSave();
      LayoutAirfoil->addWidget(wtip);
      connect(wtip, SIGNAL(valueChanged()), this, SLOT(NACAtypeChanged()));
    }
    if (_tip->NACA_65_Option == 0)
      setBladesectionProp(_tip);

    gAirfoilHMT->setLayout(LayoutAirfoil);
    layBlade->addWidget(gAirfoilHMT);
    gAirfoilHMT->setHidden(_pVane->vaneType != profile_NACA65);
  }

  //helical blade
  if (_pVane->haveBaseClass("T1dInducer") && (_pVane->blade->bladeType == _pVane->blade->bladeTypes::Helical_Axial_Type || _pVane->vaneType == 13))
  {
    T1dInducer * pInd = dynamic_cast<T1dInducer*>(_pVane);
    // Axial pitch for helical blade
    if (pInd)
    {
      bool isHelix_parameterization = pInd->isHelixParameterization;
      if (isHelix_parameterization)
      {
        // Blade edge input type
        QStringList _allHelixBladeEdgeInPutType = QStringList() << tr("Fractional location") << tr("Helical angle") << tr("Relative axial distance");
        _HelixBladeEdgeInPutType = pInd->bladeHelix->BladeEdge_input_type;
        if (w_Property* w = w_Property::getPropertyWidget(&_HelixBladeEdgeInPutType, tr("Helix blade edge input type: "), this, &_allHelixBladeEdgeInPutType, false, false, true))
        {
          w->setAutoSave();
          QHBoxLayout* hOptionBox = new QHBoxLayout();
          hOptionBox->addWidget(w);
          hOptionBox->addStretch(1);
          connect(w, SIGNAL(valueChanged()), this, SLOT(helixBladeEdgeInPutTypeChanged()));
          mainLayout->addLayout(hOptionBox);
        }
      }
    }
  }

  // for impeller
  bool condition_Fan = (isFan && isRadial) && !isAxialToRadalFan;
  if ( (_pVane->haveBaseClass("T1dImpeller") && !_pVane->getUseGeomCurve()) && (condition_Fan || isCompressor || isPump))
  {
    QStringList _allbladeType;
    if (is3DVanelFan)
      _allbladeType = QStringList() << tr("Ruled free") << tr("Ruled");
    else  if (isCompressor)
      _allbladeType = QStringList() << tr("Ruled") << tr("Ruled free") << tr("Full 3D") << tr("Blade editor");
    else  if (isPump)
      _allbladeType = QStringList() << tr("Ruled free") << tr("Full 3D") << tr("Axial element free");
    else
      _allbladeType = QStringList() << tr("SCA") << tr("Flat") << tr("DCA") << tr("Ruled free") << tr("SCA_Flat");

    _bladeType = getCurrentBladeTypeName();

    if (w_Property * w = w_Property::getPropertyWidget(&_bladeType, tr("Blade Type: "), this, &_allbladeType, false, false, true))
    {
      w->setAutoSave();
      QHBoxLayout* hOptionBox = new QHBoxLayout();
      hOptionBox->addWidget(w);
      hOptionBox->addStretch(1);
      connect(w, SIGNAL(valueChanged()), this, SLOT(bladeTypeChanged()));
      mainLayout->addLayout(hOptionBox);
    }
  }
  
  if (pMultiComp && (pMultiComp->haveBaseClass("T1dAirfoilDiffuser")))
  {
    QGroupBox *groupAirfoil = new QGroupBox();
    QHBoxLayout* layAirfoil = new QHBoxLayout;
    QString airfoiltype = tr("Airfoil Type: ");
    if (_pVane->vaneType == profile_NACA65)
      airfoiltype += tr("NACA Airfoil");
    else if (_pVane->vaneType == 14)
      airfoiltype += tr("SCA camber airfoil");
    else if (_pVane->vaneType == 15)
      airfoiltype += tr("DCA camber airfoil");
    else
      airfoiltype += tr("Generic Airfoil");
    QLabel* label = new QLabel(this);
    label->setText(airfoiltype);
    layAirfoil->addWidget(label);
    groupAirfoil->setLayout(layAirfoil);

    QHBoxLayout* hOptionBox = new QHBoxLayout();
    hOptionBox->addWidget(groupAirfoil);
    mainLayout->addLayout(hOptionBox);

    if (_pVane->vaneType == profile_NACA65)
    {
      T1dAirfoilDiffuser *pAirfoil = dynamic_cast<T1dAirfoilDiffuser *>(pMultiComp);
      QStringList _allNACATypeAirfoilDiffuser = QStringList() << "NACA 65-(4)06"
        << "NACA 65-(4)10"
        << "NACA 65-(8)10"
        << "NACA 65-(12)10"
        << "NACA 65-(15)10"
        << "NACA 65-(18)10"
        << "NACA 65-(21)10"
        << "NACA 65-(24)10"
        << tr("User Define");

      if(pAirfoil)_NACATypeAirfoilDiffuser = pAirfoil->nacaProfileType;
      if (w_Property * w = w_Property::getPropertyWidget(&_NACATypeAirfoilDiffuser, tr("NACA Type: "), this, &_allNACATypeAirfoilDiffuser, false, false, true))
      {
        w->setAutoSave();
        QHBoxLayout* layNACA = new QHBoxLayout;
        layNACA->addWidget(w);
        connect(w, SIGNAL(valueChanged()), this, SLOT(NACAtypeChangedAirfoilDiffuser()));
        QGroupBox* groupNACA = new QGroupBox();
        groupNACA->setLayout(layNACA);
        hOptionBox->addWidget(groupNACA);
      }
    }
  }
  
  //for radial turbine rotor blade
  if (_pVane->haveBaseClass("T1dRotor") && !_pVane->getUseGeomCurve())
  {
		//QStringList _radialTurbineRotorBladeType = QStringList() << tr("Straight-line element") << tr("Radial element");
  //  if (w_Property* wRadialTurbRotorBladeType = w_Property::getPropertyWidget(&_StringLineOrRadialElementBlade, tr("Blade type section"), this, &_radialTurbineRotorBladeType, false, true, w_Property::DLayout))
  //  {
  //    wRadialTurbRotorBladeType->setAutoSave();
  //    connect(wRadialTurbRotorBladeType, SIGNAL(valueChanged()), this, SLOT(bladeTypeChanged()));
  //    optionLayout->addWidget(wRadialTurbRotorBladeType);
  //  }

    QStringList _allbladeType = QStringList() << tr("Radial element") << tr("Straight-line element");
    _bladeType = getCurrentBladeTypeName();
    if (w_Property* w = w_Property::getPropertyWidget(&_bladeType, tr("Blade Type: "), this, &_allbladeType, false, false, true))
    {
      w->setAutoSave();
      QHBoxLayout* hOptionBox = new QHBoxLayout();
      hOptionBox->addWidget(w);
      hOptionBox->addStretch(1);
      connect(w, SIGNAL(valueChanged()), this, SLOT(bladeTypeChanged()));
      mainLayout->addLayout(hOptionBox);
    }
  }

  _vpw = new w_VectorPropertyWidgetH(this);
  mainLayout->addWidget(_vpw);
  if (_pVane->isTandemBlade)
  {
    _vpwTandemBlade = new w_VectorPropertyWidgetH(this);
    mainLayout->addWidget(_vpwTandemBlade);
  }

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  //check box  
  if (_pVane->haveBaseClass("T1dNozzle") || _pVane->haveBaseClass("T1dInducer") || _pVane->haveBaseClass("T1dVane_IGV") || _pVane->haveBaseClass("T1dRotor")
    || (_pVane->haveBaseClass("T1dAxVaned") && (_pVane->vaneType == profile_flat || _pVane->vaneType == profile_SCA || _pVane->vaneType == profile_SCA_C4))
    || (_pVane->haveBaseClass("T1dDiffuser_Vane") && _pVane->vaneType != profile_wedge && _pVane->vaneType != profile_NACA65))
  {
    cChord = new QCheckBox(tr("Chord ratio option"), this);
    cChord->setChecked(_mean->isChordRatio);
    connect(cChord, SIGNAL(clicked()), this, SLOT(controlEvent()));
    buttonsLayout->addWidget(cChord);
  }

  if (_pVane->haveBaseClass("T1dImpeller"))
  {
    bladeEditorButton = new QPushButton(tr("Blade editor"));
    connect(bladeEditorButton, SIGNAL(clicked()), this, SLOT(BladeEditorDlg()));
    buttonsLayout->addWidget(bladeEditorButton);
    bladeEditorButton->setHidden(_pVane->blade->bladeType != _pVane->blade->bladeTypes::blade_usebladeCurves);
  }

  refreshPropertyList();
  QPushButton *okButton = new w_OKButton;
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QPushButton *cancelButton = new w_CancelButton;
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(okButton);
  buttonsLayout->addWidget(cancelButton);

  mainLayout->addLayout(buttonsLayout);
  setLayout(mainLayout);
  setMinimumSize(QSize(800, 500));
}

void T1dBladeSetupDlg::onHMTOptionChange()
{
  if (_pVane && (_pVane->haveBaseClass("T1dAxVaned") && !_pVane->haveBaseClass("T1dInducer")) && _pVane->vaneType == profile_NACA65)
  {
    if (_HMT == 0)
    {
      whub->hide();
      wtip->hide();
    }
    else
    {
      whub->show();
      wtip->show();
    }
  }

  refreshPropertyList();
}

int T1dBladeSetupDlg::getCurrentVaneTypeNameForAxComp()
{
  int vaneIndex = 0;
  if (isCompressor)
  {
    switch (_pVane->vaneType)
    {
    case profile_NACA65: vaneIndex = 0; break;
    case profile_C4:  vaneIndex = 1; break;
    case profile_DCA: vaneIndex = 2; break;
    case profile_MCA: vaneIndex = 3; break;
      //case profile_Pritchard: vaneIndex = 4; break; //remove Pritchard
    case profile_Generic: vaneIndex = 4; break;
    default:vaneIndex = 0; break;
    }
  }
  else if (isPump)
  {
    switch (_pVane->vaneType)
    {
    case profile_flat: vaneIndex = 0; break;
    case profile_SCA: vaneIndex = 1; break;
    }
  }
  else if (isFan)
  {
    if (isMixedFan)
    {
      if (_pVane->haveBaseClass("T1dAxRotor"))
      {
        switch (_pVane->vaneType)
        {
        case profile_flat: vaneIndex = 0; break;
        case profile_SCA: vaneIndex = 1; break;
        }
      }
      else
      {
        switch (_pVane->vaneType)
        {
        case profile_flat: vaneIndex = 0; break;
        case profile_SCA: vaneIndex = 1; break;
        case profile_SCA_C4: vaneIndex = 2; break;
        }
      }
    }
    else
    {
      switch (_pVane->vaneType)
      {
      case profile_flat: vaneIndex = 0; break;
      case profile_SCA: vaneIndex = 1; break;
      case profile_SCA_C4: vaneIndex = 2; break;
      case profile_NACA65: vaneIndex = 3; break;
      }
    }
  }
  else
  {
    if (_pVane->vaneType == profile_NACA65)
      vaneIndex = 1;
    else if (_pVane->vaneType == profile_Generic)
      vaneIndex = 2;
    else
      vaneIndex = 0;
  }

  return vaneIndex;
}

void T1dBladeSetupDlg::onWrapAngleOptionChange()
{
  _pVane->isUserWrapAngleDefine = _wrapAngle;
  refreshPropertyList();
}

void T1dBladeSetupDlg::onMeanGeometryChange()
{
  refreshPropertyList();
}

void T1dBladeSetupDlg::airfoilTypeChanged()
{
  if (isCompressor)
  {
    switch (_AxialCompAirfoilType)
    {
    case 0: _pVane->vaneType = profile_NACA65; break;
    case 1: _pVane->vaneType = profile_C4; break;
    case 2: _pVane->vaneType = profile_DCA; break;
    case 3: _pVane->vaneType = profile_MCA;; break;
      //case 4: pComp->vaneType = profile_Pritchard; break;
    case 4: _pVane->vaneType = profile_Generic; break;
    default:_pVane->vaneType = profile_NACA65; break;
    }
  }
  else if (isPump)
  {
    switch (_AxialCompAirfoilType)
    {
    case 0: _pVane->vaneType = profile_flat; break;
    case 1: _pVane->vaneType = profile_SCA; break;
    }
  }
  else if (isFan)
  {
    if (isMixedFan)
    {
      if (_pVane->haveBaseClass("T1dAxRotor"))
      {
        switch (_AxialCompAirfoilType)
        {
        case 0: _pVane->vaneType = profile_flat; break;
        case 1: _pVane->vaneType = profile_SCA; break;
        }
      }
      else
      {
        switch (_AxialCompAirfoilType)
        {
        case 0: _pVane->vaneType = profile_flat; break;
        case 1: _pVane->vaneType = profile_SCA; break;
        case 2:
          _pVane->vaneType = 14; break;
        }
      }
    }
    else // Axial fan
    {
      switch (_AxialCompAirfoilType)
      {
      case 0: _pVane->vaneType = profile_flat; break;
      case 1: _pVane->vaneType = profile_SCA; break;
      case 2: _pVane->vaneType = profile_SCA_C4; break;
      case 3: _pVane->vaneType = profile_NACA65; break;
      }
    }
  }
  else if (isMixedFan)
  {
    switch (_AxialCompAirfoilType)
    {
    case 0: _pVane->vaneType = profile_flat; break;
    case 1: _pVane->vaneType = profile_SCA; break;
    }
  }
  else
  {
    if (_AxialCompAirfoilType == 1)
      _pVane->vaneType = profile_NACA65;
    else if (_AxialCompAirfoilType == 2)
      _pVane->vaneType = profile_Generic;
    else
      _pVane->vaneType = profile_flat;
  }
  if (_pVane->vaneType == profile_NACA65)
  {
    _pVane->bladeAngleOption = 1;
    _pVane->blade->setBldSect();
    for (int i = 0; i < 3; i++)
      _pVane->blade->pBldSect[i]->isChordRatio = true;
  }
  if (gAirfoilHMT)
    gAirfoilHMT->setHidden(_pVane->vaneType != profile_NACA65);

  refreshPropertyList();
}

void T1dBladeSetupDlg::NACAtypeChanged()
{
  _hub->NACA_65_Option = _hubNACAType;
  _hub->setNACA_65_Info_AxComp(_hub->NACA_65_Option);
  _mean->NACA_65_Option = _meanNACAType;
  _mean->setNACA_65_Info_AxComp(_mean->NACA_65_Option);
  _tip->NACA_65_Option = _tipNACAType;
  _tip->setNACA_65_Info_AxComp(_tip->NACA_65_Option);

  refreshPropertyList();
}

void T1dBladeSetupDlg::NACAtypeChangedAirfoilDiffuser()
{
  T1dAirfoilDiffuser *pAirfoil = dynamic_cast<T1dAirfoilDiffuser *>(pMultiComp);
  if (pAirfoil)
  {
    pAirfoil->nacaProfileType = _NACATypeAirfoilDiffuser;
    pAirfoil->SetNacaProfileParameters();
  }
  refreshPropertyList();
}

void T1dBladeSetupDlg::helixBladeEdgeInPutTypeChanged()
{
  T1dInducer *pInducer = dynamic_cast<T1dInducer *>(_pVane);
  if(pInducer)pInducer->bladeHelix->BladeEdge_input_type = _HelixBladeEdgeInPutType;
  refreshPropertyList();
}

void T1dBladeSetupDlg::angleOptionChange()
{
  //_pVane->bladeAngleOption = _angle;
  refreshPropertyList();
}

void T1dBladeSetupDlg::controlEvent()
{
  refreshPropertyList();
}

void T1dBladeSetupDlg::bladeTypeChanged()
{
  if (isCompressor)
  {
    switch (_bladeType)
    {
    case 0: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Type;
      break;
    case 1: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Free_Type;
      break;
    case 2: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Free_3D_Type;
      break;
    case 3: _pVane->blade->bladeType = _pVane->blade->bladeTypes::blade_usebladeCurves;
      break;
    }
  }
  else if (isPump)
  {
    switch (_bladeType)
    {
    case 0: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Free_Type;
      break;
    case 1: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Free_3D_Type;
      break;
    case 2: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Axial_Element_Free_Type;
      break;
    }
  }
  if (isFan)
  {

    if (is3DVanelFan)
    {
      switch (_bladeType)
      {
      case 0: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Free_Type;
        break;
      case 1: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Type;
        break;
      }
    }
    else
    {
      switch (_bladeType)
      {
      case 0: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Radial_2D_SCA_Type;
        break;
      case 1: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Radial_2D_Straight_Type;
        break;
      case 2: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Radial_2D_DCA_Type;
        break;
      case 3: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Ruled_3D_Free_Type;
        break;
      case 4: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Radial_2D_SCA_Straight_Type;
        break;
      }
    }
  }

  if (_pVane->haveBaseClass("T1dImpeller"))
  {
    T1dImpeller* pImp = dynamic_cast<T1dImpeller*>(_pVane);
    pImp->validateBladeType();

    _hasMeanGeometry = _pVane->isShowMeanlineGeometryParameters();
  }

	if (isTurbine)
	{
		switch (_bladeType)
		{
		case 0: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Radial_Element_Rotor_Type;
			break;
		case 1: _pVane->blade->bladeType = _pVane->blade->bladeTypes::Straight_Line_Element_Rotor_Type;
			break;
		}
	}
  bladeEditorButton->setHidden(_pVane->blade->bladeType != _pVane->blade->bladeTypes::blade_usebladeCurves);

	refreshPropertyList();
}

void T1dBladeSetupDlg::BladeEditorDlg()
{
  // 1.
  T1dBladeEditor bladeEditor;

  QVector<double> spans = QVector<double>() << 0. << 50. << 100.;

  bladeEditor.setVaned(_pVane);

  bladeEditor.initialBladeEditor(spans);
  // 2.
  T1dBladeEditorDlg bladeEditorDlg(this);

  int error = bladeEditorDlg.initializePage();

  bladeEditorDlg.setBladeEditor(&bladeEditor);


  bladeEditorDlg.update();

  refreshPropertyList();

}

void T1dBladeSetupDlg::accept()
{
  _pVane->isHMT = _HMT;
  _pVane->bladeAngleOption = _angle;
  _pVane->setHasMeanlineGeometry(_hasMeanGeometry);

  if (cChord)
  {
    _mean->isChordRatio = cChord->isChecked();
    _hub->isChordRatio = _mean->isChordRatio;
    _tip->isChordRatio = _mean->isChordRatio;
  }
  else
  {
    //if (_mean->isChordRatio && _pVane->vaneType != profile_NACA65)
    //{
    //  _mean->isChordRatio = false;
    //  _hub->isChordRatio = _mean->isChordRatio;
    //  _tip->isChordRatio = _mean->isChordRatio;
    //}
  }
  _vpw->accept();
  if (_pVane->isTandemBlade && _vpwTandemBlade)
    _vpwTandemBlade->accept();

  if (!(_HMT)) {
    TObject::copyObject(_hub, _mean, true);
    TObject::copyObject(_tip, _mean, true);
  }
  w_QDialog::accept();
}

void T1dBladeSetupDlg::refreshPropertyList()
{
  QVector<QVector<property_t *>> vProperties;
  QVector<property_t *>  properties;
  QVector<TObject*> objects;
  QStringList columnNames;
  _pVane->clearTags();
  _pVane->setTag("_bLETE", !_angle);
  bool bChord = false;
  if (cChord)
    bChord = cChord->isChecked();
  else
    bChord = _mean->isChordRatio;
  _pVane->setTag("_bChord", bChord);
  _pVane->setTag("_isVariableVane", isVariableVane);
  QStringList propList;
  QStringList readOnlyList;
  QMap<QString, QString> dispMap;
  _pVane->getBladeSetupPropList(propList, readOnlyList, dispMap);
  if (_HMT == 0)//Define at rms location only
  {
    columnNames.push_back(tr("Mean"));
    if (_mean)
      objects.push_back(_mean);
  }
  else//Define at hub, rms and tip
  {
    columnNames.push_back(tr("Hub"));
    columnNames.push_back(tr("Mean"));
    columnNames.push_back(tr("Tip"));
    if (_hub)
      objects.push_back(_hub);
    if (_mean)
      objects.push_back(_mean);
    if (_tip)
      objects.push_back(_tip);
  }

  if (_pVane->haveBaseClass("T1dVane_IGV") || _pVane->haveBaseClass("T1dNozzle"))
  {
    if (isVariableVane)
    {
      foreach(QString s, propList)
      {
        if (property_t* p = _pVane->property(s))
        {
          readOnlyList.append(s);
        }
      }
    }
  }
  
  if (_pVane && (_pVane->haveBaseClass("T1dAxVaned") && !_pVane->haveBaseClass("T1dInducer")) && _pVane->vaneType == profile_NACA65)
  {
    foreach(TObject* o, objects)
    {
      if (o->property("NACA_65_Option")->getValue().toInt() != 5)
        readOnlyList << "CL" << "ac_ratio" << "thickmax_chord";
      //readOnlyList << "camberAngle";
    }
  }

  foreach(QString s, propList)
  {
    properties.clear();
    foreach(TObject* o, objects)
    {
      if (property_t* p = o->property(s))
      {
        properties.push_back(p);
        if (readOnlyList.indexOf(s) >= 0)
          SET_PROPERTY_READONLY(p);
        else
          SET_PROPERTY_WRITABLE(p);

        if (o == _mean&& _HMT==1)
        {
          if (!_hasMeanGeometry)
            SET_PROPERTY_READONLY(p);
          else
            SET_PROPERTY_WRITABLE(p);
        }

        if (o == _hub && _HMT == 1 && isTurbine)
        {
					if (_bladeType == 1)
						SET_PROPERTY_WRITABLE(p);
					else
						SET_PROPERTY_READONLY(p);
        }

        if (dispMap.contains(s))
          p->setDisplayName(dispMap[s]);
      }
    }
    vProperties.push_back(properties);
  }

  // For Axial Element free Blade
  if (isPump && _pVane->haveBaseClass("T1dImpeller"))
  {
    foreach(QString s, propList)
    {
      bool isthink = (s == "thick1" || s == "thick2" || s == "thickMax" || s == "thickmax_loc_chord");
      foreach(TObject * o, objects)
      {
        if (property_t* p = o->property(s))
        {
          if ((o == _hub || o == _tip) && _bladeType == 2 && !isthink)
            SET_PROPERTY_READONLY(p);
          else
            SET_PROPERTY_WRITABLE(p);
        }
      }
    }
  }

  if (isFan && isRadial && isGenericFan && _pVane->haveBaseClass("T1dImpeller"))
  {
    bool isfree = (_bladeType == 3);
    foreach(QString s, propList)
    {
      foreach(TObject * o, objects)
      {
        if (property_t* p = o->property(s))
        {
          if(isfree && o == _mean)
            SET_PROPERTY_READONLY(p);
          if (!isfree && (o == _mean || o == _tip))
            SET_PROPERTY_READONLY(p);
        }
      }
    }
  } 

  _vpw->setProperties(vProperties, &columnNames);

  if (_pVane->isTandemBlade)
    refreshPropertyListTandem(propList, readOnlyList, dispMap);
}

void T1dBladeSetupDlg::refreshPropertyListTandem(const QStringList propList, const QStringList readOnlyList, const QMap<QString, QString> dispMap)
{
  T1dBlade* subBlade = _pVane->getTandemSubBlade();
  if (!VALIDO(subBlade))
    subBlade = _pVane->createTandemSubBlade();
  subBlade->setBldSect();

  QStringList columnNames;
  if (_HMT == 0)//Define at rms location only
    columnNames = QStringList() << tr("Mean");
  else
    columnNames = QStringList() << tr("Hub") << tr("Mean") << tr("Tip");

  QVector<QVector<property_t *>> vProperties;
  QVector<property_t *>  properties;
  QVector<TObject*> objects;
  if (_HMT == 0)//Define at rms location only
  {
    if (subBlade->pBldSect[1])
      objects.push_back(subBlade->pBldSect[1]);
  }
  else//Define at hub, rms and tip
  {
    if (subBlade->pBldSect[0])
      objects.push_back(subBlade->pBldSect[0]);
    if (subBlade->pBldSect[1])
      objects.push_back(subBlade->pBldSect[1]);
    if (subBlade->pBldSect[2])
      objects.push_back(subBlade->pBldSect[2]);
  }

  foreach(QString s, propList)
  {
    properties.clear();
    forLoop(k, objects.size())
    {
      if (property_t* p = objects[k]->property(s))
      {
        properties.push_back(p);
        if (readOnlyList.indexOf(s) >= 0)
          SET_PROPERTY_READONLY(p);
        else
          SET_PROPERTY_WRITABLE(p);

        if (_HMT == 1)
        {
          if (k == 1)
          {
            if (!_hasMeanGeometry)
              SET_PROPERTY_READONLY(p);
            else
              SET_PROPERTY_WRITABLE(p);
          }

          if (k == 0 && isTurbine)
          {
            if (_bladeType == 1)
              SET_PROPERTY_WRITABLE(p);
            else
              SET_PROPERTY_READONLY(p);
          }
        }

        if (dispMap.contains(s))
          p->setDisplayName(dispMap[s]);

        if (isFan && isRadial && isGenericFan && _pVane->haveBaseClass("T1dImpeller"))
        {
          if (k > 0)//mean or tip
            SET_PROPERTY_READONLY(p);
        }
      }
    }
    vProperties.push_back(properties);
  }

  if (_vpwTandemBlade)
    _vpwTandemBlade->setProperties(vProperties, &columnNames);
}

T1dBladeSetupDlg::~T1dBladeSetupDlg()
{

}

