//PassCat - Passwords Recovery Tool
//This file is part of PassCat Project

//Written by : @maldevel
//Website : https ://www.twelvesec.com/
//GIT : https://github.com/twelvesec/passcat

//TwelveSec(@Twelvesec)

//This program is free software : you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.If not, see < http://www.gnu.org/licenses/>.

//For more see the file 'LICENSE' for copying permission.

#include "libfilezilla.h"

#include <iostream>
#include "config.h"
#include "libsystem.h"
#include "libxml.h"

#pragma comment (lib, "Shlwapi.lib")
#include <Shlwapi.h>

void _print_passwords(std::wstring filename, std::wstring XPATH) {

	MSXML::IXMLDOMNodeListPtr list = libxml::select_by_path(filename, XPATH);

	for (long i = 0; i != list->length; ++i) {

		std::wcout << "Host: " << list->item[i]->selectSingleNode("Host")->text << std::endl;
		std::wcout << "Port: " << list->item[i]->selectSingleNode("Port")->text << std::endl;
		std::wcout << "Username: " << list->item[i]->selectSingleNode("User")->text << std::endl;

		BYTE* decoded = 0;
		DWORD decodedLen = 0;

		if (!CryptStringToBinaryW(list->item[i]->selectSingleNode("Pass")->text, 0, CRYPT_STRING_BASE64, NULL, &decodedLen, NULL, NULL)) {
			std::wcout << std::endl;
			continue;
		}

		decoded = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (decodedLen + 1) * sizeof(BYTE));
		if (decoded == NULL) {
			std::wcout << std::endl;
			continue;
		}

		if (!CryptStringToBinaryW(list->item[i]->selectSingleNode("Pass")->text, 0, CRYPT_STRING_BASE64, decoded, &decodedLen, NULL, NULL)) {
			std::wcout << std::endl;
			HeapFree(GetProcessHeap(), 0, decoded);
			continue;
		}

		std::cout << "Password: " << decoded << std::endl;
		std::wcout << std::endl;
		HeapFree(GetProcessHeap(), 0, decoded);
	}
}

void libfilezilla::print_filezilla_passwords(void) {
	std::wstring filezilla_path = libsystem::get_filezilla_path();
	std::wstring filezilla_recent_servers = filezilla_path + L"\\" + FILEZILLA_FILE_ONE;
	std::wstring filezilla_site_manager = filezilla_path + L"\\" + FILEZILLA_FILE_TWO;

	if (!PathFileExistsW(filezilla_recent_servers.c_str())) {
		return;
	}

	_print_passwords(filezilla_recent_servers, FILEZILLA_XPATH_ONE);

	if (!PathFileExistsW(filezilla_site_manager.c_str())) {
		return;
	}

	_print_passwords(filezilla_site_manager, FILEZILLA_XPATH_TWO);
}