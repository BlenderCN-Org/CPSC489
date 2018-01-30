#include "stdafx.h"
#include "app.h"
#include "errors.h"

// error variables
typedef std::map<LanguageCode, STDSTRINGW> language_map_t;
typedef std::map<ErrorCode, language_map_t> codemap_t;
static codemap_t codemap;
static LanguageCode language = LC_ENGLISH;

void InsertErrorString(ErrorCode code, LanguageCode language, LPCWSTR error)
{
 typedef std::pair<codemap_t::iterator, bool> result_type;
 result_type result;
 result = codemap.insert(codemap_t::value_type(code, language_map_t()));
 result.first->second.insert(language_map_t::value_type(language, error));
}

void InitErrorStrings(void)
{
 // free pre-existing data
 FreeErrorStrings();

 // Common Errors
 InsertErrorString(EC_SUCCESS, LC_ENGLISH, L"");
 InsertErrorString(EC_SUCCESS, LC_FRENCH, L"");

 // Windows Errors
 InsertErrorString(EC_WIN32_REGISTER_WINDOW, LC_ENGLISH, L"Failed to register window class.");
 InsertErrorString(EC_WIN32_REGISTER_WINDOW, LC_FRENCH, L"Impossible d'enregistrer la classe de la fenêtre.");

 InsertErrorString(EC_WIN32_MAIN_WINDOW, LC_ENGLISH, L"Invalid main window.");
 InsertErrorString(EC_WIN32_MAIN_WINDOW, LC_FRENCH, L"Fenêtre principale invalide.");

 // templates
 // InsertErrorString(, LC_ENGLISH, L"");
 // InsertErrorString(, LC_FRENCH, L"");
}

void FreeErrorStrings(void)
{
 codemap.clear();
}

STDSTRINGW FindError(ErrorCode code, LanguageCode language)
{
 auto iter = codemap.find(code);
 if(iter == codemap.end()) {
    STDSTRINGSTREAMW ss;
    ss << L"Error message code 0x" << std::hex << code << std::dec << L" not found.";
    return ss.str();
   }
 auto msgiter = iter->second.find(language);
 if(msgiter == iter->second.end()) return L"Error message not found for this language.";
 return msgiter->second;
}

bool Error(ErrorCode code)
{
 STDSTRINGW error = FindError(code, GetLanguageCode());
 ErrorBox(error.c_str());
 return false;
}

bool Error(ErrorCode code, LanguageCode language)
{
 STDSTRINGW error = FindError(code, language);
 ErrorBox(error.c_str());
 return false;
}

LanguageCode GetLanguageCode(void)
{
 return ::language;
}

void SetLanguageCode(LanguageCode code)
{
 ::language = code; 
}