//---------------------------------------------------------------------------

#ifndef Unit6H
#define Unit6H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <System.Win.ScktComp.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdIPWatch.hpp>
#include <Vcl.ExtCtrls.hpp>
//--------------------------------------
 /*	--------------------- Process of client dissconnect ----------------------

Server task : if is turn to server,

			  Step1 : server can choose a unchoosing number,
					  then send the number to client

				  -----------------------------------

Client task : if client got the number,

			  Step2 : it need to judge it's lines and send to Server

				  -----------------------------------

Server task : Server recive the client,

			  Step3 : Server need to judge Server's lines ,
					  and send the packet to ALL client who is the winner
					  (judge Server and Client who has mach more line)

				  ---------------- END ---------------

----------------------- Format of bingo package --------------

			Notice: Don't use the '#' symbol in the packet.

ID: 5bytes, "BINGO"
host: unfixed length, until encounter "#"
Main function: 1 byte
Sub function: 1 byte
lines: 2 bytes, string format, ex: "02" means it has 2 lines.
Select nember: 2 byte, encounter "!"
error code: 1 byte, encounter "@"

example:

   BINGO192.178.11.3#2102$01!3@

ID: BINGO
host: "192.168.11.3"
-----------s1: #
Main function: 2
Sub function: 1
lines:2
-----------s2: $
Select nember: "01"
-----------s3: !
error code: 3
-----------s4: @

----------------------------------------------------
New connection
MainFunc:1
SubFunc :1
Msg: client name

----------------------------------------------------------------*/
//BINGO192.178.11.3#2102$01!3@
typedef struct _PACKET
{
  String ID;  			//BINGO
  String Host; 			// unfixed length

  String s1;  			// "#"

  String MainFunc;  	// 2 byte
  String SubFunc;  		// 1 byte
  String lines; 		// 2 byte

  String s2; 			// "$"

  String Select_number; // "01"
  String error_code;  	// "!3"

  String s3; 			// "@"
}PACKET, *pPACKET;		//普通,指標

typedef struct _CLIENT_SOCKET
{
  bool active;
  TCustomWinSocket *pSocket;
  String ClientID;
}CLIENTSOCKET, *pCLIENTSOCKET;//普通,指標
//---------------------------------------------------------------------------
class TForm6 : public TForm
{
__published:	// IDE-managed Components
	TClientSocket *ClientSocket1;
	TServerSocket *ServerSocket1;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TMainMenu *MainMenu1;
	TIdIPWatch *IdIPWatch1;
	TTimer *Timer1;
	TMenuItem *file1;
	TMenuItem *listen;
	TMenuItem *Connect1;
	TMenuItem *Disconnect1;
	TComboBox *ComboBox1;
	TMemo *Memo1;
	TMemo *Memo2;
	TLabel *Label2;
	TLabel *Label1;
	TMemo *Memo3;
	TButton *Button1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall CheckClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall listenClick(TObject *Sender);
	void __fastcall ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Disconnect1Click(TObject *Sender);
	void __fastcall ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket1Connect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket1Disconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket1Error(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
	void __fastcall ClientSocket1Read(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Connect1Click(TObject *Sender);

	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ButOnClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);




private:	// User declarations((((((代表「不想被別人使用」
public:		// User declarations((((((代表「想被別人使用」
	__fastcall TForm6(TComponent* Owner);
	bool CheckClients(TCustomWinSocket *);
	String AssemblePacket(PACKET );
	pPACKET DisassemblePacket(String );
	void ShowPacket(PACKET);
	void ShowPacket(pPACKET);
	void ParsePacket(pPACKET);
	void ReflashClientList();

	void re_dice();
	void push(String);
	void check_line();
	void restart();
	void check_who_win(String);
	void btn_Visible(bool);
	void Bo_en();
    void So_en(String);
	void anm_check(int);
	void Delay(int);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm6 *Form6;
//---------------------------------------------------------------------------
#endif
