#ifndef __SGITMDSPI_H__
#define __SGITMDSPI_H__

#include "sgit/SgitFtdcMdApi.h"
#include "quickfix/fix42/MarketDataRequest.h"
#include "quickfix/fix42/MarketDataSnapshotFullRefresh.h"
#include "Convert.h"
#include <float.h>
#include "Poco/RWLock.h"
#include "Const.h"
#include <math.h>

using namespace fstech;
using namespace  std;

std::string const ALL_SYMBOL = "all";
double const THRESHOLD = 2*DBL_MIN;

inline bool feq( double const x, double const y ) { return fabs( x - y ) < THRESHOLD; }

class CSgitContext;
class CSgitMdSpi : public CThostFtdcMdSpi
{
public:
  CSgitMdSpi(CSgitContext *pSgitCtx, CThostFtdcMdApi *pMdReqApi, const std::string &ssTradeId, const std::string &ssPassword);
  virtual ~CSgitMdSpi();

  bool OnMessage(const FIX::Message& oMsg, const FIX::SessionID& oSessionID, std::string& ssErrMsg);

  bool MarketDataRequest(const FIX42::MarketDataRequest& oMarketDataRequest, std::string& ssErrMsg);

protected:

  bool GetMarketData(const std::string ssSymbol, CThostFtdcDepthMarketDataField &stuMarketData);

  bool CheckValid(
    const std::set<std::string> &symbolSetIn, std::set<std::string> &symbolSetOrg,
    const std::string &ssMDReqID, char chScrbReqType, const STUScrbParm &stuScrbParm, char &chRejReason, std::string &ssErrMsg);

  //发送快照
  bool SendMarketDataSet(const FIX42::MarketDataRequest& oMarketDataRequest, const std::set<std::string> &symbolSet, const STUScrbParm &stuScrbParm, std::string &ssErrMsg);

  //发布行情
  void PubMarketData(const CThostFtdcDepthMarketDataField &stuDepthMarketData);

  FIX42::MarketDataSnapshotFullRefresh CreateSnapShot(
    const CThostFtdcDepthMarketDataField &stuMarketData, 
    const STUScrbParm &stuScrbParm, 
    const std::string &ssMDReqID = "");

  void AddPrice(FIX42::MarketDataSnapshotFullRefresh &oMdSnapShot, char chEntryType, double dPrice, int iVolume = 0, int iPos = 0);

  //建立订阅关系
  void AddSub(const std::set<std::string> &symbolSet, const STUScrbParm &stuScrbParm);

  //取消订阅关系
  void DelSub(const std::set<std::string> &symbolSet, const STUScrbParm &stuScrbParm);

  void Send(const std::string &ssSessionKey, FIX42::MarketDataSnapshotFullRefresh oMdSnapShot);

  void AddFixInfo(const FIX::Message& oMsg);

  ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
  virtual void OnFrontConnected();

  ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
  ///@param nReason 错误原因
  ///        0x1001 网络读失败
  ///        0x1002 网络写失败
  ///        0x2001 接收心跳超时
  ///        0x2002 发送心跳失败
  ///        0x2003 收到错误报文
  virtual void OnFrontDisconnected(int nReason);

  ///心跳超时警告。当长时间未收到报文时，该方法被调用。
  ///@param nTimeLapse 距离上次接收报文的时间
  virtual void OnHeartBeatWarning(int nTimeLapse);


  ///登录请求响应
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///登出请求响应
  virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

  ///错误应答
  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///订阅行情应答
  virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///取消订阅行情应答
  virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///订阅询价应答
  virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

  ///取消订阅询价应答
  virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

  ///深度行情通知
  virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

  ///询价通知
  virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {};

  ///递延交割行情
  virtual void OnRtnDeferDeliveryQuot(CThostDeferDeliveryQuot* pQuot){};

private:
  CSgitContext														              *m_pSgitCtx;
  CThostFtdcMdApi													              *m_pMdReqApi;
	AtomicCounter														              m_acRequestId;
  CThostFtdcReqUserLoginField                           m_stuLogin;

  //行情MDReqID (262)记录，用于判断是否有重复
  std::map<std::string, std::set<std::string> >         m_mapMDReqID;
  FastMutex													                    m_fastmutexLockMDReqID;

  //订阅关系 代码->订阅session
  std::map<std::string, std::set<STUScrbParm> >         m_mapCode2ScrbParmSet; 
  RWLock                                                m_rwLockCode2ScrbParmSet;

  ////订阅全量代码的session
  //std::set<std::string>                                 m_setSubAllCodeSession;

  //保存全市场行情快照
  std::map<std::string, CThostFtdcDepthMarketDataField> m_mapSnapshot;
  //全市场行情快照读写锁
  RWLock                                                m_rwLockSnapShot;
};

#endif // __SGITMDSPI_H__
