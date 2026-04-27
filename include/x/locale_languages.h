/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifndef LOCALE_MINIMUM
#define LOCALE_MINIMUM 0
#endif

/* Locale Generator */
#define X_LANG(lang, ...) X_LANG_##lang(__VA_ARGS__)

/* Standard Locale */
#define X_LANG_C(...) X(LANG_C, __VA_ARGS__)

/* English - 1.5B */
#if 1500000000 >= LOCALE_MINIMUM
#define X_LANG_EN(...) X(LANG_EN, __VA_ARGS__)
#else
#define X_LANG_EN(...)
#endif

/* Mandarin Chinese - 1.1B */
#if 1100000000 >= LOCALE_MINIMUM
#define X_LANG_ZH(...) X(LANG_ZH, __VA_ARGS__)
#else
#define X_LANG_ZH(...)
#endif

/* Hindi - 600M */
#if 600000000 >= LOCALE_MINIMUM
#define X_LANG_HI(...) X(LANG_HI, __VA_ARGS__)
#else
#define X_LANG_HI(...)
#endif

/* Spanish - 560M */
#if 560000000 >= LOCALE_MINIMUM
#define X_LANG_ES(...) X(LANG_ES, __VA_ARGS__)
#else
#define X_LANG_ES(...)
#endif

/* Malay - 290M */
#if 290000000 >= LOCALE_MINIMUM
#define X_LANG_MS(...) X(LANG_MS, __VA_ARGS__)
#else
#define X_LANG_MS(...)
#endif

/* French - 280M */
#if 280000000 >= LOCALE_MINIMUM
#define X_LANG_FR(...) X(LANG_FR, __VA_ARGS__)
#else
#define X_LANG_FR(...)
#endif

/* Arabic - 270M */
#if 270000000 >= LOCALE_MINIMUM
#define X_LANG_AR(...) X(LANG_AR, __VA_ARGS__)
#else
#define X_LANG_AR(...)
#endif

/* Bengali - 270M */
#if 270000000 >= LOCALE_MINIMUM
#define X_LANG_BN(...) X(LANG_BN, __VA_ARGS__)
#else
#define X_LANG_BN(...)
#endif

/* Portuguese - 260M */
#if 260000000 >= LOCALE_MINIMUM
#define X_LANG_PT(...) X(LANG_PT, __VA_ARGS__)
#else
#define X_LANG_PT(...)
#endif

/* Russian - 260M */
#if 260000000 >= LOCALE_MINIMUM
#define X_LANG_RU(...) X(LANG_RU, __VA_ARGS__)
#else
#define X_LANG_RU(...)
#endif

/* Urdu - 230M */
#if 230000000 >= LOCALE_MINIMUM
#define X_LANG_UR(...) X(LANG_UR, __VA_ARGS__)
#else
#define X_LANG_UR(...)
#endif

/* Indonesian - 200M */
#if 200000000 >= LOCALE_MINIMUM
#define X_LANG_ID(...) X(LANG_ID, __VA_ARGS__)
#else
#define X_LANG_ID(...)
#endif

/* German - 135M */
#if 135000000 >= LOCALE_MINIMUM
#define X_LANG_DE(...) X(LANG_DE, __VA_ARGS__)
#else
#define X_LANG_DE(...)
#endif

/* Punjabi - 125M */
#if 125000000 >= LOCALE_MINIMUM
#define X_LANG_PA(...) X(LANG_PA, __VA_ARGS__)
#else
#define X_LANG_PA(...)
#endif

/* Japanese - 125M */
#if 125000000 >= LOCALE_MINIMUM
#define X_LANG_JA(...) X(LANG_JA, __VA_ARGS__)
#else
#define X_LANG_JA(...)
#endif

/* Swahili - 100M */
#if 100000000 >= LOCALE_MINIMUM
#define X_LANG_SW(...) X(LANG_SW, __VA_ARGS__)
#else
#define X_LANG_SW(...)
#endif

/* Marathi - 95M */
#if 95000000 >= LOCALE_MINIMUM
#define X_LANG_MR(...) X(LANG_MR, __VA_ARGS__)
#else
#define X_LANG_MR(...)
#endif

/* Telugu - 95M */
#if 95000000 >= LOCALE_MINIMUM
#define X_LANG_TE(...) X(LANG_TE, __VA_ARGS__)
#else
#define X_LANG_TE(...)
#endif

/* Turkish - 90M */
#if 90000000 >= LOCALE_MINIMUM
#define X_LANG_TR(...) X(LANG_TR, __VA_ARGS__)
#else
#define X_LANG_TR(...)
#endif

/* Yue Chinese (Cantonese) - 85M */
#if 85000000 >= LOCALE_MINIMUM
#define X_LANG_YUE(...) X(LANG_YUE, __VA_ARGS__)
#else
#define X_LANG_YUE(...)
#endif

/* Tamil - 85M */
#if 85000000 >= LOCALE_MINIMUM
#define X_LANG_TA(...) X(LANG_TA, __VA_ARGS__)
#else
#define X_LANG_TA(...)
#endif

/* Vietnamese - 85M */
#if 85000000 >= LOCALE_MINIMUM
#define X_LANG_VI(...) X(LANG_VI, __VA_ARGS__)
#else
#define X_LANG_VI(...)
#endif

/* Korean - 82M */
#if 82000000 >= LOCALE_MINIMUM
#define X_LANG_KO(...) X(LANG_KO, __VA_ARGS__)
#else
#define X_LANG_KO(...)
#endif

/* Hausa - 80M */
#if 80000000 >= LOCALE_MINIMUM
#define X_LANG_HA(...) X(LANG_HA, __VA_ARGS__)
#else
#define X_LANG_HA(...)
#endif

/* Wu Chinese - 80M */
#if 80000000 >= LOCALE_MINIMUM
#define X_LANG_WUU(...) X(LANG_WUU, __VA_ARGS__)
#else
#define X_LANG_WUU(...)
#endif

/* Farsi - 70M */
#if 70000000 >= LOCALE_MINIMUM
#define X_LANG_FA(...) X(LANG_FA, __VA_ARGS__)
#else
#define X_LANG_FA(...)
#endif

/* Javanese - 68M */
#if 68000000 >= LOCALE_MINIMUM
#define X_LANG_JV(...) X(LANG_JV, __VA_ARGS__)
#else
#define X_LANG_JV(...)
#endif

/* Italian - 68M */
#if 68000000 >= LOCALE_MINIMUM
#define X_LANG_IT(...) X(LANG_IT, __VA_ARGS__)
#else
#define X_LANG_IT(...)
#endif

/* Berber/Tamazight - 30M+ (North Africa) */
#if 30000000 >= LOCALE_MINIMUM
#define X_LANG_BER(...) X(LANG_BER, __VA_ARGS__)
#else
#define X_LANG_BER(...)
#endif

/* Thai - 60M */
#if 60000000 >= LOCALE_MINIMUM
#define X_LANG_TH(...) X(LANG_TH, __VA_ARGS__)
#else
#define X_LANG_TH(...)
#endif

/* Gujarati - 60M */
#if 60000000 >= LOCALE_MINIMUM
#define X_LANG_GU(...) X(LANG_GU, __VA_ARGS__)
#else
#define X_LANG_GU(...)
#endif

/* Kannada - 55M */
#if 55000000 >= LOCALE_MINIMUM
#define X_LANG_KN(...) X(LANG_KN, __VA_ARGS__)
#else
#define X_LANG_KN(...)
#endif

/* Bhojpuri - 52M */
#if 52000000 >= LOCALE_MINIMUM
#define X_LANG_BHO(...) X(LANG_BHO, __VA_ARGS__)
#else
#define X_LANG_BHO(...)
#endif

/* Pashto - 50M */
#if 50000000 >= LOCALE_MINIMUM
#define X_LANG_PS(...) X(LANG_PS, __VA_ARGS__)
#else
#define X_LANG_PS(...)
#endif

/* Min Nan Chinese - 49M */
#if 49000000 >= LOCALE_MINIMUM
#define X_LANG_NAN(...) X(LANG_NAN, __VA_ARGS__)
#else
#define X_LANG_NAN(...)
#endif

/* Hakka Chinese - 48M */
#if 48000000 >= LOCALE_MINIMUM
#define X_LANG_HAK(...) X(LANG_HAK, __VA_ARGS__)
#else
#define X_LANG_HAK(...)
#endif

/* Cebuano (Visayan) - 25M */
#if 25000000 >= LOCALE_MINIMUM
#define X_LANG_CEB(...) X(LANG_CEB, __VA_ARGS__)
#else
#define X_LANG_CEB(...)
#endif

/* Filipino - 45M */
#if 45000000 >= LOCALE_MINIMUM
#define X_LANG_FIL(...) X(LANG_FIL, __VA_ARGS__)
#else
#define X_LANG_FIL(...)
#endif

/* Tagalog - 45M */
#if 45000000 >= LOCALE_MINIMUM
#define X_LANG_TL(...) X(LANG_TL, __VA_ARGS__)
#else
#define X_LANG_TL(...)
#endif

/* Yoruba - 45M */
#if 45000000 >= LOCALE_MINIMUM
#define X_LANG_YO(...) X(LANG_YO, __VA_ARGS__)
#else
#define X_LANG_YO(...)
#endif

/* Polish - 45M */
#if 45000000 >= LOCALE_MINIMUM
#define X_LANG_PL(...) X(LANG_PL, __VA_ARGS__)
#else
#define X_LANG_PL(...)
#endif

/* Sundanese - 42M */
#if 42000000 >= LOCALE_MINIMUM
#define X_LANG_SU(...) X(LANG_SU, __VA_ARGS__)
#else
#define X_LANG_SU(...)
#endif

/* Ukrainian - 40M */
#if 40000000 >= LOCALE_MINIMUM
#define X_LANG_UK(...) X(LANG_UK, __VA_ARGS__)
#else
#define X_LANG_UK(...)
#endif

/* Burmese - 40M */
#if 40000000 >= LOCALE_MINIMUM
#define X_LANG_MY(...) X(LANG_MY, __VA_ARGS__)
#else
#define X_LANG_MY(...)
#endif

/* Malayalam - 38M */
#if 38000000 >= LOCALE_MINIMUM
#define X_LANG_ML(...) X(LANG_ML, __VA_ARGS__)
#else
#define X_LANG_ML(...)
#endif

/* Awadhi - 38M */
#if 38000000 >= LOCALE_MINIMUM
#define X_LANG_AWA(...) X(LANG_AWA, __VA_ARGS__)
#else
#define X_LANG_AWA(...)
#endif

/* Oriya (Odia) - 38M */
#if 38000000 >= LOCALE_MINIMUM
#define X_LANG_OR(...) X(LANG_OR, __VA_ARGS__)
#else
#define X_LANG_OR(...)
#endif

/* Oromo - 37M */
#if 37000000 >= LOCALE_MINIMUM
#define X_LANG_OM(...) X(LANG_OM, __VA_ARGS__)
#else
#define X_LANG_OM(...)
#endif

/* Fulani/Fula - 35M */
#if 35000000 >= LOCALE_MINIMUM
#define X_LANG_FF(...) X(LANG_FF, __VA_ARGS__)
#else
#define X_LANG_FF(...)
#endif

/* Amharic - 35M */
#if 35000000 >= LOCALE_MINIMUM
#define X_LANG_AM(...) X(LANG_AM, __VA_ARGS__)
#else
#define X_LANG_AM(...)
#endif

/* Uzbek - 34M */
#if 34000000 >= LOCALE_MINIMUM
#define X_LANG_UZ(...) X(LANG_UZ, __VA_ARGS__)
#else
#define X_LANG_UZ(...)
#endif

/* Maithili - 34M */
#if 34000000 >= LOCALE_MINIMUM
#define X_LANG_MAI(...) X(LANG_MAI, __VA_ARGS__)
#else
#define X_LANG_MAI(...)
#endif

/* Sindhi - 32M */
#if 32000000 >= LOCALE_MINIMUM
#define X_LANG_SD(...) X(LANG_SD, __VA_ARGS__)
#else
#define X_LANG_SD(...)
#endif

/* Igbo - 31M */
#if 31000000 >= LOCALE_MINIMUM
#define X_LANG_IG(...) X(LANG_IG, __VA_ARGS__)
#else
#define X_LANG_IG(...)
#endif

/* Kurdish - 30M */
#if 30000000 >= LOCALE_MINIMUM
#define X_LANG_KU(...) X(LANG_KU, __VA_ARGS__)
#else
#define X_LANG_KU(...)
#endif

/* Romanian - 26M */
#if 26000000 >= LOCALE_MINIMUM
#define X_LANG_RO(...) X(LANG_RO, __VA_ARGS__)
#else
#define X_LANG_RO(...)
#endif

/* Saraiki - 26M */
#if 26000000 >= LOCALE_MINIMUM
#define X_LANG_SKR(...) X(LANG_SKR, __VA_ARGS__)
#else
#define X_LANG_SKR(...)
#endif

/* Malagasy - 25M */
#if 25000000 >= LOCALE_MINIMUM
#define X_LANG_MG(...) X(LANG_MG, __VA_ARGS__)
#else
#define X_LANG_MG(...)
#endif

/* Dutch - 25M */
#if 25000000 >= LOCALE_MINIMUM
#define X_LANG_NL(...) X(LANG_NL, __VA_ARGS__)
#else
#define X_LANG_NL(...)
#endif

/* Azerbaijani - 24M */
#if 24000000 >= LOCALE_MINIMUM
#define X_LANG_AZ(...) X(LANG_AZ, __VA_ARGS__)
#else
#define X_LANG_AZ(...)
#endif

/* Somali - 22M */
#if 22000000 >= LOCALE_MINIMUM
#define X_LANG_SO(...) X(LANG_SO, __VA_ARGS__)
#else
#define X_LANG_SO(...)
#endif

/* Akan - 20M */
#if 20000000 >= LOCALE_MINIMUM
#define X_LANG_AK(...) X(LANG_AK, __VA_ARGS__)
#else
#define X_LANG_AK(...)
#endif

/* Zhuang - 18M */
#if 18000000 >= LOCALE_MINIMUM
#define X_LANG_ZA(...) X(LANG_ZA, __VA_ARGS__)
#else
#define X_LANG_ZA(...)
#endif

/* Chhattisgarhi - 18M */
#if 18000000 >= LOCALE_MINIMUM
#define X_LANG_HNE(...) X(LANG_HNE, __VA_ARGS__)
#else
#define X_LANG_HNE(...)
#endif

/* Sinhala - 17M */
#if 17000000 >= LOCALE_MINIMUM
#define X_LANG_SI(...) X(LANG_SI, __VA_ARGS__)
#else
#define X_LANG_SI(...)
#endif

/* Nepali - 16M */
#if 16000000 >= LOCALE_MINIMUM
#define X_LANG_NE(...) X(LANG_NE, __VA_ARGS__)
#else
#define X_LANG_NE(...)
#endif

/* Khmer - 16M */
#if 16000000 >= LOCALE_MINIMUM
#define X_LANG_KM(...) X(LANG_KM, __VA_ARGS__)
#else
#define X_LANG_KM(...)
#endif

/* Assamese - 15M */
#if 15000000 >= LOCALE_MINIMUM
#define X_LANG_AS(...) X(LANG_AS, __VA_ARGS__)
#else
#define X_LANG_AS(...)
#endif

/* Lingala - 15M */
#if 15000000 >= LOCALE_MINIMUM
#define X_LANG_LN(...) X(LANG_LN, __VA_ARGS__)
#else
#define X_LANG_LN(...)
#endif

/* Swedish - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_SV(...) X(LANG_SV, __VA_ARGS__)
#else
#define X_LANG_SV(...)
#endif

/* Luganda - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_LG(...) X(LANG_LG, __VA_ARGS__)
#else
#define X_LANG_LG(...)
#endif

/* Madurese - 14M */
#if 14000000 >= LOCALE_MINIMUM
#define X_LANG_MAD(...) X(LANG_MAD, __VA_ARGS__)
#else
#define X_LANG_MAD(...)
#endif

/* Greek - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_EL(...) X(LANG_EL, __VA_ARGS__)
#else
#define X_LANG_EL(...)
#endif

/* Deccan - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_DCC(...) X(LANG_DCC, __VA_ARGS__)
#else
#define X_LANG_DCC(...)
#endif

/* Chittagonian - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_CTG(...) X(LANG_CTG, __VA_ARGS__)
#else
#define X_LANG_CTG(...)
#endif

/* Marwari - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_MWR(...) X(LANG_MWR, __VA_ARGS__)
#else
#define X_LANG_MWR(...)
#endif

/* Magahi - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_MAG(...) X(LANG_MAG, __VA_ARGS__)
#else
#define X_LANG_MAG(...)
#endif

/* Haryanvi - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_BGC(...) X(LANG_BGC, __VA_ARGS__)
#else
#define X_LANG_BGC(...)
#endif

/* Hungarian - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_HU(...) X(LANG_HU, __VA_ARGS__)
#else
#define X_LANG_HU(...)
#endif

/* Kazakh - 13M */
#if 13000000 >= LOCALE_MINIMUM
#define X_LANG_KK(...) X(LANG_KK, __VA_ARGS__)
#else
#define X_LANG_KK(...)
#endif

/* Zulu - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_ZU(...) X(LANG_ZU, __VA_ARGS__)
#else
#define X_LANG_ZU(...)
#endif

/* Serbian - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_SR(...) X(LANG_SR, __VA_ARGS__)
#else
#define X_LANG_SR(...)
#endif

/* Kinyarwanda - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_RW(...) X(LANG_RW, __VA_ARGS__)
#else
#define X_LANG_RW(...)
#endif

/* Haitian Creole - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_HT(...) X(LANG_HT, __VA_ARGS__)
#else
#define X_LANG_HT(...)
#endif

/* Chewa/Nyanja - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_NY(...) X(LANG_NY, __VA_ARGS__)
#else
#define X_LANG_NY(...)
#endif

/* Wolof - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_WO(...) X(LANG_WO, __VA_ARGS__)
#else
#define X_LANG_WO(...)
#endif

/* Turkmen - 12M */
#if 12000000 >= LOCALE_MINIMUM
#define X_LANG_TK(...) X(LANG_TK, __VA_ARGS__)
#else
#define X_LANG_TK(...)
#endif

/* Sylheti - 11M */
#if 11000000 >= LOCALE_MINIMUM
#define X_LANG_SYL(...) X(LANG_SYL, __VA_ARGS__)
#else
#define X_LANG_SYL(...)
#endif

/* Dhundhari - 11M */
#if 11000000 >= LOCALE_MINIMUM
#define X_LANG_DHD(...) X(LANG_DHD, __VA_ARGS__)
#else
#define X_LANG_DHD(...)
#endif

/* Shona - 11M */
#if 11000000 >= LOCALE_MINIMUM
#define X_LANG_SN(...) X(LANG_SN, __VA_ARGS__)
#else
#define X_LANG_SN(...)
#endif

/* Twi - 11M */
#if 11000000 >= LOCALE_MINIMUM
#define X_LANG_TW(...) X(LANG_TW, __VA_ARGS__)
#else
#define X_LANG_TW(...)
#endif

/* Catalan - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_CA(...) X(LANG_CA, __VA_ARGS__)
#else
#define X_LANG_CA(...)
#endif

/* Czech - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_CS(...) X(LANG_CS, __VA_ARGS__)
#else
#define X_LANG_CS(...)
#endif

/* Min Bei Chinese - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_MNP(...) X(LANG_MNP, __VA_ARGS__)
#else
#define X_LANG_MNP(...)
#endif

/* Min Dong Chinese - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_CDO(...) X(LANG_CDO, __VA_ARGS__)
#else
#define X_LANG_CDO(...)
#endif

/* Uyghur - 10M */
#if 10000000 >= LOCALE_MINIMUM
#define X_LANG_UG(...) X(LANG_UG, __VA_ARGS__)
#else
#define X_LANG_UG(...)
#endif

/* Hebrew - ~9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_HE(...) X(LANG_HE, __VA_ARGS__)
#else
#define X_LANG_HE(...)
#endif

/* Hiligaynon - 9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_HIL(...) X(LANG_HIL, __VA_ARGS__)
#else
#define X_LANG_HIL(...)
#endif

/* Mossi - 9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_MOS(...) X(LANG_MOS, __VA_ARGS__)
#else
#define X_LANG_MOS(...)
#endif

/* Balochi - 9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_BAL(...) X(LANG_BAL, __VA_ARGS__)
#else
#define X_LANG_BAL(...)
#endif

/* Tigrinya - 9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_TI(...) X(LANG_TI, __VA_ARGS__)
#else
#define X_LANG_TI(...)
#endif

/* Quechua - 9M */
#if 9000000 >= LOCALE_MINIMUM
#define X_LANG_QU(...) X(LANG_QU, __VA_ARGS__)
#else
#define X_LANG_QU(...)
#endif

/* Gikuyu/Kikuyu - 8M */
#if 8000000 >= LOCALE_MINIMUM
#define X_LANG_KI(...) X(LANG_KI, __VA_ARGS__)
#else
#define X_LANG_KI(...)
#endif

/* Lao - ~8M */
#if 8000000 >= LOCALE_MINIMUM
#define X_LANG_LO(...) X(LANG_LO, __VA_ARGS__)
#else
#define X_LANG_LO(...)
#endif

/* Xhosa - 8M */
#if 8000000 >= LOCALE_MINIMUM
#define X_LANG_XH(...) X(LANG_XH, __VA_ARGS__)
#else
#define X_LANG_XH(...)
#endif

/* Konkani - 8M */
#if 8000000 >= LOCALE_MINIMUM
#define X_LANG_KOK(...) X(LANG_KOK, __VA_ARGS__)
#else
#define X_LANG_KOK(...)
#endif

/* Santali - 7.4M */
#if 7400000 >= LOCALE_MINIMUM
#define X_LANG_SAT(...) X(LANG_SAT, __VA_ARGS__)
#else
#define X_LANG_SAT(...)
#endif

/* Afrikaans - 7M */
#if 7000000 >= LOCALE_MINIMUM
#define X_LANG_AF(...) X(LANG_AF, __VA_ARGS__)
#else
#define X_LANG_AF(...)
#endif

/* Kashmiri - 7M */
#if 7000000 >= LOCALE_MINIMUM
#define X_LANG_KS(...) X(LANG_KS, __VA_ARGS__)
#else
#define X_LANG_KS(...)
#endif

/* Kabyle - 7M */
#if 7000000 >= LOCALE_MINIMUM
#define X_LANG_KAB(...) X(LANG_KAB, __VA_ARGS__)
#else
#define X_LANG_KAB(...)
#endif

/* Guarani - 6.5M */
#if 6500000 >= LOCALE_MINIMUM
#define X_LANG_GN(...) X(LANG_GN, __VA_ARGS__)
#else
#define X_LANG_GN(...)
#endif

/* Danish - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_DA(...) X(LANG_DA, __VA_ARGS__)
#else
#define X_LANG_DA(...)
#endif

/* Bulgarian - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_BG(...) X(LANG_BG, __VA_ARGS__)
#else
#define X_LANG_BG(...)
#endif

/* Armenian - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_HY(...) X(LANG_HY, __VA_ARGS__)
#else
#define X_LANG_HY(...)
#endif

/* Kongo - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_KON(...) X(LANG_KON, __VA_ARGS__)
#else
#define X_LANG_KON(...)
#endif

/* Mongolian - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_MN(...) X(LANG_MN, __VA_ARGS__)
#else
#define X_LANG_MN(...)
#endif

/* Tibetan - 6M */
#if 6000000 >= LOCALE_MINIMUM
#define X_LANG_BO(...) X(LANG_BO, __VA_ARGS__)
#else
#define X_LANG_BO(...)
#endif

/* Finnish - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_FI(...) X(LANG_FI, __VA_ARGS__)
#else
#define X_LANG_FI(...)
#endif

/* Tatar - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_TT(...) X(LANG_TT, __VA_ARGS__)
#else
#define X_LANG_TT(...)
#endif

/* Belarusian - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_BE(...) X(LANG_BE, __VA_ARGS__)
#else
#define X_LANG_BE(...)
#endif

/* Fante - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_FAT(...) X(LANG_FAT, __VA_ARGS__)
#else
#define X_LANG_FAT(...)
#endif

/* Luo - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_LUO(...) X(LANG_LUO, __VA_ARGS__)
#else
#define X_LANG_LUO(...)
#endif

/* Slovak - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_SK(...) X(LANG_SK, __VA_ARGS__)
#else
#define X_LANG_SK(...)
#endif

/* Croatian - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_HR(...) X(LANG_HR, __VA_ARGS__)
#else
#define X_LANG_HR(...)
#endif

/* Albanian - 5M */
#if 5000000 >= LOCALE_MINIMUM
#define X_LANG_SQ(...) X(LANG_SQ, __VA_ARGS__)
#else
#define X_LANG_SQ(...)
#endif

/* Norwegian Bokmål - 4.5M */
#if 4500000 >= LOCALE_MINIMUM
#define X_LANG_NB(...) X(LANG_NB, __VA_ARGS__)
#else
#define X_LANG_NB(...)
#endif

/* Georgian - 4M */
#if 4000000 >= LOCALE_MINIMUM
#define X_LANG_KA(...) X(LANG_KA, __VA_ARGS__)
#else
#define X_LANG_KA(...)
#endif

/* Tok Pisin - 4M */
#if 4000000 >= LOCALE_MINIMUM
#define X_LANG_TPI(...) X(LANG_TPI, __VA_ARGS__)
#else
#define X_LANG_TPI(...)
#endif

/* Bemba - 4M */
#if 4000000 >= LOCALE_MINIMUM
#define X_LANG_BEM(...) X(LANG_BEM, __VA_ARGS__)
#else
#define X_LANG_BEM(...)
#endif

/* Lithuanian - 3M */
#if 3000000 >= LOCALE_MINIMUM
#define X_LANG_LT(...) X(LANG_LT, __VA_ARGS__)
#else
#define X_LANG_LT(...)
#endif

/* Dogri - 2.6M */
#if 2600000 >= LOCALE_MINIMUM
#define X_LANG_DOI(...) X(LANG_DOI, __VA_ARGS__)
#else
#define X_LANG_DOI(...)
#endif

/* Bosnian - 2.5M */
#if 2500000 >= LOCALE_MINIMUM
#define X_LANG_BS(...) X(LANG_BS, __VA_ARGS__)
#else
#define X_LANG_BS(...)
#endif

/* Galician - 2.4M */
#if 2400000 >= LOCALE_MINIMUM
#define X_LANG_GL(...) X(LANG_GL, __VA_ARGS__)
#else
#define X_LANG_GL(...)
#endif

/* Macedonian - 2M */
#if 2000000 >= LOCALE_MINIMUM
#define X_LANG_MK(...) X(LANG_MK, __VA_ARGS__)
#else
#define X_LANG_MK(...)
#endif

/* Aymara - 2M */
#if 2000000 >= LOCALE_MINIMUM
#define X_LANG_AY(...) X(LANG_AY, __VA_ARGS__)
#else
#define X_LANG_AY(...)
#endif

/* Esperanto - 2M */
#if 2000000 >= LOCALE_MINIMUM
#define X_LANG_EO(...) X(LANG_EO, __VA_ARGS__)
#else
#define X_LANG_EO(...)
#endif

/* Manipuri/Meitei - 1.8M */
#if 1800000 >= LOCALE_MINIMUM
#define X_LANG_MNI(...) X(LANG_MNI, __VA_ARGS__)
#else
#define X_LANG_MNI(...)
#endif

/* Irish Gaelic - 1.7M */
#if 1700000 >= LOCALE_MINIMUM
#define X_LANG_GA(...) X(LANG_GA, __VA_ARGS__)
#else
#define X_LANG_GA(...)
#endif

/* Nahuatl - 1.7M */
#if 1700000 >= LOCALE_MINIMUM
#define X_LANG_NAH(...) X(LANG_NAH, __VA_ARGS__)
#else
#define X_LANG_NAH(...)
#endif

/* Chechen - 1.7M */
#if 1700000 >= LOCALE_MINIMUM
#define X_LANG_CE(...) X(LANG_CE, __VA_ARGS__)
#else
#define X_LANG_CE(...)
#endif

/* Latvian - 1.5M */
#if 1500000 >= LOCALE_MINIMUM
#define X_LANG_LV(...) X(LANG_LV, __VA_ARGS__)
#else
#define X_LANG_LV(...)
#endif

/* Bodo - 1.5M */
#if 1500000 >= LOCALE_MINIMUM
#define X_LANG_BRX(...) X(LANG_BRX, __VA_ARGS__)
#else
#define X_LANG_BRX(...)
#endif

/* Basque - 1.2M */
#if 1200000 >= LOCALE_MINIMUM
#define X_LANG_EU(...) X(LANG_EU, __VA_ARGS__)
#else
#define X_LANG_EU(...)
#endif

/* Estonian - 1M */
#if 1000000 >= LOCALE_MINIMUM
#define X_LANG_ET(...) X(LANG_ET, __VA_ARGS__)
#else
#define X_LANG_ET(...)
#endif

/* Sardinian - 1M */
#if 1000000 >= LOCALE_MINIMUM
#define X_LANG_SC(...) X(LANG_SC, __VA_ARGS__)
#else
#define X_LANG_SC(...)
#endif

/* Aramaic - 1M */
#if 1000000 >= LOCALE_MINIMUM
#define X_LANG_ARC(...) X(LANG_ARC, __VA_ARGS__)
#else
#define X_LANG_ARC(...)
#endif

/* Syriac - 1M */
#if 1000000 >= LOCALE_MINIMUM
#define X_LANG_SYC(...) X(LANG_SYC, __VA_ARGS__)
#else
#define X_LANG_SYC(...)
#endif

/* Welsh - 900K */
#if 900000 >= LOCALE_MINIMUM
#define X_LANG_CY(...) X(LANG_CY, __VA_ARGS__)
#else
#define X_LANG_CY(...)
#endif

/* Yiddish - 600K */
#if 600000 >= LOCALE_MINIMUM
#define X_LANG_YI(...) X(LANG_YI, __VA_ARGS__)
#else
#define X_LANG_YI(...)
#endif

/* Maltese - 520K */
#if 520000 >= LOCALE_MINIMUM
#define X_LANG_MT(...) X(LANG_MT, __VA_ARGS__)
#else
#define X_LANG_MT(...)
#endif

/* Samoan - 510K */
#if 510000 >= LOCALE_MINIMUM
#define X_LANG_SM(...) X(LANG_SM, __VA_ARGS__)
#else
#define X_LANG_SM(...)
#endif

/* Occitan - 500K */
#if 500000 >= LOCALE_MINIMUM
#define X_LANG_OC(...) X(LANG_OC, __VA_ARGS__)
#else
#define X_LANG_OC(...)
#endif

/* Norwegian Nynorsk - 500K */
#if 500000 >= LOCALE_MINIMUM
#define X_LANG_NN(...) X(LANG_NN, __VA_ARGS__)
#else
#define X_LANG_NN(...)
#endif

/* Luxembourgish - 400K */
#if 400000 >= LOCALE_MINIMUM
#define X_LANG_LB(...) X(LANG_LB, __VA_ARGS__)
#else
#define X_LANG_LB(...)
#endif

/* Icelandic - 380K */
#if 380000 >= LOCALE_MINIMUM
#define X_LANG_IS(...) X(LANG_IS, __VA_ARGS__)
#else
#define X_LANG_IS(...)
#endif

/* Papiamento - 350K */
#if 350000 >= LOCALE_MINIMUM
#define X_LANG_PAP(...) X(LANG_PAP, __VA_ARGS__)
#else
#define X_LANG_PAP(...)
#endif

/* Fijian - 340K */
#if 340000 >= LOCALE_MINIMUM
#define X_LANG_FJ(...) X(LANG_FJ, __VA_ARGS__)
#else
#define X_LANG_FJ(...)
#endif

/* Naxi - 300K */
#if 300000 >= LOCALE_MINIMUM
#define X_LANG_DTX(...) X(LANG_DTX, __VA_ARGS__)
#else
#define X_LANG_DTX(...)
#endif

/* Dzongkha - 240K */
#if 240000 >= LOCALE_MINIMUM
#define X_LANG_DZ(...) X(LANG_DZ, __VA_ARGS__)
#else
#define X_LANG_DZ(...)
#endif

/* Montenegrin - 230K */
#if 230000 >= LOCALE_MINIMUM
#define X_LANG_CNR(...) X(LANG_CNR, __VA_ARGS__)
#else
#define X_LANG_CNR(...)
#endif

/* Breton - 200K */
#if 200000 >= LOCALE_MINIMUM
#define X_LANG_BR(...) X(LANG_BR, __VA_ARGS__)
#else
#define X_LANG_BR(...)
#endif

/* Ladino - 200K */
#if 200000 >= LOCALE_MINIMUM
#define X_LANG_LAD(...) X(LANG_LAD, __VA_ARGS__)
#else
#define X_LANG_LAD(...)
#endif

/* Tongan - 190K */
#if 190000 >= LOCALE_MINIMUM
#define X_LANG_TO(...) X(LANG_TO, __VA_ARGS__)
#else
#define X_LANG_TO(...)
#endif

/* Navajo - 170K */
#if 170000 >= LOCALE_MINIMUM
#define X_LANG_NV(...) X(LANG_NV, __VA_ARGS__)
#else
#define X_LANG_NV(...)
#endif

/* Corsican - 150K */
#if 150000 >= LOCALE_MINIMUM
#define X_LANG_CO(...) X(LANG_CO, __VA_ARGS__)
#else
#define X_LANG_CO(...)
#endif

/* Maori - 150K */
#if 150000 >= LOCALE_MINIMUM
#define X_LANG_MI(...) X(LANG_MI, __VA_ARGS__)
#else
#define X_LANG_MI(...)
#endif

/* Cree - 117K */
#if 117000 >= LOCALE_MINIMUM
#define X_LANG_CR(...) X(LANG_CR, __VA_ARGS__)
#else
#define X_LANG_CR(...)
#endif

/* Faroese - 72K */
#if 72000 >= LOCALE_MINIMUM
#define X_LANG_FO(...) X(LANG_FO, __VA_ARGS__)
#else
#define X_LANG_FO(...)
#endif

/* Tahitian - 70K */
#if 70000 >= LOCALE_MINIMUM
#define X_LANG_TY(...) X(LANG_TY, __VA_ARGS__)
#else
#define X_LANG_TY(...)
#endif

/* Scottish Gaelic - 60K */
#if 60000 >= LOCALE_MINIMUM
#define X_LANG_GD(...) X(LANG_GD, __VA_ARGS__)
#else
#define X_LANG_GD(...)
#endif

/* Romansh - 60K */
#if 60000 >= LOCALE_MINIMUM
#define X_LANG_RM(...) X(LANG_RM, __VA_ARGS__)
#else
#define X_LANG_RM(...)
#endif

/* Kalaallisut (Greenlandic) - 57K */
#if 57000 >= LOCALE_MINIMUM
#define X_LANG_KL(...) X(LANG_KL, __VA_ARGS__)
#else
#define X_LANG_KL(...)
#endif

/* Klingon - 50K */
#if 50000 >= LOCALE_MINIMUM
#define X_LANG_TLH(...) X(LANG_TLH, __VA_ARGS__)
#else
#define X_LANG_TLH(...)
#endif

/* Inuktitut - 40K */
#if 40000 >= LOCALE_MINIMUM
#define X_LANG_IU(...) X(LANG_IU, __VA_ARGS__)
#else
#define X_LANG_IU(...)
#endif

/* Toki Pona - 30K */
#if 30000 >= LOCALE_MINIMUM
#define X_LANG_TOK(...) X(LANG_TOK, __VA_ARGS__)
#else
#define X_LANG_TOK(...)
#endif

/* Northern Sami - 30K */
#if 30000 >= LOCALE_MINIMUM
#define X_LANG_SE(...) X(LANG_SE, __VA_ARGS__)
#else
#define X_LANG_SE(...)
#endif

/* Seychellois Creole - 100K */
#if 100000 >= LOCALE_MINIMUM
#define X_LANG_SEY(...) X(LANG_SEY, __VA_ARGS__)
#else
#define X_LANG_SEY(...)
#endif

/* Cape Verdean Creole - 1M */
#if 1000000 >= LOCALE_MINIMUM
#define X_LANG_CPV(...) X(LANG_CPV, __VA_ARGS__)
#else
#define X_LANG_CPV(...)
#endif

/* Mauritian Creole - 1.3M */
#if 1300000 >= LOCALE_MINIMUM
#define X_LANG_MRU(...) X(LANG_MRU, __VA_ARGS__)
#else
#define X_LANG_MRU(...)
#endif

/* Sanskrit - 25K */
#if 25000 >= LOCALE_MINIMUM
#define X_LANG_SA(...) X(LANG_SA, __VA_ARGS__)
#else
#define X_LANG_SA(...)
#endif

/* Hawaiian - 24K */
#if 24000 >= LOCALE_MINIMUM
#define X_LANG_HAW(...) X(LANG_HAW, __VA_ARGS__)
#else
#define X_LANG_HAW(...)
#endif

/* Ge'ez (Ethiopian) - 5K */
#if 5000 >= LOCALE_MINIMUM
#define X_LANG_GEZ(...) X(LANG_GEZ, __VA_ARGS__)
#else
#define X_LANG_GEZ(...)
#endif

/* Pali (Buddhist texts) - 1K */
#if 1000 >= LOCALE_MINIMUM
#define X_LANG_PI(...) X(LANG_PI, __VA_ARGS__)
#else
#define X_LANG_PI(...)
#endif

/* Cherokee - 2K */
#if 2000 >= LOCALE_MINIMUM
#define X_LANG_CHR(...) X(LANG_CHR, __VA_ARGS__)
#else
#define X_LANG_CHR(...)
#endif

/* Manx - 2K */
#if 2000 >= LOCALE_MINIMUM
#define X_LANG_GV(...) X(LANG_GV, __VA_ARGS__)
#else
#define X_LANG_GV(...)
#endif

/* Latin - 1K */
#if 1000 >= LOCALE_MINIMUM
#define X_LANG_LA(...) X(LANG_LA, __VA_ARGS__)
#else
#define X_LANG_LA(...)
#endif

/* Literary Chinese - 1K */
#if 1000 >= LOCALE_MINIMUM
#define X_LANG_LZH(...) X(LANG_LZH, __VA_ARGS__)
#else
#define X_LANG_LZH(...)
#endif

/* Avestan (Zoroastrian) - 100 */
#if 100 >= LOCALE_MINIMUM
#define X_LANG_AVE(...) X(LANG_AVE, __VA_ARGS__)
#else
#define X_LANG_AVE(...)
#endif

/* Lojban - 5K */
#if 5000 >= LOCALE_MINIMUM
#define X_LANG_JBO(...) X(LANG_JBO, __VA_ARGS__)
#else
#define X_LANG_JBO(...)
#endif

/* Quenya (Tolkien) - 10K */
#if 10000 >= LOCALE_MINIMUM
#define X_LANG_QYA(...) X(LANG_QYA, __VA_ARGS__)
#else
#define X_LANG_QYA(...)
#endif

/* Sindarin (Tolkien) - 8K */
#if 8000 >= LOCALE_MINIMUM
#define X_LANG_SJN(...) X(LANG_SJN, __VA_ARGS__)
#else
#define X_LANG_SJN(...)
#endif

/* Interlingua - 10K */
#if 10000 >= LOCALE_MINIMUM
#define X_LANG_IA(...) X(LANG_IA, __VA_ARGS__)
#else
#define X_LANG_IA(...)
#endif

/* Ido - 2K */
#if 2000 >= LOCALE_MINIMUM
#define X_LANG_IO(...) X(LANG_IO, __VA_ARGS__)
#else
#define X_LANG_IO(...)
#endif

/* Volapük - 20 */
#if 20 >= LOCALE_MINIMUM
#define X_LANG_VO(...) X(LANG_VO, __VA_ARGS__)
#else
#define X_LANG_VO(...)
#endif

/* Lingua Franca Nova - 200 */
#if 200 >= LOCALE_MINIMUM
#define X_LANG_LFN(...) X(LANG_LFN, __VA_ARGS__)
#else
#define X_LANG_LFN(...)
#endif

/* Cornish - 600 */
#if 600 >= LOCALE_MINIMUM
#define X_LANG_KW(...) X(LANG_KW, __VA_ARGS__)
#else
#define X_LANG_KW(...)
#endif

/* Coptic - 300 */
#if 300 >= LOCALE_MINIMUM
#define X_LANG_COP(...) X(LANG_COP, __VA_ARGS__)
#else
#define X_LANG_COP(...)
#endif

/* Old Church Slavonic - 100 */
#if 100 >= LOCALE_MINIMUM
#define X_LANG_CU(...) X(LANG_CU, __VA_ARGS__)
#else
#define X_LANG_CU(...)
#endif

/* Old Norse - 50 */
#if 50 >= LOCALE_MINIMUM
#define X_LANG_NON(...) X(LANG_NON, __VA_ARGS__)
#else
#define X_LANG_NON(...)
#endif

/* Old English - 50 */
#if 50 >= LOCALE_MINIMUM
#define X_LANG_ANG(...) X(LANG_ANG, __VA_ARGS__)
#else
#define X_LANG_ANG(...)
#endif

/* Include Languages */
#define X_LANGUAGES \
	X_LANG(C,    "C",    0x43,       0x000043  ) /* C */ \
	X_LANG(EN,   "en",   0x454E,     0x454E47  ) /* English */ \
	X_LANG(ZH,   "zh",   0x5A48,     0x5A484F  ) /* Mandarin Chinese */ \
	X_LANG(HI,   "hi",   0x4849,     0x48494E  ) /* Hindi */ \
	X_LANG(ES,   "es",   0x4553,     0x535041  ) /* Spanish */ \
	X_LANG(MS,   "ms",   0x4D53,     0x4D5341  ) /* Malay */ \
	X_LANG(FR,   "fr",   0x4652,     0x465241  ) /* French */ \
	X_LANG(AR,   "ar",   0x4152,     0x415241  ) /* Arabic */ \
	X_LANG(BN,   "bn",   0x424E,     0x42454E  ) /* Bengali */ \
	X_LANG(PT,   "pt",   0x5054,     0x504F52  ) /* Portuguese */ \
	X_LANG(RU,   "ru",   0x5255,     0x525553  ) /* Russian */ \
	X_LANG(UR,   "ur",   0x5552,     0x555244  ) /* Urdu */ \
	X_LANG(ID,   "id",   0x4944,     0x494E44  ) /* Indonesian */ \
	X_LANG(DE,   "de",   0x4445,     0x444555  ) /* German */ \
	X_LANG(PA,   "pa",   0x5041,     0x50414E  ) /* Punjabi */ \
	X_LANG(JA,   "ja",   0x4A41,     0x4A504E  ) /* Japanese */ \
	X_LANG(SW,   "sw",   0x5357,     0x535741  ) /* Swahili */ \
	X_LANG(MR,   "mr",   0x4D52,     0x4D4152  ) /* Marathi */ \
	X_LANG(TE,   "te",   0x5445,     0x54454C  ) /* Telugu */ \
	X_LANG(TR,   "tr",   0x5452,     0x545552  ) /* Turkish */ \
	X_LANG(YUE,  "yue",  0x595545,   0x595545  ) /* Yue / Cantonese */ \
	X_LANG(TA,   "ta",   0x5441,     0x54414D  ) /* Tamil */ \
	X_LANG(VI,   "vi",   0x5649,     0x564945  ) /* Vietnamese */ \
	X_LANG(KO,   "ko",   0x4B4F,     0x4B4F52  ) /* Korean */ \
	X_LANG(HA,   "ha",   0x4841,     0x484155  ) /* Hausa */ \
	X_LANG(WUU,  "wuu",  0x575555,   0x575555  ) /* Wu Chinese */ \
	X_LANG(FA,   "fa",   0x4641,     0x464153  ) /* Persian (Farsi) */ \
	X_LANG(JV,   "jv",   0x4A56,     0x4A4156  ) /* Javanese */ \
	X_LANG(IT,   "it",   0x4954,     0x495441  ) /* Italian */ \
	X_LANG(BER,  "ber",  0x424552,   0x424552  ) /* Berber / Tamazight */ \
	X_LANG(TH,   "th",   0x5448,     0x544841  ) /* Thai */ \
	X_LANG(GU,   "gu",   0x4755,     0x47554A  ) /* Gujarati */ \
	X_LANG(KN,   "kn",   0x4B4E,     0x4B414E  ) /* Kannada */ \
	X_LANG(BHO,  "bho",  0x42484F,   0x42484F  ) /* Bhojpuri */ \
	X_LANG(PS,   "ps",   0x5053,     0x505553  ) /* Pashto */ \
	X_LANG(NAN,  "nan",  0x4E414E,   0x4E414E  ) /* Min Nan Chinese */ \
	X_LANG(HAK,  "hak",  0x48414B,   0x48414B  ) /* Hakka Chinese */ \
	X_LANG(CEB,  "ceb",  0x434542,   0x434542  ) /* Cebuano */ \
	X_LANG(FIL,  "fil",  0x46494C,   0x46494C  ) /* Filipino */ \
	X_LANG(TL,   "tl",   0x544C,     0x54474C  ) /* Tagalog */ \
	X_LANG(YO,   "yo",   0x594F,     0x594F52  ) /* Yoruba */ \
	X_LANG(PL,   "pl",   0x504C,     0x504F4C  ) /* Polish */ \
	X_LANG(SU,   "su",   0x5355,     0x53554E  ) /* Sundanese */ \
	X_LANG(UK,   "uk",   0x554B,     0x554B52  ) /* Ukrainian */ \
	X_LANG(MY,   "my",   0x4D59,     0x4D5941  ) /* Burmese / Myanmar */ \
	X_LANG(ML,   "ml",   0x4D4C,     0x4D414C  ) /* Malayalam */ \
	X_LANG(AWA,  "awa",  0x415741,   0x415741  ) /* Awadhi */ \
	X_LANG(OR,   "or",   0x4F52,     0x4F5249  ) /* Oriya*/ \
	X_LANG(OM,   "om",   0x4F4D,     0x4F524D  ) /* Oromo */ \
	X_LANG(FF,   "ff",   0x4646,     0x46554C  ) /* Fulah */ \
	X_LANG(AM,   "am",   0x414D,     0x414D48  ) /* Amharic */ \
	X_LANG(UZ,   "uz",   0x555A,     0x555A42  ) /* Uzbek */ \
	X_LANG(MAI,  "mai",  0x4D4149,   0x4D4149  ) /* Maithili */ \
	X_LANG(SD,   "sd",   0x5344,     0x534E44  ) /* Sindhi */ \
	X_LANG(IG,   "ig",   0x4947,     0x49424F  ) /* Igbo */ \
	X_LANG(KU,   "ku",   0x4B55,     0x4B5552  ) /* Kurdish */ \
	X_LANG(RO,   "ro",   0x524F,     0x524F4E  ) /* Romanian */ \
	X_LANG(SKR,  "skr",  0x534B52,   0x534B52  ) /* Saraiki */ \
	X_LANG(MG,   "mg",   0x4D47,     0x4D4C47  ) /* Malagasy */ \
	X_LANG(NL,   "nl",   0x4E4C,     0x4E4C44  ) /* Dutch */ \
	X_LANG(AZ,   "az",   0x415A,     0x415A45  ) /* Azerbaijani */ \
	X_LANG(SO,   "so",   0x534F,     0x534F4D  ) /* Somali */ \
	X_LANG(AK,   "ak",   0x414B,     0x414B41  ) /* Akan */ \
	X_LANG(ZA,   "za",   0x5A41,     0x5A4841  ) /* Zhuang */ \
	X_LANG(HNE,  "hne",  0x484E45,   0x484E45  ) /* Chhattisgarhi */ \
	X_LANG(SI,   "si",   0x5349,     0x53494E  ) /* Sinhala */ \
	X_LANG(NE,   "ne",   0x4E45,     0x4E4550  ) /* Nepali */ \
	X_LANG(KM,   "km",   0x4B4D,     0x4B484D  ) /* Khmer */ \
	X_LANG(AS,   "as",   0x4153,     0x41534D  ) /* Assamese */ \
	X_LANG(LN,   "ln",   0x4C4E,     0x4C494E  ) /* Lingala */ \
	X_LANG(SV,   "sv",   0x5356,     0x535645  ) /* Swedish */ \
	X_LANG(LG,   "lg",   0x4C47,     0x4C5547  ) /* Luganda */ \
	X_LANG(MAD,  "mad",  0x4D4144,   0x4D4144  ) /* Madurese */ \
	X_LANG(EL,   "el",   0x454C,     0x454C4C  ) /* Greek */ \
	X_LANG(DCC,  "dcc",  0x444343,   0x444343  ) /* Deccan */ \
	X_LANG(CTG,  "ctg",  0x435447,   0x435447  ) /* Chittagonian */ \
	X_LANG(MWR,  "mwr",  0x4D5752,   0x4D5752  ) /* Marwari */ \
	X_LANG(MAG,  "mag",  0x4D4147,   0x4D4147  ) /* Magahi */ \
	X_LANG(BGC,  "bgc",  0x424743,   0x424743  ) /* Haryanvi */ \
	X_LANG(HU,   "hu",   0x4855,     0x48554E  ) /* Hungarian */ \
	X_LANG(KK,   "kk",   0x4B4B,     0x4B415A  ) /* Kazakh */ \
	X_LANG(ZU,   "zu",   0x5A55,     0x5A554C  ) /* Zulu */ \
	X_LANG(SR,   "sr",   0x5352,     0x535250  ) /* Serbian */ \
	X_LANG(RW,   "rw",   0x5257,     0x4B494E  ) /* Kinyarwanda */ \
	X_LANG(HT,   "ht",   0x4854,     0x484154  ) /* Haitian Creole */ \
	X_LANG(NY,   "ny",   0x4E59,     0x4E5941  ) /* Nyanja Chewa */ \
	X_LANG(WO,   "wo",   0x574F,     0x574F4C  ) /* Wolof */ \
	X_LANG(TK,   "tk",   0x544B,     0x54554B  ) /* Turkmen */ \
	X_LANG(SYL,  "syl",  0x53594C,   0x53594C  ) /* Sylheti */ \
	X_LANG(DHD,  "dhd",  0x444844,   0x444844  ) /* Dhundhari */ \
	X_LANG(SN,   "sn",   0x534E,     0x534E41  ) /* Shona */ \
	X_LANG(TW,   "tw",   0x5457,     0x545749  ) /* Twi */ \
	X_LANG(CA,   "ca",   0x4341,     0x434154  ) /* Catalan */ \
	X_LANG(CS,   "cs",   0x4353,     0x434553  ) /* Czech */ \
	X_LANG(MNP,  "mnp",  0x4D4E50,   0x4D4E50  ) /* Min Bei Chinese */ \
	X_LANG(CDO,  "cdo",  0x43444F,   0x43444F  ) /* Min Dong Chinese */ \
	X_LANG(UG,   "ug",   0x5547,     0x554947  ) /* Uyghur */ \
	X_LANG(HE,   "he",   0x4845,     0x484542  ) /* Hebrew */ \
	X_LANG(HIL,  "hil",  0x48494C,   0x48494C  ) /* Hiligaynon */ \
	X_LANG(MOS,  "mos",  0x4D4F53,   0x4D4F53  ) /* Mossi */ \
	X_LANG(BAL,  "bal",  0x42414C,   0x42414C  ) /* Balochi */ \
	X_LANG(TI,   "ti",   0x5449,     0x544952  ) /* Tigrinya */ \
	X_LANG(QU,   "qu",   0x5155,     0x515545  ) /* Quechua */ \
	X_LANG(KI,   "ki",   0x4B49,     0x4B494B  ) /* Kikuyu */ \
	X_LANG(LO,   "lo",   0x4C4F,     0x4C414F  ) /* Lao */ \
	X_LANG(XH,   "xh",   0x5848,     0x58484F  ) /* Xhosa */ \
	X_LANG(KOK,  "kok",  0x4B4F4B,   0x4B4F4B  ) /* Konkani */ \
	X_LANG(SAT,  "sat",  0x534154,   0x534154  ) /* Santali */ \
	X_LANG(AF,   "af",   0x4146,     0x414652  ) /* Afrikaans */ \
	X_LANG(KS,   "ks",   0x4B53,     0x4B4153  ) /* Kashmiri */ \
	X_LANG(KAB,  "kab",  0x4B4142,   0x4B4142  ) /* Kabyle */ \
	X_LANG(GN,   "gn",   0x474E,     0x47524E  ) /* Guarani */ \
	X_LANG(DA,   "da",   0x4441,     0x44414E  ) /* Danish */ \
	X_LANG(BG,   "bg",   0x4247,     0x42474E  ) /* Bulgarian */ \
	X_LANG(HY,   "hy",   0x4859,     0x48594E  ) /* Armenian */ \
	X_LANG(KON,  "kon",  0x4B4F4E,   0x4B4F4E  ) /* Kongo */ \
	X_LANG(MN,   "mn",   0x4D4E,     0x4D4E47  ) /* Mongolian */ \
	X_LANG(BO,   "bo",   0x424F,     0x424F44  ) /* Tibetan */ \
	X_LANG(FI,   "fi",   0x4649,     0x46494E  ) /* Finnish */ \
	X_LANG(TT,   "tt",   0x5454,     0x544154  ) /* Tatar */ \
	X_LANG(BE,   "be",   0x4245,     0x42454C  ) /* Belarusian */ \
	X_LANG(FAT,  "fat",  0x464154,   0x464154  ) /* Fante */ \
	X_LANG(LUO,  "luo",  0x4C554F,   0x4C554F  ) /* Luo */ \
	X_LANG(SK,   "sk",   0x534B,     0x534C4F  ) /* Slovak */ \
	X_LANG(HR,   "hr",   0x4852,     0x485256  ) /* Croatian */ \
	X_LANG(SQ,   "sq",   0x5351,     0x535149  ) /* Albanian */ \
	X_LANG(NB,   "nb",   0x4E42,     0x4E4F42  ) /* Norwegian Bokmål */ \
	X_LANG(KA,   "ka",   0x4B41,     0x4B4152  ) /* Georgian */ \
	X_LANG(TPI,  "tpi",  0x545049,   0x545049  ) /* Tok Pisin */ \
	X_LANG(BEM,  "bem",  0x42454D,   0x42454D  ) /* Bemba */ \
	X_LANG(LT,   "lt",   0x4C54,     0x4C5455  ) /* Lithuanian */ \
	X_LANG(DOI,  "doi",  0x444F49,   0x444F49  ) /* Dogri */ \
	X_LANG(BS,   "bs",   0x4253,     0x424F53  ) /* Bosnian */ \
	X_LANG(GL,   "gl",   0x474C,     0x474C47  ) /* Galician */ \
	X_LANG(MK,   "mk",   0x4D4B,     0x4D4B44  ) /* Macedonian */ \
	X_LANG(AY,   "ay",   0x4159,     0x41594D  ) /* Aymara */ \
	X_LANG(EO,   "eo",   0x454F,     0x45504F  ) /* Esperanto */ \
	X_LANG(MNI,  "mni",  0x4D4E49,   0x4D4E49  ) /* Manipuri */ \
	X_LANG(GA,   "ga",   0x4741,     0x474C45  ) /* Irish Gaelic */ \
	X_LANG(NAH,  "nah",  0x4E4148,   0x4E4148  ) /* Nahuatl */ \
	X_LANG(CE,   "ce",   0x4345,     0x434845  ) /* Chechen */ \
	X_LANG(LV,   "lv",   0x4C56,     0x4C5641  ) /* Latvian */ \
	X_LANG(BRX,  "brx",  0x425258,   0x425258  ) /* Bodo */ \
	X_LANG(EU,   "eu",   0x4555,     0x455553  ) /* Basque */ \
	X_LANG(ET,   "et",   0x4554,     0x45544F  ) /* Estonian */ \
	X_LANG(SC,   "sc",   0x5343,     0x53434E  ) /* Sardinian */ \
	X_LANG(ARC,  "arc",  0x415243,   0x415243  ) /* Aramaic */ \
	X_LANG(SYC,  "syc",  0x535943,   0x535943  ) /* Syriac */ \
	X_LANG(CY,   "cy",   0x4359,     0x43594D  ) /* Welsh */ \
	X_LANG(YI,   "yi",   0x5949,     0x494949  ) /* Yiddish */ \
	X_LANG(MT,   "mt",   0x4D54,     0x4D544C  ) /* Maltese */ \
	X_LANG(SM,   "sm",   0x534D,     0x534D4F  ) /* Samoan */ \
	X_LANG(OC,   "oc",   0x4F43,     0x4F4349  ) /* Occitan */ \
	X_LANG(NN,   "nn",   0x4E4E,     0x4E4E4F  ) /* Norwegian Nynorsk */ \
	X_LANG(LB,   "lb",   0x4C42,     0x4C545A  ) /* Luxembourgish */ \
	X_LANG(IS,   "is",   0x4953,     0x49534C  ) /* Icelandic */ \
	X_LANG(PAP,  "pap",  0x504150,   0x504150  ) /* Papiamento */ \
	X_LANG(FJ,   "fj",   0x464A,     0x46494A  ) /* Fijian */ \
	X_LANG(DTX,  "dtx",  0x445458,   0x445458  ) /* Naxi */ \
	X_LANG(DZ,   "dz",   0x445A,     0x445A4F  ) /* Dzongkha */ \
	X_LANG(CNR,  "cnr",  0x434E52,   0x434E52  ) /* Montenegrin */ \
	X_LANG(BR,   "br",   0x4252,     0x425245  ) /* Breton */ \
	X_LANG(LAD,  "lad",  0x4C4144,   0x4C4144  ) /* Ladino */ \
	X_LANG(TO,   "to",   0x544F,     0x544F4E  ) /* Tongan */ \
	X_LANG(NV,   "nv",   0x4E56,     0x4E4156  ) /* Navajo */ \
	X_LANG(CO,   "co",   0x434F,     0x434F53  ) /* Corsican */ \
	X_LANG(MI,   "mi",   0x4D49,     0x4D5249  ) /* Māori */ \
	X_LANG(CR,   "cr",   0x4352,     0x435245  ) /* Cree */ \
	X_LANG(FO,   "fo",   0x464F,     0x46414F  ) /* Faroese */ \
	X_LANG(TY,   "ty",   0x5459,     0x544148  ) /* Tahitian */ \
	X_LANG(GD,   "gd",   0x4744,     0x474C41  ) /* Scottish Gaelic */ \
	X_LANG(RM,   "rm",   0x524D,     0x524F48  ) /* Romansh */ \
	X_LANG(KL,   "kl",   0x4B4C,     0x4B414C  ) /* Kalaallisut */ \
	X_LANG(TLH,  "tlh",  0x544C48,   0x544C48  ) /* Klingon */ \
	X_LANG(IU,   "iu",   0x4955,     0x494B55  ) /* Inuktitut */ \
	X_LANG(TOK,  "tok",  0x544F4B,   0x544F4B  ) /* Toki Pona */ \
	X_LANG(SE,   "se",   0x5345,     0x534D45  ) /* Northern Sami */ \
	X_LANG(SEY,  "sey",  0x534559,   0x534559  ) /* Seychellois Creole */ \
	X_LANG(CPV,  "cpv",  0x435056,   0x435056  ) /* Cape Verdean Creole */ \
	X_LANG(MRU,  "mru",  0x4D5255,   0x4D5255  ) /* Mauritian Creole */ \
	X_LANG(SA,   "sa",   0x5341,     0x53414E  ) /* Sanskrit */ \
	X_LANG(HAW,  "haw",  0x484157,   0x484157  ) /* Hawaiian */ \
	X_LANG(GEZ,  "gez",  0x47455A,   0x47455A  ) /* Ge'ez */ \
	X_LANG(PI,   "pi",   0x5049,     0x504C49  ) /* Pali */ \
	X_LANG(CHR,  "chr",  0x434852,   0x434852  ) /* Cherokee */ \
	X_LANG(GV,   "gv",   0x4756,     0x474C56  ) /* Manx */ \
	X_LANG(LA,   "la",   0x4C41,     0x4C4154  ) /* Latin */ \
	X_LANG(LZH,  "lzh",  0x4C5A48,   0x4C5A48  ) /* Literary Chinese */ \
	X_LANG(AVE,  "ave",  0x415645,   0x415645  ) /* Avestan */ \
	X_LANG(JBO,  "jbo",  0x4A424F,   0x4A424F  ) /* Lojban */ \
	X_LANG(QYA,  "qya",  0x515941,   0x515941  ) /* Quenya */ \
	X_LANG(SJN,  "sjn",  0x534A4E,   0x534A4E  ) /* Sindarin */ \
	X_LANG(IA,   "ia",   0x4941,     0x494141  ) /* Interlingua */ \
	X_LANG(IO,   "io",   0x494F,     0x49444F  ) /* Ido */ \
	X_LANG(VO,   "vo",   0x564F,     0x564F4C  ) /* Volapük */ \
	X_LANG(LFN,  "lfn",  0x4C464E,   0x4C464E  ) /* Lingua Franca Nova */ \
	X_LANG(KW,   "kw",   0x4B57,     0x434F52  ) /* Cornish */ \
	X_LANG(COP,  "cop",  0x434F50,   0x434F50  ) /* Coptic */ \
	X_LANG(CU,   "cu",   0x4355,     0x434855  ) /* Old Church Slavonic */ \
	X_LANG(NON,  "non",  0x4E4F4E,   0x4E4F4E  ) /* Old Norse */ \
	X_LANG(ANG,  "ang",  0x414E47,   0x414E47  ) /* Old English */
