/*
 * Copyright (c) 2015 mniip
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

#include <znc/Modules.h>
#include <znc/Chan.h>
#include <znc/Client.h>
#include <set>

class CSnomaskModule: public CModule
{
private:
	CString DefaultSnoticeDefs =
		"# Charybdis server notices: (except +s)\n"
		"C \\*\\*\\* Notice -- CLICONN * * * * * * 0 *\n"
		"C \\*\\*\\* Notice -- CLIEXIT * * * * * *\n"
		"Z \\*\\*\\* Notice -- OPERSPY * * *\n"
		"b \\*\\*\\* Notice -- Possible Flooder *[*@*] on * target: *\n"
		"b \\*\\*\\* Notice -- Excessive target change from * (*@*)\n"
		"b \\*\\*\\* Notice -- User * (*@*) trying to join * is a possible spambot\n"
		"c \\*\\*\\* Notice -- Client connecting: * (*@*) [*] {*} [*]\n"
		"c \\*\\*\\* Notice -- Client exiting: * (*@*) [*] [*]\n"
		"d \\*\\*\\* Notice -- WHOX overflow while sending information about * to *\n"
		"d \\*\\*\\* Notice -- Bad user@host: *@* From: *(via *)\n"
		"d \\*\\*\\* Notice -- Bad realhost: * From: *(via *)\n"
		"d \\*\\*\\* Notice -- Ignored noop SAVE message for * from *\n"
		"d \\*\\*\\* Notice -- \\*\\*\\* Bogus TS * on * ignored from *\n"
		"d \\*\\*\\* Notice -- Send message to *[*] dropped from *(Fake Dir)\n"
		"d \\*\\*\\* Notice -- Bad Nick from SIGNON: * From: *(via *)\n"
		"d \\*\\*\\* Notice -- Bad user@host from SIGNON: *@* From: *(via *)\n"
		"d \\*\\*\\* Notice -- Umode +* is now orphaned\n"
		"d \\*\\*\\* Notice -- Orphaned umode +* is picked up by module\n"
		"d \\*\\*\\* Notice -- Message for *[*] from *\n"
		"d \\*\\*\\* Notice -- Message for *[*@*!*] from * (TS, ignored)\n"
		"d \\*\\*\\* Notice -- Unknown prefix (*) from *, Squitting *\n"
		"d \\*\\*\\* Notice -- Invalid prefix (*) from *\n"
		"d \\*\\*\\* Notice -- \\*\\*\\* Long channel name from * (* > *): *\n"
		"f \\*\\*\\* Notice -- Too many clients, rejecting *[*].\n"
		"f \\*\\*\\* Notice -- Too many local connections for *!*@*\n"
		"f \\*\\*\\* Notice -- Too many global connections for *!*@*\n"
		"f \\*\\*\\* Notice -- Too many user connections for *!*@*\n"
		"f \\*\\*\\* Notice -- I-line is full for *!*@* (*)\n"
		"k \\*\\*\\* Notice -- Received KILL message for *!*@*. From * Path: *!*!*!* *\n"
		"k \\*\\*\\* Notice -- Received KILL message for *!*@*. From * *\n"
		"k \\*\\*\\* Notice -- Ignored SAVE message for * from *\n"
		"k \\*\\*\\* Notice -- Nick collision on *(* <- *)(both *)\n"
		"k \\*\\*\\* Notice -- Nick collision on *(* <- *)(older *)\n"
		"k \\*\\*\\* Notice -- Nick collision on *(* <- *)(newer *)\n"
		"k \\*\\*\\* Notice -- Nick change collision from * to *(* <- *)(both *)\n"
		"k \\*\\*\\* Notice -- Nick change collision from * to *(* <- *)(older *)\n"
		"k \\*\\*\\* Notice -- Nick change collision from * to *(* <- *)(newer *)\n"
		"k \\*\\*\\* Notice -- Received SAVE message for * from *\n"
		"k \\*\\*\\* Notice -- Nick collision due to services forced nick change on *\n"
		"k \\*\\*\\* Notice -- Nick collision due to SVSLOGIN on *\n"
		"n \\*\\*\\* Notice -- Nick change: From * to * [*@*]\n"
		"r \\*\\*\\* Notice -- Listed on DNSBL *: * (*@*) [*] [*]\n"
		"r \\*\\*\\* Notice -- Invalid username: * (*@*)\n"
		"u \\*\\*\\* Notice -- Unauthorised client connection from *!*@* [*] on [*/*].\n"
		"x \\*\\*\\* Notice -- Server * being introduced by *\n"
		"x \\*\\*\\* Notice -- Server * split from *\n"
		"y \\*\\*\\* Notice -- User * (*@*) is attempting to join locally juped channel * (*)\n"
		"y \\*\\*\\* Notice -- trace requested by * (*@*) [*] on *\n"
		"y \\*\\*\\* Notice -- trace requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- info requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- LINKS '*' requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- admin requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- STATS p requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- STATS * requested by * (*@*) [*] on *\n"
		"y \\*\\*\\* Notice -- STATS * requested by * (*@*) [*]\n"
		"y \\*\\*\\* Notice -- motd requested by * (*@*) [*]";

public:
 	MODCONSTRUCTOR(CSnomaskModule)
	{
		AddHelpCommand();
		AddCommand("Snomask", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnSnomaskCommand), "<snomask>", "Update the snomask.");
		AddCommand("AddClient", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnAddClientCommand), "<identifier>", "Add a client.");
		AddCommand("DelClient", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnDelClientCommand), "<identifier>", "Remove a client.");
		AddCommand("ListClients", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnListClientsCommand), "", "List clients and their snomasks.");
		AddCommand("ListDefs", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnListDefsCommand), "<char> <pattern>", "Print snotice definitions.");
		AddCommand("AddDef", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnAddDefCommand), "<char> <pattern>", "Add a snotice definition. Pattern can contain *, \\* and \\\\.");
		AddCommand("DelDef", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnDelDefCommand), "<number>", "Delete a definition by index.");
		AddCommand("SwapDefs", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnSwapDefsCommand), "<number> <number>", "Swap two definitions by index.");
		AddCommand("DefaultDefs", static_cast<CModCommand::ModCmdFunc>(&CSnomaskModule::OnDefaultDefsCommand), "", "Restore the default set of definitions.");
	}

private:
	CString MergeMask(CString const &mask, CString const &update)
	{
		std::set<char> letters(mask.begin(), mask.end());
		bool add = true;
		for(char c : update)
		{
			if(c == '+')
				add = true;
			else if(c == '-')
				add = false;
			else if(isalnum(c))
			{
				if(add)
					letters.insert(c);
				else
					letters.erase(c);
			}
		}
		CString ret = "";
		for(std::set<char>::const_iterator it = letters.begin(); it != letters.end(); it++)
			ret.push_back(*it);
		return ret;
	}

	CString Unescape(CString const &str)
	{
		CString ret = "";
		for(size_t p = 0; p < str.size(); p++)
		{
			if(str[p] == '\\')
			{
				p++;
				if(p >= str.size())
					break;
			}
			ret.push_back(str[p]);
		}
		return ret;
	}

	bool WildMatch(CString const &pattern, CString const &string)
	{
		VCString parts;
		size_t last = 0;
		for(size_t p = 0; p < pattern.size(); p++)
			if(pattern[p] == '\\')
			{
				p++;
			}
			else if(pattern[p] == '*')
			{
				parts.push_back(Unescape(pattern.substr(last, p - last)));
				last = p + 1;
			}
		parts.push_back(Unescape(pattern.substr(last, pattern.size() - last)));
		size_t j = 0;
		for(size_t i = 0; i < parts.size(); i++)
		{
			if(i == 0)
			{
				size_t pos = string.find(parts[i]);
				if(pos != 0)
					return false;
				j = parts[i].size();
			}
			else if(i == parts.size() - 1)
			{
				size_t pos = string.rfind(parts[i]);
				if(pos != string.size() - parts[i].size())
					return false;
			}
			else
			{
				size_t pos = string.find(parts[i], j);
				if(pos == std::string::npos)
					return false;
				j = pos + parts[i].size();
			}
		}
		return true;
	}

	CString GetDefinitions()
	{
		if(FindNV("definitions") == EndNV())
			SetNV("definitions", DefaultSnoticeDefs);
		return GetNV("definitions");
	}

	char GetSnomaskChar(CString const &notice)
	{
		SCString defs;
		GetDefinitions().Split("\n", defs);
		for(CString const &def : defs)
			if(def.size() >= 2 && def[0] != '#')
				if(WildMatch(def.substr(2), notice))
					return def[0];
		return 0;
	}

	CString GetClientSnomask(CString const &identifier)
	{
		return GetNV("mask_" + identifier);
	}

	void SetClientSnomask(CString const &identifier, CString const &mask)
	{
		SetNV("mask_" + identifier, mask);
	}

	void DeleteClient(CString const &identifier)
	{
		DelNV("mask_" + identifier);
	}

	bool HaveClient(CString const &identifier)
	{
		return !identifier.empty() && FindNV("mask_" + identifier) != EndNV();
	}

	void OnAddClientCommand(CString const &line)
	{
		CString identifier = line.Token(1);
		if(identifier.empty())
		{
			identifier = GetClient()->GetIdentifier();
			if(identifier.empty())
			{
				PutModule("No identifier provided and current client has no identifier");
				return;
			}
		}
		if(HaveClient(identifier))
		{
			PutModule("Client already exists: " + identifier);
			return;
		}
		SetClientSnomask(identifier, "");
		PutModule("Added client: " + identifier);
	}

	void OnDelClientCommand(CString const &line)
	{
		CString identifier = line.Token(1);
		if(identifier.empty())
		{
			identifier = GetClient()->GetIdentifier();
			if(identifier.empty())
			{
				PutModule("No identifier provided and current client has no identifier");
				return;
			}
		}
		if(!HaveClient(identifier))
		{
			PutModule("Unknown client: " + identifier);
			return;
		}
		DeleteClient(identifier);
		PutModule("Deleted client: " + identifier);
	}

	void OnListClientsCommand(CString const &line)
	{
		CString const current = GetClient()->GetIdentifier();
		CTable table;
		table.AddColumn("Client");
		table.AddColumn("Snomask");
		for(MCString::iterator it = BeginNV(); it != EndNV(); it++)
			if(it->first.StartsWith("mask_"))
			{
				table.AddRow();
				CString const identifier = it->first.substr(CString("mask_").size());
				if(identifier.Equals(current))
					table.SetCell("Client", "*" + identifier);
				else
					table.SetCell("Client", identifier);
				table.SetCell("Snomask", "+" + it->second);
			}
		if(table.empty())
			PutModule("No clients defined");
		else
			PutModule(table);
	}

	void OnSnomaskCommand(CString const &line)
	{
		CString const mask = line.Token(1);
		CString const identifier = GetClient()->GetIdentifier();
		if(identifier.empty())
		{
			PutModule("Current client has no identifier");
			return;
		}
		if(!HaveClient(identifier))
		{
			PutModule("Unknown client: " + identifier);
			return;
		}
		CString const newmask = MergeMask(GetClientSnomask(identifier), mask);
		SetClientSnomask(identifier, newmask);
		PutModule("Snomask: +" + newmask);
	}

	VCString Parse()
	{
		VCString ret;
		GetDefinitions().Split("\n", ret);
		return ret;
	}

	void Save(VCString list)
	{
		SetNV("definitions", CString("\n").Join(list.begin(), list.end()));
	}

	void OnListDefsCommand(CString const &line)
	{
		VCString list = Parse();
		CTable table;
		table.AddColumn("Id");
		table.AddColumn("Definition");
		for(size_t i = 0; i < list.size(); i++)
		{
			table.AddRow();
			table.SetCell("Id", CString(i + 1));
			table.SetCell("Definition", list[i]);
		}
		if(list.size())
			PutModule(table);
		else
			PutModule("No definitions defined");
	}

	void OnAddDefCommand(CString const &line)
	{
		VCString list = Parse();
		CString const definition = line.Token(1, true);
		list.push_back(definition);
		Save(list);
		PutModule("Added definition: " + definition);
	}

	void OnDelDefCommand(CString const &line)
	{
		unsigned int i = line.Token(1).ToUInt();
		VCString list = Parse();
		if(i > list.size() || i <= 0)
		{
			PutModule("Illegal number requested");
			return;
		}
		CString const removed = list[i - 1];
		list.erase(list.begin() + (i - 1));
		Save(list);
		PutModule("Removed definition: " + removed);
	}

	void OnSwapDefsCommand(CString const &line)
	{
		unsigned int i = line.Token(1).ToUInt();
		unsigned int j = line.Token(2).ToUInt();
		VCString list = Parse();
		if(i > list.size() || i <= 0 || j > list.size() || j <= 0)
		{
			PutModule("Illegal number requested");
			return;
		}
		CString const tmp = list[i - 1];
		list[i - 1] = list[j - 1];
		list[j - 1] = tmp;
		Save(list);
		PutModule("Swapped definitions");
	}

	void OnDefaultDefsCommand(CString const &line)
	{
		SetNV("definitions", DefaultSnoticeDefs);
		OnListDefsCommand(line);
	}

public:
	EModRet OnSendToClient(CString &line, CClient &client) override
	{
		CString const identifier = client.GetIdentifier();
		if(client.GetNetwork() && HaveClient(identifier))
		{
			CString msg = line;
			if(msg.StartsWith("@"))
				msg = msg.Token(1, true);

			CString const source = msg.Token(0);
			CString const cmd = msg.Token(1);
			if(cmd.Equals("NOTICE") && !source.Contains("!"))
			{
				CString const notice = msg.Token(3, true).TrimPrefix_n(":");
				char sno = GetSnomaskChar(notice);
				if(sno != 0 && !GetClientSnomask(identifier).Contains(CString(sno)))
					return HALT;
			}
		}
		return CONTINUE;
	}

	CString GetWebMenuTitle() override { return "Snomask"; }

	bool OnWebRequest(CWebSock &sock, CString const &pageName, CTemplate &tmpl) override
	{
		if(pageName == "index")
		{
			if(sock.IsPost())
				SetNV("definitions", sock.GetRawParam("definitions", true));

			tmpl["definitions"] = GetDefinitions();
			for(MCString::iterator it = BeginNV(); it != EndNV(); it++)
				if(it->first.StartsWith("mask_"))
				{
					CTemplate &row = tmpl.AddRow("ClientLoop");
					row["Client"] = it->first.substr(CString("mask_").size());
					row["Snomask"] = "+" + it->second;
				}
			return true;
		}
		else if(pageName == "defaultdefs")
		{
			SetNV("definitions", DefaultSnoticeDefs);
			sock.Redirect(GetWebPath());
			return true;
		}
		else if(pageName == "addclient")
		{
			if(sock.IsPost())
			{
				CString const &identifier = sock.GetParam("client").Token(0);
				if(!identifier.size())
				{
					sock.PrintErrorPage("Client identifier can't be empty");
					return true;
				}
				SetClientSnomask(identifier, MergeMask("", sock.GetParam("snomask")));
				sock.Redirect(GetWebPath());
			}
			return true;
		}
		else if(pageName == "editclient")
		{
			if(sock.IsPost())
			{
				CString const &identifier = sock.GetParam("client");
				if(!HaveClient(identifier))
				{
					sock.PrintErrorPage("No such client identifier: '" + identifier + "'");
					return true;
				}
				SetClientSnomask(identifier, MergeMask("", sock.GetParam("snomask")));
				sock.Redirect(GetWebPath());
			}
			else
			{
				CString const &identifier = sock.GetParam("client", false);
				if(!HaveClient(identifier))
				{
					sock.PrintErrorPage("No such client identifier");
					return true;
				}
				tmpl["client"] = identifier;
				tmpl["snomask"] = GetClientSnomask(identifier);
			}
			return true;
		}
		else if(pageName == "delclient")
		{
			DeleteClient(sock.GetParam("client", false));
			sock.Redirect(GetWebPath());
			return true;
		}
		return false;
	}
};

template <>
void TModInfo<CSnomaskModule>(CModInfo& Info) {
	Info.SetWikiPage("snomask");
	Info.SetHasArgs(false);
}

NETWORKMODULEDEFS(CSnomaskModule, "Client identifier based server notice filter")
