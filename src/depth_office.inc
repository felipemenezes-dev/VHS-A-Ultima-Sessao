// Office silhouettes in source-image coordinates (1672 x 941).
// Side furniture is attached to the perimeter, so its visible side faces stay
// in front of John. Separate upper/lower cabinets preserve the stepped aisle.
ADD(ESCRITORIO, "estante lateral esquerda alta", 0,
    (Vector2){210,212}, {252,207}, {296,218}, {302,269}, {302,341},
    {309,359}, {309,575}, {282,579}, {282,641}, {207,641}, {207,570});
ADD(ESCRITORIO, "estante lateral esquerda baixa", 0,
    (Vector2){307,343}, {341,342}, {346,363}, {370,364}, {373,493},
    {367,590}, {364,704}, {290,709}, {286,649}, {295,645}, {297,580}, {301,580});
ADD(ESCRITORIO, "armario lateral direito superior", 0,
    (Vector2){1395,313}, {1451,312}, {1455,351}, {1460,479},
    {1405,479}, {1403,382}, {1391,381});
ADD(ESCRITORIO, "estante lateral direita equipamentos", 0,
    (Vector2){1314,377}, {1359,374}, {1394,380}, {1405,396}, {1428,401},
    {1430,432}, {1430,476}, {1403,479}, {1403,511}, {1457,511},
    {1460,714}, {1355,722}, {1321,716}, {1315,559}, {1309,446});
ADD(ESCRITORIO, "bobina esquerda", 0,
    (Vector2){1341,407}, {1344,396}, {1354,391}, {1366,394}, {1377,401},
    {1379,415}, {1373,429}, {1358,432}, {1345,425});
ADD(ESCRITORIO, "bobina direita", 0,
    (Vector2){1381,408}, {1385,398}, {1399,394}, {1413,402}, {1419,414},
    {1415,427}, {1404,436}, {1391,431}, {1384,423});
// Desk: tabletop, cabinet pedestals and back panel. The floor beneath stays clear.
ADD(ESCRITORIO, "tampo mesa", 403,
    (Vector2){665,227}, {1031,226}, {1104,273}, {1107,323},
    {1101,334}, {665,333});
ADD(ESCRITORIO, "gaveteiro mesa esquerdo", 403,
    (Vector2){670,333}, {770,334}, {765,400}, {676,401});
ADD(ESCRITORIO, "gaveteiro mesa direito", 403,
    (Vector2){986,333}, {1105,334}, {1099,401}, {989,398});
ADD(ESCRITORIO, "painel traseiro mesa", 403,
    (Vector2){768,333}, {987,333}, {986,365}, {768,365});
ADD(ESCRITORIO, "televisor sobre mesa", 403,
    (Vector2){981,181}, {1014,155}, {1085,181}, {1082,250},
    {1062,270}, {980,247});
ADD(ESCRITORIO, "luminaria mesa", 403,
    (Vector2){701,245}, {699,239}, {702,232}, {708,230}, {707,203},
    {699,194}, {703,184}, {720,174}, {727,163}, {739,164},
    {750,175}, {760,188}, {757,198}, {744,207}, {731,208},
    {727,201}, {725,190}, {714,195}, {714,230}, {725,235},
    {729,246}, {719,254}, {708,253});
ADD(ESCRITORIO, "cadeira atras mesa", 278,
    (Vector2){838,166}, {850,155}, {900,155}, {912,170},
    {916,220}, {920,227}, {834,227});
ADD(ESCRITORIO, "cadeira visitante encosto e assento", 418,
    (Vector2){521,315}, {549,290}, {559,286}, {580,309}, {575,330},
    {615,342}, {625,355}, {614,372}, {603,381}, {572,378},
    {545,364}, {532,351});
ADD(ESCRITORIO, "cadeira visitante pedestal", 418,
    (Vector2){566,377}, {586,379}, {585,391}, {597,396}, {608,388},
    {616,389}, {616,396}, {594,404}, {602,413}, {598,420},
    {588,416}, {579,404}, {550,406}, {541,400}, {542,393},
    {550,391}, {553,397}, {570,392});
ADD(ESCRITORIO, "banqueta direita tampo", 708,
    (Vector2){1224,639}, {1267,619}, {1288,627}, {1298,652},
    {1254,676}, {1230,661});
ADD(ESCRITORIO, "banqueta perna esquerda", 708,
    (Vector2){1230,659}, {1239,665}, {1245,699}, {1240,705});
ADD(ESCRITORIO, "banqueta perna direita", 708,
    (Vector2){1288,657}, {1298,654}, {1300,701}, {1294,707});
ADD(ESCRITORIO, "banqueta travessa", 708,
    (Vector2){1242,687}, {1294,679}, {1296,687}, {1244,695});
ADD(ESCRITORIO, "armario traseiro esquerdo", 291,
    (Vector2){323,174}, {412,174}, {419,283}, {409,291}, {327,290});
ADD(ESCRITORIO, "livros sobre armario", 291,
    (Vector2){331,167}, {359,163}, {366,173}, {394,169}, {411,177},
    {409,201}, {326,201});
ADD(ESCRITORIO, "estante traseira", 286,
    (Vector2){580,58}, {777,57}, {776,227}, {666,227}, {665,285}, {582,285});
ADD(ESCRITORIO, "arquivo traseiro direito", 274,
    (Vector2){1257,72}, {1329,74}, {1342,94}, {1337,271}, {1255,274});
ADD(ESCRITORIO, "livros traseiros direitos", 280,
    (Vector2){1116,181}, {1181,179}, {1185,208}, {1219,208},
    {1245,228}, {1236,277}, {1116,280});
// Structural foreground is broken at the exit: never cover the red doorway.
ADD(ESCRITORIO, "parede lateral esquerda", 0,
    (Vector2){171,5}, {208,3}, {207,44}, {204,742}, {184,760}, {161,757});
ADD(ESCRITORIO, "parede lateral direita", 0,
    (Vector2){1456,31}, {1501,28}, {1505,745}, {1480,760}, {1469,749});
ADD(ESCRITORIO, "parede frontal esquerda", 0,
    (Vector2){141,743}, {181,741}, {185,759}, {737,759},
    {738,789}, {185,800}, {184,833}, {144,831}, {138,786});
ADD(ESCRITORIO, "batente esquerdo", 0,
    (Vector2){738,741}, {766,742}, {771,779}, {765,906}, {739,906});
ADD(ESCRITORIO, "parede frontal direita", 0,
    (Vector2){937,761}, {1489,760}, {1493,742}, {1530,743},
    {1537,789}, {1532,832}, {1496,832}, {1488,799}, {937,790});
ADD(ESCRITORIO, "batente direito", 0,
    (Vector2){908,741}, {934,740}, {939,907}, {908,907}, {904,778});
