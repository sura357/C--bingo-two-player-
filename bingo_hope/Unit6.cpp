//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit6.h"
#include "Unit1.h"

#define  CLIENT_MAX 1 //定義連線人數
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「變數」
String strHost;
bool blnServer;  			//當成server or client


String ClientHostName = "";
String ServerName = "";
String newClient = "";

bool fgConnectState;  		//是否在連線狀態
int step = 0;     			//步驟
bool turn ;
int who = 0;



bool re = false;            //重新
int total;                  //連線數
bool win = false;           //勝負
String asktimes = 0;

TButton* butt[5][5];		//按鈕陣列
int bingo[5][5];			//數值陣列
bool b_stat[5][5];          //狀態陣列

TButton* but_en[5][5];		//按鈕陣列
String en[5][5];            //敵人陣列
bool ben_stat[5][5];          //狀態陣列
String en_Strpkg ="";
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「load」
__fastcall TForm6::TForm6(TComponent* Owner) //初始化
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
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Menu」
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「listen」
void __fastcall TForm6::listenClick(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	Form6->Caption="I am a server: "+IdIPWatch1->LocalIP();

	listen->Checked = !listen->Checked;

	ComboBox1->Clear();

	if (listen->Checked)//等於Server
	{
		listen->Enabled = false;
		ClientSocket1->Active = false;//關閉 Client

		try
		{
			ServerSocket1->Active = true;//開啟 Server
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
	else//關掉server
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

	Form6->Tag=0; //表示為server
    Memo2->Enabled = true;
}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「connect」
void __fastcall TForm6::Connect1Click(TObject *Sender)
{
	Form6->Caption="I am a client";

	if (ClientSocket1->Active)
		ClientSocket1->Active = false;

	strHost="127.0.0.1";
	if (InputQuery("Chat Connection", "Chat Server IP: ", strHost))//輸入視窗
	{
		if (strHost.Length() > 0)
		{
			ClientSocket1->Host = strHost;
			//ShowMessage("C:連接中");//------------------------------------------------------------------------------
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
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「disconnect」
void __fastcall TForm6::Disconnect1Click(TObject *Sender)
{
	PACKET pkg;
	String strPkg;

	if(blnServer) //此功能不是給server用的
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
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Server&Client」
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Server」
void __fastcall TForm6::ServerSocket1ClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{;
	PACKET pkg;
	String strPkg;
	int clientNum;
	//ShowMessage("S:Client連線成功");//------------------------------------------------------------------------------
	clientNum = ServerSocket1->Socket->ActiveConnections;

	if(clientNum > CLIENT_MAX)
	{
	  Form2->Memo1->Lines->Add("已達連線最大數量");
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

		//ShowMessage("S:回傳對方位址");//------------------------------------------------------------------------------
		ServerSocket1->Socket->Connections[0]->SendText(strPkg);
	}
    Sleep(100);
	restart();
	btn_Visible(true);
	Timer1->Enabled = true;
    Button1 -> Visible = true;

	step = 0;
	turn = true;

	Label1->Caption = "我方回合";
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
	//ShowMessage("S收到:"+strPacket);

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
    ShowMessage("Some 錯誤");


	}

	

}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Client」
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
	Label1->Caption = "敵方回合";
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
		//ShowMessage("C:讀取");
		pPkg=DisassemblePacket(strPacket);
		if(pPkg==NULL)
		{
			ShowMessage("就像我的腦袋，空空如也");
			Memo1->Lines->Add("Packet error");
			return;
		}

		ParsePacket(pPkg);
		delete pPkg;

	}
    step = 0;
	}catch(...)
	{
	ShowMessage("Some 錯誤");


	}



}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Packet」
//■■■■■■■■■■■■■■■■■組封包
String TForm6::AssemblePacket(PACKET pkg)
{
	String str="",tmp;
   //檔頭
	if(pkg.ID != "BINGO")//如不是賓果，則是「特殊指令」直接回傳
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
//■■■■■■■■■■■■■■■■■拆封包
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
   //宣告新的記憶體
   pPkg=new PACKET;//指標型態的要用「->」不是「=」

   pPkg->ID = tmpPkg.ID;
   pPkg->Host = tmpPkg.Host;

   pPkg->MainFunc = tmpPkg.MainFunc;
   pPkg->SubFunc = tmpPkg.SubFunc;

   pPkg->lines = tmpPkg.lines;
   pPkg->Select_number = tmpPkg.Select_number;

   pPkg->error_code = tmpPkg.error_code;

   return pPkg;//回傳，之後再自行判斷是否要刪掉記憶體
}
//■■■■■■■■■■■■■■■■■解析封包*******************************************************************
void TForm6::ParsePacket(pPACKET pPkg) //送出字串到Memo
{;
	int mFunc,sFunc;

	mFunc = StrToInt(pPkg->MainFunc);// 1:起始 ,2:點選
	sFunc = StrToInt(pPkg->SubFunc);

	switch(mFunc)
	{
		case 1:
			switch(sFunc)
			{
				case 1://1,1:收到這則封包的，確認成為Client
					ClientHostName = pPkg->Host;
					//ShowMessage("C:收到己方位址:"+ClientHostName);//------------------------------------------------------------------------------
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

				case 2://1,2:斷線封包
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
                    Label1->Caption = "我方回合";
					Label1->Font->Color = clBlue;

					if(total>=3)
						check_who_win(pPkg->lines);

				break;

				case 1:
					anm_check(StrToInt(pPkg->Select_number));
					check_who_win(pPkg->lines);
				break;

				case 2:
					ShowMessage("Game！");
					Label1->Caption = "Game！";
				break;

				case 3:
					ShowMessage("Wasted");
					Label1->Caption = "Wasted";
				break;
				case 4:
					ShowMessage("平手的啦");
					Label1->Caption = "平手的啦";
				break;
			}
			//ShowMessage("成功收到對方訊息");

			break;

		case 3://3,0:對方強制斷線
			if(!blnServer)//不是server用的
			{
				ServerSocket1->Active = false;

			}

			Disconnect1Click(ClientSocket1);
			ShowMessage("Discord");

		break;
	}

}
//■■■■■■■■■■■■■■■■■顯示封包
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
void TForm6::ShowPacket(pPACKET pPkg)//p=point(指標
{
	Form6 -> Memo1 ->Lines->Add("=================");
	Form6 -> Memo1 ->Lines->Add("ID= "+ pPkg->ID);
	Form6 -> Memo1 ->Lines->Add("Host= "+ pPkg->Host);
	Form6 -> Memo1 ->Lines->Add("MainFunc= "+ pPkg->MainFunc);
	Form6 -> Memo1 ->Lines->Add("SubFunc= "+ pPkg->SubFunc);
	Form6 -> Memo1 ->Lines->Add("Msg length= " +pPkg->lines);
	Form6 -> Memo1 ->Lines->Add("Msg= "+ pPkg->Select_number);
}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「Button_click」
//■■■■■■■■■■■■■■■■■按鈕事件
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
//■■■■■■■■■■■■■■■■■重新按鈕
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
//■■■■■■■■■■■■■■■■■檢查連線數
void TForm6::check_line()
{
	int X = 0, Y = 0, left_top = 0, left_bot = 0,count_left_top = 0, count_left_bot = 0;

	for (int i = 0; i < 5; i++)
	{
		int count_X = 0, count_Y = 0;
		for (int j = 0; j < 5; j++)
		{
			if (b_stat[i][j])//列
				count_X++;
			if (b_stat[j][i])//欄
				count_Y++;
			if (i == j && b_stat[j][i])//「\」
				count_left_top++;
			if (i + j == 5 - 1 && b_stat[j][i])//「/」
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
	//斜的"/"&"\"
	total = X + Y + left_top + left_bot;
	Label2->Caption = "共："+IntToStr(total)+"條";
	if(total>=3)
	{
        win = true;
	}

}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「void」
//■■■■■■■■■■■■■■■■■亂數
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
		V[i] = rand() % (25 - 1 + 1) + 1;   //亂數產生

		for (int j = 0; j < i; j++)
		{
			while (V[i] == V[j])    //檢查是否與前面產生的數值發生重複，如果有就重新產生
			{
				j = 0;  //如有重複，將變數j設為0，再次檢查 (因為還是有重複的可能)
				V[i] = rand() % (25 - 1 + 1) + 1;   //重新產生
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
//■■■■■■■■■■■■■■■■■隱藏顯示按鈕
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
//■■■■■■■■■■■■■■■■■推送封包
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

	pkg.MainFunc=		   	"2";//傳送點選封包是「02」

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

	Label1->Caption = "敵方回合";
	Label1->Font->Color = clRed;
}
//■■■■■■■■■■■■■■■■■確認敵人封包
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
//■■■■■■■■■■■■■■■■■重新下一局
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
        Label1->Caption = "我方回合";
		Label1->Font->Color = clBlue;
	}
	else
	{
		turn = false;
		Label1->Caption = "敵方回合";
		Label1->Font->Color = clRed;
	}
	//Bo_en();
}
//■■■■■■■■■■■■■■■■■誰贏
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

	pkg.MainFunc=		   	"2";//傳送點選封包是「2」

	ShowMessage("對方："+IntToStr(anm_line)+",我方："+IntToStr(total));
	if(anm_line==total)//平手
	{
		pkg.SubFunc=		"4";ShowMessage("平手的啦");
		Label1->Caption = "平手的啦";
	}
	else if(anm_line > total)//對方勝利
	{
		pkg.SubFunc=		"2";ShowMessage("Wasted");
		Label1->Caption = "Wasted";
	}
	else if(anm_line < total)//我方勝利
	{
		pkg.SubFunc=		"3";ShowMessage("Game！");
		Label1->Caption = "Game！";
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
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「敵人」
//■■■■■■■■■■■■■■■■■包版面
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

//■■■■■■■■■■■■■■■■■收版面
void TForm6::So_en(String strPkg)
{
	//ShowMessage("拆50");
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


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■「else」
//■■■■■■■■■■■■■■■■■檢查新連線
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
//■■■■■■■■■■■■■■■■■檢查連線數
void __fastcall TForm6::CheckClick(TObject *Sender)
{
	int number;
	TServerWinSocket *sSocket;//指標，臨時要一塊記憶體，不知道在哪裡、要怎麼找，所以要用指標指向那個區塊。
						//上下兩行只是分開來(因為太長)，Socket是TServerWinSocket型別
	sSocket=ServerSocket1->Socket;

   number= sSocket->ActiveConnections;

   String time =  Time();

   Form2->Memo1->Lines->Add("[" + time + "] Current conncetion number: "+  //目前有幾個連線
						   IntToStr(number));

   Form2->Memo1->Lines->Add(sSocket->Connections[0]->SocketHandle);
}
//■■■■■■■■■■■■■■■■■刷新列表
void TForm6::ReflashClientList()
{;
	ComboBox1->Clear();

	if(ServerSocket1->Socket->ActiveConnections>1)
	{
		ComboBox1->Items->Add("ALL");
	}


	//逐個列出來加進ComboBox裡
	for(int i=0;i<ServerSocket1->Socket->ActiveConnections;i++)
        ComboBox1->Items->Add(ServerSocket1->Socket->Connections[i]->RemoteAddress+"-"+
						  IntToStr(ServerSocket1->Socket->Connections[i]->SocketHandle));


   ComboBox1->ItemIndex=0;
   ComboBox1->Enabled = true;
}
//■■■■■■■■■■■■■■■■■Timer 1
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

void __fastcall TForm6::FormClose(TObject *Sender, TCloseAction &Action)//跟關閉的一樣
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
			//ShowMessage("C:送出");
			ClientSocket1->Socket->SendText(strPkg) ;
		}

	}

	*/

}
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
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

