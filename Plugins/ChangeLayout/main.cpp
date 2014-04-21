#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
static void fEnRu(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'`' : *pBuf = L'ё'; break;
        case L'q' : *pBuf = L'й'; break;
        case L'w' : *pBuf = L'ц'; break;
        case L'e' : *pBuf = L'у'; break;
        case L'r' : *pBuf = L'к'; break;
        case L't' : *pBuf = L'е'; break;
        case L'y' : *pBuf = L'н'; break;
        case L'u' : *pBuf = L'г'; break;
        case L'i' : *pBuf = L'ш'; break;
        case L'o' : *pBuf = L'щ'; break;
        case L'p' : *pBuf = L'з'; break;
        case L'[' : *pBuf = L'х'; break;
        case L']' : *pBuf = L'ъ'; break;
        case L'a' : *pBuf = L'ф'; break;
        case L's' : *pBuf = L'ы'; break;
        case L'd' : *pBuf = L'в'; break;
        case L'f' : *pBuf = L'а'; break;
        case L'g' : *pBuf = L'п'; break;
        case L'h' : *pBuf = L'р'; break;
        case L'j' : *pBuf = L'о'; break;
        case L'k' : *pBuf = L'л'; break;
        case L'l' : *pBuf = L'д'; break;
        case L';' : *pBuf = L'ж'; break;
        case L'\'': *pBuf = L'э'; break;
        case L'z' : *pBuf = L'я'; break;
        case L'x' : *pBuf = L'ч'; break;
        case L'c' : *pBuf = L'с'; break;
        case L'v' : *pBuf = L'м'; break;
        case L'b' : *pBuf = L'и'; break;
        case L'n' : *pBuf = L'т'; break;
        case L'm' : *pBuf = L'ь'; break;
        case L',' : *pBuf = L'б'; break;
        case L'.' : *pBuf = L'ю'; break;
        case L'/' : *pBuf = L'.'; break;
        case L'~' : *pBuf = L'Ё'; break;
        case L'@' : *pBuf = L'"'; break;
        case L'#' : *pBuf = L'№'; break;
        case L'$' : *pBuf = L';'; break;
        case L'^' : *pBuf = L':'; break;
        case L'&' : *pBuf = L'?'; break;
        case L'Q' : *pBuf = L'Й'; break;
        case L'W' : *pBuf = L'Ц'; break;
        case L'E' : *pBuf = L'У'; break;
        case L'R' : *pBuf = L'К'; break;
        case L'T' : *pBuf = L'Е'; break;
        case L'Y' : *pBuf = L'Н'; break;
        case L'U' : *pBuf = L'Г'; break;
        case L'I' : *pBuf = L'Ш'; break;
        case L'O' : *pBuf = L'Щ'; break;
        case L'P' : *pBuf = L'З'; break;
        case L'{' : *pBuf = L'Х'; break;
        case L'}' : *pBuf = L'Ъ'; break;
        case L'|' : *pBuf = L'/'; break;
        case L'A' : *pBuf = L'Ф'; break;
        case L'S' : *pBuf = L'Ы'; break;
        case L'D' : *pBuf = L'В'; break;
        case L'F' : *pBuf = L'А'; break;
        case L'G' : *pBuf = L'П'; break;
        case L'H' : *pBuf = L'Р'; break;
        case L'J' : *pBuf = L'О'; break;
        case L'K' : *pBuf = L'Л'; break;
        case L'L' : *pBuf = L'Д'; break;
        case L':' : *pBuf = L'Ж'; break;
        case L'"' : *pBuf = L'Э'; break;
        case L'Z' : *pBuf = L'Я'; break;
        case L'X' : *pBuf = L'Ч'; break;
        case L'C' : *pBuf = L'С'; break;
        case L'V' : *pBuf = L'М'; break;
        case L'B' : *pBuf = L'И'; break;
        case L'N' : *pBuf = L'Т'; break;
        case L'M' : *pBuf = L'Ь'; break;
        case L'<' : *pBuf = L'Б'; break;
        case L'>' : *pBuf = L'Ю'; break;
        case L'?' : *pBuf = L','; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
static void fRuEn(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'ё' : *pBuf = L'`'; break;
        case L'й' : *pBuf = L'q'; break;
        case L'ц' : *pBuf = L'w'; break;
        case L'у' : *pBuf = L'e'; break;
        case L'к' : *pBuf = L'r'; break;
        case L'е' : *pBuf = L't'; break;
        case L'н' : *pBuf = L'y'; break;
        case L'г' : *pBuf = L'u'; break;
        case L'ш' : *pBuf = L'i'; break;
        case L'щ' : *pBuf = L'o'; break;
        case L'з' : *pBuf = L'p'; break;
        case L'х' : *pBuf = L'['; break;
        case L'ъ' : *pBuf = L']'; break;
        case L'ф' : *pBuf = L'a'; break;
        case L'ы' : *pBuf = L's'; break;
        case L'в' : *pBuf = L'd'; break;
        case L'а' : *pBuf = L'f'; break;
        case L'п' : *pBuf = L'g'; break;
        case L'р' : *pBuf = L'h'; break;
        case L'о' : *pBuf = L'j'; break;
        case L'л' : *pBuf = L'k'; break;
        case L'д' : *pBuf = L'l'; break;
        case L'ж' : *pBuf = L';'; break;
        case L'э' : *pBuf = L'\'';break;
        case L'я' : *pBuf = L'z'; break;
        case L'ч' : *pBuf = L'x'; break;
        case L'с' : *pBuf = L'c'; break;
        case L'м' : *pBuf = L'v'; break;
        case L'и' : *pBuf = L'b'; break;
        case L'т' : *pBuf = L'n'; break;
        case L'ь' : *pBuf = L'm'; break;
        case L'б' : *pBuf = L','; break;
        case L'ю' : *pBuf = L'.'; break;
        case L'.' : *pBuf = L'/'; break;
        case L'Ё' : *pBuf = L'~'; break;
        case L'"' : *pBuf = L'@'; break;
        case L'№' : *pBuf = L'#'; break;
        case L';' : *pBuf = L'$'; break;
        case L':' : *pBuf = L'^'; break;
        case L'?' : *pBuf = L'&'; break;
        case L'Й' : *pBuf = L'Q'; break;
        case L'Ц' : *pBuf = L'W'; break;
        case L'У' : *pBuf = L'E'; break;
        case L'К' : *pBuf = L'R'; break;
        case L'Е' : *pBuf = L'T'; break;
        case L'Н' : *pBuf = L'Y'; break;
        case L'Г' : *pBuf = L'U'; break;
        case L'Ш' : *pBuf = L'I'; break;
        case L'Щ' : *pBuf = L'O'; break;
        case L'З' : *pBuf = L'P'; break;
        case L'Х' : *pBuf = L'{'; break;
        case L'Ъ' : *pBuf = L'}'; break;
        case L'/' : *pBuf = L'|'; break;
        case L'Ф' : *pBuf = L'A'; break;
        case L'Ы' : *pBuf = L'S'; break;
        case L'В' : *pBuf = L'D'; break;
        case L'А' : *pBuf = L'F'; break;
        case L'П' : *pBuf = L'G'; break;
        case L'Р' : *pBuf = L'H'; break;
        case L'О' : *pBuf = L'J'; break;
        case L'Л' : *pBuf = L'K'; break;
        case L'Д' : *pBuf = L'L'; break;
        case L'Ж' : *pBuf = L':'; break;
        case L'Э' : *pBuf = L'"'; break;
        case L'Я' : *pBuf = L'Z'; break;
        case L'Ч' : *pBuf = L'X'; break;
        case L'С' : *pBuf = L'C'; break;
        case L'М' : *pBuf = L'V'; break;
        case L'И' : *pBuf = L'B'; break;
        case L'Т' : *pBuf = L'N'; break;
        case L'Ь' : *pBuf = L'M'; break;
        case L'Б' : *pBuf = L'<'; break;
        case L'Ю' : *pBuf = L'>'; break;
        case L',' : *pBuf = L'?'; break;
        case L'і' : *pBuf = L's'; break;
        case L'І' : *pBuf = L'S'; break;
        case L'ї' : *pBuf = L']'; break;
        case L'Ї' : *pBuf = L'}'; break;
        case L'є' : *pBuf = L'\'';break;
        case L'Є' : *pBuf = L'"'; break;
        case L'ў' : *pBuf = L'o'; break;
        case L'Ў' : *pBuf = L'O'; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
static void fDefault(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'`' : *pBuf = L'ё'; break;
        case L'q' : *pBuf = L'й'; break;
        case L'w' : *pBuf = L'ц'; break;
        case L'e' : *pBuf = L'у'; break;
        case L'r' : *pBuf = L'к'; break;
        case L't' : *pBuf = L'е'; break;
        case L'y' : *pBuf = L'н'; break;
        case L'u' : *pBuf = L'г'; break;
        case L'i' : *pBuf = L'ш'; break;
        case L'o' : *pBuf = L'щ'; break;
        case L'p' : *pBuf = L'з'; break;
        case L'[' : *pBuf = L'х'; break;
        case L']' : *pBuf = L'ъ'; break;
        case L'a' : *pBuf = L'ф'; break;
        case L's' : *pBuf = L'ы'; break;
        case L'd' : *pBuf = L'в'; break;
        case L'f' : *pBuf = L'а'; break;
        case L'g' : *pBuf = L'п'; break;
        case L'h' : *pBuf = L'р'; break;
        case L'j' : *pBuf = L'о'; break;
        case L'k' : *pBuf = L'л'; break;
        case L'l' : *pBuf = L'д'; break;
        case L';' : *pBuf = L'ж'; break;
        case L'\'': *pBuf = L'э'; break;
        case L'z' : *pBuf = L'я'; break;
        case L'x' : *pBuf = L'ч'; break;
        case L'c' : *pBuf = L'с'; break;
        case L'v' : *pBuf = L'м'; break;
        case L'b' : *pBuf = L'и'; break;
        case L'n' : *pBuf = L'т'; break;
        case L'm' : *pBuf = L'ь'; break;
        case L',' : *pBuf = L'б'; break;
        case L'.' : *pBuf = L'ю'; break;
        case L'~' : *pBuf = L'Ё'; break;
        case L'@' : *pBuf = L'"'; break;
        case L'#' : *pBuf = L'№'; break;
        case L'$' : *pBuf = L';'; break;
        case L'^' : *pBuf = L':'; break;
        case L'&' : *pBuf = L'?'; break;
        case L'Q' : *pBuf = L'Й'; break;
        case L'W' : *pBuf = L'Ц'; break;
        case L'E' : *pBuf = L'У'; break;
        case L'R' : *pBuf = L'К'; break;
        case L'T' : *pBuf = L'Е'; break;
        case L'Y' : *pBuf = L'Н'; break;
        case L'U' : *pBuf = L'Г'; break;
        case L'I' : *pBuf = L'Ш'; break;
        case L'O' : *pBuf = L'Щ'; break;
        case L'P' : *pBuf = L'З'; break;
        case L'{' : *pBuf = L'Х'; break;
        case L'}' : *pBuf = L'Ъ'; break;
        case L'|' : *pBuf = L'/'; break;
        case L'A' : *pBuf = L'Ф'; break;
        case L'S' : *pBuf = L'Ы'; break;
        case L'D' : *pBuf = L'В'; break;
        case L'F' : *pBuf = L'А'; break;
        case L'G' : *pBuf = L'П'; break;
        case L'H' : *pBuf = L'Р'; break;
        case L'J' : *pBuf = L'О'; break;
        case L'K' : *pBuf = L'Л'; break;
        case L'L' : *pBuf = L'Д'; break;
        case L':' : *pBuf = L'Ж'; break;
        case L'"' : *pBuf = L'Э'; break;
        case L'Z' : *pBuf = L'Я'; break;
        case L'X' : *pBuf = L'Ч'; break;
        case L'C' : *pBuf = L'С'; break;
        case L'V' : *pBuf = L'М'; break;
        case L'B' : *pBuf = L'И'; break;
        case L'N' : *pBuf = L'Т'; break;
        case L'M' : *pBuf = L'Ь'; break;
        case L'<' : *pBuf = L'Б'; break;
        case L'>' : *pBuf = L'Ю'; break;
            //
        case L'ё' : *pBuf = L'`'; break;
        case L'й' : *pBuf = L'q'; break;
        case L'ц' : *pBuf = L'w'; break;
        case L'у' : *pBuf = L'e'; break;
        case L'к' : *pBuf = L'r'; break;
        case L'е' : *pBuf = L't'; break;
        case L'н' : *pBuf = L'y'; break;
        case L'г' : *pBuf = L'u'; break;
        case L'ш' : *pBuf = L'i'; break;
        case L'щ' : *pBuf = L'o'; break;
        case L'з' : *pBuf = L'p'; break;
        case L'х' : *pBuf = L'['; break;
        case L'ъ' : *pBuf = L']'; break;
        case L'ф' : *pBuf = L'a'; break;
        case L'ы' : *pBuf = L's'; break;
        case L'в' : *pBuf = L'd'; break;
        case L'а' : *pBuf = L'f'; break;
        case L'п' : *pBuf = L'g'; break;
        case L'р' : *pBuf = L'h'; break;
        case L'о' : *pBuf = L'j'; break;
        case L'л' : *pBuf = L'k'; break;
        case L'д' : *pBuf = L'l'; break;
        case L'ж' : *pBuf = L';'; break;
        case L'э' : *pBuf = L'\'';break;
        case L'я' : *pBuf = L'z'; break;
        case L'ч' : *pBuf = L'x'; break;
        case L'с' : *pBuf = L'c'; break;
        case L'м' : *pBuf = L'v'; break;
        case L'и' : *pBuf = L'b'; break;
        case L'т' : *pBuf = L'n'; break;
        case L'ь' : *pBuf = L'm'; break;
        case L'б' : *pBuf = L','; break;
        case L'ю' : *pBuf = L'.'; break;
        case L'Ё' : *pBuf = L'~'; break;
        case L'№' : *pBuf = L'#'; break;
        case L'Й' : *pBuf = L'Q'; break;
        case L'Ц' : *pBuf = L'W'; break;
        case L'У' : *pBuf = L'E'; break;
        case L'К' : *pBuf = L'R'; break;
        case L'Е' : *pBuf = L'T'; break;
        case L'Н' : *pBuf = L'Y'; break;
        case L'Г' : *pBuf = L'U'; break;
        case L'Ш' : *pBuf = L'I'; break;
        case L'Щ' : *pBuf = L'O'; break;
        case L'З' : *pBuf = L'P'; break;
        case L'Х' : *pBuf = L'{'; break;
        case L'Ъ' : *pBuf = L'}'; break;
        case L'Ф' : *pBuf = L'A'; break;
        case L'Ы' : *pBuf = L'S'; break;
        case L'В' : *pBuf = L'D'; break;
        case L'А' : *pBuf = L'F'; break;
        case L'П' : *pBuf = L'G'; break;
        case L'Р' : *pBuf = L'H'; break;
        case L'О' : *pBuf = L'J'; break;
        case L'Л' : *pBuf = L'K'; break;
        case L'Д' : *pBuf = L'L'; break;
        case L'Ж' : *pBuf = L':'; break;
        case L'Э' : *pBuf = L'"'; break;
        case L'Я' : *pBuf = L'Z'; break;
        case L'Ч' : *pBuf = L'X'; break;
        case L'С' : *pBuf = L'C'; break;
        case L'М' : *pBuf = L'V'; break;
        case L'И' : *pBuf = L'B'; break;
        case L'Т' : *pBuf = L'N'; break;
        case L'Ь' : *pBuf = L'M'; break;
        case L'Б' : *pBuf = L'<'; break;
        case L'Ю' : *pBuf = L'>'; break;
        case L'і' : *pBuf = L's'; break;
        case L'І' : *pBuf = L'S'; break;
        case L'ї' : *pBuf = L']'; break;
        case L'Ї' : *pBuf = L'}'; break;
        case L'є' : *pBuf = L'\'';break;
        case L'Є' : *pBuf = L'"'; break;
        case L'ў' : *pBuf = L'o'; break;
        case L'Ў' : *pBuf = L'O'; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
static void fUpper(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'a': *pBuf = L'A'; break;
        case L'b': *pBuf = L'B'; break;
        case L'c': *pBuf = L'C'; break;
        case L'd': *pBuf = L'D'; break;
        case L'e': *pBuf = L'E'; break;
        case L'f': *pBuf = L'F'; break;
        case L'g': *pBuf = L'G'; break;
        case L'h': *pBuf = L'H'; break;
        case L'i': *pBuf = L'I'; break;
        case L'j': *pBuf = L'J'; break;
        case L'k': *pBuf = L'K'; break;
        case L'l': *pBuf = L'L'; break;
        case L'm': *pBuf = L'M'; break;
        case L'n': *pBuf = L'N'; break;
        case L'o': *pBuf = L'O'; break;
        case L'p': *pBuf = L'P'; break;
        case L'q': *pBuf = L'Q'; break;
        case L'r': *pBuf = L'R'; break;
        case L's': *pBuf = L'S'; break;
        case L't': *pBuf = L'T'; break;
        case L'u': *pBuf = L'U'; break;
        case L'v': *pBuf = L'V'; break;
        case L'w': *pBuf = L'W'; break;
        case L'x': *pBuf = L'X'; break;
        case L'y': *pBuf = L'Y'; break;
        case L'z': *pBuf = L'Z'; break;
        case L'ä': *pBuf = L'Ä'; break;
        case L'ö': *pBuf = L'Ö'; break;
        case L'ü': *pBuf = L'Ü'; break;
            //
        case L'а': *pBuf = L'А'; break;
        case L'б': *pBuf = L'Б'; break;
        case L'в': *pBuf = L'В'; break;
        case L'г': *pBuf = L'Г'; break;
        case L'д': *pBuf = L'Д'; break;
        case L'е': *pBuf = L'Е'; break;
        case L'ё': *pBuf = L'Ё'; break;
        case L'ж': *pBuf = L'Ж'; break;
        case L'з': *pBuf = L'З'; break;
        case L'и': *pBuf = L'И'; break;
        case L'й': *pBuf = L'Й'; break;
        case L'к': *pBuf = L'К'; break;
        case L'л': *pBuf = L'Л'; break;
        case L'м': *pBuf = L'М'; break;
        case L'н': *pBuf = L'Н'; break;
        case L'о': *pBuf = L'О'; break;
        case L'п': *pBuf = L'П'; break;
        case L'р': *pBuf = L'Р'; break;
        case L'с': *pBuf = L'С'; break;
        case L'т': *pBuf = L'Т'; break;
        case L'у': *pBuf = L'У'; break;
        case L'ф': *pBuf = L'Ф'; break;
        case L'х': *pBuf = L'Х'; break;
        case L'ц': *pBuf = L'Ц'; break;
        case L'ч': *pBuf = L'Ч'; break;
        case L'ш': *pBuf = L'Ш'; break;
        case L'щ': *pBuf = L'Щ'; break;
        case L'ъ': *pBuf = L'Ъ'; break;
        case L'ы': *pBuf = L'Ы'; break;
        case L'ь': *pBuf = L'Ь'; break;
        case L'э': *pBuf = L'Э'; break;
        case L'ю': *pBuf = L'Ю'; break;
        case L'я': *pBuf = L'Я'; break;
        case L'ґ': *pBuf = L'Ґ'; break;
        case L'і': *pBuf = L'І'; break;
        case L'ї': *pBuf = L'Ї'; break;
        case L'є': *pBuf = L'Є'; break;
        case L'ў': *pBuf = L'Ў'; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
static void fLower(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'A': *pBuf = L'a'; break;
        case L'B': *pBuf = L'b'; break;
        case L'C': *pBuf = L'c'; break;
        case L'D': *pBuf = L'd'; break;
        case L'E': *pBuf = L'e'; break;
        case L'F': *pBuf = L'f'; break;
        case L'G': *pBuf = L'g'; break;
        case L'H': *pBuf = L'h'; break;
        case L'I': *pBuf = L'i'; break;
        case L'J': *pBuf = L'j'; break;
        case L'K': *pBuf = L'k'; break;
        case L'L': *pBuf = L'l'; break;
        case L'M': *pBuf = L'm'; break;
        case L'N': *pBuf = L'n'; break;
        case L'O': *pBuf = L'o'; break;
        case L'P': *pBuf = L'p'; break;
        case L'Q': *pBuf = L'q'; break;
        case L'R': *pBuf = L'r'; break;
        case L'S': *pBuf = L's'; break;
        case L'T': *pBuf = L't'; break;
        case L'U': *pBuf = L'u'; break;
        case L'V': *pBuf = L'v'; break;
        case L'W': *pBuf = L'w'; break;
        case L'X': *pBuf = L'x'; break;
        case L'Y': *pBuf = L'y'; break;
        case L'Z': *pBuf = L'z'; break;
        case L'Ä': *pBuf = L'ä'; break;
        case L'Ö': *pBuf = L'ö'; break;
        case L'Ü': *pBuf = L'ü'; break;
            //
        case L'А': *pBuf = L'а'; break;
        case L'Б': *pBuf = L'б'; break;
        case L'В': *pBuf = L'в'; break;
        case L'Г': *pBuf = L'г'; break;
        case L'Д': *pBuf = L'д'; break;
        case L'Е': *pBuf = L'е'; break;
        case L'Ё': *pBuf = L'ё'; break;
        case L'Ж': *pBuf = L'ж'; break;
        case L'З': *pBuf = L'з'; break;
        case L'И': *pBuf = L'и'; break;
        case L'Й': *pBuf = L'й'; break;
        case L'К': *pBuf = L'к'; break;
        case L'Л': *pBuf = L'л'; break;
        case L'М': *pBuf = L'м'; break;
        case L'Н': *pBuf = L'н'; break;
        case L'О': *pBuf = L'о'; break;
        case L'П': *pBuf = L'п'; break;
        case L'Р': *pBuf = L'р'; break;
        case L'С': *pBuf = L'с'; break;
        case L'Т': *pBuf = L'т'; break;
        case L'У': *pBuf = L'у'; break;
        case L'Ф': *pBuf = L'ф'; break;
        case L'Х': *pBuf = L'х'; break;
        case L'Ц': *pBuf = L'ц'; break;
        case L'Ч': *pBuf = L'ч'; break;
        case L'Ш': *pBuf = L'ш'; break;
        case L'Щ': *pBuf = L'щ'; break;
        case L'Ъ': *pBuf = L'ъ'; break;
        case L'Ы': *pBuf = L'ы'; break;
        case L'Ь': *pBuf = L'ь'; break;
        case L'Э': *pBuf = L'э'; break;
        case L'Ю': *pBuf = L'ю'; break;
        case L'Я': *pBuf = L'я'; break;
        case L'Ґ': *pBuf = L'ґ'; break;
        case L'І': *pBuf = L'і'; break;
        case L'Ї': *pBuf = L'ї'; break;
        case L'Є': *pBuf = L'є'; break;
        case L'Ў': *pBuf = L'ў'; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
static void fInvert(wchar_t *pBuf, wchar_t *wChar)
{
    while (true)
    {
        switch (*wChar)
        {
        case L'A': *pBuf = L'a'; break;
        case L'B': *pBuf = L'b'; break;
        case L'C': *pBuf = L'c'; break;
        case L'D': *pBuf = L'd'; break;
        case L'E': *pBuf = L'e'; break;
        case L'F': *pBuf = L'f'; break;
        case L'G': *pBuf = L'g'; break;
        case L'H': *pBuf = L'h'; break;
        case L'I': *pBuf = L'i'; break;
        case L'J': *pBuf = L'j'; break;
        case L'K': *pBuf = L'k'; break;
        case L'L': *pBuf = L'l'; break;
        case L'M': *pBuf = L'm'; break;
        case L'N': *pBuf = L'n'; break;
        case L'O': *pBuf = L'o'; break;
        case L'P': *pBuf = L'p'; break;
        case L'Q': *pBuf = L'q'; break;
        case L'R': *pBuf = L'r'; break;
        case L'S': *pBuf = L's'; break;
        case L'T': *pBuf = L't'; break;
        case L'U': *pBuf = L'u'; break;
        case L'V': *pBuf = L'v'; break;
        case L'W': *pBuf = L'w'; break;
        case L'X': *pBuf = L'x'; break;
        case L'Y': *pBuf = L'y'; break;
        case L'Z': *pBuf = L'z'; break;
        case L'Ä': *pBuf = L'a'; break;
        case L'Ö': *pBuf = L'o'; break;
        case L'Ü': *pBuf = L'u'; break;
        case L'a': *pBuf = L'A'; break;
        case L'b': *pBuf = L'B'; break;
        case L'c': *pBuf = L'C'; break;
        case L'd': *pBuf = L'D'; break;
        case L'e': *pBuf = L'E'; break;
        case L'f': *pBuf = L'F'; break;
        case L'g': *pBuf = L'G'; break;
        case L'h': *pBuf = L'H'; break;
        case L'i': *pBuf = L'I'; break;
        case L'j': *pBuf = L'J'; break;
        case L'k': *pBuf = L'K'; break;
        case L'l': *pBuf = L'L'; break;
        case L'm': *pBuf = L'M'; break;
        case L'n': *pBuf = L'N'; break;
        case L'o': *pBuf = L'O'; break;
        case L'p': *pBuf = L'P'; break;
        case L'q': *pBuf = L'Q'; break;
        case L'r': *pBuf = L'R'; break;
        case L's': *pBuf = L'S'; break;
        case L't': *pBuf = L'T'; break;
        case L'u': *pBuf = L'U'; break;
        case L'v': *pBuf = L'V'; break;
        case L'w': *pBuf = L'W'; break;
        case L'x': *pBuf = L'X'; break;
        case L'y': *pBuf = L'Y'; break;
        case L'z': *pBuf = L'Z'; break;
        case L'ä': *pBuf = L'Ä'; break;
        case L'ö': *pBuf = L'Ö'; break;
        case L'ü': *pBuf = L'Ü'; break;
            //
        case L'А': *pBuf = L'а'; break;
        case L'Б': *pBuf = L'б'; break;
        case L'В': *pBuf = L'в'; break;
        case L'Г': *pBuf = L'г'; break;
        case L'Д': *pBuf = L'д'; break;
        case L'Е': *pBuf = L'е'; break;
        case L'Ё': *pBuf = L'ё'; break;
        case L'Ж': *pBuf = L'ж'; break;
        case L'З': *pBuf = L'з'; break;
        case L'И': *pBuf = L'и'; break;
        case L'Й': *pBuf = L'й'; break;
        case L'К': *pBuf = L'к'; break;
        case L'Л': *pBuf = L'л'; break;
        case L'М': *pBuf = L'м'; break;
        case L'Н': *pBuf = L'н'; break;
        case L'О': *pBuf = L'о'; break;
        case L'П': *pBuf = L'п'; break;
        case L'Р': *pBuf = L'р'; break;
        case L'С': *pBuf = L'с'; break;
        case L'Т': *pBuf = L'т'; break;
        case L'У': *pBuf = L'у'; break;
        case L'Ф': *pBuf = L'ф'; break;
        case L'Х': *pBuf = L'х'; break;
        case L'Ц': *pBuf = L'ц'; break;
        case L'Ч': *pBuf = L'ч'; break;
        case L'Ш': *pBuf = L'ш'; break;
        case L'Щ': *pBuf = L'щ'; break;
        case L'Ъ': *pBuf = L'ъ'; break;
        case L'Ы': *pBuf = L'ы'; break;
        case L'Ь': *pBuf = L'ь'; break;
        case L'Э': *pBuf = L'э'; break;
        case L'Ю': *pBuf = L'ю'; break;
        case L'Я': *pBuf = L'я'; break;
        case L'Ґ': *pBuf = L'ґ'; break;
        case L'І': *pBuf = L'і'; break;
        case L'Ї': *pBuf = L'ї'; break;
        case L'Є': *pBuf = L'є'; break;
        case L'Ў': *pBuf = L'ў'; break;
        case L'а': *pBuf = L'А'; break;
        case L'б': *pBuf = L'Б'; break;
        case L'в': *pBuf = L'В'; break;
        case L'г': *pBuf = L'Г'; break;
        case L'д': *pBuf = L'Д'; break;
        case L'е': *pBuf = L'Е'; break;
        case L'ё': *pBuf = L'Ё'; break;
        case L'ж': *pBuf = L'Ж'; break;
        case L'з': *pBuf = L'З'; break;
        case L'и': *pBuf = L'И'; break;
        case L'й': *pBuf = L'Й'; break;
        case L'к': *pBuf = L'К'; break;
        case L'л': *pBuf = L'Л'; break;
        case L'м': *pBuf = L'М'; break;
        case L'н': *pBuf = L'Н'; break;
        case L'о': *pBuf = L'О'; break;
        case L'п': *pBuf = L'П'; break;
        case L'р': *pBuf = L'Р'; break;
        case L'с': *pBuf = L'С'; break;
        case L'т': *pBuf = L'Т'; break;
        case L'у': *pBuf = L'У'; break;
        case L'ф': *pBuf = L'Ф'; break;
        case L'х': *pBuf = L'Х'; break;
        case L'ц': *pBuf = L'Ц'; break;
        case L'ч': *pBuf = L'Ч'; break;
        case L'ш': *pBuf = L'Ш'; break;
        case L'щ': *pBuf = L'Щ'; break;
        case L'ъ': *pBuf = L'Ъ'; break;
        case L'ы': *pBuf = L'Ы'; break;
        case L'ь': *pBuf = L'Ь'; break;
        case L'э': *pBuf = L'Э'; break;
        case L'ю': *pBuf = L'Ю'; break;
        case L'я': *pBuf = L'Я'; break;
        case L'ґ': *pBuf = L'Ґ'; break;
        case L'і': *pBuf = L'І'; break;
        case L'ї': *pBuf = L'Ї'; break;
        case L'є': *pBuf = L'Є'; break;
        case L'ў': *pBuf = L'Ў'; break;
        case L'\0': *pBuf = L'\0';return;
        default   : *pBuf = *wChar; break;
        }
        ++pBuf;
        ++wChar;
    }
}

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool)
{
    return true;
}

//-------------------------------------------------------------------------------------------------
void EXPORT fMsg(const wchar_t *wMsg)
{
    char cType;
    if (wcscmp(wMsg, L"/en-ru") == 0)
        cType = 'e';
    else if (wcscmp(wMsg, L"/ru-en") == 0)
        cType = 'r';
    else if (wcscmp(wMsg, L"/default") == 0)
        cType = 'd';
    else if (wcscmp(wMsg, L"/upper") == 0)
        cType = 'u';
    else if (wcscmp(wMsg, L"/lower") == 0)
        cType = 'l';
    else if (wcscmp(wMsg, L"/invert") == 0)
        cType = 'i';
    else
        return;

    if (GetAsyncKeyState(VK_CONTROL) < 0)
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    if (GetAsyncKeyState(VK_SHIFT) < 0)
        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    if (GetAsyncKeyState(VK_MENU) < 0)
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event('C', 0, 0, 0);
    keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

    Sleep(50);

    if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(0))
    {
        bool bValid = false;
        if (wchar_t *const hClipboardText = static_cast<wchar_t*>(GetClipboardData(CF_UNICODETEXT)))
            if (GlobalLock(hClipboardText))
            {
                if (const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (wcslen(hClipboardText)+1)*sizeof(wchar_t)))
                {
                    if (wchar_t *const pBuf = static_cast<wchar_t*>(GlobalLock(hGlobal)))
                    {
                        switch (cType)
                        {
                        case 'e': fEnRu(pBuf, hClipboardText); break;
                        case 'r': fRuEn(pBuf, hClipboardText); break;
                        case 'd': fDefault(pBuf, hClipboardText); break;
                        case 'u': fUpper(pBuf, hClipboardText); break;
                        case 'l': fLower(pBuf, hClipboardText); break;
                        case 'i': fInvert(pBuf, hClipboardText); break;
                        }

                        GlobalUnlock(hClipboardText);

                        if (EmptyClipboard())
                        {
                            if (SetClipboardData(CF_UNICODETEXT, hGlobal))
                            {
                                if (GlobalUnlock(hGlobal) == 0)
                                {
                                    if (GlobalFree(hGlobal) == 0)
                                        bValid = true;
                                }
                                else
                                    GlobalFree(hGlobal);
                            }
                            else
                            {
                                GlobalUnlock(hGlobal);
                                GlobalFree(hGlobal);
                            }
                        }
                        else
                        {
                            GlobalUnlock(hGlobal);
                            GlobalFree(hGlobal);
                        }
                    }
                    else
                    {
                        GlobalFree(hGlobal);
                        GlobalUnlock(hClipboardText);
                    }
                }
                else
                    GlobalUnlock(hClipboardText);
            }

        if (CloseClipboard() && bValid)
        {
            keybd_event(VK_CONTROL, 0, 0, 0);
            keybd_event('V', 0, 0, 0);
            keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        }
    }
}
