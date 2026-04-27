/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifndef LOCALE_MINIMUM
#define LOCALE_MINIMUM 0
#endif

#define X_CC(cc, ...) X_CC_##cc(__VA_ARGS__)

/* Standard Locale */
#define X_CC_NONE(...) X(CC_NONE, __VA_ARGS__)

/* India */
#if 1463865525 >= LOCALE_MINIMUM
#define X_CC_IN(...) X(CC_IN, __VA_ARGS__)
#else
#define X_CC_IN(...)
#endif

/* China */
#if 1425178782 >= LOCALE_MINIMUM
#define X_CC_CN(...) X(CC_CN, __VA_ARGS__)
#else
#define X_CC_CN(...)
#endif

/* United States */
#if 345426571 >= LOCALE_MINIMUM
#define X_CC_US(...) X(CC_US, __VA_ARGS__)
#else
#define X_CC_US(...)
#endif

/* Indonesia */
#if 283487931 >= LOCALE_MINIMUM
#define X_CC_ID(...) X(CC_ID, __VA_ARGS__)
#else
#define X_CC_ID(...)
#endif

/* Pakistan */
#if 251269164 >= LOCALE_MINIMUM
#define X_CC_PK(...) X(CC_PK, __VA_ARGS__)
#else
#define X_CC_PK(...)
#endif

/* Nigeria */
#if 236747130 >= LOCALE_MINIMUM
#define X_CC_NG(...) X(CC_NG, __VA_ARGS__)
#else
#define X_CC_NG(...)
#endif

/* Brazil */
#if 211998573 >= LOCALE_MINIMUM
#define X_CC_BR(...) X(CC_BR, __VA_ARGS__)
#else
#define X_CC_BR(...)
#endif

/* Bangladesh */
#if 174701211 >= LOCALE_MINIMUM
#define X_CC_BD(...) X(CC_BD, __VA_ARGS__)
#else
#define X_CC_BD(...)
#endif

/* Russia */
#if 144820423 >= LOCALE_MINIMUM
#define X_CC_RU(...) X(CC_RU, __VA_ARGS__)
#else
#define X_CC_RU(...)
#endif

/* Mexico */
#if 130861007 >= LOCALE_MINIMUM
#define X_CC_MX(...) X(CC_MX, __VA_ARGS__)
#else
#define X_CC_MX(...)
#endif

/* Ethiopia */
#if 129719719 >= LOCALE_MINIMUM
#define X_CC_ET(...) X(CC_ET, __VA_ARGS__)
#else
#define X_CC_ET(...)
#endif

/* Japan */
#if 123201945 >= LOCALE_MINIMUM
#define X_CC_JP(...) X(CC_JP, __VA_ARGS__)
#else
#define X_CC_JP(...)
#endif

/* Philippines */
#if 120861595 >= LOCALE_MINIMUM
#define X_CC_PH(...) X(CC_PH, __VA_ARGS__)
#else
#define X_CC_PH(...)
#endif

/* Egypt */
#if 118365995 >= LOCALE_MINIMUM
#define X_CC_EG(...) X(CC_EG, __VA_ARGS__)
#else
#define X_CC_EG(...)
#endif

/* DR Congo */
#if 112079340 >= LOCALE_MINIMUM
#define X_CC_CD(...) X(CC_CD, __VA_ARGS__)
#else
#define X_CC_CD(...)
#endif

/* Vietnam */
#if 101598527 >= LOCALE_MINIMUM
#define X_CC_VN(...) X(CC_VN, __VA_ARGS__)
#else
#define X_CC_VN(...)
#endif

/* Iran */
#if 92083021 >= LOCALE_MINIMUM
#define X_CC_IR(...) X(CC_IR, __VA_ARGS__)
#else
#define X_CC_IR(...)
#endif

/* Turkey */
#if 87968168 >= LOCALE_MINIMUM
#define X_CC_TR(...) X(CC_TR, __VA_ARGS__)
#else
#define X_CC_TR(...)
#endif

/* Germany */
#if 84119100 >= LOCALE_MINIMUM
#define X_CC_DE(...) X(CC_DE, __VA_ARGS__)
#else
#define X_CC_DE(...)
#endif

/* Thailand */
#if 71801279 >= LOCALE_MINIMUM
#define X_CC_TH(...) X(CC_TH, __VA_ARGS__)
#else
#define X_CC_TH(...)
#endif

/* Tanzania */
#if 69329634 >= LOCALE_MINIMUM
#define X_CC_TZ(...) X(CC_TZ, __VA_ARGS__)
#else
#define X_CC_TZ(...)
#endif

/* United Kingdom */
#if 69138192 >= LOCALE_MINIMUM
#define X_CC_GB(...) X(CC_GB, __VA_ARGS__)
#else
#define X_CC_GB(...)
#endif

/* France */
#if 66548530 >= LOCALE_MINIMUM
#define X_CC_FR(...) X(CC_FR, __VA_ARGS__)
#else
#define X_CC_FR(...)
#endif

/* South Africa */
#if 61186943 >= LOCALE_MINIMUM
#define X_CC_ZA(...) X(CC_ZA, __VA_ARGS__)
#else
#define X_CC_ZA(...)
#endif

/* Italy */
#if 58876764 >= LOCALE_MINIMUM
#define X_CC_IT(...) X(CC_IT, __VA_ARGS__)
#else
#define X_CC_IT(...)
#endif

/* Kenya */
#if 57052004 >= LOCALE_MINIMUM
#define X_CC_KE(...) X(CC_KE, __VA_ARGS__)
#else
#define X_CC_KE(...)
#endif

/* Myanmar */
#if 55770232 >= LOCALE_MINIMUM
#define X_CC_MM(...) X(CC_MM, __VA_ARGS__)
#else
#define X_CC_MM(...)
#endif

/* Colombia */
#if 52695952 >= LOCALE_MINIMUM
#define X_CC_CO(...) X(CC_CO, __VA_ARGS__)
#else
#define X_CC_CO(...)
#endif

/* South Korea */
#if 51717590 >= LOCALE_MINIMUM
#define X_CC_KR(...) X(CC_KR, __VA_ARGS__)
#else
#define X_CC_KR(...)
#endif

/* Sudan */
#if 50467278 >= LOCALE_MINIMUM
#define X_CC_SD(...) X(CC_SD, __VA_ARGS__)
#else
#define X_CC_SD(...)
#endif

/* Uganda */
#if 50015092 >= LOCALE_MINIMUM
#define X_CC_UG(...) X(CC_UG, __VA_ARGS__)
#else
#define X_CC_UG(...)
#endif

/* Spain */
#if 47519628 >= LOCALE_MINIMUM
#define X_CC_ES(...) X(CC_ES, __VA_ARGS__)
#else
#define X_CC_ES(...)
#endif

/* Algeria */
#if 46814308 >= LOCALE_MINIMUM
#define X_CC_DZ(...) X(CC_DZ, __VA_ARGS__)
#else
#define X_CC_DZ(...)
#endif

/* Argentina */
#if 46654581 >= LOCALE_MINIMUM
#define X_CC_AR(...) X(CC_AR, __VA_ARGS__)
#else
#define X_CC_AR(...)
#endif

/* Iraq */
#if 46042015 >= LOCALE_MINIMUM
#define X_CC_IQ(...) X(CC_IQ, __VA_ARGS__)
#else
#define X_CC_IQ(...)
#endif

/* Afghanistan */
#if 43942744 >= LOCALE_MINIMUM
#define X_CC_AF(...) X(CC_AF, __VA_ARGS__)
#else
#define X_CC_AF(...)
#endif

/* Canada */
#if 39623954 >= LOCALE_MINIMUM
#define X_CC_CA(...) X(CC_CA, __VA_ARGS__)
#else
#define X_CC_CA(...)
#endif

/* Poland */
#if 38539201 >= LOCALE_MINIMUM
#define X_CC_PL(...) X(CC_PL, __VA_ARGS__)
#else
#define X_CC_PL(...)
#endif

/* Morocco */
#if 38151373 >= LOCALE_MINIMUM
#define X_CC_MA(...) X(CC_MA, __VA_ARGS__)
#else
#define X_CC_MA(...)
#endif

/* Angola */
#if 37885849 >= LOCALE_MINIMUM
#define X_CC_AO(...) X(CC_AO, __VA_ARGS__)
#else
#define X_CC_AO(...)
#endif

/* Saudi Arabia */
#if 37473929 >= LOCALE_MINIMUM
#define X_CC_SA(...) X(CC_SA, __VA_ARGS__)
#else
#define X_CC_SA(...)
#endif

/* Ukraine */
#if 36744636 >= LOCALE_MINIMUM
#define X_CC_UA(...) X(CC_UA, __VA_ARGS__)
#else
#define X_CC_UA(...)
#endif

/* Uzbekistan */
#if 35163944 >= LOCALE_MINIMUM
#define X_CC_UZ(...) X(CC_UZ, __VA_ARGS__)
#else
#define X_CC_UZ(...)
#endif

/* Yemen */
#if 35158272 >= LOCALE_MINIMUM
#define X_CC_YE(...) X(CC_YE, __VA_ARGS__)
#else
#define X_CC_YE(...)
#endif

/* Peru */
#if 34700764 >= LOCALE_MINIMUM
#define X_CC_PE(...) X(CC_PE, __VA_ARGS__)
#else
#define X_CC_PE(...)
#endif

/* Ghana */
#if 34633583 >= LOCALE_MINIMUM
#define X_CC_GH(...) X(CC_GH, __VA_ARGS__)
#else
#define X_CC_GH(...)
#endif

/* Mozambique */
#if 34631765 >= LOCALE_MINIMUM
#define X_CC_MZ(...) X(CC_MZ, __VA_ARGS__)
#else
#define X_CC_MZ(...)
#endif

/* Malaysia */
#if 34627970 >= LOCALE_MINIMUM
#define X_CC_MY(...) X(CC_MY, __VA_ARGS__)
#else
#define X_CC_MY(...)
#endif

/* Madagascar */
#if 31930539 >= LOCALE_MINIMUM
#define X_CC_MG(...) X(CC_MG, __VA_ARGS__)
#else
#define X_CC_MG(...)
#endif

/* Nepal */
#if 31900369 >= LOCALE_MINIMUM
#define X_CC_NP(...) X(CC_NP, __VA_ARGS__)
#else
#define X_CC_NP(...)
#endif

/* Côte d'Ivoire */
#if 29893667 >= LOCALE_MINIMUM
#define X_CC_CI(...) X(CC_CI, __VA_ARGS__)
#else
#define X_CC_CI(...)
#endif

/* Cameroon */
#if 29321637 >= LOCALE_MINIMUM
#define X_CC_CM(...) X(CC_CM, __VA_ARGS__)
#else
#define X_CC_CM(...)
#endif

/* Venezuela */
#if 28405543 >= LOCALE_MINIMUM
#define X_CC_VE(...) X(CC_VE, __VA_ARGS__)
#else
#define X_CC_VE(...)
#endif

/* Niger */
#if 28175161 >= LOCALE_MINIMUM
#define X_CC_NE(...) X(CC_NE, __VA_ARGS__)
#else
#define X_CC_NE(...)
#endif

/* Australia */
#if 26713205 >= LOCALE_MINIMUM
#define X_CC_AU(...) X(CC_AU, __VA_ARGS__)
#else
#define X_CC_AU(...)
#endif

/* North Korea */
#if 26244582 >= LOCALE_MINIMUM
#define X_CC_KP(...) X(CC_KP, __VA_ARGS__)
#else
#define X_CC_KP(...)
#endif

/* Syria */
#if 24672760 >= LOCALE_MINIMUM
#define X_CC_SY(...) X(CC_SY, __VA_ARGS__)
#else
#define X_CC_SY(...)
#endif

/* Mali */
#if 24478595 >= LOCALE_MINIMUM
#define X_CC_ML(...) X(CC_ML, __VA_ARGS__)
#else
#define X_CC_ML(...)
#endif

/* Burkina Faso */
#if 23832495 >= LOCALE_MINIMUM
#define X_CC_BF(...) X(CC_BF, __VA_ARGS__)
#else
#define X_CC_BF(...)
#endif

/* Taiwan */
#if 23588932 >= LOCALE_MINIMUM
#define X_CC_TW(...) X(CC_TW, __VA_ARGS__)
#else
#define X_CC_TW(...)
#endif

/* Sri Lanka */
#if 21893579 >= LOCALE_MINIMUM
#define X_CC_LK(...) X(CC_LK, __VA_ARGS__)
#else
#define X_CC_LK(...)
#endif

/* Malawi */
#if 21763309 >= LOCALE_MINIMUM
#define X_CC_MW(...) X(CC_MW, __VA_ARGS__)
#else
#define X_CC_MW(...)
#endif

/* Zambia */
#if 21314956 >= LOCALE_MINIMUM
#define X_CC_ZM(...) X(CC_ZM, __VA_ARGS__)
#else
#define X_CC_ZM(...)
#endif

/* Romania */
#if 19892812 >= LOCALE_MINIMUM
#define X_CC_RO(...) X(CC_RO, __VA_ARGS__)
#else
#define X_CC_RO(...)
#endif

/* Kazakhstan */
#if 19871560 >= LOCALE_MINIMUM
#define X_CC_KZ(...) X(CC_KZ, __VA_ARGS__)
#else
#define X_CC_KZ(...)
#endif

/* Chile */
#if 19764771 >= LOCALE_MINIMUM
#define X_CC_CL(...) X(CC_CL, __VA_ARGS__)
#else
#define X_CC_CL(...)
#endif

/* Chad */
#if 19496723 >= LOCALE_MINIMUM
#define X_CC_TD(...) X(CC_TD, __VA_ARGS__)
#else
#define X_CC_TD(...)
#endif

/* Somalia */
#if 19009151 >= LOCALE_MINIMUM
#define X_CC_SO(...) X(CC_SO, __VA_ARGS__)
#else
#define X_CC_SO(...)
#endif

/* Senegal */
#if 18647788 >= LOCALE_MINIMUM
#define X_CC_SN(...) X(CC_SN, __VA_ARGS__)
#else
#define X_CC_SN(...)
#endif

/* Ecuador */
#if 18495660 >= LOCALE_MINIMUM
#define X_CC_EC(...) X(CC_EC, __VA_ARGS__)
#else
#define X_CC_EC(...)
#endif

/* Netherlands */
#if 17618299 >= LOCALE_MINIMUM
#define X_CC_NL(...) X(CC_NL, __VA_ARGS__)
#else
#define X_CC_NL(...)
#endif

/* Cambodia */
#if 17196202 >= LOCALE_MINIMUM
#define X_CC_KH(...) X(CC_KH, __VA_ARGS__)
#else
#define X_CC_KH(...)
#endif

/* Zimbabwe */
#if 16967891 >= LOCALE_MINIMUM
#define X_CC_ZW(...) X(CC_ZW, __VA_ARGS__)
#else
#define X_CC_ZW(...)
#endif

/* Guinea */
#if 14894381 >= LOCALE_MINIMUM
#define X_CC_GN(...) X(CC_GN, __VA_ARGS__)
#else
#define X_CC_GN(...)
#endif

/* Benin */
#if 14462724 >= LOCALE_MINIMUM
#define X_CC_BJ(...) X(CC_BJ, __VA_ARGS__)
#else
#define X_CC_BJ(...)
#endif

/* Rwanda */
#if 14430210 >= LOCALE_MINIMUM
#define X_CC_RW(...) X(CC_RW, __VA_ARGS__)
#else
#define X_CC_RW(...)
#endif

/* Burundi */
#if 13935301 >= LOCALE_MINIMUM
#define X_CC_BI(...) X(CC_BI, __VA_ARGS__)
#else
#define X_CC_BI(...)
#endif

/* Bolivia */
#if 12547028 >= LOCALE_MINIMUM
#define X_CC_BO(...) X(CC_BO, __VA_ARGS__)
#else
#define X_CC_BO(...)
#endif

/* Tunisia */
#if 12525137 >= LOCALE_MINIMUM
#define X_CC_TN(...) X(CC_TN, __VA_ARGS__)
#else
#define X_CC_TN(...)
#endif

/* South Sudan */
#if 11943408 >= LOCALE_MINIMUM
#define X_CC_SS(...) X(CC_SS, __VA_ARGS__)
#else
#define X_CC_SS(...)
#endif

/* Belgium */
#if 11738763 >= LOCALE_MINIMUM
#define X_CC_BE(...) X(CC_BE, __VA_ARGS__)
#else
#define X_CC_BE(...)
#endif

/* Haiti */
#if 11723991 >= LOCALE_MINIMUM
#define X_CC_HT(...) X(CC_HT, __VA_ARGS__)
#else
#define X_CC_HT(...)
#endif

/* Jordan */
#if 11515323 >= LOCALE_MINIMUM
#define X_CC_JO(...) X(CC_JO, __VA_ARGS__)
#else
#define X_CC_JO(...)
#endif

/* Dominican Republic */
#if 11427557 >= LOCALE_MINIMUM
#define X_CC_DO(...) X(CC_DO, __VA_ARGS__)
#else
#define X_CC_DO(...)
#endif

/* Cuba */
#if 11212191 >= LOCALE_MINIMUM
#define X_CC_CU(...) X(CC_CU, __VA_ARGS__)
#else
#define X_CC_CU(...)
#endif

/* Czech Republic */
#if 10900555 >= LOCALE_MINIMUM
#define X_CC_CZ(...) X(CC_CZ, __VA_ARGS__)
#else
#define X_CC_CZ(...)
#endif

/* Honduras */
#if 10825810 >= LOCALE_MINIMUM
#define X_CC_HN(...) X(CC_HN, __VA_ARGS__)
#else
#define X_CC_HN(...)
#endif

/* Sweden */
#if 10606999 >= LOCALE_MINIMUM
#define X_CC_SE(...) X(CC_SE, __VA_ARGS__)
#else
#define X_CC_SE(...)
#endif

/* Tajikistan */
#if 10491013 >= LOCALE_MINIMUM
#define X_CC_TJ(...) X(CC_TJ, __VA_ARGS__)
#else
#define X_CC_TJ(...)
#endif

/* Portugal */
#if 10425292 >= LOCALE_MINIMUM
#define X_CC_PT(...) X(CC_PT, __VA_ARGS__)
#else
#define X_CC_PT(...)
#endif

/* Azerbaijan */
#if 10412651 >= LOCALE_MINIMUM
#define X_CC_AZ(...) X(CC_AZ, __VA_ARGS__)
#else
#define X_CC_AZ(...)
#endif

/* Papua New Guinea */
#if 10329931 >= LOCALE_MINIMUM
#define X_CC_PG(...) X(CC_PG, __VA_ARGS__)
#else
#define X_CC_PG(...)
#endif

/* Hungary */
#if 10156239 >= LOCALE_MINIMUM
#define X_CC_HU(...) X(CC_HU, __VA_ARGS__)
#else
#define X_CC_HU(...)
#endif

/* United Arab Emirates */
#if 10081785 >= LOCALE_MINIMUM
#define X_CC_AE(...) X(CC_AE, __VA_ARGS__)
#else
#define X_CC_AE(...)
#endif

/* Greece */
#if 10047817 >= LOCALE_MINIMUM
#define X_CC_GR(...) X(CC_GR, __VA_ARGS__)
#else
#define X_CC_GR(...)
#endif

/* Togo */
#if 9395440 >= LOCALE_MINIMUM
#define X_CC_TG(...) X(CC_TG, __VA_ARGS__)
#else
#define X_CC_TG(...)
#endif

/* Belarus */
#if 9177654 >= LOCALE_MINIMUM
#define X_CC_BY(...) X(CC_BY, __VA_ARGS__)
#else
#define X_CC_BY(...)
#endif

/* Israel */
#if 9174520 >= LOCALE_MINIMUM
#define X_CC_IL(...) X(CC_IL, __VA_ARGS__)
#else
#define X_CC_IL(...)
#endif

/* Austria */
#if 9120813 >= LOCALE_MINIMUM
#define X_CC_AT(...) X(CC_AT, __VA_ARGS__)
#else
#define X_CC_AT(...)
#endif

/* Switzerland */
#if 8921981 >= LOCALE_MINIMUM
#define X_CC_CH(...) X(CC_CH, __VA_ARGS__)
#else
#define X_CC_CH(...)
#endif

/* Sierra Leone */
#if 8908040 >= LOCALE_MINIMUM
#define X_CC_SL(...) X(CC_SL, __VA_ARGS__)
#else
#define X_CC_SL(...)
#endif

/* Laos */
#if 7769819 >= LOCALE_MINIMUM
#define X_CC_LA(...) X(CC_LA, __VA_ARGS__)
#else
#define X_CC_LA(...)
#endif

/* Hong Kong */
#if 7536100 >= LOCALE_MINIMUM
#define X_CC_HK(...) X(CC_HK, __VA_ARGS__)
#else
#define X_CC_HK(...)
#endif

/* Nicaragua */
#if 7196726 >= LOCALE_MINIMUM
#define X_CC_NI(...) X(CC_NI, __VA_ARGS__)
#else
#define X_CC_NI(...)
#endif

/* Kyrgyzstan */
#if 7186008 >= LOCALE_MINIMUM
#define X_CC_KG(...) X(CC_KG, __VA_ARGS__)
#else
#define X_CC_KG(...)
#endif

/* Paraguay */
#if 6985303 >= LOCALE_MINIMUM
#define X_CC_PY(...) X(CC_PY, __VA_ARGS__)
#else
#define X_CC_PY(...)
#endif

/* Libya */
#if 6888388 >= LOCALE_MINIMUM
#define X_CC_LY(...) X(CC_LY, __VA_ARGS__)
#else
#define X_CC_LY(...)
#endif

/* Bulgaria */
#if 6757689 >= LOCALE_MINIMUM
#define X_CC_BG(...) X(CC_BG, __VA_ARGS__)
#else
#define X_CC_BG(...)
#endif

/* Serbia */
#if 6619630 >= LOCALE_MINIMUM
#define X_CC_RS(...) X(CC_RS, __VA_ARGS__)
#else
#define X_CC_RS(...)
#endif

/* Turkmenistan */
#if 6544037 >= LOCALE_MINIMUM
#define X_CC_TM(...) X(CC_TM, __VA_ARGS__)
#else
#define X_CC_TM(...)
#endif

/* Congo */
#if 6373943 >= LOCALE_MINIMUM
#define X_CC_CG(...) X(CC_CG, __VA_ARGS__)
#else
#define X_CC_CG(...)
#endif

/* El Salvador */
#if 6364943 >= LOCALE_MINIMUM
#define X_CC_SV(...) X(CC_SV, __VA_ARGS__)
#else
#define X_CC_SV(...)
#endif

/* Singapore */
#if 6046118 >= LOCALE_MINIMUM
#define X_CC_SG(...) X(CC_SG, __VA_ARGS__)
#else
#define X_CC_SG(...)
#endif

/* Denmark */
#if 5977412 >= LOCALE_MINIMUM
#define X_CC_DK(...) X(CC_DK, __VA_ARGS__)
#else
#define X_CC_DK(...)
#endif

/* Central African Republic */
#if 5742315 >= LOCALE_MINIMUM
#define X_CC_CF(...) X(CC_CF, __VA_ARGS__)
#else
#define X_CC_CF(...)
#endif

/* Finland */
#if 5617310 >= LOCALE_MINIMUM
#define X_CC_FI(...) X(CC_FI, __VA_ARGS__)
#else
#define X_CC_FI(...)
#endif

/* Norway */
#if 5587523 >= LOCALE_MINIMUM
#define X_CC_NO(...) X(CC_NO, __VA_ARGS__)
#else
#define X_CC_NO(...)
#endif

/* Slovakia */
#if 5506760 >= LOCALE_MINIMUM
#define X_CC_SK(...) X(CC_SK, __VA_ARGS__)
#else
#define X_CC_SK(...)
#endif

/* Palestine */
#if 5495440 >= LOCALE_MINIMUM
#define X_CC_PS(...) X(CC_PS, __VA_ARGS__)
#else
#define X_CC_PS(...)
#endif

/* Liberia */
#if 5463618 >= LOCALE_MINIMUM
#define X_CC_LR(...) X(CC_LR, __VA_ARGS__)
#else
#define X_CC_LR(...)
#endif

/* Lebanon */
#if 5353930 >= LOCALE_MINIMUM
#define X_CC_LB(...) X(CC_LB, __VA_ARGS__)
#else
#define X_CC_LB(...)
#endif

/* Costa Rica */
#if 5256612 >= LOCALE_MINIMUM
#define X_CC_CR(...) X(CC_CR, __VA_ARGS__)
#else
#define X_CC_CR(...)
#endif

/* Ireland */
#if 5255017 >= LOCALE_MINIMUM
#define X_CC_IE(...) X(CC_IE, __VA_ARGS__)
#else
#define X_CC_IE(...)
#endif

/* New Zealand */
#if 5228100 >= LOCALE_MINIMUM
#define X_CC_NZ(...) X(CC_NZ, __VA_ARGS__)
#else
#define X_CC_NZ(...)
#endif

/* Mauritania */
#if 5169391 >= LOCALE_MINIMUM
#define X_CC_MR(...) X(CC_MR, __VA_ARGS__)
#else
#define X_CC_MR(...)
#endif

/* Kuwait */
#if 4934562 >= LOCALE_MINIMUM
#define X_CC_KW(...) X(CC_KW, __VA_ARGS__)
#else
#define X_CC_KW(...)
#endif

/* Oman */
#if 4687059 >= LOCALE_MINIMUM
#define X_CC_OM(...) X(CC_OM, __VA_ARGS__)
#else
#define X_CC_OM(...)
#endif

/* Panama */
#if 4534350 >= LOCALE_MINIMUM
#define X_CC_PA(...) X(CC_PA, __VA_ARGS__)
#else
#define X_CC_PA(...)
#endif

/* Croatia */
#if 3853200 >= LOCALE_MINIMUM
#define X_CC_HR(...) X(CC_HR, __VA_ARGS__)
#else
#define X_CC_HR(...)
#endif

/* Georgia */
#if 3807872 >= LOCALE_MINIMUM
#define X_CC_GE(...) X(CC_GE, __VA_ARGS__)
#else
#define X_CC_GE(...)
#endif

/* Eritrea */
#if 3748902 >= LOCALE_MINIMUM
#define X_CC_ER(...) X(CC_ER, __VA_ARGS__)
#else
#define X_CC_ER(...)
#endif

/* Mongolia */
#if 3504741 >= LOCALE_MINIMUM
#define X_CC_MN(...) X(CC_MN, __VA_ARGS__)
#else
#define X_CC_MN(...)
#endif

/* Uruguay */
#if 3386588 >= LOCALE_MINIMUM
#define X_CC_UY(...) X(CC_UY, __VA_ARGS__)
#else
#define X_CC_UY(...)
#endif

/* Puerto Rico */
#if 3205691 >= LOCALE_MINIMUM
#define X_CC_PR(...) X(CC_PR, __VA_ARGS__)
#else
#define X_CC_PR(...)
#endif

/* Bosnia and Herzegovina */
#if 3164253 >= LOCALE_MINIMUM
#define X_CC_BA(...) X(CC_BA, __VA_ARGS__)
#else
#define X_CC_BA(...)
#endif

/* Qatar */
#if 3053140 >= LOCALE_MINIMUM
#define X_CC_QA(...) X(CC_QA, __VA_ARGS__)
#else
#define X_CC_QA(...)
#endif

/* Moldova */
#if 3034961 >= LOCALE_MINIMUM
#define X_CC_MD(...) X(CC_MD, __VA_ARGS__)
#else
#define X_CC_MD(...)
#endif

/* Armenia */
#if 2974429 >= LOCALE_MINIMUM
#define X_CC_AM(...) X(CC_AM, __VA_ARGS__)
#else
#define X_CC_AM(...)
#endif

/* Lithuania */
#if 2859110 >= LOCALE_MINIMUM
#define X_CC_LT(...) X(CC_LT, __VA_ARGS__)
#else
#define X_CC_LT(...)
#endif

/* Jamaica */
#if 2825544 >= LOCALE_MINIMUM
#define X_CC_JM(...) X(CC_JM, __VA_ARGS__)
#else
#define X_CC_JM(...)
#endif

/* Namibia */
#if 2777232 >= LOCALE_MINIMUM
#define X_CC_NA(...) X(CC_NA, __VA_ARGS__)
#else
#define X_CC_NA(...)
#endif

/* Gambia */
#if 2773168 >= LOCALE_MINIMUM
#define X_CC_GM(...) X(CC_GM, __VA_ARGS__)
#else
#define X_CC_GM(...)
#endif

/* Albania */
#if 2761785 >= LOCALE_MINIMUM
#define X_CC_AL(...) X(CC_AL, __VA_ARGS__)
#else
#define X_CC_AL(...)
#endif

/* Botswana */
#if 2675352 >= LOCALE_MINIMUM
#define X_CC_BW(...) X(CC_BW, __VA_ARGS__)
#else
#define X_CC_BW(...)
#endif

/* Gabon */
#if 2538952 >= LOCALE_MINIMUM
#define X_CC_GA(...) X(CC_GA, __VA_ARGS__)
#else
#define X_CC_GA(...)
#endif

/* Lesotho */
#if 2330318 >= LOCALE_MINIMUM
#define X_CC_LS(...) X(CC_LS, __VA_ARGS__)
#else
#define X_CC_LS(...)
#endif

/* Guinea-Bissau */
#if 2201923 >= LOCALE_MINIMUM
#define X_CC_GW(...) X(CC_GW, __VA_ARGS__)
#else
#define X_CC_GW(...)
#endif

/* Slovenia */
#if 2123949 >= LOCALE_MINIMUM
#define X_CC_SI(...) X(CC_SI, __VA_ARGS__)
#else
#define X_CC_SI(...)
#endif

/* North Macedonia */
#if 2085051 >= LOCALE_MINIMUM
#define X_CC_MK(...) X(CC_MK, __VA_ARGS__)
#else
#define X_CC_MK(...)
#endif

/* Equatorial Guinea */
#if 1879706 >= LOCALE_MINIMUM
#define X_CC_GQ(...) X(CC_GQ, __VA_ARGS__)
#else
#define X_CC_GQ(...)
#endif

/* Latvia */
#if 1871871 >= LOCALE_MINIMUM
#define X_CC_LV(...) X(CC_LV, __VA_ARGS__)
#else
#define X_CC_LV(...)
#endif

/* Bahrain */
#if 1607049 >= LOCALE_MINIMUM
#define X_CC_BH(...) X(CC_BH, __VA_ARGS__)
#else
#define X_CC_BH(...)
#endif

/* Trinidad and Tobago */
#if 1543883 >= LOCALE_MINIMUM
#define X_CC_TT(...) X(CC_TT, __VA_ARGS__)
#else
#define X_CC_TT(...)
#endif

/* Timor-Leste */
#if 1413958 >= LOCALE_MINIMUM
#define X_CC_TL(...) X(CC_TL, __VA_ARGS__)
#else
#define X_CC_TL(...)
#endif

/* Estonia */
#if 1373101 >= LOCALE_MINIMUM
#define X_CC_EE(...) X(CC_EE, __VA_ARGS__)
#else
#define X_CC_EE(...)
#endif

/* Mauritius */
#if 1281353 >= LOCALE_MINIMUM
#define X_CC_MU(...) X(CC_MU, __VA_ARGS__)
#else
#define X_CC_MU(...)
#endif

/* Cyprus */
#if 1260138 >= LOCALE_MINIMUM
#define X_CC_CY(...) X(CC_CY, __VA_ARGS__)
#else
#define X_CC_CY(...)
#endif

/* Eswatini */
#if 1223362 >= LOCALE_MINIMUM
#define X_CC_SZ(...) X(CC_SZ, __VA_ARGS__)
#else
#define X_CC_SZ(...)
#endif

/* Djibouti */
#if 1168722 >= LOCALE_MINIMUM
#define X_CC_DJ(...) X(CC_DJ, __VA_ARGS__)
#else
#define X_CC_DJ(...)
#endif

/* Réunion */
#if 981796 >= LOCALE_MINIMUM
#define X_CC_RE(...) X(CC_RE, __VA_ARGS__)
#else
#define X_CC_RE(...)
#endif

/* Fiji */
#if 936375 >= LOCALE_MINIMUM
#define X_CC_FJ(...) X(CC_FJ, __VA_ARGS__)
#else
#define X_CC_FJ(...)
#endif

/* Comoros */
#if 869595 >= LOCALE_MINIMUM
#define X_CC_KM(...) X(CC_KM, __VA_ARGS__)
#else
#define X_CC_KM(...)
#endif

/* Guyana */
#if 813834 >= LOCALE_MINIMUM
#define X_CC_GY(...) X(CC_GY, __VA_ARGS__)
#else
#define X_CC_GY(...)
#endif

/* Bhutan */
#if 789667 >= LOCALE_MINIMUM
#define X_CC_BT(...) X(CC_BT, __VA_ARGS__)
#else
#define X_CC_BT(...)
#endif

/* Solomon Islands */
#if 740424 >= LOCALE_MINIMUM
#define X_CC_SB(...) X(CC_SB, __VA_ARGS__)
#else
#define X_CC_SB(...)
#endif

/* Macao */
#if 704149 >= LOCALE_MINIMUM
#define X_CC_MO(...) X(CC_MO, __VA_ARGS__)
#else
#define X_CC_MO(...)
#endif

/* Luxembourg */
#if 668606 >= LOCALE_MINIMUM
#define X_CC_LU(...) X(CC_LU, __VA_ARGS__)
#else
#define X_CC_LU(...)
#endif

/* Suriname */
#if 639759 >= LOCALE_MINIMUM
#define X_CC_SR(...) X(CC_SR, __VA_ARGS__)
#else
#define X_CC_SR(...)
#endif

/* Montenegro */
#if 619211 >= LOCALE_MINIMUM
#define X_CC_ME(...) X(CC_ME, __VA_ARGS__)
#else
#define X_CC_ME(...)
#endif

/* Cabo Verde */
#if 603899 >= LOCALE_MINIMUM
#define X_CC_CV(...) X(CC_CV, __VA_ARGS__)
#else
#define X_CC_CV(...)
#endif

/* Malta */
#if 539607 >= LOCALE_MINIMUM
#define X_CC_MT(...) X(CC_MT, __VA_ARGS__)
#else
#define X_CC_MT(...)
#endif

/* Maldives */
#if 527799 >= LOCALE_MINIMUM
#define X_CC_MV(...) X(CC_MV, __VA_ARGS__)
#else
#define X_CC_MV(...)
#endif

/* Brunei */
#if 460053 >= LOCALE_MINIMUM
#define X_CC_BN(...) X(CC_BN, __VA_ARGS__)
#else
#define X_CC_BN(...)
#endif

/* Belize */
#if 419137 >= LOCALE_MINIMUM
#define X_CC_BZ(...) X(CC_BZ, __VA_ARGS__)
#else
#define X_CC_BZ(...)
#endif

/* Iceland */
#if 383853 >= LOCALE_MINIMUM
#define X_CC_IS(...) X(CC_IS, __VA_ARGS__)
#else
#define X_CC_IS(...)
#endif

/* Vanuatu */
#if 334506 >= LOCALE_MINIMUM
#define X_CC_VU(...) X(CC_VU, __VA_ARGS__)
#else
#define X_CC_VU(...)
#endif

/* French Polynesia */
#if 306279 >= LOCALE_MINIMUM
#define X_CC_PF(...) X(CC_PF, __VA_ARGS__)
#else
#define X_CC_PF(...)
#endif

/* Barbados */
#if 283279 >= LOCALE_MINIMUM
#define X_CC_BB(...) X(CC_BB, __VA_ARGS__)
#else
#define X_CC_BB(...)
#endif

/* São Tomé and Príncipe */
#if 231597 >= LOCALE_MINIMUM
#define X_CC_ST(...) X(CC_ST, __VA_ARGS__)
#else
#define X_CC_ST(...)
#endif

/* Samoa */
#if 227196 >= LOCALE_MINIMUM
#define X_CC_WS(...) X(CC_WS, __VA_ARGS__)
#else
#define X_CC_WS(...)
#endif

/* Saint Lucia */
#if 180251 >= LOCALE_MINIMUM
#define X_CC_LC(...) X(CC_LC, __VA_ARGS__)
#else
#define X_CC_LC(...)
#endif

/* Kiribati */
#if 136958 >= LOCALE_MINIMUM
#define X_CC_KI(...) X(CC_KI, __VA_ARGS__)
#else
#define X_CC_KI(...)
#endif

/* Grenada */
#if 127411 >= LOCALE_MINIMUM
#define X_CC_GD(...) X(CC_GD, __VA_ARGS__)
#else
#define X_CC_GD(...)
#endif

/* Micronesia */
#if 114164 >= LOCALE_MINIMUM
#define X_CC_FM(...) X(CC_FM, __VA_ARGS__)
#else
#define X_CC_FM(...)
#endif

/* Tonga */
#if 107773 >= LOCALE_MINIMUM
#define X_CC_TO(...) X(CC_TO, __VA_ARGS__)
#else
#define X_CC_TO(...)
#endif

/* Seychelles */
#if 107660 >= LOCALE_MINIMUM
#define X_CC_SC(...) X(CC_SC, __VA_ARGS__)
#else
#define X_CC_SC(...)
#endif

/* Saint Vincent and the Grenadines */
#if 103698 >= LOCALE_MINIMUM
#define X_CC_VC(...) X(CC_VC, __VA_ARGS__)
#else
#define X_CC_VC(...)
#endif

/* Jersey */
#if 103267 >= LOCALE_MINIMUM
#define X_CC_JE(...) X(CC_JE, __VA_ARGS__)
#else
#define X_CC_JE(...)
#endif

/* Antigua and Barbuda */
#if 93772 >= LOCALE_MINIMUM
#define X_CC_AG(...) X(CC_AG, __VA_ARGS__)
#else
#define X_CC_AG(...)
#endif

/* Isle of Man */
#if 84710 >= LOCALE_MINIMUM
#define X_CC_IM(...) X(CC_IM, __VA_ARGS__)
#else
#define X_CC_IM(...)
#endif

/* Andorra */
#if 81938 >= LOCALE_MINIMUM
#define X_CC_AD(...) X(CC_AD, __VA_ARGS__)
#else
#define X_CC_AD(...)
#endif

/* Dominica */
#if 73040 >= LOCALE_MINIMUM
#define X_CC_DM(...) X(CC_DM, __VA_ARGS__)
#else
#define X_CC_DM(...)
#endif

/* Guernsey */
#if 64216 >= LOCALE_MINIMUM
#define X_CC_GG(...) X(CC_GG, __VA_ARGS__)
#else
#define X_CC_GG(...)
#endif

/* Greenland */
#if 56643 >= LOCALE_MINIMUM
#define X_CC_GL(...) X(CC_GL, __VA_ARGS__)
#else
#define X_CC_GL(...)
#endif

/* Faroe Islands */
#if 53358 >= LOCALE_MINIMUM
#define X_CC_FO(...) X(CC_FO, __VA_ARGS__)
#else
#define X_CC_FO(...)
#endif

/* Saint Kitts and Nevis */
#if 47755 >= LOCALE_MINIMUM
#define X_CC_KN(...) X(CC_KN, __VA_ARGS__)
#else
#define X_CC_KN(...)
#endif

/* Marshall Islands */
#if 41569 >= LOCALE_MINIMUM
#define X_CC_MH(...) X(CC_MH, __VA_ARGS__)
#else
#define X_CC_MH(...)
#endif

/* Liechtenstein */
#if 39790 >= LOCALE_MINIMUM
#define X_CC_LI(...) X(CC_LI, __VA_ARGS__)
#else
#define X_CC_LI(...)
#endif

/* Monaco */
#if 36594 >= LOCALE_MINIMUM
#define X_CC_MC(...) X(CC_MC, __VA_ARGS__)
#else
#define X_CC_MC(...)
#endif

/* San Marino */
#if 33581 >= LOCALE_MINIMUM
#define X_CC_SM(...) X(CC_SM, __VA_ARGS__)
#else
#define X_CC_SM(...)
#endif

/* Palau */
#if 18055 >= LOCALE_MINIMUM
#define X_CC_PW(...) X(CC_PW, __VA_ARGS__)
#else
#define X_CC_PW(...)
#endif

/* Nauru */
#if 12780 >= LOCALE_MINIMUM
#define X_CC_NR(...) X(CC_NR, __VA_ARGS__)
#else
#define X_CC_NR(...)
#endif

/* Tuvalu */
#if 11396 >= LOCALE_MINIMUM
#define X_CC_TV(...) X(CC_TV, __VA_ARGS__)
#else
#define X_CC_TV(...)
#endif

/* Vatican City */
#if 810 >= LOCALE_MINIMUM
#define X_CC_VA(...) X(CC_VA, __VA_ARGS__)
#else
#define X_CC_VA(...)
#endif

/* Include Countries */
#define X_COUNTRIES \
	X_CC(NONE, "\0", 0x0000) /* NONE */ \
	X_CC(IN, "IN", 0x494E) /* India */ \
	X_CC(CN, "CN", 0x434E) /* China */ \
	X_CC(US, "US", 0x5553) /* United States */ \
	X_CC(ID, "ID", 0x4944) /* Indonesia */ \
	X_CC(PK, "PK", 0x504B) /* Pakistan */ \
	X_CC(NG, "NG", 0x4E47) /* Nigeria */ \
	X_CC(BR, "BR", 0x4252) /* Brazil */ \
	X_CC(BD, "BD", 0x4244) /* Bangladesh */ \
	X_CC(RU, "RU", 0x5255) /* Russia */ \
	X_CC(MX, "MX", 0x4D58) /* Mexico */ \
	X_CC(ET, "ET", 0x4554) /* Ethiopia */ \
	X_CC(JP, "JP", 0x4A50) /* Japan */ \
	X_CC(PH, "PH", 0x5048) /* Philippines */ \
	X_CC(EG, "EG", 0x4547) /* Egypt */ \
	X_CC(CD, "CD", 0x4344) /* Congo, Democratic Republic of */ \
	X_CC(VN, "VN", 0x564E) /* Vietnam */ \
	X_CC(IR, "IR", 0x4952) /* Iran */ \
	X_CC(TR, "TR", 0x5452) /* Turkey */ \
	X_CC(DE, "DE", 0x4445) /* Germany */ \
	X_CC(TH, "TH", 0x5448) /* Thailand */ \
	X_CC(TZ, "TZ", 0x545A) /* Tanzania */ \
	X_CC(GB, "GB", 0x4742) /* Great Britain */ \
	X_CC(FR, "FR", 0x4652) /* France */ \
	X_CC(ZA, "ZA", 0x5A41) /* South Africa */ \
	X_CC(IT, "IT", 0x4954) /* Italy */ \
	X_CC(KE, "KE", 0x4B45) /* Kenya */ \
	X_CC(MM, "MM", 0x4D4D) /* Myanmar */ \
	X_CC(CO, "CO", 0x434F) /* Colombia */ \
	X_CC(KR, "KR", 0x4B52) /* South Korea */ \
	X_CC(SD, "SD", 0x5344) /* Sudan */ \
	X_CC(UG, "UG", 0x5547) /* Uganda */ \
	X_CC(ES, "ES", 0x4553) /* Spain */ \
	X_CC(DZ, "DZ", 0x445A) /* Algeria */ \
	X_CC(AR, "AR", 0x4152) /* Argentina */ \
	X_CC(IQ, "IQ", 0x4951) /* Iraq */ \
	X_CC(AF, "AF", 0x4146) /* Afghanistan */ \
	X_CC(CA, "CA", 0x4341) /* Canada */ \
	X_CC(PL, "PL", 0x504C) /* Poland */ \
	X_CC(MA, "MA", 0x4D41) /* Morocco */ \
	X_CC(AO, "AO", 0x414F) /* Angola */ \
	X_CC(SA, "SA", 0x5341) /* Saudi Arabia */ \
	X_CC(UA, "UA", 0x5541) /* Ukraine */ \
	X_CC(UZ, "UZ", 0x555A) /* Uzbekistan */ \
	X_CC(YE, "YE", 0x5945) /* Yemen */ \
	X_CC(PE, "PE", 0x5045) /* Peru */ \
	X_CC(GH, "GH", 0x4748) /* Ghana */ \
	X_CC(MZ, "MZ", 0x4D5A) /* Mozambique */ \
	X_CC(MY, "MY", 0x4D59) /* Malaysia */ \
	X_CC(MG, "MG", 0x4D47) /* Madagascar */ \
	X_CC(NP, "NP", 0x4E50) /* Nepal */ \
	X_CC(CI, "CI", 0x4349) /* Côte d'Ivoire */ \
	X_CC(CM, "CM", 0x434D) /* Cameroon */ \
	X_CC(VE, "VE", 0x5645) /* Venezuela */ \
	X_CC(NE, "NE", 0x4E45) /* Niger */ \
	X_CC(AU, "AU", 0x4155) /* Australia */ \
	X_CC(KP, "KP", 0x4B50) /* North Korea */ \
	X_CC(SY, "SY", 0x5359) /* Syria */ \
	X_CC(ML, "ML", 0x4D4C) /* Mali */ \
	X_CC(BF, "BF", 0x4246) /* Burkina Faso */ \
	X_CC(TW, "TW", 0x5457) /* Taiwan */ \
	X_CC(LK, "LK", 0x4C4B) /* Sri Lanka */ \
	X_CC(MW, "MW", 0x4D57) /* Malawi */ \
	X_CC(ZM, "ZM", 0x5A4D) /* Zambia */ \
	X_CC(RO, "RO", 0x524F) /* Romania */ \
	X_CC(KZ, "KZ", 0x4B5A) /* Kazakhstan */ \
	X_CC(CL, "CL", 0x434C) /* Chile */ \
	X_CC(TD, "TD", 0x5444) /* Chad */ \
	X_CC(SO, "SO", 0x534F) /* Somalia */ \
	X_CC(SN, "SN", 0x534E) /* Senegal */ \
	X_CC(EC, "EC", 0x4543) /* Ecuador */ \
	X_CC(NL, "NL", 0x4E4C) /* Netherlands */ \
	X_CC(KH, "KH", 0x4B48) /* Cambodia */ \
	X_CC(ZW, "ZW", 0x5A57) /* Zimbabwe */ \
	X_CC(GN, "GN", 0x474E) /* Guinea */ \
	X_CC(BJ, "BJ", 0x424A) /* Benin */ \
	X_CC(RW, "RW", 0x5257) /* Rwanda */ \
	X_CC(BI, "BI", 0x4249) /* Burundi */ \
	X_CC(BO, "BO", 0x424F) /* Bolivia */ \
	X_CC(TN, "TN", 0x544E) /* Tunisia */ \
	X_CC(SS, "SS", 0x5353) /* South Sudan */ \
	X_CC(BE, "BE", 0x4245) /* Belgium */ \
	X_CC(HT, "HT", 0x4854) /* Haiti */ \
	X_CC(JO, "JO", 0x4A4F) /* Jordan */ \
	X_CC(DO, "DO", 0x444F) /* Dominican Republic */ \
	X_CC(CU, "CU", 0x4355) /* Cuba */ \
	X_CC(CZ, "CZ", 0x435A) /* Czechia */ \
	X_CC(HN, "HN", 0x484E) /* Honduras */ \
	X_CC(SE, "SE", 0x5345) /* Sweden */ \
	X_CC(TJ, "TJ", 0x544A) /* Tajikistan */ \
	X_CC(PT, "PT", 0x5054) /* Portugal */ \
	X_CC(AZ, "AZ", 0x415A) /* Azerbaijan */ \
	X_CC(PG, "PG", 0x5047) /* Papua New Guinea */ \
	X_CC(HU, "HU", 0x4855) /* Hungary */ \
	X_CC(AE, "AE", 0x4145) /* United Arab Emirates */ \
	X_CC(GR, "GR", 0x4752) /* Greece */ \
	X_CC(TG, "TG", 0x5447) /* Togo */ \
	X_CC(BY, "BY", 0x4259) /* Belarus */ \
	X_CC(IL, "IL", 0x494C) /* Israel */ \
	X_CC(AT, "AT", 0x4154) /* Austria */ \
	X_CC(CH, "CH", 0x4348) /* Switzerland */ \
	X_CC(SL, "SL", 0x534C) /* Sierra Leone */ \
	X_CC(LA, "LA", 0x4C41) /* Laos */ \
	X_CC(HK, "HK", 0x484B) /* Hong Kong */ \
	X_CC(NI, "NI", 0x4E49) /* Nicaragua */ \
	X_CC(KG, "KG", 0x4B47) /* Kyrgyzstan */ \
	X_CC(PY, "PY", 0x5059) /* Paraguay */ \
	X_CC(LY, "LY", 0x4C59) /* Libya */ \
	X_CC(BG, "BG", 0x4247) /* Bulgaria */ \
	X_CC(RS, "RS", 0x5253) /* Serbia */ \
	X_CC(TM, "TM", 0x544D) /* Turkmenistan */ \
	X_CC(CG, "CG", 0x4347) /* Congo */ \
	X_CC(SV, "SV", 0x5356) /* El Salvador */ \
	X_CC(SG, "SG", 0x5347) /* Singapore */ \
	X_CC(DK, "DK", 0x444B) /* Denmark */ \
	X_CC(CF, "CF", 0x4346) /* Central African Republic */ \
	X_CC(FI, "FI", 0x4649) /* Finland */ \
	X_CC(NO, "NO", 0x4E4F) /* Norway */ \
	X_CC(SK, "SK", 0x534B) /* Slovakia */ \
	X_CC(PS, "PS", 0x5053) /* Palestine */ \
	X_CC(LR, "LR", 0x4C52) /* Liberia */ \
	X_CC(LB, "LB", 0x4C42) /* Lebanon */ \
	X_CC(CR, "CR", 0x4352) /* Costa Rica */ \
	X_CC(IE, "IE", 0x4945) /* Ireland */ \
	X_CC(NZ, "NZ", 0x4E5A) /* New Zealand */ \
	X_CC(MR, "MR", 0x4D52) /* Mauritania */ \
	X_CC(KW, "KW", 0x4B57) /* Kuwait */ \
	X_CC(OM, "OM", 0x4F4D) /* Oman */ \
	X_CC(PA, "PA", 0x5041) /* Panama */ \
	X_CC(HR, "HR", 0x4852) /* Croatia */ \
	X_CC(GE, "GE", 0x4745) /* Georgia */ \
	X_CC(ER, "ER", 0x4552) /* Eritrea */ \
	X_CC(MN, "MN", 0x4D4E) /* Mongolia */ \
	X_CC(UY, "UY", 0x5559) /* Uruguay */ \
	X_CC(PR, "PR", 0x5052) /* Puerto Rico */ \
	X_CC(BA, "BA", 0x4241) /* Bosnia and Herzegovina */ \
	X_CC(QA, "QA", 0x5141) /* Qatar */ \
	X_CC(MD, "MD", 0x4D44) /* Moldova */ \
	X_CC(AM, "AM", 0x414D) /* Armenia */ \
	X_CC(LT, "LT", 0x4C54) /* Lithuania */ \
	X_CC(JM, "JM", 0x4A4D) /* Jamaica */ \
	X_CC(NA, "NA", 0x4E41) /* Namibia */ \
	X_CC(GM, "GM", 0x474D) /* Gambia */ \
	X_CC(AL, "AL", 0x414C) /* Albania */ \
	X_CC(BW, "BW", 0x4257) /* Botswana */ \
	X_CC(GA, "GA", 0x4741) /* Gabon */ \
	X_CC(LS, "LS", 0x4C53) /* Lesotho */ \
	X_CC(GW, "GW", 0x4757) /* Guinea-Bissau */ \
	X_CC(SI, "SI", 0x5349) /* Slovenia */ \
	X_CC(MK, "MK", 0x4D4B) /* North Macedonia */ \
	X_CC(GQ, "GQ", 0x4751) /* Equatorial Guinea */ \
	X_CC(LV, "LV", 0x4C56) /* Latvia */ \
	X_CC(BH, "BH", 0x4248) /* Bahrain */ \
	X_CC(TT, "TT", 0x5454) /* Trinidad and Tobago */ \
	X_CC(TL, "TL", 0x544C) /* Timor-Leste */ \
	X_CC(EE, "EE", 0x4545) /* Estonia */ \
	X_CC(MU, "MU", 0x4D55) /* Mauritius */ \
	X_CC(CY, "CY", 0x4359) /* Cyprus */ \
	X_CC(SZ, "SZ", 0x535A) /* Eswatini */ \
	X_CC(DJ, "DJ", 0x444A) /* Djibouti */ \
	X_CC(RE, "RE", 0x5245) /* Réunion */ \
	X_CC(FJ, "FJ", 0x464A) /* Fiji */ \
	X_CC(KM, "KM", 0x4B4D) /* Comoros */ \
	X_CC(GY, "GY", 0x4759) /* Guyana */ \
	X_CC(BT, "BT", 0x4254) /* Bhutan */ \
	X_CC(SB, "SB", 0x5342) /* Solomon Islands */ \
	X_CC(MO, "MO", 0x4D4F) /* Macao */ \
	X_CC(LU, "LU", 0x4C55) /* Luxembourg */ \
	X_CC(SR, "SR", 0x5352) /* Suriname */ \
	X_CC(ME, "ME", 0x4D45) /* Montenegro */ \
	X_CC(CV, "CV", 0x4356) /* Cape Verde */ \
	X_CC(MT, "MT", 0x4D54) /* Malta */ \
	X_CC(MV, "MV", 0x4D56) /* Maldives */ \
	X_CC(BN, "BN", 0x424E) /* Brunei */ \
	X_CC(BZ, "BZ", 0x425A) /* Belize */ \
	X_CC(IS, "IS", 0x4953) /* Iceland */ \
	X_CC(VU, "VU", 0x5655) /* Vanuatu */ \
	X_CC(PF, "PF", 0x5046) /* French Polynesia */ \
	X_CC(BB, "BB", 0x4242) /* Barbados */ \
	X_CC(ST, "ST", 0x5354) /* São Tomé and Príncipe */ \
	X_CC(WS, "WS", 0x5753) /* Samoa */ \
	X_CC(LC, "LC", 0x4C43) /* Saint Lucia */ \
	X_CC(KI, "KI", 0x4B49) /* Kiribati */ \
	X_CC(GD, "GD", 0x4744) /* Grenada */ \
	X_CC(FM, "FM", 0x464D) /* Micronesia */ \
	X_CC(TO, "TO", 0x544F) /* Tonga */ \
	X_CC(SC, "SC", 0x5343) /* Seychelles */ \
	X_CC(VC, "VC", 0x5643) /* Saint Vincent and the Grenadines */ \
	X_CC(JE, "JE", 0x4A45) /* Jersey */ \
	X_CC(AG, "AG", 0x4147) /* Antigua and Barbuda */ \
	X_CC(IM, "IM", 0x494D) /* Isle of Man */ \
	X_CC(AD, "AD", 0x4144) /* Andorra */ \
	X_CC(DM, "DM", 0x444D) /* Dominica */ \
	X_CC(GG, "GG", 0x4747) /* Guernsey */ \
	X_CC(GL, "GL", 0x474C) /* Greenland */ \
	X_CC(FO, "FO", 0x464F) /* Faroe Islands */ \
	X_CC(KN, "KN", 0x4B4E) /* Saint Kitts and Nevis */ \
	X_CC(MH, "MH", 0x4D48) /* Marshall Islands */ \
	X_CC(LI, "LI", 0x4C49) /* Liechtenstein */ \
	X_CC(MC, "MC", 0x4D43) /* Monaco */ \
	X_CC(SM, "SM", 0x534D) /* San Marino */ \
	X_CC(PW, "PW", 0x5057) /* Palau */ \
	X_CC(NR, "NR", 0x4E52) /* Nauru */ \
	X_CC(TV, "TV", 0x5456) /* Tuvalu */ \
	X_CC(VA, "VA", 0x5641) /* Vatican City */
