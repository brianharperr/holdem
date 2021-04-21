#pragma once

//Game Logic
extern "C"{
    #include "../../lib/game.h"
}

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

//GUI Data
typedef struct gui_data{
	int port_num;
	bool online;
	Game *game_buff;
} gui_data;

//App
class GUI : public wxApp
{
    public:
	virtual bool OnInit();
};

//Raise Menu
class RaiseMenu : public wxDialog
{
	public:
		RaiseMenu( Game *game, wxFrame *frame);
	private:
		Game *g;
		wxFrame *f;
		wxSlider *raise_slider;
		wxButton *okButton;
		wxButton *allinButton;
		wxButton *closeButton; 

		void OnOkay(wxCommandEvent& event);
		void OnAllIn(wxCommandEvent& event);
		void OnClose(wxCommandEvent& event);
};

//App Layout
class Frame : public wxFrame
{
	public:
		Frame( Game *game );
		Game *g;
		gui_data *gui_gd;
		int curr_round;

		void RoundHandler();
		void Update();
	private:
		int client_player_idx;

		Deck *deck;

		wxMenuBar *menuBar;
		wxMenu *menuFile;
		wxMenu *menuHelp;

		wxBoxSizer *frameContainer;
		wxBoxSizer *players_nameContainer;
		wxBoxSizer *players_stackContainer;
		wxBoxSizer *community_cardsContainer;
		wxBoxSizer *hole_cardsContainer;
		wxBoxSizer *potContainer;
		wxBoxSizer *optionsContainer;

		wxStaticText *players_nameText[9];
		wxStaticText *players_stackText[9];
		wxStaticText *total_potText;

		wxStaticBitmap *card_bmps[7];

		wxButton *foldButton;
		wxButton *callButton;
		wxButton *raiseButton;
		wxButton *okButton;

		RaiseMenu *raise_menu;

		void InitContainers();
		void InitText();
		void InitCards();
		void InitOptions();
		void UpdateText();
		bool IsLastMove();
		std::string GetCardFilepath(Card c);

		void OnFold(wxCommandEvent& event);
		void OnCall(wxCommandEvent& event);
		void OnRaise(wxCommandEvent& event);
		void OnOkay(wxCommandEvent& event);
};

class SettingsMenu : public wxDialog
{
	public:
		SettingsMenu( Game *g, gui_data *gd );
		wxTextCtrl *start_stack_txtctrl;
		wxTextCtrl *min_bet_txtctrl;
		wxTextCtrl *port_number_txtctrl;
		wxSlider *num_players_slider;
		gui_data *gui_gd;
	private:
		void OnOnline(wxCommandEvent& event);
		void OnOkay(wxCommandEvent& event);
		void OnClose(wxCommandEvent& event);
};

class Menu : public wxMenuBar
{
	public:
		Menu( Game *g, gui_data *gd, Frame *f );
		wxMenu *menuFile;
		wxMenu *menuHelp;
		gui_data *gui_gd;
		Frame *frame;
	private:
		Game *game;
		SettingsMenu *settings_menu;
		void OnNewGame(wxCommandEvent& event);
		void OnSettings(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
};
