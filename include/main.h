#ifndef HEADERS_INCLUS

	#define HEADERS_INCLUS

	#include <wx/wx.h>
	#include <wx/statline.h>
	#include <wx/stdpaths.h>
	#include <wx/hyperlink.h>
	#include <wx/filename.h>
	#include <iostream>

	//#include <ctime>

	#include <SQLite/sqlite3.h>


	using namespace std;
	
	class requete_sql;
	class connexion_bdd;
	class App_GestNotes;
	
	class Frame_principale;
	class Frame_admin;
	class Frame_admin_ajouter;
	class Frame_login;

	enum
	{
		PAS_CONNECTE=-1,
		ELEVE=0,
		PROF=1,
		ADMIN=2,
	};

	class requete_sql : public wxString //connexion a la BDD
	{
		public:
			requete_sql(sqlite3 *&bdd,const string& texte);
			int bind(const string &cle,const string &valeur);
			int bind(const string &cle,int valeur);
			int bind(int cle,int valeur);
			int bind(int cle,const string &valeur);
			int fetch();
			int getColumn_int(int numero);
			string getColumn_text(int numero);
			double getColumn_float(int numero);
			void closeCursor();


		private:
			sqlite3_stmt* requete;
			sqlite3* bdd_private;
			int* types;
			int nb_colonnes;

	};

	class connexion_bdd : public wxString
	{
		private:
			sqlite3* bdd;
			string requete_precedente;
			requete_sql* req;

		public:
			connexion_bdd();
			~connexion_bdd();
			int exec(const string &texte);
			requete_sql* prepare(const string &texte);
			int getColumn_int(int numero);
			string getColumn_text(int numero);
			double getColumn_float(int numero);
			void close();

	};

	class App_GestNotes : public wxApp
	{
		public :
			virtual bool OnInit();
			virtual int OnExit();
		protected:
			connexion_bdd *bdd;
	};
	DECLARE_APP(App_GestNotes);

	class Frame_principale: public wxFrame
	{
		protected:
			connexion_bdd* bdd;
			int matricule;
			Frame_login *parent;
			
		public:
			Frame_principale(Frame_login *parent,int &matricule,connexion_bdd*& bdd);
			virtual void onClose(wxCloseEvent &evenement);
			virtual void onQuit(wxCommandEvent &evenement);
			virtual void onAbout(wxCommandEvent &evenement);
	};

	class Frame_admin_ajouter : public wxDialog
	{
		public:
			Frame_admin_ajouter(Frame_principale* parent_arg,connexion_bdd*& bdd);
			void onClick_radio(wxCommandEvent &evenement);
			void onChange_select_matiere(wxCommandEvent &evenement);
			void onClick(wxCommandEvent &evenement);
			
		private:
			int id, nombre_matiere;
			connexion_bdd* bdd;
			Frame_principale* parent;
			wxArrayString texte_select;

			wxRadioButton *input_radio_prof;
			wxRadioButton *input_radio_eleve;
			wxRadioButton *input_radio_admin;
			wxRadioButton *input_radio_matricule_oui;
			wxRadioButton *input_radio_matricule_non;

				wxTextCtrl *input_ajout_nom, *input_ajout_prenom, *input_ajout_mdp,
						*input_ajout_eleve__nom_responsable,
						*input_ajout_eleve__prenom_responsable,
						*input_ajout_eleve__tel_responsable,
						*input_ajout_eleve__mail_responsable,
						*input_ajout_eleve__nom_rue,
						*input_ajout_eleve__rue,
						*input_ajout_eleve__code_postal,
						*input_ajout_eleve__ville,
						*input_ajout_eleve__tel_mobile,
						*input_ajout_matricule;

			wxStaticText*  label_ajouter_prof__matiere,
						*label_ajout_eleve__nom_responsable,
						*label_ajout_eleve__prenom_responsable,
						*label_ajout_eleve__tel_responsable,
						*label_ajout_eleve__mail_responsable,
						*label_ajout_eleve__sexe,
						*label_ajout_eleve__nom_rue,
						*label_ajout_eleve__rue,
						*label_ajout_eleve__code_postal,
						*label_ajout_eleve__ville,
						*label_ajout_eleve__tel_mobile,
						*label_ajout_eleve__groupe;

			wxChoice *input_ajout_eleve__sexe,
					 *input_ajout_eleve__groupe;

			wxComboBox *input_select_matiere_ajout;

			wxButton *bouton_valider_ajout;
			Frame_principale* frame_parente;
	};

	class Frame_admin : public Frame_principale
	{
		private:

			
		public:
			Frame_admin(Frame_login* parent,int &matricule,connexion_bdd*& bdd);
			void onAjouter(wxCommandEvent &evenement);
			void onClick_radio_ajout(wxCommandEvent &evenement);
			void onChange_select_matiere(wxCommandEvent &evenement);
			void onClick_ajouter(wxCommandEvent &evenement);
	};

	class Frame_login : public wxFrame
	{
		public:
			Frame_login(connexion_bdd*& arg_bdd);
			virtual ~Frame_login(){};

		private:
			wxTextCtrl *input_login,*input_mdp;
			wxButton *bouton_valider, *bouton_annuler;
			connexion_bdd* bdd;
			int type;
			Frame_principale* frame_parente;

			void onClick_valider(wxCommandEvent &evenement);
			void onClick_annuler(wxCommandEvent &evenement);
			void onChange(wxCommandEvent &evenement);
			void onClose(wxCloseEvent &evenement);
	};

#endif
