/*
 * Copyright (c) 2017 mniip
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <znc/version.h>

#if VERSION_MAJOR > 1 || (VERSION_MAJOR == 1 && VERSION_MINOR >= 7)
#define HAVE_CMESSAGE
#endif

#include <znc/znc.h>
#include <znc/Modules.h>
#include <znc/IRCNetwork.h>
#include <znc/User.h>
#ifdef HAVE_CMESSAGE
#include <znc/Message.h>
#endif

class CSnoHandleModule: public CModule
{
public:
 	MODCONSTRUCTOR(CSnoHandleModule)
	{
	}

#ifdef HAVE_CMESSAGE
	EModRet OnRawMessage(CMessage &message)
	{
		if(message.GetType() == CMessage::Type::Notice)
		{
			CIRCNetwork *network = GetNetwork();
			CString target = ((CNoticeMessage)message).GetTarget();
			if(!target.Equals(network->GetNick()))
				if(!network->FindChan(target))
				{
					bool result = false;
					NETWORKMODULECALL(OnPrivNoticeMessage(message), network->GetUser(), network, nullptr, &result);
					return result ? HALT : CONTINUE;
				}
		}
		return CONTINUE;
	}
#else
	EModRet OnRaw(CString &line)
	{
		CString cmd = line.Token(1);
		if(cmd.Equals("NOTICE"))
		{
			CIRCNetwork *network = GetNetwork();
			CString rest = line.Token(2, true);
			CString target = rest.Token(0);
			if(!target.Equals(network->GetNick()))
				if(!network->FindChan(target))
				{
					CNick nick(line.Token(0).TrimPrefix_n());
					CString msg = rest.Token(1, true);
					msg.LeftChomp();
					bool result = false;
					NETWORKMODULECALL(OnPrivNotice(nick, msg), network->GetUser(), network, nullptr, &result);
					return result ? HALT : CONTINUE;
				}
		}
		return CONTINUE;
	}
#endif
};

template <>
void TModInfo<CSnoHandleModule>(CModInfo& Info) {
	Info.SetWikiPage("snohandle");
}

NETWORKMODULEDEFS(CSnoHandleModule, "Handle server notices as private")
