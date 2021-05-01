#include "../lib/gui.h"

wxIMPLEMENT_APP(GUI);

////////////////////////////////////-----APP INITIALIZATION-----/////////////////////////////
bool GUI::OnInit()
{

    Game *game = game_create(0, 0, 0);
    
    Frame *frame = new Frame(game);
    frame->Show(true);

    return true;
}

//////////////////////////////////////-----APP LAYOUT-----///////////////////////////////////
Frame::Frame( Game* game) : wxFrame( NULL, wxID_ANY, wxT("NL Texas Hold 'Em"), wxDefaultPosition, wxSize(425, 425), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL )
{

	client_player_idx = 0;

	//Store game data
	g = game;
	curr_round = 0;
	//GUI Data
	gui_gd = (struct gui_data*)malloc(sizeof(struct gui_data));
	gui_gd->port_num = 777;
	gui_gd->online = false;
	gui_gd->game_buff = game;

	//Window Icon
	SetIcon(wxIcon(wxT("../lib/icon.xpm")));
	//Add custom wxMenuBar
	SetMenuBar( new Menu( g, gui_gd, this ) );

	//Status Bar
	CreateStatusBar(1);
	SetStatusText(wxT("Ready"), 0);

	InitContainers();

	
	InitText();
	InitCards();
	
	InitOptions();
	RoundHandler();
	Update();
	SetSizer( frameContainer );
	Layout();
	SetMinSize(wxSize(425, 425));
	Centre( wxBOTH );
}

void Frame::InitContainers()
{

	frameContainer = new wxBoxSizer( wxVERTICAL );

	players_nameContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( players_nameContainer, 0, wxEXPAND, 5 );

	players_stackContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( players_stackContainer, 0, wxEXPAND, 5 );

	community_cardsContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( community_cardsContainer, 1, wxALIGN_CENTER_HORIZONTAL, 5 );

	hole_cardsContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( hole_cardsContainer, 1, wxALIGN_CENTER_HORIZONTAL, 5 );

	potContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( potContainer, 1, wxEXPAND, 5);

	optionsContainer = new wxBoxSizer( wxHORIZONTAL );
	frameContainer->Add( optionsContainer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

}

void Frame::InitCards()
{

	card_bmps[0] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT( "../lib/card-bmps/back.bmp" ), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	card_bmps[1] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT( "../lib/card-bmps/back.bmp" ), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );

	card_bmps[2] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../lib/card-bmps/back.bmp"), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	community_cardsContainer->Add( card_bmps[2], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	card_bmps[3] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../lib/card-bmps/back.bmp"), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	community_cardsContainer->Add( card_bmps[3], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	card_bmps[4] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../lib/card-bmps/back.bmp"), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	community_cardsContainer->Add( card_bmps[4], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	card_bmps[5] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../lib/card-bmps/back.bmp"), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	community_cardsContainer->Add( card_bmps[5], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	card_bmps[6] = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("../lib/card-bmps/back.bmp"), wxBITMAP_TYPE_BMP ), wxDefaultPosition, wxDefaultSize, 0 );
	community_cardsContainer->Add( card_bmps[6], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	card_bmps[0]->SetBitmap(wxBitmap(GetCardFilepath(g->table[client_player_idx].hand[0])));
	hole_cardsContainer->Add( card_bmps[0], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	card_bmps[1]->SetBitmap(wxBitmap(GetCardFilepath(g->table[client_player_idx].hand[1])));
	hole_cardsContainer->Add( card_bmps[1], 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
}

void Frame::InitOptions()
{
	
	foldButton = new wxButton( this, wxID_STOP, wxT( "FOLD" ), wxDefaultPosition, wxDefaultSize, 0 );
	optionsContainer->Add( foldButton, 0, wxALL, 5);
	callButton = new wxButton( this, wxID_YES, wxT( "CALL" ), wxDefaultPosition, wxDefaultSize, 0 );
	optionsContainer->Add( callButton, 0, wxALL, 5);
	raiseButton = new wxButton( this, wxID_OK, wxT( "RAISE" ), wxDefaultPosition, wxDefaultSize, 0 );
	optionsContainer->Add( raiseButton , 0, wxALL, 5);
	okButton = new wxButton( this, wxID_REDO, wxT( "OK" ), wxDefaultPosition, wxDefaultSize, 0 );
	optionsContainer->Add( okButton, 0, wxALL, 5);
	okButton->Hide();

	Bind(wxEVT_BUTTON, &Frame::OnFold, this, wxID_STOP);
	Bind(wxEVT_BUTTON, &Frame::OnCall, this, wxID_YES);
	Bind(wxEVT_BUTTON, &Frame::OnRaise, this, wxID_OK);
	Bind(wxEVT_BUTTON, &Frame::OnOkay, this, wxID_REDO);


}

void Frame::InitText()
{
	for(int i = 0; i < MAX_NUM_PLAYERS; ++i)
	{
		players_nameText[i] = 0;
		players_stackText[i] = 0;
	}
	for(int i = 0; i < MAX_NUM_PLAYERS; ++i)
	{
		if(i == g->gi.turn_idx){
			players_nameText[i] = new wxStaticText( this, wxID_ANY, wxString::Format( wxT( "[%d]" ), i), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
		}else{
			players_nameText[i] = new wxStaticText( this, wxID_ANY, wxString::Format( wxT( "%d" ), i), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
		}

		players_nameText[i]->Wrap( -1 );
		players_nameContainer->Add( players_nameText[i], 1, wxALL, 5 );
		players_stackText[i] = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
		players_stackText[i]->Wrap( -1 );
		players_stackContainer->Add( players_stackText[i], 1, wxALL, 5); 

		if(i >= g->gd.num_players){
		 	players_nameText[i]->Hide();
			players_stackText[i]->Hide();
		 }else{
		 	players_stackText[i]->SetLabel(wxString::Format( wxT( "%d" ), g->table[i].stack));
		 }
	}

	total_potText = new wxStaticText( this, wxID_ANY, wxString::Format( wxT( "POT: %d     MIN BET: %d" ), g->gd.total_pot, g->gd.curr_min_bet), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL );
	total_potText->Wrap( -1 );
	potContainer->Add( total_potText, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

	SetStatusText( wxString::Format( wxT( "WAITING ON PLAYER %d..." ), g->gi.turn_idx), 0);
	Layout();
}

void Frame::Update()
{

	for(int i = 0; i < MAX_NUM_PLAYERS; ++i)
	{

		if(i < g->gd.num_players){
			if(g->table[i].out){
				players_nameText[i]->SetLabel(wxString::Format( wxT( "F" ), i));
			}else{
				if(i == g->gi.turn_idx){
					players_nameText[i]->SetLabel(wxString::Format( wxT( "[%d]" ), i));
				}else{
					players_nameText[i]->SetLabel(wxString::Format( wxT( "%d" ), i));
				}
			}
			players_stackText[i]->SetLabel(wxString::Format( wxT( "%d" ), g->table[i].stack));
			if(!(players_nameText[i]->IsShown())){
				players_nameText[i]->Show();
				players_stackText[i]->Show();
			}
		}else{
			players_nameText[i]->Hide();
			players_stackText[i]->Hide();
		}
	}
	total_potText->SetLabel(wxString::Format( wxT( "POT: %d     MIN BET: %d" ), g->gd.total_pot, g->gd.curr_min_bet));
	SetStatusText( wxString::Format( wxT( "WAITING ON PLAYER %d..." ), g->gi.turn_idx), 0);

	Layout();

}

bool Frame::IsLastMove()
{
	return g->gi.turn_idx == g->gi.end_player_idx;
}

void Frame::RoundHandler()
{
	switch( g->gd.round )
	{
		case PREFLOP:
			round_handler( g );
			card_bmps[0]->SetBitmap(wxBitmap(GetCardFilepath(g->table[client_player_idx].hand[0])));
			card_bmps[1]->SetBitmap(wxBitmap(GetCardFilepath(g->table[client_player_idx].hand[1])));

			card_bmps[2]->SetBitmap(wxBitmap(wxT("../lib/card-bmps/back.bmp")));
			card_bmps[3]->SetBitmap(wxBitmap(wxT("../lib/card-bmps/back.bmp")));
			card_bmps[4]->SetBitmap(wxBitmap(wxT("../lib/card-bmps/back.bmp")));
			card_bmps[5]->SetBitmap(wxBitmap(wxT("../lib/card-bmps/back.bmp")));
			card_bmps[6]->SetBitmap(wxBitmap(wxT("../lib/card-bmps/back.bmp")));

			Update();
			break;
		case FLOP:
			round_handler( g );
			card_bmps[2]->SetBitmap(wxBitmap(GetCardFilepath(g->gd.community[0])));
			card_bmps[3]->SetBitmap(wxBitmap(GetCardFilepath(g->gd.community[1])));
			card_bmps[4]->SetBitmap(wxBitmap(GetCardFilepath(g->gd.community[2])));
			Update();
			break;
		case TURN:
			round_handler( g );
			card_bmps[5]->SetBitmap(wxBitmap(GetCardFilepath(g->gd.community[3])));
			Update();
			break;
		case RIVER:
			round_handler( g );
			card_bmps[6]->SetBitmap(wxBitmap(GetCardFilepath(g->gd.community[4])));
			Update();
			break;
		case END:

			unsigned short* winner = pot_winner( g, g->pot_list->list[0] );
			winnings_distribute( g );
			card_bmps[0]->SetBitmap(wxBitmap(GetCardFilepath(g->table[winner[0]].hand[0])));
			card_bmps[1]->SetBitmap(wxBitmap(GetCardFilepath(g->table[winner[0]].hand[1])));
			total_potText->SetLabel(wxString::Format( wxT( "Player %d wins with a %s!" ), winner[0], value_str[hand_rank(winner[1])]));

			callButton->Hide();
			foldButton->Hide();
			raiseButton->Hide();
			okButton->Show();
			break;
	}
	Layout();
}

void Frame::OnFold(wxCommandEvent& event)
{
	if(IsLastMove())
	{
		fold( g );
		RoundHandler();
	}else
	{
		fold( g );
		Update();
	}
}

void Frame::OnCall(wxCommandEvent& event)
{
	if(IsLastMove())
	{
		call( g );
		RoundHandler();
	}else
	{
		printf("Not last move.\n");
		call( g );
		Update();
	}
}

void Frame::OnRaise(wxCommandEvent& event)
{
	raise_menu = new RaiseMenu(g, this);
	raise_menu->Show(true);
}

void Frame::OnOkay(wxCommandEvent& event)
{
	callButton->Show();
	foldButton->Show();
	raiseButton->Show();
	okButton->Hide();
	g->gd.round = PREFLOP;
	RoundHandler();
}

std::string Frame::GetCardFilepath(Card c)
{
	std::string dir = "../lib/card-bmps/";
	std::string end = ".bmp";
	char suit;
	std::string path = "";
	std::string rank = "2345678901231";
	int r = (c >> 8) & 0xF;
	if (c & 0x8000)
      suit = 'c';
    else if (c & 0x4000)
      suit = 'd';
    else if (c & 0x2000)
      suit = 'h';
    else
      suit = 's';

  	if( r == 12 || r < 8){
  		path = dir + suit +  "0" + rank[r] + end;
  	}else if(r >= 8){
  		path = dir + suit +  "1" + rank[r] + end;
  	}
  	return path;
}

//////////////////////////////////////-----MENU BAR-----/////////////////////////////////////
Menu::Menu(Game *g, gui_data *gd, Frame *f) : wxMenuBar()
{
	frame = f;
	game = g;
	gui_gd = gd;
	gui_gd->game_buff = game_create(0, 0, 0);
	menuFile = new wxMenu;

    menuFile->Append( wxID_OK, wxT("New Game") );
    menuFile->Append( wxID_FLOPPY, wxT("Settings") );
    menuFile->Append( wxID_EXIT );

    menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    
    this->Append(menuFile, "&File");
    this->Append(menuHelp, "&Help");

    //Event binds
	Bind(wxEVT_MENU, &Menu::OnNewGame, this, wxID_OK);
	Bind(wxEVT_MENU, &Menu::OnSettings, this, wxID_FLOPPY);
	Bind(wxEVT_MENU, &Menu::OnExit, this, wxID_EXIT);


	Bind(wxEVT_MENU, &Menu::OnAbout, this, wxID_ABOUT);

}

void Menu::OnNewGame(wxCommandEvent& event)
{
	frame->g = gui_gd->game_buff;
	game->gd.round = PREFLOP;
	frame->RoundHandler();
	frame->Update();
}

void Menu::OnSettings(wxCommandEvent& event)
{

	settings_menu = new SettingsMenu( game, gui_gd );
	settings_menu->Show(true);
}

void Menu::OnExit(wxCommandEvent& event)
{
	Destroy();
}

void Menu::OnAbout(wxCommandEvent& event)
{
	wxMessageBox( "No Limits Texas Hold 'Em Poker\nv1.0\nBrian Harper", "About", wxOK | wxICON_INFORMATION );
}

////////////////////////////////-----SETTINGS MENU-----///////////////////////////////////
SettingsMenu::SettingsMenu(Game *g, gui_data *gd) : wxDialog( NULL, wxID_ANY, wxT( "Settings" ), wxDefaultPosition, wxSize( 250,325 ) )
{

	gui_gd = gd;
	//Containers
	wxBoxSizer *settings_container = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *num_players_container = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *start_stack_container = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *min_bet_container = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *port_number_container = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttons_container = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *online_container = new wxBoxSizer( wxHORIZONTAL );
	//Text Control
	start_stack_txtctrl = new wxTextCtrl( this, wxID_ANY, wxString::Format( wxT( "%d" ), gui_gd->game_buff->gd.start_stack), wxDefaultPosition, wxDefaultSize, 0 );
	min_bet_txtctrl = new wxTextCtrl( this, wxID_ANY, wxString::Format( wxT( "%d" ), gui_gd->game_buff->gd.min_bet), wxDefaultPosition, wxDefaultSize, 0 );
	port_number_txtctrl = new wxTextCtrl( this, wxID_ANY, wxString::Format( wxT( "%d" ), gui_gd->port_num), wxDefaultPosition, wxDefaultSize, 0 );

	//Text
	wxStaticText *num_players_text = new wxStaticText( this, wxID_ANY, wxT("Players"), wxDefaultPosition, wxDefaultSize, 0 );
	wxStaticText *start_stack_text = new wxStaticText( this, wxID_ANY, wxT("Start Stack"), wxDefaultPosition, wxDefaultSize, 0 );
	wxStaticText *min_bet_text = new wxStaticText( this, wxID_ANY, wxT("Minimum Bet"), wxDefaultPosition, wxDefaultSize, 0 );
	wxStaticText *port_number_text = new wxStaticText( this, wxID_ANY, wxT("Port Number"), wxDefaultPosition, wxDefaultSize, 0 );

	//Turn off text wrapping
	num_players_text->Wrap( -1 ); start_stack_text->Wrap( -1 ); min_bet_text->Wrap( -1 ); port_number_text->Wrap( -1 );

	//Slider
	num_players_slider = new wxSlider(this, -1, gui_gd->game_buff->gd.num_players, 3, 9, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL, wxDefaultValidator, wxT("Players"));
	
	//Checkbox
	wxCheckBox *online_checkbox = new wxCheckBox( this, wxID_ANY, wxT( "Online" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	online_checkbox->SetValue(gui_gd->online);

	//Buttons
	wxButton *okButton = new wxButton(this, -1 , wxT( "Ok" ), wxDefaultPosition, wxSize( 70,30 ));
	wxButton *closeButton = new wxButton(this, -1, wxT( "Close" ), wxDefaultPosition, wxSize( 70,30 ));

	//Event binds
	okButton->Bind(wxEVT_BUTTON, &SettingsMenu::OnOkay, this);
	closeButton->Bind(wxEVT_BUTTON, &SettingsMenu::OnClose, this);
	online_checkbox->Bind(wxEVT_CHECKBOX, &SettingsMenu::OnOnline, this);

	//Add controls to containers
	num_players_container->Add( num_players_text, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
	num_players_container->Add( num_players_slider, 0, wxALL | wxEXPAND, 5);
	settings_container->Add( num_players_container, 1, wxEXPAND, 5);

	start_stack_container->Add( 10, 0, 1, 0, 5 );
	start_stack_container->Add( start_stack_text, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	start_stack_container->Add( start_stack_txtctrl, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	start_stack_container->Add( 10, 0, 1, 0, 5 );
	settings_container->Add( start_stack_container, 1, wxEXPAND, 5);

	min_bet_container->Add( 10, 0, 1, 0, 5 );
	min_bet_container->Add( min_bet_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	min_bet_container->Add( min_bet_txtctrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	min_bet_container->Add( 10, 0, 1, 0, 5 );
	settings_container->Add( min_bet_container, 1, wxEXPAND, 5);

	port_number_container->Add( 10, 0, 1, 0, 5 );
	port_number_container->Add( port_number_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	port_number_container->Add( port_number_txtctrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	port_number_container->Add( 10, 0, 1, 0, 5 );
	settings_container->Add( port_number_container, 1, wxEXPAND, 5);

	online_container->Add( online_checkbox, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	settings_container->Add( online_container, 0, wxALIGN_CENTER_HORIZONTAL, 5);

	buttons_container->Add( okButton, 1);
	buttons_container->Add( closeButton, 1, wxLEFT, 5);
	settings_container->Add( buttons_container, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10 );


	this->SetSizer( settings_container );
	Layout();
	Centre();
	ShowModal();
	Destroy();
}

void SettingsMenu::OnOnline(wxCommandEvent& event)
{
	gui_gd->online = !gui_gd->online;
}

void SettingsMenu::OnOkay(wxCommandEvent& event)
{
	gui_gd->game_buff = game_create(num_players_slider->GetValue(), wxAtoi(start_stack_txtctrl->GetValue()), wxAtoi(min_bet_txtctrl->GetValue()));
	Destroy();
}

void SettingsMenu::OnClose(wxCommandEvent& event)
{
	Destroy();
}

RaiseMenu::RaiseMenu( Game *game, wxFrame* frame ) : wxDialog( NULL, wxID_ANY, wxT( "Raise" ), wxDefaultPosition, wxSize( 300,125 ) )
{
	g = game;
	f = frame;
	//Containers
	wxBoxSizer *menu_container = new wxBoxSizer( wxVERTICAL);
	wxBoxSizer *slider_container = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttons_container = new wxBoxSizer( wxHORIZONTAL );
	//Slider
	raise_slider = new wxSlider(this, -1, 0, g->gd.curr_min_bet, g->table[g->gi.turn_idx].stack, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL, wxDefaultValidator, wxT("Raise Amount"));
	//Buttons
	okButton = new wxButton(this, -1 , wxT( "Ok" ), wxDefaultPosition, wxSize( 70,30 ));
	allinButton = new wxButton(this, -1, wxT( "All In" ), wxDefaultPosition, wxSize( 70,30 ));
	closeButton = new wxButton(this, -1, wxT( "Close" ), wxDefaultPosition, wxSize( 70,30 ));

	okButton->Bind(wxEVT_BUTTON, &RaiseMenu::OnOkay, this);
	allinButton->Bind(wxEVT_BUTTON, &RaiseMenu::OnAllIn, this);
	closeButton->Bind(wxEVT_BUTTON, &RaiseMenu::OnClose, this);

	slider_container->Add( raise_slider, 1, wxALL | wxEXPAND, 5);
	menu_container->Add(slider_container, 1, wxEXPAND, 5);
	buttons_container->Add( okButton, 1);
	buttons_container->Add( allinButton, 1);
	buttons_container->Add( closeButton, 1, wxLEFT, 5);
	menu_container->Add(buttons_container, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	this->SetSizer( menu_container );
	Layout();
	Centre();
	ShowModal();
	Destroy();
}

void RaiseMenu::OnOkay(wxCommandEvent& event)
{
	raise_bet(g, raise_slider->GetValue());
	f->Update();
	Destroy();
}

void RaiseMenu::OnAllIn(wxCommandEvent& event)
{
	printf("STACK: %d\n",g->table[g->gi.turn_idx].stack);
	raise_bet(g, g->table[g->gi.turn_idx].stack);
	f->Update();
	Destroy();
}

void RaiseMenu::OnClose(wxCommandEvent& event)
{
	Destroy();
}