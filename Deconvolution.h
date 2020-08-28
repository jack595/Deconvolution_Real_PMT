#ifndef Deconvolution_h
#define Deconvolution_h

#include <boost/python.hpp>
#include "SniperKernel/AlgBase.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include <map>
#include "TH1D.h"
#include "TTree.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
class ElecFeeCrate;
class DataBufSvcV2;

namespace JM {
class ElecFeeCrate;
class ElecFeeChannel;
class CalibHeader;
}

class Deconvolution : public AlgBase {
  public:
  Deconvolution(const std::string& name);
  ~Deconvolution();

  public:
  bool initialize();
  bool execute();
  bool finalize();

  private:
  std::vector<bool> hmmtpmt;
  bool calibrate(JM::ElecFeeChannel& channel, int pmtId, std::vector<double>& charge, std::vector<double>& time, std::vector<int>& width, std::vector<int>& peak);

  private:
  IDataMemMgr* m_memMgr;

  JM::ElecFeeCrate* m_crate;
  std::string m_CalibFile;
  std::string m_FilterFile;
  std::map<int, std::vector<double>> m_SPERE;
  std::map<int, std::vector<double>> m_SPEIM;
  std::vector<double> m_filter_mcp;
  std::vector<double> m_filter_dyn;
  int m_totalPMT;
  double m_threshold;
  double m_threshold_mcp;
  double m_threshold_dyn;
  TH1D* Raw;
  TH1* Freq;
  TH1* Back;
  int m_stat;
  double m_para1;
  double m_para2;
  double m_para3;
  double m_para4;
  double m_para5;
  double m_para6;
  int m_length;
  int m_window;
  int m_hc;
  int m_c;

  // user data definitions
  std::vector<float> m_charge;
  std::vector<float> m_time;
  std::vector<int> m_pmtId;
  std::vector<int> m_width;
  std::vector<int> m_peak;
  std::vector<double> gain;     // gain correction from calib data
  std::vector<double> toffset;  // gain correction from calib data
  float m_totalpe;
  int m_trigtimesec;
  int m_trigtimenanosec;
  TTree* m_calib;
  bool filter_flag;

  int debug;
  TTree* m_debug;
  TH1D* debugWave;
};

#endif

