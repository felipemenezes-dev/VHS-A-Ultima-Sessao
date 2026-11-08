#include "camadas.h"
#include <math.h>
#include <string.h>


typedef struct {
    Texture2D textura;
    Vector2 origem;
    float profundidade;
    int sala;
} Camada;
static Camada camadas[256];
static int quantidadeCamadas;


static bool pontoDentroPoligono(Vector2 p, const Vector2 *v, int quantidade) {
    bool atingiu = false;
    for (int i = 0, j = quantidade - 1; i < quantidade; j = i++) {
        if ((v[i].y > p.y) != (v[j].y > p.y) &&
            p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x)
            atingiu = !atingiu;
    }
    return atingiu;
}


static bool adicionarCamada(const Image *imagensOriginais, int sala, const char *nome,
                            float profundidade, const Vector2 *pontos, int quantidade) {
    if (quantidadeCamadas >= (int)(sizeof camadas / sizeof *camadas) || quantidade < 3)
        return false;
    float esquerda = LARGURA_MUNDO, topo = ALTURA_MUNDO, direita = 0, base = 0;
    for (int i = 0; i < quantidade; i++) {
        esquerda = fminf(esquerda, pontos[i].x);
        topo = fminf(topo, pontos[i].y);
        direita = fmaxf(direita, pontos[i].x);
        base = fmaxf(base, pontos[i].y);
    }
    int x = (int)fmaxf(0, floorf(esquerda)), y = (int)fmaxf(0, floorf(topo));
    int largura = (int)fminf(LARGURA_MUNDO, ceilf(direita)) - x;
    int altura = (int)fminf(ALTURA_MUNDO, ceilf(base)) - y;
    if (largura <= 0 || altura <= 0)
        return false;
    Image recorte = GenImageColor(largura, altura, BLANK);
    if (!recorte.data)
        return false;
    Color *cores = recorte.data;
    const Color *original = imagensOriginais[sala].data;
    for (int linha = 0; linha < altura; linha++)
        for (int coluna = 0; coluna < largura; coluna++)
            if (pontoDentroPoligono((Vector2){x + coluna + .5f, y + linha + .5f}, pontos,
                                    quantidade))
                cores[linha * largura + coluna] =
                    original[(y + linha) * LARGURA_MUNDO + x + coluna];
    Texture2D textura = LoadTextureFromImage(recorte);
    UnloadImage(recorte);
    if (!textura.id) {
        TraceLog(LOG_ERROR, "Falha no recorte visual: %s", nome);
        return false;
    }
    SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    camadas[quantidadeCamadas++] = (Camada){textura, {(float)x, (float)y}, profundidade, sala};
    return true;
}


void liberarCamadas(void) {
    for (int i = 0; i < quantidadeCamadas; i++)
        UnloadTexture(camadas[i].textura);
    memset(camadas, 0, sizeof camadas);
    quantidadeCamadas = 0;
}


#define ADICIONAR_CAMADA(sala, nome, profundidade, ...)                                            \
    do {                                                                                           \
        const Vector2 contorno[] = {__VA_ARGS__};                                                  \
        if (sucesso)                                                                               \
            sucesso = adicionarCamada(imagensOriginais, sala, nome, profundidade, contorno,        \
                                      (int)(sizeof contorno / sizeof *contorno));                  \
    } while (0)


static bool carregarCamadasEscritorio(const Image *imagensOriginais) {
    bool sucesso = true;
    ADICIONAR_CAMADA(ESCRITORIO, "estante lateral esquerda alta", 0, (Vector2){210, 212},
                     {252, 207}, {296, 218}, {302, 269}, {302, 341}, {309, 359}, {309, 575},
                     {282, 579}, {282, 641}, {207, 641}, {207, 570});
    ADICIONAR_CAMADA(ESCRITORIO, "estante lateral esquerda baixa", 0, (Vector2){307, 343},
                     {341, 342}, {346, 363}, {370, 364}, {373, 493}, {367, 590}, {364, 704},
                     {290, 709}, {286, 649}, {295, 645}, {297, 580}, {301, 580});
    ADICIONAR_CAMADA(ESCRITORIO, "armario lateral direito superior", 0, (Vector2){1395, 313},
                     {1451, 312}, {1455, 351}, {1460, 479}, {1405, 479}, {1403, 382}, {1391, 381});
    ADICIONAR_CAMADA(ESCRITORIO, "estante lateral direita equipamentos", 0, (Vector2){1314, 377},
                     {1359, 374}, {1394, 380}, {1405, 396}, {1428, 401}, {1430, 432}, {1430, 476},
                     {1403, 479}, {1403, 511}, {1457, 511}, {1460, 714}, {1355, 722}, {1321, 716},
                     {1315, 559}, {1309, 446});
    ADICIONAR_CAMADA(ESCRITORIO, "bobina esquerda", 0, (Vector2){1341, 407}, {1344, 396},
                     {1354, 391}, {1366, 394}, {1377, 401}, {1379, 415}, {1373, 429}, {1358, 432},
                     {1345, 425});
    ADICIONAR_CAMADA(ESCRITORIO, "bobina direita", 0, (Vector2){1381, 408}, {1385, 398},
                     {1399, 394}, {1413, 402}, {1419, 414}, {1415, 427}, {1404, 436}, {1391, 431},
                     {1384, 423});


    ADICIONAR_CAMADA(ESCRITORIO, "tampo mesa", 403, (Vector2){665, 227}, {1031, 226}, {1104, 273},
                     {1107, 323}, {1101, 334}, {665, 333});
    ADICIONAR_CAMADA(ESCRITORIO, "gaveteiro mesa esquerdo", 403, (Vector2){670, 333}, {770, 334},
                     {765, 400}, {676, 401});
    ADICIONAR_CAMADA(ESCRITORIO, "gaveteiro mesa direito", 403, (Vector2){986, 333}, {1105, 334},
                     {1099, 401}, {989, 398});
    ADICIONAR_CAMADA(ESCRITORIO, "painel traseiro mesa", 403, (Vector2){768, 333}, {987, 333},
                     {986, 365}, {768, 365});
    ADICIONAR_CAMADA(ESCRITORIO, "televisor sobre mesa", 403, (Vector2){981, 181}, {1014, 155},
                     {1085, 181}, {1082, 250}, {1062, 270}, {980, 247});
    ADICIONAR_CAMADA(ESCRITORIO, "luminaria mesa", 403, (Vector2){701, 245}, {699, 239}, {702, 232},
                     {708, 230}, {707, 203}, {699, 194}, {703, 184}, {720, 174}, {727, 163},
                     {739, 164}, {750, 175}, {760, 188}, {757, 198}, {744, 207}, {731, 208},
                     {727, 201}, {725, 190}, {714, 195}, {714, 230}, {725, 235}, {729, 246},
                     {719, 254}, {708, 253});
    ADICIONAR_CAMADA(ESCRITORIO, "cadeira atras mesa", 278, (Vector2){838, 166}, {850, 155},
                     {900, 155}, {912, 170}, {916, 220}, {920, 227}, {834, 227});
    ADICIONAR_CAMADA(ESCRITORIO, "cadeira visitante encosto e assento", 418, (Vector2){521, 315},
                     {549, 290}, {559, 286}, {580, 309}, {575, 330}, {615, 342}, {625, 355},
                     {614, 372}, {603, 381}, {572, 378}, {545, 364}, {532, 351});
    ADICIONAR_CAMADA(ESCRITORIO, "cadeira visitante pedestal", 418, (Vector2){566, 377}, {586, 379},
                     {585, 391}, {597, 396}, {608, 388}, {616, 389}, {616, 396}, {594, 404},
                     {602, 413}, {598, 420}, {588, 416}, {579, 404}, {550, 406}, {541, 400},
                     {542, 393}, {550, 391}, {553, 397}, {570, 392});
    ADICIONAR_CAMADA(ESCRITORIO, "banqueta direita tampo", 708, (Vector2){1224, 639}, {1267, 619},
                     {1288, 627}, {1298, 652}, {1254, 676}, {1230, 661});
    ADICIONAR_CAMADA(ESCRITORIO, "banqueta perna esquerda", 708, (Vector2){1230, 659}, {1239, 665},
                     {1245, 699}, {1240, 705});
    ADICIONAR_CAMADA(ESCRITORIO, "banqueta perna direita", 708, (Vector2){1288, 657}, {1298, 654},
                     {1300, 701}, {1294, 707});
    ADICIONAR_CAMADA(ESCRITORIO, "banqueta travessa", 708, (Vector2){1242, 687}, {1294, 679},
                     {1296, 687}, {1244, 695});
    ADICIONAR_CAMADA(ESCRITORIO, "armario traseiro esquerdo", 291, (Vector2){323, 174}, {412, 174},
                     {419, 283}, {409, 291}, {327, 290});
    ADICIONAR_CAMADA(ESCRITORIO, "livros sobre armario", 291, (Vector2){331, 167}, {359, 163},
                     {366, 173}, {394, 169}, {411, 177}, {409, 201}, {326, 201});
    ADICIONAR_CAMADA(ESCRITORIO, "estante traseira", 286, (Vector2){580, 58}, {777, 57}, {776, 227},
                     {666, 227}, {665, 285}, {582, 285});
    ADICIONAR_CAMADA(ESCRITORIO, "arquivo traseiro direito", 274, (Vector2){1257, 72}, {1329, 74},
                     {1342, 94}, {1337, 271}, {1255, 274});
    ADICIONAR_CAMADA(ESCRITORIO, "livros traseiros direitos", 280, (Vector2){1116, 181},
                     {1181, 179}, {1185, 208}, {1219, 208}, {1245, 228}, {1236, 277}, {1116, 280});


    ADICIONAR_CAMADA(ESCRITORIO, "parede lateral esquerda", 0, (Vector2){171, 5}, {208, 3},
                     {207, 44}, {204, 742}, {184, 760}, {161, 757});
    ADICIONAR_CAMADA(ESCRITORIO, "parede lateral direita", 0, (Vector2){1456, 31}, {1501, 28},
                     {1505, 745}, {1480, 760}, {1469, 749});
    ADICIONAR_CAMADA(ESCRITORIO, "parede frontal esquerda", 0, (Vector2){141, 743}, {181, 741},
                     {185, 759}, {737, 759}, {738, 789}, {185, 800}, {184, 833}, {144, 831},
                     {138, 786});
    ADICIONAR_CAMADA(ESCRITORIO, "batente esquerdo", 0, (Vector2){738, 741}, {766, 742}, {771, 779},
                     {765, 906}, {739, 906});
    ADICIONAR_CAMADA(ESCRITORIO, "parede frontal direita", 0, (Vector2){937, 761}, {1489, 760},
                     {1493, 742}, {1530, 743}, {1537, 789}, {1532, 832}, {1496, 832}, {1488, 799},
                     {937, 790});
    ADICIONAR_CAMADA(ESCRITORIO, "batente direito", 0, (Vector2){908, 741}, {934, 740}, {939, 907},
                     {908, 907}, {904, 778});
    return sucesso;
}


static bool carregarCamadasSalaoBanheiro(const Image *imagensOriginais) {
    bool sucesso = true;
    ADICIONAR_CAMADA(SALAO, "Estante central esquerda", 516, (Vector2){321, 413},
                     (Vector2){331, 383}, (Vector2){345, 257}, (Vector2){374, 254},
                     (Vector2){380, 238}, (Vector2){386, 237}, (Vector2){389, 255},
                     (Vector2){413, 257}, (Vector2){426, 268}, (Vector2){428, 353},
                     (Vector2){413, 503}, (Vector2){409, 516}, (Vector2){325, 514});
    ADICIONAR_CAMADA(SALAO, "Estante central meio esquerda", 520, (Vector2){570, 421},
                     (Vector2){578, 348}, (Vector2){590, 257}, (Vector2){614, 254},
                     (Vector2){616, 239}, (Vector2){623, 237}, (Vector2){627, 257},
                     (Vector2){647, 259}, (Vector2){660, 267}, (Vector2){666, 413},
                     (Vector2){660, 518}, (Vector2){574, 520});
    ADICIONAR_CAMADA(SALAO, "Estante central meio direita", 518, (Vector2){956, 426},
                     (Vector2){956, 269}, (Vector2){968, 257}, (Vector2){992, 255},
                     (Vector2){995, 238}, (Vector2){1001, 239}, (Vector2){1005, 256},
                     (Vector2){1028, 259}, (Vector2){1039, 270}, (Vector2){1046, 425},
                     (Vector2){1046, 516}, (Vector2){958, 518});
    ADICIONAR_CAMADA(SALAO, "Estante central direita", 529, (Vector2){1263, 429},
                     (Vector2){1249, 277}, (Vector2){1254, 260}, (Vector2){1284, 254},
                     (Vector2){1287, 237}, (Vector2){1294, 240}, (Vector2){1298, 257},
                     (Vector2){1324, 264}, (Vector2){1343, 395}, (Vector2){1355, 432},
                     (Vector2){1350, 528}, (Vector2){1264, 528});
    ADICIONAR_CAMADA(SALAO, "Estante baixa frontal", 677, (Vector2){1073, 544},
                     (Vector2){1078, 531}, (Vector2){1092, 531}, (Vector2){1092, 522},
                     (Vector2){1178, 522}, (Vector2){1178, 533}, (Vector2){1192, 535},
                     (Vector2){1200, 569}, (Vector2){1194, 675}, (Vector2){1074, 677});


    ADICIONAR_CAMADA(SALAO, "Balcao horizontal", 649, (Vector2){153, 565}, (Vector2){166, 527},
                     (Vector2){172, 526}, (Vector2){175, 513}, (Vector2){217, 513},
                     (Vector2){220, 528}, (Vector2){434, 527}, (Vector2){432, 575},
                     (Vector2){420, 650}, (Vector2){164, 649});
    ADICIONAR_CAMADA(SALAO, "Balcao vertical", 786, (Vector2){434, 527}, (Vector2){440, 520},
                     (Vector2){451, 521}, (Vector2){456, 527}, (Vector2){483, 528},
                     (Vector2){486, 534}, (Vector2){499, 540}, (Vector2){520, 560},
                     (Vector2){512, 709}, (Vector2){500, 786}, (Vector2){411, 780},
                     (Vector2){405, 718}, (Vector2){420, 650}, (Vector2){432, 575});


    ADICIONAR_CAMADA(SALAO, "Estante fundo esquerda", 275, (Vector2){208, 154}, (Vector2){217, 153},
                     (Vector2){222, 109}, (Vector2){257, 106}, (Vector2){267, 115},
                     (Vector2){275, 103}, (Vector2){300, 99}, (Vector2){311, 107},
                     (Vector2){314, 150}, (Vector2){324, 161}, (Vector2){326, 267},
                     (Vector2){214, 275});
    ADICIONAR_CAMADA(SALAO, "Estante fundo centro", 218, (Vector2){668, 76}, (Vector2){681, 75},
                     (Vector2){683, 64}, (Vector2){707, 64}, (Vector2){714, 77}, (Vector2){851, 77},
                     (Vector2){852, 75}, (Vector2){863, 77}, (Vector2){866, 213},
                     (Vector2){669, 218});
    ADICIONAR_CAMADA(SALAO, "Estante fundo direita", 216, (Vector2){952, 103}, (Vector2){960, 102},
                     (Vector2){964, 103}, (Vector2){1068, 103}, (Vector2){1078, 111},
                     (Vector2){1082, 211}, (Vector2){954, 216});
    ADICIONAR_CAMADA(
        SALAO, "Televisores superiores", 337, (Vector2){1327, 245}, (Vector2){1337, 190},
        (Vector2){1362, 180}, (Vector2){1367, 124}, (Vector2){1394, 114}, (Vector2){1437, 134},
        (Vector2){1439, 188}, (Vector2){1420, 194}, (Vector2){1428, 217}, (Vector2){1420, 256},
        (Vector2){1434, 246}, (Vector2){1464, 263}, (Vector2){1460, 302}, (Vector2){1450, 302},
        (Vector2){1458, 326}, (Vector2){1437, 338}, (Vector2){1407, 329}, (Vector2){1404, 310},
        (Vector2){1394, 318}, (Vector2){1326, 288});


    ADICIONAR_CAMADA(SALAO, "Estante lateral esquerda superior", 0, (Vector2){92, 289},
                     (Vector2){142, 167}, (Vector2){169, 166}, (Vector2){191, 179},
                     (Vector2){201, 282}, (Vector2){170, 371}, (Vector2){155, 396},
                     (Vector2){111, 384}, (Vector2){104, 339});
    ADICIONAR_CAMADA(SALAO, "Estante lateral esquerda inferior", 0, (Vector2){25, 568},
                     (Vector2){73, 566}, (Vector2){84, 582}, (Vector2){98, 667},
                     (Vector2){108, 674}, (Vector2){111, 699}, (Vector2){97, 725},
                     (Vector2){72, 765}, (Vector2){42, 742});
    ADICIONAR_CAMADA(SALAO, "Estante lateral direita superior", 0, (Vector2){1480, 166},
                     (Vector2){1508, 166}, (Vector2){1537, 207}, (Vector2){1584, 292},
                     (Vector2){1566, 370}, (Vector2){1547, 410}, (Vector2){1508, 437},
                     (Vector2){1475, 323});
    ADICIONAR_CAMADA(SALAO, "Estante lateral direita intermediaria", 0, (Vector2){1559, 357},
                     (Vector2){1584, 358}, (Vector2){1626, 430}, (Vector2){1619, 492},
                     (Vector2){1580, 541}, (Vector2){1541, 515}, (Vector2){1514, 438});
    ADICIONAR_CAMADA(SALAO, "Estante lateral direita inferior", 0, (Vector2){1587, 558},
                     (Vector2){1619, 551}, (Vector2){1646, 622}, (Vector2){1644, 733},
                     (Vector2){1609, 772}, (Vector2){1581, 711}, (Vector2){1573, 641});


    ADICIONAR_CAMADA(SALAO, "Parede lateral esquerda superior", 0, (Vector2){15, 266},
                     (Vector2){137, 33}, (Vector2){146, 42}, (Vector2){106, 141},
                     (Vector2){85, 211}, (Vector2){72, 265}, (Vector2){33, 334},
                     (Vector2){24, 366});
    ADICIONAR_CAMADA(SALAO, "Parede lateral esquerda abaixo da porta", 0, (Vector2){18, 568},
                     (Vector2){37, 557}, (Vector2){40, 721}, (Vector2){47, 747},
                     (Vector2){21, 736});
    ADICIONAR_CAMADA(SALAO, "Parede lateral direita", 0, (Vector2){1534, 36}, (Vector2){1656, 266},
                     (Vector2){1655, 736}, (Vector2){1636, 755}, (Vector2){1635, 300},
                     (Vector2){1529, 61});
    ADICIONAR_CAMADA(
        SALAO, "Televisores inferiores", 710, (Vector2){1454, 543}, (Vector2){1473, 533},
        (Vector2){1491, 540}, (Vector2){1519, 544}, (Vector2){1544, 530}, (Vector2){1573, 542},
        (Vector2){1592, 574}, (Vector2){1583, 627}, (Vector2){1567, 643}, (Vector2){1583, 650},
        (Vector2){1574, 704}, (Vector2){1534, 710}, (Vector2){1476, 650}, (Vector2){1455, 590});


    ADICIONAR_CAMADA(SALAO, "Estantes dianteiras esquerdas", 0, (Vector2){105, 774},
                     (Vector2){116, 735}, (Vector2){375, 734}, (Vector2){385, 750},
                     (Vector2){387, 838}, (Vector2){131, 839});
    ADICIONAR_CAMADA(SALAO, "Estantes dianteiras direitas", 0, (Vector2){1042, 760},
                     (Vector2){1050, 756}, (Vector2){1194, 757}, (Vector2){1198, 736},
                     (Vector2){1200, 717}, (Vector2){1231, 712}, (Vector2){1246, 736},
                     (Vector2){1429, 734}, (Vector2){1446, 767}, (Vector2){1442, 838},
                     (Vector2){1044, 840});
    ADICIONAR_CAMADA(SALAO, "Parede frontal esquerda", 0, (Vector2){19, 734}, (Vector2){132, 838},
                     (Vector2){655, 838}, (Vector2){655, 941}, (Vector2){0, 941},
                     (Vector2){0, 755});
    ADICIONAR_CAMADA(SALAO, "Parede frontal direita", 0, (Vector2){1020, 838}, (Vector2){1542, 838},
                     (Vector2){1655, 734}, (Vector2){1672, 755}, (Vector2){1672, 941},
                     (Vector2){1020, 941});
    ADICIONAR_CAMADA(SALAO, "Moldura de saida esquerda", 0, (Vector2){700, 728},
                     (Vector2){738, 728}, (Vector2){740, 875}, (Vector2){709, 879},
                     (Vector2){698, 764});
    ADICIONAR_CAMADA(SALAO, "Moldura de saida direita", 0, (Vector2){938, 728}, (Vector2){975, 729},
                     (Vector2){980, 874}, (Vector2){940, 879});
    ADICIONAR_CAMADA(SALAO, "Travessa superior da saida", 0, (Vector2){701, 727},
                     (Vector2){972, 727}, (Vector2){977, 742}, (Vector2){977, 765},
                     (Vector2){698, 765}, (Vector2){698, 742});


    ADICIONAR_CAMADA(BANHEIRO, "Cabine esquerda", 575, (Vector2){188, 329}, (Vector2){331, 330},
                     (Vector2){350, 207}, (Vector2){359, 146}, (Vector2){372, 145},
                     (Vector2){353, 326}, (Vector2){359, 544}, (Vector2){369, 571},
                     (Vector2){348, 576}, (Vector2){338, 558}, (Vector2){204, 556});
    ADICIONAR_CAMADA(BANHEIRO, "Cabine direita", 576, (Vector2){347, 331}, (Vector2){499, 331},
                     (Vector2){520, 150}, (Vector2){534, 143}, (Vector2){543, 153},
                     (Vector2){531, 325}, (Vector2){533, 573}, (Vector2){515, 577},
                     (Vector2){504, 553}, (Vector2){367, 552});
    ADICIONAR_CAMADA(BANHEIRO, "Bancada da pia", 383, (Vector2){679, 255}, (Vector2){797, 255},
                     (Vector2){800, 260}, (Vector2){836, 260}, (Vector2){838, 230},
                     (Vector2){847, 230}, (Vector2){851, 260}, (Vector2){951, 260},
                     (Vector2){955, 250}, (Vector2){965, 246}, (Vector2){974, 260},
                     (Vector2){1001, 260}, (Vector2){1010, 320}, (Vector2){1006, 333},
                     (Vector2){998, 333}, (Vector2){997, 382}, (Vector2){981, 382},
                     (Vector2){981, 370}, (Vector2){705, 370}, (Vector2){705, 383},
                     (Vector2){689, 383}, (Vector2){687, 333}, (Vector2){678, 331});
    ADICIONAR_CAMADA(BANHEIRO, "Lixeira", 790, (Vector2){155, 694}, (Vector2){163, 674},
                     (Vector2){181, 660}, (Vector2){207, 654}, (Vector2){230, 661},
                     (Vector2){244, 676}, (Vector2){247, 695}, (Vector2){236, 711},
                     (Vector2){235, 779}, (Vector2){218, 791}, (Vector2){178, 789},
                     (Vector2){161, 775});
    ADICIONAR_CAMADA(BANHEIRO, "Balde", 798, (Vector2){1364, 753}, (Vector2){1378, 732},
                     (Vector2){1398, 721}, (Vector2){1430, 731}, (Vector2){1455, 747},
                     (Vector2){1448, 775}, (Vector2){1440, 799}, (Vector2){1380, 801},
                     (Vector2){1364, 776});


    ADICIONAR_CAMADA(BANHEIRO, "Parede lateral esquerda", 0, (Vector2){142, 130},
                     (Vector2){181, 129}, (Vector2){195, 31}, (Vector2){232, 31},
                     (Vector2){211, 166}, (Vector2){184, 174}, (Vector2){161, 377},
                     (Vector2){145, 504}, (Vector2){105, 800}, (Vector2){60, 845});
    ADICIONAR_CAMADA(BANHEIRO, "Parede lateral direita superior", 0, (Vector2){1372, 66},
                     (Vector2){1413, 32}, (Vector2){1427, 130}, (Vector2){1457, 132},
                     (Vector2){1479, 306}, (Vector2){1543, 305}, (Vector2){1548, 350},
                     (Vector2){1437, 349}, (Vector2){1418, 394}, (Vector2){1410, 580},
                     (Vector2){1389, 584}, (Vector2){1355, 334});
    ADICIONAR_CAMADA(BANHEIRO, "Batente direito", 0, (Vector2){1471, 417}, (Vector2){1491, 426},
                     (Vector2){1527, 791}, (Vector2){1566, 800}, (Vector2){1600, 845},
                     (Vector2){1482, 835}, (Vector2){1473, 596});
    ADICIONAR_CAMADA(BANHEIRO, "Batente esquerdo", 0, (Vector2){1417, 391}, (Vector2){1438, 401},
                     (Vector2){1424, 584}, (Vector2){1408, 590});
    ADICIONAR_CAMADA(BANHEIRO, "Parede dianteira", 0, (Vector2){95, 799}, (Vector2){1007, 799},
                     (Vector2){1012, 850}, (Vector2){1166, 850}, (Vector2){1169, 801},
                     (Vector2){1565, 801}, (Vector2){1600, 846}, (Vector2){1214, 846},
                     (Vector2){1214, 895}, (Vector2){962, 895}, (Vector2){964, 846},
                     (Vector2){60, 846});
    return sucesso;
}


static bool carregarCamadasEstoqueProjecao(const Image *imagensOriginais) {
    bool sucesso = true;
    ADICIONAR_CAMADA(
        ESTOQUE, "estante central com lona", 647, (Vector2){520, 278}, (Vector2){527, 261},
        (Vector2){546, 257}, (Vector2){565, 270}, (Vector2){585, 268}, (Vector2){599, 280},
        (Vector2){605, 313}, (Vector2){601, 342}, (Vector2){600, 644}, (Vector2){519, 647},
        (Vector2){516, 476}, (Vector2){519, 353}, (Vector2){511, 336}, (Vector2){514, 300});
    ADICIONAR_CAMADA(
        ESTOQUE, "estante central direita", 617, (Vector2){615, 104}, (Vector2){623, 102},
        (Vector2){624, 111}, (Vector2){692, 109}, (Vector2){696, 101}, (Vector2){703, 102},
        (Vector2){705, 282}, (Vector2){698, 305}, (Vector2){704, 341}, (Vector2){703, 610},
        (Vector2){692, 616}, (Vector2){691, 603}, (Vector2){626, 609}, (Vector2){624, 617},
        (Vector2){615, 617}, (Vector2){614, 535}, (Vector2){610, 530}, (Vector2){614, 516});
    ADICIONAR_CAMADA(ESTOQUE, "lixeira central", 625, (Vector2){616, 552}, (Vector2){632, 550},
                     (Vector2){645, 558}, (Vector2){647, 574}, (Vector2){643, 614},
                     (Vector2){636, 622}, (Vector2){622, 624}, (Vector2){611, 617},
                     (Vector2){608, 572}, (Vector2){608, 562});
    ADICIONAR_CAMADA(ESTOQUE, "estante fundo esquerda", 352, (Vector2){397, 102},
                     (Vector2){407, 102}, (Vector2){408, 111}, (Vector2){526, 110},
                     (Vector2){528, 102}, (Vector2){537, 102}, (Vector2){539, 276},
                     (Vector2){520, 287}, (Vector2){511, 341}, (Vector2){411, 343},
                     (Vector2){411, 352}, (Vector2){401, 351});
    ADICIONAR_CAMADA(ESTOQUE, "pilha entre estantes", 281, (Vector2){550, 153}, (Vector2){593, 153},
                     (Vector2){598, 194}, (Vector2){602, 194}, (Vector2){600, 266},
                     (Vector2){589, 278}, (Vector2){553, 274}, (Vector2){547, 258});
    ADICIONAR_CAMADA(ESTOQUE, "caixas junto a porta esquerda", 260, (Vector2){707, 182},
                     (Vector2){736, 183}, (Vector2){739, 216}, (Vector2){744, 219},
                     (Vector2){744, 247}, (Vector2){732, 259}, (Vector2){707, 257});
    ADICIONAR_CAMADA(ESTOQUE, "caixas junto a porta direita", 263, (Vector2){934, 183},
                     (Vector2){963, 183}, (Vector2){964, 153}, (Vector2){990, 151},
                     (Vector2){992, 126}, (Vector2){1044, 125}, (Vector2){1048, 194},
                     (Vector2){1043, 204}, (Vector2){1042, 251}, (Vector2){1008, 262},
                     (Vector2){970, 257}, (Vector2){970, 250}, (Vector2){934, 252});
    ADICIONAR_CAMADA(ESTOQUE, "lixeira do fundo", 303, (Vector2){1036, 234}, (Vector2){1052, 230},
                     (Vector2){1067, 237}, (Vector2){1071, 248}, (Vector2){1067, 288},
                     (Vector2){1059, 300}, (Vector2){1044, 303}, (Vector2){1032, 293},
                     (Vector2){1029, 247});
    ADICIONAR_CAMADA(ESTOQUE, "estante fundo direita", 370, (Vector2){1088, 102},
                     (Vector2){1096, 102}, (Vector2){1097, 111}, (Vector2){1189, 107},
                     (Vector2){1191, 102}, (Vector2){1200, 103}, (Vector2){1204, 345},
                     (Vector2){1188, 356}, (Vector2){1176, 355}, (Vector2){1175, 366},
                     (Vector2){1145, 370}, (Vector2){1124, 360}, (Vector2){1096, 357},
                     (Vector2){1095, 367}, (Vector2){1087, 367});
    ADICIONAR_CAMADA(ESTOQUE, "bancada de fundo", 364, (Vector2){1209, 99}, (Vector2){1214, 99},
                     (Vector2){1212, 69}, (Vector2){1251, 68}, (Vector2){1251, 100},
                     (Vector2){1307, 100}, (Vector2){1311, 101}, (Vector2){1313, 99},
                     (Vector2){1355, 99}, (Vector2){1361, 107}, (Vector2){1364, 343},
                     (Vector2){1356, 352}, (Vector2){1354, 342}, (Vector2){1218, 342},
                     (Vector2){1218, 363}, (Vector2){1208, 364});


    ADICIONAR_CAMADA(ESTOQUE, "estante lateral esquerda", 0, (Vector2){173, 244},
                     (Vector2){231, 243}, (Vector2){239, 251}, (Vector2){239, 381},
                     (Vector2){247, 390}, (Vector2){248, 512}, (Vector2){241, 548},
                     (Vector2){156, 548}, (Vector2){156, 389}, (Vector2){164, 378},
                     (Vector2){168, 260});
    ADICIONAR_CAMADA(ESTOQUE, "caixas esquerda", 0, (Vector2){163, 512}, (Vector2){194, 512},
                     (Vector2){197, 504}, (Vector2){241, 505}, (Vector2){247, 535},
                     (Vector2){247, 598}, (Vector2){157, 599}, (Vector2){155, 547});
    ADICIONAR_CAMADA(
        ESTOQUE, "lona canto inferior esquerdo", 0, (Vector2){156, 600}, (Vector2){210, 599},
        (Vector2){228, 602}, (Vector2){247, 596}, (Vector2){262, 595}, (Vector2){277, 599},
        (Vector2){291, 595}, (Vector2){305, 605}, (Vector2){325, 605}, (Vector2){333, 625},
        (Vector2){333, 671}, (Vector2){327, 701}, (Vector2){318, 728}, (Vector2){305, 733},
        (Vector2){294, 723}, (Vector2){270, 730}, (Vector2){252, 725}, (Vector2){241, 734},
        (Vector2){211, 727}, (Vector2){211, 706}, (Vector2){153, 703});
    ADICIONAR_CAMADA(ESTOQUE, "caixas inferiores", 0, (Vector2){222, 731}, (Vector2){315, 731},
                     (Vector2){315, 700}, (Vector2){348, 700}, (Vector2){352, 666},
                     (Vector2){405, 667}, (Vector2){409, 677}, (Vector2){465, 672},
                     (Vector2){472, 701}, (Vector2){476, 749}, (Vector2){552, 751},
                     (Vector2){559, 781}, (Vector2){215, 781});
    ADICIONAR_CAMADA(ESTOQUE, "estante lateral direita", 0, (Vector2){1416, 194},
                     (Vector2){1468, 194}, (Vector2){1475, 228}, (Vector2){1492, 239},
                     (Vector2){1494, 283}, (Vector2){1486, 314}, (Vector2){1503, 316},
                     (Vector2){1509, 507}, (Vector2){1499, 547}, (Vector2){1429, 557},
                     (Vector2){1408, 543}, (Vector2){1399, 421}, (Vector2){1393, 327},
                     (Vector2){1391, 261}, (Vector2){1408, 254});
    ADICIONAR_CAMADA(
        ESTOQUE, "caixas e lixeira direita", 0, (Vector2){1331, 278}, (Vector2){1365, 279},
        (Vector2){1376, 289}, (Vector2){1377, 340}, (Vector2){1387, 350}, (Vector2){1393, 389},
        (Vector2){1380, 407}, (Vector2){1387, 419}, (Vector2){1384, 454}, (Vector2){1377, 467},
        (Vector2){1358, 469}, (Vector2){1340, 461}, (Vector2){1336, 443}, (Vector2){1320, 442},
        (Vector2){1313, 414}, (Vector2){1314, 349}, (Vector2){1329, 344});
    ADICIONAR_CAMADA(ESTOQUE, "armario e caixas canto direito", 0, (Vector2){1432, 522},
                     (Vector2){1456, 523}, (Vector2){1459, 502}, (Vector2){1508, 502},
                     (Vector2){1515, 529}, (Vector2){1516, 571}, (Vector2){1505, 579},
                     (Vector2){1518, 598}, (Vector2){1517, 704}, (Vector2){1446, 710},
                     (Vector2){1424, 683}, (Vector2){1423, 582}, (Vector2){1430, 572});
    ADICIONAR_CAMADA(ESTOQUE, "bancada direita tampo", 701, (Vector2){1247, 550},
                     (Vector2){1415, 550}, (Vector2){1417, 587}, (Vector2){1423, 610},
                     (Vector2){1420, 634}, (Vector2){1251, 634}, (Vector2){1247, 619});
    ADICIONAR_CAMADA(ESTOQUE, "bancada direita base", 701, (Vector2){1251, 634},
                     (Vector2){1420, 634}, (Vector2){1416, 696}, (Vector2){1403, 701},
                     (Vector2){1403, 687}, (Vector2){1265, 687}, (Vector2){1265, 701},
                     (Vector2){1253, 701});
    ADICIONAR_CAMADA(ESTOQUE, "parede lateral esquerda", 0, (Vector2){131, 138},
                     (Vector2){234, 138}, (Vector2){234, 155}, (Vector2){151, 157},
                     (Vector2){120, 707}, (Vector2){209, 707}, (Vector2){207, 728},
                     (Vector2){94, 730});
    ADICIONAR_CAMADA(ESTOQUE, "parede lateral direita", 0, (Vector2){1438, 138},
                     (Vector2){1540, 138}, (Vector2){1579, 729}, (Vector2){1463, 730},
                     (Vector2){1463, 710}, (Vector2){1554, 708}, (Vector2){1519, 156},
                     (Vector2){1438, 157});
    ADICIONAR_CAMADA(ESTOQUE, "parede frontal esquerda", 0, (Vector2){202, 780},
                     (Vector2){723, 780}, (Vector2){724, 766}, (Vector2){759, 765},
                     (Vector2){759, 917}, (Vector2){725, 917}, (Vector2){725, 822},
                     (Vector2){176, 806}, (Vector2){179, 727}, (Vector2){205, 727});
    ADICIONAR_CAMADA(ESTOQUE, "parede frontal direita", 0, (Vector2){936, 766}, (Vector2){970, 766},
                     (Vector2){970, 781}, (Vector2){1470, 780}, (Vector2){1466, 729},
                     (Vector2){1495, 729}, (Vector2){1497, 807}, (Vector2){970, 822},
                     (Vector2){968, 917}, (Vector2){936, 917});


    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa central tampo", 407, (Vector2){683, 278},
                     (Vector2){991, 277}, (Vector2){993, 344}, (Vector2){987, 355},
                     (Vector2){681, 355}, (Vector2){678, 340});
    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa central gaveteiro esquerdo", 407, (Vector2){684, 354},
                     (Vector2){748, 354}, (Vector2){748, 395}, (Vector2){690, 399},
                     (Vector2){690, 407}, (Vector2){681, 407});
    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa central gaveteiro direito", 407, (Vector2){923, 353},
                     (Vector2){987, 353}, (Vector2){984, 405}, (Vector2){976, 407},
                     (Vector2){976, 399}, (Vector2){923, 398});
    ADICIONAR_CAMADA(
        SALA_PROJECAO, "projetor central", 407, (Vector2){767, 251}, (Vector2){770, 246},
        (Vector2){771, 192}, (Vector2){781, 190}, (Vector2){785, 221}, (Vector2){797, 221},
        (Vector2){799, 237}, (Vector2){812, 237}, (Vector2){816, 242}, (Vector2){837, 242},
        (Vector2){844, 251}, (Vector2){847, 326}, (Vector2){772, 329}, (Vector2){767, 316});
    ADICIONAR_CAMADA(SALA_PROJECAO, "bobina sobre mesa", 407, (Vector2){706, 270},
                     (Vector2){719, 266}, (Vector2){734, 268}, (Vector2){745, 274},
                     (Vector2){749, 286}, (Vector2){746, 305}, (Vector2){738, 314},
                     (Vector2){717, 318}, (Vector2){701, 313}, (Vector2){696, 299},
                     (Vector2){697, 280});
    ADICIONAR_CAMADA(SALA_PROJECAO, "fitas sobre mesa central", 407, (Vector2){855, 239},
                     (Vector2){883, 239}, (Vector2){887, 250}, (Vector2){885, 260},
                     (Vector2){891, 267}, (Vector2){888, 280}, (Vector2){894, 293},
                     (Vector2){892, 324}, (Vector2){853, 325}, (Vector2){849, 303},
                     (Vector2){853, 286}, (Vector2){848, 276}, (Vector2){853, 261});
    ADICIONAR_CAMADA(SALA_PROJECAO, "maletas sobre mesa central", 407, (Vector2){911, 253},
                     (Vector2){935, 253}, (Vector2){943, 260}, (Vector2){949, 251},
                     (Vector2){973, 251}, (Vector2){982, 263}, (Vector2){978, 298},
                     (Vector2){909, 298});
    ADICIONAR_CAMADA(SALA_PROJECAO, "banco central assento", 431, (Vector2){801, 368},
                     (Vector2){815, 363}, (Vector2){827, 366}, (Vector2){837, 372},
                     (Vector2){837, 382}, (Vector2){830, 389}, (Vector2){810, 392},
                     (Vector2){797, 385}, (Vector2){796, 376});
    ADICIONAR_CAMADA(SALA_PROJECAO, "banco central perna esquerda", 431, (Vector2){805, 390},
                     (Vector2){812, 391}, (Vector2){808, 422}, (Vector2){802, 423});
    ADICIONAR_CAMADA(SALA_PROJECAO, "banco central perna direita", 431, (Vector2){826, 391},
                     (Vector2){832, 389}, (Vector2){835, 422}, (Vector2){831, 423});
    ADICIONAR_CAMADA(SALA_PROJECAO, "banco central perna frontal", 431, (Vector2){816, 392},
                     (Vector2){823, 392}, (Vector2){823, 431}, (Vector2){817, 431});
    ADICIONAR_CAMADA(SALA_PROJECAO, "televisor esquerdo", 397, (Vector2){485, 226},
                     (Vector2){554, 206}, (Vector2){572, 224}, (Vector2){577, 286},
                     (Vector2){586, 295}, (Vector2){587, 373}, (Vector2){491, 399},
                     (Vector2){479, 383}, (Vector2){475, 290}, (Vector2){492, 282});
    ADICIONAR_CAMADA(SALA_PROJECAO, "estante fundo esquerda", 288, (Vector2){351, 120},
                     (Vector2){392, 119}, (Vector2){395, 138}, (Vector2){432, 138},
                     (Vector2){443, 159}, (Vector2){443, 278}, (Vector2){432, 285},
                     (Vector2){429, 278}, (Vector2){347, 280}, (Vector2){343, 270},
                     (Vector2){345, 172}, (Vector2){352, 168});
    ADICIONAR_CAMADA(
        SALA_PROJECAO, "bobina pedestal fundo esquerda", 274, (Vector2){579, 99},
        (Vector2){598, 83}, (Vector2){620, 81}, (Vector2){640, 92}, (Vector2){650, 113},
        (Vector2){651, 136}, (Vector2){642, 151}, (Vector2){633, 157}, (Vector2){637, 181},
        (Vector2){652, 184}, (Vector2){659, 270}, (Vector2){582, 274}, (Vector2){578, 191},
        (Vector2){594, 181}, (Vector2){596, 159}, (Vector2){580, 147}, (Vector2){573, 127});
    ADICIONAR_CAMADA(SALA_PROJECAO, "bobina pedestal fundo direita", 272, (Vector2){1026, 98},
                     (Vector2){1046, 83}, (Vector2){1066, 84}, (Vector2){1083, 96},
                     (Vector2){1093, 115}, (Vector2){1090, 139}, (Vector2){1074, 154},
                     (Vector2){1074, 184}, (Vector2){1090, 189}, (Vector2){1091, 269},
                     (Vector2){1010, 272}, (Vector2){1010, 191}, (Vector2){1036, 181},
                     (Vector2){1037, 157}, (Vector2){1022, 145}, (Vector2){1018, 126});
    ADICIONAR_CAMADA(
        SALA_PROJECAO, "estantes laterais esquerdas", 0, (Vector2){332, 278}, (Vector2){345, 266},
        (Vector2){366, 265}, (Vector2){383, 273}, (Vector2){386, 290}, (Vector2){381, 317},
        (Vector2){390, 331}, (Vector2){388, 420}, (Vector2){407, 428}, (Vector2){450, 435},
        (Vector2){453, 514}, (Vector2){445, 541}, (Vector2){425, 557}, (Vector2){384, 551},
        (Vector2){375, 540}, (Vector2){375, 608}, (Vector2){358, 637}, (Vector2){355, 696},
        (Vector2){344, 705}, (Vector2){290, 705}, (Vector2){284, 688}, (Vector2){288, 568},
        (Vector2){301, 545}, (Vector2){300, 450}, (Vector2){314, 428}, (Vector2){316, 383},
        (Vector2){326, 338});
    ADICIONAR_CAMADA(SALA_PROJECAO, "pilha alta esquerda", 0, (Vector2){409, 276},
                     (Vector2){418, 263}, (Vector2){436, 260}, (Vector2){449, 269},
                     (Vector2){454, 284}, (Vector2){449, 300}, (Vector2){456, 305},
                     (Vector2){455, 331}, (Vector2){459, 342}, (Vector2){459, 394},
                     (Vector2){465, 402}, (Vector2){462, 446}, (Vector2){453, 450},
                     (Vector2){398, 449}, (Vector2){398, 415}, (Vector2){404, 393},
                     (Vector2){403, 354}, (Vector2){409, 323}, (Vector2){404, 305});
    ADICIONAR_CAMADA(SALA_PROJECAO, "caixa frontal esquerda", 0, (Vector2){375, 629},
                     (Vector2){451, 629}, (Vector2){459, 646}, (Vector2){452, 740},
                     (Vector2){439, 754}, (Vector2){360, 742}, (Vector2){356, 682});
    ADICIONAR_CAMADA(
        SALA_PROJECAO, "bobinas inferiores esquerdas", 0, (Vector2){297, 700}, (Vector2){317, 690},
        (Vector2){336, 695}, (Vector2){346, 708}, (Vector2){348, 732}, (Vector2){357, 721},
        (Vector2){370, 713}, (Vector2){387, 716}, (Vector2){397, 730}, (Vector2){400, 744},
        (Vector2){411, 735}, (Vector2){425, 736}, (Vector2){435, 745}, (Vector2){436, 771},
        (Vector2){290, 771}, (Vector2){290, 733}, (Vector2){288, 718});
    ADICIONAR_CAMADA(
        SALA_PROJECAO, "estantes laterais direitas", 0, (Vector2){1271, 201}, (Vector2){1291, 201},
        (Vector2){1308, 212}, (Vector2){1311, 230}, (Vector2){1305, 247}, (Vector2){1319, 250},
        (Vector2){1323, 270}, (Vector2){1331, 289}, (Vector2){1340, 314}, (Vector2){1342, 364},
        (Vector2){1364, 389}, (Vector2){1368, 420}, (Vector2){1394, 448}, (Vector2){1400, 521},
        (Vector2){1410, 557}, (Vector2){1409, 631}, (Vector2){1415, 671}, (Vector2){1410, 737},
        (Vector2){1384, 754}, (Vector2){1313, 749}, (Vector2){1305, 687}, (Vector2){1308, 613},
        (Vector2){1294, 578}, (Vector2){1291, 485}, (Vector2){1299, 441}, (Vector2){1260, 434},
        (Vector2){1245, 413}, (Vector2){1247, 365}, (Vector2){1235, 349}, (Vector2){1242, 316},
        (Vector2){1236, 292}, (Vector2){1243, 269}, (Vector2){1241, 251}, (Vector2){1262, 247},
        (Vector2){1260, 231}, (Vector2){1260, 212});
    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa lateral direita tampo", 571, (Vector2){1125, 426},
                     (Vector2){1266, 425}, (Vector2){1278, 447}, (Vector2){1277, 492},
                     (Vector2){1127, 496}, (Vector2){1121, 480});
    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa lateral direita perna", 571, (Vector2){1127, 495},
                     (Vector2){1141, 495}, (Vector2){1140, 554}, (Vector2){1131, 555});
    ADICIONAR_CAMADA(SALA_PROJECAO, "mesa lateral direita gaveteiro", 571, (Vector2){1204, 493},
                     (Vector2){1276, 493}, (Vector2){1271, 565}, (Vector2){1263, 571},
                     (Vector2){1262, 563}, (Vector2){1198, 565}, (Vector2){1195, 549});
    ADICIONAR_CAMADA(SALA_PROJECAO, "aparelho mesa lateral direita", 571, (Vector2){1203, 384},
                     (Vector2){1232, 382}, (Vector2){1242, 390}, (Vector2){1243, 429},
                     (Vector2){1234, 442}, (Vector2){1203, 440}, (Vector2){1198, 422});
    ADICIONAR_CAMADA(SALA_PROJECAO, "luminaria direita cupula", 571, (Vector2){1167, 381},
                     (Vector2){1175, 381}, (Vector2){1183, 389}, (Vector2){1187, 398},
                     (Vector2){1183, 405}, (Vector2){1166, 402}, (Vector2){1159, 395});
    ADICIONAR_CAMADA(SALA_PROJECAO, "luminaria direita haste", 571, (Vector2){1168, 386},
                     (Vector2){1171, 389}, (Vector2){1147, 411}, (Vector2){1148, 430},
                     (Vector2){1145, 430}, (Vector2){1143, 409});
    ADICIONAR_CAMADA(SALA_PROJECAO, "luminaria direita base", 571, (Vector2){1140, 429},
                     (Vector2){1154, 429}, (Vector2){1160, 435}, (Vector2){1158, 441},
                     (Vector2){1140, 442}, (Vector2){1135, 436});
    ADICIONAR_CAMADA(SALA_PROJECAO, "caixas inferiores direitas", 0, (Vector2){1217, 683},
                     (Vector2){1265, 682}, (Vector2){1268, 675}, (Vector2){1296, 676},
                     (Vector2){1304, 693}, (Vector2){1317, 697}, (Vector2){1319, 730},
                     (Vector2){1327, 737}, (Vector2){1324, 773}, (Vector2){1213, 773},
                     (Vector2){1208, 716});
    ADICIONAR_CAMADA(SALA_PROJECAO, "parede lateral esquerda", 0, (Vector2){245, 159},
                     (Vector2){299, 158}, (Vector2){300, 176}, (Vector2){267, 178},
                     (Vector2){243, 744}, (Vector2){288, 744}, (Vector2){288, 769},
                     (Vector2){217, 765});
    ADICIONAR_CAMADA(SALA_PROJECAO, "parede lateral direita", 0, (Vector2){1344, 158},
                     (Vector2){1425, 158}, (Vector2){1458, 767}, (Vector2){1385, 772},
                     (Vector2){1385, 747}, (Vector2){1433, 744}, (Vector2){1405, 179},
                     (Vector2){1344, 180});
    ADICIONAR_CAMADA(SALA_PROJECAO, "parede frontal esquerda", 0, (Vector2){267, 771},
                     (Vector2){727, 771}, (Vector2){728, 746}, (Vector2){754, 745},
                     (Vector2){754, 869}, (Vector2){276, 869}, (Vector2){265, 807});
    ADICIONAR_CAMADA(SALA_PROJECAO, "parede frontal direita", 0, (Vector2){903, 746},
                     (Vector2){929, 747}, (Vector2){929, 772}, (Vector2){1414, 771},
                     (Vector2){1410, 837}, (Vector2){1399, 868}, (Vector2){903, 869});
    return sucesso;
}


#undef ADICIONAR_CAMADA


bool iniciarCamadas(void) {
    liberarCamadas();
    Image imagensOriginais[NUM_CENARIOS] = {0};
    bool sucesso = true;
    for (int i = 0; i < NUM_CENARIOS; i++) {
        imagensOriginais[i] = LoadImage(salas[i].imagem);
        if (!imagensOriginais[i].data || imagensOriginais[i].width != LARGURA_MUNDO ||
            imagensOriginais[i].height != ALTURA_MUNDO) {
            sucesso = false;
            break;
        }
        ImageFormat(&imagensOriginais[i], PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }
    if (sucesso) {
        sucesso = carregarCamadasEscritorio(imagensOriginais) &&
                  carregarCamadasSalaoBanheiro(imagensOriginais) &&
                  carregarCamadasEstoqueProjecao(imagensOriginais);
    }
    for (int i = 0; i < NUM_CENARIOS; i++)
        if (imagensOriginais[i].data)
            UnloadImage(imagensOriginais[i]);
    if (!sucesso)
        liberarCamadas();
    return sucesso;
}


void desenharJohnComCamadas(Texture2D fundo, Texture2D john, int sala, Vector2 posicao) {
    (void)fundo;
    if (!john.id || john.height <= 0)
        return;
    float largura = ALTURA_JOHN * john.width / john.height;
    Rectangle personagem = {posicao.x - largura / 2, posicao.y - ALTURA_JOHN, largura, ALTURA_JOHN};
    DrawTexturePro(john, (Rectangle){0, 0, john.width, john.height}, personagem, (Vector2){0, 0}, 0,
                   WHITE);


    for (int fixa = 0; fixa < 2; fixa++) {
        for (int i = 0; i < quantidadeCamadas; i++) {
            const Camada *f = &camadas[i];
            if (f->sala != sala || (f->profundidade == 0) != fixa ||
                (f->profundidade > 0 && posicao.y >= f->profundidade))
                continue;
            Rectangle limites = {f->origem.x, f->origem.y, f->textura.width, f->textura.height};
            if (CheckCollisionRecs(personagem, limites))
                DrawTextureV(f->textura, f->origem, WHITE);
        }
    }
}

void desenharPersonagensComCamadas(Texture2D john, Vector2 posicaoJohn,
    Texture2D inimigo, Vector2 posicaoInimigo, int sala) {
    Texture2D imagens[] = {john, inimigo};
    Vector2 posicoes[] = {posicaoJohn, posicaoInimigo};
    if (posicaoJohn.y > posicaoInimigo.y) {
        imagens[0] = inimigo;
        imagens[1] = john;
        posicoes[0] = posicaoInimigo;
        posicoes[1] = posicaoJohn;
    }
    float profundidadeAnterior = -1;
    for (int personagem = 0; personagem < 3; personagem++) {
        float profundidade = personagem < 2 ? posicoes[personagem].y : INFINITY;
        for (int indice = 0; indice < quantidadeCamadas; indice++) {
            const Camada *camada = &camadas[indice];
            if (camada->sala == sala && camada->profundidade > 0 &&
                camada->profundidade > profundidadeAnterior && camada->profundidade <= profundidade)
                DrawTextureV(camada->textura, camada->origem, WHITE);
        }
        if (personagem < 2) {
            Texture2D imagem = imagens[personagem];
            float largura = ALTURA_JOHN * imagem.width / imagem.height;
            Rectangle destino = {posicoes[personagem].x - largura / 2,
                posicoes[personagem].y - ALTURA_JOHN, largura, ALTURA_JOHN};
            DrawTexturePro(imagem, (Rectangle){0, 0, imagem.width, imagem.height},
                destino, (Vector2){0, 0}, 0, WHITE);
        }
        profundidadeAnterior = profundidade;
    }
    for (int indice = 0; indice < quantidadeCamadas; indice++) {
        const Camada *camada = &camadas[indice];
        if (camada->sala == sala && camada->profundidade == 0)
            DrawTextureV(camada->textura, camada->origem, WHITE);
    }
}
