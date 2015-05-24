#include "main.h"

/*
todo : editer les groupes
calculer moyenne eleve. Si <10 prévenir prof responsable

*/


Frame_prof::Frame_prof(Frame_login* parent,int& matricule,connexion_bdd*& bdd) : Frame_principale(parent,matricule,bdd)
{
	
	std::map<int,wxString>::iterator it_matieres_classes;
	
	this->preparer_matieres_classes();
	
	wxArrayString texte_choix_matieres,texte_choix_classes;
	
	for(it_matieres_classes=choix_matieres.begin();it_matieres_classes!=choix_matieres.end();it_matieres_classes++)
		texte_choix_matieres.Add(it_matieres_classes->second);
	
	for(it_matieres_classes=choix_classes.begin();it_matieres_classes!=choix_classes.end();it_matieres_classes++)
		texte_choix_classes.Add(it_matieres_classes->second);
	
	
	bdd->exec("SELECT * FROM reglages");
	notes_hors_bareme=(bdd->getColumn_int(0)==1)?false:true;//on récupère si on a le droit aux notes hors barème
	arrondi_affichage_notes=bdd->getColumn_int(1);
	
	
	this->SetSize(wxDefaultCoord,wxDefaultCoord,770,625);
   
	wxPanel *fenetre = new wxPanel(this);
    wxBoxSizer *sizer_principal = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer *conteneur_choix_matiere_classe = new wxStaticBoxSizer(wxHORIZONTAL,fenetre,_T("Choisir la matière et la classe : "));
	wxStaticBoxSizer *conteneur_notes 	             = new wxStaticBoxSizer(wxHORIZONTAL,fenetre,_T("Notes : "));
	
	liste_matieres	= new wxChoice (fenetre, -1, wxDefaultPosition,wxDefaultSize,texte_choix_matieres);
	liste_classes	= new wxChoice (fenetre, -1, wxDefaultPosition,wxDefaultSize,texte_choix_classes);
	
	wxStaticText *texte_header=new wxStaticText(fenetre, -1, _T("Gestion des notes")); // boite de choix pour les notes
	
	liste_notes=new wxDataViewListCtrl(fenetre,-1, wxDefaultPosition, wxDefaultSize);
	
	liste_matieres->SetSelection(0);
	liste_classes->SetSelection(0);
	
	liste_notes->AppendTextColumn(_T("Élève"),  	 wxDATAVIEW_CELL_INERT	,wxCOL_WIDTH_AUTOSIZE);
	liste_notes->AppendTextColumn(_T("CE"),			 wxDATAVIEW_CELL_EDITABLE);// 
	liste_notes->AppendTextColumn(_T("TAI"),		 wxDATAVIEW_CELL_EDITABLE);
	liste_notes->AppendTextColumn(_T("Projet"),		 wxDATAVIEW_CELL_EDITABLE);
	liste_notes->AppendTextColumn(_T("DE"),			 wxDATAVIEW_CELL_EDITABLE);
	liste_notes->AppendTextColumn(_T("Commentaires"),wxDATAVIEW_CELL_EDITABLE);
			
	this->afficher_liste();
	
	liste_notes->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, 	&Frame_prof::onDbclick_notes,this);// lorsqu'on double clique sur les notes
	liste_notes->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,&Frame_prof::onChange_notes,this);// lorsque la valeur est changée
	liste_matieres->Bind(wxEVT_CHOICE,					&Frame_prof::onChange_matiere,this);
	liste_classes->Bind(wxEVT_CHOICE,					&Frame_prof::onChange_classe,this);
	
	sizer_principal->Add(texte_header,0,wxALIGN_CENTER);
	sizer_principal->Add(conteneur_choix_matiere_classe,0,wxALIGN_CENTER);
	sizer_principal->Add(conteneur_notes,1,wxALIGN_CENTER|wxEXPAND,0);
	
	conteneur_choix_matiere_classe->Add(liste_matieres,1,wxALIGN_CENTER|wxALL,5);
	conteneur_choix_matiere_classe->Add(liste_classes,1,wxALIGN_CENTER|wxALL,5);
	
	conteneur_notes->Add(liste_notes,1,wxALIGN_CENTER|wxEXPAND);
	
	
	fenetre->SetSizer(sizer_principal);

	this->SetStatusText(_T("GestNotes - Accès Professeur"));
	this->Show();

	//liste déroulante en haut.->matière

			//saisir notes
			//changer groupe(NON)
			//si accord de l'admin : changer les notes
			//ecrire des commentaires pour les bulletins
}



void Frame_prof::onChange_notes(wxDataViewEvent &evenement)
{
	if(evenement.GetColumn()==-1 || evenement.GetColumn()==0) return ;
		
	int id_eleve;
	bool pas_trouve=true;
	wxString note;
	double note_double, ancienne_note=-1;
	requete_sql *req;
	
	std::string texte_req;
	std::map<int,int>::iterator it=liste_eleves.begin();
	
	while(pas_trouve && it != liste_eleves.end()) 
	{
		if(it->second == liste_notes->GetSelectedRow()) pas_trouve=false;//on récupère l'id de l'élève en cours a partir de la ligne séléctionnée
		else it++;// (on parcourt la map des élèves pour retrouver l'id)
	}
	
	if(pas_trouve==true) return;//si il y a une erreur interne au programme (c'est pas censé arriver, mais bon...)
	
	id_eleve=it->first;
	
	if(evenement.GetColumn()==5) return this->onChange_commentaires(id_eleve);
	

	note=liste_notes->GetTextValue(liste_notes->GetSelectedRow(),evenement.GetColumn());//et on récupère la note modifiée à partir de la ligne/colonne en cours.
	note.Replace(',','.');
	
	req=bdd->prepare("SELECT note FROM notes WHERE id_eleve=:id_eleve AND id_matiere=:id_matiere AND type_note=:type_note");
	req->bind(":id_matiere",id_matiere_en_cours);
	req->bind(":id_eleve",id_eleve);
	req->bind(":type_note",evenement.GetColumn());//on regarde si cette note existe déja
	
	if(req->fetch()) ancienne_note=req->getColumn_float(0);
	
	req->closeCursor();
	
	if(!note.IsEmpty())
	{		
		if(note.ToDouble(&note_double) && note_double>=0.0 && (!notes_hors_bareme || note_double<20.0))//on vérifie que on peux ajouter la note.
		{
			texte_req=(ancienne_note!=-1)?// si c'est pas le cas : on l'ajoute;sinon on la crée.
			"UPDATE notes SET note=:note WHERE id_eleve=:id_eleve AND id_matiere=:id_matiere AND type_note=:type_note":
			"INSERT INTO notes VALUES (:id_eleve,:id_matiere,:note,:type_note)";
		
			req=bdd->prepare(texte_req);
			req->bind(":id_matiere",id_matiere_en_cours);
			req->bind(":id_eleve",id_eleve);
			req->bind(":type_note",evenement.GetColumn());
			req->bind(":note",note_double);
			req->fetch();
			req->closeCursor();
			
			if(fabs(arrondi(arrondi_affichage_notes,note_double)-note_double)>0.001)// si la nouvelle note est trop précise : on la "rogne" a l'arrondi
				liste_notes->SetTextValue(wxString::Format("%g",arrondi(arrondi_affichage_notes,note_double)),liste_notes->GetSelectedRow(),evenement.GetColumn());
		}
		else
		{
			wxMessageBox(_T("Erreur ! la note n'est pas valide !"),_T("Erreur"));//si la note est invalide : on remet la valeur précédente (si il y en avait une)
			if(ancienne_note==-1) liste_notes->SetTextValue("",								  					 liste_notes->GetSelectedRow(),evenement.GetColumn());
			else liste_notes->SetTextValue(wxString::Format("%g",arrondi(arrondi_affichage_notes,ancienne_note)),liste_notes->GetSelectedRow(),evenement.GetColumn());
		}
		//toDO ->calculer moyenne eleve. Si <10 prévenir prof responsable
	}
	else 
	{
		if(ancienne_note!=-1)
		{
			req=bdd->prepare("DELETE FROM notes WHERE id_eleve=:id_eleve AND id_matiere=:id_matiere AND type_note=:type_note");
			req->bind(":id_matiere",id_matiere_en_cours);//si une note existait : on la dégage
			req->bind(":id_eleve",id_eleve);
			req->bind(":type_note",evenement.GetColumn());
			req->fetch();
			req->closeCursor();
		}
	}
	
}

void Frame_prof::onChange_commentaires(int id_eleve)
{
	std::string texte_req;
	std::string commentaire=std::string(liste_notes->GetTextValue(liste_notes->GetSelectedRow(),5));
	
	requete_sql *req=bdd->prepare("SELECT count(*) FROM commentaires WHERE id_eleve=:id_eleve AND id_matiere=:id_matiere");
	req->bind(":id_eleve",id_eleve);
	req->bind(":id_matiere",id_matiere_en_cours);
	req->fetch();
	
	texte_req=(req->getColumn_int(0)==1)?// si c'est pas le cas : on l'ajoute;sinon on la crée.
		"UPDATE commentaires SET commentaire=:commentaire WHERE id_eleve=:id_eleve AND id_matiere=:id_matiere":
		"INSERT INTO commentaires VALUES (:id_matiere,:id_eleve,:commentaire)";
	req->closeCursor();
	
	
	req=bdd->prepare(texte_req);
	req->bind(":commentaire",commentaire);
	req->bind(":id_eleve",id_eleve);
	req->bind(":id_matiere",id_matiere_en_cours);
	req->fetch();
	req->closeCursor();
}


void Frame_prof::onDbclick_notes(wxDataViewEvent &evenement)
{
	if(evenement.GetColumn()==-1 || evenement.GetColumn()==0) return ;
		
	wxDataViewItem item=liste_notes->GetCurrentItem();
	wxDataViewColumn *colonne=evenement.GetDataViewColumn();
	
	liste_notes->EditItem(item,colonne);
}


void Frame_prof::preparer_matieres_classes()
{
	requete_sql *req=NULL;
	id_matiere_en_cours=-1;
	std::map<int,wxString>::iterator it;
	
	
	//on parcourt la liste des matières
	req=bdd->prepare("SELECT matieres.nom, matieres.id_matiere FROM profs JOIN matieres ON matieres.id_matiere=profs.matiere WHERE profs.id=:matricule");
	req->bind(":matricule",matricule);
	
	while(req->fetch())
	{	
		it=choix_matieres.find(req->getColumn_int(1));//getColumn_int(1) : l'id de la matière en cour
		
		if(it== choix_matieres.end())  choix_matieres[req->getColumn_int(1)]=wxString(req->getColumn_text(0));
	}
	req->closeCursor();
	
	it = choix_matieres.begin();
	id_matiere_en_cours=it->first;
	
	//On séléctionne les classes correspondant à la matière.
	req=bdd->prepare("SELECT classes.nom, classes.id FROM profs JOIN classes ON classes.id=profs.classe WHERE profs.id=:matricule AND profs.matiere=:matiere");
	req->bind(":matricule",matricule);
	req->bind(":matiere",id_matiere_en_cours);
	
	while(req->fetch())
	{
		it=choix_classes.find(req->getColumn_int(1));//getColumn_int(1) : l'id de la matière en cour
		
		if(it== choix_classes.end())  choix_classes[req->getColumn_int(1)]=wxString(req->getColumn_text(0));
	}
	it = choix_classes.begin();
	id_classe_en_cours=it->first;
	
	req->closeCursor(); 
	
	id_classe_en_cours=0;
	id_matiere_en_cours=0;
}

void Frame_prof::onChange_matiere(wxCommandEvent &evenement)
{
	if(liste_matieres->GetSelection()==wxNOT_FOUND)
		return;

	int i=0,nouvelle_matiere=-1;
	std::map<int,wxString>::iterator it;
	requete_sql* req;
	
	for(it=choix_matieres.begin();it!=choix_matieres.end();it++)
	{
		if(i==liste_matieres->GetSelection())
		{
			if(it->first==id_matiere_en_cours)
				return;
			
			nouvelle_matiere=it->first;
		}
		i++;
	}
	
	if(nouvelle_matiere!=-1)
	{
		choix_classes.clear();
		//On séléctionne les classes correspondant à la matière.
		req=bdd->prepare("SELECT classes.nom, classes.id FROM profs JOIN classes ON classes.id=profs.classe WHERE profs.id=:matricule AND profs.matiere=:matiere");
		req->bind(":matricule",matricule);
		req->bind(":matiere",nouvelle_matiere);
		
		
		while(req->fetch())
		{
			it=choix_classes.find(req->getColumn_int(1));//getColumn_int(1) : l'id de la matière en cour
			
			if(it== choix_classes.end())
			{
				choix_classes[req->getColumn_int(1)]=wxString(req->getColumn_text(0));
			}
		}
		it = choix_classes.begin();
		id_classe_en_cours=it->first;
		
		req->closeCursor(); 
		
		//------------------------------------------------
		liste_classes->Clear();
		
		for(it=choix_classes.begin();it!=choix_classes.end();it++)
			liste_classes->Append(it->second);
		
		liste_classes->SetSelection(0);
	
		it=choix_classes.begin();
		
		id_matiere_en_cours=nouvelle_matiere;
		id_classe_en_cours=it->first;
		
		liste_notes->DeleteAllItems();
		liste_eleves.clear();
		this->afficher_liste();

	}
}


void Frame_prof::onChange_classe(wxCommandEvent &evenement)
{
	if(liste_classes->GetSelection()==wxNOT_FOUND)
		return;

	int i=0,nouvelle_classe=-1;
	std::map<int,wxString>::iterator it;
	
	for(it=choix_classes.begin();it!=choix_classes.end();it++)
	{
		if(i==liste_classes->GetSelection())
		{
			if(it->first==id_classe_en_cours)
				return;
			
			nouvelle_classe=it->first;
		}
		i++;
	}
	id_classe_en_cours=nouvelle_classe;
	
	liste_notes->DeleteAllItems();
	liste_eleves.clear();
	this->afficher_liste();
}

void Frame_prof::afficher_liste()
{
	requete_sql *req;
	
	req=bdd->prepare("SELECT eleves.id,eleves.nom,eleves.prenom, 0 AS pas_de_note, notes.note,notes.type_note FROM eleves	\
															LEFT OUTER JOIN notes ON notes.id_eleve=eleves.id 				\
							WHERE notes.id_matiere=:matiere	AND eleves.classe=:classe									\
					UNION																									\
					 SELECT eleves.id,eleves.nom,eleves.prenom, 1 AS pas_de_note, notes.note,notes.type_note FROM eleves  	\
															LEFT OUTER JOIN notes ON notes.id_eleve=eleves.id 				\
							WHERE eleves.classe=:classe AND notes.note IS null");
	
	req->bind(":classe",id_classe_en_cours);
	req->bind(":matiere",id_matiere_en_cours);
	
	std::map<int,int>::iterator it;
	wxVector<wxVariant> ligne;//wxWidgets à besoin d'un vecteur pour créer une ligne. Ce vecteurs doit contenir les champs des colonnes de cette ligne
	
	int position_y;
	wxString texte_note;
	
	for(int i=0;i<6;i++) ligne.push_back(wxVariant(""));//on crée une ligne avec que des colonnes vides
    
	while(req->fetch())
	{
		texte_note="";
		
		it=liste_eleves.find(req->getColumn_int(0));//si la valeur n'est pas trouvée, it sera = a liste_eleves.end()
		if(it== liste_eleves.end())//if(l'id de l'éleve n'est pas dans le vector) vector[id] = la_position_ou_on_ajoute_cette_matiere
		{
			liste_eleves[req->getColumn_int(0)]=liste_eleves.size()-1;
			
			ligne[0]=wxVariant(string(req->getColumn_text(2))+" "+string(req->getColumn_text(1)));//on effectue l'ajout de la ligne
			liste_notes->AppendItem(ligne);	
		}
		
		if(req->getColumn_int(3)==0)//si il y a une note a afficher
		{
			position_y=req->getColumn_int(5);
			texte_note<<arrondi(arrondi_affichage_notes,req->getColumn_float(4));
				
			liste_notes->SetTextValue(texte_note,liste_eleves[req->getColumn_int(0)],position_y);
			//if(il y a une note a afficher) ON affiche la note en (position_x/liste_matiere; position_y/le_type_de_note)
		}
	}
	req->closeCursor();
	
	
	//puis on s'occupe des commentaires :
	req=bdd->prepare("SELECT commentaires.id_eleve,commentaires.commentaire FROM commentaires JOIN eleves ON eleves.id=commentaires.id_eleve WHERE commentaires.id_matiere=:matiere AND eleves.classe=:classe");
	req->bind(":matiere",id_matiere_en_cours);
	req->bind(":classe",id_classe_en_cours);
	
	while(req->fetch())//on affiche tout les commentaires correspondant au prof en cours/matière en cours
	{
		it=liste_eleves.find(req->getColumn_int(0));
		
		if(it!=liste_eleves.end())//pour éviter une erreur interne au programme (normalement ca peux pas arriver, mais bon...)
		{
			liste_notes->SetTextValue(req->getColumn_text(1),liste_eleves[req->getColumn_int(0)],5);
		}
	}
	req->closeCursor();
	
}
