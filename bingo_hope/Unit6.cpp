//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit6.h"
#include "Unit1.h"

#define  CLIENT_MAX 1 //�w�q�s�u�H��
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;

//�������������������������������������������������������������������������������������������������������������������������������������������������������������u�ܼơv
String strHost;
bool blnServer;  			//��server or client


String ClientHostName = "";
String ServerName = "";
String newClient = "";

bool fgConnectState;  		//�O�_�b�s�u���A
int step = 0;     			//�B�J
bool turn ;
int who = 0;



bool re = false;            //���s
int total;                  //�s�u��
bool win = false;           //�ӭt
String asktimes = 0;

TButton* butt[5][5];		//���s�}�C
int bingo[5][5];			//�ƭȰ}�C
bool b_stat[5][5];          //���A�}�C

TButton* but_en[5][5];		//���s�}�C
String en[5][5];            //�ĤH�}�C
bool ben_stat[5][5];          //���A�}�C
String en_Strpkg ="";
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uload�v
__fastcall TForm6::TForm6(TComponent* Owner) //��l��
	: TForm(Owner)
{
	Memo1->Lines->Clear();
	Memo2->Lines->Clear();
	re_dice();

	int o=0;
	int w = 50,h = 50;
	for(int i = 0;i<5;i++)
	{
		for(int j = 0;j<5;j++)
		{
			TButton *but = new TButton(this);
			butt[i][j] = but;

			but->Left = w*i+25;
			but->Top = j*h+50;
			but->Height = 50;
			but->Width = 50;
			but->Name = "B_"+IntToStr(bingo[i][j]);
			but->Caption = bingo[i][j];



			Form6->InsertControl(but);
			but->OnClick = ButOnClick;

			b_stat[i][j] = false;
			//---------------------------------------
			TButton *but2 = new TButton(this);
			but_en[i][j]= but2;

			but2->Left = w*i+300;
			but2->Top = j*h+50;
			but2->Height = 50;
			but2->Width = 50;
			but2->Name = "Ben_"+IntToStr(o);o++;
			//ShowMessage(IntToStr(o));
			but2->Caption = "";
			Form6->InsertControl(but2);
			//but2->OnClick = ButOnClick;

			ben_stat[i][j] = false;
		}


	}
	btn_Visible(false);

}
void __fastcall TForm6::FormCreate(TObject *Sender)
{
	randomize();

	fgConnectState = false;

	Disconnect1 -> Enabled = false;

	Form6 -> Caption = "Local host:"+IdIPWatch1->LocalIP();

}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uMenu�v
//�����������������������������������������������������������������������������ulisten�v
void __fastcall TForm6::listenClick(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	Form6->Caption="I am a server: "+IdIPWatch1->LocalIP();

	listen->Checked = !listen->Checked;

	ComboBox1->Clear();

	if (listen->Checked)//����Server
	{
		listen->Enabled = false;
		ClientSocket1->Active = false;//���� Client

		try
		{
			ServerSocket1->Active = true;//�}�� Server
			blnServer = true;//
		}
		catch(...)
		{
			listen->Checked=false;
			ShowMessage("Be a server failure.");
			return;
		}

		ServerName=IdIPWatch1->LocalIP();
		//StatusBar1->SimpleText="Status: Chat Server Listening...";
		Connect1->Enabled=false;
		Form2->Memo1->Lines->Add("Server: "+ServerName+", socket: "+IntToStr(ServerSocket1->Socket->SocketHandle));
		ComboBox1->Enabled=true;
		blnServer = true;//
	}
	else//����server
	  {
		if (ServerSocket1->Active)
		  {

			// build packet
			pkg.ID="BINGO";
			pkg.Host=ServerName;
			pkg.MainFunc="1";
			pkg.SubFunc="2";
			pkg.lines = "00";
			pkg.Select_number = "00";
			pkg.error_code = "3";

			strPkg = AssemblePacket(pkg);
			ServerSocket1->Socket->Connections[0]->SendText(strPkg);

			ServerSocket1->Active = false;
		  }
		Connect1->Enabled=true;

		//StatusBar1->SimpleText="Status : Server Close";
	  }

	Form6->Tag=0; //��ܬ�server
    Memo2->Enabled = true;
}
//�����������������������������������������������������������������������������uconnect�v
void __fastcall TForm6::Connect1Click(TObject *Sender)
{
	Form6->Caption="I am a client";

	if (ClientSocket1->Active)
		ClientSocket1->Active = false;

	strHost="127.0.0.1";
	if (InputQuery("Chat Connection", "Chat Server IP: ", strHost))//��J����
	{
		if (strHost.Length() > 0)
		{
			ClientSocket1->Host = strHost;
			//ShowMessage("C:�s����");//------------------------------------------------------------------------------
			ClientSocket1->Active = true;
			//Timer1->Enabled=true;

			listen->Checked = false;
            Disconnect1->Enabled = false;
			blnServer = false ;

			ComboBox1->Enabled=false;

			Memo2->Enabled = true;
		}
	}
}
//�����������������������������������������������������������������������������udisconnect�v
void __fastcall TForm6::Disconnect1Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	if(blnServer) //���\�ण�O��server�Ϊ�
		return;
    Timer1 ->Enabled = false;
	// build packet
	pkg.ID="BINGO";
	pkg.MainFunc="3";
	pkg.SubFunc="2";
	pkg.lines = "00";
	pkg.Select_number = "00";
	pkg.error_code = "3";

	pkg.Host=ClientHostName;


	strPkg=AssemblePacket(pkg);

	ClientSocket1->Socket->SendText(strPkg);
	ClientSocket1->Active = false;

	String A = Time();


	Memo2->Lines->Add("["+A+"] Disconnect.");
	listen->Checked = false;
	Connect1->Checked = false;
	Disconnect1->Checked = false;

	fgConnectState=false;
	Memo2->Visible=false;

	//StatusBar1->SimpleText="Status: Disconnect." ;
	Form6->Caption="";

}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uServer&Client�v
//�����������������������������������������������������������������������������uServer�v
void __fastcall TForm6::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	PACKET pkg;
	String strPkg;
	int clientNum;
	//ShowMessage("S:Client�s�u���\");//------------------------------------------------------------------------------
	clientNum = ServerSocket1->Socket->ActiveConnections;

	if(clientNum > CLIENT_MAX)
	{
	  Form2->Memo1->Lines->Add("�w�F�s�u�̤j�ƶq");
	  Socket->Close();
	  return;
	}

  //StatusBar1->SimpleText= "Status: Connect from " + Socket->RemoteHost+
						  " socket ID: "+ IntToStr(Socket->SocketHandle);

  if(!fgConnectState)
  {
    fgConnectState = true;
  if(!CheckClients(Socket)) //new connection
	{
		pkg.ID="BINGO";
		pkg.MainFunc = "1";
		pkg.SubFunc = "1";
		pkg.lines = "00";
		pkg.Select_number = "26";
		pkg.error_code = "3";
		ClientHostName = ServerSocket1->Socket->Connections[0]->RemoteAddress+"-"+
						  IntToStr(ServerSocket1->Socket->Connections[0]->SocketHandle);
		pkg.Host = ServerName;//ServerName  ClientHostName
		strPkg = AssemblePacket(pkg);

		//ShowMessage("S:�^�ǹ���}");//------------------------------------------------------------------------------
		ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	}
    Sleep(100);
	restart();
	btn_Visible(true);
	Timer1->Enabled = true;
    Button1 -> Visible = true;

	step = 0;
	turn = true;

	Label1->Caption = "�ڤ�^�X";
	Label1->Font->Color = clBlue;
  }

}
void __fastcall TForm6::ServerSocket1Accept(TObject *Sender, TCustomWinSocket *Socket)
{
	fgConnectState = true;

	//ShowMessage("1");//------------------------------------------------------------------------------
	ReflashClientList();
}
void __fastcall TForm6::ServerSocket1ClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	Form2->Memo1 -> Lines -> Add("'" + Socket->RemoteAddress + "-" + IntToStr(Socket->SocketHandle) + "'disconnection comfirmed");
	//StatusBar1 -> SimpleText = "Status: Listening...";


	if( ServerSocket1 -> Socket -> ActiveConnections == 1 )
	{
		fgConnectState = false;
		Memo2->Visible = false;
		btn_Visible(false);
		restart();
		ComboBox1 -> Enabled = false;

		Timer1 -> Enabled = false;
		Button1 -> Visible = false;
	}

}
void __fastcall TForm6::ServerSocket1ClientError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode)
{
	//StatusBar1->SimpleText = "Error from Client socket.";
	ReflashClientList();
}
void __fastcall TForm6::ServerSocket1ClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	pPACKET pPkg;
	String strPacket;
	String A = Time();

	strPacket = Socket->ReceiveText();
	//ShowMessage("S����:"+strPacket);

	if(strPacket =="handhand" || strPacket =="handok"||strPacket =="okhand"||strPacket =="handokhand")
	{asktimes=0;asktimes = 0;return;}

	try
	{

	if(strPacket =="hand")
	{
		//ShowMessage(strPacket);
		step = 0;
	}else if(strPacket.Length() ==50)
	{
		So_en(strPacket);
		//Bo_en();
		return;
	}else if(strPacket =="restart")
	{
		//restart();
        String s = "again?";
		String b = "restart";

		if ( Application->MessageBox(s.c_str(),b.c_str(), MB_OKCANCEL) == IDOK)
		{
			restart();
			Bo_en();
            Delay(400);
			ServerSocket1->Socket->Connections[0]->SendText("ok") ;
		}
		else
		{
			ServerSocket1->Socket->Connections[0]->SendText("no") ;
		}
	}else if(strPacket =="ok")
	{
		restart();
        Bo_en();
		asktimes = 0;
	}else if(strPacket =="no")
	{
		asktimes = 0;

	}else// if(strPacket.Length()<35 && strPacket.Length()>10)
	{
		Memo3->Lines->Add(strPacket);
		pPkg=DisassemblePacket(strPacket);



		if(pPkg==NULL)
		{
			Memo1->Lines->Add("Packet error");
			return;
		}

		ParsePacket(pPkg);
		delete pPkg;
	}
	step = 0;
	}catch(...)
	{
    ShowMessage("Some ���~");


	}

	

}
//�����������������������������������������������������������������������������uClient�v
void __fastcall TForm6::ClientSocket1Connect(TObject *Sender, TCustomWinSocket *Socket)
{
	Timer1->Enabled = true;
	restart();
	btn_Visible(true);

	//StatusBar1 -> SimpleText= "Status: Connect to " + Socket -> RemoteHost ;
	fgConnectState=true;
	Memo2->Clear();
	Memo2->Visible=true;

	listen -> Enabled=false;
	Connect1->Enabled=false;
	Disconnect1->Enabled=true;
    turn = false;

	ClientSocket1->ClientType=ctNonBlocking;
	Label1->Caption = "�Ĥ�^�X";
	Label1->Font->Color = clRed;
}
void __fastcall TForm6::ClientSocket1Disconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	Timer1->Enabled=false;
	//StatusBar1->SimpleText="server disconnected";
	fgConnectState=false;
	Memo2->Visible=false;
	//ShowMessage("dis");

	listen->Enabled=true;
	Disconnect1->Enabled=false;
	Connect1->Enabled=true;
	Button1 ->Visible = false;

    fgConnectState = false;
	Memo2->Visible = false;
	btn_Visible(false);
	restart();
	ComboBox1 -> Enabled = false;

	Timer1 -> Enabled = false;
	Button1 -> Visible = false;
}
void __fastcall TForm6::ClientSocket1Error(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode)
{
	Memo2->Lines->Add("Error: " + strHost);
	ErrorCode = 0;
}
void __fastcall TForm6::ClientSocket1Read(TObject *Sender, TCustomWinSocket *Socket)

{
	pPACKET pPkg;
	String strPacket;

	if(!fgConnectState)
		return;

	

	String A = Time();
	strPacket=Socket->ReceiveText();

	//if(strPacket !="hand")
	//{
	//	ShowMessage(strPacket.Length());
	//}

	if(strPacket =="handhand" || strPacket =="handok"||strPacket =="okhand"||strPacket =="handokhand")
	{asktimes=0;asktimes = 0;return;}

    try
	{
    if(strPacket =="hand")
	{
		step = 0;
		Delay(400);
		ClientSocket1->Socket->SendText("hand");
	}else if(strPacket.Length() ==50)
	{
		//ShowMessage(strPacket);
		So_en(strPacket);
		Bo_en();
		//return;
	}else if(strPacket =="restart")
	{
		//restart();
		String s = "again?";
		String b = "restart";

		if ( Application->MessageBox(s.c_str(),b.c_str(), MB_OKCANCEL) == IDOK)
		{
			restart();
			Bo_en();
			Delay(400);
			ClientSocket1->Socket->SendText("ok");
		}
		else
		{
			ClientSocket1->Socket->SendText("no");
		}
	}else if(strPacket =="ok")
	{
		restart();
        Bo_en();
		asktimes = 0;
	}else if(strPacket =="no")
	{
		asktimes = 0;

	}
	else
	{
		Memo3->Lines->Add(strPacket);
		//ShowMessage("C:Ū��");
		pPkg=DisassemblePacket(strPacket);
		if(pPkg==NULL)
		{
			ShowMessage("�N���ڪ����U�A�ŪŦp�]");
			Memo1->Lines->Add("Packet error");
			return;
		}

		ParsePacket(pPkg);
		delete pPkg;

	}
    step = 0;
	}catch(...)
	{
	ShowMessage("Some ���~");


	}



}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uPacket�v
//�����������������������������������իʥ]
String TForm6::AssemblePacket(PACKET pkg)
{
	String str="",tmp;
   //���Y
	if(pkg.ID != "BINGO")//�p���O���G�A�h�O�u�S����O�v�����^��
	{
		 str = pkg.ID;
		 return str;
	}

	str += pkg.ID;
	str+=pkg.Host;
	str += "#";

	str += pkg.MainFunc;
	str += pkg.SubFunc;
	str += pkg.lines;
	str +=  "$";

	str += pkg.Select_number;

	str += "!"+pkg.error_code;//3
	str +=  "@";

   return str;
}
//������������������������������������ʥ]
pPACKET TForm6::DisassemblePacket(String strPkg)
{
	PACKET tmpPkg;
	pPACKET pPkg;
	int separator;
	String strTmp;

   Memo2->Lines->Add("===================");



//--------------------------------------------------------
try
{
	separator = strPkg.Pos("#");
	tmpPkg.Host = strPkg.SubString(6,separator-6);

	tmpPkg.MainFunc = strPkg.SubString(separator+1,1);//1
	tmpPkg.SubFunc = strPkg.SubString(separator+2,1);//2
	tmpPkg.lines = strPkg.SubString(separator+3,1);//3

	separator = strPkg.Pos("$");
	tmpPkg.Select_number = strPkg.SubString(separator+1,2);//12

	separator = strPkg.Pos("@");
	tmpPkg.error_code = strPkg.SubString(separator-1,1);//12

}catch(...)
{

}
  
//--------------------------------------------------------
   //�ŧi�s���O����
   pPkg=new PACKET;//���Ы��A���n�Ρu->�v���O�u=�v

   pPkg->ID = tmpPkg.ID;
   pPkg->Host = tmpPkg.Host;

   pPkg->MainFunc = tmpPkg.MainFunc;
   pPkg->SubFunc = tmpPkg.SubFunc;

   pPkg->lines = tmpPkg.lines;
   pPkg->Select_number = tmpPkg.Select_number;

   pPkg->error_code = tmpPkg.error_code;

   return pPkg;//�^�ǡA����A�ۦ�P�_�O�_�n�R���O����
}
//�����������������������������������ѪR�ʥ]*******************************************************************
void TForm6::ParsePacket(pPACKET pPkg) //�e�X�r���Memo
{;
	int mFunc,sFunc;

	mFunc = StrToInt(pPkg->MainFunc);// 1:�_�l ,2:�I��
	sFunc = StrToInt(pPkg->SubFunc);

	switch(mFunc)
	{
		case 1:
			switch(sFunc)
			{
				case 1://1,1:����o�h�ʥ]���A�T�{����Client
					ClientHostName = pPkg->Host;
					//ShowMessage("C:����v���}:"+ClientHostName);//------------------------------------------------------------------------------
					Form2->Memo1-> Lines-> Add("New name: "+ClientHostName);
					Form6->Caption=ClientHostName;
					Delay(100);
					ClientSocket1->Socket->SendText("ok");
					restart();
					btn_Visible(true);
					fgConnectState = true;
					blnServer = false;
                    Button1 ->Visible = true;

				break;

				case 2://1,2:�_�u�ʥ]
					if(blnServer)
					{
						ServerSocket1->Active = false;
					}else
					{
						ClientSocket1->Active = false;

						listen->Checked = false;
						Connect1->Checked = false;
						Disconnect1->Checked = false;
					}
					fgConnectState = false;
					blnServer = NULL;


				break;
			}
		break;

		case 2:////2,0

			if(pPkg->lines != "00" && pPkg->Select_number != "00")
			{
				Memo1->Lines->Add("["+pPkg->Host+"]:");
				Memo1->Lines->Add("lines:" + pPkg->lines);
				Memo1->Lines->Add("select:" + pPkg->Select_number);
			}

            switch(sFunc)
			{
				case 0:
					anm_check(StrToInt(pPkg->Select_number));
                    Label1->Caption = "�ڤ�^�X";
					Label1->Font->Color = clBlue;

					if(total>=3)
						check_who_win(pPkg->lines);

				break;

				case 1:
					anm_check(StrToInt(pPkg->Select_number));
					check_who_win(pPkg->lines);
				break;

				case 2:
					ShowMessage("Game�I");
					Label1->Caption = "Game�I";
				break;

				case 3:
					ShowMessage("Wasted");
					Label1->Caption = "Wasted";
				break;
				case 4:
					ShowMessage("���⪺��");
					Label1->Caption = "���⪺��";
				break;
			}
			//ShowMessage("���\������T��");

			break;

		case 3://3,0:���j���_�u
			if(!blnServer)//���Oserver�Ϊ�
			{
				ServerSocket1->Active = false;

			}

			Disconnect1Click(ClientSocket1);
			ShowMessage("Discord");

		break;
	}

}
//������������������������������������ܫʥ]
void TForm6::ShowPacket(PACKET pkg)
{
	Form6 -> Memo1 ->Lines->Add("=================");
	Form6 -> Memo1 ->Lines->Add("ID= "+ pkg.ID);
	Form6 -> Memo1 ->Lines->Add("Host= "+ pkg.Host);
	Form6 -> Memo1 ->Lines->Add("MainFunc= "+ pkg.MainFunc);
	Form6 -> Memo1 ->Lines->Add("SubFunc= "+ pkg.SubFunc);
	Form6 -> Memo1 ->Lines->Add("lines= " + pkg.lines);
	Form6 -> Memo1 ->Lines->Add("Select_number= "+ pkg.Select_number);
}
void TForm6::ShowPacket(pPACKET pPkg)//p=point(����
{
	Form6 -> Memo1 ->Lines->Add("=================");
	Form6 -> Memo1 ->Lines->Add("ID= "+ pPkg->ID);
	Form6 -> Memo1 ->Lines->Add("Host= "+ pPkg->Host);
	Form6 -> Memo1 ->Lines->Add("MainFunc= "+ pPkg->MainFunc);
	Form6 -> Memo1 ->Lines->Add("SubFunc= "+ pPkg->SubFunc);
	Form6 -> Memo1 ->Lines->Add("Msg length= " +pPkg->lines);
	Form6 -> Memo1 ->Lines->Add("Msg= "+ pPkg->Select_number);
}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uButton_click�v
//�������������������������������������s�ƥ�
void __fastcall TForm6::ButOnClick(TObject *Sender)
{;
	TControl *btn = dynamic_cast<TControl *>(Sender);
	//ShowMessage(btn->Name);
	String SL;

	if(turn && fgConnectState){
	for (int i=0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(btn->Name == butt[i][j]->Name && !b_stat[i][j])
			{

				SL = butt[i][j]->Caption;
				b_stat[i][j] = true;

				btn -> Enabled = false;


				turn = false;

				for (int a=0; a < 5; a++)
				{
					for (int b = 0; b < 5; b++)
					{
						if(en[i][j] == SL)
						{


						}
					}

				}
				
			}


		}
	}
	check_line();

	push(SL);
	}
				//return;
	
}
//�������������������������������������s���s
void __fastcall TForm6::Button1Click(TObject *Sender)
{
	if(asktimes != 0)
		return;

	if(blnServer)
		ServerSocket1->Socket->Connections[0]->SendText("restart");
	else
		ClientSocket1->Socket->SendText("restart");

	asktimes += 1;
}
//�����������������������������������ˬd�s�u��
void TForm6::check_line()
{
	int X = 0, Y = 0, left_top = 0, left_bot = 0,count_left_top = 0, count_left_bot = 0;

	for (int i = 0; i < 5; i++)
	{
		int count_X = 0, count_Y = 0;
		for (int j = 0; j < 5; j++)
		{
			if (b_stat[i][j])//�C
				count_X++;
			if (b_stat[j][i])//��
				count_Y++;
			if (i == j && b_stat[j][i])//�u\�v
				count_left_top++;
			if (i + j == 5 - 1 && b_stat[j][i])//�u/�v
				count_left_bot++;
		}
		if (count_X == 5)
			X++;
		if (count_Y == 5)
			Y++;
	}
	if (count_left_top == 5)
	left_top++;
	if (count_left_bot == 5)
	left_bot++;
	//�ת�"/"&"\"
	total = X + Y + left_top + left_bot;
	Label2->Caption = "�@�G"+IntToStr(total)+"��";
	if(total>=3)
	{
        win = true;
	}

}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������uvoid�v
//�����������������������������������ü�
void TForm6::re_dice()
{
	int V[25];
	int count = 0;
	int aa=0;
	if(blnServer)
	{
	   aa=0;
	}else
	{
      	aa=5;
	}

	String myStrpkg = "";
	srand((unsigned)time(NULL)+aa);
	for (int i = 0; i < 25; i++)
	{
		V[i] = rand() % (25 - 1 + 1) + 1;   //�üƲ���

		for (int j = 0; j < i; j++)
		{
			while (V[i] == V[j])    //�ˬd�O�_�P�e�����ͪ��ƭȵo�ͭ��ơA�p�G���N���s����
			{
				j = 0;  //�p�����ơA�N�ܼ�j�]��0�A�A���ˬd (�]���٬O�����ƪ��i��)
				V[i] = rand() % (25 - 1 + 1) + 1;   //���s����
			}

		}
			

	}
    en_Strpkg ="";
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			bingo[i][j] = V[count];
			myStrpkg += StringOfChar('0',2 - IntToStr(bingo[i][j]).Length()) + bingo[i][j];
			count++;
		}


}
//����������������������������������������ܫ��s
void TForm6::btn_Visible(bool visi)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0;j<5 ; j++)
		{
			butt[i][j]->Visible = visi;
			but_en[i][j]->Visible = visi;
		}
	}

}
//�������������������������������������e�ʥ]
void TForm6::push(String SL)
{;
	PACKET pkg;
	String strPkg;
	String srtClient;

	turn = false;

  if(blnServer)
	pkg.Host=IdIPWatch1->LocalIP();
  else
	pkg.Host=ClientHostName;


	//String L="";
	//L = StringOfChar('0',2-IntToStr(total).Length()) + IntToStr(total);

	String S="";
	S = StringOfChar('0',2 - SL.Length()) + SL;

	pkg.ID=					"BINGO";
	if(blnServer)
		pkg.Host=				ServerName;
	else
		pkg.Host=				ClientHostName;

	pkg.MainFunc=		   	"2";//�ǰe�I��ʥ]�O�u02�v

	if(total >= 3)
	{
		pkg.SubFunc=		"1";
	}
	else
	{
		pkg.SubFunc=		"0";
	}


	pkg.lines=				IntToStr(total);
	pkg.Select_number = 	S;

	pkg.error_code = 		"3";

	strPkg = AssemblePacket(pkg);
	//ShowMessage(strPkg);

	if(blnServer)
		ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	else
		ClientSocket1->Socket->SendText(strPkg);

	Label1->Caption = "�Ĥ�^�X";
	Label1->Font->Color = clRed;
}
//�����������������������������������T�{�ĤH�ʥ]
void TForm6::anm_check(int d_select)
{
	for (int i=0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(d_select == bingo[i][j] && !b_stat[i][j])
			{
				b_stat[i][j] = true;
				turn = true;
				butt[i][j]->Enabled = false;
				//but_en[j][i]->Enabled = false;
			}


		}
	}
	check_line();



}
//�������������������������������������s�U�@��
void TForm6::restart()
{
	Label1->Caption = "";
    total = 0;
    win = false;

	re_dice();
    for(int i = 0;i<5;i++)
	{
		for(int j = 0;j<5;j++)
		{
			butt[i][j]->Caption = bingo[i][j];
			butt[i][j]->Enabled = true;
			b_stat[i][j] = false;
		}


	}

	if(blnServer)
	{
		turn = true;
        Label1->Caption = "�ڤ�^�X";
		Label1->Font->Color = clBlue;
	}
	else
	{
		turn = false;
		Label1->Caption = "�Ĥ�^�X";
		Label1->Font->Color = clRed;
	}
	//Bo_en();
}
//������������������������������������Ĺ
void TForm6::check_who_win(String anm_lines)
{
    PACKET pkg;
	String strPkg;
	int anm_line =  StrToInt(anm_lines);
    turn = false;

  if(blnServer)
	pkg.Host=IdIPWatch1->LocalIP();
  else
	pkg.Host=ClientHostName;


	String L="0";
	L = StringOfChar('0',2-IntToStr(total).Length()) + IntToStr(total);



	pkg.ID=					"BINGO";
	if(blnServer)
		pkg.Host=				ServerName;
	else
		pkg.Host=				ClientHostName;

	pkg.MainFunc=		   	"2";//�ǰe�I��ʥ]�O�u2�v

	ShowMessage("���G"+IntToStr(anm_line)+",�ڤ�G"+IntToStr(total));
	if(anm_line==total)//����
	{
		pkg.SubFunc=		"4";ShowMessage("���⪺��");
		Label1->Caption = "���⪺��";
	}
	else if(anm_line > total)//���ӧQ
	{
		pkg.SubFunc=		"2";ShowMessage("Wasted");
		Label1->Caption = "Wasted";
	}
	else if(anm_line < total)//�ڤ�ӧQ
	{
		pkg.SubFunc=		"3";ShowMessage("Game�I");
		Label1->Caption = "Game�I";
	}
	Label1->Caption = "";
	pkg.lines=				L;

	pkg.error_code = 		"3";

	strPkg = AssemblePacket(pkg);
	//ShowMessage(strPkg);

	if(blnServer)
		ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	else
		ClientSocket1->Socket->SendText(strPkg);

	
}
//�������������������������������������������������������������������������������������������������������������������������������������������������������������u�ĤH�v
//�����������������������������������]����
void TForm6::Bo_en()
{
	String strPkg = "";
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
		   strPkg += StringOfChar('0',2-IntToStr(bingo[j][i]).Length()) + IntToStr(bingo[j][i]);
		}
	}
	//ShowMessage(strPkg);
	if(blnServer)
		ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	else
		ClientSocket1->Socket->SendText(strPkg);

}

//����������������������������������������
void TForm6::So_en(String strPkg)
{
	//ShowMessage("��50");
	for (int i = 0; i < 5; i++)
	{
		for (int j=0; j < 5; j++)
		{
			int Location = (i+5*j)*2+1;
			en[i][j] = strPkg.SubString(Location,2);

			but_en[i][j]->Caption = "?";//en[i][j];
		}
	}




}


//�������������������������������������������������������������������������������������������������������������������������������������������������������������uelse�v
//�����������������������������������ˬd�s�s�u
bool TForm6::CheckClients(TCustomWinSocket *Socket)
{
  bool fg;
  int sHnd;
  String hostIP;

  hostIP=Socket->RemoteAddress;
  sHnd=Socket->SocketHandle;

  newClient= hostIP+"-"+IntToStr(sHnd);
  Form2->Memo1->Lines->Add("New connection: "+newClient);
  Form2->Memo1->Lines->Add("Connection number: "+
                           IntToStr(ServerSocket1->Socket->ActiveConnections));

  fg=false;
  return fg;
}
//�����������������������������������ˬd�s�u��
void __fastcall TForm6::CheckClick(TObject *Sender)
{
	int number;
	TServerWinSocket *sSocket;//���СA�{�ɭn�@���O����A�����D�b���̡B�n����A�ҥH�n�Ϋ��Ы��V���Ӱ϶��C
						//�W�U���u�O���}��(�]���Ӫ�)�ASocket�OTServerWinSocket���O
	sSocket=ServerSocket1->Socket;

   number= sSocket->ActiveConnections;

   String time =  Time();

   Form2->Memo1->Lines->Add("[" + time + "] Current conncetion number: "+  //�ثe���X�ӳs�u
						   IntToStr(number));

   Form2->Memo1->Lines->Add(sSocket->Connections[0]->SocketHandle);
}
//������������������������������������s�C��
void TForm6::ReflashClientList()
{;
	ComboBox1->Clear();

	if(ServerSocket1->Socket->ActiveConnections>1)
	{
		ComboBox1->Items->Add("ALL");
	}


	//�v�ӦC�X�ӥ[�iComboBox��
	for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
        ComboBox1->Items->Add(ServerSocket1->Socket->Connections[i]->RemoteAddress+"-"+
						  IntToStr(ServerSocket1->Socket->Connections[i]->SocketHandle));


   ComboBox1->ItemIndex=0;
   ComboBox1->Enabled = true;
}
//����������������������������������Timer 1
void __fastcall TForm6::Timer1Timer(TObject *Sender)
{
	if(step > 10)
	{
		if(blnServer)
		{
			ServerSocket1->Active = false;
			listen->Checked = false;
		}
		else
		{
			ClientSocket1->Active = false;

			listen->Checked = false;
			Connect1->Checked = false;
			Disconnect1->Checked = false;
		}
		fgConnectState = false;
		blnServer = NULL;

		ShowMessage("Time Out");
		Timer1 -> Enabled = false;
		Button1 -> Visible = false;
	}

	if(blnServer)
		ServerSocket1->Socket->Connections[0]->SendText("hand");
	//else
		//ClientSocket1->Socket->SendText("hand");
	step++;

	//ClientSocket1->Active=false;
}

void __fastcall TForm6::FormClose(TObject *Sender, TCloseAction &Action)//���������@��
{; /*
	PACKET pkg;
	String strPkg;
	String srtClient;

	if(fgConnectState)
	{
		 if(blnServer)
		 pkg.Host=IdIPWatch1->LocalIP();
	   else
		 pkg.Host=ClientHostName;

		pkg.ID="BINGO";
		pkg.MainFunc = "01";
		pkg.SubFunc = "2";
		pkg.lines = "0";
		pkg.Select_number = "00";
		pkg.error_code = "3";


	   strPkg=AssemblePacket(pkg);

		if (blnServer)
			ServerSocket1->Socket->Connections[0]->SendText(strPkg);
		else // Client
		{
			//ShowMessage("C:�e�X");
			ClientSocket1->Socket->SendText(strPkg) ;
		}

	}

	*/

}
//����������������������������������������������������������������������������������������������������������������������������������������������������������������������
void TForm6::Delay(int iMilliSeconds)
{
  int iStart;

  iStart = GetTickCount();
  while (GetTickCount() - iStart <= iMilliSeconds)
  {
    Sleep(1);
    Application->ProcessMessages();
   }
}

//---------------------------------------------------------------------------

