#include "Toolkit.h"
#include <cctype>
#include "Poco/File.h"

bool CToolkit::isAliasAcct(const std::string &ssAcct)
{
  for (std::string::const_iterator cit = ssAcct.begin(); cit != ssAcct.end(); cit++)
    if(std::isalpha(*cit)) return true;

  return false;
}

bool CToolkit::getStrinIfSet(Poco::AutoPtr<Poco::Util::IniFileConfiguration> apCfg, const std::string &ssProperty, std::string &ssValue)
{
	ssValue = "";
	if (apCfg->hasProperty(ssProperty))
	{
		ssValue = apCfg->getString(ssProperty);
		return true;
	}

	return false;
}

std::string CToolkit::GenAcctAliasKey(const std::string &ssTargetCompID, const std::string &ssOnBehalfOfCompID, const std::string &ssTradeID)
{
  return ssTargetCompID + "|" + ssOnBehalfOfCompID + "|" + ssTradeID;
}

std::string CToolkit::GetAcctAliasKey(const std::string &ssAccount, const FIX::Message& oMsg)
{
	FIX::SenderCompID senderCompId;
	FIX::OnBehalfOfCompID onBehalfOfCompId;
	std::string ssSenderCompId = oMsg.getHeader().getFieldIfSet(senderCompId) ? senderCompId.getValue() : "";
	std::string ssOnBehalfOfCompID = oMsg.getHeader().getFieldIfSet(onBehalfOfCompId) ? onBehalfOfCompId.getValue() : "";

	return CToolkit::GenAcctAliasKey(ssSenderCompId, ssOnBehalfOfCompID, ssAccount);
}

bool CToolkit::isExist(const std::string &ssFilePath)
{
	Poco::File file = Poco::File(ssFilePath);
	return file.exists();
}

