/*******************************************************************************
* File Name: Thermistor.c
* Version 1.20
*
* Description:
*  This file provides the source code to the API for the ThermistorCalc
*  Component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Thermistor.h"


/*******************************************************************************
* Function Name: Thermistor_GetResistance
********************************************************************************
*
* Summary:
*  The digital values of the voltages across the reference resistor and the
*  thermistor are passed to this function as parameters. The function returns
*  the resistance, based on the voltage values.
*
* Parameters:
*  vReference: the voltage across the reference resistor;
*  vThermistor: the voltage across the thermistor.
*  The ratio of these two voltages is used by this function. Therefore, the
*  units for both parameters must be the same.
*
* Return:
*  The return value is the resistance across the thermistor. The value returned
*  is the resistance in Ohms.
*
*******************************************************************************/
uint32 Thermistor_GetResistance(int16 vReference, int16 vThermistor)
                                      
{
    int32 resT;
    int16 temp;

    /* Calculate thermistor resistance from the voltages */
    resT = Thermistor_REF_RESISTOR * ((int32)vThermistor);
    if (vReference < 0)
    {
        temp = -vReference;
        temp = (int16)((uint16)((uint16)temp >> 1u));
        temp = -temp;
    }
    else
    {
        temp = (int16)((uint16)((uint16)vReference >> 1u));
    }
    resT += temp;
    resT /= vReference;

    /* The ordering of Reference resistor value is specifically designed to keep the result from overflowing */
    return ((uint32)((uint32)resT << Thermistor_REF_RES_SHIFT));
}


/*******************************************************************************
* Function Name: Thermistor_GetTemperature
********************************************************************************
*
* Summary:
*  The value of the thermistor resistance is passed to this function as a
*  parameter. The function returns the temperature, based on the resistance
*  value. The method used to calculate the temperature is dependent on whether
*  Equation or LUT was selected in the Customizer.
*
* Parameters:
*  resT: the resistance across the thermistor in Ohms.
*
* Return:
*  The return value is the temperature in 1/100ths of degrees C. For example,
*  the return value is 2345, when the actual temperature is 23.45 degrees C.
*
*******************************************************************************/
int16 Thermistor_GetTemperature(uint32 resT) 
{
    int16 tempTR;
    static const uint32 CYCODE Thermistor_LUT[] = { 419380u, 416455u, 413552u, 410671u, 407812u, 404975u, 402160u, 399366u,
 396593u, 393842u, 391111u, 388401u, 385711u, 383042u, 380393u, 377764u, 375155u, 372566u, 369996u, 367445u, 364914u,
 362402u, 359909u, 357435u, 354979u, 352542u, 350123u, 347722u, 345339u, 342974u, 340627u, 338298u, 335986u, 333691u,
 331413u, 329153u, 326909u, 324682u, 322471u, 320278u, 318100u, 315939u, 313793u, 311664u, 309551u, 307453u, 305371u,
 303304u, 301252u, 299216u, 297195u, 295189u, 293198u, 291221u, 289259u, 287311u, 285378u, 283459u, 281554u, 279664u,
 277787u, 275924u, 274074u, 272239u, 270416u, 268608u, 266812u, 265029u, 263260u, 261503u, 259760u, 258029u, 256311u,
 254605u, 252912u, 251231u, 249562u, 247906u, 246261u, 244629u, 243008u, 241399u, 239802u, 238217u, 236643u, 235080u,
 233529u, 231989u, 230460u, 228942u, 227435u, 225939u, 224454u, 222979u, 221515u, 220062u, 218619u, 217186u, 215764u,
 214352u, 212950u, 211558u, 210177u, 208805u, 207443u, 206090u, 204748u, 203415u, 202091u, 200777u, 199472u, 198177u,
 196891u, 195614u, 194346u, 193087u, 191837u, 190596u, 189364u, 188141u, 186926u, 185720u, 184522u, 183333u, 182152u,
 180980u, 179816u, 178660u, 177513u, 176373u, 175241u, 174118u, 173002u, 171895u, 170795u, 169702u, 168618u, 167541u,
 166471u, 165409u, 164355u, 163308u, 162268u, 161236u, 160210u, 159192u, 158181u, 157178u, 156181u, 155191u, 154208u,
 153231u, 152262u, 151299u, 150343u, 149394u, 148451u, 147515u, 146586u, 145662u, 144745u, 143835u, 142931u, 142033u,
 141141u, 140255u, 139376u, 138503u, 137635u, 136774u, 135918u, 135069u, 134225u, 133387u, 132555u, 131728u, 130907u,
 130092u, 129283u, 128479u, 127680u, 126887u, 126099u, 125317u, 124540u, 123769u, 123002u, 122241u, 121485u, 120734u,
 119989u, 119248u, 118512u, 117782u, 117056u, 116335u, 115620u, 114909u, 114202u, 113501u, 112804u, 112113u, 111425u,
 110743u, 110065u, 109391u, 108722u, 108058u, 107398u, 106743u, 106091u, 105445u, 104802u, 104164u, 103531u, 102901u,
 102276u, 101655u, 101038u, 100425u, 99816u, 99212u, 98611u, 98014u, 97422u, 96833u, 96248u, 95668u, 95091u, 94517u,
 93948u, 93382u, 92821u, 92263u, 91708u, 91158u, 90610u, 90067u, 89527u, 88991u, 88458u, 87929u, 87403u, 86881u, 86362u,
 85847u, 85335u, 84826u, 84321u, 83819u, 83320u, 82825u, 82333u, 81844u, 81358u, 80876u, 80396u, 79920u, 79447u, 78977u,
 78510u, 78046u, 77585u, 77127u, 76672u, 76220u, 75771u, 75325u, 74882u, 74442u, 74004u, 73570u, 73138u, 72709u, 72283u,
 71859u, 71439u, 71021u, 70605u, 70193u, 69783u, 69376u, 68971u, 68569u, 68169u, 67772u, 67378u, 66986u, 66597u, 66210u,
 65826u, 65444u, 65065u, 64688u, 64313u, 63941u, 63571u, 63204u, 62839u, 62476u, 62116u, 61758u, 61402u, 61048u, 60697u,
 60348u, 60001u, 59656u, 59314u, 58973u, 58635u, 58299u, 57965u, 57634u, 57304u, 56976u, 56651u, 56328u, 56006u, 55687u,
 55369u, 55054u, 54741u, 54429u, 54120u, 53812u, 53507u, 53203u, 52902u, 52602u, 52304u, 52008u, 51714u, 51421u, 51131u,
 50842u, 50555u, 50270u, 49986u, 49705u, 49425u, 49147u, 48871u, 48596u, 48323u, 48052u, 47782u, 47514u, 47248u, 46984u,
 46721u, 46459u, 46200u, 45942u, 45685u, 45430u, 45177u, 44925u, 44675u, 44426u, 44179u, 43934u, 43690u, 43447u, 43206u,
 42966u, 42728u, 42492u, 42257u, 42023u, 41790u, 41560u, 41330u, 41102u, 40875u, 40650u, 40426u, 40203u, 39982u, 39762u,
 39544u, 39327u, 39111u, 38896u, 38683u, 38471u, 38260u, 38051u, 37843u, 37636u, 37431u, 37226u, 37023u, 36821u, 36621u,
 36421u, 36223u, 36026u, 35830u, 35635u, 35442u, 35249u, 35058u, 34868u, 34679u, 34492u, 34305u, 34120u, 33935u, 33752u,
 33570u, 33389u, 33209u, 33030u, 32852u, 32675u, 32499u, 32325u, 32151u, 31978u, 31807u, 31636u, 31467u, 31298u, 31131u,
 30964u, 30799u, 30634u, 30470u, 30308u, 30146u, 29985u, 29826u, 29667u, 29509u, 29352u, 29196u, 29041u, 28887u, 28734u,
 28581u, 28430u, 28279u, 28130u, 27981u, 27833u, 27686u, 27540u, 27394u, 27250u, 27106u, 26964u, 26822u, 26680u, 26540u,
 26401u, 26262u, 26124u, 25987u, 25851u, 25715u, 25581u, 25447u, 25314u, 25181u, 25050u, 24919u, 24789u, 24660u, 24531u,
 24404u, 24276u, 24150u, 24025u, 23900u, 23776u, 23652u, 23530u, 23408u, 23286u, 23166u, 23046u, 22927u, 22808u, 22690u,
 22573u, 22457u, 22341u, 22226u, 22111u, 21997u, 21884u, 21772u, 21660u, 21549u, 21438u, 21328u, 21219u, 21110u, 21002u,
 20894u, 20787u, 20681u, 20576u, 20470u, 20366u, 20262u, 20159u, 20056u, 19954u, 19853u, 19752u, 19651u, 19551u, 19452u,
 19353u, 19255u, 19158u, 19061u, 18964u, 18868u, 18773u, 18678u, 18584u, 18490u, 18397u, 18304u, 18212u, 18120u, 18029u,
 17938u, 17848u, 17758u, 17669u, 17581u, 17492u, 17405u, 17318u, 17231u, 17145u, 17059u, 16974u, 16889u, 16805u, 16721u,
 16638u, 16555u, 16472u, 16390u, 16309u, 16228u, 16147u, 16067u, 15987u, 15908u, 15829u, 15751u, 15673u, 15595u, 15518u,
 15441u, 15365u, 15289u, 15214u, 15139u, 15064u, 14990u, 14916u, 14843u, 14770u, 14697u, 14625u, 14553u, 14482u, 14411u,
 14340u, 14270u, 14200u, 14131u, 14062u, 13993u, 13924u, 13857u, 13789u, 13722u, 13655u, 13588u, 13522u, 13457u, 13391u,
 13326u, 13261u, 13197u, 13133u, 13069u, 13006u, 12943u, 12880u, 12818u, 12756u, 12695u, 12633u, 12572u, 12512u, 12451u,
 12391u, 12332u, 12273u, 12214u, 12155u, 12096u, 12038u, 11981u, 11923u, 11866u, 11809u, 11753u, 11697u, 11641u, 11585u,
 11530u, 11475u, 11420u, 11365u, 11311u, 11257u, 11204u, 11151u, 11098u, 11045u, 10992u, 10940u, 10888u, 10837u, 10785u,
 10734u, 10684u, 10633u, 10583u, 10533u, 10483u, 10434u, 10384u, 10335u, 10287u, 10238u, 10190u, 10142u, 10095u, 10047u,
 10000u, 9953u, 9906u, 9860u, 9814u, 9768u, 9722u, 9677u, 9631u, 9586u, 9542u, 9497u, 9453u, 9409u, 9365u, 9321u, 9278u,
 9235u, 9192u, 9149u, 9107u, 9065u, 9023u, 8981u, 8939u, 8898u, 8857u, 8816u, 8775u, 8734u, 8694u, 8654u, 8614u, 8574u,
 8535u, 8496u, 8457u, 8418u, 8379u, 8341u, 8302u, 8264u, 8226u, 8189u, 8151u, 8114u, 8077u, 8040u, 8003u, 7966u, 7930u,
 7894u, 7858u, 7822u, 7787u, 7751u, 7716u, 7681u, 7646u, 7611u, 7577u, 7542u, 7508u, 7474u, 7440u, 7406u, 7373u, 7340u,
 7306u, 7273u, 7241u, 7208u, 7175u, 7143u, 7111u, 7079u, 7047u, 7015u, 6984u, 6953u, 6921u, 6890u, 6859u, 6829u, 6798u,
 6768u, 6737u, 6707u, 6677u, 6647u, 6618u, 6588u, 6559u, 6530u, 6501u, 6472u, 6443u, 6414u, 6386u, 6357u, 6329u, 6301u,
 6273u, 6245u, 6218u, 6190u, 6163u, 6136u, 6108u, 6081u, 6055u, 6028u, 6001u, 5975u, 5949u, 5922u, 5896u, 5870u, 5845u,
 5819u, 5793u, 5768u, 5743u, 5717u, 5692u, 5668u, 5643u, 5618u, 5593u, 5569u, 5545u, 5521u, 5496u, 5473u, 5449u, 5425u,
 5401u, 5378u, 5354u, 5331u, 5308u, 5285u, 5262u, 5239u, 5217u, 5194u, 5172u, 5149u, 5127u, 5105u, 5083u, 5061u, 5039u,
 5017u, 4996u, 4974u, 4953u, 4931u, 4910u, 4889u, 4868u, 4847u, 4826u, 4806u, 4785u, 4765u, 4744u, 4724u, 4704u, 4684u,
 4664u, 4644u, 4624u, 4604u, 4585u, 4565u, 4546u, 4526u, 4507u, 4488u, 4469u, 4450u, 4431u, 4412u, 4393u, 4375u, 4356u,
 4338u, 4320u, 4301u, 4283u, 4265u, 4247u, 4229u, 4211u, 4194u, 4176u, 4158u, 4141u, 4123u, 4106u, 4089u, 4072u, 4055u,
 4038u, 4021u, 4004u, 3987u, 3970u, 3954u, 3937u, 3921u, 3904u, 3888u, 3872u, 3856u, 3840u, 3824u, 3808u, 3792u, 3776u,
 3761u, 3745u, 3729u, 3714u, 3699u, 3683u, 3668u, 3653u, 3638u, 3623u, 3608u, 3593u, 3578u, 3563u, 3548u, 3534u, 3519u,
 3505u, 3490u, 3476u, 3462u, 3448u, 3433u, 3419u, 3405u, 3391u, 3377u, 3364u, 3350u, 3336u, 3322u, 3309u, 3295u, 3282u,
 3269u, 3255u, 3242u, 3229u, 3216u, 3203u, 3190u, 3177u, 3164u, 3151u, 3138u, 3125u, 3113u, 3100u, 3087u, 3075u, 3063u,
 3050u, 3038u, 3026u, 3013u, 3001u, 2989u, 2977u, 2965u, 2953u, 2941u, 2929u, 2918u, 2906u, 2894u, 2883u, 2871u, 2860u,
 2848u, 2837u, 2825u, 2814u, 2803u, 2792u, 2780u, 2769u, 2758u, 2747u, 2736u, 2726u, 2715u, 2704u, 2693u, 2682u, 2672u,
 2661u, 2651u, 2640u, 2630u, 2619u, 2609u, 2599u, 2588u, 2578u, 2568u, 2558u, 2548u, 2538u, 2528u, 2518u, 2508u, 2498u,
 2488u, 2478u, 2468u, 2459u, 2449u, 2439u, 2430u, 2420u, 2411u, 2401u, 2392u, 2383u, 2373u, 2364u, 2355u, 2346u, 2337u,
 2327u, 2318u, 2309u, 2300u, 2291u, 2282u, 2274u, 2265u, 2256u, 2247u, 2239u, 2230u, 2221u, 2213u, 2204u, 2196u, 2187u,
 2179u, 2170u, 2162u, 2153u, 2145u, 2137u, 2129u, 2120u, 2112u, 2104u, 2096u, 2088u, 2080u, 2072u, 2064u, 2056u, 2048u,
 2040u, 2033u, 2025u, 2017u, 2009u, 2002u, 1994u, 1986u, 1979u, 1971u, 1964u, 1956u, 1949u, 1941u, 1934u, 1927u, 1919u,
 1912u, 1905u, 1898u, 1890u, 1883u, 1876u, 1869u, 1862u, 1855u, 1848u, 1841u, 1834u, 1827u, 1820u, 1813u, 1807u, 1800u,
 1793u, 1786u, 1780u, 1773u, 1766u, 1760u, 1753u, 1746u, 1740u, 1733u, 1727u, 1720u, 1714u, 1708u, 1701u, 1695u, 1688u,
 1682u, 1676u, 1670u, 1663u, 1657u, 1651u, 1645u, 1639u, 1633u, 1627u, 1621u, 1615u, 1609u, 1603u, 1597u, 1591u, 1585u,
 1579u, 1573u, 1567u, 1562u, 1556u, 1550u, 1544u, 1539u, 1533u, 1527u, 1522u, 1516u, 1511u, 1505u, 1500u, 1494u, 1489u,
 1483u, 1478u, 1472u, 1467u, 1462u, 1456u, 1451u, 1446u, 1440u, 1435u, 1430u, 1425u, 1419u, 1414u, 1409u, 1404u, 1399u,
 1394u, 1389u, 1384u, 1379u, 1374u, 1369u, 1364u, 1359u, 1354u, 1349u, 1344u, 1339u, 1334u, 1330u, 1325u, 1320u, 1315u,
 1311u, 1306u, 1301u, 1296u, 1292u, 1287u, 1283u, 1278u, 1273u, 1269u, 1264u, 1260u, 1255u, 1251u, 1246u, 1242u, 1237u,
 1233u, 1229u, 1224u, 1220u, 1215u, 1211u, 1207u, 1203u, 1198u, 1194u, 1190u, 1186u, 1181u, 1177u, 1173u, 1169u, 1165u,
 1161u, 1156u, 1152u, 1148u, 1144u, 1140u, 1136u, 1132u, 1128u, 1124u, 1120u, 1116u, 1112u, 1108u, 1105u, 1101u, 1097u,
 1093u, 1089u, 1085u, 1081u, 1078u, 1074u, 1070u, 1066u, 1063u, 1059u, 1055u, 1052u, 1048u, 1044u, 1041u, 1037u, 1033u,
 1030u, 1026u, 1023u, 1019u, 1016u, 1012u, 1009u, 1005u, 1002u, 998u, 995u, 991u, 988u, 984u, 981u, 978u, 974u, 971u,
 967u, 964u, 961u, 958u, 954u, 951u, 948u, 944u, 941u, 938u, 935u, 932u, 928u, 925u, 922u, 919u, 916u, 913u, 909u, 906u,
 903u, 900u, 897u, 894u, 891u, 888u, 885u, 882u, 879u, 876u, 873u, 870u, 867u, 864u, 861u, 858u, 855u, 852u, 850u, 847u,
 844u, 841u, 838u, 835u, 832u, 830u, 827u, 824u, 821u, 819u, 816u, 813u, 810u, 808u, 805u, 802u, 799u, 797u, 794u, 791u,
 789u, 786u, 784u, 781u, 778u, 776u, 773u, 771u, 768u, 765u, 763u, 760u, 758u, 755u, 753u, 750u, 748u, 745u, 743u, 740u,
 738u, 735u, 733u, 731u, 728u, 726u, 723u, 721u, 719u, 716u, 714u, 712u, 709u, 707u, 705u, 702u, 700u, 698u, 695u, 693u,
 691u, 689u, 686u, 684u, 682u, 680u, 677u, 675u, 673u, 671u, 669u, 666u, 664u, 662u, 660u, 658u, 656u, 653u, 651u, 649u,
 647u, 645u, 643u, 641u, 639u, 637u, 635u, 633u, 630u, 628u, 626u, 624u, 622u, 620u, 618u, 616u, 614u, 612u, 610u, 608u,
 606u, 605u, 603u, 601u, 599u, 597u, 595u, 593u, 591u, 589u, 587u, 585u, 584u, 582u, 580u, 578u, 576u, 574u, 572u, 571u,
 569u, 567u, 565u, 563u, 562u, 560u, 558u, 556u, 555u, 553u, 551u, 549u, 548u, 546u, 544u, 542u, 541u, 539u, 537u, 536u,
 534u, 532u, 530u, 529u, 527u, 525u, 524u, 522u, 521u, 519u, 517u, 516u, 514u, 512u, 511u, 509u, 508u, 506u, 504u, 503u,
 501u, 500u, 498u, 497u, 495u, 494u, 492u, 490u, 489u, 487u, 486u, 484u, 483u, 481u, 480u, 478u, 477u, 475u, 474u, 472u,
 471u, 470u, 468u, 467u, 465u, 464u, 462u, 461u, 459u, 458u, 457u, 455u, 454u, 452u, 451u, 450u, 448u, 447u, 446u, 444u,
 443u, 441u, 440u, 439u, 437u, 436u, 435u, 433u, 432u, 431u, 429u, 428u, 427u, 426u, 424u, 423u, 422u, 420u, 419u, 418u,
 417u, 415u, 414u, 413u, 412u, 410u, 409u, 408u, 407u, 405u, 404u, 403u, 402u, 400u, 399u, 398u, 397u, 396u, 394u, 393u,
 392u, 391u, 390u, 389u, 387u, 386u, 385u, 384u, 383u, 382u, 381u, 379u, 378u, 377u, 376u, 375u, 374u, 373u, 372u, 370u,
 369u, 368u, 367u, 366u, 365u, 364u, 363u, 362u, 361u, 360u, 358u, 357u, 356u, 355u, 354u, 353u, 352u, 351u, 350u, 349u,
 348u, 347u, 346u, 345u, 344u, 343u, 342u, 341u, 340u, 339u, 338u, 337u, 336u, 335u, 334u, 333u, 332u, 331u, 330u, 329u,
 328u, 327u, 326u, 325u, 324u, 323u, 322u, 321u, 321u, 320u, 319u, 318u, 317u, 316u, 315u, 314u, 313u, 312u, 311u, 310u,
 310u, 309u, 308u, 307u, 306u, 305u, 304u, 303u, 302u, 302u, 301u, 300u, 299u, 298u, 297u, 296u, 296u, 295u, 294u, 293u,
 292u, 291u, 291u, 290u, 289u, 288u, 287u, 286u, 286u, 285u, 284u, 283u, 282u, 281u, 281u, 280u, 279u, 278u, 278u, 277u,
 276u, 275u, 274u, 274u, 273u, 272u, 271u, 271u, 270u, 269u, 268u, 267u, 267u, 266u, 265u, 264u, 264u, 263u, 262u, 261u,
 261u, 260u };


    #if (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD)

        float32 stEqn;
        float32 logrT;

        /* Calculate thermistor resistance from the voltages */
        #if(!CY_PSOC3)
            logrT = (float32)log((float64)resT);
        #else
            logrT = log((float32)resT);
        #endif  /* (!CY_PSOC3) */

        /* Calculate temperature from the resistance of thermistor using Steinhart-Hart Equation */
        #if(!CY_PSOC3)
            stEqn = (float32)(Thermistor_THA + (Thermistor_THB * logrT) + 
                             (Thermistor_THC * pow((float64)logrT, (float32)3)));
        #else
            stEqn = (float32)(Thermistor_THA + (Thermistor_THB * logrT) + 
                             (Thermistor_THC * pow(logrT, (float32)3)));
        #endif  /* (!CY_PSOC3) */

        tempTR = (int16)((float32)((((1.0 / stEqn) - Thermistor_K2C) * (float32)Thermistor_SCALE) + 0.5));

    #else /* Thermistor_IMPLEMENTATION == Thermistor_LUT_METHOD */

        uint16 mid;
        uint16 first = 0u;
        uint16 last = Thermistor_LUT_SIZE - 1u;

        /* Calculate temperature from the resistance of thermistor by using the LUT */
        while (first < last)
        {
            mid = (first + last) >> 1u;

            if ((0u == mid) || ((Thermistor_LUT_SIZE - 1u) == mid) || (resT == Thermistor_LUT[mid]))
            {
                last = mid;
                break;
            }
            else if (Thermistor_LUT[mid] > resT)
            {
                first = mid + 1u;
            }
            else /* (Thermistor_LUT[mid] < resT) */
            {
                last = mid - 1u;
            }
        }

        /* Calculation the closest entry in the LUT */
        if ((Thermistor_LUT[last] > resT) && (last < (Thermistor_LUT_SIZE - 1u)) &&
           ((Thermistor_LUT[last] - resT) > (resT - Thermistor_LUT[last + 1u])))
        {
            last++;
        }
        else if ((Thermistor_LUT[last] < resT) && (last > 0u) &&
                ((Thermistor_LUT[last - 1u] - resT) < (resT - Thermistor_LUT[last])))
        {
            last--;
        }
        else
        {
            /* Closest entry in the LUT already found */
        }

        tempTR = Thermistor_MIN_TEMP + (int16)((uint16)(last * Thermistor_ACCURACY));

    #endif /* (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD) */

    return (tempTR);
}


/* [] END OF FILE */
