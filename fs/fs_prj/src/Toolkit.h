#ifndef __TOOLKIT_H__
#define __TOOLKIT_H__

#include <string>
#include "Poco/Util/IniFileConfiguration.h"

class CToolkit
{
public:
  static bool isAliasAcct(const std::string &ssAcct);

	static bool getStrinIfSet(
    Poco::AutoPtr<Poco::Util::IniFileConfiguration> apCfg, 
    const std::string &ssProperty, 
    std::string &ssValue);
  
  static std::string GenAcctAliasKey(
    const std::string &ssTargetCompID, 
    const std::string &ssOnBehalfOfCompID, 
    const std::string &ssTradeID); 
};
#endif // __TOOLKIT_H__