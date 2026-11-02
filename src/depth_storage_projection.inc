/* Visual silhouettes traced from the original 1672 x 941 artwork.
 * These polygons are independent of the movement/collision masks.
 * Separate parts deliberately leave floor visible between legs and drawers. */

/* Storage: freestanding shelves and their irregular fabric/box silhouettes. */
ADD(ESTOQUE, "estante central com lona", 647,
    (Vector2){520,278}, (Vector2){527,261}, (Vector2){546,257},
    (Vector2){565,270}, (Vector2){585,268}, (Vector2){599,280},
    (Vector2){605,313}, (Vector2){601,342}, (Vector2){600,644},
    (Vector2){519,647}, (Vector2){516,476}, (Vector2){519,353},
    (Vector2){511,336}, (Vector2){514,300});
ADD(ESTOQUE, "estante central direita", 617,
    (Vector2){615,104}, (Vector2){623,102}, (Vector2){624,111},
    (Vector2){692,109}, (Vector2){696,101}, (Vector2){703,102},
    (Vector2){705,282}, (Vector2){698,305}, (Vector2){704,341},
    (Vector2){703,610}, (Vector2){692,616}, (Vector2){691,603},
    (Vector2){626,609}, (Vector2){624,617}, (Vector2){615,617},
    (Vector2){614,535}, (Vector2){610,530}, (Vector2){614,516});
ADD(ESTOQUE, "lixeira central", 625,
    (Vector2){616,552}, (Vector2){632,550}, (Vector2){645,558},
    (Vector2){647,574}, (Vector2){643,614}, (Vector2){636,622},
    (Vector2){622,624}, (Vector2){611,617}, (Vector2){608,572},
    (Vector2){608,562});
ADD(ESTOQUE, "estante fundo esquerda", 352,
    (Vector2){397,102}, (Vector2){407,102}, (Vector2){408,111},
    (Vector2){526,110}, (Vector2){528,102}, (Vector2){537,102},
    (Vector2){539,276}, (Vector2){520,287}, (Vector2){511,341},
    (Vector2){411,343}, (Vector2){411,352}, (Vector2){401,351});
ADD(ESTOQUE, "pilha entre estantes", 281,
    (Vector2){550,153}, (Vector2){593,153}, (Vector2){598,194},
    (Vector2){602,194}, (Vector2){600,266}, (Vector2){589,278},
    (Vector2){553,274}, (Vector2){547,258});
ADD(ESTOQUE, "caixas junto a porta esquerda", 260,
    (Vector2){707,182}, (Vector2){736,183}, (Vector2){739,216},
    (Vector2){744,219}, (Vector2){744,247}, (Vector2){732,259},
    (Vector2){707,257});
ADD(ESTOQUE, "caixas junto a porta direita", 263,
    (Vector2){934,183}, (Vector2){963,183}, (Vector2){964,153},
    (Vector2){990,151}, (Vector2){992,126}, (Vector2){1044,125},
    (Vector2){1048,194}, (Vector2){1043,204}, (Vector2){1042,251},
    (Vector2){1008,262}, (Vector2){970,257}, (Vector2){970,250},
    (Vector2){934,252});
ADD(ESTOQUE, "lixeira do fundo", 303,
    (Vector2){1036,234}, (Vector2){1052,230}, (Vector2){1067,237},
    (Vector2){1071,248}, (Vector2){1067,288}, (Vector2){1059,300},
    (Vector2){1044,303}, (Vector2){1032,293}, (Vector2){1029,247});
ADD(ESTOQUE, "estante fundo direita", 370,
    (Vector2){1088,102}, (Vector2){1096,102}, (Vector2){1097,111},
    (Vector2){1189,107}, (Vector2){1191,102}, (Vector2){1200,103},
    (Vector2){1204,345}, (Vector2){1188,356}, (Vector2){1176,355},
    (Vector2){1175,366}, (Vector2){1145,370}, (Vector2){1124,360},
    (Vector2){1096,357}, (Vector2){1095,367}, (Vector2){1087,367});
ADD(ESTOQUE, "bancada de fundo", 364,
    (Vector2){1209,99}, (Vector2){1214,99}, (Vector2){1212,69},
    (Vector2){1251,68}, (Vector2){1251,100}, (Vector2){1307,100},
    (Vector2){1311,101}, (Vector2){1313,99}, (Vector2){1355,99},
    (Vector2){1361,107}, (Vector2){1364,343}, (Vector2){1356,352},
    (Vector2){1354,342}, (Vector2){1218,342}, (Vector2){1218,363},
    (Vector2){1208,364});

/* Wall-attached shelves: their visible inward faces stay in the foreground. */
ADD(ESTOQUE, "estante lateral esquerda", 0,
    (Vector2){173,244}, (Vector2){231,243}, (Vector2){239,251},
    (Vector2){239,381}, (Vector2){247,390}, (Vector2){248,512},
    (Vector2){241,548}, (Vector2){156,548}, (Vector2){156,389},
    (Vector2){164,378}, (Vector2){168,260});
ADD(ESTOQUE, "caixas esquerda", 0,
    (Vector2){163,512}, (Vector2){194,512}, (Vector2){197,504},
    (Vector2){241,505}, (Vector2){247,535}, (Vector2){247,598},
    (Vector2){157,599}, (Vector2){155,547});
ADD(ESTOQUE, "lona canto inferior esquerdo", 0,
    (Vector2){156,600}, (Vector2){210,599}, (Vector2){228,602},
    (Vector2){247,596}, (Vector2){262,595}, (Vector2){277,599},
    (Vector2){291,595}, (Vector2){305,605}, (Vector2){325,605},
    (Vector2){333,625}, (Vector2){333,671}, (Vector2){327,701},
    (Vector2){318,728}, (Vector2){305,733}, (Vector2){294,723},
    (Vector2){270,730}, (Vector2){252,725}, (Vector2){241,734},
    (Vector2){211,727}, (Vector2){211,706}, (Vector2){153,703});
ADD(ESTOQUE, "caixas inferiores", 0,
    (Vector2){222,731}, (Vector2){315,731}, (Vector2){315,700},
    (Vector2){348,700}, (Vector2){352,666}, (Vector2){405,667},
    (Vector2){409,677}, (Vector2){465,672}, (Vector2){472,701},
    (Vector2){476,749}, (Vector2){552,751}, (Vector2){559,781},
    (Vector2){215,781});
ADD(ESTOQUE, "estante lateral direita", 0,
    (Vector2){1416,194}, (Vector2){1468,194}, (Vector2){1475,228},
    (Vector2){1492,239}, (Vector2){1494,283}, (Vector2){1486,314},
    (Vector2){1503,316}, (Vector2){1509,507}, (Vector2){1499,547},
    (Vector2){1429,557}, (Vector2){1408,543}, (Vector2){1399,421},
    (Vector2){1393,327}, (Vector2){1391,261}, (Vector2){1408,254});
ADD(ESTOQUE, "caixas e lixeira direita", 0,
    (Vector2){1331,278}, (Vector2){1365,279}, (Vector2){1376,289},
    (Vector2){1377,340}, (Vector2){1387,350}, (Vector2){1393,389},
    (Vector2){1380,407}, (Vector2){1387,419}, (Vector2){1384,454},
    (Vector2){1377,467}, (Vector2){1358,469}, (Vector2){1340,461},
    (Vector2){1336,443}, (Vector2){1320,442}, (Vector2){1313,414},
    (Vector2){1314,349}, (Vector2){1329,344});
ADD(ESTOQUE, "armario e caixas canto direito", 0,
    (Vector2){1432,522}, (Vector2){1456,523}, (Vector2){1459,502},
    (Vector2){1508,502}, (Vector2){1515,529}, (Vector2){1516,571},
    (Vector2){1505,579}, (Vector2){1518,598}, (Vector2){1517,704},
    (Vector2){1446,710}, (Vector2){1424,683}, (Vector2){1423,582},
    (Vector2){1430,572});
ADD(ESTOQUE, "bancada direita tampo", 701,
    (Vector2){1247,550}, (Vector2){1415,550}, (Vector2){1417,587},
    (Vector2){1423,610}, (Vector2){1420,634}, (Vector2){1251,634},
    (Vector2){1247,619});
ADD(ESTOQUE, "bancada direita base", 701,
    (Vector2){1251,634}, (Vector2){1420,634}, (Vector2){1416,696},
    (Vector2){1403,701}, (Vector2){1403,687}, (Vector2){1265,687},
    (Vector2){1265,701}, (Vector2){1253,701});
ADD(ESTOQUE, "parede lateral esquerda", 0,
    (Vector2){131,138}, (Vector2){234,138}, (Vector2){234,155},
    (Vector2){151,157}, (Vector2){120,707}, (Vector2){209,707},
    (Vector2){207,728}, (Vector2){94,730});
ADD(ESTOQUE, "parede lateral direita", 0,
    (Vector2){1438,138}, (Vector2){1540,138}, (Vector2){1579,729},
    (Vector2){1463,730}, (Vector2){1463,710}, (Vector2){1554,708},
    (Vector2){1519,156}, (Vector2){1438,157});
ADD(ESTOQUE, "parede frontal esquerda", 0,
    (Vector2){202,780}, (Vector2){723,780}, (Vector2){724,766},
    (Vector2){759,765}, (Vector2){759,917}, (Vector2){725,917},
    (Vector2){725,822}, (Vector2){176,806}, (Vector2){179,727},
    (Vector2){205,727});
ADD(ESTOQUE, "parede frontal direita", 0,
    (Vector2){936,766}, (Vector2){970,766}, (Vector2){970,781},
    (Vector2){1470,780}, (Vector2){1466,729}, (Vector2){1495,729},
    (Vector2){1497,807}, (Vector2){970,822}, (Vector2){968,917},
    (Vector2){936,917});

/* Projection room: equipment is separated from its tables and floor cables. */
ADD(SALA_PROJECAO, "mesa central tampo", 407,
    (Vector2){683,278}, (Vector2){991,277}, (Vector2){993,344},
    (Vector2){987,355}, (Vector2){681,355}, (Vector2){678,340});
ADD(SALA_PROJECAO, "mesa central gaveteiro esquerdo", 407,
    (Vector2){684,354}, (Vector2){748,354}, (Vector2){748,395},
    (Vector2){690,399}, (Vector2){690,407}, (Vector2){681,407});
ADD(SALA_PROJECAO, "mesa central gaveteiro direito", 407,
    (Vector2){923,353}, (Vector2){987,353}, (Vector2){984,405},
    (Vector2){976,407}, (Vector2){976,399}, (Vector2){923,398});
ADD(SALA_PROJECAO, "projetor central", 407,
    (Vector2){767,251}, (Vector2){770,246}, (Vector2){771,192},
    (Vector2){781,190}, (Vector2){785,221}, (Vector2){797,221},
    (Vector2){799,237}, (Vector2){812,237}, (Vector2){816,242},
    (Vector2){837,242}, (Vector2){844,251}, (Vector2){847,326},
    (Vector2){772,329}, (Vector2){767,316});
ADD(SALA_PROJECAO, "bobina sobre mesa", 407,
    (Vector2){706,270}, (Vector2){719,266}, (Vector2){734,268},
    (Vector2){745,274}, (Vector2){749,286}, (Vector2){746,305},
    (Vector2){738,314}, (Vector2){717,318}, (Vector2){701,313},
    (Vector2){696,299}, (Vector2){697,280});
ADD(SALA_PROJECAO, "fitas sobre mesa central", 407,
    (Vector2){855,239}, (Vector2){883,239}, (Vector2){887,250},
    (Vector2){885,260}, (Vector2){891,267}, (Vector2){888,280},
    (Vector2){894,293}, (Vector2){892,324}, (Vector2){853,325},
    (Vector2){849,303}, (Vector2){853,286}, (Vector2){848,276},
    (Vector2){853,261});
ADD(SALA_PROJECAO, "maletas sobre mesa central", 407,
    (Vector2){911,253}, (Vector2){935,253}, (Vector2){943,260},
    (Vector2){949,251}, (Vector2){973,251}, (Vector2){982,263},
    (Vector2){978,298}, (Vector2){909,298});
ADD(SALA_PROJECAO, "banco central assento", 431,
    (Vector2){801,368}, (Vector2){815,363}, (Vector2){827,366},
    (Vector2){837,372}, (Vector2){837,382}, (Vector2){830,389},
    (Vector2){810,392}, (Vector2){797,385}, (Vector2){796,376});
ADD(SALA_PROJECAO, "banco central perna esquerda", 431,
    (Vector2){805,390}, (Vector2){812,391}, (Vector2){808,422},
    (Vector2){802,423});
ADD(SALA_PROJECAO, "banco central perna direita", 431,
    (Vector2){826,391}, (Vector2){832,389}, (Vector2){835,422},
    (Vector2){831,423});
ADD(SALA_PROJECAO, "banco central perna frontal", 431,
    (Vector2){816,392}, (Vector2){823,392}, (Vector2){823,431},
    (Vector2){817,431});
ADD(SALA_PROJECAO, "televisor esquerdo", 397,
    (Vector2){485,226}, (Vector2){554,206}, (Vector2){572,224},
    (Vector2){577,286}, (Vector2){586,295}, (Vector2){587,373},
    (Vector2){491,399}, (Vector2){479,383}, (Vector2){475,290},
    (Vector2){492,282});
ADD(SALA_PROJECAO, "estante fundo esquerda", 288,
    (Vector2){351,120}, (Vector2){392,119}, (Vector2){395,138},
    (Vector2){432,138}, (Vector2){443,159}, (Vector2){443,278},
    (Vector2){432,285}, (Vector2){429,278}, (Vector2){347,280},
    (Vector2){343,270}, (Vector2){345,172}, (Vector2){352,168});
ADD(SALA_PROJECAO, "bobina pedestal fundo esquerda", 274,
    (Vector2){579,99}, (Vector2){598,83}, (Vector2){620,81},
    (Vector2){640,92}, (Vector2){650,113}, (Vector2){651,136},
    (Vector2){642,151}, (Vector2){633,157}, (Vector2){637,181},
    (Vector2){652,184}, (Vector2){659,270}, (Vector2){582,274},
    (Vector2){578,191}, (Vector2){594,181}, (Vector2){596,159},
    (Vector2){580,147}, (Vector2){573,127});
ADD(SALA_PROJECAO, "bobina pedestal fundo direita", 272,
    (Vector2){1026,98}, (Vector2){1046,83}, (Vector2){1066,84},
    (Vector2){1083,96}, (Vector2){1093,115}, (Vector2){1090,139},
    (Vector2){1074,154}, (Vector2){1074,184}, (Vector2){1090,189},
    (Vector2){1091,269}, (Vector2){1010,272}, (Vector2){1010,191},
    (Vector2){1036,181}, (Vector2){1037,157}, (Vector2){1022,145},
    (Vector2){1018,126});
ADD(SALA_PROJECAO, "estantes laterais esquerdas", 0,
    (Vector2){332,278}, (Vector2){345,266}, (Vector2){366,265},
    (Vector2){383,273}, (Vector2){386,290}, (Vector2){381,317},
    (Vector2){390,331}, (Vector2){388,420}, (Vector2){407,428},
    (Vector2){450,435}, (Vector2){453,514}, (Vector2){445,541},
    (Vector2){425,557}, (Vector2){384,551}, (Vector2){375,540},
    (Vector2){375,608}, (Vector2){358,637}, (Vector2){355,696},
    (Vector2){344,705}, (Vector2){290,705}, (Vector2){284,688},
    (Vector2){288,568}, (Vector2){301,545}, (Vector2){300,450},
    (Vector2){314,428}, (Vector2){316,383}, (Vector2){326,338});
ADD(SALA_PROJECAO, "pilha alta esquerda", 0,
    (Vector2){409,276}, (Vector2){418,263}, (Vector2){436,260},
    (Vector2){449,269}, (Vector2){454,284}, (Vector2){449,300},
    (Vector2){456,305}, (Vector2){455,331}, (Vector2){459,342},
    (Vector2){459,394}, (Vector2){465,402}, (Vector2){462,446},
    (Vector2){453,450}, (Vector2){398,449}, (Vector2){398,415},
    (Vector2){404,393}, (Vector2){403,354}, (Vector2){409,323},
    (Vector2){404,305});
ADD(SALA_PROJECAO, "caixa frontal esquerda", 0,
    (Vector2){375,629}, (Vector2){451,629}, (Vector2){459,646},
    (Vector2){452,740}, (Vector2){439,754}, (Vector2){360,742},
    (Vector2){356,682});
ADD(SALA_PROJECAO, "bobinas inferiores esquerdas", 0,
    (Vector2){297,700}, (Vector2){317,690}, (Vector2){336,695},
    (Vector2){346,708}, (Vector2){348,732}, (Vector2){357,721},
    (Vector2){370,713}, (Vector2){387,716}, (Vector2){397,730},
    (Vector2){400,744}, (Vector2){411,735}, (Vector2){425,736},
    (Vector2){435,745}, (Vector2){436,771}, (Vector2){290,771},
    (Vector2){290,733}, (Vector2){288,718});
ADD(SALA_PROJECAO, "estantes laterais direitas", 0,
    (Vector2){1271,201}, (Vector2){1291,201}, (Vector2){1308,212},
    (Vector2){1311,230}, (Vector2){1305,247}, (Vector2){1319,250},
    (Vector2){1323,270}, (Vector2){1331,289}, (Vector2){1340,314},
    (Vector2){1342,364}, (Vector2){1364,389}, (Vector2){1368,420},
    (Vector2){1394,448}, (Vector2){1400,521}, (Vector2){1410,557},
    (Vector2){1409,631}, (Vector2){1415,671}, (Vector2){1410,737},
    (Vector2){1384,754}, (Vector2){1313,749}, (Vector2){1305,687},
    (Vector2){1308,613}, (Vector2){1294,578}, (Vector2){1291,485},
    (Vector2){1299,441}, (Vector2){1260,434}, (Vector2){1245,413},
    (Vector2){1247,365}, (Vector2){1235,349}, (Vector2){1242,316},
    (Vector2){1236,292}, (Vector2){1243,269}, (Vector2){1241,251},
    (Vector2){1262,247}, (Vector2){1260,231}, (Vector2){1260,212});
ADD(SALA_PROJECAO, "mesa lateral direita tampo", 571,
    (Vector2){1125,426}, (Vector2){1266,425}, (Vector2){1278,447},
    (Vector2){1277,492}, (Vector2){1127,496}, (Vector2){1121,480});
ADD(SALA_PROJECAO, "mesa lateral direita perna", 571,
    (Vector2){1127,495}, (Vector2){1141,495}, (Vector2){1140,554},
    (Vector2){1131,555});
ADD(SALA_PROJECAO, "mesa lateral direita gaveteiro", 571,
    (Vector2){1204,493}, (Vector2){1276,493}, (Vector2){1271,565},
    (Vector2){1263,571}, (Vector2){1262,563}, (Vector2){1198,565},
    (Vector2){1195,549});
ADD(SALA_PROJECAO, "aparelho mesa lateral direita", 571,
    (Vector2){1203,384}, (Vector2){1232,382}, (Vector2){1242,390},
    (Vector2){1243,429}, (Vector2){1234,442}, (Vector2){1203,440},
    (Vector2){1198,422});
ADD(SALA_PROJECAO, "luminaria direita cupula", 571,
    (Vector2){1167,381}, (Vector2){1175,381}, (Vector2){1183,389},
    (Vector2){1187,398}, (Vector2){1183,405}, (Vector2){1166,402},
    (Vector2){1159,395});
ADD(SALA_PROJECAO, "luminaria direita haste", 571,
    (Vector2){1168,386}, (Vector2){1171,389}, (Vector2){1147,411},
    (Vector2){1148,430}, (Vector2){1145,430}, (Vector2){1143,409});
ADD(SALA_PROJECAO, "luminaria direita base", 571,
    (Vector2){1140,429}, (Vector2){1154,429}, (Vector2){1160,435},
    (Vector2){1158,441}, (Vector2){1140,442}, (Vector2){1135,436});
ADD(SALA_PROJECAO, "caixas inferiores direitas", 0,
    (Vector2){1217,683}, (Vector2){1265,682}, (Vector2){1268,675},
    (Vector2){1296,676}, (Vector2){1304,693}, (Vector2){1317,697},
    (Vector2){1319,730}, (Vector2){1327,737}, (Vector2){1324,773},
    (Vector2){1213,773}, (Vector2){1208,716});
ADD(SALA_PROJECAO, "parede lateral esquerda", 0,
    (Vector2){245,159}, (Vector2){299,158}, (Vector2){300,176},
    (Vector2){267,178}, (Vector2){243,744}, (Vector2){288,744},
    (Vector2){288,769}, (Vector2){217,765});
ADD(SALA_PROJECAO, "parede lateral direita", 0,
    (Vector2){1344,158}, (Vector2){1425,158}, (Vector2){1458,767},
    (Vector2){1385,772}, (Vector2){1385,747}, (Vector2){1433,744},
    (Vector2){1405,179}, (Vector2){1344,180});
ADD(SALA_PROJECAO, "parede frontal esquerda", 0,
    (Vector2){267,771}, (Vector2){727,771}, (Vector2){728,746},
    (Vector2){754,745}, (Vector2){754,869}, (Vector2){276,869},
    (Vector2){265,807});
ADD(SALA_PROJECAO, "parede frontal direita", 0,
    (Vector2){903,746}, (Vector2){929,747}, (Vector2){929,772},
    (Vector2){1414,771}, (Vector2){1410,837}, (Vector2){1399,868},
    (Vector2){903,869});
