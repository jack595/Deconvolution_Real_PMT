//////////////////////////////////
/*
Algorithm for PMT waveform reconstruction, with the deconvolution method.
v1, Zeyuan Yu.
Author: yuzy@ihep.ac.cn
July 12, 2016
*/
/*
v2. Add a simple hit counting algorithm on the deconvolution results.
If a hit has a charge integral less than 1.5 p.e., and it is far away from other
other hit, its charge is set to 1 p.e.
Energy resolution is expected to be improved by at least 0.1% at 1MeV.
By Zeyuan Yu.
Oct. 18, 2016
*/
/*
v3. Update for the J17v1:
    1) Modify according to the FADC configurations and ouput format
By Zeyuan Yu.
Apr. 6, 2017
*/
/*
v4. Input PMT calibration parameters
By Xuantong Zhang.
zhangxuantong@ihep.ac.cn
Sept. 29, 2019
*/
/*
v5. Add hits discrimination
By Xuantong Zhang.
zhangxuantong@ihep.ac.cn
Mar. 24, 2020
*/
//////////////////////////////////
#ifndef Deconvolution_cc
#define Deconvolution_cc
#include "Deconvolution.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "EvtNavigator/NavBuffer.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "SniperKernel/SniperPtr.h"
#include "RootWriter/RootWriter.h"
#include "DataRegistritionSvc/DataRegistritionSvc.h"
#include "Event/ElecHeader.h"
#include "Event/CalibHeader.h"
#include "Identifier/CdID.h"
#include <TFile.h>
#include "TROOT.h"
#include <iostream>
#include <TTree.h>
#include <TSystem.h>
#include <sstream>
#include "TVirtualFFT.h"
#include "math.h"
#include "Geometry/PMTParamSvc.h"

#include "PMTCalibSvc/PMTCalibSvc.h"
using namespace std;

DECLARE_ALGORITHM(Deconvolution);

Deconvolution::Deconvolution(const string &name) : AlgBase(name), m_memMgr(0), m_totalPMT(17612), m_threshold_mcp(0.025), m_threshold_dyn(0.025), Raw(0), Freq(0), Back(0), m_para1(110.), m_para2(160.), m_para3(15.), m_para4(80.), m_para5(130.), m_para6(15.), m_length(1250), m_window(9), m_hc(0), m_c(0)
{
  declProp("TotalPMT", m_totalPMT);
  declProp("CalibFile", m_CalibFile);
  declProp("Threshold1", m_threshold_mcp);
  declProp("Threshold2", m_threshold_dyn);
  declProp("Para1", m_para1);
  declProp("Para2", m_para2);
  declProp("Para3", m_para3);
  declProp("Para4", m_para4);
  declProp("Para5", m_para5);
  declProp("Para6", m_para6);
  declProp("Length", m_length);
  declProp("Window", m_window);
  declProp("HitCounting", m_hc);
  declProp("Calib", m_c);
  declProp("Filter", m_FilterFile);
}

Deconvolution::~Deconvolution()
{
}

bool Deconvolution::initialize()
{
  debug = 1;
  m_stat = 0;

  // filter file
  TFile *ff = 0;
  filter_flag = false;
  if (m_FilterFile == "gaus") {
    filter_flag = false;
  } else {
    // if m_FilterFile is empty, try to load the default one.
    // The order: load the data under JUNOTOP, then the data under WORKTOP
    if (m_FilterFile.empty()) {
      if (getenv("JUNOTOP")) {
        std::string s = getenv("JUNOTOP");
        s += "/data/Reconstruction/Deconvolution/share/filter3_m.root";
        if (!gSystem->AccessPathName(s.c_str())) {
          m_FilterFile = s;
        }
      }
      if (getenv("WORKTOP")) {
        std::string s = getenv("WORKTOP");
        s += "/data/Reconstruction/Deconvolution/share/filter3_m.root";
        if (!gSystem->AccessPathName(s.c_str())) {
          m_FilterFile = s;
        }
      }
      LogInfo << "Loading default FilterFile: " << m_FilterFile << std::endl;
      ff = TFile::Open(m_FilterFile.c_str(), "read");

    } else {
      LogInfo << "Loading  FilterFile: " << m_FilterFile << std::endl;
      ff = TFile::Open(m_FilterFile.c_str(), "read");
    }

    if (!ff) {
      LogError << "Could not open filter file." << endl;
      return false;
    }
    filter_flag = true;
  }

  // prepare the filter, specified for the J16v2
  if (filter_flag == false) {
    m_filter_mcp.clear();
    m_filter_dyn.clear();
    for (int i = 0; i < m_length; i++) {
      if (i < m_para1)
        m_filter_mcp.push_back(1);
      else if (i < m_para2)
        m_filter_mcp.push_back(exp(-0.5 * ((i - m_para1) / m_para3) * ((i - m_para1) / m_para3)));
      else {
        m_filter_mcp.push_back(0);
      }  // Gaussian like filters. Paramters have been optimized for the current
         // electronics simulation outputs
    }
    for (int i = 0; i < m_length; i++) {
      if (i < m_para4)
        m_filter_dyn.push_back(1);
      else if (i < m_para5)
        m_filter_dyn.push_back(exp(-0.5 * ((i - m_para4) / m_para6) * ((i - m_para4) / m_para6)));
      else {
        m_filter_dyn.push_back(0);
      }  // Gaussian like filters. Paramters have been optimized for the current
         // electronics simulation outputs
    }
    LogInfo << "Use Gaussian filter" << std::endl;
  } else {
    m_filter_mcp.clear();
    m_filter_dyn.clear();
    TH1D *fh0 = (TH1D *)ff->Get("fh0");
    TH1D *fn0 = (TH1D *)ff->Get("fn0");
    LogInfo << "get filter ok" << std::endl;
    for (int i = 0; i < m_length; i++) {
      m_filter_mcp.push_back(fn0->GetBinContent(i + 1));
      m_filter_dyn.push_back(fh0->GetBinContent(i + 1));
    }
    LogInfo << "Use default Wiener filter" << std::endl;
  }

  // read gain file

  if (m_CalibFile.empty()) {
    if (getenv("JUNOTOP")) {
      std::string s = getenv("JUNOTOP");
      s += "/data/Reconstruction/Deconvolution/share/SPE_v20.root";
      if (!gSystem->AccessPathName(s.c_str())) {
        m_CalibFile = s;
      }
    }
    if (getenv("WORKTOP")) {
      std::string s = getenv("WORKTOP");
      s += "/data/Reconstruction/Deconvolution/share/SPE_v20.root";
      if (!gSystem->AccessPathName(s.c_str())) {
        m_CalibFile = s;
      }
    }
  }
  TFile *f = TFile::Open(m_CalibFile.c_str());
  if (!f) {
    LogError << "Could not open SPE calibration file." << endl;
    return false;
  }
  // fill the gain map
  int ReadoutLength = 0;
  int warnings = 0, warnings1 = 0;
  for (int i = 0; i < m_totalPMT; i++) {
    ostringstream ss1;
    ss1 << "PMTID_" << i << "_SPERE";
    ostringstream ss2;
    ss2 << "PMTID_" << i << "_SPEIM";
    TH1F *re = (TH1F *)f->Get(ss1.str().c_str());
    TH1F *im = (TH1F *)f->Get(ss2.str().c_str());
    if (!re || !im) {
      if (warnings < 10) {
        LogWarn << "SPE spectrum for PMTID " << i << " is missing. Will use PMT 0 as default." << endl;
      }
      warnings++;
      continue;
    }
    vector<double> SPERE, SPEIM;
    if (re->GetNbinsX() != im->GetNbinsX()) {
      LogError << "PMT SPE spectra for PMTID " << i << "are not consistent. Bye!" << endl;
      return false;
    }
    ReadoutLength = re->GetNbinsX();
    // if (ReadoutLength != int(m_para2)) {
    if (ReadoutLength != (m_para2 > m_para5 ? int(m_para2) : int(m_para5))) {
      if (warnings1 < 10) {
        LogWarn << "Cut off frequency of spe and filter for PMTID " << i << " are not consistent. Will use the short one." << endl;
      }
      warnings1++;
    }
    for (int j = 0; j < ReadoutLength; j++) {
      SPERE.push_back(re->GetBinContent(j + 1));
      SPEIM.push_back(im->GetBinContent(j + 1));
    }
    m_SPERE.insert(pair<int, std::vector<double>>(i, SPERE));
    m_SPEIM.insert(pair<int, std::vector<double>>(i, SPEIM));
    vector<double>(SPERE).swap(SPERE);
    vector<double>(SPEIM).swap(SPEIM);
    delete re;
    delete im;
  }
  LogInfo << "A total of " << warnings << " PMTs do not have pre-defined SPE spectrum." << endl;
  LogInfo << "A total of " << warnings1 << " PMTs have different cutoff frequencies for filter and SPE spectrum" << endl;
  f->Close();
  if (m_SPERE.size() == 0) {
    LogError << "NO PMT SPE WAVEFORM!" << endl;
    return false;
  }

  // get hmmt pmt id
  hmmtpmt.clear();

  SniperPtr<PMTParamSvc> pmtParamSvc(getParent(), "PMTParamSvc");
  if (pmtParamSvc.invalid()) {
    LogError << "Failed to get PMTParamSvc instance!" << std::endl;
    return false;
  }

  for (int i = 0; i < m_totalPMT; i++) {
    bool isHama = pmtParamSvc->isHamamatsu(i);
    hmmtpmt.push_back(isHama);
  }

  // prepare PMT gain calib data
  SniperPtr<PMTCalibSvc> calSvc(getParent(), "PMTCalibSvc");
  if (calSvc.invalid()) {
    LogError << "Failed to get PMTCalibSvc instance!" << std::endl;
    return false;
  }
  if (m_c == 0) {
    gain = calSvc->getGain();
    toffset = calSvc->getTimeOffset();
    LogInfo << "Load gain and timeoffset correction successfully" << std::endl;
  }

  // prepare histograms to protect the memory
  Raw = new TH1D("raw", "raw", m_length, 0, m_length);
  //
  m_memMgr = SniperPtr<IDataMemMgr>(getParent(), "BufferMemMgr").data();
  // SniperPtr<DataRegistritionSvc> drsSvc(getScope(),"DataRegistritionSvc");
  // if ( ! drsSvc.valid() ) {
  //     LogError << "Failed to get DataRegistritionSvc instance!" << endl;
  //     return false;
  // }
  // drsSvc->registerData("JM::CalibEvent","/Event/Calib");
  // user data definitions
  SniperPtr<RootWriter> svc(getParent(), "RootWriter");
  if (svc.invalid()) {
    LogError << "Can't Locate RootWriter. If you want to use it, please "
             << "enalbe it in your job option file." << std::endl;
    return false;
  }
  gROOT->ProcessLine("#include <vector>");

  m_calib = svc->bookTree("CALIBEVT/calibevt", "simple output of calibration data");
  m_calib->Branch("Charge", &m_charge);
  m_calib->Branch("Time", &m_time);
  m_calib->Branch("PMTID", &m_pmtId);
  m_calib->Branch("TotalPE", &m_totalpe, "TotalPE/F");
  m_calib->Branch("Width", &m_width);
  m_calib->Branch("Peak", &m_peak);
  m_calib->Branch("TrigTimeSec", &m_trigtimesec, "TrigTimeSec/I");
  m_calib->Branch("TrigTimeNanoSec", &m_trigtimenanosec, "TrigTimeNanoSec/I");

  if (debug == 1) {
    debugWave = new TH1D("debugWave", "debugWave", m_length, 0, m_length);
    m_debug = svc->bookTree("CALIBEVT/debug", "debug waves");
    m_debug->Branch("BugWaves", "TH1D", &debugWave);
  }

  LogInfo << "Initialized successfully" << endl;
  return true;
}

bool Deconvolution::execute()
{
  // LogDebug << "start PMT waveform unfolding" << endl;
  LogInfo << "Start PMT waveform unfolding" << endl;
  // preparation of the user output
  m_charge.clear();
  m_time.clear();
  m_pmtId.clear();
  m_width.clear();
  m_peak.clear();
  m_totalpe = 0;
  // read the electronics event
  SniperDataPtr<JM::NavBuffer> navBuf(getParent(), "/Event");
  if (navBuf.invalid()) {
    LogWarn << "can't find the buffer. " << std::endl;
    return false;
  }
  if (getRoot()->Snoopy().state() != Sniper::RunState::Running) {
    LogWarn << "Stop running." << std::endl;
    return true;
  }
  JM::EvtNavigator *nav = navBuf->curEvt();
  // LogInfo << "EvtNav: " << nav << std::endl;
  if (!nav) {
    LogWarn << "can't load event navigator." << std::endl;
    return getRoot()->stop();
  }
  JM::ElecHeader *eh = dynamic_cast<JM::ElecHeader *>(nav->getHeader("/Event/Elec"));
  // only use large pmts
  if (!eh->hasEvent()) return true;
  JM::ElecEvent *ee = dynamic_cast<JM::ElecEvent *>(eh->event());
  const JM::ElecFeeCrate &efc = ee->elecFeeCrate();
  m_crate = const_cast<JM::ElecFeeCrate *>(&efc);

  map<int, JM::ElecFeeChannel> feeChannels = m_crate->channelData();

  list<JM::CalibPMTChannel *> cpcl;  // CalibPMTChannel list

  map<int, JM::ElecFeeChannel>::iterator it;
  map<int, vector<double>>::iterator spe = m_SPERE.begin();
  for (it = feeChannels.begin(); it != feeChannels.end(); ++it) {

    JM::ElecFeeChannel &channel = (it->second);
    if (channel.adc().size() == 0) {
      continue;
    }
    if (channel.adc().size() != m_length) {
      LogError << "Length of the electronics simulation data is " << channel.adc().size() << " ns" << endl;
      LogError << "Length of the pre-defined length of deconvolution is " << m_length << " ns" << endl;
      LogError << "ERROR: inconsistent waveform length in the unfolding." << endl;
      return false;
    }
    int pmtID = it->first;  // remeber to check the conversion from electronics id to pmt id
    if (pmtID >= m_totalPMT) continue;
    double nPE = 0, firstHitTime = 0;
    vector<double> charge;
    vector<double> time;
    vector<int> width;
    vector<int> peak;
    if (hmmtpmt[pmtID] == true)
      m_threshold = m_threshold_dyn;
    else
      m_threshold = m_threshold_mcp;
    bool StatusC = calibrate(channel, pmtID, charge, time, width, peak);
    if (StatusC == false) {
      LogError << "ERROR when unfolding the waveform." << endl;
      return false;
    }

    if (charge.size() == 0) continue;  // Does not pass the threshold. Do not save.

    unsigned int detID = CdID::id(static_cast<unsigned int>(pmtID), 0);
    // Identifier id = Identifier(pmtID);
    for (int j = 0; j < int(charge.size()); j++) {
      // nPE += charge[j];
      // PMT calib
      if (m_c == 0) {
        charge[j] /= gain.at(pmtID);
        time[j] -= toffset.at(pmtID);
      }

      nPE += charge[j];
      m_charge.push_back(charge[j]);
      m_time.push_back(time[j]);
      m_pmtId.push_back(detID);
      m_width.push_back(width[j]);
      m_peak.push_back(peak[j]);
    }

    m_totalpe += nPE;
    if (charge.size() > 0) firstHitTime = time[0];
    JM::CalibPMTChannel *cpc = new JM::CalibPMTChannel;
    cpc->setNPE(nPE);
    cpc->setPmtId(detID);
    cpc->setFirstHitTime(firstHitTime);
    cpc->setTime(time);
    cpc->setCharge(charge);
    cpcl.push_back(cpc);
  }
  TimeStamp trigTimeStamp = m_crate->TriggerTime();
  m_trigtimesec = (int)trigTimeStamp.GetSec();
  m_trigtimenanosec = (int)trigTimeStamp.GetNanoSec();

  m_calib->Fill();
  JM::CalibEvent *ce = new JM::CalibEvent;
  ce->setCalibPMTCol(cpcl);
  JM::CalibHeader *ch = new JM::CalibHeader;
  ch->setEvent(ce);

  nav->addHeader("/Event/Calib", ch);

  // LogDebug << "End of the PMT Waveform Unfolding" << endl;
  ProcInfo_t info;
  gSystem->GetProcInfo(&info);
  double m_memory = info.fMemResident / 1024;
  LogInfo << "Memory cost: " << m_memory << endl;
  LogInfo << "End of the PMT Waveform Unfolding" << endl;
  return true;
}

bool Deconvolution::finalize()
{
  return true;
}

bool Deconvolution::calibrate(JM::ElecFeeChannel &channel, int pmtId, vector<double> &charge, vector<double> &time, vector<int> &width, vector<int> &peak)
{
  vector<unsigned int> &adc_int = channel.adc();
  if (adc_int.size() == 0) {
    return false;
  }
  vector<double> adc;
  for (int i = 0; i < adc_int.size(); i++) {
    // if (adc_int.at(i) >> 14 == 0) adc.push_back(double(adc_int.at(i)));
    // if (adc_int.at(i) >> 14 == 1) adc.push_back((double(adc_int.at(i)) - (1 << 14)) * .4 / .06);
    // if (adc_int.at(i) >> 8 == 2) adc.push_back((double(adc_int.at(i)) - (2 << 8)) * 8. / .06);
    double tmpadc = double(adc_int.at(i));
    if (tmpadc > 1e9) tmpadc = 0;  // Robust for unsigned int adc value becomes very large. If this bug is fixed, this part can be deleted.
    adc.push_back(tmpadc);
  }

  map<int, vector<double>>::iterator spe_re = m_SPERE.find(pmtId);
  map<int, vector<double>>::iterator spe_im = m_SPEIM.find(pmtId);
  if (spe_re == m_SPERE.end()) {
    if (m_stat < 10) {
      LogWarn << "Could not find the spe calibration waveform for PMT " << pmtId << ", and will use channel 0 as default." << endl;
    } else if (m_stat == 10) {
      LogWarn << "Further spe waveform warning will be suppressed." << endl;
    }
    m_stat++;
    spe_re = m_SPERE.begin();
    spe_im = m_SPEIM.begin();
  }

  // substract baseline
  double baseline = 0;
  for (int i = 0; i < 50 && i < int(adc.size()); i++) baseline += adc[i];
  baseline /= 50.;
  for (int i = 0; i < int(adc.size()); i++) {
    Raw->SetBinContent(i + 1, adc[i] - baseline);
  }

  // filter
  delete TVirtualFFT::GetCurrentTransform();
  TVirtualFFT::SetTransform(0);
  Freq = Raw->FFT(Freq, "MAG");
  const int N = m_length;
  std::vector<double> re_full_vec(N);
  std::vector<double> im_full_vec(N);
  double *re_full = &re_full_vec.front();
  double *im_full = &im_full_vec.front();
  for (int i = 0; i < N; i++) {
    re_full[i] = 0;
    im_full[i] = 0;
  }
  TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
  fft->GetPointsComplex(re_full, im_full);
  for (int i = 0; i < N; i++) {
    if (hmmtpmt.at(pmtId) == false) {
      re_full[i] *= m_filter_mcp[i];
      im_full[i] *= m_filter_mcp[i];
    } else {
      re_full[i] *= m_filter_dyn[i];
      im_full[i] *= m_filter_dyn[i];
    }
  }

  // division in frequency domain
  int NN = N;
  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &NN, "C2R M K");
  for (int i = 0; i < (spe_re->second).size(); i++) {
    if ((spe_re->second)[i] != 0 || (spe_im->second)[i] != 0) {
      double a11 = re_full[i], b11 = im_full[i], c11 = (spe_re->second)[i], d11 = (spe_im->second)[i];
      double f11 = c11 * c11 + d11 * d11;
      re_full[i] = (a11 * c11 + b11 * d11) / f11;
      im_full[i] = (b11 * c11 - a11 * d11) / f11;
    } else {
      re_full[i] = 0;
      im_full[i] = 0;
    }
  }
  for (int i = (spe_re->second).size(); i < m_length; i++) {
    re_full[i] = 0;
    im_full[i] = 0;
  }
  re_full[0] = 0;
  im_full[0] = 0;
  fft_back->SetPointsComplex(re_full, im_full);
  fft_back->Transform();
  Back = TH1::TransformHisto(fft_back, Back, "Re");
  Back->Scale(1. / N);

  // integral
  // double *AC = new double[N];
  std::vector<double> AC(N);
  std::vector<double> AC2;
  for (int i = 0; i < N; i++) {
    // time shift 100ns, because phase shifts in deconvolution, shifting time is based on average waveform starting time
    AC[i] = Back->GetBinContent((i + N - 100) % N + 1);
  }
  double max, min;
  Back->GetMinimumAndMaximum(min, max);
  int imax = int(max * 1000.);
  int imin = int(min * 1000.);
  AC2 = AC;
  // delete Back;
  // Back = 0;
  baseline = 0;
  delete fft_back;
  fft_back = 0;

  for (int i = 0; i < N; i++) {
    Raw->SetBinContent(i + 1, 0);
    Back->SetBinContent(i + 1, 0);
  }

  // wave recon
  std::vector<int> oneHit;
  std::vector<std::vector<int>> hit;
  std::vector<std::vector<int>> singleHit;
  std::vector<int> singleHitPeak;
  std::vector<int> hitPeakCounter;
  std::vector<int> hitWidth;

  // rough baseline
  // for (int i = 0; i < 40; i++) baseline += AC[i];
  // baseline /= 40.;
  const int tmpsize = imax - imin + 1;
  vector<int> tmpv(tmpsize);
  for (int i = 0; i < m_length; i++) {
    int tmpidx = int(AC[i] * 1000.);
    if (tmpidx >= tmpsize || tmpidx < imin) LogError << "Wrong idx size!" << std::endl;
    tmpv.at(tmpidx - imin)++;
  }
  baseline = max_element(tmpv.begin(), tmpv.end()) - tmpv.begin() + imin;
  baseline /= 1000.;
  for (int i = 0; i < N; i++) AC[i] -= baseline;

  // rough hits
  oneHit.clear();
  hit.clear();
  int Pass = 0;
  for (int i = 0; i < N; i++) {
    if (Pass == 0 && AC[i] > m_threshold) {
      for (int j = i; j >= 0; j--) {
        oneHit.insert(oneHit.begin(), j);
        if (AC[j] <= 0) break;
      }
      Pass = 1;
      continue;
    }
    if (Pass == 1) {
      oneHit.push_back(i);
      if (AC[i] <= 0 || i == N - 1) {
        if (oneHit.size() > 12) hit.push_back(oneHit);
        Pass = 0;
        oneHit.clear();
      }
    }
    if (hit.size() > 990) {
      LogWarn << "Too much hits recognized! Move to next waveform." << std::endl;
      break;  // if it really happend, it should print out warning and move to next waveform.
    }
  }

  // accurate baseline
  int flag;
  int counter = 0;
  baseline = 0;
  for (int i = 0; i < N; i++) {
    flag = 1;
    for (int j = 0; j < hit.size(); j++) {
      if (i >= hit.at(j).front() - 9 && i <= hit.at(j).back() + 9) {
        flag = 0;
        break;
      }
    }
    if (flag != 0) {
      baseline += AC[i];
      counter++;
    }
  }
  if (counter < 40) {
    LogWarn << "No enough points for baseline calculation, baseline will set to be the default one." << std::endl;  // make sure to have enough points to calculate baseline.
    if (debug == 1) {
      for (int i = 0; i < N; i++) debugWave->SetBinContent(i + 1, AC2[i]);
      TString debugTitle = Form("Error: No enough baseline points, Counted points = %d", counter);
      debugWave->SetTitle(debugTitle);
      m_debug->Fill();
    }
  } else if (counter != 0) {
    baseline /= (counter + 0.);
    for (int i = 0; i < N; i++) AC[i] -= baseline;
  }

  // accurate hits
  hit.clear();
  oneHit.clear();  // only when "if (hit.size() > 990) break;" happens, it will be used.
  Pass = 0;
  for (int i = 0; i < N; i++) {
    if (Pass == 0 && AC[i] > m_threshold) {
      for (int j = i; j >= 0; j--) {
        oneHit.insert(oneHit.begin(), j);
        if (AC[j] <= 0) break;
      }
      Pass = 1;
      continue;
    }
    if (Pass == 1) {
      oneHit.push_back(i);
      if (AC[i] <= 0 || i == N - 1) {
        if (oneHit.size() > 12) hit.push_back(oneHit);  // 12 ns width for J19
        Pass = 0;
        oneHit.clear();
      }
    }
    if (hit.size() > 990) {
      LogWarn << "Too much hits recognized! Move to next waveform." << std::endl;
      if (debug == 1) {
        for (int i = 0; i < N; i++) debugWave->SetBinContent(i + 1, AC2[i]);
        TString debugTitle = Form("Error: Too much hits! hits = %lu", hit.size());
        debugWave->SetTitle(debugTitle);
        m_debug->Fill();
      }
      break;  // if it really happend, it should print out warning and move to next waveform.
    }
  }

  // find singal hit
  singleHit.clear();
  singleHitPeak.clear();
  hitPeakCounter.clear();
  int Rise = 1;
  counter = 0;
  int HitCount = hit.size();
  int HitBTime[1000] = {0};
  int HitETime[1000] = {0};
  hitWidth.clear();
  for (int i = 0; i < hit.size(); i++) {
    Rise = 1;
    int sT = -1;
    int eT;
    hitPeakCounter.push_back(0);
    // record hit starting time and ending time
    HitBTime[i] = hit.at(i).front();
    HitETime[i] = hit.at(i).back();
    for (int j = 0; j < hit.at(i).size(); j++) {
      // find vally
      if (Rise == 0) {
        if (AC[hit.at(i).at(j)] > AC[hit.at(i).at(j - 1)]) {
          singleHit.push_back(oneHit);
          oneHit.clear();
          counter++;
          Rise = 1;
        }
      }
      oneHit.push_back(hit.at(i).at(j));
      // find peak
      if (Rise == 1 && j > 0) {
        if (AC[hit.at(i).at(j)] < AC[hit.at(i).at(j - 1)]) {
          singleHitPeak.push_back(hit.at(i).at(j - 1));
          if (AC[hit.at(i).at(j - 1)] >= m_threshold) hitPeakCounter.at(i)++;
          Rise = 0;
        }
      }
      // find overthreshold starting and ending point and their width
      if (AC[hit.at(i).at(j)] >= m_threshold) {
        eT = j;
        if (sT == -1) sT = j;
      }
    }
    // finish imcompleted hits, most are half peak at beginning or last of waveform
    if (oneHit.size() != 0) {
      if (Rise == 1) {
        singleHitPeak.push_back(hit.at(i).back());
        if (AC[hit.at(i).back()] >= m_threshold) hitPeakCounter.at(i)++;
      }
      singleHit.push_back(oneHit);
      oneHit.clear();
      counter++;
    }
    hitWidth.push_back(eT - sT + 1);
  }
  // exclude fake peaks which are underthreshold
  for (int i = 0; i < singleHitPeak.size(); i++) {
    if (AC[singleHitPeak.at(i)] < m_threshold) {
      singleHitPeak.erase(singleHitPeak.begin() + i);
      i--;
      counter--;
    }
  }
  // peak number test, robust check
  if (counter != singleHitPeak.size()) {
    LogWarn << "Error! Wrong hit peak counting!" << std::endl;
    if (debug == 1) {
      for (int i = 0; i < N; i++) debugWave->SetBinContent(i + 1, AC2[i]);
      TString debugTitle = Form("Error: Wrong hit peak counting! counter = %d, singleHitPeak.size() = %lu", counter, singleHitPeak.size());
      debugWave->SetTitle(debugTitle);
      m_debug->Fill();
    }
  }
  int tempN = 0;
  for (int i = 0; i < hitPeakCounter.size(); i++) tempN += hitPeakCounter.at(i);
  if (tempN != singleHitPeak.size() || hitPeakCounter.size() != hit.size()) {
    LogWarn << "Error2! Wrong hit peak counting!" << std::endl;
    if (debug == 1) {
      for (int i = 0; i < N; i++) debugWave->SetBinContent(i + 1, AC2[i]);
      TString debugTitle = Form("Error2: Wrong hit peak counting! tempN = %d, singleHitPeak.size() = %lu", tempN, singleHitPeak.size());
      debugWave->SetTitle(debugTitle);
      m_debug->Fill();
    }
  }
  for (int i = 0; i < hitWidth.size(); i++) {
    if (hitWidth.at(i) > 100 && debug == 1) {
      for (int i = 0; i < N; i++) debugWave->SetBinContent(i + 1, AC2[i]);
      TString debugTitle = Form("Warning: Too long width, width = %lu", hitWidth.at(i));
      debugWave->SetTitle(debugTitle);
      m_debug->Fill();
    }
  }

  int inteW = m_window;
  if (HitCount == 0) return true;
  for (int i = 0; i < HitCount; i++) {
    time.push_back(HitBTime[i]);
    int inte_begin = 0, inte_end = 0;
    int singleHit = 0;
    if (HitCount == 1) {
      inte_begin = HitBTime[i] - inteW + 1;
      if (inte_begin < 0) inte_begin = 0;
      inte_end = HitETime[i] + inteW;
      if (inte_end > N) inte_end = N;
      singleHit = 1;
    } else {
      if (i == 0) {
        inte_begin = HitBTime[i] - inteW + 1;
        if (inte_begin < 0) inte_begin = 0;
        int interval = HitBTime[1] - HitETime[0];
        if (interval > 2 * inteW) {
          inte_end = HitETime[0] + inteW;
          singleHit = 1;
        } else
          inte_end = int(HitETime[0] + interval / 2.);
      } else if (i == HitCount - 1) {
        inte_end = HitETime[i] + inteW;
        if (inte_end > N) inte_end = N;
        int interval = HitBTime[i] - HitETime[i - 1];
        if (interval > 2 * inteW) {
          inte_begin = HitBTime[i] - inteW + 1;
          singleHit = 1;
        } else
          inte_begin = int(HitBTime[i] - interval / 2.);
      } else {
        int single1 = 0, single2 = 0;
        int interval = HitBTime[i + 1] - HitETime[i];
        if (interval > inteW * 2) {
          inte_end = HitETime[i] + inteW;
          single1 = 1;
        } else
          inte_end = int(HitETime[i] + interval / 2.);
        interval = HitBTime[i] - HitETime[i - 1];
        if (interval > 2 * inteW) {
          inte_begin = HitBTime[i] - inteW + 1;
          single2 = 1;
        } else
          inte_begin = int(HitBTime[i] - interval / 2.);
        if (single1 == 1 && single2 == 1) singleHit = 1;
      }
    }
    double tempC = 0;
    for (int j = inte_begin; j < inte_end; j++) tempC += AC[j];
    if (m_hc) {  // the hit counting method is opened
      if (singleHit) {
        if (tempC < 1.55) tempC = 1;  // A simple hit counting, to remove the PMT amplitude fluctuations.
                                      // else if(tempC<2.5) tempC = 2;
                                      // else if(tempC<3.5) tempC = 3;
      }
    }
    charge.push_back(tempC);
  }
  width = hitWidth;
  peak = hitPeakCounter;

  return true;
}

#endif

