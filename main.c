#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VSTUPNI_SOUBOR "jizerska50.txt"
#define VYSTUPNI_SOUBOR "vysledkova_listina.txt"
#define POCET_RADKU 150
#define POCET_ZNAKU 100
#define DEL ",;:()\n"

typedef struct {
    int h;
    int m;
    int s;
} CAS;

typedef struct {
    int st_cislo;
    char prijmeni[15];
    char jmeno[20];
    int rocnik;
    char narodnost[5];
    CAS cas;
} ZAVODNIK;

int nacti_zavodniky(ZAVODNIK *zavodnik) {
    int index_zavodnika = 0;
    FILE *soubor = fopen(VSTUPNI_SOUBOR, "r");
    if (soubor == NULL) {
        printf("Chyba: Nepodařilo se otevřít soubor %s\n", VSTUPNI_SOUBOR);
        return 0;
    } else {
        char radek[POCET_ZNAKU];
        char *cast_radku;
        while (fgets(radek, sizeof(radek), soubor) != NULL) {
            if (index_zavodnika != 0) {   // preskoceni prvniho radku zahlavi
                int sloupec = 0;
                cast_radku = strtok(radek, DEL);

                while (cast_radku != NULL) {
                    switch (sloupec) {
                        case 0:
                            zavodnik[index_zavodnika - 1].st_cislo = atoi(cast_radku);
                            break;
                        case 1:
                            strcpy(zavodnik[index_zavodnika - 1].prijmeni, cast_radku);
                            break;
                        case 2:
                            strcpy(zavodnik[index_zavodnika - 1].jmeno, cast_radku);
                            break;
                        case 3:
                            zavodnik[index_zavodnika - 1].rocnik = atoi(cast_radku);
                            break;
                        case 4:
                            strcpy(zavodnik[index_zavodnika - 1].narodnost, cast_radku);
                            break;
                        case 5:
                            zavodnik[index_zavodnika - 1].cas.h = atoi(cast_radku);
                            break;
                        case 6:
                            zavodnik[index_zavodnika - 1].cas.m = atoi(cast_radku);
                            break;
                        case 7:
                            zavodnik[index_zavodnika - 1].cas.s = atoi(cast_radku);
                            break;
                    }
                    cast_radku = strtok(NULL, DEL);
                    sloupec++;
                }
            }
            index_zavodnika++; // posunuti na dalsi radek
        }
        if (fclose(soubor) == EOF) {
            printf("Chyba: Soubor %s se nepodařilo zavřít\n", VSTUPNI_SOUBOR);
        }
    }
    return index_zavodnika - 1;  // pocet zavodniku bez prvniho radku
}

int rok(){
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    return timeinfo->tm_year + 1900;
}

int cas_na_sekundy(CAS cas){
    return cas.h * 3600 + cas.m * 60 + cas.s;
}

CAS sekundy_na_cas(int celkove_sekundy){
    CAS vysledny_cas;
    vysledny_cas.h = celkove_sekundy / 3600;
    vysledny_cas.m = (celkove_sekundy % 3600) / 60;
    vysledny_cas.s = celkove_sekundy % 60;
    return vysledny_cas;
}

void serad_zavodniky(ZAVODNIK *zavodnik, int pocet_zavodniku) {
    for (int i = 0; i < pocet_zavodniku - 1; i++) {
        for (int j = 0; j < pocet_zavodniku - i - 1; j++) {
            int cas1 = cas_na_sekundy(zavodnik[j].cas);
            int cas2 = cas_na_sekundy(zavodnik[j + 1].cas);
            if (cas1 > cas2) {
                ZAVODNIK temp = zavodnik[j];
                zavodnik[j] = zavodnik[j + 1];
                zavodnik[j + 1] = temp;
            }
        }
    }
}

void vypis_startovni_listinu(ZAVODNIK *zavodnik, int pocet_zavodniku) {
    printf("STARTOVNI LISTINA - JIZERSKA 50\n\n");
    printf("startovni cislo      prijmeni          jmeno  rocnik stat\n");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < pocet_zavodniku; i++) {
        printf("%15d %13s %15s %6d %4s\n",
               zavodnik[i].st_cislo,
               zavodnik[i].prijmeni,
               zavodnik[i].jmeno,
               zavodnik[i].rocnik,
               zavodnik[i].narodnost);
    }
    printf("---------------------------------------------------------\n");

    int pocet_cechu = 0;
    for (int i = 0; i < pocet_zavodniku; i++) {
        if (strcmp(zavodnik[i].narodnost, "CZE") == 0) {
            pocet_cechu++;
        }
    }
    int index_nejmladsiho = 0;
    for (int i = 0; i < pocet_zavodniku; ++i) {
        if (zavodnik[i].rocnik > zavodnik[index_nejmladsiho].rocnik){
            index_nejmladsiho = i;
        }
    }
    int vek = rok() - zavodnik[index_nejmladsiho].rocnik;

    printf("Pocet zavodniku: %d \n", pocet_zavodniku);
    printf("Pocet cechu: %d \n", pocet_cechu);
    printf("Nejmladsim zavodnikem je %s %s z %s. Je mu %d let.\n", zavodnik[index_nejmladsiho].jmeno,
           zavodnik[index_nejmladsiho].prijmeni, zavodnik[index_nejmladsiho].narodnost, vek);
}

void zapis_vysledky(ZAVODNIK *zavodnik, int pocet_zavodniku) {
    FILE *soubor = fopen(VYSTUPNI_SOUBOR, "w");
    if (soubor == NULL) {
        printf("Chyba: Nepodařilo se vytvořit soubor %s\n", VYSTUPNI_SOUBOR);
        return;
    }
    fprintf(soubor, "VYSLEDKOVA LISTINA - JIZERSKA 50\n\n");
    fprintf(soubor, "poradi cislo    prijmeni          jmeno  rocnik stat     cas ztrata\n");
    fprintf(soubor, "-------------------------------------------------------------------\n");

    int vitezny_cas_sekundy = cas_na_sekundy(zavodnik[0].cas);

    for (int i = 0; i < pocet_zavodniku; i++) {
        int aktualni_sekundy = cas_na_sekundy(zavodnik[i].cas);
        int ztrata = aktualni_sekundy - vitezny_cas_sekundy;
        CAS ztrata_cas = sekundy_na_cas(ztrata);

        fprintf(soubor, "%3d. %3d %13s %15s %6d %4s   %2d:%02d:%02d",
                i + 1,
                zavodnik[i].st_cislo,
                zavodnik[i].prijmeni,
                zavodnik[i].jmeno,
                zavodnik[i].rocnik,
                zavodnik[i].narodnost,
                zavodnik[i].cas.h,
                zavodnik[i].cas.m,
                zavodnik[i].cas.s);

        if (i == 0) {
            fprintf(soubor, "\n");
        } else {
            fprintf(soubor, " +%d:%02d:%02d\n", ztrata_cas.h, ztrata_cas.m, ztrata_cas.s);
        }
    }
    fprintf(soubor, "-------------------------------------------------------------------\n");

    if (fclose(soubor) != 0) {
        printf("Chyba: Soubor %s se nezavrel\n", VYSTUPNI_SOUBOR);
    }
}

int main() {
    ZAVODNIK zavodnik[POCET_RADKU];
    int pocet = nacti_zavodniky(zavodnik);
    vypis_startovni_listinu(zavodnik, pocet);
    serad_zavodniky(zavodnik, pocet);
    zapis_vysledky(zavodnik, pocet);

    return 0;
}
