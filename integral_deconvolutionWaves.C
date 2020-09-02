#include<vector>
#include "TString.h"
#include "TH1D.h"
#include "TFile.h"
void integral_deconvolutionWaves(const TString name)
{
    using namespace std;

    TFile* f_waves = new TFile()


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
