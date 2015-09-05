#ifndef CUSTOM
#define CUSTOM

#define BEGIN(x) case x: while (true) { switch (*wChar) {
#define SWAP(x, y) case x : *pBuf = y; break;
#define END case L'\0': *pBuf = L'\0'; goto m; default: *pBuf = *wChar;} ++pBuf; ++wChar;}
#define SWAPCHARS \
    \
    BEGIN(L'e') \
    SWAP(L'`',L'ё')SWAP(L'q',L'й')SWAP(L'w',L'ц')SWAP(L'e',L'у')SWAP(L'r',L'к')SWAP(L't',L'е')SWAP(L'y',L'н')SWAP(L'u',L'г') \
    SWAP(L'i',L'ш')SWAP(L'o',L'щ')SWAP(L'p',L'з')SWAP(L'[',L'х')SWAP(L']',L'ъ')SWAP(L'a',L'ф')SWAP(L's',L'ы')SWAP(L'd',L'в') \
    SWAP(L'f',L'а')SWAP(L'g',L'п')SWAP(L'h',L'р')SWAP(L'j',L'о')SWAP(L'k',L'л')SWAP(L'l',L'д')SWAP(L';',L'ж')SWAP(L'\'',L'э') \
    SWAP(L'z',L'я')SWAP(L'x',L'ч')SWAP(L'c',L'с')SWAP(L'v',L'м')SWAP(L'b',L'и')SWAP(L'n',L'т')SWAP(L'm',L'ь')SWAP(L',',L'б') \
    SWAP(L'.',L'/')SWAP(L'/',L'.')SWAP(L'~',L'Ё')SWAP(L'@',L'"')SWAP(L'#',L'№')SWAP(L'$',L';')SWAP(L'^',L':')SWAP(L'&',L'?') \
    SWAP(L'Q',L'Й')SWAP(L'W',L'Ц')SWAP(L'E',L'У')SWAP(L'R',L'К')SWAP(L'T',L'Е')SWAP(L'Y',L'Н')SWAP(L'U',L'Г')SWAP(L'I',L'Ш') \
    SWAP(L'O',L'Щ')SWAP(L'P',L'З')SWAP(L'{',L'Х')SWAP(L'}',L'Ъ')SWAP(L'|',L'/')SWAP(L'A',L'Ф')SWAP(L'S',L'Ы')SWAP(L'D',L'В') \
    SWAP(L'F',L'А')SWAP(L'G',L'П')SWAP(L'H',L'Р')SWAP(L'J',L'О')SWAP(L'K',L'Л')SWAP(L'L',L'Д')SWAP(L':',L'Ж')SWAP(L'"',L'Э') \
    SWAP(L'Z',L'Я')SWAP(L'X',L'Ч')SWAP(L'C',L'С')SWAP(L'V',L'М')SWAP(L'B',L'И')SWAP(L'N',L'Т')SWAP(L'M',L'Ь')SWAP(L'<',L'Б') \
    SWAP(L'>',L'Ю')SWAP(L'?',L',') \
    END \
    \
    BEGIN(L'c') \
    SWAP(L'ё',L'`')SWAP(L'й',L'q')SWAP(L'ц',L'w')SWAP(L'у',L'e')SWAP(L'к',L'r')SWAP(L'е',L't')SWAP(L'н',L'y')SWAP(L'г',L'u') \
    SWAP(L'ш',L'i')SWAP(L'щ',L'o')SWAP(L'з',L'p')SWAP(L'х',L'[')SWAP(L'ъ',L']')SWAP(L'ф',L'a')SWAP(L'ы',L's')SWAP(L'в',L'd') \
    SWAP(L'а',L'f')SWAP(L'п',L'g')SWAP(L'р',L'h')SWAP(L'о',L'j')SWAP(L'л',L'k')SWAP(L'д',L'l')SWAP(L'ж',L';')SWAP(L'э',L'\'') \
    SWAP(L'я',L'z')SWAP(L'ч',L'x')SWAP(L'с',L'c')SWAP(L'м',L'v')SWAP(L'и',L'b')SWAP(L'т',L'n')SWAP(L'ь',L'm')SWAP(L'б',L',') \
    SWAP(L'ю',L'.')SWAP(L'.',L'/')SWAP(L'Ё',L'~')SWAP(L'"',L'@')SWAP(L'№',L'#')SWAP(L';',L'$')SWAP(L':',L'^')SWAP(L'?',L'&') \
    SWAP(L'Й',L'Q')SWAP(L'Ц',L'W')SWAP(L'У',L'E')SWAP(L'К',L'R')SWAP(L'Е',L'T')SWAP(L'Н',L'Y')SWAP(L'Г',L'U')SWAP(L'Ш',L'I') \
    SWAP(L'Щ',L'O')SWAP(L'З',L'P')SWAP(L'Х',L'{')SWAP(L'Ъ',L'}')SWAP(L'/',L'|')SWAP(L'Ф',L'A')SWAP(L'Ы',L'S')SWAP(L'В',L'D') \
    SWAP(L'А',L'F')SWAP(L'П',L'G')SWAP(L'Р',L'H')SWAP(L'О',L'J')SWAP(L'Л',L'K')SWAP(L'Д',L'L')SWAP(L'Ж',L':')SWAP(L'Э',L'"') \
    SWAP(L'Я',L'Z')SWAP(L'Ч',L'X')SWAP(L'С',L'C')SWAP(L'М',L'V')SWAP(L'И',L'B')SWAP(L'Т',L'N')SWAP(L'Ь',L'M')SWAP(L'Б',L'<') \
    SWAP(L'Ю',L'>')SWAP(L',',L'?')SWAP(L'і',L's')SWAP(L'І',L'S')SWAP(L'ї',L']')SWAP(L'Ї',L'}')SWAP(L'є',L'\'')SWAP(L'Є',L'"') \
    SWAP(L'ў',L'o')SWAP(L'Ў',L'O') \
    END \
    \
    BEGIN(L'd') \
    SWAP(L'`',L'ё')SWAP(L'q',L'й')SWAP(L'w',L'ц')SWAP(L'e',L'у')SWAP(L'r',L'к')SWAP(L't',L'е')SWAP(L'y',L'н')SWAP(L'u',L'г') \
    SWAP(L'i',L'ш')SWAP(L'o',L'щ')SWAP(L'p',L'з')SWAP(L'[',L'х')SWAP(L']',L'ъ')SWAP(L'a',L'ф')SWAP(L's',L'ы')SWAP(L'd',L'в') \
    SWAP(L'f',L'а')SWAP(L'g',L'п')SWAP(L'h',L'р')SWAP(L'j',L'о')SWAP(L'k',L'л')SWAP(L'l',L'д')SWAP(L';',L'ж')SWAP(L'\'',L'э') \
    SWAP(L'z',L'я')SWAP(L'x',L'ч')SWAP(L'c',L'с')SWAP(L'v',L'м')SWAP(L'b',L'и')SWAP(L'n',L'т')SWAP(L'm',L'ь')SWAP(L',',L'б') \
    SWAP(L'.',L'/')SWAP(L'~',L'Ё')SWAP(L'@',L'"')SWAP(L'#',L'№')SWAP(L'$',L';')SWAP(L'^',L':')SWAP(L'&',L'?')SWAP(L'Q',L'Й') \
    SWAP(L'W',L'Ц')SWAP(L'E',L'У')SWAP(L'R',L'К')SWAP(L'T',L'Е')SWAP(L'Y',L'Н')SWAP(L'U',L'Г')SWAP(L'I',L'Ш')SWAP(L'O',L'Щ') \
    SWAP(L'P',L'З')SWAP(L'{',L'Х')SWAP(L'}',L'Ъ')SWAP(L'|',L'/')SWAP(L'A',L'Ф')SWAP(L'S',L'Ы')SWAP(L'D',L'В')SWAP(L'F',L'А') \
    SWAP(L'G',L'П')SWAP(L'H',L'Р')SWAP(L'J',L'О')SWAP(L'K',L'Л')SWAP(L'L',L'Д')SWAP(L':',L'Ж')SWAP(L'"',L'Э')SWAP(L'Z',L'Я') \
    SWAP(L'X',L'Ч')SWAP(L'C',L'С')SWAP(L'V',L'М')SWAP(L'B',L'И')SWAP(L'N',L'Т')SWAP(L'M',L'Ь')SWAP(L'<',L'Б')SWAP(L'>',L'Ю') \
    SWAP(L'ё',L'`')SWAP(L'й',L'q')SWAP(L'ц',L'w')SWAP(L'у',L'e')SWAP(L'к',L'r')SWAP(L'е',L't')SWAP(L'н',L'y')SWAP(L'г',L'u') \
    SWAP(L'ш',L'i')SWAP(L'щ',L'o')SWAP(L'з',L'p')SWAP(L'х',L'[')SWAP(L'ъ',L']')SWAP(L'ф',L'a')SWAP(L'ы',L's')SWAP(L'в',L'd') \
    SWAP(L'а',L'f')SWAP(L'п',L'g')SWAP(L'р',L'h')SWAP(L'о',L'j')SWAP(L'л',L'k')SWAP(L'д',L'l')SWAP(L'ж',L';')SWAP(L'э',L'\'') \
    SWAP(L'я',L'z')SWAP(L'ч',L'x')SWAP(L'с',L'c')SWAP(L'м',L'v')SWAP(L'и',L'b')SWAP(L'т',L'n')SWAP(L'ь',L'm')SWAP(L'б',L',') \
    SWAP(L'ю',L'.')SWAP(L'Ё',L'~')SWAP(L'№',L'#')SWAP(L'Й',L'Q')SWAP(L'Ц',L'W')SWAP(L'У',L'E')SWAP(L'К',L'R')SWAP(L'Е',L'T') \
    SWAP(L'Н',L'Y')SWAP(L'Г',L'U')SWAP(L'Ш',L'I')SWAP(L'Щ',L'O')SWAP(L'З',L'P')SWAP(L'Х',L'{')SWAP(L'Ъ',L'}')SWAP(L'Ф',L'A') \
    SWAP(L'Ы',L'S')SWAP(L'В',L'D')SWAP(L'А',L'F')SWAP(L'П',L'G')SWAP(L'Р',L'H')SWAP(L'О',L'J')SWAP(L'Л',L'K')SWAP(L'Д',L'L') \
    SWAP(L'Ж',L':')SWAP(L'Э',L'"')SWAP(L'Я',L'Z')SWAP(L'Ч',L'X')SWAP(L'С',L'C')SWAP(L'М',L'V')SWAP(L'И',L'B')SWAP(L'Т',L'N') \
    SWAP(L'Ь',L'M')SWAP(L'Б',L'<')SWAP(L'Ю',L'>')SWAP(L'і',L's')SWAP(L'І',L'S')SWAP(L'ї',L']')SWAP(L'Ї',L'}')SWAP(L'є',L'\'') \
    SWAP(L'Є',L'"')SWAP(L'ў',L'o')SWAP(L'Ў',L'O') \
    END \
    \
    BEGIN(L'u') \
    SWAP(L'a',L'A')SWAP(L'b',L'B')SWAP(L'c',L'C')SWAP(L'd',L'D')SWAP(L'e',L'E')SWAP(L'f',L'F')SWAP(L'g',L'G')SWAP(L'h',L'H') \
    SWAP(L'i',L'I')SWAP(L'j',L'J')SWAP(L'k',L'K')SWAP(L'l',L'L')SWAP(L'm',L'M')SWAP(L'n',L'N')SWAP(L'o',L'O')SWAP(L'p',L'P') \
    SWAP(L'q',L'Q')SWAP(L'r',L'R')SWAP(L's',L'S')SWAP(L't',L'T')SWAP(L'u',L'U')SWAP(L'v',L'V')SWAP(L'w',L'W')SWAP(L'x',L'X') \
    SWAP(L'y',L'Y')SWAP(L'z',L'Z')SWAP(L'ä',L'Ä')SWAP(L'ö',L'Ö')SWAP(L'ü',L'Ü')SWAP(L'а',L'А')SWAP(L'б',L'Б')SWAP(L'в',L'В') \
    SWAP(L'г',L'Г')SWAP(L'д',L'Д')SWAP(L'е',L'Е')SWAP(L'ё',L'Ё')SWAP(L'ж',L'Ж')SWAP(L'з',L'З')SWAP(L'и',L'И')SWAP(L'й',L'Й') \
    SWAP(L'к',L'К')SWAP(L'л',L'Л')SWAP(L'м',L'М')SWAP(L'н',L'Н')SWAP(L'о',L'О')SWAP(L'п',L'П')SWAP(L'р',L'Р')SWAP(L'с',L'С') \
    SWAP(L'т',L'Т')SWAP(L'у',L'У')SWAP(L'ф',L'Ф')SWAP(L'х',L'Х')SWAP(L'ц',L'Ц')SWAP(L'ч',L'Ч')SWAP(L'ш',L'Ш')SWAP(L'щ',L'Щ') \
    SWAP(L'ъ',L'Ъ')SWAP(L'ы',L'Ы')SWAP(L'ь',L'Ь')SWAP(L'э',L'Э')SWAP(L'ю',L'Ю')SWAP(L'я',L'Я')SWAP(L'ґ',L'Ґ')SWAP(L'і',L'І') \
    SWAP(L'ї',L'Ї')SWAP(L'є',L'Є')SWAP(L'ў',L'Ў') \
    END \
    \
    BEGIN(L'l') \
    SWAP(L'A',L'a')SWAP(L'B',L'b')SWAP(L'C',L'c')SWAP(L'D',L'd')SWAP(L'E',L'e')SWAP(L'F',L'f')SWAP(L'G',L'g')SWAP(L'H',L'h') \
    SWAP(L'I',L'i')SWAP(L'J',L'j')SWAP(L'K',L'k')SWAP(L'L',L'l')SWAP(L'M',L'm')SWAP(L'N',L'n')SWAP(L'O',L'o')SWAP(L'P',L'p') \
    SWAP(L'Q',L'q')SWAP(L'R',L'r')SWAP(L'S',L's')SWAP(L'T',L't')SWAP(L'U',L'u')SWAP(L'V',L'v')SWAP(L'W',L'w')SWAP(L'X',L'x') \
    SWAP(L'Y',L'y')SWAP(L'Z',L'z')SWAP(L'Ä',L'ä')SWAP(L'Ö',L'ö')SWAP(L'Ü',L'ü')SWAP(L'А',L'а')SWAP(L'Б',L'б')SWAP(L'В',L'в') \
    SWAP(L'Г',L'г')SWAP(L'Д',L'д')SWAP(L'Е',L'е')SWAP(L'Ё',L'ё')SWAP(L'Ж',L'ж')SWAP(L'З',L'з')SWAP(L'И',L'и')SWAP(L'Й',L'й') \
    SWAP(L'К',L'к')SWAP(L'Л',L'л')SWAP(L'М',L'м')SWAP(L'Н',L'н')SWAP(L'О',L'о')SWAP(L'П',L'п')SWAP(L'Р',L'р')SWAP(L'С',L'с') \
    SWAP(L'Т',L'т')SWAP(L'У',L'у')SWAP(L'Ф',L'ф')SWAP(L'Х',L'х')SWAP(L'Ц',L'ц')SWAP(L'Ч',L'ч')SWAP(L'Ш',L'ш')SWAP(L'Щ',L'щ') \
    SWAP(L'Ъ',L'ъ')SWAP(L'Ы',L'ы')SWAP(L'Ь',L'ь')SWAP(L'Э',L'э')SWAP(L'Ю',L'ю')SWAP(L'Я',L'я')SWAP(L'Ґ',L'ґ')SWAP(L'І',L'і') \
    SWAP(L'Ї',L'ї')SWAP(L'Є',L'є')SWAP(L'Ў',L'ў') \
    END \
    \
    BEGIN(L'i') \
    SWAP(L'A',L'a')SWAP(L'B',L'b')SWAP(L'C',L'c')SWAP(L'D',L'd')SWAP(L'E',L'e')SWAP(L'F',L'f')SWAP(L'G',L'g')SWAP(L'H',L'h') \
    SWAP(L'I',L'i')SWAP(L'J',L'j')SWAP(L'K',L'k')SWAP(L'L',L'l')SWAP(L'M',L'm')SWAP(L'N',L'n')SWAP(L'O',L'o')SWAP(L'P',L'p') \
    SWAP(L'Q',L'q')SWAP(L'R',L'r')SWAP(L'S',L's')SWAP(L'T',L't')SWAP(L'U',L'u')SWAP(L'V',L'v')SWAP(L'W',L'w')SWAP(L'X',L'x') \
    SWAP(L'Y',L'y')SWAP(L'Z',L'z')SWAP(L'Ä',L'a')SWAP(L'Ö',L'o')SWAP(L'Ü',L'u')SWAP(L'a',L'A')SWAP(L'b',L'B')SWAP(L'c',L'C') \
    SWAP(L'd',L'D')SWAP(L'e',L'E')SWAP(L'f',L'F')SWAP(L'g',L'G')SWAP(L'h',L'H')SWAP(L'i',L'I')SWAP(L'j',L'J')SWAP(L'k',L'K') \
    SWAP(L'l',L'L')SWAP(L'm',L'M')SWAP(L'n',L'N')SWAP(L'o',L'O')SWAP(L'p',L'P')SWAP(L'q',L'Q')SWAP(L'r',L'R')SWAP(L's',L'S') \
    SWAP(L't',L'T')SWAP(L'u',L'U')SWAP(L'v',L'V')SWAP(L'w',L'W')SWAP(L'x',L'X')SWAP(L'y',L'Y')SWAP(L'z',L'Z')SWAP(L'ä',L'Ä') \
    SWAP(L'ö',L'Ö')SWAP(L'ü',L'Ü')SWAP(L'А',L'а')SWAP(L'Б',L'б')SWAP(L'В',L'в')SWAP(L'Г',L'г')SWAP(L'Д',L'д')SWAP(L'Е',L'е') \
    SWAP(L'Ё',L'ё')SWAP(L'Ж',L'ж')SWAP(L'З',L'з')SWAP(L'И',L'и')SWAP(L'Й',L'й')SWAP(L'К',L'к')SWAP(L'Л',L'л')SWAP(L'М',L'м') \
    SWAP(L'Н',L'н')SWAP(L'О',L'о')SWAP(L'П',L'п')SWAP(L'Р',L'р')SWAP(L'С',L'с')SWAP(L'Т',L'т')SWAP(L'У',L'у')SWAP(L'Ф',L'ф') \
    SWAP(L'Х',L'х')SWAP(L'Ц',L'ц')SWAP(L'Ч',L'ч')SWAP(L'Ш',L'ш')SWAP(L'Щ',L'щ')SWAP(L'Ъ',L'ъ')SWAP(L'Ы',L'ы')SWAP(L'Ь',L'ь') \
    SWAP(L'Э',L'э')SWAP(L'Ю',L'ю')SWAP(L'Я',L'я')SWAP(L'Ґ',L'ґ')SWAP(L'І',L'і')SWAP(L'Ї',L'ї')SWAP(L'Є',L'є')SWAP(L'Ў',L'ў') \
    SWAP(L'а',L'А')SWAP(L'б',L'Б')SWAP(L'в',L'В')SWAP(L'г',L'Г')SWAP(L'д',L'Д')SWAP(L'е',L'Е')SWAP(L'ё',L'Ё')SWAP(L'ж',L'Ж') \
    SWAP(L'з',L'З')SWAP(L'и',L'И')SWAP(L'й',L'Й')SWAP(L'к',L'К')SWAP(L'л',L'Л')SWAP(L'м',L'М')SWAP(L'н',L'Н')SWAP(L'о',L'О') \
    SWAP(L'п',L'П')SWAP(L'р',L'Р')SWAP(L'с',L'С')SWAP(L'т',L'Т')SWAP(L'у',L'У')SWAP(L'ф',L'Ф')SWAP(L'х',L'Х')SWAP(L'ц',L'Ц') \
    SWAP(L'ч',L'Ч')SWAP(L'ш',L'Ш')SWAP(L'щ',L'Щ')SWAP(L'ъ',L'Ъ')SWAP(L'ы',L'Ы')SWAP(L'ь',L'Ь')SWAP(L'э',L'Э')SWAP(L'ю',L'Ю') \
    SWAP(L'я',L'Я')SWAP(L'ґ',L'Ґ')SWAP(L'і',L'І')SWAP(L'ї',L'Ї')SWAP(L'є',L'Є')SWAP(L'ў',L'Ў') \
    END \

#endif // CUSTOM
