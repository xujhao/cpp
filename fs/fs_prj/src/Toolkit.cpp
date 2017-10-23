#include "Toolkit.h"
#include <cctype>
#include "Poco/File.h"

#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

bool CToolkit::IsAliasAcct(const std::string &ssAcct)
{
  for (std::string::const_iterator cit = ssAcct.begin(); cit != ssAcct.end(); cit++)
    if(std::isalpha(*cit)) return true;

  return false;
}

bool CToolkit::GetStrinIfSet(Poco::AutoPtr<Poco::Util::IniFileConfiguration> apCfg, const std::string &ssProperty, std::string &ssValue)
{
	ssValue = "";
	if (apCfg->hasProperty(ssProperty))
	{
		ssValue = apCfg->getString(ssProperty);
		return true;
	}

	return false;
}

std::string CToolkit::GenAcctAliasKey(const FIX::SessionID &oSessionID, const std::string &ssOnBehalfOfCompID, const std::string &ssAccountAlias)
{
  return oSessionID.toString() + "|" + ssOnBehalfOfCompID + "|" + ssAccountAlias;
}

std::string CToolkit::GenAcctAliasKey(const FIX::Message& oRecvMsg, const std::string &ssAccount)
{
	FIX::OnBehalfOfCompID onBehalfOfCompId;
	//std::string ssOnBehalfOfCompID = oRecvMsg.getHeader().getFieldIfSet(onBehalfOfCompId) ? onBehalfOfCompId.getValue() : "";
  oRecvMsg.getHeader().getFieldIfSet(onBehalfOfCompId);

	return CToolkit::GenAcctAliasKey(oRecvMsg.getSessionID(), onBehalfOfCompId.getValue(), ssAccount);
}

bool CToolkit::IsExist(const std::string &ssFilePath)
{
	Poco::File file = Poco::File(ssFilePath);
	return file.exists();
}

std::string CToolkit::GetUuid()
{
  Poco::LocalDateTime now;
  return Poco::DateTimeFormatter::format(now, "%Y%m%d%H%M%S%i");
}

std::string CToolkit::GetSessionKey(const FIX::Message& oRecvMsg)
{
  FIX::OnBehalfOfCompID onBehalfOfCompId;
  oRecvMsg.getHeader().getFieldIfSet(onBehalfOfCompId);
  return oRecvMsg.getSessionID().toString() + "|" + onBehalfOfCompId.getValue();
}

bool CToolkit::IsTdRequest(const FIX::MsgType &msgType)
{
  return msgType == FIX::MsgType_NewOrderSingle 
    || msgType == FIX::MsgType_OrderCancelRequest 
    || msgType == FIX::MsgType_OrderStatusRequest ? true : false;
}

bool CToolkit::IsMdRequest(const FIX::MsgType &msgType)
{
  return msgType == FIX::MsgType_MarketDataRequest ? true : false;
}

