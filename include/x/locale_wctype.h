/* (c) 2025 FRINKnet & Friends – MIT licence */

/* X(LANG, d, u, l, p, u_count, l_count, p_count) */

/* Include Ctype */
X_LANG(C,    L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0)
X_LANG(EN,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0)  // English
X_LANG(ZH,   L"0123456789", L"一", L"龥", L"。，、？！：；「」『』（）《》", 0, 0, 16) // Chinese
X_LANG(HI,   L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Hindi
X_LANG(ES,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÑ", L"abcdefghijklmnopqrstuvwxyzñ", L"¿¡", 27, 27, 0) // Spanish
X_LANG(MS,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Malay
X_LANG(FR,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÀÂÇÉÈÊËÎÏÔÙÛÜ", L"abcdefghijklmnopqrstuvwxyzàâçéèêëîïôùûü", L"", 34, 34, 0) // French
X_LANG(AR,   L"٠١٢٣٤٥٦٧٨٩", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛؟ـ٪٫٬٭؞۔۝", 32, 9, 11) // Arabic
X_LANG(BN,   L"০১২৩৪৫৬৭৮৯", L"অআইঈউঊএঐওঔকখগঘঙচছজঝঞটঠডঢণতথদধনপফবভমযরলশষসহড়ঢ়য়্র্ল", L"ািীুূেৈোৌংঁঃ", L"।্", 49, 15, 2) // Bengali
X_LANG(PT,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÃÁÂÇÉÍÓÔÕÚ", L"abcdefghijklmnopqrstuvwxyzãáâçéíóôõú", L"", 35, 35, 0) // Portuguese
X_LANG(RU,   L"0123456789", L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ", L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя", L"", 33, 33, 0) // Russian
X_LANG(UR,   L"۰۱۲۳۴۵۶۷۸۹", L"ابپتٹثجچحخدذرزڑژسشصضطظعغفقکگلمنہءیےؤئے", L"ًٍَُِّْٰ", L"،۔؟", 40, 8, 3) // Urdu
X_LANG(ID,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Indonesian
X_LANG(DE,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜß", L"abcdefghijklmnopqrstuvwxyzäöüß", L"", 30, 30, 0) //German
X_LANG(PA,   L"0123456789", L"ਅਆਇਈਉਊਏਐਓਔਕਖਗਘਙਚਛਜਝਞਟਠਡਢਣਤਥਦਧਨਪਫਬਭਮਯਰਲਵਸਹਖ਼ਗ਼ਜ਼ਫ਼ਣ਼ਲ਼", L"ਅਆਇਈਉਊਏਐਓਔਕਖਗਘਙਚਛਜਝਞਟਠਡਢਣਤਥਦਧਨਪਫਬਭਮਯਰਲਵਸਹਖ਼ਗ਼ਜ਼ਫ਼ਣ਼ਲ਼", L"", 45, 45, 0) // Punjabi
X_LANG(JA,   L"0123456789", L"一", L"龯", L"。、・「」『』（）［］【】《》", 0, 0, 15) // Japanese
X_LANG(SW,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Swahili
X_LANG(MR,   L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसहळ्र्ण", L"ािीूृेैोूंँः", L"।॥॰", 45, 11, 3) // Marathi
X_LANG(TE,   L"౦౧౨౩౪౫౬౭౮౯", L"అ", L"హ", L"", 0, 0, 0) // Telugu
X_LANG(TR,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZİI", L"abcdefghijklmnopqrstuvwxyzıi", L"", 28, 28, 0) // Turkish
X_LANG(YUE,  L"〇一二三四五六七八九", L"一", L"龥", L"。，、？！：；「」『』（）《》",  0, 0, 16) // Cantonese
X_LANG(TA,   L"௦௧௨௩௪௫௬௭௮௯", L"அஆஇஈஉஊஎஏஐஒஓஔஃகஙசஞடணதநபமயரலவஸஹளஷஜ",  L"ாிீுூெேைொோௌ்", L"", 31, 11, 0) // Tamil
X_LANG(VI,   L"0123456789", L"AĂÂĐÊÔƠƯÁÀẢÃẠẮẰẲẴẶẤẦẨẪẬẾỀỂỄỆỐỒỔỖỘỚỜỞỠỢỨỪỮỰÝỲỴỸỶÍÌỈĨỊÚÙỦŨỤ", L"aăâđêôơưáàảãạắằẳẵặấầẩẫậếềểễệốồổỗộớờởỡợứừữựýỳỵỹỷíìỉĩịúùủũụ", L"", 109, 109, 0) // Vietnamese
X_LANG(KO,   L"0123456789", L"가", L"힣", L"。，、？！：；「」『』（）《》", 1, 1, 15) // Korean
X_LANG(HA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Hausa
X_LANG(FA,   L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقکگلمنهویپچژگ", L"ًٌٍَُِّْٰ", L"،؛؟ـ٪٫٬٭؞۔۝", 33, 9, 11) // Persian
X_LANG(JV,   L"꧐꧑꧒꧓꧔꧕꧖꧗꧘꧙", L"ꦀꦁꦂꦃꦄꦅꦆꦇꦈꦉꦊꦋꦌꦍꦎꦏꦐꦑꦒꦓꦔꦕꦖꦗꦘꦙꦚꦛꦜꦝꦞꦟꦠꦡꦢꦣꦤꦥꦦꦧꦨꦩꦪꦫꦬꦭꦮꦯꦰꦱꦲ", L"ꦴꦵꦶꦷꦸꦹꦺꦻꦼꦽꦾꦿ꧀", L"꧁꧂꧃꧄꧅꧆꧇꧈꧉꧊꧋꧌꧍꧞꧟", 53, 13, 0) // Javanese
X_LANG(IT,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Italian
X_LANG(BER,  L"0123456789", L"ⴰⴱⴽⴷⴹⴻⴼⴳⵀⵃⵄⵅⵆⵇⵈⵉⵊⵋⵌⵍⵎⵏⵐⵑⵒⵓⵔⵕⵖⵗⵘⵙⵚⵜⵝⵞⵟⵠⵡⵢⵣⵤⵥⵦⵧ⵨⵩⵪⵫⵬⵭⵮ⵯ", L"", L"", 50, 0, 0) // Berber
X_LANG(TH,   L"๐๑๒๓๔๕๖๗๘๙", L"กขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบบปผฝพฟภมยรฤลฦวศษสหฬอฮ", L"ะัาำิีึืฺุู้๊๋์", L"ฯๆ", 47, 15, 2) // Thai
X_LANG(GU,   L"૦૧૨૩૪૫૬૭૮૯", L"અઆઇઈઉઊઋૠએઐઓઔકખગઘઙચછજઝઞટઠડઢણતથદધનપફબભમયરલવશષસહળ", L"ાિીુૂૃૄેૈોૌંઃ", L"", 41, 11, 0) // Gujarati
X_LANG(KN,   L"೦೧೨೩೪೫೬೭೮೯", L"ಅಆಇಈಉಊಋೠಎಏಐಒಓಔಕಖಗಘಙಚಛಜಝಞಟಠಡಢಣತಥದಧನಪಫಬಭಮಯರಲವಶಷಸಹಳ", L"ಾಿೀುೂೃೆೇೈೊೋೌ್", L"", 48, 12, 0) // Kannada
X_LANG(BHO,  L"०१२३४५६७८९", L"अआइईउऊऋॠऌॡएऐओऔकखगघङचछजझञटठडढणतथदधनपफबभमयरलवशषसह", L"ािीुूृॄॢॣेैोौंःँ", L"", 47, 12, 0) // Bhojpuri
X_LANG(PS,   L"۰۱۲۳۴۵۶۷۸۹",  L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤةپچژکگیڅځډړښګڼ", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 40, 9, 8) // Pashto
X_LANG(NAN,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZĀĒĪŌŪÂÊÎÔÛÁÉÍÓÚÀÈÌÒÙ", L"abcdefghijklmnopqrstuvwxyzāēīōūâêîôûáéíóúàèìòù", L"", 46, 46, 0) // Min Nan
X_LANG(HAK,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZĀĒĪŌŪÂÊÎÔÛÁÉÍÓÚÀÈÌÒÙ", L"abcdefghijklmnopqrstuvwxyzāēīōūâêîôûáéíóúàèìòù", L"", 46,  46, 0) // Hakka
X_LANG(CEB,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Cebuano
X_LANG(FIL,  L"0123456789", L"ABCDEFGHIJKLMNÑOPQRSTUVWXYZ", L"abcdefghijklmnñopqrstuvwxyz", L"", 27, 27, 0) // Filipino
X_LANG(TL,   L"0123456789", L"ABCDEFGHIJKLMNÑOPQRSTUVWXYZ", L"abcdefghijklmnñopqrstuvwxyz", L"", 27, 27, 0) // Tagalog
X_LANG(YO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Yoruba
X_LANG(PL,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZĄĆĘŁŃÓŚŹŻ", L"abcdefghijklmnopqrstuvwxyząćęłńóśźż", L"", 34, 34, 0) // Polish
X_LANG(SU,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Sundanese
X_LANG(UK,   L"0123456789", L"АБВГДЕЄЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЮЯІЇҐ", L"абвгдеєжзийклмнопрстуфхцчшщьюяіїґ", L"", 33, 33, 0) // Ukrainian
X_LANG(MY,   L"၀၁၂၃၄၅၆၇၈၉", L"ကခဂဃငစဆဇဈဉညဋဌဍဎဏတထဒဓနပဖဗဘမယရလဝသဟဠအဣဤဥဦဧဪဨဩ", L"ာါိီုူေဲဳဴံ့း်္", L"", 40, 13, 0) // Myanmar
X_LANG(ML,   L"൦൧൨൩൪൫൬൭൮൯", L"അ", L"ൿ", L"", 0, 0, 0) // Malayalam
X_LANG(AWA,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Awadhi
X_LANG(OR,   L"୦୧୨୩୪୫୬୭୮୯", L"ଅ", L"ୱ", L"।॥", 0, 0, 2) // Odia
X_LANG(OM,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Oromo
X_LANG(FF,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Fulani
X_LANG(AM,   L"0123456789", L"ሀ", L"ፚ", L"፡።፣፤፥፦፧፨፠", 0, 0, 9) // Amharic
X_LANG(UZ,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Uzbek
X_LANG(MAI,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Maithili
X_LANG(SD,   L"٠١٢٣٤٥٦٧٨٩",  L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤةپچڄٽٺڊڙکگ", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 40, 9, 8) // Sindhi
X_LANG(IG,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Igbo
X_LANG(KU,   L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ",  32, 9, 8) // Kurdish
X_LANG(RO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÂĂÎȘȚ", L"abcdefghijklmnopqrstuvwxyzâăîșț", L"", 31, 31, 0) // Romanian
X_LANG(SKR,  L"٠١٢٣٤٥٦٧٨٩", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 32, 9, 8) // Saraiki
X_LANG(MG,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Malagasy
X_LANG(NL,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Dutch
X_LANG(AZ,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÇĞİÖŞÜƏ", L"abcdefghijklmnopqrstuvwxyzçğıöşüə", L"", 32, 32, 0) // Azerbaijani
X_LANG(SO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Somali
X_LANG(ZA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Zhuang
X_LANG(HNE,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Chhattisgarhi
X_LANG(SI,   L"෦෧෨෩෪෫෬෭෮෯",  L"අ", L"ෆ", L"", 0, 0, 0) // Sinhala
X_LANG(NE,   L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Nepali
X_LANG(KM,   L"០១២៣៤៥៦៧៨៩", L"ក", L"អ", L"", 0, 0, 0) // Khmer
X_LANG(AS,   L"0123456789", L"অ", L"হ", L"", 0, 0, 0) // Assamese
X_LANG(LN,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Lingala
X_LANG(LG,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Luganda
X_LANG(MAD,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Madurese
X_LANG(EL,   L"0123456789", L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ", L"αβγδεζηθικλμνξοπρστυφχψω", L"", 24, 24, 0) // Greek
X_LANG(DCC,  L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 32, 9, 8) // Deccan
X_LANG(CTG,  L"0123456789", L"অ", L"হ", L"", 0, 0, 0) // Chittagonian
X_LANG(MWR,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Marwari
X_LANG(MAG,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Magahi
X_LANG(BGC,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Haryanvi
X_LANG(HU,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÁÉÍÓÖŐÚÜŰ", L"abcdefghijklmnopqrstuvwxyzáéíóöőúüű", L"", 35, 35, 0) // Hungarian
X_LANG(KK,   L"0123456789", L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯҒҚҮҰІ", L"абвгдеёжзийклмнопрстуфхцчшщъыьэюяғқүұі", L"", 38, 38, 0) // Kazakh
X_LANG(ZU,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Zulu
X_LANG(SR,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZЂЈЉЊЋЏ", L"abcdefghijklmnopqrstuvwxyzђјљњћџ", L"", 32, 32, 0) // Serbian
X_LANG(RW,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Kinyarwanda
X_LANG(HT,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Haitian Creole
X_LANG(NY,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Nyanja
X_LANG(WO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Wolof
X_LANG(TK,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Turkmen
X_LANG(SYL,  L"0123456789", L"অ", L"হ", L"", 0, 0, 0) // Sylheti
X_LANG(DHD,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Dhundhari
X_LANG(SN,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Shona
X_LANG(AK,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Akan
X_LANG(CA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÇÑ", L"abcdefghijklmnopqrstuvwxyzçñ", L"", 28, 28, 0) // Catalan
X_LANG(CS,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZČĎĚŇŘŠŤÚŮÝŽ", L"abcdefghijklmnopqrstuvwxyzčďěňřšťúůýž", L"", 37, 37, 0) // Czech
X_LANG(MNP,  L"0123456789", L"一", L"龥", L"。，、？！：；「」『』（）《》", 0, 0, 16) // Northern Min
X_LANG(CDO,  L"0123456789", L"一", L"龥", L"。，、？！：；「」『』（）《》", 0, 0, 16) // Min Dong
X_LANG(UG,   L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة",  L"ًٌٍَُِّْٰ",  L"،؛.:/؟!ـ", 32, 9, 8) // Uyghur
X_LANG(HE,   L"0123456789", L"אבגדהוזחטיכךלמםנןסעפףצץקרשת", L"ְֱֲֳִֵֶַָֹֻּֿׁׂ", L"֑֖֛֢֣֤֥֦֧֪֚֭֮֒֓֔֕֗֘֙֜֝֞֟֠֡֨֩֫֬֯׀׃״׳", 27, 15, 35) // Hebrew
X_LANG(HIL,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Hiligaynon
X_LANG(MOS,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Mossi
X_LANG(BAL,  L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 32, 9, 8) // Balochi
X_LANG(TI,   L"0123456789", L"ሀ", L"ፚ", L"፡።፣፤፥፦፧፨፠", 0, 0, 9) // Tigrinya
X_LANG(QU,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Quechua
X_LANG(KI,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz",  L"", 26, 26, 0) // Kikuyu
X_LANG(XH,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Xhosa
X_LANG(KOK,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीूृेैोौंँः", L"।॥॰", 43, 14, 3) // Konkani
X_LANG(KON,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Kikongo
X_LANG(SAT,  L"0123456789", L"ᱚ", L"᱿", L"", 0, 0, 0) // Santali
X_LANG(AF,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Afrikaans
X_LANG(KS,   L"۰۱۲۳۴۵۶۷۸۹", L"ابتثجحخدذرزسشصضطظعغفقكلمنهويءآإؤة", L"ًٌٍَُِّْٰ", L"،؛.:/؟!ـ", 32, 9, 8) // Kashmiri
X_LANG(GN,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Guarani
X_LANG(BO,   L"༠༡༢༣༤༥༦༧༨༩", L"ༀ", L"༼", L"་།༎༏༐", 0, 0, 0) // Tibetan
X_LANG(KAB,  L"0123456789", L"ⴰⴱⴽⴷⴹⴻⴼⴳⵀⵃⵄⵅⵆⵇⵈⵉⵊⵋⵌⵍⵎⵏⵐⵑⵒⵓⵔⵕⵖⵗⵘⵙⵚⵜⵝⵞⵟⵠⵡⵢⵣⵤⵥⵦⵧ⵨⵩⵪⵫⵬⵭⵮ⵯ", L"", L"", 50, 0, 0) // Kabyle
X_LANG(TT,   L"0123456789", L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯӘҖҢӨҮҺ", L"абвгдеёжзийклмнопрстуфхцчшщъыьэюяәҗңөүһ", L"", 39, 39, 0) // Tatar
X_LANG(BE,   L"0123456789", L"АБВГДЕЁЖЗІЙКЛМНОПРСТУЎФХЦЧШЫЬЭЮЯ", L"абвгдеёжзійклмнопрстуўфхцчшыьэюя", L"", 32, 32, 0) // Belarusian
X_LANG(LUO,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Luo
X_LANG(CE,   L"0123456789", L"АБВГВЕДЗЕИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ", L"абвгведзэйійклмнопрстуфхцчшщъыьэюя", L"", 33, 33, 0) // Chechen
X_LANG(DTX,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Dutton World
X_LANG(NB,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÆØÅ", L"abcdefghijklmnopqrstuvwxyzæøå", L"", 29, 29, 0) // Norwegian Bokmål
X_LANG(TPI,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Tok Pisin
X_LANG(BEM,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Bemba
X_LANG(DOI,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघचछजझटठडढतथदधनपफबभमयरलवशसह", L"ािीुूृेैोौंँः", L"।॥", 43, 11, 2) // Dogri
X_LANG(BS,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZČĆĐŠŽ", L"abcdefghijklmnopqrstuvwxyzčćđšž", L"", 30, 30, 0) // Bosnian
X_LANG(GL,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Galician
X_LANG(MK,   L"0123456789", L"АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ", L"абвгдѓежѕијклљмнњопрстќуфхцчџш", L"", 31, 31, 0) // Macedonian
X_LANG(AY,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Aymara
X_LANG(EO,   L"0123456789", L"ABCĈDEFGĜHĤIĴJKLMNOPRSŜTUŬVZ", L"abcĉdefgĝhĥiĵjklmnoprsŝtuŭvz", L"", 28, 28, 0) // Esperanto
X_LANG(MNI,  L"꯰꯱꯲꯳꯴꯵꯶꯷꯸꯹", L"ꯀꯁꯂꯃꯄꯅꯆꯇꯈꯉꯊꯋꯌꯍꯎꯏꯐꯑꯒꯓꯔꯕꯖꯗꯘꯙꯚꯛꯜꯝꯞꯟꯠꯡꯢꯣꯤꯥꯦꯧꯨꯩꯪ", L"", L"", 43, 0, 0) // Manipuri
X_LANG(GA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÁÉÍÓÚ", L"abcdefghijklmnopqrstuvwxyzáéíóú", L"", 31, 31, 0) // Irish
X_LANG(NAH,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Nahuatl
X_LANG(BRX,  L"0123456789", L"अआइईउऊऋएऐओऔकखगघङचछजझटठडढणतथदधनपफबभमयरलवशषसह", L"ािीुूृेैोौंँः", L"।॥", 43, 11, 2) // Bodo
X_LANG(EU,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Basque
X_LANG(SC,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Sardinian
X_LANG(ARC,  L"0123456789", L"ܐܒܓܕܗܘܙܚܛܝܟܠܡܢܣܥܦܨܩܪܫܬ", L"ܱܴܷܸܹܻܼܾ݂݄݆ܰܲܳܵܶܺܽܿ݀݁݃݅", L"܀܁܂܃܄܅܆܇܈܉܊܋܌܍܎܏", 22, 23, 16) // Aramaic
X_LANG(CY,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Welsh
X_LANG(YI,   L"0123456789", L"אבגדהוזחטיכךלמםנןסעפףצץקרשת", L"ֱֲֳִֵֶַָֻּׁׂ", L"׀׃״׳", 27, 10, 4) // Yiddish
X_LANG(SM,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Samoan
X_LANG(OC,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Occitan
X_LANG(NN,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÆØÅ", L"abcdefghijklmnopqrstuvwxyzæøå", L"", 29, 29, 0) // Norwegian Nynorsk
X_LANG(LB,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄËÏÖÜ", L"abcdefghijklmnopqrstuvwxyzäëïöü", L"", 31, 31, 0)
X_LANG(FJ,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Fijian
X_LANG(PAP,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Papiamento
X_LANG(DZ,   L"༠༡༢༣༤༥༦༧༨༩", L"ཀཁགགྷངཅཆཇཉཏཐདདྷནཔཕབབྷམཙཚཛཛྷཝཞཟའཡརལཤསཧཨ", L"ཱིེོུ༹", L"་།༔༄༅", 34, 5, 5) // Dzongkha
X_LANG(CNR,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZĆČĐŠŽ", L"abcdefghijklmnopqrstuvwxyzćčđšž", L"", 31, 31, 0)
X_LANG(BR,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Breton
X_LANG(LAD,  L"0123456789", L"אבגדהוזחטיכךלמםנןסעפףצץקרשת", L"ֱֲֳִֵֶַָֹֻּׁׂ", L"׀׃״׳", 27, 11, 4) // Ladino
X_LANG(TO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Tongan
X_LANG(NV,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Navajo
X_LANG(CO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Corsican
X_LANG(MI,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Maori
X_LANG(CR,   L"0123456789", L"ᐊᐁᐃᐅᐆᐋᐌᐎᐑᐕᐖᐗᐘᐙᐚᐛᐜᐝᐞᐟᐠᐡᐢᐣᐤᐥᐦᐧᐨᐩᐪᐫᐬᐭᐮᐯᐰᐱᐲᐳᐴᐵᐶᐷᐸᐹᐺᐻᐼᐽᐾᐿ", L"", L"", 55, 0, 0) // Cree
X_LANG(FO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÁÐÍÓÚÝ", L"abcdefghijklmnopqrstuvwxyzáðíóúý", L"", 30, 30, 0) // Faroese
X_LANG(TY,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Tahitian
X_LANG(GD,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Scottish Gaelic
X_LANG(RM,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Romansh
X_LANG(KL,   L"0123456789", L"ABCDEFGHIJKLMNOPRSUV", L"abcdefghijklmnoprsuv", L"", 18, 18, 0) // Greenlandic
X_LANG(TLH,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Klingon
X_LANG(IU,   L"0123456789", L"ᐊᐁᐃᐅᐆᐋᐌᐎᐑᐕᐖᐗᐘᐙᐚᐛᐜᐝᐞᐟᐠᐡᐢᐣᐤᐥᐦᐧᐨᐩᐪᐫᐬᐭᐮᐯᐰᐱᐲᐳᐴᐵᐶᐷᐸᐹᐺᐻᐼᐽᐾᐿ", L"", L"", 55, 0, 0) // Inuktitut
X_LANG(TOK,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Toki Pona
X_LANG(SE,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZČĐŊŠŽÁ", L"abcdefghijklmnopqrstuvwxyzčđŋšžá", L"", 34, 34, 0) // Northern Sami
X_LANG(SEY,  L"߀߁߂߃߄߅߆߇߈߉", L"ߊߋߌߍߎߏߐߑߒߓߔߕߖߗߘߙߚߛߜߝߞߟߠߡߢߣߤߥߦߧߨߩߪ", L"߲߫߬߭߮߯߰߱߳", L"߸߹", 27, 9, 2) // Koyra Chiini
X_LANG(CPV,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Kriol
X_LANG(MRU,  L"𖩠𖩡𖩢𖩣𖩤𖩥𖩦𖩧𖩨𖩩", L"𖩀𖩁𖩂𖩃𖩄𖩅𖩆𖩇𖩈𖩉𖩊𖩋𖩌𖩍𖩎𖩏𖩐𖩑𖩒𖩓𖩔𖩕𖩖𖩗𖩘𖩙𖩚𖩛𖩜𖩝𖩞", L"", L"𖩮𖩯", 31, 0, 2) // Moruru
X_LANG(SA,   L"0123456789", L"अआइईउऊऋॠऌॡएऐओऔकखगघङचछजझञटठडढणतथदधनपफबभमयरलवशषसह", L"ािीुूृॄॢॣेैोौंँःँ़्", L"।॥", 48, 14, 2) // Sanskrit
X_LANG(HAW,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Hawaiian
X_LANG(GEZ,  L"0123456789", L"ሀሁሂሃሄህሆሇለሉሊላሌልሎሏሐሑሒሓሔሕሖሗመሙሚማሜምሞሟሠሡሢሣሤሥሦሧረሩሪራሬርሮሯሰሱሲሳሴስሶሷሸሹሺሻሼሽሾሿ", L"ሀሁሂሃሄህሆሇለሉሊላሌልሎሏሐሑሒሓሔሕሖሗመሙሚማሜምሞሟሠሡሢሣሤሥሦሧረሩሪራሬርሮሯሰሱሲሳሴስሶሷሸሹሺሻሼሽሾሿ", L"", 56, 56, 0) // Ge'ez
X_LANG(PI,   L"0123456789", L"अआइईउऊऋॠऌॡएऐओऔकखगघङचछजझञटठडढणतथदधनपफबभमयरलवशषसह", L"ािीुूृॄॢॣेैोौंँःँ़्", L"।॥", 48, 14, 2) // Pali
X_LANG(CHR,  L"0123456789", L"ᎠᎡᎢᎣᎤᎥᎦᎧᎨᎩᎪᎫᎬᎭᎮᎯᎰᎱᎲᎳᎴᎵᎶᎷᎸᎹᎺᎻᎼᎽᎾᎿᏀᏁᏂᏃᏄᏅᏆᏇᏈᏉᏊᏋᏌᏍᏎᏏᏐᏑᏒᏓᏔᏕᏖᏗᏘᏙᏚᏛᏜᏝᏞᏟᏠᏡᏢᏣᏤᏥᏦᏧᏨᏩᏪᏫᏬᏭᏮᏯᏰᏱᏲᏳᏴ", L"ᎠᎡᎢᎣᎤᎥᎦᎧᎨᎩᎪᎫᎬᎭᎮᎯᎰᎱᎲᎳᎴᎵᎶᎷᎸᎹᎺᎻᎼᎽᎾᎿᏀᏁᏂᏃᏄᏅᏆᏇᏈᏉᏊᏋᏌᏍᏎᏏᏐᏑᏒᏓᏔᏕᏖᏗᏘᏙᏚᏛᏜᏝᏞᏟᏠᏡᏢᏣᏤᏥᏦᏧᏨᏩᏪᏫᏬᏭᏮᏯᏰᏱᏲᏳᏴ", L"", 86, 86, 0) // Cherokee
X_LANG(GV,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Manx
X_LANG(LA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Latin
X_LANG(LZH,  L"0123456789", L"一", L"龥", L"。，、？！：；「」『』（）《》", 0, 0, 16) // Literary Chinese
X_LANG(AVE,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Avestan
X_LANG(JBO,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Lojban
X_LANG(QYA,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Quenya
X_LANG(SJN,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Sindarin
X_LANG(IA,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Interlingua
X_LANG(IO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) //  Ido
X_LANG(VO,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Volapük
X_LANG(LFN,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Lingua Franca Nova
X_LANG(KW,   L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz", L"", 26, 26, 0) // Cornish
X_LANG(COP,  L"0123456789", L"ⲀⲂⲄⲆⲈⲊⲌⲎⲐⲒⲔⲖⲘⲚⲜⲞⲠⲢⲤⲦⲨⲪⲬⲮⲰⲲⲴⲶⲸⲺⲼⲾ", L"ⲁⲃⲅⲇⲉⲋⲍⲏⲑⲓⲕⲗⲙⲛⲝⲟⲡⲣⲥⲧⲩⲫⲭⲯⲱⲳⲵⲷⲹⲻⲽⲿ", L"", 32, 32, 0) // Coptic
X_LANG(CU,   L"0123456789", L"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯѢѲѴ", L"абвгдежзийклмнопрстуфхцчшщъыьэюяѣѳѵ", L"", 36, 36, 0) // Church Slavonic
X_LANG(NON,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZÆØÅ", L"abcdefghijklmnopqrstuvwxyzæøå", L"", 29, 29, 0) // Old Norse
X_LANG(ANG,  L"0123456789", L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", L"abcdefghijklmnopqrstuvwxyz",  L"", 26, 26, 0) // Old English
