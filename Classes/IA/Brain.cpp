#include "stdafx.h"
#include "Brain.h"

//Variables Globales

//Variable d'environnement

//Variables de gain
int energie = 1;
int gainBijoux = 3;
int poussiere = 4;
int coutBijoux = -10;

//Liste des actions atoiques
std::queue<int> listAction;

//Variable de scoring
int cout = 0;
int sore = 0;
int gainMin = 0;

//variables temporelles pour exploration
int tempsExplore = 60;
int tempsCompteur = 0;

//Variable terminaison nettoyage
bool ramassageFini = true;


Brain::Brain()
{
}


Brain::~Brain()
{
}

bool executeAction(std::queue<int> listAction) {
	//TODO Remplir les cas du switch case
	if (listAction.empty()) {
		return false;
	}
	else {
		switch (listAction.front()) //Ne pas oublier le calcul de l'energie dans les actions
		{
		case 0 :
				//Deplacement vers le haut
				break;
		case 1 :
				//Deplacement vers droite
				break;
		case 2 :
				//Deplacement vers la bas
				break;
		case 3 :
				//Deplacement vers la gauche
				break;
		case 4 :
				//Aspirer
				break;
		case 5 :
				//Ramasser
				break; 
		default:
			break;
		}
		listAction.pop();
		return true;
	}

}

int estimationGain(std::queue<int> listAction) {	//TODO fonction d'acces au nombre de bijoux sur une case
	
	//Pile qui sert de containers intermediaire pour le calcul
	std::queue<int> mirror;
	//Variable de retour
	int gainEstimer = 0;

	//Boucle de calcul
	while (!listAction.empty()) {
		mirror.push(listAction.front());
		switch (listAction.front()) //Ne pas oublier le calcul de l'energie dans les actions
		{
		case 0:
			//Deplacement vers le haut
			gainEstimer -= energie;
			break;
		case 1:
			//Deplacement vers droite
			gainEstimer -= energie;
			break;
		case 2:
			//Deplacement vers la bas
			gainEstimer -= energie;
			break;
		case 3:
			//Deplacement vers la gauche
			gainEstimer -= energie;
			break;
		case 4:
			//Aspirer
			gainEstimer -= energie;
			gainEstimer += poussiere;
			break;
		case 5:
			//Ramasser
			gainEstimer -= energie;
			//gainEstimer += gainBijoux *
			break;
		default:
			break;
		}
		listAction.pop();
	}

	//Remettre les elements dans la pile
	while (!mirror.empty()) { listAction.push(mirror.front());mirror.pop(); }

	return gainEstimer;
}

void IA()
{
	for (;;) {
		if (tempsCompteur < tempsExplore) {
			if (ramassageFini) {
				tempsCompteur++;
			}
			else {
				if (executeAction(listAction) == false) {
					ramassageFini = true;
				}
			}
		}
		else {
			//explorer();
			cout += energie;
			//listAction = calculListAction(environnement);
			int estGain = estimationGain(listAction);

				if (estGain <= 0) {
					tempsExplore += tempsExplore;
					ramassageFini = true;
					//Vide la liste d'action
					while (!listAction.empty()) { listAction.pop(); }
				}
				else {
					if (tempsExplore > 1) {
						tempsExplore--;
					}
					ramassageFini = false;
				}
		}
	}
}