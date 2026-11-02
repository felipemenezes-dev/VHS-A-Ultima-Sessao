/* Visual silhouettes in source-art coordinates (1672 x 941).
 * These are independent of the opaque outlines in collision masks. */

/* Hall: isolated shelving. The crown and sloping sides follow the art. */
ADD(SALAO, "Estante central esquerda", 516,
    (Vector2){321,413}, (Vector2){331,383}, (Vector2){345,257},
    (Vector2){374,254}, (Vector2){380,238}, (Vector2){386,237},
    (Vector2){389,255}, (Vector2){413,257}, (Vector2){426,268},
    (Vector2){428,353}, (Vector2){413,503}, (Vector2){409,516},
    (Vector2){325,514});
ADD(SALAO, "Estante central meio esquerda", 520,
    (Vector2){570,421}, (Vector2){578,348}, (Vector2){590,257},
    (Vector2){614,254}, (Vector2){616,239}, (Vector2){623,237},
    (Vector2){627,257}, (Vector2){647,259}, (Vector2){660,267},
    (Vector2){666,413}, (Vector2){660,518}, (Vector2){574,520});
ADD(SALAO, "Estante central meio direita", 518,
    (Vector2){956,426}, (Vector2){956,269}, (Vector2){968,257},
    (Vector2){992,255}, (Vector2){995,238}, (Vector2){1001,239},
    (Vector2){1005,256}, (Vector2){1028,259}, (Vector2){1039,270},
    (Vector2){1046,425}, (Vector2){1046,516}, (Vector2){958,518});
ADD(SALAO, "Estante central direita", 529,
    (Vector2){1263,429}, (Vector2){1249,277}, (Vector2){1254,260},
    (Vector2){1284,254}, (Vector2){1287,237}, (Vector2){1294,240},
    (Vector2){1298,257}, (Vector2){1324,264}, (Vector2){1343,395},
    (Vector2){1355,432}, (Vector2){1350,528}, (Vector2){1264,528});
ADD(SALAO, "Estante baixa frontal", 677,
    (Vector2){1073,544}, (Vector2){1078,531}, (Vector2){1092,531},
    (Vector2){1092,522}, (Vector2){1178,522}, (Vector2){1178,533},
    (Vector2){1192,535}, (Vector2){1200,569}, (Vector2){1194,675},
    (Vector2){1074,677});

/* Each arm of the L has its own floor baseline: John in front of the
 * horizontal arm must not inherit the deeper vertical arm's ordering. */
ADD(SALAO, "Balcao horizontal", 649,
    (Vector2){153,565}, (Vector2){166,527}, (Vector2){172,526},
    (Vector2){175,513}, (Vector2){217,513}, (Vector2){220,528},
    (Vector2){434,527}, (Vector2){432,575}, (Vector2){420,650},
    (Vector2){164,649});
ADD(SALAO, "Balcao vertical", 786,
    (Vector2){434,527}, (Vector2){440,520}, (Vector2){451,521},
    (Vector2){456,527}, (Vector2){483,528}, (Vector2){486,534},
    (Vector2){499,540}, (Vector2){520,560}, (Vector2){512,709},
    (Vector2){500,786}, (Vector2){411,780}, (Vector2){405,718},
    (Vector2){420,650}, (Vector2){432,575});

/* Back-wall furniture is only in front while John is behind its own base. */
ADD(SALAO, "Estante fundo esquerda", 275,
    (Vector2){208,154}, (Vector2){217,153}, (Vector2){222,109},
    (Vector2){257,106}, (Vector2){267,115}, (Vector2){275,103},
    (Vector2){300,99}, (Vector2){311,107}, (Vector2){314,150},
    (Vector2){324,161}, (Vector2){326,267}, (Vector2){214,275});
ADD(SALAO, "Estante fundo centro", 218,
    (Vector2){668,76}, (Vector2){681,75}, (Vector2){683,64},
    (Vector2){707,64}, (Vector2){714,77}, (Vector2){851,77},
    (Vector2){852,75}, (Vector2){863,77}, (Vector2){866,213},
    (Vector2){669,218});
ADD(SALAO, "Estante fundo direita", 216,
    (Vector2){952,103}, (Vector2){960,102}, (Vector2){964,103},
    (Vector2){1068,103}, (Vector2){1078,111}, (Vector2){1082,211},
    (Vector2){954,216});
ADD(SALAO, "Televisores superiores", 337,
    (Vector2){1327,245}, (Vector2){1337,190}, (Vector2){1362,180},
    (Vector2){1367,124}, (Vector2){1394,114}, (Vector2){1437,134},
    (Vector2){1439,188}, (Vector2){1420,194}, (Vector2){1428,217},
    (Vector2){1420,256}, (Vector2){1434,246}, (Vector2){1464,263},
    (Vector2){1460,302}, (Vector2){1450,302}, (Vector2){1458,326},
    (Vector2){1437,338}, (Vector2){1407,329}, (Vector2){1404,310},
    (Vector2){1394,318}, (Vector2){1326,288});

/* Side shelving faces the room. Kept foreground along its long footprint. */
ADD(SALAO, "Estante lateral esquerda superior", 0,
    (Vector2){92,289}, (Vector2){142,167}, (Vector2){169,166},
    (Vector2){191,179}, (Vector2){201,282}, (Vector2){170,371},
    (Vector2){155,396}, (Vector2){111,384}, (Vector2){104,339});
ADD(SALAO, "Estante lateral esquerda inferior", 0,
    (Vector2){25,568}, (Vector2){73,566}, (Vector2){84,582},
    (Vector2){98,667}, (Vector2){108,674}, (Vector2){111,699},
    (Vector2){97,725}, (Vector2){72,765}, (Vector2){42,742});
ADD(SALAO, "Estante lateral direita superior", 0,
    (Vector2){1480,166}, (Vector2){1508,166}, (Vector2){1537,207},
    (Vector2){1584,292}, (Vector2){1566,370}, (Vector2){1547,410},
    (Vector2){1508,437}, (Vector2){1475,323});
ADD(SALAO, "Estante lateral direita intermediaria", 0,
    (Vector2){1559,357}, (Vector2){1584,358}, (Vector2){1626,430},
    (Vector2){1619,492}, (Vector2){1580,541}, (Vector2){1541,515},
    (Vector2){1514,438});
ADD(SALAO, "Estante lateral direita inferior", 0,
    (Vector2){1587,558}, (Vector2){1619,551}, (Vector2){1646,622},
    (Vector2){1644,733}, (Vector2){1609,772}, (Vector2){1581,711},
    (Vector2){1573,641});
/* Wall rims are independent from the shelf cutouts. On the left there is
 * deliberately no connecting polygon across the bathroom doorway. */
ADD(SALAO, "Parede lateral esquerda superior", 0,
    (Vector2){15,266}, (Vector2){137,33}, (Vector2){146,42},
    (Vector2){106,141}, (Vector2){85,211}, (Vector2){72,265},
    (Vector2){33,334}, (Vector2){24,366});
ADD(SALAO, "Parede lateral esquerda abaixo da porta", 0,
    (Vector2){18,568}, (Vector2){37,557}, (Vector2){40,721},
    (Vector2){47,747}, (Vector2){21,736});
ADD(SALAO, "Parede lateral direita", 0,
    (Vector2){1534,36}, (Vector2){1656,266}, (Vector2){1655,736},
    (Vector2){1636,755}, (Vector2){1635,300}, (Vector2){1529,61});
ADD(SALAO, "Televisores inferiores", 710,
    (Vector2){1454,543}, (Vector2){1473,533}, (Vector2){1491,540},
    (Vector2){1519,544}, (Vector2){1544,530}, (Vector2){1573,542},
    (Vector2){1592,574}, (Vector2){1583,627}, (Vector2){1567,643},
    (Vector2){1583,650}, (Vector2){1574,704}, (Vector2){1534,710},
    (Vector2){1476,650}, (Vector2){1455,590});

/* Front shelves and wall: separate left/right groups leave exit clear. */
ADD(SALAO, "Estantes dianteiras esquerdas", 0,
    (Vector2){105,774}, (Vector2){116,735}, (Vector2){375,734},
    (Vector2){385,750}, (Vector2){387,838}, (Vector2){131,839});
ADD(SALAO, "Estantes dianteiras direitas", 0,
    (Vector2){1042,760}, (Vector2){1050,756}, (Vector2){1194,757},
    (Vector2){1198,736}, (Vector2){1200,717}, (Vector2){1231,712},
    (Vector2){1246,736}, (Vector2){1429,734}, (Vector2){1446,767},
    (Vector2){1442,838}, (Vector2){1044,840});
ADD(SALAO, "Parede frontal esquerda", 0,
    (Vector2){19,734}, (Vector2){132,838}, (Vector2){655,838},
    (Vector2){655,941}, (Vector2){0,941}, (Vector2){0,755});
ADD(SALAO, "Parede frontal direita", 0,
    (Vector2){1020,838}, (Vector2){1542,838}, (Vector2){1655,734},
    (Vector2){1672,755}, (Vector2){1672,941}, (Vector2){1020,941});
ADD(SALAO, "Moldura de saida esquerda", 0,
    (Vector2){700,728}, (Vector2){738,728}, (Vector2){740,875},
    (Vector2){709,879}, (Vector2){698,764});
ADD(SALAO, "Moldura de saida direita", 0,
    (Vector2){938,728}, (Vector2){975,729}, (Vector2){980,874},
    (Vector2){940,879});
ADD(SALAO, "Travessa superior da saida", 0,
    (Vector2){701,727}, (Vector2){972,727}, (Vector2){977,742},
    (Vector2){977,765}, (Vector2){698,765}, (Vector2){698,742});

/* Bathroom: each stall face follows its actual silhouette; the sink does not
 * repaint the floor between its legs. */
ADD(BANHEIRO, "Cabine esquerda", 575,
    (Vector2){188,329}, (Vector2){331,330}, (Vector2){350,207},
    (Vector2){359,146}, (Vector2){372,145}, (Vector2){353,326},
    (Vector2){359,544}, (Vector2){369,571}, (Vector2){348,576},
    (Vector2){338,558}, (Vector2){204,556});
ADD(BANHEIRO, "Cabine direita", 576,
    (Vector2){347,331}, (Vector2){499,331}, (Vector2){520,150},
    (Vector2){534,143}, (Vector2){543,153}, (Vector2){531,325},
    (Vector2){533,573}, (Vector2){515,577}, (Vector2){504,553},
    (Vector2){367,552});
ADD(BANHEIRO, "Bancada da pia", 383,
    (Vector2){679,255}, (Vector2){797,255}, (Vector2){800,260},
    (Vector2){836,260}, (Vector2){838,230}, (Vector2){847,230},
    (Vector2){851,260}, (Vector2){951,260}, (Vector2){955,250},
    (Vector2){965,246}, (Vector2){974,260}, (Vector2){1001,260},
    (Vector2){1010,320}, (Vector2){1006,333}, (Vector2){998,333},
    (Vector2){997,382}, (Vector2){981,382}, (Vector2){981,370},
    (Vector2){705,370}, (Vector2){705,383}, (Vector2){689,383},
    (Vector2){687,333}, (Vector2){678,331});
ADD(BANHEIRO, "Lixeira", 790,
    (Vector2){155,694}, (Vector2){163,674}, (Vector2){181,660},
    (Vector2){207,654}, (Vector2){230,661}, (Vector2){244,676},
    (Vector2){247,695}, (Vector2){236,711}, (Vector2){235,779},
    (Vector2){218,791}, (Vector2){178,789}, (Vector2){161,775});
ADD(BANHEIRO, "Balde", 798,
    (Vector2){1364,753}, (Vector2){1378,732}, (Vector2){1398,721},
    (Vector2){1430,731}, (Vector2){1455,747}, (Vector2){1448,775},
    (Vector2){1440,799}, (Vector2){1380,801}, (Vector2){1364,776});

/* Perimeter walls are in front of the play plane. The right-hand door's
 * visible opening is deliberately absent from these polygons. */
ADD(BANHEIRO, "Parede lateral esquerda", 0,
    (Vector2){142,130}, (Vector2){181,129}, (Vector2){195,31},
    (Vector2){232,31}, (Vector2){211,166}, (Vector2){184,174},
    (Vector2){161,377}, (Vector2){145,504}, (Vector2){105,800},
    (Vector2){60,845});
ADD(BANHEIRO, "Parede lateral direita superior", 0,
    (Vector2){1372,66}, (Vector2){1413,32}, (Vector2){1427,130},
    (Vector2){1457,132}, (Vector2){1479,306}, (Vector2){1543,305},
    (Vector2){1548,350}, (Vector2){1437,349}, (Vector2){1418,394},
    (Vector2){1410,580}, (Vector2){1389,584}, (Vector2){1355,334});
ADD(BANHEIRO, "Batente direito", 0,
    (Vector2){1471,417}, (Vector2){1491,426}, (Vector2){1527,791},
    (Vector2){1566,800}, (Vector2){1600,845}, (Vector2){1482,835},
    (Vector2){1473,596});
ADD(BANHEIRO, "Batente esquerdo", 0,
    (Vector2){1417,391}, (Vector2){1438,401}, (Vector2){1424,584},
    (Vector2){1408,590});
ADD(BANHEIRO, "Parede dianteira", 0,
    (Vector2){95,799}, (Vector2){1007,799}, (Vector2){1012,850},
    (Vector2){1166,850}, (Vector2){1169,801}, (Vector2){1565,801},
    (Vector2){1600,846}, (Vector2){1214,846}, (Vector2){1214,895},
    (Vector2){962,895}, (Vector2){964,846}, (Vector2){60,846});
