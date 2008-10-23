#pragma once
#include "TimeSeries.h"

#include <vector>

#include "FastDelegate.h"
using namespace fastdelegate;

#include "MergeDatedDatumCarrier.h"
#include "MinHeap.h"

class CMergeDatedDatums {
public:
  CMergeDatedDatums(void);
  virtual ~CMergeDatedDatums(void);

  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;
  //typedef FastDelegate1<const CTrade &> OnTradeHandler;

  void Add( CTimeSeries<CQuote> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CTrade> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CBar> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CMarketDepth> *pSeries, OnDatumHandler );
  void Run( void );
  void Stop( void );
  unsigned long GetCountProcessedDatums( void ) { return m_cntProcessedDatums; };
  unsigned long GetCountCarrierReorders( void ) { return m_cntReorders; };
protected:
  // this could be reimplemented with a linked list, if a linked list is sortable,
  //   ie, do the ordered insertions one at a time
  // or stay with a vector and do binary search each time
  //std::vector<CMergeCarrierBase *> m_vCarriers;
  CMinHeap<CMergeCarrierBase *, CMergeCarrierBase> m_vCarriers;

  // not all states or commands are implemented yet
  enum enumMergingState { eInit, eRunning, ePaused, eStopped };
  enum enumMergingCommands { eUnknown, eRun, eStop, ePause, eResume, eReset };

  enumMergingState m_state;
  enumMergingCommands m_request;

  unsigned long m_cntProcessedDatums;
  unsigned long m_cntReorders;


private:
};
