#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//Taille des lignes et colonnes du terrain
#define TAILLE 10
//Nombre de bateaux à placer pour chaque joueur
#define NBRBATMAX 5
//Taille maximale d'un bateau (forcément supérieur ou égal au nombre maximal de bateaux
#define TAILLEBATMAX 5

//Création d'un type pour le statut des cases et des bateaux
enum STATE{

    Vide,Occupee,Touchee,Entiere,Endommage,Coule

};

//Création du type cellule, représentant le plateau de jeu
typedef struct Cellule{

    int ligne;
    int colonne;
    enum STATE etat;

}Cellule;

//Création du type bateau contenant plusieurs cellules pour leur position
typedef struct Bateau{

    Cellule tabCellule[TAILLE];
    int tailleBateau;
    enum STATE etat;

}Bateau;

//Création du type flotte, contenant des bateaux et le nom du joueur
typedef struct Flotte{

    Bateau tabBateau[TAILLE];
    char nomJoueur[30];
    enum STATE etat;
    int tour;
    Cellule coups[TAILLE*TAILLE];

}Flotte;

//Création d'un type booléen
enum BOOL{

    Vrai,Faux,

};


//Définition des procédures et fonctions
enum BOOL compareCellules(Cellule case1, Cellule case2);
enum BOOL verificationBat(Cellule casePlat, Bateau boat);
enum BOOL verifOrient(int increm,char choixOrient,Cellule choixCase,Flotte flotteJ, int tailleB);
enum STATE verificationFlotte(Flotte flotteJ, Cellule target);

Flotte majEtat(Flotte joueurVise, Cellule caseVisee);
Flotte jeuEnCours(Flotte joueurTir, Flotte joueurCible,int nbrCells);
Flotte initJeu();
Flotte verifEtat(Flotte joueurTouche, int nbrCells);

Bateau creationBateau(int ligne, int colonne, int taille, char orientation);

Cellule creationCellule(int ligne,int colonne);
Cellule saisieCoord(int i);

void afficheFlotte(Flotte joueur);
void afficheEtat(enum STATE etat);
void gestionPartie(int nbrCells);
void afficheCoups(Flotte joueur);
//



int main()
//BUT : gérer une partie de bataille navale
//ENTREE : fonctions et procédures gérant les choix des joueurs
//SORTIE : déroulement de la partie jusqu'à ce qu'un des joueurs ait perdu tous ses bateaux
{
    int i,nbrCells;

    nbrCells=0;

    //Calcul du nombre de cases utilisées par les bateaux, défini par la valeur de NBRBATMAX et TAILLEBATMAX
    for(i=0;i<NBRBATMAX;i++){
        nbrCells=nbrCells+(TAILLEBATMAX-i);
    }

    //Appel de la procédure gérant le déroulement de la partie
    gestionPartie(nbrCells);

    return 0;
}

void gestionPartie(int nbrCells)
//BUT : Gérer le lancement et la fin de la partie
//ENTREE : Flotte des joueurs
//SORTIE : Annonce du gagnant
{
    int numGagnant;
    Flotte joueur1;
    Flotte joueur2;
    enum STATE vaincu;

    vaincu=0;

    //Prise des noms des joueurs
    printf("Bonjour, entrez le nom des deux joueurs.\n");
    fgets(joueur1.nomJoueur,30,stdin);
    fgets(joueur2.nomJoueur,30,stdin);

    //On désigne par défaut le joueur2 comme vainqueur
    numGagnant=2;

    //Création de leurs flottes
    joueur1=initJeu(joueur1);
    joueur2=initJeu(joueur2);

    //Tant que les deux joueurs ont encore des bateaux
    while (vaincu!=5){

        //Le premier joueur joue
        joueur2=jeuEnCours(joueur1,joueur2,nbrCells);
        vaincu=joueur2.etat;

        //Si le joueur a perdu tous ses bateaux, la partie s'arrête
        if(vaincu==5){
            //On met à jour le numéro du vainqueur
            numGagnant=1;
            break;
        }

        //Au tour du joueur suivant
        joueur1=jeuEnCours(joueur2,joueur1,nbrCells);
        vaincu=joueur1.etat;

    }

    //On efface ce qu'il y a à l'écran
    system("cls");

    //On annonce la fin de partie et le vainqueur
    if(numGagnant=1){
        printf("Partie finie, bravo %s",joueur1.nomJoueur);
    }else{
        printf("Partie finie, bravo %s",joueur2.nomJoueur);
    }

}

Flotte jeuEnCours(Flotte joueurTir, Flotte joueurCible, int nbrCells)
//BUT : Gérer les choix des joueurs et leurs conséquences
//ENTREE : Cases choisies par les joueurs
//SORTIE : Modification des flottes
{
    char ready;
    Cellule caseVisee;

    //On nettoie le buffer
    fflush(stdin);

    //Tant que le joueur n'indique pas qu'il est prêt en appuyant sur la bonne touche, on lui redemande
    do{
        system("cls");
        printf("%s Appuyez sur la touche p puis entree quand vous etes prets a jouer.",joueurTir.nomJoueur);
        scanf("%c",&ready);
    }while(ready!='p');

    //Une fois qu'il est prêt, on affiche sa flotte et ses tirs précédents
    printf("Votre flotte :\n");
    afficheFlotte(joueurTir);
    afficheCoups(joueurTir);

    //On lui demande quelle case il veut viser et on en récupère les coordonnées
    printf("\nQuelle case voulez-vous viser chez votre adversaire ? Entrez la ligne, puis la colonne :\n");
    scanf("%i",&caseVisee.ligne);
    scanf("%i",&caseVisee.colonne);

    //On sauvegarde les coordonnées du tir
    joueurTir.coups[joueurTir.tour].ligne=caseVisee.ligne;
    joueurTir.coups[joueurTir.tour].colonne=caseVisee.colonne;

    //On regarde si la case choisie était occupée par un bateau adverse
    if(verificationFlotte(joueurCible,caseVisee)==Occupee){

        //Si oui, on l'indique au joueur...
        printf("Touche !\n");
        //... on met à jour la flotte du joueur touché...
        joueurCible=majEtat(joueurCible,caseVisee);
        joueurCible=verifEtat(joueurCible, nbrCells);
        //... et on sauvegarde le résultat son tir
        joueurTir.coups[joueurTir.tour].etat=2;

    }else if(verificationFlotte(joueurCible,caseVisee)==Vide){

        //Si non, on indique au joueur qu'il n'a rien touché...
        printf("A l\'eau.\n");
        //... et on sauvegarde l'état de la case visée
        joueurTir.coups[joueurTir.tour].etat=0;

    }

    //Le nombre de tours joués s'incrémente
    joueurTir.tour=joueurTir.tour+1;

    //On demande au joueur d'indiquer quand il est prêt à laisser le tour au suivant
    printf("Appuyez sur k et entree pour passer au tour de %s",joueurCible.nomJoueur);
    while(ready!='k'){
            scanf("%c",&ready);
    }

    //Tout s'est bien passé, on renvoie la flotte mise à jour
    return joueurCible;

};

Flotte verifEtat(Flotte joueurTouche, int nbrCells)
//BUT : Vérifier si les joueurs ont encore des bateaux non coulés
//ENTREE : Flotte du joueur ciblé en dernier
//SORTIE : Vérification de l'état de la flotte
{
    int i,j,counterB;

    counterB=0;

    for(i=0;i<NBRBATMAX;i++){

        //On vérifie l'état de chaque bateau
        for(j=0;j<joueurTouche.tabBateau[i].tailleBateau;j++){

            //On vérifie combien de fois chaque bateau a été touché
            if(joueurTouche.tabBateau[i].tabCellule[j].etat==2){
                counterB++;
            }
        }
    }

    //On compare le nombre de cases de bateaux touchées avec le nombre de cellules occupées par les bateaux
    if(counterB==nbrCells){
        //Si tous les bateaux sont coulés, on indique que la flotte est coulée
        joueurTouche.etat=5;
    }else if(counterB<nbrCells){
        //Si il y a encore des bateaux non coulés, on indique que la flotte est endommagée
        joueurTouche.etat=4;
    }else if(counterB==0){
        //Si aucun bateau n'est touché, la flotte est entière
        joueurTouche.etat=3;
    }

    //On renvoie la flotte mise à jour
    return joueurTouche;
};

void afficheCoups(Flotte joueur)
//BUT : Afficher les précédents tirs du joueur
//ENTREE : Coordonnées des cases visées et leur état
//SORTIE : affichage des coups du joueur
{
    int i;

    for(i=0;i<joueur.tour;i++){
        printf("\n Voici vos choix precedents : \n");
        printf("%i : %i = ",joueur.coups[i].ligne,joueur.coups[i].colonne);
        afficheEtat(joueur.coups[i].etat);
        printf("\n");
    }
}

void afficheFlotte(Flotte joueur)
//BUT : Afficher la position des bateaux du joueur et leur état
//ENTREE : Flotte du joueur actuel
//SORTIE : Affichage de la flotte
{
    int i,j;

    for(i=0;i<NBRBATMAX;i++){
        for(j=0;j<(TAILLEBATMAX-i);j++){

            //On écrit les coordonnées des cases occupées par les bateaux du joueur
            printf("Coordonnees bateau numero %i = %i : %i ",i+1,joueur.tabBateau[i].tabCellule[j].ligne, joueur.tabBateau[i].tabCellule[j].colonne);

            //Puis l'état de ces cases
            printf(" Etat du bateau : ");
            afficheEtat(joueur.tabBateau[i].tabCellule[j].etat);
            printf("\n");
        }
    }
}


void afficheEtat(enum STATE etat)
//BUT : Afficher l'état du bateau
//ENTREE : Flotte du joueur
//SORTIE : Etat de la flotte
{
    switch(etat){
    case 0:
        printf("Vide");
        break;
    case 1:
        printf("Occupee");
        break;
    case 2:
        printf("Touche");
        break;
    case 3:
        printf("Entier");
        break;
    case 4:
        printf("Endommage");
        break;
    case 5:
        printf("Coule");
        break;
    }
}

Cellule creationCellule(int ligne,int colonne)
//BUT : définir les cases occupées
//ENTREE : valeur de la ligne et colonne où le bateau est posé
//SORTIE : ligne et colonne de la cellule, initialisées
{
    Cellule caseOccupee;
    caseOccupee.colonne=colonne;
    caseOccupee.ligne=ligne;
    caseOccupee.etat=Occupee;

    return caseOccupee;
}

enum BOOL compareCellules(Cellule case1, Cellule case2)
//BUT : comparer deux cases
//ENTREE : deux cases du plateau de jeu
//SORTIE : vrai si les cases sont identiques, faux si elles sont différentes
{
    if( (case1.ligne == case2.ligne) && (case1.colonne == case2.colonne) ){

        return Vrai;

    }

    return Faux;
}

Bateau creationBateau(int ligne, int colonne, int taille, char orientation)
//BUT : créer un bateau
//ENTREE : valeurs du bateau à placer
//SORTIE : bateau positionné sur le plateau
{
    int i;
    Bateau boat;
    boat.tailleBateau=taille;

    for(i=0;i<taille;i++){
        switch (orientation)
        {
            case 'g':
                boat.tabCellule[i] = creationCellule(ligne, colonne - i);
                break;
            case 'd':
                boat.tabCellule[i] = creationCellule(ligne, colonne + i);
                break;
            case 'h':
                boat.tabCellule[i] = creationCellule(ligne - i, colonne);
                break;
            case 'b':
                boat.tabCellule[i] = creationCellule(ligne + i, colonne);
                break;
        }
    }

    return boat;
}

enum BOOL verificationBat(Cellule casePlat, Bateau boat)
//BUT : vérifier qu'une case appartient à un bateau
//ENTREE : cellule du plateau de jeu
//SORTIR : indique si la cellule est occupée ou non par un bateau
{
    int i;

    for(i=0;i<TAILLEBATMAX;i++){

        if(compareCellules(casePlat,boat.tabCellule[i])==Vrai){

            return Vrai;

        }
    }

    return Faux;
}

Flotte majEtat(Flotte joueurVise, Cellule caseVisee)
{
    int i,j;

    for(i=0;i<TAILLE;i++){
        for(j=0;j<TAILLE;j++){
            if(joueurVise.tabBateau[i].tabCellule[j].ligne==caseVisee.ligne){
               if(joueurVise.tabBateau[i].tabCellule[j].colonne==caseVisee.colonne){
                    joueurVise.tabBateau[i].tabCellule[j].etat=2;
               }
            }
        }
    }
    return joueurVise;
};

enum STATE verificationFlotte(Flotte flotteJ, Cellule target)
//BUT : vérifier qu'une case est occupée par un bateau d'une flotte en particulier
//ENTREE : nom du joueur visé, case visée
//SORTIE : indique si la case était occupée ou non par le joueur
{
    int i;

    for(i=0;i<TAILLE;i++){
        if(verificationBat(target,flotteJ.tabBateau[i])==Vrai){

            return Occupee;

        }
    }

    return Vide;
}

Flotte initJeu(Flotte initFlotte)
//BUT : permettre aux joueurs de choisir où poser leurs bateaux
//ENTREE : choix des joueurs : ligne, colonne, orientation, taille
//SORTIE : jeu initialisé et prêt pour la partie
{
    int i;
    Cellule placement;
    char orientation;
    enum BOOL ok;

    ok=Vrai;

    system("cls");

    //Le joueur n'a pas encore joué
    initFlotte.tour=0;

    //Indication du début de constitution de la flotte
    printf("=> %s Positionnez vos %i bateaux du plus grand au plus petit.\n",initFlotte.nomJoueur,NBRBATMAX);

    //On demande au joueur de choisir où place ses bateaux
    for(i=0;i<(NBRBATMAX);i++){

        //On nettoie le buffer des saisies au clavier
        fflush (stdin) ;

        //On redemande les coordonnées du bateau tant qu'il n'est pas bien positionné
        do{
            //On redemande la ligne et la colonne choisies tant qu'elles ne rentrent pas dans le tableau
            do{

                printf("Entrez la ligne de votre bateau %i svp (de 0 a %i) :\n",i+1,TAILLE-1);
                scanf("%i", &placement.ligne);
                printf("Entrez la colonne svp (de 0 a %i) :\n",TAILLE-1);
                scanf("%i", &placement.colonne);

            }while((placement.ligne>=TAILLE)||(placement.colonne>=TAILLE));

            //On demande dans quel sens le joueur veut placer son bateau tant qu'il ne saisit pas un caractère valide
            do{
                //On nettoie le buffer des saisies au clavier
                fflush (stdin) ;

                //On demande au joueur de choisir un sens dans lequel placer son bateau
                printf("Choisissez la direction de votre bateau, d pour droite, h pour haut, g pour gauche, b pour bas :\n");
                scanf("%c", &orientation);

                //On vérifie la validité de la saisie du joueur
                if((orientation=='d')||(orientation=='g')||(orientation=='h')||(orientation=='b')){
                    ok = Vrai;
                }else{
                    ok=Faux;
                }

            }while(ok==Faux);

            //On efface ce qui est affiché à l'écran pour plus de lisibilité
            system("cls");

        //On vérifie que l'orientation choisie ne fait sortir le bateau du terrain ou en chevaucher un autre
        }while(verifOrient(NBRBATMAX,orientation,placement,initFlotte,(TAILLEBATMAX-i))==Faux);

        //Création du bateau
        initFlotte.tabBateau[i]=creationBateau(placement.ligne, placement.colonne, TAILLEBATMAX-i, orientation);

    }

    //Tout s'est bien passé, la flotte du joueur est créée
    return initFlotte;


}

enum BOOL verifOrient(int increm,char choixOrient,Cellule choixCase,Flotte flotteJ, int tailleB)
//BUT : Vérifier que le bateau est bien positionnable dans le sens que le joueur a choisi
//ENTREE : Orientation du bateau choisie par le joueur
//SORTIE : Indique si l'orientation est correcte
{
    int i,j;
    enum BOOL valide;
    valide = Vrai;

    //On vérifie que le bateau peut être placé sur la case choisie et dans le sens voulu
    for(i=0;i<increm;i++){
            switch (choixOrient)
            {
                //Vers le haut
                case 'h' :
                    for(j=0;j<tailleB;j++){

                        //La case est déjà occupée ou le bateau sort du plateau de jeu : le joueur doit changer de choix
                        if((valide==compareCellules(choixCase, flotteJ.tabBateau[i].tabCellule[j]))||((choixCase.ligne)<0)){
                            printf("Position impossible, choisissez d'autres coordonnees svp.\n");
                            return Faux;
                            break;
                        }

                        choixCase.ligne--;
                    }

                //Vers le bas
                case 'b' :
                    for(j=0;j<tailleB;j++){

                        //La case est déjà occupée ou le bateau sort du plateau de jeu : le joueur doit changer de choix
                        if((valide==compareCellules(choixCase, flotteJ.tabBateau[i].tabCellule[j]))||((choixCase.ligne)>TAILLE)){
                            printf("Position impossible, choisissez d'autres coordonnees svp.\n");
                            return Faux;
                            break;
                        }

                        choixCase.ligne++;
                    }

                //Vers la gauche
                case 'g' :
                    for(j=0;j<tailleB;j++){

                        //La case est déjà occupée ou le bateau sort du plateau de jeu : le joueur doit changer de choix
                        if((valide==compareCellules(choixCase, flotteJ.tabBateau[i].tabCellule[j]))||((choixCase.colonne)<0)){
                            printf("Position impossible, choisissez d'autres coordonnees svp.\n");
                            return Faux;
                            break;
                        }

                        choixCase.colonne--;
                    }

                //Vers la droite
                case 'd' :
                    for(j=0;j<tailleB;j++){

                        //La case est déjà occupée ou le bateau sort du plateau de jeu : le joueur doit changer de choix
                        if((valide==compareCellules(choixCase, flotteJ.tabBateau[i].tabCellule[j]))||((choixCase.colonne)>TAILLE)){
                            printf("Position impossible, choisissez d'autres coordonnees svp.\n");
                            return Faux;
                            break;
                        }

                        choixCase.colonne++;
                    }
            }
    }

    return valide;
};


