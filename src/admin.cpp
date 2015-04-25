/*
AUTHOR : 

*/
#include "main.h"

/*

>Editer l'appréciation Générale d'une classe
>Supprimer élève
>Modifier un élève
>Backend : 

*/

Frame_admin::Frame_admin(Frame_login* parent,int& matricule,connexion_bdd*& bdd) : Frame_principale(parent,matricule,bdd)
{
	this->SetSize(wxDefaultCoord,wxDefaultCoord,770,625);

	wxPanel *fenetre = new wxPanel(this);

	wxBoxSizer *sizer_principal = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_principal_haut= new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer *sizer_principal_bas= new wxStaticBoxSizer(wxHORIZONTAL,fenetre,_T("Autres : "));
	wxBoxSizer *sizer_gauche= new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_centre=new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_droite=new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer *texte_conteneur_ajout 	= new wxStaticBoxSizer(wxVERTICAL,fenetre,_T("Utilisateurs : "));
	wxStaticBoxSizer *texte_conteneur_arrondi = new wxStaticBoxSizer(wxVERTICAL,fenetre,_T("Arrondi des moyennes : "));
	wxStaticText *texte=new wxStaticText(fenetre, -1, _T("Gestion des utilisateurs\nde GestNotes :\n\n"));
	wxButton *button_ajouter= new wxButton(fenetre, -1, _T("Ajouter"));
	wxButton *button_modifier= new wxButton(fenetre, -1, _T("Modifier"));
	wxButton *button_supprimer= new wxButton(fenetre, -1, _T("Supprimer"));

	input_checkbox__notes_hors_bareme=new wxCheckBox(fenetre,-1,_T("Autoriser les notes hors barème\n(au dessus de 20)\n\n"));
	input_checkbox__afficher_buletins=new wxCheckBox(fenetre,-1,_T("Autoriser l'affichage/impression des buletins"));

	//Arrondi des moyennes (n'affecte pas les calculs)
	wxStaticText  *label_arrondir_moyenne 	= new wxStaticText(fenetre, -1, _T("Arrondir les moyennes sur les buletins\n(n'affecte pas les calculs)\n\n"));
	input_radio__arrondi_cent=new wxRadioButton(fenetre,-1,_T("Arrondi au 1/100° de point"));
	input_radio__arrondi_dix=new wxRadioButton(fenetre,-1,_T("Arrondi au 1/10° de point"));
	input_radio__arrondi_demi=new wxRadioButton(fenetre,-1,_T("Arrondi au demi point"));
	input_radio__arrondi_un=new wxRadioButton(fenetre,-1,_T("Arrondi au point"));

	sizer_principal->Add(sizer_principal_haut,1);
	sizer_principal->Add(sizer_principal_bas,1,wxEXPAND|wxALL,15);

	sizer_principal_haut->Add(sizer_gauche,1,wxTOP |wxCENTER,1);
	sizer_principal_haut->Add(sizer_centre,1,wxTOP|wxALIGN_CENTER,1);
	sizer_principal_haut->Add(sizer_droite,1,wxTOP|wxALIGN_CENTER,1);

	sizer_centre->Add(texte_conteneur_arrondi);

	sizer_gauche->Add(texte_conteneur_ajout,0,wxLEFT,15);
	texte_conteneur_ajout->Add(texte,0, wxALIGN_CENTER_HORIZONTAL,5);
	texte_conteneur_ajout->Add(button_ajouter,0,wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL,5);
	texte_conteneur_ajout->Add(button_modifier,0,wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL,5);
	texte_conteneur_ajout->Add(button_supprimer,0,wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL,5);

	texte_conteneur_arrondi->Add(label_arrondir_moyenne);
	texte_conteneur_arrondi->Add(input_radio__arrondi_cent);
	texte_conteneur_arrondi->Add(input_radio__arrondi_dix);
	texte_conteneur_arrondi->Add(input_radio__arrondi_demi);
	texte_conteneur_arrondi->Add(input_radio__arrondi_un);

	sizer_droite->Add(input_checkbox__notes_hors_bareme,1);
	sizer_droite->Add(input_checkbox__afficher_buletins,1);

	fenetre->SetSizer(sizer_principal);

	button_ajouter->Bind(					wxEVT_BUTTON,		&Frame_admin::onAjouter,	 	  this);
	input_checkbox__afficher_buletins->Bind(wxEVT_CHECKBOX, 	&Frame_admin::onCheck_Buletins,	  this);
	button_modifier->Bind(					wxEVT_BUTTON,		&Frame_admin::onModifier,		  this);
	button_supprimer->Bind(					wxEVT_BUTTON,		&Frame_admin::onSupprimer,		  this);
	input_radio__arrondi_cent->Bind(		wxEVT_RADIOBUTTON,	&Frame_admin::onChange_arrondi,	  this);
	input_radio__arrondi_dix->Bind(			wxEVT_RADIOBUTTON,	&Frame_admin::onChange_arrondi,	  this);
	input_radio__arrondi_demi->Bind(		wxEVT_RADIOBUTTON,	&Frame_admin::onChange_arrondi,	  this);
	input_radio__arrondi_un->Bind(			wxEVT_RADIOBUTTON,	&Frame_admin::onChange_arrondi,	  this);
	input_checkbox__notes_hors_bareme->Bind(wxEVT_CHECKBOX,		&Frame_admin::onClick_hors_bareme,this);
	
	bdd->exec("select * from reglages");
	
	if(bdd->getColumn_int(2)==1)//affichage buletins
	{
		input_checkbox__afficher_buletins->SetValue(true);
		input_checkbox__afficher_buletins->Disable();
	}
	
	if(bdd->getColumn_int(0)==1) input_checkbox__notes_hors_bareme->SetValue(true);//notes_hors_bareme
	
	if(bdd->getColumn_int(1)==100)	 	input_radio__arrondi_cent->SetValue(true);//précision
	else if(bdd->getColumn_int(1)==10)	input_radio__arrondi_dix-> SetValue(true);
	else if(bdd->getColumn_int(1)==2)	input_radio__arrondi_demi->SetValue(true);
	else if(bdd->getColumn_int(1)==1)	input_radio__arrondi_un->  SetValue(true);
	
	this->SetStatusText(_T("GestNotes - Accès Admin"));
	this->Show();

}


void Frame_admin::onAjouter(wxCommandEvent &evenement)
{
	Frame_ajout_modification_membre(this,bdd);
}

void Frame_admin::onCheck_Buletins(wxCommandEvent &evenement)
{
	int reponse=wxMessageBox(_T("Voulez vous vraiment autoriser l'impression des buletins?\nVous ne pourrez pas annuler cette décision"), _T("Confirmer"), wxYES_NO);
    
	if (reponse == wxNO)
	{
		input_checkbox__afficher_buletins->SetValue(false);
		evenement.Skip(false);
		return ;
	}
		
	bdd->exec("update reglages set affichage_buletins=1");
	input_checkbox__afficher_buletins->SetValue(true);
	input_checkbox__afficher_buletins->Disable();
}

void Frame_admin::onSupprimer(wxCommandEvent &evenement)
{
		
}

void Frame_admin::onModifier(wxCommandEvent &evenement)
{
	
}
void Frame_admin::onClick_hors_bareme(wxCommandEvent &evenement)
{
	if(input_checkbox__notes_hors_bareme->IsChecked()) bdd->exec("update reglages set notes_hors_bareme=1");
	else bdd->exec("update reglages set notes_hors_bareme=0");
}

void Frame_admin::onChange_arrondi(wxCommandEvent &evenement)
{
	int valeur=10;
	if(input_radio__arrondi_cent->GetValue())		valeur=100;
	else if(input_radio__arrondi_dix->GetValue())	valeur=10;
	else if(input_radio__arrondi_demi->GetValue())	valeur=2;
	else if(input_radio__arrondi_un->GetValue()) 	valeur=1;
	else wxMessageBox("erreur !");
	
	wxMessageBox("test");
	requete_sql* req=bdd->prepare("update reglages set precision=?;");
	req->bind(1,valeur);
	req->fetch();
	
	req->closeCursor();
	//bdd->exec("COMMIT;");
}
// -------------

Frame_ajout_modification_membre::Frame_ajout_modification_membre(Frame_principale* parent,connexion_bdd*& bdd_private, int matricule,int acces) : wxDialog(parent, wxID_ANY,_T("GestNotes - Ajouter"),wxDefaultPosition,wxSize(670,510))
{
	bdd=bdd_private;

	while(bdd->exec("select nom from matieres")) texte_select.Add(bdd->getColumn_text(0));
	texte_select.Add(_T("<Ajouter Une Matière>"));
	while(bdd->exec("select nom from classes")) texte_classes.Add(bdd->getColumn_text(0));
	texte_classes.Add(_T("<Ajouter Une Classe>"));
	
	wxPanel          *fenetre					= new wxPanel(this,-1);
	wxBoxSizer       *contenu_fenetre_sans_marge= new wxBoxSizer(wxVERTICAL);
	wxBoxSizer       *contenur_radio_ajout		= new wxBoxSizer(wxVERTICAL);
	wxBoxSizer       *contenur_radio_ajout_2	= new wxBoxSizer(wxVERTICAL);
	wxBoxSizer       *contenur_radio_ajout_2_horisontal= new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer		 *conteneur_ajout_horisontal= new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer *texte_conteneur_ajout 	= new wxStaticBoxSizer(wxVERTICAL,fenetre,_T("Ajouter : "));
	wxFlexGridSizer  *conteneur_ajout_gauche	= new wxFlexGridSizer(2,8,5);
	wxFlexGridSizer  *conteneur_ajout_droite	= new wxFlexGridSizer(2,11,5);
	wxStaticLine     *separarion_horisontale 	= new wxStaticLine(fenetre, -1);
	wxStaticLine     *separarion_verticale	 	= new wxStaticLine(fenetre, -1,wxDefaultPosition, wxDefaultSize,wxLI_VERTICAL);

	wxStaticText* texte_ajouter_prof 		= new wxStaticText(fenetre, -1, _T("\nAjouter un professeur, ou un admin : \n"));
	wxStaticText* label_ajouter_matricule 	= new wxStaticText(fenetre, -1, _T("\nLier cette personne à un profil existant : \n"));
	wxStaticText* label_ajout_nom 			= new wxStaticText(fenetre, -1, _T("Nom : "));
	wxStaticText* label_ajout_prenom		= new wxStaticText(fenetre, -1, _T("Prénom : "));
	wxStaticText* label_ajout_mdp 			= new wxStaticText(fenetre, -1, _T("Mot de Passe: "));
	wxStaticText* label_ajout_radio			= new wxStaticText(fenetre, -1, _T("Ajouter un : "));
	label_ajouter_prof__matiere				= new wxStaticText(fenetre, -1, _T("Matière : "));
	label_ajout_eleve__prenom_responsable	= new wxStaticText(fenetre, -1, _T("Prénom du Responsable : "));
	label_ajout_eleve__tel_responsable		= new wxStaticText(fenetre, -1, _T("Tel. du Responsable : "));
	label_ajout_eleve__mail_responsable		= new wxStaticText(fenetre, -1, _T("Email du responsable : "));
	label_ajout_eleve__sexe					= new wxStaticText(fenetre, -1, _T("Sexe : "));

	label_ajout_eleve__nom_rue				= new wxStaticText(fenetre, -1, _T("Rue : "));
	label_ajout_eleve__rue					= new wxStaticText(fenetre, -1, _T("Numero de Rue : "));
	label_ajout_eleve__code_postal			= new wxStaticText(fenetre, -1, _T("Code Postal : "));
	label_ajout_eleve__ville				= new wxStaticText(fenetre, -1, _T("Ville : "));
	label_ajout_eleve__tel_mobile			= new wxStaticText(fenetre, -1, _T("Tel. : "));
	label_ajout_eleve__groupe				= new wxStaticText(fenetre, -1, _T("Groupe : "));
	label_ajout_eleve__nom_responsable 		= new wxStaticText(fenetre, -1, _T("Nom du responsable : "));
	label_ajout_eleve__classe		 		= new wxStaticText(fenetre, -1, _T("Classe : "));
	input_ajout_matricule					= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_nom							= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_prenom						= new wxTextCtrl(fenetre, -1, _T(""));

	input_ajout_mdp							= new wxTextCtrl(fenetre, -1, _T(""),wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
	input_ajout_eleve__prenom_responsable	= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__tel_responsable		= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__mail_responsable		= new wxTextCtrl(fenetre, -1, _T(""));

	input_ajout_eleve__nom_responsable		= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__nom_rue				= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__rue					= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__code_postal			= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__ville				= new wxTextCtrl(fenetre, -1, _T(""));
	input_ajout_eleve__tel_mobile			= new wxTextCtrl(fenetre, -1, _T(""));
	
	wxArrayString strings_sexes;
	wxArrayString strings_groupes;
	strings_sexes.Add(_T("Fille"));
	strings_sexes.Add(_T("Garçon"));
	strings_groupes.Add(_T("Groupe A"));
	strings_groupes.Add(_T("Groupe B"));

	input_ajout_eleve__sexe		= new wxChoice     (fenetre, -1, wxDefaultPosition,wxDefaultSize,strings_sexes);
	input_ajout_eleve__groupe	= new wxChoice     (fenetre, -1, wxDefaultPosition,wxDefaultSize,strings_groupes);

	input_radio_prof  			= new wxRadioButton(fenetre, -1, _T("Professeur"));
	input_radio_admin 			= new wxRadioButton(fenetre, -1, _T("Administrateur"));
	input_radio_eleve 			= new wxRadioButton(fenetre, -1, _T("Élève"));
	input_radio_matricule_oui	= new wxRadioButton(fenetre, -1, _T("Oui"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
	input_radio_matricule_non	= new wxRadioButton(fenetre, -1, _T("Non"));
	input_ajout_eleve__classe	= new wxComboBox   (fenetre, -1, _T("<séléctionner>"), wxDefaultPosition,wxDefaultSize,texte_classes);
	input_select_matiere_ajout	= new wxComboBox   (fenetre, -1, _T("<séléctionner>"), wxDefaultPosition,wxDefaultSize,texte_select);
	bouton_valider_ajout		= new wxButton     (fenetre, -1, _T("Valider"));


	contenu_fenetre_sans_marge->Add(texte_conteneur_ajout,0,  wxALL, 5);

	texte_conteneur_ajout->Add(texte_ajouter_prof,0,wxALIGN_CENTER);
	texte_conteneur_ajout->Add(conteneur_ajout_horisontal);

	conteneur_ajout_horisontal->Add(conteneur_ajout_gauche);
	conteneur_ajout_horisontal->Add(separarion_verticale,0, wxALL | wxEXPAND, 5);
	conteneur_ajout_horisontal->Add(conteneur_ajout_droite);

	conteneur_ajout_gauche->Add(label_ajout_nom);
	conteneur_ajout_gauche->Add(input_ajout_nom);
	conteneur_ajout_gauche->Add(label_ajout_prenom);
	conteneur_ajout_gauche->Add(input_ajout_prenom);
	conteneur_ajout_gauche->Add(label_ajout_mdp);
	conteneur_ajout_gauche->Add(input_ajout_mdp);
	conteneur_ajout_gauche->Add(label_ajout_radio);
	conteneur_ajout_gauche->Add(contenur_radio_ajout);
	conteneur_ajout_gauche->Add(label_ajouter_prof__matiere);
	conteneur_ajout_gauche->Add(input_select_matiere_ajout);
	conteneur_ajout_gauche->Add(label_ajouter_matricule,wxALIGN_CENTER);
	conteneur_ajout_gauche->Add(contenur_radio_ajout_2_horisontal);
	conteneur_ajout_gauche->Add(label_ajout_eleve__classe);
	conteneur_ajout_gauche->Add(input_ajout_eleve__classe);

	contenur_radio_ajout_2_horisontal->Add(contenur_radio_ajout_2);

	texte_conteneur_ajout->Add(separarion_horisontale,0, wxALL | wxEXPAND, 5);
	texte_conteneur_ajout->Add(bouton_valider_ajout,1, wxALIGN_CENTER_HORIZONTAL,5);

	contenur_radio_ajout->Add(input_radio_prof,1);
	contenur_radio_ajout->Add(input_radio_admin,1);
	contenur_radio_ajout->Add(input_radio_eleve,1);
	contenur_radio_ajout_2->Add(input_radio_matricule_non);
	contenur_radio_ajout_2->Add(input_radio_matricule_oui);

	contenur_radio_ajout_2_horisontal->Add(input_ajout_matricule,0,wxALIGN_CENTER);
	input_radio_prof->SetValue(true);
	input_radio_matricule_non->SetValue(true);


	this->onClick_radio(*(new wxCommandEvent()));
	this->onChange_select(*(new wxCommandEvent()));


	conteneur_ajout_droite->Add(label_ajout_eleve__nom_responsable);
	conteneur_ajout_droite->Add(input_ajout_eleve__nom_responsable);
	conteneur_ajout_droite->Add(label_ajout_eleve__prenom_responsable);
	conteneur_ajout_droite->Add(input_ajout_eleve__prenom_responsable);
	conteneur_ajout_droite->Add(label_ajout_eleve__tel_responsable);
	conteneur_ajout_droite->Add(input_ajout_eleve__tel_responsable);
	conteneur_ajout_droite->Add(label_ajout_eleve__mail_responsable);
	conteneur_ajout_droite->Add(input_ajout_eleve__mail_responsable);
	conteneur_ajout_droite->Add(label_ajout_eleve__sexe);
	conteneur_ajout_droite->Add(input_ajout_eleve__sexe);
	conteneur_ajout_droite->Add(label_ajout_eleve__nom_rue);
	conteneur_ajout_droite->Add(input_ajout_eleve__nom_rue);
	conteneur_ajout_droite->Add(label_ajout_eleve__rue);
	conteneur_ajout_droite->Add(input_ajout_eleve__rue);
	conteneur_ajout_droite->Add(label_ajout_eleve__code_postal);
	conteneur_ajout_droite->Add(input_ajout_eleve__code_postal);
	conteneur_ajout_droite->Add(label_ajout_eleve__ville);
	conteneur_ajout_droite->Add(input_ajout_eleve__ville);
	conteneur_ajout_droite->Add(label_ajout_eleve__tel_mobile);
	conteneur_ajout_droite->Add(input_ajout_eleve__tel_mobile);
	conteneur_ajout_droite->Add(label_ajout_eleve__groupe);
	conteneur_ajout_droite->Add(input_ajout_eleve__groupe);

	bouton_valider_ajout->Bind(      wxEVT_BUTTON,		&Frame_ajout_modification_membre::onClick,			this);
	input_select_matiere_ajout->Bind(wxEVT_COMBOBOX,    &Frame_ajout_modification_membre::onChange_select, 	this);
	input_ajout_eleve__classe->Bind( wxEVT_COMBOBOX,    &Frame_ajout_modification_membre::onChange_select, 	this);
	input_radio_prof->Bind(          wxEVT_RADIOBUTTON, &Frame_ajout_modification_membre::onClick_radio, 	this);
	input_radio_eleve->Bind(         wxEVT_RADIOBUTTON, &Frame_ajout_modification_membre::onClick_radio,	this);
	input_radio_admin->Bind(         wxEVT_RADIOBUTTON, &Frame_ajout_modification_membre::onClick_radio,	this);
	input_radio_matricule_oui->Bind( wxEVT_RADIOBUTTON ,&Frame_ajout_modification_membre::onClick_radio,	this);
	input_radio_matricule_non->Bind( wxEVT_RADIOBUTTON ,&Frame_ajout_modification_membre::onClick_radio,	this);


	fenetre->SetSizer(contenu_fenetre_sans_marge);
	this->ShowModal();
}

void Frame_ajout_modification_membre::onClick_radio(wxCommandEvent &evenement)
{
	if(input_radio_matricule_non->GetValue())
	{
		input_ajout_matricule->Disable();

		input_ajout_nom->Enable();
		input_ajout_prenom->Enable();
		input_ajout_mdp->Enable();

		input_radio_admin->Enable();
		input_radio_eleve->Enable();
	}
	else if(input_radio_matricule_oui->GetValue())
	{
		input_ajout_matricule->Enable();

		input_radio_admin->Disable();
		input_radio_eleve->Disable();
		input_radio_prof->SetValue(true);

		input_ajout_nom->Disable();
		input_ajout_prenom->Disable();
		input_ajout_mdp->Disable();
	}

	if(input_radio_eleve->GetValue())
	{
		label_ajout_eleve__nom_responsable->Enable();
		input_ajout_eleve__nom_responsable->Enable();
		input_ajout_eleve__nom_responsable->Disable();
		input_ajout_eleve__prenom_responsable->Enable();
		input_ajout_eleve__tel_responsable->Enable();
		input_ajout_eleve__mail_responsable->Enable();
		input_ajout_eleve__nom_responsable->Enable();
		input_ajout_eleve__nom_rue->Enable();
		input_ajout_eleve__rue->Enable();
		input_ajout_eleve__code_postal->Enable();
		input_ajout_eleve__ville->Enable();
		input_ajout_eleve__tel_mobile->Enable();
		input_ajout_eleve__sexe->Enable();
		input_ajout_eleve__groupe->Enable();
		input_ajout_eleve__classe->Enable();
		
		label_ajout_eleve__classe->Enable();
		label_ajout_eleve__sexe->Enable();
		label_ajout_eleve__groupe->Enable();
		label_ajout_eleve__nom_responsable->Enable();
		label_ajout_eleve__prenom_responsable->Enable();
		label_ajout_eleve__tel_responsable->Enable();
		label_ajout_eleve__mail_responsable->Enable();
		label_ajout_eleve__sexe->Enable();
		label_ajout_eleve__nom_rue->Enable();
		label_ajout_eleve__rue->Enable();
		label_ajout_eleve__code_postal->Enable();
		label_ajout_eleve__ville->Enable();
		label_ajout_eleve__tel_mobile->Enable();
		label_ajout_eleve__groupe->Enable();
		label_ajout_eleve__nom_responsable->Enable();

		input_select_matiere_ajout->Disable();
		label_ajouter_prof__matiere->Disable();
	}
	else
	{
		if(input_radio_prof->GetValue())
		{
			input_select_matiere_ajout->Enable();
			label_ajouter_prof__matiere->Enable();
			input_ajout_eleve__classe->Enable();
			label_ajout_eleve__classe->Enable();
	
		}
		else if(input_radio_admin->GetValue())
		{
			input_select_matiere_ajout->Disable();
			label_ajouter_prof__matiere->Disable();
			input_ajout_eleve__classe->Disable();
			label_ajout_eleve__classe->Disable();
		}

		input_ajout_eleve__nom_responsable->Disable();
		input_ajout_eleve__prenom_responsable->Disable();
		input_ajout_eleve__tel_responsable->Disable();
		input_ajout_eleve__mail_responsable->Disable();
		input_ajout_eleve__nom_responsable->Disable();
		input_ajout_eleve__nom_rue->Disable();
		input_ajout_eleve__rue->Disable();
		input_ajout_eleve__code_postal->Disable();
		input_ajout_eleve__ville->Disable();
		input_ajout_eleve__tel_mobile->Disable();
		input_ajout_eleve__sexe->Disable();
		input_ajout_eleve__groupe->Disable();

		label_ajout_eleve__sexe->Disable();
		label_ajout_eleve__groupe->Disable();
		label_ajout_eleve__nom_responsable->Disable();
		label_ajout_eleve__prenom_responsable->Disable();
		label_ajout_eleve__tel_responsable->Disable();
		label_ajout_eleve__mail_responsable->Disable();
		label_ajout_eleve__sexe->Disable();
		label_ajout_eleve__nom_rue->Disable();
		label_ajout_eleve__rue->Disable();
		label_ajout_eleve__code_postal->Disable();
		label_ajout_eleve__ville->Disable();
		label_ajout_eleve__tel_mobile->Disable();
		label_ajout_eleve__groupe->Disable();
		label_ajout_eleve__nom_responsable->Disable();
	}
}



void Frame_ajout_modification_membre::onChange_select(wxCommandEvent &evenement)
{
	if((unsigned int)input_select_matiere_ajout->GetSelection()==texte_select.GetCount()-1)
	{
		input_select_matiere_ajout->SetEditable(true);
		input_select_matiere_ajout->SetValue("");
	}
	else
	{
		input_select_matiere_ajout->SetEditable(false);
		input_select_matiere_ajout->SetBackgroundColour(0xFFFFFF);
	}
	if((unsigned int)input_ajout_eleve__classe->GetSelection()==texte_classes.GetCount()-1)
	{
		input_ajout_eleve__classe->SetEditable(true);
		input_ajout_eleve__classe->SetValue("");
	}
	else
	{
		input_ajout_eleve__classe->SetEditable(false);
		input_ajout_eleve__classe->SetBackgroundColour(0xFFFFFF);
	}
}

void Frame_ajout_modification_membre::onClick(wxCommandEvent &evenement)
{
	requete_sql *req=NULL;
	int type_ajout=-1;
	int matricule=0;

	if(input_radio_prof->GetValue())		type_ajout=PROF;// si c'est un prof
	else if(input_radio_admin->GetValue()) 	type_ajout=ADMIN; // un admin
	else if(input_radio_eleve->GetValue()) 	type_ajout=ELEVE;//un éleve
	
	//si ((les 3 inputs principaux sont ok && id non existant) || (id existant&& id_non_vide))
	//&& (admin || ((prof || eleve) && le combobox classe est ok))
	//&& (prof && le combobox matière est ok)
	//|| (eleve && 
	
	if( ((!(input_ajout_mdp->IsEmpty() || input_ajout_nom->IsEmpty() ||input_ajout_prenom->IsEmpty()) && input_radio_matricule_non->GetValue()) || (input_radio_matricule_oui->GetValue() && !input_ajout_matricule->IsEmpty())) && 
	
	(((type_ajout==PROF || type_ajout==ELEVE) && (unsigned int)input_ajout_eleve__classe->GetSelection()!=(texte_classes.GetCount()-1) && (input_ajout_eleve__classe->GetSelection()!=wxNOT_FOUND || (input_ajout_eleve__classe->GetValue().Cmp(_T("<séléctionner>"))!=0 && input_ajout_eleve__classe->GetValue().Cmp(_T(""))!=0))) ||type_ajout==ADMIN) &&
	( (type_ajout==PROF && 
		((unsigned int)input_select_matiere_ajout->GetSelection()!=(texte_select.GetCount()-1) && (input_select_matiere_ajout->GetSelection()!=wxNOT_FOUND || (input_select_matiere_ajout->GetValue().Cmp(_T("<séléctionner>"))!=0 && input_select_matiere_ajout->GetValue().Cmp(_T(""))!=0)))) ||
	  (type_ajout==ELEVE && 
		(!input_ajout_eleve__nom_responsable->IsEmpty() && input_ajout_eleve__groupe->GetSelection()!=wxNOT_FOUND && input_ajout_eleve__sexe->GetSelection()!=wxNOT_FOUND && !input_ajout_eleve__nom_rue->IsEmpty() && !input_ajout_eleve__code_postal->IsEmpty() && !input_ajout_eleve__ville->IsEmpty() && !input_ajout_eleve__tel_mobile->IsEmpty() && !input_ajout_eleve__nom_responsable->IsEmpty() && ! input_ajout_eleve__prenom_responsable->IsEmpty() && !input_ajout_eleve__tel_responsable->IsEmpty() && !input_ajout_eleve__mail_responsable->IsEmpty())) ||
		type_ajout==ADMIN
	))
		
	{// si tout les champs sont OK
	
		if(input_radio_matricule_oui->GetValue())// si on demande a associer la professeur a un matricule existant
		{
			req=bdd->prepare("SELECT count(*) as nbr, type from login_centralise where matricule=:matricule");
			req->bind(":matricule",string(input_ajout_matricule->GetValue().mb_str()));
			req->fetch();

			if(req->getColumn_int(0)!=1)
			{
				req->closeCursor();
				wxMessageBox(_T("Erreur ! Ce matricule n'existe pas"));
				return ;
			}

			if(req->getColumn_int(1)!=type_ajout)
			{
				req->closeCursor();
				wxMessageBox(_T("Erreur ! Impossible d'ajouter quelqu'un qui n'est pas du même type (prof, admin, élêve)"));
				return ;
			}

			matricule=wxAtoi(input_ajout_matricule->GetValue());
			req->closeCursor();

			req=bdd->prepare("select nom, prenom from profs where id=?");
			req->bind(1,matricule);
			req->fetch();
			input_ajout_nom->SetValue(req->getColumn_text(0));
			input_ajout_prenom->SetValue(req->getColumn_text(1));

			req->closeCursor();
		}
		else
		{
			req=bdd->prepare("INSERT INTO `login_centralise` (mdp, type) VALUES(:mdp,:type)");
			req->bind(":mdp",string(input_ajout_mdp->GetValue().mb_str()));
			req->bind(":type",type_ajout);

			req->fetch();
			req->closeCursor();

			bdd->exec("select last_insert_rowid() AS ligne FROM login_centralise limit 1");
			matricule=bdd->getColumn_int(0);
		}
		
		//si c'est un prof ou un éleve : on doit lui affecter une classe
		if( (type_ajout==PROF || type_ajout==ELEVE) && input_ajout_eleve__classe->GetSelection()==wxNOT_FOUND)//si on as une nouvelle classe : on l'ajoute en BDD ! :p
		{																									//l'id d'une classe est input_ajout_eleve__classe.GetSelection()-1
			req=bdd->prepare("insert into classes (nom)  values(:nom_classe)");
			req->bind(":nom_classe", string(input_ajout_eleve__classe->GetValue().mb_str()));
			req->fetch();
			req->closeCursor();
				
			texte_classes.Insert(input_ajout_eleve__classe->GetValue(), texte_classes.GetCount()-1);
				
			input_ajout_eleve__classe->Clear();
			input_ajout_eleve__classe->Append(texte_classes);
			input_ajout_eleve__classe->SetSelection(texte_classes.GetCount()-2);
			this->onChange_select(*(new wxCommandEvent()));
		}
		
		if(type_ajout==PROF)
		{
			if(input_select_matiere_ajout->GetSelection()==wxNOT_FOUND)//si on as une nouvelle matière : on l'ajoute en BDD ! :p
			{//l'id de la matiere est texte_select.GetSelection()-1
				req=bdd->prepare("insert into matieres (nom) values (:nom_matiere)");
				req->bind(":nom_matiere", string(input_select_matiere_ajout->GetValue().mb_str()));
				req->fetch();
				req->closeCursor();
				
				texte_select.Insert(input_select_matiere_ajout->GetValue(), texte_select.GetCount()-1);
				
				input_select_matiere_ajout->Clear();
				input_select_matiere_ajout->Append(texte_select);
				input_select_matiere_ajout->SetSelection(texte_select.GetCount()-2);
				this->onChange_select(*(new wxCommandEvent()));

			}

			req=bdd->prepare("insert into profs values (:matricule,:nom,:prenom,:matiere,:classe)");
			req->bind(":matiere",input_select_matiere_ajout->GetSelection()-1);
			req->bind(":classe",input_ajout_eleve__classe->GetSelection()-1);

		}
		else if(type_ajout==ELEVE)
		{
			req=bdd->prepare("insert into eleve values (:matricule,:prenom,:nom,:classe,:groupe,:sexe,:date_inscription,:rue,:num_rue,:code_postal,:ville,:tel_mobile,:nom_responsable,:prenom_responsable:tel_responsable,:mail_responsable)");
			req->bind(":groupe",input_ajout_eleve__groupe->GetSelection());
			req->bind(":sexe",  input_ajout_eleve__sexe->  GetSelection());
			req->bind(":date_inscription",time(NULL));

			req->bind(":nom_rue",string(input_ajout_eleve__nom_rue->GetValue().mb_str()));
			req->bind(":code_postal",wxAtoi(input_ajout_eleve__code_postal->GetValue()));
			req->bind(":classe",input_ajout_eleve__classe->GetSelection()-1);
			req->bind(":ville",string(input_ajout_eleve__ville->GetValue().mb_str()));
			req->bind(":tel_mobile",wxAtoi(input_ajout_eleve__tel_mobile->GetValue()));
			req->bind(":nom_responsable",string(input_ajout_eleve__nom_responsable->GetValue().mb_str()));
			req->bind(":prenom_responsable",string(input_ajout_eleve__prenom_responsable->GetValue().mb_str()));
			req->bind(":adresse_responsable",string(input_ajout_eleve__tel_responsable->GetValue().mb_str()));
			req->bind(":mail_responsable",string(input_ajout_eleve__mail_responsable->GetValue().mb_str()));
		}
		else if(type_ajout==ADMIN)  req=bdd->prepare("insert into admin values (:matricule,:nom,:prenom)");
	}
	else
	{
		wxMessageBox("Erreur ! Avez vous rempli tout les champs?");//tout n'est pas ok
		return ;
	}
	
	
	req->bind(":matricule",matricule);
	req->bind(":nom",string(input_ajout_nom->GetValue().mb_str()));
	req->bind(":prenom",string(input_ajout_prenom->GetValue().mb_str()));
	req->fetch();
	req->closeCursor();

	wxString fin;
	if(input_radio_matricule_non->GetValue())
		fin<<_T("Le profil a été créé. Son matricule est le : ")<<matricule;
	else
		fin<<_T("Le proffesseur a été affecté au matricule numéro ")<<matricule<<_(".");
	wxMessageBox(fin,_T("Succès"));
	this->Close();
}
