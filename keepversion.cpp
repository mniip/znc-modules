#include <znc/znc.h>
#include <znc/Modules.h>

class CKeepVersionModule: public CModule
{
public:
	MODCONSTRUCTOR(CKeepVersionModule) {}

	virtual EModRet OnUserCTCPReply(CString &target, CString &message) override
	{
		CString const &pattern = " via " + CZNC::GetTag(false);
		if(message.Token(0) == "VERSION" && message.EndsWith(pattern))
			message = message.substr(0, message.size() - pattern.size());
		return CONTINUE;
	}
};

template<> void TModInfo<CKeepVersionModule>(CModInfo &info)
{
	info.SetWikiPage("keepversion");
}

NETWORKMODULEDEFS(CKeepVersionModule, "Preserve the original version string sent by the client")
